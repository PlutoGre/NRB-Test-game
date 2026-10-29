package com.oldmountain.dzm.util;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import net.sf.json.JSONObject;

import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Handbook;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.SkillBook;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.HandbookInfo.OPHandbookInfoRet;

public class HandbookUtil {
	public static final int HAS_GOT=0;//0拥有该武将
	public static final int HAS_MEET=1;//1见过该武将
	public static final int HAS_NOT_MEET=2;//2没见过该武将
	
	public static Handbook insertOrUpdateHandbook(Player player, String discipleInfo, String equipInfo , String skillInfo){
		
				
		Handbook handbook = new Handbook();
		handbook.setPlayerId(player.getId());
		if(discipleInfo != null && !discipleInfo.equals("")&&!discipleInfo.equals("{}")){
			handbook.setDiscipleInfo(discipleInfo);
		}else{
			if(player.getHandbook().getDiscipleInfo() != null && !player.getHandbook().getDiscipleInfo().equals("") && !player.getHandbook().getDiscipleInfo().equals("{}")){
				handbook.setDiscipleInfo(player.getHandbook().getDiscipleInfo());
			}else{
				handbook.setDiscipleInfo("{}");
			}
		}
		if(equipInfo != null && !equipInfo.equals("")&&!equipInfo.equals("{}")){
			handbook.setEquipInfo(equipInfo);
		}else{
			if(player.getHandbook().getEquipInfo() != null && !player.getHandbook().getEquipInfo().equals("") && !player.getHandbook().getEquipInfo().equals("{}")){
				handbook.setEquipInfo(player.getHandbook().getEquipInfo());
			}else{
				handbook.setEquipInfo("{}");
			}
		}
		if(skillInfo != null && !skillInfo.equals("")&&!skillInfo.equals("{}")){
			handbook.setSkillInfo(skillInfo);
		}else{
			if(player.getHandbook().getSkillInfo() != null && !player.getHandbook().getSkillInfo().equals("") && !player.getHandbook().getSkillInfo().equals("{}")){
				handbook.setSkillInfo(player.getHandbook().getSkillInfo());
			}else{
				handbook.setSkillInfo("{}");
			}
		}
		player.addOrUpdateHandbook(handbook);
		return handbook;
	}
	
	public static void gotDisciple(Player player , int discipleId){
		addDisciple(player, discipleId,0);
	}
	
	public static void meetDisciple(Player player , int discipleId){
		addDisciple(player, discipleId,1);
	}

	public static void addDisciple(Player player,int discipleId, int showType){
		player.addDisciple(discipleId,showType);
	}

	public static void gotEquip(Player player , int equipId){
		addEquip(player, equipId, 0);
	}
	
	public static void meetEquip(Player player , int equipId){
		addEquip(player, equipId, 1);
	}

	public static void addEquip(Player player,int equipId, int showType){
		player.addEquip(equipId,showType);
	}

	public static void gotSkill(Player player , int skillId){
		addSkill(player,skillId, 0);
	}
	
	public static void meetSkill(Player player , int skillId){
		addSkill(player,skillId, 1);
	}
	
	public static void addSkill(Player player,int skillId, int showType){
		player.addSkill(skillId,showType);
	}
	public static OPHandbookInfoRet.Builder buildHandbookBuilder(Handbook handbook){
		OPHandbookInfoRet.Builder builder = OPHandbookInfoRet.newBuilder();
		int time = Util.getServerTime();
		builder.setServertime(time);
		
		//TODO 玩家弟子列表，需要确定的是存在player中还是单独存一个
		OPHandbookInfoRet.PersonInfo.Builder personBuilder = OPHandbookInfoRet.PersonInfo.newBuilder();
		OPHandbookInfoRet.EquipmentInfo.Builder equipmentBuilder = OPHandbookInfoRet.EquipmentInfo.newBuilder();
		OPHandbookInfoRet.SkillInfo.Builder skillBuilder = OPHandbookInfoRet.SkillInfo.newBuilder();

		
        JSONObject discipleItem = new JSONObject();
        int[] discipleRewardItem = new int[200];
        /*
        discipleItem.put(1, discipleRewardItem[0]);
        discipleItem.put(2, discipleRewardItem[1]);
        discipleItem.put(3, discipleRewardItem[2]);
        */
        
//		HandbookUtil.insertOrUpdateHandbook(player, discipleItem.toString(), discipleItem.toString(), discipleItem.toString());
		
        /*
		HandbookUtil.gotDisciple(player, 3);
		HandbookUtil.meetDisciple(player, 9);
		HandbookUtil.gotEquip(player, 10);
		HandbookUtil.meetDisciple(player, 22);
		*/

//		HandbookUtil.meetEquip(player, 25);
		int id = 0;

		JSONObject discipleContent = JSONObject.fromObject(handbook.getDiscipleInfo());
		if(handbook.getDiscipleInfo() != null && !handbook.getDiscipleInfo().equals("") && !handbook.getDiscipleInfo().equals("{}")){
			Iterator<?> iterator = discipleContent.keys();
			
			while(iterator.hasNext()){
				id = Integer.parseInt(iterator.next().toString());
				personBuilder.setId(id);
				personBuilder.setUserid(id);
				personBuilder.setUserstatus(discipleContent.getInt(String.valueOf(id)));
				builder.addPersonInfo(personBuilder);
			}
			
		}

		//0拥有该武将
		//1见过该武将
		//2没见过该武将
		
		JSONObject equipContent = JSONObject.fromObject(handbook.getEquipInfo());
		if(handbook.getEquipInfo() != null && !handbook.getEquipInfo().equals("") && !handbook.getEquipInfo().equals("{}")){
			Iterator<?> iterator = equipContent.keys();
			
			while(iterator.hasNext()){
				id = Integer.parseInt(iterator.next().toString());
				equipmentBuilder.setId(id);
				equipmentBuilder.setEquipmentid(id);
				equipmentBuilder.setEquipmentstatus(equipContent.getInt(String.valueOf(id)));
				builder.addEquipmentInfo(equipmentBuilder);
			}
		}
		
		JSONObject skillContent = JSONObject.fromObject(handbook.getSkillInfo());
		if(handbook.getSkillInfo() != null && !handbook.getSkillInfo().equals("") && !handbook.getSkillInfo().equals("{}")){
			Iterator<?> iterator = skillContent.keys();
			while(iterator.hasNext()){
				id = Integer.parseInt(iterator.next().toString());
				skillBuilder.setId(id);
				skillBuilder.setSkillid(id);
				skillBuilder.setSkillstatus(skillContent.getInt(String.valueOf(id)));
				builder.addSkillInfo(skillBuilder);
			}
		}
		
		return builder;
	}

