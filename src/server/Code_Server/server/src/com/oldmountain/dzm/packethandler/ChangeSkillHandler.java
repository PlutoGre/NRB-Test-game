package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.FateXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Skill;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChangeSkill.OPChangeSkill;
import com.oldmountain.dzm.message.ChangeSkill.OPChangeSkillRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ChangeSkillHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("SkillInfo");
	public static void changeSkill(Packet packet,IoSession session){
		OPChangeSkill params = null;
		OPChangeSkillRet.Builder builder = OPChangeSkillRet.newBuilder();
		OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPChangeSkill.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int skillId = params.getNewskillid();
		int battlePostion = params.getBattleposition();
		int skillPosition = params.getSkillposition();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("change skill handler received: {}, {}, {}", new Object[]{id , skillId , battlePostion , skillPosition});
		long current_time = Util.getServerMillTime();
		List<String> battleIdsList = Arrays.asList(player.getBattleids().split(","));
		if (battlePostion < 0 || battlePostion > battleIdsList.size() - 1) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_SKILLRET_S_VALUE);
			errorBuilder.setErrormsg("position over length");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		int battleId = Integer.parseInt(battleIdsList.get(battlePostion));
		
		HashMap<Integer, Battle> userBattles = player.getBattlearray();
		HashMap<Integer, Skill> userSkills = player.getSkills();
		
		if (!userBattles.containsKey(battleId) || !userSkills.containsKey(skillId)) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_SKILLRET_S_VALUE);
			errorBuilder.setErrormsg("params error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		Battle battle = userBattles.get(battleId);
		Skill skill = userSkills.get(skillId);
		
		if (skill.getIsdefaultskill() == 1) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_SKILLRET_S_VALUE);
			errorBuilder.setErrormsg("skill is a default skill");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change skill handler: {}, {}, {}", new Object[]{id});
			return;
		}
		/**********************处理装备之前的阵容属性 begin*************************/
		if (skill.getBattleid() != 0) {//技能已被其他位置的阵容装备了，需要处理另外阵容的属性
			if (userBattles.containsKey(skill.getBattleid())) {
				int index = battleIdsList.indexOf(String.valueOf(skill.getBattleid()));
				Battle preBattle = userBattles.get(skill.getBattleid());
				Disciple preDisciple = player.getDisciples().get(preBattle.getDiscipleid());
				
				if (preBattle.getSkill1() == skillId) {
					preBattle.setSkill1(0);
					preBattle.setSkill1item(0);
					preBattle.setSkill1quantity(0);
					preBattle.setSkill1type(0);
				}
				else if (preBattle.getSkill2() == skillId) {
					preBattle.setSkill2(0);
					preBattle.setSkill2item(0);
					preBattle.setSkill2quantity(0);
					preBattle.setSkill2type(0);
				}
				
				//检查缘的情况
				BattleUtil.resetBattleProperty(preBattle, preDisciple, player, new ArrayList<Integer>(), 3);
				BattleUtil.buildBattleBuilder(battleBuilder, preBattle, index);
				builder.addBattle(battleBuilder);
				player.updateOneBattle(preBattle);
				ObjectAccessor.getBattleArray(playerId).getBattlearrays().set(index, preBattle);
			}
		}
		/**********************处理装备之前的阵容属性 end*************************/
		
		/**********************处理要装备的阵容属性 begin*************************/
		
		battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
		int preSkillId = 0;
		if (skillPosition == 1) {
			preSkillId = battle.getSkill1();
			battle.setSkill1(skillId);
			battle.setSkill1item(skill.getItemid());
			battle.setSkill1quantity(skill.getQuantity());
			battle.setSkill1type(skill.getType());
		}
		else {
			preSkillId = battle.getSkill2();
			battle.setSkill2(skillId);
			battle.setSkill2item(skill.getItemid());
			battle.setSkill2quantity(skill.getQuantity());
			battle.setSkill2type(skill.getType());
		}
		
		Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
		
		if (preSkillId != 0) {//弟子的该位置已经有技能了，属于替换
			if (userSkills.containsKey(preSkillId)) {
				Skill preSkill = userSkills.get(preSkillId);
				preSkill.setBattleid(0);
				player.updateOneSkill(preSkill);
			}
		}
		skill.setBattleid(battleId);
		player.updateOneSkill(skill);
		//检查缘
		BattleUtil.resetBattleProperty(battle, disciple, player, new ArrayList<Integer>(), 3);
		BattleUtil.buildBattleBuilder(battleBuilder, battle, battlePostion);
		builder.addBattle(battleBuilder);
		player.updateOneBattle(battle);
		ObjectAccessor.getBattleArray(playerId).getBattlearrays().set(battlePostion, battle);
		/**********************处理要装备的阵容属性end*************************/
		
		builder.setServertime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_CHANGE_SKILLRET_S_VALUE, builder);
		player.setLastVerifyTime(Util.getServerTime());
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("change skill handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}
