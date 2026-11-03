package com.oldmountain.dzm.admin;

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;
import java.util.Collections;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.hibernate.Hibernate;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.message.AdventureInfo.OPAdventureInfoRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.util.Util;

/**
 * @brief 永久开放的 All Blue 和幸运转盘，负责随机奖励、积分、兑换及幂等事务。
 * 业务在游戏主循环执行；扣费、活动状态、请求回执和奖励传书在同一事务提交。
 */
public final class PermanentActivities {
    /** 两端共用的有界 JSON 包；复用原 TCP/WebSocket 封包，不依赖新版 Lua 原生接口。 */
    public static final int REQUEST = 48001, RESPONSE = 48002;
    private static final Logger LOG = LoggerFactory.getLogger(PermanentActivities.class);
    private static final SecureRandom RANDOM = new SecureRandom();
    private static JSONObject config;
    private static JSONArray rewards;
    private static AdminCatalog catalog;
    private static String configRevision;
    private static int totalWeight;
    private static TimeZone timezone;
    private static volatile boolean initialized;
    /** 提交结果不明确时，阻止旧缓存被定时保存，待数据库恢复后重新读取真实余额。 */
    private static final Set<Integer> uncertainBalances = Collections.newSetFromMap(new ConcurrentHashMap<Integer, Boolean>());

    private PermanentActivities() { }

