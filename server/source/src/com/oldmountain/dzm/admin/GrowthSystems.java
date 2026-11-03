package com.oldmountain.dzm.admin;

import java.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import static com.oldmountain.dzm.admin.ExpansionSystems.*;

/**
 * @brief 装备、奥义、船员、潜能和船舱的扩展规则及战斗加成。
 * 扩展等级与原实例 ID 绑定，原强化、穿戴和卡牌模板保持有效。
 */
public final class GrowthSystems {
    private static final Logger LOG=LoggerFactory.getLogger(GrowthSystems.class);
    private static final Map<String,String> NAMES=new HashMap<String,String>();
    private GrowthSystems() { }
    static String name(String kind,int id){
        String key=kind+":"+id;
        if(NAMES.isEmpty()) { JSONArray items=catalog.list();for(int i=0;i<items.size();i++){JSONObject item=items.getJSONObject(i);NAMES.put(item.getString("key"),item.getString("name"));} }
        return NAMES.containsKey(key)?NAMES.get(key):kind+" #"+id;
    }
    static JSONObject record(JSONObject state,String mode,int id){return object(object(object(state,mode),"items"),String.valueOf(id));}
    static JSONObject readRecord(JSONObject state,String mode,int id){
        if(state==null||!state.containsKey(mode))return new JSONObject();JSONObject root=state.getJSONObject(mode);
        if(!root.containsKey("items")||!root.getJSONObject("items").containsKey(String.valueOf(id)))return new JSONObject();return root.getJSONObject("items").getJSONObject(String.valueOf(id));
    }
    static Equip equip(Player player,int id){Equip item=player.getEquips().get(id);if(item==null||item.getExpiretime()!=0)throw new IllegalArgumentException("装备不存在");return item;}
    static boolean equipped(Player player,Equip item){
        if(item.getBattleid()!=0)return true;int id=item.getId();for(Battle b:player.getBattlearray().values())if(b.getEquip1()==id||b.getEquip2()==id||b.getEquip3()==id)return true;return false;
    }
    static void limited(int level,int cap){if(level>=cap)throw new IllegalArgumentException("已达到当前上限");}

