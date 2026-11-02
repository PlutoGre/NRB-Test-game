package com.oldmountain.dzm;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import org.slf4j.Logger;
import org.apache.commons.configuration.XMLConfiguration;
import org.apache.mina.common.IoSession;
import org.slf4j.LoggerFactory;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.Heartbeat.OPHeartbeatRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.persist.DataLander;
import com.oldmountain.dzm.util.Util;

public class ServerShutDown {
	private static final Logger logger = LoggerFactory.getLogger(DZM.class.getName());
	
	public ServerShutDown(){
		doShutDownWork();
	}
	
	private void doShutDownWork(){
		Runtime.getRuntime().addShutdownHook(new Thread(){
			public void run(){
				XMLConfiguration conf = DZM.getConf();
				logger.info("start report to online user......");
				HashMap<Integer, IoSession> onlineSessions = ObjectAccessor.getOnlineSession();
				Iterator<Map.Entry<Integer, IoSession>> iterator = onlineSessions.entrySet().iterator();
				int currentTime = Util.getServerTime();
				while (iterator.hasNext()) {
					Map.Entry<Integer, IoSession> entry = iterator.next();
					IoSession session = entry.getValue();
					logger.info("------report to user:" + session.getAttribute("playerid") + "-------");
					OPHeartbeatRet.Builder builder = OPHeartbeatRet.newBuilder();
					builder.setServertime(currentTime);
					builder.setServerStopTime(0);
					builder.setServerReopenTime(DZM.closeNeedTime);
					Packet pt = new Packet(OP.code.OPCODE_HEARTBEATRET_S_VALUE, builder.build().toByteArray());
					session.write(pt);
				}
				logger.info("report to online user finished");
				
				logger.info("start unbinding mina......");
				DZM.getAcceptor().unbind(new InetSocketAddress(conf.getString("connection.address"), conf.getInt("connection.port")));
				logger.info("unbind mina complete");
				
				try
				{
					Thread.sleep(10000);
				}catch(InterruptedException e)
				{
					logger.info(e.getMessage());
				}
				
				logger.info("start shutting down......");
				DataLander.landingImmediately();
				logger.info("shutting down complete");
			}
		});
	}
}