    /**
     * 在启动主循环中载入固定奖池并创建独立存档表，配置错误时停止启动。
     * @param items 当前已加载的物品目录
     * @throws Exception 配置、模板或数据库不可用
     */
    public static void initialize(AdminCatalog items) throws Exception {
        initialized = false; totalWeight = 0;
        catalog = items;
        byte[] bytes = Files.readAllBytes(Paths.get("xml/permanentActivities.json"));
        if (bytes.length > 131072) throw new IllegalArgumentException("永久活动配置过大");
        config = JSONObject.fromObject(new String(bytes, StandardCharsets.UTF_8));
        configRevision = hex(MessageDigest.getInstance("SHA-256").digest(bytes));
        timezone = TimeZone.getTimeZone(config.getString("timezone"));
        if (!timezone.getID().equals(config.getString("timezone"))) throw new IllegalArgumentException("永久活动时区无效");
        rewards = config.getJSONArray("rewards");
        if (rewards.size() != 8) throw new IllegalArgumentException("两项活动需要固定 8 项奖励");
        for (int i = 0; i < rewards.size(); i++) {
            JSONObject item = rewards.getJSONObject(i);
            attachment(item);
            int weight = bounded(item, "weight", 1, 1000000);
            totalWeight = add(totalWeight, weight);
        }
        for (String activity : new String[]{"allBlue", "fortuneWheel"}) {
            JSONObject rule = rules(activity);
            if (rule.getInt("adventureId") != ("allBlue".equals(activity) ? 9001 : 9002)) throw new IllegalArgumentException("活动入口编号不能改变");
            bounded(rule, "dailyFree", 0, 100);
            bounded(rule, "singleCost", 1, 1000000);
            bounded(rule, "pointsPerDraw", 1, 10000);
            JSONArray shop = rule.getJSONArray("shop");
            if (shop.size() < 1 || shop.size() > 8) throw new IllegalArgumentException("积分商店需有 1–8 项商品");
            java.util.Set<Integer> ids = new java.util.HashSet<Integer>();
            for (int i = 0; i < shop.size(); i++) {
                JSONObject offer = shop.getJSONObject(i);
                if (!ids.add(bounded(offer, "id", 1, 99))) throw new IllegalArgumentException("商店商品编号重复");
                bounded(offer, "reward", 0, 7); bounded(offer, "points", 1, 10000000);
            }
        }
        bounded(rules("allBlue"), "tenCost", 1, 10000000);
        JSONObject wheel = rules("fortuneWheel");
        bounded(wheel, "resetCost", 0, 1000000); attachment(wheel.getJSONObject("duplicateReward"));
        java.util.Set<Integer> ids = new java.util.HashSet<Integer>();
        JSONArray milestones = wheel.getJSONArray("milestones");
        if (milestones.size() > 8) throw new IllegalArgumentException("累计奖励最多 8 档");
        for (int i = 0; i < milestones.size(); i++) {
            JSONObject milestone = milestones.getJSONObject(i);
            if (!ids.add(bounded(milestone, "id", 1, 99))) throw new IllegalArgumentException("累计奖励编号重复");
            bounded(milestone, "draws", 1, 10000000); bounded(milestone, "reward", 0, 7);
        }
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        try {
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_permanent_activity (playerId INT NOT NULL, "
                    + "activity VARCHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL, state TEXT NOT NULL, "
                    + "revision INT NOT NULL, updatedAt INT NOT NULL, PRIMARY KEY(playerId,activity)) "
                    + "ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            db.createSQLQuery("CREATE TABLE IF NOT EXISTS nrb_permanent_activity_requests (playerId INT NOT NULL, "
                    + "activity VARCHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL, "
                    + "requestId CHAR(32) CHARACTER SET ascii COLLATE ascii_bin NOT NULL, "
                    + "reply MEDIUMTEXT NOT NULL, createdAt INT NOT NULL, PRIMARY KEY(playerId,activity,requestId)) "
                    + "ENGINE=InnoDB DEFAULT CHARSET=utf8").executeUpdate();
            tx.commit(); initialized = true;
        } catch (RuntimeException error) { tx.rollback(); throw error; }
        LOG.info("initialize: permanent activities ready, rewards={}, config={}", rewards.size(), configRevision.substring(0, 12));
    }

    /** 向活动列表追加无等级、无排期限制的两个入口。 @param reply 原活动列表 */
    public static void appendEntries(OPAdventureInfoRet.Builder reply) {
        if (!initialized) return;
        int index = reply.getAdventureItemCount();
        for (String activity : new String[]{"allBlue", "fortuneWheel"}) {
            reply.addAdventureItem(OPAdventureInfoRet.AdventureItem.newBuilder()
                    .setId(index++).setAdventureId(rules(activity).getInt("adventureId"))
                    .setLevelLimit(0).setBeginTime(0).setEndTime(Integer.MAX_VALUE).setVersion(1));
        }
    }

    /**
     * 处理已通过账号和角色校验的活动请求；传输断开后可用同一请求编号重试。
     * @param packet 游戏主循环中的网络包
     */
    public static void handle(Packet packet) {
        JSONObject request = null;
        JSONObject reply;
        Player player = null;
        try {
            if (!initialized) throw new IllegalStateException("活动尚未就绪，请稍后重试");
            if (packet.data.array().length > 4096) throw new IllegalArgumentException("活动请求过大");
            request = JSONObject.fromObject(new String(packet.data.array(), StandardCharsets.UTF_8));
            String activity = request.optString("activity"); rules(activity);
            int id = (Integer) packet.session.getAttribute("playerid");
            player = ObjectAccessor.getOnlineUser(id);
            if (player == null || player.getSession() != packet.session
                    || !player.getPuid().equals(packet.session.getAttribute("nrb.account.uid"))) {
                throw new IllegalArgumentException("登录连接已改变，请重新进入游戏");
            }
            // 与登录工作线程、定时保存协调，防止旧余额覆盖刚提交的活动消费。
            synchronized (AccountService.GAME_LOGIN_LOCK) {
                synchronized (player) {
                    if (player.getSession() != packet.session) throw new IllegalArgumentException("登录连接已改变，请重新进入游戏");
                    if (!reconcileBalance(player)) throw new IllegalStateException("活动资产等待数据库恢复");
                    reply = dispatch(player, request);
                }
            }
        } catch (IllegalArgumentException error) {
            reply = failure(request, error.getMessage());
        } catch (Exception error) {
            LOG.error("handle: permanent activity request failed, playerId=" + (player == null ? 0 : player.getId()), error);
            reply = failure(request, "请求结果尚未确认，请重试查询；已发送的操作会按原编号核对");
            reply.put("uncertain", true);
        }
        Packet response = new Packet(RESPONSE, reply.toString().getBytes(StandardCharsets.UTF_8));
        // MINA 可能分配更大的池化缓冲区；限定实际 JSON 长度，不能把尾部补零一并发送。
        response.data.flip();
        packet.session.write(response);
    }

    private static JSONObject dispatch(Player player, JSONObject request) {
        String activity = request.getString("activity"), action = request.optString("action", "info");
        JSONObject rule = rules(activity);
        String requestId = request.optString("requestId");
        boolean mutate = !"info".equals(action);
        if (mutate && !requestId.matches("[a-f0-9]{32}")) throw new IllegalArgumentException("无效操作编号");
        Session db = DZM.getEntityManager().getSessionFactory();
        Transaction tx = db.beginTransaction();
        int previousSys = player.getSysgoldcoins(), previousRecharge = player.getRechargegoldcoins();
        JSONObject reply;
        boolean committed = false;
        boolean commitAttempted = false;
        try {
            JSONObject initial = new JSONObject(); initial.put("points", 0); initial.put("draws", 0);
            initial.put("mask", 0); initial.put("freeDay", ""); initial.put("freeUsed", 0);
            initial.put("claimed", new JSONArray()); initial.put("lastRewards", new JSONArray());
            db.createSQLQuery("INSERT IGNORE INTO nrb_permanent_activity(playerId,activity,state,revision,updatedAt) "
                    + "VALUES(:id,:activity,:state,0,:now)").setInteger("id", player.getId()).setString("activity", activity)
                    .setString("state", initial.toString()).setInteger("now", Util.getServerTime()).executeUpdate();
            Object[] row = (Object[]) db.createSQLQuery("SELECT state,revision FROM nrb_permanent_activity "
                    + "WHERE playerId=:id AND activity=:activity FOR UPDATE")
                    .addScalar("state", Hibernate.STRING).addScalar("revision", Hibernate.INTEGER)
                    .setInteger("id", player.getId()).setString("activity", activity).uniqueResult();
            JSONObject state = JSONObject.fromObject((String) row[0]);
            int revision = ((Number) row[1]).intValue();
            JSONObject result = null;
            if (mutate) {
                String saved = (String) db.createSQLQuery("SELECT reply FROM nrb_permanent_activity_requests "
                        + "WHERE playerId=:id AND activity=:activity AND requestId=:request")
                        .addScalar("reply", Hibernate.STRING).setInteger("id", player.getId())
                        .setString("activity", activity).setString("request", requestId).uniqueResult();
                if (saved != null) {
                    result = JSONObject.fromObject(saved); result.put("replayed", true);
                } else {
                    // 先查幂等回执；重启后修改奖池也必须能核对上一版已经提交的请求。
                    if (!configRevision.equals(request.optString("configRevision"))) throw new IllegalArgumentException("活动规则已更新，请刷新后重试");
                    if (request.optInt("revision", -1) != revision) throw new IllegalArgumentException("活动进度已改变，请刷新后重试");
                    result = execute(player, activity, action, request, state, rule);
                    int cost = result.getInt("cost");
                    if (cost < 0 || player.getGoldCoins() < cost) throw new IllegalArgumentException("钻石不足");
                    if (cost > 0) player.consumeGold(cost);
                    int mails = saveMails(db, player, activity, requestId, result.getJSONArray("rewards"));
                    result.put("mailCount", mails);
                    revision = add(revision, 1);
                    db.createSQLQuery("UPDATE nrb_permanent_activity SET state=:state,revision=:revision,updatedAt=:now "
                            + "WHERE playerId=:id AND activity=:activity")
                            .setString("state", state.toString()).setInteger("revision", revision)
                            .setInteger("now", Util.getServerTime()).setInteger("id", player.getId()).setString("activity", activity).executeUpdate();
                    // 原玩家表仍保存资产；与活动账本、传书共同提交，重启后不会重复扣费或发奖。
                    db.saveOrUpdate(player);
                    db.createSQLQuery("INSERT INTO nrb_permanent_activity_requests(playerId,activity,requestId,reply,createdAt) "
                            + "VALUES(:id,:activity,:request,:reply,:now)")
                            .setInteger("id", player.getId()).setString("activity", activity).setString("request", requestId)
                            .setString("reply", result.toString()).setInteger("now", Util.getServerTime()).executeUpdate();
                    LOG.debug("dispatch: prepared activity={}, action={}, playerId={}, request={}, cost={}, mails={}",
                            new Object[]{activity, action, player.getId(), requestId, cost, mails});
                }
            }
            reply = snapshot(player, activity, state, revision);
            reply.put("ok", true); reply.put("requestId", requestId); reply.put("action", action);
            if (result != null) reply.put("result", result);
            commitAttempted = true; tx.commit(); committed = true;
        } catch (RuntimeException error) {
            try { if (!committed && tx.isActive()) tx.rollback(); }
            catch (RuntimeException rollbackError) { LOG.error("dispatch: rollback status unknown, playerId=" + player.getId(), rollbackError); }
            if (commitAttempted) uncertainBalances.add(player.getId());
            else { player.setSysgoldcoins(previousSys); player.setRechargegoldcoins(previousRecharge); }
            throw error;
        }
        // 传书已落库后再刷新内存和界面；推送失败不影响幂等回执及奖励归属。
        if (mutate) {
            LOG.info("dispatch: committed activity={}, action={}, playerId={}, request={}", new Object[]{activity, action, player.getId(), requestId});
            player.setNeedSyncRewards(true);
            try { player.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE,
                    RewardServices.getSysReward(player).setServertime(Util.getServerTime())); }
            catch (Exception error) { LOG.warn("dispatch: mailbox refresh deferred, playerId=" + player.getId(), error); }
        }
        return reply;
    }

