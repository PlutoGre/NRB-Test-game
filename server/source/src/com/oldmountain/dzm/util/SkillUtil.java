package com.oldmountain.dzm.util;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillXmlTemplate;
import com.oldmountain.dzm.message.LeagueStructExt.OPLeaguaShopBuyRet;
import com.oldmountain.dzm.message.UserSkillInfo.OPUserSkillInfoRet;

public class SkillUtil {
	public static Skill newSkill(Player player,int itemId,int isDefault){
		SkillXmlTemplate defaultSkill = XMLTemplateService.getSkillTemplate(itemId);
		Skill skill = new Skill();
		skill.setPlayerid(player.getId());
		skill.setItemid(defaultSkill.getId());
		skill.setType(defaultSkill.getType());
		skill.setQuantity(defaultSkill.getQuantity());
		skill.setIsdefaultskill(isDefault);
		player.addOneSkill(skill);
		return skill;
	}
	public static OPUserSkillInfoRet.SkillInfo.Builder buildSkillBuilder(Skill skill){
		OPUserSkillInfoRet.SkillInfo.Builder builder = OPUserSkillInfoRet.SkillInfo.newBuilder();
		
		builder.setId(skill.getId());
		builder.setItemid(skill.getItemid());
		builder.setLevel(skill.getLevel());
		builder.setIsborn(skill.getIsdefaultskill());
		builder.setConsume(skill.getConsume());
		builder.setQuantity(skill.getQuantity());
		
		return builder;
	}
	
	public static OPLeaguaShopBuyRet.SkillInfo.Builder buildSkillBuilderForLeagug(Skill skill){
		OPLeaguaShopBuyRet.SkillInfo.Builder builder = OPLeaguaShopBuyRet.SkillInfo.newBuilder();
		
		builder.setId(skill.getId());
		builder.setItemid(skill.getItemid());
		builder.setLevel(skill.getLevel());
		builder.setIsborn(skill.getIsdefaultskill());
		builder.setConsume(skill.getConsume());
		builder.setQuantity(skill.getQuantity());
		
		return builder;
	}
}
