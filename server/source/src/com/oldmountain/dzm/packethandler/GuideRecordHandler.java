package com.oldmountain.dzm.packethandler;

import java.util.Map;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.GuideBuyTool;
import com.oldmountain.dzm.message.GuideRecord;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class GuideRecordHandler {
	private static final Logger log = LoggerFactory.getLogger(GuideBuyToolHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("CdkeyReward");
	public static void recordType(Packet packet, IoSession session) {
		GuideRecord.OPGuideRecord params = null;
		GuideRecord.OPGuideRecordRet.Builder builder = GuideRecord.OPGuideRecordRet.newBuilder();
		try{
			params = GuideRecord.OPGuideRecord .parseFrom(packet.data.array());
		}catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		int version = params.getVersion();
		String type = params.getType();
		
		int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        long current_time = Util.getServerMillTime();
        int playerid=player.getId();
        /******************************************************************/
        Map<String,Integer> firstGain = Util.jsonToMap(player.getFirstgain());
        if(!firstGain.containsKey(type))
        {
        	player.setFirstGainByType(type);
        }
        builder.setServertime(Util.getServerTime());
        /******************************************************************/
        player.sendPacket(OP.code.OPCODE_GUIDE_RECORDRET_S_VALUE, builder);
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("user battle handler ret send: {}, {}, {}", new Object[]{playerid ,taken_time});
	}

}