    /**
     * 提交确认丢失后，从数据库恢复余额，禁止用不确定的内存值覆盖已提交资产。
     * @param player 当前角色；由网络分发、重新登录和定时保存共同调用
     * @return 是否可以继续操作或保存该角色
     */
    public static boolean reconcileBalance(Player player) {
        if (player == null || !uncertainBalances.contains(player.getId())) return true;
        synchronized (player) {
            Transaction tx = null;
            try {
                Session db = DZM.getEntityManager().getSessionFactory();
                tx = db.beginTransaction();
                Object[] row = (Object[]) db.createSQLQuery("SELECT sysgoldcoins,rechargegoldcoins FROM player WHERE id=:id")
                        .addScalar("sysgoldcoins", Hibernate.INTEGER).addScalar("rechargegoldcoins", Hibernate.INTEGER)
                        .setInteger("id", player.getId()).uniqueResult();
                if (row == null) throw new IllegalStateException("活动角色不存在");
                tx.commit();
                player.setSysgoldcoins(((Number) row[0]).intValue()); player.setRechargegoldcoins(((Number) row[1]).intValue());
                uncertainBalances.remove(player.getId());
                LOG.info("reconcileBalance: restored committed balance, playerId={}", player.getId()); return true;
            } catch (RuntimeException error) {
                try { if (tx != null && tx.isActive()) tx.rollback(); }
                catch (RuntimeException rollbackError) { LOG.error("reconcileBalance: rollback failed, playerId=" + player.getId(), rollbackError); }
                LOG.error("reconcileBalance: database not ready, playerId=" + player.getId(), error); return false;
            }
        }
    }

