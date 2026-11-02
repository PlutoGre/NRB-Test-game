package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.DiscipleXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChangeDisciple.OPChangeDisciple;
import com.oldmountain.dzm.message.ChangeDisciple.OPChangeDiscipleRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ChangeDiscipleHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("BattleArrayInfo");
	public static void changeDiscipleHandler(Packet packet,IoSession session){
		OPChangeDisciple params = null;
		OPChangeDiscipleRet.Builder builder = OPChangeDiscipleRet.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPChangeDisciple.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return ;
		}
		int positionId = params.getPosition();
		int discipleId = params.getDiscipleid();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int playerid=player.getId();
		logger2.info("change disciple handler received: {}, {}, {}", new Object[]{playerid , positionId , discipleId});

		long current_time = Util.getServerMillTime();
		if (positionId < 0) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE);
			errorBuilder.setErrormsg("position error!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change disciple handler: {}, {}, {}", new Object[]{playerid});
			return;
		}
		
		Disciple disciple = player.getDisciples().get(discipleId);
		if (disciple == null) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE);
			errorBuilder.setErrormsg("disciple not exist!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change disciple handler: {}, {}, {}", new Object[]{playerid});
			return;
		}
		if (disciple.getBattleid() != 0) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE);
			errorBuilder.setErrormsg("already in battle!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change disciple handler: {}, {}, {}", new Object[]{playerid});
			return;
		}
		int countLimit = BattleUtil.getBattleCountByLevel(player.getLevel());
		if (positionId > countLimit - 1) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE);
			errorBuilder.setErrormsg("over limit");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change disciple handler: {}, {}, {}", new Object[]{playerid});
			return;
		}
		
		String[] battles = player.getBattleids().split(",");
		if (positionId > battles.length -1 ) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE);
			errorBuilder.setErrormsg("position too large!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change disciple handler: {}, {}, {}", new Object[]{playerid});
			return;
		}
		int battleId = Integer.parseInt(battles[positionId]);
		ArrayList<Integer> battleDiscipleItems = new ArrayList<Integer>();
		for (int i = 0; i < battles.length; i++) {
			int tId = Integer.parseInt(battles[i]);
			Battle tBattle = player.getBattlearray().get(tId);
			battleDiscipleItems.add(tBattle.getItemid());
		}
		
		DiscipleXmlTemplate discipleInfo = XMLTemplateService.getDiscipleTemplate(disciple.getItemid());
		String fates = discipleInfo.getFates();
		String[] fatesArray = fates.split(",");
		String fateStatus = "0";
		for (int i = 1; i < fatesArray.length; i++) {
			fateStatus += ",0"; 
		}
		
		HashMap<Integer, Skill>userSkills = player.getSkills();
		int sameSkill=0;
		Battle battle = player.getBattlearray().get(battleId);
		if (battle.getDiscipleid() == 0) {//该位置之前没有上阵弟子
			if (battleDiscipleItems.indexOf(0) != positionId) {
				errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE);
				errorBuilder.setErrormsg("before battle not have disciple");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				logger2.info("Error Reporter send: change disciple handler: {}, {}, {}", new Object[]{playerid});
				return;
			}
			
		}
		else {//之前有上阵弟子
			// 添加天赋技能重复判断
			Skill skill=userSkills.get(disciple.getSkill1());
			if(skill.getItemid()==battle.getSkill1item())
			{
				sameSkill=1;
			}  
			else if(skill.getItemid()==battle.getSkill2item())
			{
				sameSkill=2;
			}
			int preDiscipleId = battle.getDiscipleid();
			Disciple preDisciple = player.getDisciples().get(preDiscipleId);
			preDisciple.setBattleid(0);
			player.updateOneDisciple(preDisciple);
			int preDefaultSkillId = battle.getDefaultskill();
			Skill preDefaultSkill = player.getSkills().get(preDefaultSkillId);
			preDefaultSkill.setBattleid(0);
			player.updateOneSkill(preDefaultSkill);
		}
		
		
		//设置阵容的基本属性
		battle.setDiscipleid(discipleId);
		battle.setItemid(disciple.getItemid());
		battle.setLevel(disciple.getLevel());
		Skill defaultSkill = userSkills.get(disciple.getSkill1());
		battle.setDefaultskill(defaultSkill.getId());
		battle.setDefaultskilltype(defaultSkill.getType());
		battle.setDefaultskillitem(defaultSkill.getItemid());
		battle.setDefaultskillquantity(defaultSkill.getQuantity());
		defaultSkill.setBattleid(battleId);
		//新船员天赋技能与阵容中第一个非天赋技能重复
		if(sameSkill==1){
			Skill skill1=userSkills.get(battle.getSkill1());
			skill1.setBattleid(0);
			battle.setSkill1(0);
			battle.setSkill1item(0);
			battle.setSkill1quantity(0);
			battle.setSkill1type(0);
		}
		else if(sameSkill==2){
			Skill skill2=userSkills.get(battle.getSkill2());
			skill2.setBattleid(0);
			battle.setSkill2(0);
			battle.setSkill2item(0);
			battle.setSkill2quantity(0);
			battle.setSkill2type(0);
		}
		//设置阵容的缘分信息
		battle.setFates(fates);
		battle.setFatesstatus(fateStatus);
		defaultSkill.setBattleid(battleId);
		
		player.updateOneSkill(defaultSkill);
		disciple.setBattleid(battleId);
		player.updateOneDisciple(disciple);
		
		battleDiscipleItems.set(positionId, disciple.getItemid());
		
		
		//检查所有阵容里面人物的缘
		HashMap<Integer, Battle>userBattles = player.getBattlearray();
		BattleArrayObject bObject = BattleUtil.getUserBattleArrayObject(player);
		ArrayList<Battle> battleArrays = bObject.getBattlearrays();
		
		for (int i = 0; i < battles.length; i++) {
			int id = Integer.parseInt(battles[i]);
			Battle battle2 = userBattles.get(id);
			if (battle2.getDiscipleid() == 0) {
				continue;
			}
			
			if (i == positionId) {//是刚放上去的位置，需要判断技能，装备和人物触发的缘
				BattleUtil.resetBattleProperty(battle2, disciple, player, battleDiscipleItems, 1);
				OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
				BattleUtil.buildBattleBuilder(battleBuilder, battle2, i);
				builder.addBattle(battleBuilder);
			}
			else{//其他的位置只需要判断由人物触发的缘
				Disciple disciple2 = player.getDisciples().get(battle2.getDiscipleid());
				BattleUtil.resetBattleProperty(battle2, disciple2, player, battleDiscipleItems, 4);
				OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
				BattleUtil.buildBattleBuilder(battleBuilder, battle2, i);
				builder.addBattle(battleBuilder);
			}
			
			player.updateOneBattle(battle2);
			battleArrays.set(i, battle2);
		}
		BattleUtil.getUserBattleArrayObject(player).setBattlearrays(battleArrays);
		player.setLastVerifyTime(Util.getServerTime());
		builder.setServertime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_CHANGE_DISCIPLERET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("change disciple handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}
}
