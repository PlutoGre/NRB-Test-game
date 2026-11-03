package com.oldmountain.dzm.admin;

import java.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import com.oldmountain.dzm.entity.*;
import static com.oldmountain.dzm.admin.ExpansionSystems.*;
import static com.oldmountain.dzm.admin.GrowthSystems.*;

/** @brief 船员成长进化、超进化、培养重生和可保留或放弃的潜能训练。 */
final class CrewGrowth {
    private CrewGrowth() { }
    static Disciple crew(Player p,int id){Disciple d=p.getDisciples().get(id);if(d==null||d.getExpiretime()!=0)throw new IllegalArgumentException("船员不存在");return d;}
    static void execute(String mode,String action,JSONObject request,final ExpansionSystems.Context ctx){
        final Disciple d=crew(ctx.player,request.optInt("id"));JSONObject rule=rules(mode),rec=record(ctx.state,mode,d.getId());
        if("training".equals(mode)){training(action,d,rec,rule,ctx);return;}
        if("evolve".equals(action)||"super".equals(action)){
            String field="evolve".equals(action)?"rank":"super";int level=rec.optInt(field);limited(level,rule.getInt(field.equals("rank")?"maxRank":"maxSuper"));
            if(field.equals("super")&&rec.optInt("rank")<rule.getInt("superRequires"))throw new IllegalArgumentException("请先完成普通进化 "+rule.getInt("superRequires")+" 阶");
            int gold=Math.multiplyExact(rule.getInt(field.equals("rank")?"rankGold":"superGold"),level+1),silver=Math.multiplyExact(rule.getInt(field.equals("rank")?"rankSilver":"superSilver"),level+1);
            ctx.gold(gold);ctx.silver(silver);rec.put(field,level+1);rec.put("spentGold",Math.addExact(rec.optInt("spentGold"),gold));rec.put("spentSilver",Math.addExact(rec.optInt("spentSilver"),silver));ctx.message(field.equals("rank")?"船员成长进化成功":"船员超进化成功");
        }else if("rebirth".equals(action)){
            if(rec.optInt("rank")+rec.optInt("super")==0)throw new IllegalArgumentException("没有可重生的本页养成进度");
            JSONArray items=new JSONArray();int gold=rec.optInt("spentGold")*rule.getInt("refundPercent")/100,silver=rec.optInt("spentSilver")*rule.getInt("refundPercent")/100;
            if(gold>0)items.add(catalog.attachment("property:1001",gold));if(silver>0)items.add(catalog.attachment("property:1002",silver));if(!items.isEmpty())ctx.mail("船员培养重生返还",items);
            object(object(ctx.state,"crew"),"items").remove(String.valueOf(d.getId()));ctx.message("培养等阶已重置，返还通过传书领取；原等级、突破、装备及潜能训练保留");
        }else throw new IllegalArgumentException("未知船员养成操作");
    }
    private static void training(String action,final Disciple d,JSONObject rec,JSONObject rule,final ExpansionSystems.Context ctx){
        if("roll".equals(action)){
            if(rec.containsKey("pending"))throw new IllegalArgumentException("请先保留或放弃当前训练结果");
            if(d.getPotentiality()<rule.getInt("potentialCost"))throw new IllegalArgumentException("船员剩余潜能不足");
            final Bag bag=ctx.player.getBags().get(rule.getInt("toolId"));int count=rule.getInt("toolCount");
            if(bag==null||bag.getCount()<count)throw new IllegalArgumentException("潜能药剂不足");
            final int remaining=bag.getCount()-count;
            if(ctx.db.createSQLQuery("UPDATE bag SET count=:remaining WHERE id=:id AND playerid=:p AND count=:old")
                .setInteger("remaining",remaining).setInteger("id",bag.getId()).setInteger("p",ctx.player.getId()).setInteger("old",bag.getCount()).executeUpdate()!=1)throw new IllegalArgumentException("药剂数量已改变，请刷新");
            ctx.updates.add(new Runnable(){public void run(){bag.setCount(remaining);}});
            JSONObject preview=new JSONObject();preview.put("attribute",RANDOM.nextInt(4));preview.put("units",1+RANDOM.nextInt(rule.getInt("maxGain")));rec.put("pending",preview);ctx.message("训练完成，请保留或放弃随机结果；放弃不退药剂");
        }else if("accept".equals(action)){
            if(!rec.containsKey("pending"))throw new IllegalArgumentException("没有待保留的训练结果");
            final int remaining=d.getPotentiality()-rule.getInt("potentialCost");if(remaining<0)throw new IllegalArgumentException("当前剩余潜能不足，请放弃结果");
            JSONObject pending=rec.getJSONObject("pending");String attribute=String.valueOf(pending.getInt("attribute"));
            JSONObject bonus=object(rec,"bonus");bonus.put(attribute,Math.addExact(bonus.optInt(attribute),pending.getInt("units")*rule.getJSONArray("attributeUnits").getInt(pending.getInt("attribute"))));
            if(ctx.db.createSQLQuery("UPDATE disciple SET potentiality=:remaining WHERE id=:id AND playerid=:p AND potentiality=:old")
                .setInteger("remaining",remaining).setInteger("id",d.getId()).setInteger("p",ctx.player.getId()).setInteger("old",d.getPotentiality()).executeUpdate()!=1)throw new IllegalArgumentException("船员潜能已改变，请刷新");
            ctx.updates.add(new Runnable(){public void run(){d.setPotentiality(remaining);}});rec.remove("pending");rec.put("accepted",Math.addExact(rec.optInt("accepted"),1));ctx.message("训练属性已保留并计入战斗");
        }else if("discard".equals(action)){
            if(!rec.containsKey("pending"))throw new IllegalArgumentException("没有待放弃的训练结果");rec.remove("pending");ctx.message("已放弃本次训练结果");
        }else throw new IllegalArgumentException("未知训练操作");
    }
    static void view(String mode,JSONObject request,ExpansionSystems.Context ctx,JSONArray lines,JSONArray actions){
        JSONObject rule=rules(mode);List<Integer> ids=new ArrayList<Integer>();for(Disciple d:ctx.player.getDisciples().values())if(d.getExpiretime()==0)ids.add(d.getId());Collections.sort(ids);
        int selected=request.optInt("id");if(!ids.contains(selected))selected=ids.isEmpty()?0:ids.get(0);
        if("crew".equals(mode))lines.add("默认成长：进化每阶 +"+rule.getInt("rankPercent")+"%，超进化每阶 +"+rule.getInt("superPercent")+"% 四维基础属性；不改变卡牌外观与模板。");
        else lines.add("每次训练消耗 "+rule.getInt("toolCount")+" 潜能药剂；四种属性等概率，随机 1～"+rule.getInt("maxGain")+" 单位。保留消耗 "+rule.getInt("potentialCost")+" 剩余潜能；放弃不扣潜能且不退药剂。");
        if(selected>0){Disciple d=crew(ctx.player,selected);JSONObject rec=record(ctx.state,mode,selected);
            lines.add("当前："+name("card",d.getItemid())+" #"+selected+"　等级 "+d.getLevel()+"　剩余潜能 "+d.getPotentiality());
            if("crew".equals(mode)){
                int rank=rec.optInt("rank"),sup=rec.optInt("super");lines.add("进化 "+rank+" / "+rule.getInt("maxRank")+"　超进化 "+sup+" / "+rule.getInt("maxSuper"));
                action(actions,"船员进化（"+rule.getInt("rankGold")*(rank+1)+" 钻石 + "+rule.getInt("rankSilver")*(rank+1)+" 贝里）","evolve",selected).put("enabled",rank<rule.getInt("maxRank"));
                action(actions,"超进化（需进化 "+rule.getInt("superRequires")+" 阶；"+rule.getInt("superGold")*(sup+1)+" 钻石 + "+rule.getInt("superSilver")*(sup+1)+" 贝里）","super",selected).put("enabled",rank>=rule.getInt("superRequires")&&sup<rule.getInt("maxSuper"));
                action(actions,"培养重生（返还 "+rule.getInt("refundPercent")+"% 本页投入，保留原等级及训练）","rebirth",selected).put("enabled",rank+sup>0);
            }else{
                final String[] attrs={"生命","攻击","防御","意志"};JSONObject bonus=object(rec,"bonus");lines.add("已保留加成：生命 "+bonus.optInt("0")+"　攻击 "+bonus.optInt("1")+"　防御 "+bonus.optInt("2")+"　意志 "+bonus.optInt("3"));
                Bag bag=ctx.player.getBags().get(rule.getInt("toolId"));lines.add("潜能药剂剩余 "+(bag==null?0:bag.getCount()));
                if(rec.containsKey("pending")){JSONObject p=rec.getJSONObject("pending");lines.add("待确认训练："+attrs[p.getInt("attribute")]+" +"+p.getInt("units")*rule.getJSONArray("attributeUnits").getInt(p.getInt("attribute")));action(actions,"保留训练结果","accept",selected);action(actions,"放弃训练结果","discard",selected);}
                else action(actions,"开始潜能训练（"+rule.getInt("toolCount")+" 药剂）","roll",selected).put("enabled",d.getPotentiality()>=rule.getInt("potentialCost"));
            }
        }
        int offset=Math.max(0,Math.min(request.optInt("offset"),Math.max(0,ids.size()-1)));
        for(int i=offset;i<Math.min(offset+4,ids.size());i++){Disciple d=crew(ctx.player,ids.get(i));action(actions,"选择："+name("card",d.getItemid())+" #"+d.getId(),"select",d.getId());}
        if(offset>0)action(actions,"上一批船员","page",Math.max(0,offset-4));if(offset+4<ids.size())action(actions,"下一批船员","page",offset+4);
    }
    static void apply(Battle b,Disciple d,Player p,JSONObject state){
        JSONObject rule=config.getJSONObject("crew"),rec=readRecord(state,"crew",d.getId());
        double rate=rule.optBoolean("enabled")?(rec.optInt("rank")*rule.optInt("rankPercent")+rec.optInt("super")*rule.optInt("superPercent"))/100.0:0;
        JSONObject train=readRecord(state,"training",d.getId());JSONObject bonus=train.containsKey("bonus")&&config.getJSONObject("training").optBoolean("enabled")?train.getJSONObject("bonus"):new JSONObject();
        b.setHealth(Math.addExact(b.getHealth(),(int)Math.round(d.getHealth()*rate)+bonus.optInt("0")));
        b.setAttack(Math.addExact(b.getAttack(),(int)Math.round(d.getAttack()*rate)+bonus.optInt("1")));
        b.setDefence(Math.addExact(b.getDefence(),(int)Math.round(d.getDefence()*rate)+bonus.optInt("2")));
        b.setRp(Math.addExact(b.getRp(),(int)Math.round(d.getRp()*rate)+bonus.optInt("3")));
    }
}