    private static JSONObject execute(Player player, String activity, String action, JSONObject request, JSONObject state, JSONObject rule) {
        JSONObject result = new JSONObject(); result.put("cost", 0); result.put("rewards", new JSONArray());
        JSONArray won = result.getJSONArray("rewards");
        if (("explore".equals(action) && "allBlue".equals(activity)) || ("spin".equals(action) && "fortuneWheel".equals(activity))) {
            int count = "allBlue".equals(activity) ? request.optInt("count", 1) : 1;
            if (count != 1 && count != 10) throw new IllegalArgumentException("探索次数只能为 1 或 10");
            int free = freeRemaining(state, rule);
            int cost = count == 10 ? rule.getInt("tenCost") : free > 0 ? 0 : rule.getInt("singleCost");
            if (player.getGoldCoins() < cost) throw new IllegalArgumentException("钻石不足");
            if ("fortuneWheel".equals(activity) && state.getInt("mask") == 255) throw new IllegalArgumentException("本轮奖励已集齐，请先重置转盘");
            if (count == 1 && free > 0) {
                int used = day().equals(state.optString("freeDay")) ? state.optInt("freeUsed") : 0;
                state.put("freeDay", day()); state.put("freeUsed", used + 1);
            }
            for (int i = 0; i < count; i++) {
                int slot = draw(); boolean duplicate = "fortuneWheel".equals(activity) && (state.getInt("mask") & 1 << slot) != 0;
                JSONObject prize = publicReward(duplicate ? rule.getJSONObject("duplicateReward") : rewards.getJSONObject(slot));
                prize.put("slot", slot); prize.put("duplicate", duplicate); won.add(prize);
                if ("fortuneWheel".equals(activity)) state.put("mask", state.getInt("mask") | 1 << slot);
            }
            state.put("draws", add(state.getInt("draws"), count));
            state.put("points", add(state.getInt("points"), rule.getInt("pointsPerDraw") * count));
            state.put("lastRewards", won); result.put("cost", cost);
        } else if ("exchange".equals(action)) {
            JSONObject offer = find(rule.getJSONArray("shop"), request.optInt("id", -1));
            int price = offer.getInt("points");
            if (state.getInt("points") < price) throw new IllegalArgumentException("积分不足");
            state.put("points", state.getInt("points") - price);
            won.add(publicReward(rewards.getJSONObject(offer.getInt("reward"))));
            state.put("lastRewards", won);
        } else if ("reset".equals(action) && "fortuneWheel".equals(activity)) {
            int mask = state.getInt("mask");
            if (mask == 0) throw new IllegalArgumentException("本轮还没有抽取，无需重置");
            int cost = mask == 255 ? 0 : rule.getInt("resetCost");
            if (player.getGoldCoins() < cost) throw new IllegalArgumentException("钻石不足");
            result.put("cost", cost); state.put("mask", 0);
        } else if ("claim".equals(action) && "fortuneWheel".equals(activity)) {
            JSONObject milestone = find(rule.getJSONArray("milestones"), request.optInt("id", -1));
            JSONArray claimed = state.getJSONArray("claimed");
            if (claimed.contains(milestone.getInt("id"))) throw new IllegalArgumentException("该累计奖励已经领取");
            if (state.getInt("draws") < milestone.getInt("draws")) throw new IllegalArgumentException("累计抽取次数不足");
            claimed.add(milestone.getInt("id")); state.put("claimed", claimed);
            won.add(publicReward(rewards.getJSONObject(milestone.getInt("reward")))); state.put("lastRewards", won);
        } else throw new IllegalArgumentException("未知活动操作");
        return result;
    }

