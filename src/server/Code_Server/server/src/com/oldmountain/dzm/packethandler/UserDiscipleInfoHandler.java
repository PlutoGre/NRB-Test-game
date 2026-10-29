package com.oldmountain.dzm.packethandler;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Disciple;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfo;
import com.oldmountain.dzm.message.UserDiscipleInfo.OPUserDiscipleInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.DiscipleUtil;
import com.oldmountain.dzm.util.Util;

/**
 * 获取用户弟子列表信息
 * @author dragon
 *
 */
public class UserDiscipleInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("DiscipleInfo");
	public static void getUserDiscipleInfo(Packet packet,IoSession session){
		OPUserDiscipleInfo params = null;
		OPUserDiscipleInfoRet.Builder builder = OPUserDiscipleInfoRet.newBuilder();
		
		try {
			params = OPUserDiscipleInfo.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			//TODO 版本控制
		}
		
		int playerId = (Integer)session.getAttribute("playerid");
		Player player = ObjectAccessor.getOnlineUser(playerId);

		logger2.info("user disciple info received: {}, {}, {}", new Object[]{playerId , version});
		long current_time = Util.getServerMillTime();
		int time = Util.getServerTime();
		builder.setServertime(time);
		
		//TODO 玩家弟子列表，需要确定的是存在player中还是单独存一个
		HashMap<Integer, Disciple> disciples = player.getDisciples();
		if (params.hasId()) {
			int id = params.getId();
			if (disciples.containsKey(id)) {
				Disciple value = disciples.get(id);				
				builder.addDisciple(DiscipleUtil.buildDiscipleBuilder(value));
			}
		}
		else {
			Iterator<Map.Entry<Integer, Disciple>> iterator = disciples.entrySet().iterator();
			while (iterator.hasNext()) {
				Map.Entry<Integer, Disciple> entry = iterator.next();
				Disciple value = entry.getValue();				
				builder.addDisciple(DiscipleUtil.buildDiscipleBuilder(value));
			}
		}
		player.setLastVerifyTime(time);
		
		player.sendPacket(OP.code.OPCODE_GET_USER_DISCIPLE_INFORET_S_VALUE, builder);
		
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user disciple info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}
}