	//检测玩家是否第一次登入图鉴系统,如果为第一次登入,则将玩家的相关信息加入图鉴中
	public static void checkFirstVisit(Player player) {
		Handbook handbook=player.getHandbook();
		Map<String, Integer> discipleMap=Util.jsonToMap(handbook.getDiscipleInfo());
		Map<String, Integer> equipMap=Util.jsonToMap(handbook.getEquipInfo());
		Map<String, Integer> skillMap=Util.jsonToMap(handbook.getSkillInfo());
		HashMap<Integer, Disciple> disciples=player.getDisciples();
		HashMap<Integer, Soul> souls=player.getSouls();
		HashMap<Integer, Equip> equips=player.getEquips();
		HashMap<Integer, Skill> skills=player.getSkills();
		HashMap<Integer, SkillBook> skillbooks=player.getSkillBooks();
		//如果玩家的装备 船员 技能图鉴中记录不全时,则将未记录的图鉴信息添加进去
		if(discipleMap.size()<discipleMap.size() || equipMap.size()<equipMap.size() || skillMap.size()<skills.size()){
			//将已有的残章信息加入图鉴列表
			if(skillbooks!=null && skillbooks.size()>0)
			{
				Iterator<Integer> keyset=skillbooks.keySet().iterator();
				while(keyset.hasNext()){
					int skill_itemId=keyset.next();
					String skill_itemId_s=String.valueOf(skill_itemId);
					if(skillMap.containsKey(skill_itemId_s) && skillMap.get(skill_itemId_s)==HAS_GOT){
						
					}
					else{
						meetSkill(player, skill_itemId);
					}
				}
			}
			//将已有技能信息加入图鉴信息
			if(skills!=null && skills.size()>0)
			{
				Iterator<Entry<Integer, Skill>> skill_iterator=skills.entrySet().iterator();
				while(skill_iterator.hasNext()){
					Entry<Integer, Skill> skill_entry=skill_iterator.next();
					Skill skill=skill_entry.getValue();
					int skill_itemid=skill.getItemid();
					String skill_itemid_s=String.valueOf(skill_itemid);
					gotSkill(player, skill_itemid);
				}
			}
			//将已有魂魄信息加入图鉴信息
			if(souls!=null && souls.size()>0)
			{
				Iterator<Integer> soul_iterator=souls.keySet().iterator();
				while(soul_iterator.hasNext())
				{
					int soul_itemid= soul_iterator.next();
					String soul_itemid_s= String.valueOf(soul_itemid);
					//如果已存在该影子对应的船员,则不添加
					if(discipleMap.containsKey(soul_itemid_s) && discipleMap.get(soul_itemid_s)==HAS_GOT)
					{
					}
					else
					{
						meetDisciple(player, soul_itemid);
					}
				}
			}
			if(disciples!=null && disciples.size()>0)
			{
				Iterator<Entry<Integer, Disciple>> disciple_iterator=disciples.entrySet().iterator();
				while(disciple_iterator.hasNext()){
					Entry<Integer, Disciple> disciple_entry=disciple_iterator.next();
					Disciple disciple=disciple_entry.getValue();
					int disciple_itemid=disciple.getItemid();
					gotDisciple(player, disciple_itemid);
				}
			}
			//将已有的装备信息加入图鉴列表
			if(equips!=null && equips.size()>0)
			{
				Iterator<Entry<Integer, Equip>> equip_iterator=equips.entrySet().iterator();
				while(equip_iterator.hasNext()){
					Entry<Integer, Equip> equip_entry=equip_iterator.next();
					Equip equip=equip_entry.getValue();
					int equip_itemid=equip.getItemid();
					String equip_itemid_s=String.valueOf(equip_itemid);
					gotEquip(player, equip_itemid);
				}
			}
		}
	}
	
	public static void gotSoul(Player player,int discipleId){
		Map<String, Integer> discipleMap=Util.jsonToMap(player.getHandbook().getDiscipleInfo());
		if(discipleMap.containsKey(""+discipleId) && discipleMap.get(""+discipleId)==HAS_GOT){
		}
		else{
			meetDisciple(player, discipleId);
		}
	}
	public static void gotSkillBook(Player player ,int skillItemId){
		Map<String, Integer> skillMap=Util.jsonToMap(player.getHandbook().getSkillInfo());
		if(skillMap.containsKey(""+skillItemId) && skillMap.get(""+skillItemId)==HAS_GOT){
			
		}
		else{
			meetSkill(player, skillItemId);
		}
	}
}