    private static JSONObject snapshot(Player player, String activity, JSONObject state, int revision) {
        JSONObject rule = rules(activity), reply = new JSONObject();
        reply.put("activity", activity); reply.put("owner", player.getPuid()); reply.put("configRevision", configRevision);
        reply.put("name", rule.getString("name")); reply.put("revision", revision);
        reply.put("points", state.getInt("points")); reply.put("draws", state.getInt("draws"));
        reply.put("mask", state.getInt("mask")); reply.put("claimed", state.getJSONArray("claimed"));
        reply.put("lastRewards", state.getJSONArray("lastRewards")); reply.put("freeRemaining", freeRemaining(state, rule));
        reply.put("goldcoins", player.getGoldCoins()); reply.put("singleCost", rule.getInt("singleCost"));
        reply.put("pointsPerDraw", rule.getInt("pointsPerDraw")); reply.put("serverDay", day());
        reply.put("serverTime", Util.getServerTime());
        reply.put("tenCost", rule.optInt("tenCost", 0)); reply.put("resetCost", state.getInt("mask") == 255 ? 0 : rule.optInt("resetCost", 0));
        if (rule.containsKey("duplicateReward")) reply.put("duplicateReward", publicReward(rule.getJSONObject("duplicateReward")));
        JSONArray pool = new JSONArray();
        for (int i = 0; i < rewards.size(); i++) {
            JSONObject reward = publicReward(rewards.getJSONObject(i));
            reward.put("slot", i); reward.put("probability", rewards.getJSONObject(i).getInt("weight") * 100.0 / totalWeight); pool.add(reward);
        }
        reply.put("pool", pool);
        JSONArray offers = new JSONArray();
        for (int i = 0; i < rule.getJSONArray("shop").size(); i++) {
            JSONObject offer = rule.getJSONArray("shop").getJSONObject(i);
            JSONObject item = publicReward(rewards.getJSONObject(offer.getInt("reward")));
            item.put("id", offer.getInt("id")); item.put("points", offer.getInt("points")); offers.add(item);
        }
        reply.put("shop", offers);
        JSONArray milestones = new JSONArray();
        if (rule.containsKey("milestones")) for (int i = 0; i < rule.getJSONArray("milestones").size(); i++) {
            JSONObject milestone = rule.getJSONArray("milestones").getJSONObject(i);
            JSONObject item = publicReward(rewards.getJSONObject(milestone.getInt("reward")));
            item.put("id", milestone.getInt("id")); item.put("draws", milestone.getInt("draws"));
            item.put("claimed", state.getJSONArray("claimed").contains(milestone.getInt("id"))); milestones.add(item);
        }
        reply.put("milestones", milestones); return reply;
    }

