package com.oldmountain.dzm.net;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.ServerDisconnect.OPServerDisConnectRet;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.net.packet.PacketHandler;
import com.oldmountain.dzm.util.Util;

import org.slf4j.LoggerFactory;


public final class SessionHandler extends IoHandlerAdapter
{

	private static final Logger logger = Logger.getLogger(SessionHandler.class.getName());
    private static final org.slf4j.Logger log = LoggerFactory.getLogger("PlayerLoginInfo");
    private static final org.slf4j.Logger logoutinfo_log = LoggerFactory.getLogger("PlayerLogoutInfo");

	@Override
	public void exceptionCaught(IoSession session, Throwable throwable)throws Exception
    {
        int playerId=0;
        try
        {
            playerId=(Integer)session.getAttribute("playerid");
            Player player=ObjectAccessor.getOnlineUser(playerId);
            log.info("player logout exceptionCaught method|#playerId:"+playerId+"#client ip:"+session.getRemoteAddress()+"#Current playerCount:"+ObjectAccessor.players.size());
            player.getSession().close();
            log.info("player logout exceptionCaught method|close #playerId:"+playerId+"# session info");
            logoutinfo_log.info("player logout info: {}, {}, {}, {}, {}, {},{},{}", new Object[]{playerId , player.getRegistertime() , player.getLevel() , player.getViplevel() , player.getExp() , Util.getServerTime(),player.getPlatform(),player.getDeviceId()});
            ObjectAccessor.removeOnlineSession(playerId);
        }
        catch (Exception e)
        {
            log.info("player logout exceptionCaught method|close session exceptionCaught :" + e.getMessage());
        }
        
        OPServerDisConnectRet.Builder builder = OPServerDisConnectRet.newBuilder();
        builder.setDisConnectID(1);
        builder.setDisConnectMsg("session closed exception");
        Packet pt = new Packet(OP.code.OPCODE_SERVER_DISCONNECT_S_VALUE, builder.build().toByteArray());
		session.write(pt);

//		logger.log(Level.SEVERE, "Exception occurred, closing session.",throwable);
		session.close();
	}

	@Override
	public void messageReceived(IoSession session, Object message)throws Exception {
		Packet msg = (Packet) message;
		msg.session = session;
		PacketHandler.RecvPacket(msg);
	}

	@Override
	public void sessionClosed(IoSession session) throws Exception
    {
        if(session == null) {
			return;
		}
		if(session.getAttribute("playerid") == null) {
			return;
		}
		int playerId = (Integer)session.getAttribute("playerid");
		Player player=ObjectAccessor.getOnlineUser(playerId);
        log.info("player logout sessionClosed method|playerId:"+playerId+"#client ip:"+session.getRemoteAddress()+"#Current playerCount:"+ObjectAccessor.players.size());
        if (playerId != 0 )
        {
            ObjectAccessor.removeOnlineSession(playerId);
		}
        
        OPServerDisConnectRet.Builder builder = OPServerDisConnectRet.newBuilder();
        builder.setDisConnectID(1);
        builder.setDisConnectMsg("session closed exception");
        Packet pt = new Packet(OP.code.OPCODE_SERVER_DISCONNECT_S_VALUE, builder.build().toByteArray());
		session.write(pt);
		logoutinfo_log.info("player logout info: {}, {}, {}, {}, {}, {},{},{}", new Object[]{playerId , player.getRegistertime() , player.getLevel() , player.getViplevel() , player.getExp() , Util.getServerTime(),player.getPlatform(),player.getDeviceId()});
        log.info("plad|cloyer logout sessionClosed methose this session playerId:"+playerId);
        session.close();
	}

	@Override
	public void sessionOpened(IoSession session) throws Exception
    {
        logger.info("session opened#client ip:"+session.getRemoteAddress()+"#Current playerCount:"+ObjectAccessor.players.size());
	}

}
