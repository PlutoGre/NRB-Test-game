package com.oldmountain.dzm.admin;

import java.util.HashSet;
import java.util.Set;
import java.util.Map;
import java.util.HashMap;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.DisposeSysMsgInfo.OPDisposeSysMsgRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.services.*;
import com.oldmountain.dzm.util.*;

/** @brief 领取管理工具传书，补齐完整卡牌、残章及多件装备和奥义的实际发放。 */
public final class AdminRewards {
    private static final Logger LOG = LoggerFactory.getLogger(AdminRewards.class);

    /**
     * 在游戏主循环中领取工具创建的传书，并同步保存本次修改的资产和领奖状态。
     * @param player 实际登录角色
     * @param reward 归属该角色的未领取传书
     * @param reply 原客户端已经支持的领奖响应
     */
    public static void claim(Player player, Reward reward, OPDisposeSysMsgRet.Builder reply) {
        LOG.debug("claim: playerId={}, rewardId={}", new Object[]{player.getId(), reward.getId()});
        if (reward.getPlayerId() != player.getId() || reward.getStatus() != 0) throw new IllegalArgumentException("传书归属或状态不正确");
        JSONArray items = reward.getRewardContentJson();
        validate(player, items);
        // 原业务创建实例会自行提交。先持久化“处理中”，进程中断后禁止盲目重领。
        reward.setStatus(2);
        DZM.getEntityManager().updateSync(reward);
        Set<Object> changed = new HashSet<Object>();
        changed.add(player);
        boolean refreshBattleArray = false;
        for (int i = 0; i < items.size(); i++) {
            JSONObject item = items.getJSONObject(i);
            int type = item.getInt("t"), id = item.getInt("i"), quantity = item.getInt("q");
            switch (type) {
            case 11001:
                if (id == UserToolConst.USER_PROPERTY_EXP || id == UserToolConst.USER_LEVEL_PROPERTY_EXP) {
                    // 仅增加经验会造成客户端等级与服务端上阵/玩法门槛不一致，必须执行正常升级流程。
                    int previousLevel = player.getLevel();
                    Util.getUserLevelByExp(player, quantity);
                    changed.addAll(player.getBattlearray().values());
                    if (player.getChallenge() != null) changed.add(player.getChallenge());
                    refreshBattleArray = true;
                    LOG.info("claim: experience applied, playerId={}, previousLevel={}, level={}",
                            new Object[]{player.getId(), previousLevel, player.getLevel()});
                } else {
                    UserPropertyServices.changeProperty(player, id, quantity);
                }
                reply.setGoldcoins(player.getGoldCoins());
                reply.setSilvercoins(player.getSilvercoins());
                reply.setExp(player.getExp());
                break;
            case 61001:
                reply.addToolInfo(BagToolServices.addBagTool(player, id, quantity));
                changed.add(player.getBags().get(id));
                break;
            case 32000:
                reply.addSoulInfo(SoulServices.addSoul(player, id, quantity));
                changed.add(player.getSouls().get(id));
                break;
            case 31000:
                for (int n = 0; n < quantity; n++) {
                    // 游戏按模板记录持有状态，重复卡沿用礼包规则转为对应品质影子。
                    if (player.getDiscipleitems().contains(id)) {
                        int souls = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(XMLTemplateService.getDiscipleTemplate(id).getQuality());
                        reply.addSoulInfo(SoulServices.addSoul(player, id, souls));
                        changed.add(player.getSouls().get(id));
                    } else {
                        Disciple disciple = DiscipleUtil.newDisciple(player, id, 1, 0);
                        reply.addDiscipleInfo(DiscipleUtil.buildDiscipleBuilder(disciple));
                        reply.addSkillInfo(SkillUtil.buildSkillBuilder(player.getSkills().get(disciple.getSkill1())));
                    }
                }
                break;
            case 51000:
                for (int n = 0; n < quantity; n++) reply.addEquipInfo(EquipServices.createEquitAlone(player, id));
                break;
            case 41000:
                for (int n = 0; n < quantity; n++) reply.addSkillInfo(SkillServices.createSkillAlone(player, id));
                break;
            case 42000:
                for (int n = 0; n < quantity; n++) SkillBookServices.obtainSkillBook(player, id);
                changed.add(player.getSkillBooks().get(XMLTemplateService.getSkillIdBySkillBookId(id)));
                break;
            default:
                throw new IllegalArgumentException("不支持的管理附件");
            }
        }
        if (player.getHandbook() != null) changed.add(player.getHandbook());
        reward.setStatus(1);
        reward.setRewardTime(Util.getServerTime());
        player.updateOneReward(reward);
        changed.add(reward);
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            for (Object entity : changed) db.saveOrUpdate(entity);
            tx.commit();
        } catch (RuntimeException e) {
            tx.rollback();
            // 原业务创建实例会独立提交，失败时禁止本进程再次领取，留待人工核对。
            LOG.error("claim: persistence failed; inspect assets before retry, playerId=" + player.getId() + ", rewardId=" + reward.getId(), e);
            throw e;
        }
        if (refreshBattleArray) syncBattleArray(player);
        LOG.info("claim: reward saved, playerId={}, rewardId={}", new Object[]{player.getId(), reward.getId()});
    }

    /** 一次经验附件可能连升多级，原升级通知只携带最后一个新阵位，需要补发完整阵容。 */
    private static void syncBattleArray(Player player) {
        OPUserBattleArrayRet.Builder reply = OPUserBattleArrayRet.newBuilder();
        String[] ids = player.getBattleids().split(",");
        int limit = BattleUtil.getBattleCountByLevel(player.getLevel());
        for (int position = 0; position < limit; position++) {
            Battle battle = player.getBattlearray().get(Integer.parseInt(ids[position]));
            OPUserBattleArrayRet.BattleArrayInfo.Builder slot = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
            BattleUtil.buildBattleBuilder(slot, battle, position);
            reply.addBattle(slot);
        }
        reply.setServertime(Util.getServerTime());
        player.sendPacket(OP.code.OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S_VALUE, reply);
    }

    private static void validate(Player player, JSONArray items) {
        if (items.size() == 0 || items.size() > 20) throw new IllegalArgumentException("附件数量不正确");
        Map<Integer, Long> soulTotals = new HashMap<Integer, Long>();
        for (int i = 0; i < items.size(); i++) {
            JSONObject item = items.getJSONObject(i);
            int type = item.getInt("t"), id = item.getInt("i"), q = item.getInt("q");
            String kind;
            switch (type) {
            case 11001:
                kind = "property";
                if (id == 1001) withinInt((long)player.getGoldCoins() + q);
                if (id == 1003) withinInt((long)player.getExp() + q);
                if (id == 1002 && player.getSilvercoins() > Long.MAX_VALUE - q) throw new IllegalArgumentException("贝里数量超出上限");
                break;
            case 61001:
                kind = "tool";
                if (player.getBags().containsKey(id)) withinInt((long)player.getBags().get(id).getCount() + q);
                break;
            case 32000:
                kind = "soul";
                addSoulTotal(player, soulTotals, id, q);
                break;
            case 31000:
                kind = "card";
                if (XMLTemplateService.getDiscipleTemplate(id) != null) {
                    Integer souls = XMLTemplateService.getSysBasicConfig().getSoulCounts().get(XMLTemplateService.getDiscipleTemplate(id).getQuality());
                    if (souls == null || souls <= 0) throw new IllegalArgumentException("该卡牌缺少重复转换配置");
                    addSoulTotal(player, soulTotals, id, (long)souls * q);
                }
                break;
            case 51000: kind = "equip"; break;
            case 41000: kind = "skill"; break;
            case 42000:
                kind = "piece";
                if (XMLTemplateService.getSkillBookConfigById(id) != null) {
                    SkillBook book = player.getSkillBooks().get(XMLTemplateService.getSkillIdBySkillBookId(id));
                    if (book != null) withinInt((long)book.getSkillBookCountBySkillBookId(id) + q);
                }
                break;
            default: throw new IllegalArgumentException("未知附件类型");
            }
            // 领取时再次查内存模板，防止发出后删除模板导致部分附件已发、部分异常。
            AdminServerCatalogHolder.get().attachment(kind + ":" + id, q);
        }
    }

    private static void withinInt(long amount) {
        if (amount < 0 || amount > Integer.MAX_VALUE) throw new IllegalArgumentException("资产数量超过旧客户端整数上限");
    }

    private static void addSoulTotal(Player player, Map<Integer, Long> totals, int id, long count) {
        // 同一封信同时包含完整卡和影子时，合并检查，防止分别合法但总量溢出。
        long before = totals.containsKey(id) ? totals.get(id)
                : player.getSouls().containsKey(id) ? player.getSouls().get(id).getCount() : 0;
        withinInt(before + count);
        totals.put(id, before + count);
    }

    /** @brief 共享启动时加载的目录，避免每次领取重复解析数千条 XML。 */
    static final class AdminServerCatalogHolder {
        private static AdminCatalog catalog;
        static void set(AdminCatalog value) { catalog = value; }
        static AdminCatalog get() {
            if (catalog == null) throw new IllegalStateException("Administration is not initialized");
            return catalog;
        }
    }
}
