package com.oldmountain.dzm.admin;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.util.Util;
import static com.oldmountain.dzm.admin.ExpansionSystems.*;
import static com.oldmountain.dzm.admin.GrowthSystems.*;

/**
 * @brief 运宝与养成的展示快照。只读取本角色的数据，不改变消耗、存档或操作权限。
 * 数值由服务端计算；客户端仅负责页签、图标、选择弹层和确认展示。
 */
final class ExpansionPresentation {
    private static final String[] ATTRS={"生命","攻击","防御","意志"};
    private ExpansionPresentation() { }

    private static void row(JSONArray rows,String label,Object value){
        JSONObject row=new JSONObject();row.put("label",label);row.put("value",String.valueOf(value));rows.add(row);
    }
    private static JSONObject subject(String kind,int template,int id,String title,int level){
        JSONObject out=new JSONObject();out.put("kind",kind);out.put("templateId",template);out.put("id",id);out.put("name",title);out.put("level",level);return out;
    }
    private static JSONObject owned(String mode,Player player,int id){
        if("equipment".equals(mode)){Equip e=player.getEquips().get(id);if(e!=null)return subject("equip",e.getItemid(),id,name("equip",e.getItemid()),e.getLevel());}
        else if("skills".equals(mode)){Skill s=player.getSkills().get(id);if(s!=null)return subject("skill",s.getItemid(),id,name("skill",s.getItemid()),s.getLevel());}
        else {Disciple d=player.getDisciples().get(id);if(d!=null)return subject("card",d.getItemid(),id,name("card",d.getItemid()),d.getLevel());}
        return new JSONObject();
    }
    private static void preview(JSONObject previews,String action,String label,Object before,Object after){
        JSONArray rows=previews.containsKey(action)?previews.getJSONArray(action):new JSONArray();
        row(rows,label,String.valueOf(before)+" → "+String.valueOf(after));previews.put(action,rows);
    }

