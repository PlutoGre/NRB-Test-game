package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.Heartbeat.OPHeartbeat;
import com.oldmountain.dzm.message.Heartbeat.OPHeartbeatRet;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

public class HeartBeatHandler {
	public static void doHeartBeat(Packet packet,IoSession session){
		OPHeartbeat params = null;
		OPHeartbeatRet.Builder builder = OPHeartbeatRet.newBuilder();
		
		try {
			params = OPHeartbeat.parseFrom(packet.data.array());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
			return;
		}
		
		int version = params.getVersion();
		
		if (version != 1) {
			return;
		}
		
		int serverTime = Util.getServerTime();
		
		builder.setServertime(serverTime);
		if (serverTime < DZM.serverCloseTime + DZM.closeNeedTime * 60) {
			int stopTime = DZM.serverCloseTime - serverTime;
			stopTime = stopTime < 0?0:stopTime;
			builder.setServerStopTime(stopTime);
			builder.setServerReopenTime(DZM.closeNeedTime);
		}
		
		Packet pt = new Packet(OP.code.OPCODE_HEARTBEATRET_S_VALUE, builder.build().toByteArray());
		session.write(pt);
		return;
	}
}
