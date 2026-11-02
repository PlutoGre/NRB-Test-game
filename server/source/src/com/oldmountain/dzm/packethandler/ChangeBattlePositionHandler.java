package com.oldmountain.dzm.packethandler;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ChangeBattlePosition.OPChangeBattlePosition;
import com.oldmountain.dzm.message.ChangeBattlePosition.OPChangeBattlePositionRet;
import com.oldmountain.dzm.message.ChangeBattlePosition.PositionId;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class ChangeBattlePositionHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("BattleArrayInfo");
	public static void changeBattlePosition(Packet packet,IoSession session){
		OPChangeBattlePosition params = null;
		OPChangeBattlePositionRet.Builder builder = OPChangeBattlePositionRet.newBuilder();
		PositionId.Builder positionIdBuilder = PositionId.newBuilder();
		OPErrorReporter.Builder errorBuilder = OPErrorReporter.newBuilder();
		
		try {
			params = OPChangeBattlePosition.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int count = params.getPositionsCount();
		if (count <= 0) {
			return;
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		int id=player.getId();
		logger2.info("change battle position handler received: {}, {}, {}", new Object[]{id});
		long current_time = Util.getServerMillTime();
		String[] battles = player.getBattleids().split(",");
		if (count != battles.length) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_BATTLE_POSITIONRET_S_VALUE);
			errorBuilder.setErrormsg("count error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change battle position handler: {}", new Object[]{id});
			return;
		}
		
		List<Integer> battleList = new ArrayList<Integer>();
		
		for (int i = 0; i < battles.length; i++) {
			battleList.add(Integer.parseInt(battles[i]));
		}
		HashMap<Integer, Integer> positions = new HashMap<Integer, Integer>();
		List<PositionId> positionIds = params.getPositionsList();
		for (int i = 0; i < positionIds.size(); i++) {
			PositionId positionId = positionIds.get(i);
			positions.put(positionId.getPosition(), positionId.getId());
		}
		
		List<Integer> newBattleList = new ArrayList<Integer>();
		for (int i = 0; i < battles.length; i++) {
			if (positions.get(i) == null) {
				errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_BATTLE_POSITIONRET_S_VALUE);
				errorBuilder.setErrormsg("no position" + i + "given");
				player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
				logger2.info("Error Reporter send: change battle position handler: {}", new Object[]{id});
				return;
			}
			
			newBattleList.add(positions.get(i));
		}
		
		battleList.retainAll(newBattleList);
		if (battleList.size() != newBattleList.size()) {
			errorBuilder.setErrorid(OP.code.OPCODE_CHANGE_BATTLE_POSITIONRET_S_VALUE);
			errorBuilder.setErrormsg("ids error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errorBuilder);
			logger2.info("Error Reporter send: change battle position handler: {}", new Object[]{id});
			return;
		}
		
		String newBattle = "";
		positionIdBuilder.setPosition(0);
		positionIdBuilder.setId(newBattleList.get(0));
		builder.addPositions(positionIdBuilder);
		BattleArrayObject battleArrayObject = new BattleArrayObject();
		battleArrayObject.setMomentum(player.getMomentum());
		battleArrayObject.setPlayerid(playerId);
		ArrayList<Battle> battleArrayList = new ArrayList<Battle>();
		for (int i = 0; i < newBattleList.size(); i++) {
			if(i==0){
				newBattle = newBattle + newBattleList.get(i);
			}
			else{
				newBattle = newBattle + "," + newBattleList.get(i);
			}
			battleArrayList.add(player.getBattlearray().get(newBattleList.get(i)));
			positionIdBuilder.setPosition(i);
			positionIdBuilder.setId(newBattleList.get(i));
			builder.addPositions(positionIdBuilder);
		}
		battleArrayObject.setBattlearrays(battleArrayList);
		ObjectAccessor.addBattleArray(battleArrayObject);
		player.setBattleids(newBattle);
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setServertime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_CHANGE_BATTLE_POSITIONRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("change battle position handler ret send: {}, {}", new Object[]{id ,taken_time});
	}
}