    static void execute(String mode,String action,JSONObject request,ExpansionSystems.Context ctx){
        if("equipment".equals(mode)) equipment(action,request,ctx);
        else if("skills".equals(mode)) SkillGrowth.execute(action,request,ctx);
        else if("crew".equals(mode)||"training".equals(mode)) CrewGrowth.execute(mode,action,request,ctx);
        else if("cabin".equals(mode)) CabinGrowth.execute(action,request,ctx);
        else throw new IllegalArgumentException("未知养成操作");
    }
    static void view(String mode,JSONObject request,ExpansionSystems.Context ctx,JSONArray lines,JSONArray actions){
        if("equipment".equals(mode)) equipmentView(request,ctx,lines,actions);
        else if("skills".equals(mode)) SkillGrowth.view(request,ctx,lines,actions);
        else if("crew".equals(mode)||"training".equals(mode)) CrewGrowth.view(mode,request,ctx,lines,actions);
        else if("cabin".equals(mode)) CabinGrowth.view(request,ctx,lines,actions);
    }
    private static void equipment(String action,JSONObject request,final ExpansionSystems.Context ctx){
        final int id=request.optInt("id");JSONObject rule=rules("equipment"),state=object(ctx.state,"equipment");
        if("synthesize".equals(action)){
            JSONArray recipes=rule.getJSONArray("recipes");boolean allowed=false;
            for(int i=0;i<recipes.size();i++)if(recipes.getInt(i)==id)allowed=true;
            if(!allowed)throw new IllegalArgumentException("此装备没有工坊配方");
            int cost=rule.getInt("craftShards");if(state.optInt("shards")<cost)throw new IllegalArgumentException("工坊碎片不足，请先分解闲置装备");
            ctx.silver(rule.getInt("craftSilver"));state.put("shards",state.getInt("shards")-cost);
            JSONArray attachment=new JSONArray();attachment.add(catalog.attachment("equip:"+id,1));ctx.mail("装备工坊合成",attachment);ctx.message("合成成功，装备已发送至传书");return;
        }
        final Equip item=equip(ctx.player,id);JSONObject rec=record(ctx.state,"equipment",id);
        if("temper".equals(action)){
            int level=rec.optInt("temper");limited(level,rule.getInt("maxTemper"));int cost=Math.multiplyExact(rule.getInt("temperSilver"),level+1);
            ctx.silver(cost);rec.put("temper",level+1);rec.put("spentSilver",Math.addExact(rec.optInt("spentSilver"),cost));ctx.message("淬炼成功，装备战斗属性已提升");
        }else if("promote".equals(action)){
            int level=rec.optInt("rank");limited(level,rule.getInt("maxRank"));int cost=Math.multiplyExact(rule.getInt("rankGold"),level+1);
            ctx.gold(cost);rec.put("rank",level+1);rec.put("spentGold",Math.addExact(rec.optInt("spentGold"),cost));ctx.message("进阶成功，装备战斗属性已提升");
        }else if("decompose".equals(action)){
            if(item.getBattleid()!=0)throw new IllegalArgumentException("请先卸下装备再分解");
            for(Battle battle:ctx.player.getBattlearray().values())if(battle.getEquip1()==id||battle.getEquip2()==id||battle.getEquip3()==id)throw new IllegalArgumentException("上阵装备不能分解");
            if(ctx.db.createSQLQuery("DELETE FROM equip WHERE id=:id AND playerid=:player AND battleid=0").setInteger("id",id).setInteger("player",ctx.player.getId()).executeUpdate()!=1)throw new IllegalArgumentException("装备状态已改变，请刷新");
            state.put("shards",Math.addExact(state.optInt("shards"),rule.getInt("decomposeShards")));
            JSONArray attachment=new JSONArray();int silver=rule.getInt("decomposeSilver")+rec.optInt("spentSilver")*80/100,gold=rec.optInt("spentGold")*80/100;
            attachment.add(catalog.attachment("property:1002",silver));if(gold>0)attachment.add(catalog.attachment("property:1001",gold));ctx.mail("装备分解返还",attachment);
            object(state,"items").remove(String.valueOf(id));
            ctx.updates.add(new Runnable(){public void run(){ctx.player.getEquips().remove(id);if(ctx.player.getNeedUpdate().containsKey("equip"))ctx.player.getNeedUpdate().get("equip").remove(id);if(ctx.player.getNeedAdd().containsKey("equip"))ctx.player.getNeedAdd().get("equip").remove(id);}});
            ctx.message("分解成功，获得工坊碎片；贝里及养成返还通过传书领取");
        }else throw new IllegalArgumentException("未知装备操作");
    }
    private static void equipmentView(JSONObject request,ExpansionSystems.Context ctx,JSONArray lines,JSONArray actions){
        JSONObject state=object(ctx.state,"equipment"),rule=rules("equipment");
        lines.add("工坊碎片 "+state.optInt("shards")+"；淬炼每级 +"+rule.getInt("temperFlat")+"，进阶每级 +"+rule.getInt("rankPercent")+"% 装备属性。");
        lines.add("淬炼及进阶均必定成功。分解仅限闲置装备，返还 80% 本工坊投入；合成奖励需到传书领取。");
        List<Integer> ids=new ArrayList<Integer>();for(Equip e:ctx.player.getEquips().values())if(e.getExpiretime()==0)ids.add(e.getId());Collections.sort(ids);
        int selected=request.optInt("id");if(!ids.contains(selected))selected=ids.isEmpty()?0:ids.get(0);
        if(selected>0){Equip item=equip(ctx.player,selected);JSONObject rec=record(ctx.state,"equipment",selected);
            lines.add("当前："+name("equip",item.getItemid())+" #"+selected+"　原强化 "+item.getLevel()+"　淬炼 "+rec.optInt("temper")+"　进阶 "+rec.optInt("rank"));
            action(actions,"淬炼（"+rule.getInt("temperSilver")*(rec.optInt("temper")+1)+" 贝里）","temper",selected).put("enabled",rec.optInt("temper")<rule.getInt("maxTemper"));
            action(actions,"装备进阶（"+rule.getInt("rankGold")*(rec.optInt("rank")+1)+" 钻石）","promote",selected).put("enabled",rec.optInt("rank")<rule.getInt("maxRank"));
            action(actions,"分解所选装备（不可撤销）","decompose",selected).put("enabled",!equipped(ctx.player,item));
        }
        int offset=Math.max(0,Math.min(request.optInt("offset"),Math.max(0,ids.size()-1)));
        for(int i=offset;i<Math.min(offset+4,ids.size());i++){Equip item=equip(ctx.player,ids.get(i));action(actions,"选择："+name("equip",item.getItemid())+" #"+item.getId()+(equipped(ctx.player,item)?"（已穿戴）":"（闲置）"),"select",item.getId());}
        if(offset>0)action(actions,"上一批装备","page",Math.max(0,offset-4));if(offset+4<ids.size())action(actions,"下一批装备","page",offset+4);
        JSONArray recipes=rule.getJSONArray("recipes");for(int i=0;i<recipes.size();i++)action(actions,"合成"+name("equip",recipes.getInt(i))+"（"+rule.getInt("craftShards")+" 碎片 + "+rule.getInt("craftSilver")+" 贝里）","synthesize",recipes.getInt(i));
    }

