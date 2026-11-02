package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.SendFriendMessage.OPSendMessage;
import com.oldmountain.dzm.message.SendFriendMessage.OPSendMessageRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.services.SysMsgInfoServices; 


/**
 * 留言包
 * 1， 解析数据包
 * 2， 保存留言数据
 * 3， 推送给相关玩家数据信息
 * 4， 下发留言成功
 * 
 * TODO 重复登陆的处理， 下发玩家数据信息.
 * 
 * @author lvjc
 */
public class SendFriendMessageHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("FriendInfo");
	
	public static void SendFriendMessage(Packet packet, IoSession session)
    {
        OPSendMessage sendMessage = null;
		try
        {
			sendMessage = OPSendMessage.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int friendid = sendMessage.getUid();
		String msg = sendMessage.getMsg();
		int version = sendMessage.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger2.info("send friend message handler received: {}, {}, {}", new Object[]{id , version , friendid , msg});
        long current_time = Util.getServerMillTime();
        OPSendMessageRet.Builder builder = OPSendMessageRet.newBuilder();
        Player friend=null;
        friend=ObjectAccessor.getOnlineUser(friendid);
        if(friend==null){
        	friend=new Player();
        	friend.setId(friendid);
        }
        SysMsgInfoServices.friendChat(player, friend, msg);
		builder.setStatus(1);
        builder.setServertime(1000);

        player.setLastVerifyTime(Util.getServerTime());
		player.sendPacket(OP.code.OPCODE_SEND_FRIEND_MESSAGERET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("send friend message handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}
