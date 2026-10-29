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
import com.oldmountain.dzm.entity.Equip;
import com.oldmountain.dzm.entity.FateXmlTemplate;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChangeEquip.OPChangeEquip;
import com.oldmountain.dzm.message.ChangeEquip.OPChangeEquipRet;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ChangeEquipHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("EquipInfo");
	public static void changeEquip(Packet packet,IoSession session){
		OPChangeEquip params = null;
		OPChangeEquipRet.Builder builder = OPChangeEquipRet.newBuilder();
		OPUserBattleArrayRet.BattleArrayInfo.Builder battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPChangeEquip.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int positionId = params.getBattleposition();
		int equipId = params.getId();
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("change equip handler received: {}, {}, {}", new Object[]{id , equipId , positionId});
		long current_time = Util.getServerMillTime();
		
		List<String> battleIdsList = Arrays.asList(player.getBattleids().split(","));
		
		if (positionId < 0 || positionId > battleIdsList.size() - 1) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_EQUIPRET_S_VALUE);
			errorBuilder.setErrormsg("position over length");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change equip handler: {}, {}, {}", new Object[]{id});
			return;
		}
		
		int battleId = Integer.parseInt(battleIdsList.get(positionId));
		HashMap<Integer, Battle> userBattles = player.getBattlearray();
		HashMap<Integer, Equip> userEquips = player.getEquips();
		
		if (!userBattles.containsKey(battleId) || !userEquips.containsKey(equipId)) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_EQUIPRET_S_VALUE);
			errorBuilder.setErrormsg("battle or equip not exist!");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change equip handler: {}", new Object[]{id});
			return;
		}
		
		Battle battle = userBattles.get(battleId);
		Equip equip = userEquips.get(equipId);
		
		/**********************处理装备之前的阵容属性 begin*************************/
		if (equip.getBattleid() != 0) {//此装备被另外的阵容装备了，需要将另外阵容的属性重新计算一下
			if (battleId == equip.getBattleid()) {
				errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_EQUIPRET_S_VALUE);
				errorBuilder.setErrormsg("this equip is already in this position");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				logger2.info("Error Reporter send: change equip handler: {}", new Object[]{id});
				return;
			}
			if (userBattles.containsKey(equip.getBattleid()) && battleIdsList.contains(String.valueOf(equip.getBattleid()))) {
				int index = battleIdsList.indexOf(String.valueOf(equip.getBattleid()));
				Battle preBattle = userBattles.get(equip.getBattleid());
				switch(equip.getType()){
					case 1:
						preBattle.setEquip1(0);
						preBattle.setEquip1item(0);
						break;
					case 2:
						preBattle.setEquip2(0);
						preBattle.setEquip2item(0);
						break;
					case 3:
						preBattle.setEquip3(0);
						preBattle.setEquip3item(0);
						break;
				}
				Disciple preDisciple = player.getDisciples().get(preBattle.getDiscipleid());
				BattleUtil.resetBattleProperty(preBattle, preDisciple, player, new ArrayList<Integer>(), 2);
				BattleUtil.buildBattleBuilder(battleBuilder, preBattle, index);
				builder.addBattle(battleBuilder);
				player.updateOneBattle(preBattle);
				ObjectAccessor.getBattleArray(playerId).getBattlearrays().set(index, preBattle);
			}
		}
		/**********************处理装备之前的阵容属性 end*************************/
		
		/**********************处理要装备的阵容属性 begin*************************/
		battleBuilder = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
		int preEquipId = 0;
		switch(equip.getType()){
			case 1:
				preEquipId = battle.getEquip1();
				break;
			case 2:
				preEquipId = battle.getEquip2();
				break;
			case 3:
				preEquipId = battle.getEquip3();
				break;
		}
		
		if(preEquipId != 0){//该阵容之前已经装备了该类型的装备
			if (userEquips.containsKey(preEquipId)) {
				Equip preEquip = userEquips.get(preEquipId);
				preEquip.setBattleid(0);
				player.updateOneEquip(preEquip);
			}
		}
		
		switch(equip.getType()){
			case 1:
				battle.setEquip1(equipId);
				battle.setEquip1item(equip.getItemid());
				break;
			case 2:
				battle.setEquip2(equipId);
				battle.setEquip2item(equip.getItemid());
				break;
			case 3:
				battle.setEquip3(equipId);
				battle.setEquip3item(equip.getItemid());
				break;
		}
	
		equip.setBattleid(battleId);
		player.updateOneEquip(equip);
		//检查缘的情况 
		Disciple disciple = player.getDisciples().get(battle.getDiscipleid());
		BattleUtil.resetBattleProperty(battle, disciple, player, new ArrayList<Integer>(), 2);
		player.updateOneBattle(battle);
		ObjectAccessor.getBattleArray(playerId).getBattlearrays().set(positionId, battle);
		BattleUtil.buildBattleBuilder(battleBuilder, battle, positionId);
		
		builder.addBattle(battleBuilder);
		/**********************处理要装备的阵容属性end*************************/
		
		player.setLastVerifyTime(Util.getServerTime());	
		
		builder.setServertime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_CHANGE_EQUIPRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("change equip handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}
}
