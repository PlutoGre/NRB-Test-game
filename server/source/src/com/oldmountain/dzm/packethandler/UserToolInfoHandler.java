package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Bag;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfo;
import com.oldmountain.dzm.message.UserToolInfo.OPUserToolInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class UserToolInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("ToolInfo");
	public static void getUserToolInfo(Packet packet,IoSession session){
		OPUserToolInfo params = null;
		OPUserToolInfoRet.Builder builder = OPUserToolInfoRet.newBuilder();
		
		try {
			params = OPUserToolInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		if (version != 1) {
			return ;
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);
		logger2.info("user tool info handler received: {}, {}, {}", new Object[]{playerId , version});

		long current_time = Util.getServerMillTime();
		//幸运星活动检测
		Util.initLuckyStarActivit(current_time, player);
		int time = Util.getServerTime();
		HashMap<Integer, Bag> userBags = player.getBags();
		
		if(userBags.size() > 0){
		
			OPUserToolInfoRet.ToolInfo.Builder builder2 = OPUserToolInfoRet.ToolInfo.newBuilder();
			
			if (params.hasId()) {
				int id = params.getId();
				if (userBags.containsKey(id)) {
					Bag bag = userBags.get(id);
					builder2.setId(bag.getId());
					builder2.setItemid(bag.getItemid());
					builder2.setCount(bag.getCount());
					
					builder.addTools(builder2);
				}
			}
			else{
				Iterator<Map.Entry<Integer, Bag>> iterator = userBags.entrySet().iterator();
				while (iterator.hasNext()) {
					Map.Entry<Integer, Bag> entry = iterator.next();
					Bag bag = entry.getValue();
					
					builder2.setId(bag.getId());
					builder2.setItemid(bag.getItemid());
					builder2.setCount(bag.getCount());
					
					builder.addTools(builder2);
				}
			}
		}
		
		player.setLastVerifyTime(time);
		
		builder.setServertime(time);
		
		player.sendPacket(OP.code.OPCODE_GET_USER_TOOL_INFORET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user tool info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
	
}
