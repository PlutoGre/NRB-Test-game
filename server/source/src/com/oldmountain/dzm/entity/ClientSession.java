package com.oldmountain.dzm.entity;

import com.oldmountain.dzm.net.ObjectAccessor;
import org.apache.mina.common.IoSession;

import com.google.protobuf.Message.Builder;
import com.oldmountain.dzm.net.packet.Packet;
import org.slf4j.LoggerFactory;

public class ClientSession
{
	private IoSession session;
    private static final org.slf4j.Logger log = LoggerFactory.getLogger("PlayerLoginInfo");
	public void setSession(IoSession s)
	{
        int playerId=(Integer)s.getAttribute("playerid");
        //关闭上一次该玩家session,是否存在该session 初期版本调试打印log add by dylan
        if(ObjectAccessor.players.containsKey(playerId))
        {
            try
            {
            	IoSession oldSession = ObjectAccessor.players.get(playerId).getSession();
            	if (!s.getRemoteAddress().equals(oldSession.getRemoteAddress())) {
            		oldSession.close();
            		log.info("player login setSession method|close last session,playerId:"+playerId);
				}
            }
            catch (Exception ignored)
            {
                log.info("player login setSession method|close last session Exception:"+ignored.getMessage());
            }
        }
		session  = s;
		ObjectAccessor.addOnlineSession(playerId, s);
        //add dy dylan 版本调试 打印登陆 登出信息
        log.info("player login setSession method|#playerId:"+playerId+"#client ip:"+session.getRemoteAddress()+"#current online player count:"+ (ObjectAccessor.players.size()+1));
		return;
	}
	public IoSession getSession()
	{
		return session;
	}
	 
	public void sendPacket(int opcode, Builder builder) {
		Packet pt = new Packet(opcode, builder.build().toByteArray());
		session.write(pt);
	}

}
