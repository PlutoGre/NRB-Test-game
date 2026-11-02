package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;

import net.sf.json.JSONObject;

import org.apache.commons.lang.StringUtils;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.Transaction;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.Leagua;
import com.oldmountain.dzm.entity.LeaguaObject;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.BackStageUpdateUserInfo.OPBackStageUpdateUserInfo;
import com.oldmountain.dzm.message.BackStageUpdateUserInfo.OPBackStageUpdateUserInfoRet;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.persist.DataLander;
import com.oldmountain.dzm.services.ActivityServices;
import com.oldmountain.dzm.services.BagToolServices;
import com.oldmountain.dzm.services.LeaguaServices;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.EquipUtil;
import com.oldmountain.dzm.util.HandbookUtil;
import com.oldmountain.dzm.util.SkillUtil;
import com.oldmountain.dzm.util.TitleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class BackStageUpdateUserInfoHandler {
	public static void doUpdateUserInfo(Packet packet,IoSession session){
		OPBackStageUpdateUserInfo params = null;
		OPBackStageUpdateUserInfoRet.Builder builder = OPBackStageUpdateUserInfoRet.newBuilder();
		
		for(int i=0;i<packet.data.array().length;++i){
			packet.data.array()[i]=(byte) (packet.data.array()[i] ^ 0xA5);
		}
		
		try {
			params = OPBackStageUpdateUserInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		String name = params.getName();
		String changeType = params.getChangeType();
		int changeNum1 = params.getChangeNum1();
		int changeNum2 = params.getChangeNum2();
		
		Player player;
		List<Object> listbyName = DZM.getEntityManager().limitQuery("from Player where name = ?", 0, 1, name);
		if (listbyName.size() == 1) {
			player = (Player)listbyName.get(0);
		}
		else {
			player = new Player();
		}
		int userid = player.getId();
		
		try{
			boolean onLineFlag = true;
			boolean doNothing = false;
			player = ObjectAccessor.getOnlineUser(userid);
			//如果该用户已经加载，取内存中的，否则，取数据库中的
			if(player != null){
				onLineFlag = true;
			}else{
				List<Object> list = DZM.getEntityManager().limitQuery("from Player where id = ?", 0, 1, userid);
				if (list.size() == 1) {
					onLineFlag = false;
					player = (Player)list.get(0);
				}
				else {
					player = new Player();
				}
			}
			// 清除玩家的阵容信息
			if ("clearBattle".equals(changeType)) {
				int playerId = player.getId();
				//内存数据落地
				if (onLineFlag) {
					if (player.getIsDirty()) {
						DataLander.landOnePlayer(player);
					}
					ObjectAccessor.delOnlineUser(playerId);
					ObjectAccessor.delBattleArray(playerId);
				}
				
				List<Equip> equips = DZM.getEntityManager().query(Equip.class, "from Equip where playerid = ?", playerId);
				for (int i = 0; i < equips.size(); i++) {
					Equip equip = equips.get(i);
					if (equip.getBattleid() != 0) {
						equip.setBattleid(0);
						DZM.getEntityManager().updateSync(equip);
					}
				}
				
				List<Skill> skills = DZM.getEntityManager().query(Skill.class, "from Skill where playerid = ?", playerId);
				for (int i = 0; i < skills.size(); i++) {
					Skill skill = skills.get(i);
					if (skill.getBattleid() != 0) {
						skill.setBattleid(0);
						DZM.getEntityManager().updateSync(skill);
					}
				}
				
				List<Disciple> disciples = DZM.getEntityManager().query(Disciple.class, "from Disciple where playerid = ?", playerId);
				for (int i = 0; i < disciples.size(); i++) {
					Disciple disciple = disciples.get(i);
					if (disciple.getBattleid() != 0) {
						disciple.setBattleid(0);
						DZM.getEntityManager().updateSync(disciple);
					}
				}
				
				List<Battle> battles = DZM.getEntityManager().limitQuery("from Battle where playerid = ?", 0,8, playerId);
				for (int i = 0; i < battles.size(); i++) {
					Battle battle = battles.get(i);
					battle.setAttack(0);
					battle.setDefaultskill(0);
					battle.setDefaultskillitem(0);
					battle.setDefaultskillquantity(0);
					battle.setDefaultskilltype(0);
					battle.setDefence(0);
					battle.setDiscipleid(0);
					battle.setEquip1(0);
					battle.setEquip1item(0);
					battle.setEquip2(0);
					battle.setEquip2item(0);
					battle.setEquip3(0);
					battle.setEquip3item(0);
					battle.setFates("");
					battle.setFatesstatus("");
					battle.setHealth(0);
					battle.setItemid(0);
					battle.setLevel(0);
					battle.setRp(0);
					battle.setSkill1(0);
					battle.setSkill1item(0);
					battle.setSkill1quantity(0);
					battle.setSkill1type(0);
					battle.setSkill2(0);
					battle.setSkill2item(0);
					battle.setSkill2quantity(0);
					battle.setSkill2type(0);
					
					DZM.getEntityManager().updateSync(battle);
				}
			}
			// 判断修复所有联盟图腾
			else if ("repairAllTotem".equals(changeType)) {
				Iterator<LeaguaObject> it = LeaguaServices.allLeaguaObject.values().iterator();
				while(it.hasNext()){
					LeaguaObject p = it.next();
					Leagua leagua = p.getLeaguaInfo();
					JSONObject totemsInfo = leagua.getLeaguaTotemInfoInJson();
					Iterator<String> keys = totemsInfo.keys();
					while (keys.hasNext()) {
						String key = keys.next();
						JSONObject totemInfo = totemsInfo.getJSONObject(key);
						totemInfo.put("lh", totemInfo.get("mh"));
						if (totemInfo.containsKey("rt")) {
							totemInfo.remove("rt");
						}
						totemsInfo.put(key, totemInfo);
					}
					leagua.setLeaguaTotemInfoFromJson(totemsInfo);
					p.updateLeaguaInfo(leagua);
				}
			}
			else if("gold".equals(changeType)){
				/*
				 * 增加元宝，增加到systemGold
				 * 减少元宝，从systemGold中减掉
				 * 如果systemGold不够减，从rechargeGold中减少
				 * 如果rechargeGold也不够，置为0
				 */

				if(player.getSysgoldcoins()+changeNum1>=0){
					player.setSysgoldcoins(player.getSysgoldcoins()+changeNum1);
				}else if(player.getSysgoldcoins()+changeNum1<0&&player.getSysgoldcoins()+player.getRechargegoldcoins()+changeNum1>=0){
					player.setRechargegoldcoins(player.getRechargegoldcoins()+player.getSysgoldcoins()+changeNum1);
					player.setSysgoldcoins(0);
				}else if(player.getSysgoldcoins()+player.getRechargegoldcoins()+changeNum1<0){
					player.setRechargegoldcoins(0);
					player.setSysgoldcoins(0);
				}else{
					doNothing = true;
				}
				//默认充值到系统增加的元宝
				
			}else if("recharge".equals(changeType)){
				//同时增加rechargeNum,rechargeGold,vipLevel
				//充值不能为负数，也就是充值额度和vip等级是不可以减少的
				if(changeNum1>=0){
					player.setRechargegoldcoins(player.getRechargegoldcoins()+changeNum1*10);
					player.setRechargenum(player.getRechargenum()+changeNum1);
					player.setViplevel(RechargeHandler.getVipLevelByNum(player.getRechargenum()));
					TitleUtil.checkNewTitle(player, 2, player.getViplevel());
					//女神回馈开启判定,活动开启则充值时增加对应额度的鲜花
					long current_time_2 = Util.getServerMillTime();
					int godness_termId=ActivityServices.getTermIdById(ActivityServices.GODDESS_REBATE, Util.getServerMillTime());
					if(godness_termId!=0){
						GoddessRebateHandler.addFlower(packet, session,changeNum1,player , godness_termId,current_time_2);
					}
				}else{
					doNothing = true;
				}
				
			}else if("silver".equals(changeType)){
				//增加银两
				if(player.getSilvercoins()+changeNum1>=0){
					player.setSilvercoins(player.getSilvercoins()+changeNum1);
				}else{
					player.setSilvercoins(0);
				}
				
			}else if("exp".equals(changeType)){
				//增加经验，经验不能减少
				if(changeNum1<0){
					doNothing = true;
				}else{
					Util.getUserLevelByExp(player, changeNum1);
				}
			}else if("member".equals(changeType)){
				//增加阵中某弟子的经验
				Disciple disciple = findDiscipleByID(player, changeNum1);
				if (disciple != null) 
				{
					int addLevel=Util.getlevelByExp(disciple, changeNum2);
					Battle battle=findBattleByID(player, changeNum1);
					if(addLevel>0){
						BattleUtil.resetBattleProperty(battle, disciple,player,null,0);
						player.updateOneBattle(battle);
					}
				} 
				else 
				{
					doNothing = true;
					//阵容中没有该弟子
				}
			}else if("tool".equals(changeType)){
				//增加道具
				if (XMLTemplateService.getToolTemplate(changeNum1) == null) {
					//如果没有该道具
					doNothing = true;
					
				}else{
					BagToolServices.addBagTool(player, changeNum1, changeNum2);
				}
			}else if("hero".equals(changeType)){
				//增加英雄，如果已经有了就不加了
				if(XMLTemplateService.getDiscipleTemplate(changeNum1)==null){
					doNothing = true;
					
				}else{
						
					List<Integer> discipleitems =player.getDiscipleitems();
					boolean alreadyGetHero = false;
					if(discipleitems!=null && discipleitems.size()>0)
					{
						for(Integer item : discipleitems)
						{
							if(item==changeNum1){
								alreadyGetHero = true;
							}
						}
					}
					if(!alreadyGetHero){
						DiscipleUtil.newDisciple(player, changeNum1, 1, 0);
						HandbookUtil.gotDisciple(player, changeNum1);//获得船员时添加图鉴信息
					}
				}
				
			}else if("soul".equals(changeType)){
				if(XMLTemplateService.getDiscipleTemplate(changeNum1)==null){
					doNothing = true;
				}else{
					if(changeNum2<=0){
						doNothing = true;
					}else{
						HashMap<Integer, Soul> userSouls = player.getSouls();
						Soul soul = new Soul();
						if (userSouls.containsKey(changeNum1)) {
							soul = userSouls.get(changeNum1);
							soul.setCount(soul.getCount() + changeNum2);
							player.updateOneSoul(soul);
						}else {
							soul.setItemid(changeNum1);
							soul.setPlayerid(player.getId());
							soul.setCount(changeNum2);
							player.addOneSoul(soul);
							HandbookUtil.gotSoul(player, changeNum1);//获取魂魄时添加图鉴信息
						}
					}
				}
			}else if("skillbook".equals(changeType)){
				//增加技能
				if( XMLTemplateService.getSkillTemplate(changeNum1)==null){
					doNothing = true;
					
				}else{
					for(int j=0 ; j<changeNum2 ; j++)
					{
						SkillUtil.buildSkillBuilder(SkillUtil.newSkill(player, changeNum1, 0));
						HandbookUtil.gotSkill(player, changeNum1);//获得技能时添加图鉴信息
					}
				}
			}else if("equip".equals(changeType)){
				//增加装备
				if( XMLTemplateService.getEquipTemplate(changeNum1)==null){
					doNothing = true;
					
				}else{
					for(int j=0 ; j<changeNum2 ; j++)
					{
						EquipUtil.buildReturn(EquipUtil.newEquip(player, changeNum1));
						HandbookUtil.gotEquip(player, changeNum1);//获得装备时添加图鉴信息
					}
				}
			}else if("power".equals(changeType)){
				//增加体力
				player.increasePowerBySys(changeNum1);
				int power = player.getPowerbychicken() + player.getPowerbytime();
				//如果GM添加的power为负值,并导致总power小于等于0,则将power设为0
				if(power<=0)
				{
					player.setPowerbychicken(0);
					player.setPowerbytime(0);
				}
			}else if("vitality".equals(changeType)){
				// 判断add精力
				player.increaseVitalityBySys(changeNum1);
				int vitality=player.getVitalitybypellet()+player.getVitalitybytime();
				//如果GM添加的vitality为负值,并导致总vitality小于等于0,则将vitality设为0
				if(vitality<=0)
				{
					player.setVitalitybypellet(0);
					player.setVitalitybytime(0);
				}
				
			}else if("team".equals(changeType)){
				// 增加全部上阵弟子经验
				addAllDiscipleExp(player, changeNum1);
			}else if("level".equals(changeType)){
				//使玩家达到指定的等级
				int addExp =getUserAddExp(changeNum1,player);
				Util.getUserLevelByExp(player, addExp);
				
			}else if("tomorrow".equals(changeType)){
			}else if("".equals(changeType)){
			}else if("".equals(changeType)){
			}else if("".equals(changeType)){
			}else if("".equals(changeType)){
			}
			else if ("clearCache".equals(changeType)) {
				if (null != ObjectAccessor.getOnlineUser(changeNum1)) {
					ObjectAccessor.delBattleArray(changeNum1);
					ObjectAccessor.delOnlineUser(changeNum1);
				}
			}
			
			if(!onLineFlag){
				DZM.getEntityManager().updateSync(player);
				
			}
			
			builder.setServertime(Util.getServerTime());
			builder.setUserid(player.getId());
			builder.setName(player.getName());
			builder.setRegistertime(player.getRegistertime());
			builder.setLastlogin(player.getLastlogin());
			builder.setLevel(player.getLevel());
			builder.setSilvercoins(player.getSilvercoins());
			builder.setGoldcoins(player.getGoldCoins());
		
		}catch(Exception e){
			e.printStackTrace();
		}
	
		
		if (version != 1) {
			return;
		}
		
		
		Packet pt = new Packet(OP.code.OPCODE_HEARTBEATRET_S_VALUE, builder.build().toByteArray());
		
		ByteBuffer datasrc = pt.data;
		int size = datasrc.limit();
		ByteBuffer data = ByteBuffer.allocate(size);
		
		datasrc.rewind();
		
		for(int i=0;i<size;++i)
		{
			byte info = (byte) (datasrc.get() ^ 0xA5);
			data.put(info);
		}
		
		data.rewind();
		pt.data = data;
		
		session.write(pt);
		return;
	}
	
	/**
	 * function: 根据传入的阵容ID 获取参战弟子的信息 params: 1. Player 需要修改的玩家 2. int battle的ID
	 * return: Disciple 参战的弟子
	 * */
	private static Disciple findDiscipleByID(Player player, int index) 
	{
		Battle battle=findBattleByID(player, index);
		HashMap<Integer, Disciple> disciples = player.getDisciples();
		Disciple disciple = disciples.get(battle.getDiscipleid());
		return disciple;
	}
	private static Battle findBattleByID(Player player, int index){
		String battleids = player.getBattleids();
		if (StringUtils.isBlank(battleids)) 
		{
			return null;
		}
		String[] ids = battleids.split(",");
		if (ids.length < index) 
		{
			return null;
		}
		int battleid = Integer.parseInt(ids[index - 1]);
		HashMap<Integer, Battle> battlearray = player.getBattlearray();
		Battle battle = battlearray.get(battleid);
		return battle;
	}
	//增加所有上阵弟子经验
	private static void addAllDiscipleExp(Player player, int amount) {
		Iterator<Entry<Integer, Battle>> iterator = player.getBattlearray().entrySet().iterator();
		int index=0;
		while (iterator.hasNext()) 
		{
			if(index>8)
			{
				break;
			}
			Entry<Integer, Battle> entry = iterator.next();
			Disciple disciple = player.getDisciples().get(entry.getValue().getDiscipleid());
			if(disciple==null)
			{
				continue;
			}
			Util.getlevelByExp(disciple, amount);
		}
	}
	
	//根据等级计算还需要多少经验能达到
	private static int getUserAddExp( int lvlEnd , Player player) 
	{
		int levelCount = XMLTemplateService.getUserLevelCount();
		if(lvlEnd>levelCount){
			lvlEnd=levelCount;
		}
		int addExp=XMLTemplateService.getUserLevelExp(lvlEnd-1)-player.getExp();
		return addExp;
	}
}