    /** 把扩展装备成长计入真实战斗快照。 @param battle 当前阵位 @param disciple 当前船员 @param player 角色 */
    public static void applyStats(Battle battle,Disciple disciple,Player player){
        JSONObject state=ExpansionSystems.cached(player.getId());if(state==null)return;
        JSONObject rule=config.getJSONObject("equipment");
        int[] ids={battle.getEquip1(),battle.getEquip2(),battle.getEquip3()};
        for(int id:ids){Equip item=player.getEquips().get(id);if(item==null||!rule.optBoolean("enabled"))continue;
            JSONObject rec=readRecord(state,"equipment",id);double ratio=rec.optInt("rank")*rule.getInt("rankPercent")/100.0;int flat=rec.optInt("temper")*rule.getInt("temperFlat");
            if(item.getType()==1)battle.setAttack(Math.addExact(battle.getAttack(),flat+(int)Math.round(item.getAttack()*ratio)));
            if(item.getType()==2)battle.setDefence(Math.addExact(battle.getDefence(),flat+(int)Math.round(item.getDefence()*ratio)));
            if(item.getType()==3)battle.setHealth(Math.addExact(battle.getHealth(),flat+(int)Math.round(item.getHealth()*ratio)));
        }
        CrewGrowth.apply(battle,disciple,player,state);
        int[] cabin=CabinGrowth.bonus(player,state);
        battle.setHealth(Math.addExact(battle.getHealth(),cabin[0]));battle.setAttack(Math.addExact(battle.getAttack(),cabin[1]));
        battle.setDefence(Math.addExact(battle.getDefence(),cabin[2]));battle.setRp(Math.addExact(battle.getRp(),cabin[3]));
    }
    /** 同步奥义战斗参数，并提供有效后备船员的人物助缘列表。 @param player 当前角色 @param battle 阵位 @param members 原上阵模板列表 @return 独立的助缘模板列表 */
    public static ArrayList<Integer> prepareBattle(Player player,Battle battle,ArrayList<Integer> members){
        if(config==null)return members;
        SkillGrowth.sync(battle,player);ArrayList<Integer> result=new ArrayList<Integer>(members);
        for(Disciple d:CabinGrowth.members(player,ExpansionSystems.cached(player.getId())))if(!result.contains(d.getItemid()))result.add(d.getItemid());return result;
    }
    /** 登录时从基础实例重算，防止保存的快照重复叠加加成。 @param player 已加载的角色 */
    public static void restore(Player player){
        ArrayList<Integer> members=new ArrayList<Integer>();for(Battle b:player.getBattlearray().values())if(b.getDiscipleid()>0&&player.getDisciples().containsKey(b.getDiscipleid()))members.add(player.getDisciples().get(b.getDiscipleid()).getItemid());
        for(Battle battle:player.getBattlearray().values()){
            Disciple disciple=player.getDisciples().get(battle.getDiscipleid());if(disciple==null)continue;
            BattleUtil.resetBattleProperty(battle,disciple,player,members,1);DZM.getEntityManager().updateSync(battle);
        }
    }
    /** 重算并推送阵容，令新成长立即影响后续战斗。 @param player 当前在线角色 */
    public static void refresh(Player player){
        restore(player);
        OPUserBattleArrayRet.Builder reply=OPUserBattleArrayRet.newBuilder();String[] ids=player.getBattleids().split(",");
        for(int i=0;i<Math.min(ids.length,BattleUtil.getBattleCountByLevel(player.getLevel()));i++){
            Battle battle=player.getBattlearray().get(Integer.parseInt(ids[i]));if(battle==null)continue;
            OPUserBattleArrayRet.BattleArrayInfo.Builder slot=OPUserBattleArrayRet.BattleArrayInfo.newBuilder();BattleUtil.buildBattleBuilder(slot,battle,i);reply.addBattle(slot);
        }
        reply.setServertime(Util.getServerTime());player.sendPacket(OP.code.OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S_VALUE,reply);
        // 复用已有完整列表响应，使主阵容和背包页不会继续显示已分解或已替换的旧实例。
        com.oldmountain.dzm.net.packet.Packet query=new com.oldmountain.dzm.net.packet.Packet(org.apache.mina.common.ByteBuffer.wrap(new byte[]{8,1}),0,player.getSession());
        com.oldmountain.dzm.packethandler.UserEquipInfoHandler.getUserEquipInfo(query,player.getSession());
        com.oldmountain.dzm.packethandler.UserSkillInfoHandler.getUserSkillInfo(query,player.getSession());
        com.oldmountain.dzm.packethandler.UserDiscipleInfoHandler.getUserDiscipleInfo(query,player.getSession());
        com.oldmountain.dzm.packethandler.UserToolInfoHandler.getUserToolInfo(query,player.getSession());
        com.oldmountain.dzm.packethandler.UserBasicInfoHandler.getUserBasicInfo(query,player.getSession());
        LOG.debug("refresh: expansion battle stats synchronized, playerId={}",player.getId());
    }
}