    private static int saveMails(Session db, Player player, String activity, String requestId, JSONArray won) {
        Map<String, JSONObject> merged = new LinkedHashMap<String, JSONObject>();
        for (int i = 0; i < won.size(); i++) {
            JSONObject item = won.getJSONObject(i), asset = item.getJSONObject("attachment");
            String key = asset.getInt("t") + ":" + asset.getInt("i");
            if (!merged.containsKey(key)) merged.put(key, JSONObject.fromObject(asset.toString()));
            else merged.get(key).put("q", add(merged.get(key).getInt("q"), asset.getInt("q")));
        }
        List<JSONArray> groups = new ArrayList<JSONArray>(); JSONArray group = new JSONArray();
        for (JSONObject item : merged.values()) {
            JSONArray candidate = JSONArray.fromObject(group.toString()); candidate.add(item);
            if (candidate.toString().length() > 240 && !group.isEmpty()) { groups.add(group); group = new JSONArray(); }
            group.add(item);
        }
        if (!group.isEmpty()) groups.add(group);
        for (JSONArray attachments : groups) {
            Reward mail = new Reward(); mail.setPlayerId(player.getId()); mail.setRewardType(7);
            mail.setRewardMsg(rules(activity).getString("name") + "活动奖励");
            // 旧表 rewardInfo 只有 VARCHAR(50)，完整请求回执已保存在独立账本。
            JSONObject info = new JSONObject(); info.put("nrb", 1); info.put("e", "allBlue".equals(activity) ? "ab" : "fw"); info.put("r", requestId.substring(0, 16));
            mail.setRewardInfoFromJson(info); mail.setRewardContentFromJson(attachments);
            mail.setStatus(0); mail.setAddTime(Util.getServerTime()); mail.setRewardTime(Util.getServerTime()); db.save(mail);
        }
        return groups.size();
    }

    private static JSONObject publicReward(JSONObject source) {
        JSONObject result = new JSONObject(); result.put("name", source.getString("name"));
        result.put("count", source.getInt("count")); result.put("icon", source.getString("icon"));
        result.put("attachment", attachment(source)); return result;
    }

    private static JSONObject attachment(JSONObject source) { return catalog.attachment(source.getString("key"), bounded(source, "count", 1, 1000000)); }
    private static JSONObject rules(String activity) {
        if (!"allBlue".equals(activity) && !"fortuneWheel".equals(activity)) throw new IllegalArgumentException("未知活动");
        return config.getJSONObject(activity);
    }
    private static int bounded(JSONObject object, String key, int min, int max) {
        int value = object.getInt(key);
        if (value < min || value > max) throw new IllegalArgumentException("活动配置数值超出范围：" + key);
        return value;
    }
    private static int add(int before, int delta) {
        long value = (long) before + delta;
        if (value < 0 || value > Integer.MAX_VALUE) throw new IllegalArgumentException("活动数值达到上限");
        return (int) value;
    }
    private static JSONObject find(JSONArray items, int id) {
        for (int i = 0; i < items.size(); i++) if (items.getJSONObject(i).getInt("id") == id) return items.getJSONObject(i);
        throw new IllegalArgumentException("奖励或商品不存在");
    }
    private static int draw() {
        int cursor = RANDOM.nextInt(totalWeight);
        for (int i = 0; i < rewards.size(); i++) { cursor -= rewards.getJSONObject(i).getInt("weight"); if (cursor < 0) return i; }
        throw new IllegalStateException("奖池权重无效");
    }
    private static String day() { SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd"); format.setTimeZone(timezone); return format.format(new Date()); }
    private static int freeRemaining(JSONObject state, JSONObject rule) {
        return Math.max(0, rule.getInt("dailyFree") - (day().equals(state.optString("freeDay")) ? state.optInt("freeUsed") : 0));
    }
    private static JSONObject failure(JSONObject request, String message) {
        JSONObject result = new JSONObject(); result.put("ok", false); result.put("message", message);
        if (request != null) { result.put("activity", request.optString("activity")); result.put("requestId", request.optString("requestId")); result.put("action", request.optString("action")); }
        return result;
    }
    private static String hex(byte[] bytes) { StringBuilder out = new StringBuilder(); for (byte value : bytes) out.append(String.format("%02x", value & 255)); return out.toString(); }
}
