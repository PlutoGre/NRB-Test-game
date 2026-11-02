package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Soul;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfo;
import com.oldmountain.dzm.message.UserSoulInfo.OPUserSoulInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class UserSoulInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("UserBasicInfo");
	public static void getUserSoulInfo(Packet packet,IoSession session){
		OPUserSoulInfo params = null;
		OPUserSoulInfoRet.Builder builder = OPUserSoulInfoRet.newBuilder();
		OPUserSoulInfoRet.SoulInfo.Builder builder2 = OPUserSoulInfoRet.SoulInfo.newBuilder();
		
		try {
			params = OPUserSoulInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version=params.getVersion();
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger2.info("user soul info handler received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		HashMap<Integer, Soul> userSouls = player.getSouls();
		if (userSouls.size() > 0) {
			if (params.hasId()) {
				int id = params.getId();
				Soul soul = userSouls.get(id);
				if (soul.getCount() > 0) {
					builder2.setId(id);
					builder2.setItemid(soul.getItemid());
					builder2.setCount(soul.getCount());
					
					builder.addSoul(builder2);
				}
				
			}
			else {
				Iterator<Map.Entry<Integer, Soul>> iterator = userSouls.entrySet().iterator();
				while (iterator.hasNext()) {
					Map.Entry<Integer, Soul> entry = iterator.next();
					Soul value = entry.getValue();
					if (value.getCount() > 0) {
						builder2.setId(value.getId());
						builder2.setItemid(value.getItemid());
						builder2.setCount(value.getCount());
						
						builder.addSoul(builder2);
					}
				}
			}
		}
		player.setLastVerifyTime(time);
		builder.setServertime(Util.getServerTime());
		
		player.sendPacket(OP.code.OPCODE_GET_USER_SOUL_INFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user soul info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
}
