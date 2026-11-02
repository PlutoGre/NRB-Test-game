package com.oldmountain.dzm.util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.*;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;

/**
 * 重置阵容属性
 * @params battle 要重置的阵容
 * @params disciple 阵容上的弟子的裸体属性
 * @params player 玩家的player对象
 * @params battleDiscipleItems 所有上阵弟子的itemid的arraylist
 * @params checkFate 检查缘分的等级，0为不检查，1为检查全部（包括装备，技能，人物触发的缘），2为检查装备触发的缘，3为检查技能触发的缘,4为检查人物触发的缘，5为检查装备和技能触发的缘
 * 
 * @author dragon
 *
 */
public class BattleUtil {
	public static void resetBattleProperty(Battle battle,Disciple disciple,Player player,ArrayList<Integer> battleDiscipleItems,int checkFate){
		int health = disciple.getHealthToInt();
		int attack = disciple.getAttackToInt();
		int defence = disciple.getDefenceToInt();
		int rp = disciple.getRpToInt();
		
		if (battle.getEquip1() != 0) {
			Equip equip = player.getEquips().get(battle.getEquip1());
			if (equip != null) {
				attack += equip.getAttackToInt();
			}
		}
		
		if (battle.getEquip2() != 0) {
			Equip equip = player.getEquips().get(battle.getEquip2());
			if (equip != null) {
				defence += equip.getDefenceToInt();
			}
		}
		
		if (battle.getEquip3() != 0) {
			Equip equip = player.getEquips().get(battle.getEquip3());
			if (equip != null) {
				health += equip.getHealthToInt();
			}
		}
		
		switch (battle.getDefaultskilltype()) {
			case 3:
				health += Math.round(disciple.getHealth() * battle.getDefaultskillquantity());
				break;
			case 4:
				attack += Math.round(disciple.getAttack() * battle.getDefaultskillquantity());
				break;
			case 5:
				defence += Math.round(disciple.getDefence() * battle.getDefaultskillquantity());
				break;
			case 6:
				rp += Math.round(disciple.getRp() * battle.getDefaultskillquantity());
				break;
		}
		switch (battle.getSkill1type()) {
			case 3:
				health += Math.round(disciple.getHealth() * battle.getSkill1quantity());
				break;
			case 4:
				attack += Math.round(disciple.getAttack() * battle.getSkill1quantity());
				break;
			case 5:
				defence += Math.round(disciple.getDefence() * battle.getSkill1quantity());
				break;
			case 6:
				rp += Math.round(disciple.getRp() * battle.getSkill1quantity());
				break;
		}
		switch (battle.getSkill2type()) {
			case 3:
				health += Math.round(disciple.getHealth() * battle.getSkill2quantity());
				break;
			case 4:
				attack += Math.round(disciple.getAttack() * battle.getSkill2quantity());
				break;
			case 5:
				defence += Math.round(disciple.getDefence() * battle.getSkill2quantity());
				break;
			case 6:
				rp += Math.round(disciple.getRp() * battle.getSkill2quantity());
				break;
		}
		
		List<Integer>fates = battle.getFatesToList();
		List<Integer>fatestatus = battle.getFatesstatusToList();
		
		if (checkFate != 0) {
			ArrayList<Integer> battleEquips = new ArrayList<Integer>();
			if (battle.getEquip1item() != 0) {
				battleEquips.add(battle.getEquip1item());
			}
			if (battle.getEquip2item() != 0) {
				battleEquips.add(battle.getEquip2item());
			}
			if (battle.getEquip3item() != 0) {
				battleEquips.add(battle.getEquip3item());
			}
			ArrayList<Integer> battleSkills = new ArrayList<Integer>();
			if (battle.getSkill1item() != 0) {
				battleSkills.add(battle.getSkill1item());
			}
			if (battle.getSkill2item() != 0) {
				battleSkills.add(battle.getSkill2item());
			}
			for (int i = 0; i < fates.size(); i++) {
				int fateId = fates.get(i);
				FateXmlTemplate fateXmlTemplate = XMLTemplateService.getFateTemplate(fateId);
				String[] requires = fateXmlTemplate.getRequire().split("_");
				ArrayList<Integer> requireList = new ArrayList<Integer>();
				if (fateXmlTemplate.getType() == 1 && (checkFate == 1 || checkFate == 4)) {
					for (int j2 = 0; j2 < requires.length; j2++) {
						requireList.add(Integer.parseInt(requires[j2]));
					}
					requireList.retainAll(battleDiscipleItems);
					if (requireList.size() == requires.length) {
						fatestatus.set(i, 1);
					}
					else {
						fatestatus.set(i, 0);
					}
				}
				else if (fateXmlTemplate.getType() == 2 && (checkFate == 1 || checkFate == 3 || checkFate == 5)) {
					if (battleSkills.contains(Integer.parseInt(requires[0]))) {
						fatestatus.set(i, 1);
					}
					else {
						fatestatus.set(i, 0);
					}
				}
				else if (fateXmlTemplate.getType() == 3 && (checkFate == 1 || checkFate == 2 || checkFate == 5)){
					if (battleEquips.contains(Integer.parseInt(requires[0]))) {
						fatestatus.set(i, 1);
					}
					else {
						fatestatus.set(i, 0);
					}
				}
			}
			String newFateStatus = "" + fatestatus.get(0);
			for (int k = 1; k < fatestatus.size(); k++) {
				newFateStatus = newFateStatus + "," + fatestatus.get(k);
			}
			battle.setFatesstatus(newFateStatus);
		}
		
		for (int i = 0; i < fatestatus.size(); i++) {
			if (fatestatus.get(i) == 1) {
				int fateId = fates.get(i);
				FateXmlTemplate fateXmlTemplate = XMLTemplateService.getFateTemplate(fateId);
				switch (fateXmlTemplate.getUpgradeType()) {
					case 1:
						health += Math.round(disciple.getHealth() * fateXmlTemplate.getQuantity());
						break;
					case 2:
						attack += Math.round(disciple.getAttack() * fateXmlTemplate.getQuantity());
						break;
					case 3:
						defence += Math.round(disciple.getDefence() * fateXmlTemplate.getQuantity());
						break;
					case 4:
						rp += Math.round(disciple.getRp() * fateXmlTemplate.getQuantity());
						break;
				}
			}
		}
		
		TeamBuffer teamBuffer = player.getTeamBuffer();
		if (teamBuffer.getHealthbufferlevel() != 0) {
			health += 6 * teamBuffer.getHealthbufferlevel();
		}
		if (teamBuffer.getAttackbufferlevel() != 0) {
			attack += 3 * teamBuffer.getAttackbufferlevel();
		}
		if (teamBuffer.getDefencebufferlevel() != 0) {
			defence += 2 * teamBuffer.getDefencebufferlevel();
		}
		if (teamBuffer.getRpbufferlevel() != 0) {
			rp += 8 * teamBuffer.getRpbufferlevel();
		}	
		
		battle.setLevel(disciple.getLevel());
		battle.setHealth(health);
		battle.setAttack(attack);
		battle.setDefence(defence);
		battle.setRp(rp);
	}
	
