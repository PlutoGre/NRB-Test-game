package com.oldmountain.dzm.admin;

import java.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import com.oldmountain.dzm.entity.*;
import static com.oldmountain.dzm.admin.ExpansionSystems.*;
import static com.oldmountain.dzm.admin.GrowthSystems.*;

/** @brief 双船体、七舱位、后备船员助缘和全队属性加成。 */
final class CabinGrowth {
    private CabinGrowth() { }
    static boolean available(Player p,Disciple d){
        if(d==null||d.getExpiretime()!=0||d.getBattleid()!=0)return false;
        for(Battle b:p.getBattlearray().values())if(b.getDiscipleid()==d.getId())return false;return true;
    }
    static JSONObject seat(JSONObject state,int index){return object(object(object(state,"cabin"),"seats"),String.valueOf(index));}
    static boolean unlocked(JSONObject seat,int index){return seat.optBoolean("unlocked",index==1);}
    static void execute(String action,JSONObject request,ExpansionSystems.Context ctx){
        JSONObject rule=rules("cabin"),state=object(ctx.state,"cabin");int slot=request.optInt("choice",1);
        if(slot<1||slot>rule.getInt("slots"))throw new IllegalArgumentException("无效舱位");JSONObject seat=seat(ctx.state,slot);
        if("ship".equals(action)){
            int ship=request.optInt("id");if(ship<1||ship>rule.getJSONArray("ships").size())throw new IllegalArgumentException("船体不存在");
            JSONObject owned=object(state,"ownedShips");if(ship!=1&&!owned.optBoolean(String.valueOf(ship))){ctx.gold(rule.getInt("shipGold"));owned.put(String.valueOf(ship),true);}
            state.put("activeShip",ship);ctx.message("已切换船体，船舱安置和舱位成长保留");return;
        }
        if("hull".equals(action)){
            JSONObject levels=object(state,"hulls");String ship=String.valueOf(state.optInt("activeShip",1));int level=levels.optInt(ship);limited(level,rule.getInt("maxHull"));
            ctx.silver(Math.multiplyExact(rule.getInt("hullSilver"),level+1));levels.put(ship,level+1);ctx.message("船体强化成功，全队属性已提升");return;
        }
        if("unlock".equals(action)){
            if(unlocked(seat,slot))throw new IllegalArgumentException("此舱位已解锁");ctx.gold(Math.multiplyExact(rule.getInt("unlockGold"),slot));seat.put("unlocked",true);ctx.message("新舱位已解锁");return;
        }
        if(!unlocked(seat,slot))throw new IllegalArgumentException("请先解锁舱位");
        if("upgrade".equals(action)){
            int level=seat.optInt("level",1);limited(level,rule.getInt("maxSeat"));ctx.silver(Math.multiplyExact(rule.getInt("seatSilver"),level+1));seat.put("level",level+1);ctx.message("舱位升级成功");
        }else if("assign".equals(action)){
            int id=request.optInt("id");Disciple d=ctx.player.getDisciples().get(id);if(!available(ctx.player,d))throw new IllegalArgumentException("请安置未上阵的后备船员");
            for(int i=1;i<=rule.getInt("slots");i++)if(i!=slot&&seat(ctx.state,i).optInt("crew")==id)throw new IllegalArgumentException("该船员已在其他舱位");
            if(seat.optInt("crew")==id)throw new IllegalArgumentException("该船员已经在此舱位");seat.put("crew",id);ctx.message("船员安置成功，将提供全队属性与人物助缘");
        }else if("remove".equals(action)){
            if(seat.optInt("crew")==0)throw new IllegalArgumentException("此舱位为空");seat.put("crew",0);ctx.message("船员已离舱，仍保留在船员背包");
        }else throw new IllegalArgumentException("未知船舱操作");
    }
    static void view(JSONObject request,ExpansionSystems.Context ctx,JSONArray lines,JSONArray actions){
        JSONObject rule=rules("cabin"),state=object(ctx.state,"cabin");int slot=request.optInt("choice",1);if(slot<1||slot>rule.getInt("slots"))slot=1;
        int ship=state.optInt("activeShip",1);String shipName=rule.getJSONArray("ships").getString(ship-1);int hull=object(state,"hulls").optInt(String.valueOf(ship));
        JSONObject seat=seat(ctx.state,slot);int occupant=seat.optInt("crew");Disciple member=ctx.player.getDisciples().get(occupant);
        lines.add("当前船体："+shipName+"　船体强化 "+hull+" / "+rule.getInt("maxHull"));
        lines.add("后备船员入舱不消耗卡牌，参与人物缘分并向全队提供基础属性的 "+rule.getInt("seatPercent")+"% × 舱位等级；改为上阵、出售或被消耗时自动离舱。");
        lines.add("当前舱位 "+slot+" / "+rule.getInt("slots")+"　"+(unlocked(seat,slot)?"已解锁，等级 "+seat.optInt("level",1):"尚未解锁")+"　住客："+(available(ctx.player,member)?name("card",member.getItemid()):"空舱"));
        int[] bonus=bonus(ctx.player,ctx.state);lines.add("当前全队加成：生命 "+bonus[0]+"　攻击 "+bonus[1]+"　防御 "+bonus[2]+"　意志 "+bonus[3]);
        action(actions,"切换到 "+(slot%rule.getInt("slots")+1)+" 号舱位","choice",slot%rule.getInt("slots")+1);
        if(!unlocked(seat,slot))action(actions,"解锁此舱位（"+rule.getInt("unlockGold")*slot+" 钻石）","unlock",0).put("choice",slot);
        else{
            action(actions,"舱位升级（"+rule.getInt("seatSilver")*(seat.optInt("level",1)+1)+" 贝里）","upgrade",0).put("choice",slot);
            if(occupant!=0)action(actions,"移出舱内船员","remove",0).put("choice",slot);
        }
        List<Integer> ids=new ArrayList<Integer>();for(Disciple d:ctx.player.getDisciples().values())if(available(ctx.player,d))ids.add(d.getId());Collections.sort(ids);
        int selected=request.optInt("id");if(!ids.contains(selected))selected=ids.isEmpty()?0:ids.get(0);
        if(selected>0){lines.add("候选船员："+name("card",ctx.player.getDisciples().get(selected).getItemid())+" #"+selected);action(actions,"安置所选后备船员","assign",selected).put("choice",slot);}
        int offset=Math.max(0,Math.min(request.optInt("offset"),Math.max(0,ids.size()-1)));
        for(int i=offset;i<Math.min(offset+4,ids.size());i++){Disciple d=ctx.player.getDisciples().get(ids.get(i));action(actions,"选择："+name("card",d.getItemid())+" #"+d.getId(),"select",d.getId());}
        if(offset>0)action(actions,"上一批后备船员","page",Math.max(0,offset-4));if(offset+4<ids.size())action(actions,"下一批后备船员","page",offset+4);
        action(actions,"船体强化（"+rule.getInt("hullSilver")*(hull+1)+" 贝里）","hull",0).put("enabled",hull<rule.getInt("maxHull"));
        int target=ship==1?2:1;action(actions,"切换至"+rule.getJSONArray("ships").getString(target-1)+(target==1||object(state,"ownedShips").optBoolean(String.valueOf(target))?"（已拥有）":"（首次 "+rule.getInt("shipGold")+" 钻石）"),"ship",target);
    }
    static List<Disciple> members(Player p,JSONObject state){
        List<Disciple> result=new ArrayList<Disciple>();if(state==null||!state.containsKey("cabin")||!config.getJSONObject("cabin").optBoolean("enabled"))return result;
        JSONObject cabin=state.getJSONObject("cabin");if(!cabin.containsKey("seats"))return result;JSONObject seats=cabin.getJSONObject("seats");Set<Integer> seen=new HashSet<Integer>();
        for(int i=1;i<=config.getJSONObject("cabin").getInt("slots");i++)if(seats.containsKey(String.valueOf(i))){JSONObject seat=seats.getJSONObject(String.valueOf(i));Disciple d=p.getDisciples().get(seat.optInt("crew"));if(unlocked(seat,i)&&available(p,d)&&seen.add(d.getId()))result.add(d);}
        return result;
    }
    static int[] bonus(Player player,JSONObject state){
        int[] out=new int[4];JSONObject rule=config.getJSONObject("cabin");if(!rule.optBoolean("enabled")||state==null||!state.containsKey("cabin"))return out;
        JSONObject cabin=state.getJSONObject("cabin");int hull=cabin.containsKey("hulls")?cabin.getJSONObject("hulls").optInt(String.valueOf(cabin.optInt("activeShip",1))):0;
        for(int i=0;i<4;i++)out[i]=hull*rule.getJSONArray("hullBonus").getInt(i);
        for(Disciple d:members(player,state)){
            int level=1;JSONObject seats=cabin.getJSONObject("seats");for(Object key:seats.keySet()){JSONObject s=seats.getJSONObject(key.toString());if(s.optInt("crew")==d.getId())level=s.optInt("level",1);}
            double rate=level*rule.getInt("seatPercent")/100.0;float[] base={d.getHealth(),d.getAttack(),d.getDefence(),d.getRp()};
            for(int i=0;i<4;i++)out[i]=Math.addExact(out[i],(int)Math.round(base[i]*rate));
        }
        return out;
    }
}
