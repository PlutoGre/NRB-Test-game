package com.oldmountain.dzm.packethandler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Battle;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ErrorReporter.OPErrorReporter;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArray;
import com.oldmountain.dzm.message.UserBattleArray.OPUserBattleArrayRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.BattleUtil;
import com.oldmountain.dzm.util.Util;

public class UserBattleArrayHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("BattleArrayInfo");
	public static void getUserBattleArray(Packet packet,IoSession session){
		
		OPUserBattleArray params = null;
		OPUserBattleArrayRet.Builder builder = OPUserBattleArrayRet.newBuilder();
		
		try {
			params = OPUserBattleArray.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		if (version != 1) {
			OPErrorReporter.Builder errBuilder = OPErrorReporter.newBuilder();
			errBuilder.setErrorid(OP.code.OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S_VALUE);
			errBuilder.setErrormsg("version error");
			player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, errBuilder);
		}
		
		logger2.info("user battle array handler received: {}, {}, {}", new Object[]{playerId , version});

		long current_time = Util.getServerMillTime();
		
		HashMap<Integer, Battle> userBattleArray = player.getBattlearray();
		List<Battle> positionBattles = new ArrayList<Battle>();
		//根据玩家的等级来确定阵容个数
		int limit = BattleUtil.getBattleCountByLevel(player.getLevel());
		String ids = player.getBattleids();
		String[] idsArray = ids.split(",");
		for (int i = 0; i < limit; i++) {
			int battleid = Integer.parseInt(idsArray[i]);
			positionBattles.add(userBattleArray.get(battleid));
		}
		
		for (int i = 0; i < positionBattles.size(); i++) {
			Battle value = positionBattles.get(i);
			OPUserBattleArrayRet.BattleArrayInfo.Builder builder2 = OPUserBattleArrayRet.BattleArrayInfo.newBuilder();
			builder2.setId(value.getId());
			builder2.setDiscipleId(value.getDiscipleid());
			builder2.setAttack(value.getAttack());
			builder2.setDefence(value.getDefence());
			builder2.setHealth(value.getHealth());
			builder2.setRp(value.getRp());
			builder2.setPosition(i);
			builder2.setSkill1(value.getSkill1());
			builder2.setSkill2(value.getSkill2());
			builder2.setEquip1(value.getEquip1());
			builder2.setEquip2(value.getEquip2());
			builder2.setEquip3(value.getEquip3());
			String fates = value.getFates();
			if(!fates.equals("") && fates != null){
				String fatestatus = value.getFatesstatus();
				String[] fatesArray = fates.split(",");
				String[] fatestatusArray = fatestatus.split(",");
				for (int j = 0; j < fatesArray.length; j++) {
					OPUserBattleArrayRet.FateInfo.Builder fateBuilder = OPUserBattleArrayRet.FateInfo.newBuilder();
					fateBuilder.setFateId(Integer.parseInt(fatesArray[j]));
					fateBuilder.setStatus(Integer.parseInt(fatestatusArray[j]));
					builder2.addFates(fateBuilder);
				}
			}
			builder.addBattle(builder2);
		}
		
		player.setLastVerifyTime(Util.getServerTime());
		
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user battle array handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
}
