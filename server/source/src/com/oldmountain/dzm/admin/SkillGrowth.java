package com.oldmountain.dzm.admin;

import java.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.util.XMLTemplateService;
import static com.oldmountain.dzm.admin.ExpansionSystems.*;
import static com.oldmountain.dzm.admin.GrowthSystems.*;

/** @brief 奥义进化与天生奥义替换，保留原实例和装配关系。 */
final class SkillGrowth {
    private SkillGrowth() { }
    static Skill skill(Player p,int id){Skill s=p.getSkills().get(id);if(s==null||s.getExpiretime()!=0)throw new IllegalArgumentException("奥义不存在");return s;}
    static void execute(String action,JSONObject request,final ExpansionSystems.Context ctx){
        final Skill skill=skill(ctx.player,request.optInt("id"));JSONObject rule=rules("skills"),rec=record(ctx.state,"skills",skill.getId());
        if("evolve".equals(action)){
            int rank=rec.optInt("rank");limited(rank,rule.getInt("maxRank"));ctx.gold(Math.multiplyExact(rule.getInt("rankGold"),rank+1));ctx.silver(Math.multiplyExact(rule.getInt("rankSilver"),rank+1));rec.put("rank",rank+1);ctx.message("奥义进化成功，实际战斗威力已提升");
        }else if("replace".equals(action)){
            if(skill.getIsdefaultskill()!=1)throw new IllegalArgumentException("只能替换船员的天生奥义");
            boolean attached=false;for(Disciple d:ctx.player.getDisciples().values())if(d.getSkill1()==skill.getId())attached=true;
            if(!attached)throw new IllegalArgumentException("天生奥义未绑定船员");
            final int target=request.optInt("choice");boolean allowed=false;JSONArray choices=rule.getJSONArray("inbornChoices");
            for(int i=0;i<choices.size();i++)if(choices.getInt(i)==target)allowed=true;
            if(!allowed||XMLTemplateService.getSkillTemplate(target)==null)throw new IllegalArgumentException("此奥义不在可替换列表");
            if(target==skill.getItemid())throw new IllegalArgumentException("已经拥有相同天生奥义");
            final SkillXmlTemplate template=XMLTemplateService.getSkillTemplate(target);
            final float quantity=template.getQuantity()+(skill.getLevel()-1)*template.getGrowth();
            ctx.gold(rule.getInt("replaceGold"));
            if(ctx.db.createSQLQuery("UPDATE skill SET itemid=:item,type=:type,quantity=:q WHERE id=:id AND playerid=:player AND isdefaultskill=1")
                .setInteger("item",target).setInteger("type",template.getType()).setFloat("q",quantity).setInteger("id",skill.getId()).setInteger("player",ctx.player.getId()).executeUpdate()!=1)throw new IllegalArgumentException("奥义状态已改变");
            ctx.updates.add(new Runnable(){public void run(){skill.setItemid(target);skill.setType(template.getType());skill.setQuantity(quantity);}});
            ctx.message("天生奥义已替换为"+name("skill",target)+"，原奥义实例等级及进化等阶保留");
        }else throw new IllegalArgumentException("未知奥义操作");
    }
    static void view(JSONObject request,ExpansionSystems.Context ctx,JSONArray lines,JSONArray actions){
        JSONObject rule=rules("skills");lines.add("奥义进化每阶提升 "+rule.getInt("rankPercent")+"% 战斗数值，必定成功；原升级等级、装配和实例编号保留。");
        List<Integer> ids=new ArrayList<Integer>();for(Skill s:ctx.player.getSkills().values())if(s.getExpiretime()==0)ids.add(s.getId());Collections.sort(ids);
        int selected=request.optInt("id");if(!ids.contains(selected))selected=ids.isEmpty()?0:ids.get(0);
        if(selected>0){Skill s=skill(ctx.player,selected);JSONObject rec=record(ctx.state,"skills",selected);int rank=rec.optInt("rank");
            lines.add("当前："+name("skill",s.getItemid())+" #"+selected+(s.getIsdefaultskill()==1?"（天生）":"（额外）")+"　等级 "+s.getLevel()+"　进化 "+rank);
            lines.add("基础数值 "+s.getQuantity()+"　进化后 "+(float)(s.getQuantity()*(1+rank*rule.getInt("rankPercent")/100.0)));
            action(actions,"奥义进化（"+rule.getInt("rankGold")*(rank+1)+" 钻石 + "+rule.getInt("rankSilver")*(rank+1)+" 贝里）","evolve",selected).put("enabled",rank<rule.getInt("maxRank"));
            if(s.getIsdefaultskill()==1){JSONArray choices=rule.getJSONArray("inbornChoices");for(int i=0;i<choices.size();i++)if(s.getItemid()!=choices.getInt(i))action(actions,"替换天生奥义为"+name("skill",choices.getInt(i))+"（"+rule.getInt("replaceGold")+" 钻石）","replace",selected).put("choice",choices.getInt(i));}
        }
        int offset=Math.max(0,Math.min(request.optInt("offset"),Math.max(0,ids.size()-1)));
        for(int i=offset;i<Math.min(offset+4,ids.size());i++){Skill s=skill(ctx.player,ids.get(i));action(actions,"选择："+name("skill",s.getItemid())+" #"+s.getId()+(s.getIsdefaultskill()==1?"（天生）":"（额外）"),"select",s.getId());}
        if(offset>0)action(actions,"上一批奥义","page",Math.max(0,offset-4));if(offset+4<ids.size())action(actions,"下一批奥义","page",offset+4);
    }
    static float quantity(Player player,Skill skill){
        JSONObject state=ExpansionSystems.cached(player.getId());JSONObject rule=config.getJSONObject("skills");
        int rank=rule.optBoolean("enabled")?readRecord(state,"skills",skill.getId()).optInt("rank"):0;
        return (float)(skill.getQuantity()*(1+rank*rule.optInt("rankPercent")/100.0));
    }
    static void sync(Battle battle,Player player){
        Skill natural=player.getSkills().get(battle.getDefaultskill()),first=player.getSkills().get(battle.getSkill1()),second=player.getSkills().get(battle.getSkill2());
        if(natural!=null){battle.setDefaultskillitem(natural.getItemid());battle.setDefaultskilltype(natural.getType());battle.setDefaultskillquantity(quantity(player,natural));}
        if(first!=null){battle.setSkill1item(first.getItemid());battle.setSkill1type(first.getType());battle.setSkill1quantity(quantity(player,first));}
        if(second!=null){battle.setSkill2item(second.getItemid());battle.setSkill2type(second.getType());battle.setSkill2quantity(quantity(player,second));}
    }
}