    /** @param actions 业务层已生成的合法操作；附加展示数据不会新增执行权限。 */
    static JSONObject snapshot(String mode,JSONObject request,Context ctx,JSONArray actions){
        JSONObject out=new JSONObject(),previews=new JSONObject();JSONArray metrics=new JSONArray();
        JSONObject rule=rules(mode);int selected=0;
        for(int i=0;i<actions.size();i++){
            JSONObject a=actions.getJSONObject(i);String action=a.getString("action");int id=a.optInt("id");
            if("select".equals(action)){
                a.put("subject",owned(mode,ctx.player,id));
                if(selected==0||id==request.optInt("id"))selected=id;
            }else if("synthesize".equals(action))a.put("subject",subject("equip",id,0,name("equip",id),1));
            else if("replace".equals(action))a.put("subject",subject("skill",a.getInt("choice"),0,name("skill",a.getInt("choice")),1));
        }
        // 当前选择可能位于另一页背包；执行按钮的实例 ID 才是业务层最终选择。
        for(int i=0;i<actions.size();i++){
            JSONObject a=actions.getJSONObject(i);String action=a.getString("action");
            if(a.optInt("id")>0&&!"select".equals(action)&&!"page".equals(action)&&!"choice".equals(action)&&!"ship".equals(action)&&!"synthesize".equals(action)){selected=a.getInt("id");break;}
        }
        if(!"transport".equals(mode)&&selected>0)out.put("selected",owned(mode,ctx.player,selected));
        if("transport".equals(mode)){
            JSONObject s=object(ctx.state,mode);int quality=Math.max(1,Math.min(3,s.optInt("quality",1))),end=s.optInt("end");
            row(metrics,"今日运输",(day().equals(s.optString("day"))?s.optInt("runs"):0)+" / "+rule.getInt("dailyRuns"));
            row(metrics,"宝船品质",new String[]{"普通","精良","珍稀"}[quality-1]);
            row(metrics,"预计贝里",rule.getJSONArray("silverRewards").getInt(quality-1));
            row(metrics,"预计药剂",rule.getJSONArray("toolRewards").getInt(quality-1));
            row(metrics,"预计钻石",rule.getJSONArray("goldRewards").getInt(quality-1));
            row(metrics,"运输状态",end==0?"等待起航":end>Util.getServerTime()?"运输中":"奖励待领取");
            if(end>0)row(metrics,"剩余时间",Math.max(0,end-Util.getServerTime())+" 秒");
            out.put("quality",quality);out.put("end",end);out.put("duration",rule.getInt("durationSeconds"));
        }else if("equipment".equals(mode)&&selected>0){
            Equip e=ctx.player.getEquips().get(selected);JSONObject rec=readRecord(ctx.state,mode,selected);
            int temper=rec.optInt("temper"),rank=rec.optInt("rank");
            row(metrics,"原强化",e.getLevel());row(metrics,"淬炼",temper+" / "+rule.getInt("maxTemper"));row(metrics,"进阶",rank+" / "+rule.getInt("maxRank"));
            row(metrics,"工坊碎片",object(ctx.state,mode).optInt("shards"));row(metrics,"装备状态",equipped(ctx.player,e)?"已穿戴，不能分解":"闲置");
            float base=e.getType()==1?e.getAttack():e.getType()==2?e.getDefence():e.getHealth();String attr=e.getType()==1?"攻击":e.getType()==2?"防御":"生命";
            float current=base+temper*rule.getInt("temperFlat")+(int)Math.round(base*rank*rule.getInt("rankPercent")/100.0);
            preview(previews,"temper",attr,current,current+rule.getInt("temperFlat"));
            preview(previews,"promote",attr,current,base+temper*rule.getInt("temperFlat")+(int)Math.round(base*(rank+1)*rule.getInt("rankPercent")/100.0));
        }else if("skills".equals(mode)&&selected>0){
            Skill s=ctx.player.getSkills().get(selected);int rank=readRecord(ctx.state,mode,selected).optInt("rank");
            row(metrics,"奥义等级",s.getLevel());row(metrics,"进化",rank+" / "+rule.getInt("maxRank"));row(metrics,"奥义类型",s.getIsdefaultskill()==1?"天生奥义":"额外奥义");
            preview(previews,"evolve","战斗数值",(float)(s.getQuantity()*(1+rank*rule.getInt("rankPercent")/100.0)),(float)(s.getQuantity()*(1+(rank+1)*rule.getInt("rankPercent")/100.0)));
        }else if(("crew".equals(mode)||"training".equals(mode))&&selected>0){
            Disciple d=ctx.player.getDisciples().get(selected);JSONObject rec=readRecord(ctx.state,mode,selected);float[] base={d.getHealth(),d.getAttack(),d.getDefence(),d.getRp()};
            row(metrics,"船员等级",d.getLevel());row(metrics,"剩余潜能",d.getPotentiality());
            if("crew".equals(mode)){
                int rank=rec.optInt("rank"),sup=rec.optInt("super");row(metrics,"进化",rank+" / "+rule.getInt("maxRank"));row(metrics,"超进化",sup+" / "+rule.getInt("maxSuper"));
                for(int i=0;i<4;i++){
                    double rate=rank*rule.getInt("rankPercent")+sup*rule.getInt("superPercent");
                    preview(previews,"evolve",ATTRS[i]+"成长加成",Math.round(base[i]*rate/100.0),Math.round(base[i]*(rate+rule.getInt("rankPercent"))/100.0));
                    preview(previews,"super",ATTRS[i]+"成长加成",Math.round(base[i]*rate/100.0),Math.round(base[i]*(rate+rule.getInt("superPercent"))/100.0));
                }
            }else{
                Bag bag=ctx.player.getBags().get(rule.getInt("toolId"));row(metrics,"潜能药剂",bag==null?0:bag.getCount());
                JSONObject bonus=rec.containsKey("bonus")?rec.getJSONObject("bonus"):new JSONObject();
                for(int i=0;i<4;i++)row(metrics,ATTRS[i]+"训练加成",bonus.optInt(String.valueOf(i)));
                if(rec.containsKey("pending")){JSONObject p=rec.getJSONObject("pending");int attr=p.getInt("attribute");row(metrics,"本次待保留",ATTRS[attr]+" +"+p.getInt("units")*rule.getJSONArray("attributeUnits").getInt(attr));}
            }
        }else if("equipment".equals(mode)){
            row(metrics,"工坊碎片",object(ctx.state,mode).optInt("shards"));
        }else if("cabin".equals(mode)){
            JSONObject state=object(ctx.state,mode);int ship=state.optInt("activeShip",1),slot=request.optInt("choice",1);if(slot<1||slot>rule.getInt("slots"))slot=1;
            out.put("slot",slot);out.put("ship",ship);row(metrics,"当前船体",rule.getJSONArray("ships").getString(ship-1));row(metrics,"船体强化",object(state,"hulls").optInt(String.valueOf(ship))+" / "+rule.getInt("maxHull"));
            int[] bonus=CabinGrowth.bonus(ctx.player,ctx.state);for(int i=0;i<4;i++)row(metrics,"全队"+ATTRS[i],"+"+bonus[i]);
            JSONArray seats=new JSONArray();
            for(int i=1;i<=rule.getInt("slots");i++){
                JSONObject storedSeats=state.containsKey("seats")?state.getJSONObject("seats"):new JSONObject();
                JSONObject seat=storedSeats.containsKey(String.valueOf(i))?storedSeats.getJSONObject(String.valueOf(i)):new JSONObject(),item=new JSONObject();
                item.put("slot",i);item.put("unlocked",CabinGrowth.unlocked(seat,i));item.put("level",seat.optInt("level",1));
                if(CabinGrowth.available(ctx.player,ctx.player.getDisciples().get(seat.optInt("crew"))))item.put("occupant",owned(mode,ctx.player,seat.optInt("crew")));
                seats.add(item);
            }
            out.put("seats",seats);
        }
        out.put("metrics",metrics);out.put("previews",previews);return out;
    }
}