	public static void buildBattleBuilder(OPUserBattleArrayRet.BattleArrayInfo.Builder builder,Battle battle,int position){
		builder.setId(battle.getId());
		builder.setDiscipleId(battle.getDiscipleid());
		builder.setAttack(battle.getAttack());
		builder.setDefence(battle.getDefence());
		builder.setHealth(battle.getHealth());
		builder.setRp(battle.getRp());
		builder.setPosition(position);
		builder.setSkill1(battle.getSkill1());
		builder.setSkill2(battle.getSkill2());
		builder.setEquip1(battle.getEquip1());
		builder.setEquip2(battle.getEquip2());
		builder.setEquip3(battle.getEquip3());
		String fates = battle.getFates();
		if(!fates.equals("") && fates != null){
			String fatestatus = battle.getFatesstatus();
			String[] fatesArray = fates.split(",");
			String[] fatestatusArray = fatestatus.split(",");
			for (int j = 0; j < fatesArray.length; j++) {
				OPUserBattleArrayRet.FateInfo.Builder fateBuilder = OPUserBattleArrayRet.FateInfo.newBuilder();
				fateBuilder.setFateId(Integer.parseInt(fatesArray[j]));
				fateBuilder.setStatus(Integer.parseInt(fatestatusArray[j]));
				builder.addFates(fateBuilder);
			}
		}
	}
	
	public static int getBattleCountByLevel(int level){
		int count = 2;
		if (level >= 26) {
			count = 8;
		}
		else if(level >= 21){
			count = 7;
		}
		else if (level >= 16) {
			count = 6;
		}
		else if (level >= 11) {
			count = 5;
		}
		else if (level >= 5) {
			count = 4;
		}
		else if (level >= 2) {
			count = 3;
		}
		
		return count;
	}
	
	public static BattleArrayObject getUserBattleArrayObject(Player player){
		BattleArrayObject battleObject = ObjectAccessor.getBattleArray(player.getId());
		if (battleObject == null) {
			battleObject = new BattleArrayObject();
			ArrayList<Battle> battleArrayList = new ArrayList<Battle>();
			HashMap<Integer, Battle> allBattles = new HashMap<Integer, Battle>();
			HashMap<Integer, Battle> userBattles = new HashMap<Integer, Battle>();
			if (player.getBattlearray() == null) {
				List<Player> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, player.getId());
				if (list.size() == 1) {
					player = list.get(0);
				}
				
				List<Battle> battles = DZM.getEntityManager().limitQuery("from Battle where playerid = ?", 0, 8, player.getId());
				
				if (battles.size() > 0) {
					for (int i = 0; i < battles.size(); i++) {
						allBattles.put(battles.get(i).getId(), battles.get(i));
					}
				}
			}
			else {
				userBattles = player.getBattlearray();
			}
			String[] battleIds = player.getBattleids().split(",");
			for (int i = 0; i < battleIds.length; i++) {
				int id = Integer.parseInt(battleIds[i]);
				Battle battle = allBattles.get(id);
				battleArrayList.add(battle);
				userBattles.put(id, battle);
			}
			player.setBattlearray(userBattles);
			battleObject.setPlayerid(player.getId());
			battleObject.setMomentum(player.getMomentum());
			battleObject.setBattlearrays(battleArrayList);
			ObjectAccessor.addBattleArray(battleObject);
		}
		
		return battleObject;
	}
}
