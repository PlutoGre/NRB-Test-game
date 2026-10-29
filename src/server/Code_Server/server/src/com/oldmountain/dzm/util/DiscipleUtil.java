package com.oldmountain.dzm.util;

import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;

public class DiscipleUtil {
	public static Disciple newDisciple(Player player,int itemId,int level,int exp){
		DiscipleXmlTemplate discipleXmlTemplate = XMLTemplateService.getDiscipleTemplate(itemId);
		Skill skill = SkillUtil.newSkill(player, discipleXmlTemplate.getDefaultSkill(),1);
		Disciple selecter = new Disciple();
		selecter.setPlayerid(player.getId());
		selecter.setItemid(itemId);
		selecter.setAttack(discipleXmlTemplate.getDefaultAttack() + (level - 1) * discipleXmlTemplate.getGrowAttack());
		selecter.setHealth(discipleXmlTemplate.getDefaultHealth() + (level - 1) * discipleXmlTemplate.getGrowHealth());
		selecter.setDefence(discipleXmlTemplate.getDefaultDefence() + (level - 1) * discipleXmlTemplate.getGrowDefence());
		selecter.setRp(discipleXmlTemplate.getDefaultRp() + (level - 1) * discipleXmlTemplate.getGrowRp());
		selecter.setPotentiality((level-1)*discipleXmlTemplate.getDefaultPotentiality());
		selecter.setSkill1(skill.getId());
		selecter.setLevel(level);
		if (level > 1) {
			float levelMutil = XMLTemplateService.getDiscipleLevelConfig(level - 1);
			int levelExp = (int)Math.ceil(levelMutil * discipleXmlTemplate.getDefaultExp());
			exp += levelExp;
		}
		selecter.setExp(exp);
		player.addOneDisciple(selecter);
		return selecter;
	}
	
	public static OPUserDiscipleInfoRet.DiscipleInfo.Builder buildDiscipleBuilder(Disciple disciple){
		OPUserDiscipleInfoRet.DiscipleInfo.Builder builder = OPUserDiscipleInfoRet.DiscipleInfo.newBuilder();
		
		builder.setId(disciple.getId());
		builder.setItemid(disciple.getItemid());
		builder.setLevel(disciple.getLevel());
		builder.setExp(disciple.getExp());
		builder.setHealth(disciple.getHealthToInt());
		builder.setAttack(disciple.getAttackToInt());
		builder.setDefence(disciple.getDefenceToInt());
		builder.setRp(disciple.getRpToInt());
		builder.setUpgradelevel(disciple.getUpgradelevel());
		builder.setPotentiality(disciple.getPotentiality());
		builder.setSkill1(disciple.getSkill1());
		builder.setBattleid(disciple.getBattleid());
		
		return builder;
	}
}
