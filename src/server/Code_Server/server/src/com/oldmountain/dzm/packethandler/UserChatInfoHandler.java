package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserChatInfo.OPUserChatInfo;
import com.oldmountain.dzm.message.UserChatInfo.OPUserChatInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;

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
public class UserChatInfoHandler {
	private static final Logger logger2 = LoggerFactory.getLogger("ChatInfo");
	public static void getChatInfo(Packet packet, IoSession session)
    {
        OPUserChatInfo selFriendsInfoArray = null;
		try
        {
			selFriendsInfoArray = OPUserChatInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = selFriendsInfoArray.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        logger2.info("user chat info handler received: {}, {}, {}", new Object[]{playerId , version});

        int current_time = Util.getServerTime();
        OPUserChatInfoRet.Builder builder = OPUserChatInfoRet.newBuilder();

        for(int i=0;i<30;i++)
        {
            OPUserChatInfoRet.ChatInfo.Builder chatInfo=OPUserChatInfoRet.ChatInfo.newBuilder();
            chatInfo.setId(i);
            chatInfo.setUId(i * 2);
            chatInfo.setName("姓名(vip" + i+"):");
            chatInfo.setMsg("哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦哇啦");
            chatInfo.setEventTime((int) System.currentTimeMillis());
            builder.addChatInfo(chatInfo);
        }
        builder.setServertime(1000);

		player.sendPacket(OP.code.OPCODE_CHATRET_S_VALUE, builder);// 发送返回包
		int current_time_2 = Util.getServerTime();
		int taken_time=current_time_2-current_time;
		logger2.info("user chat info handler ret send: {}, {}, {}", new Object[]{playerId ,taken_time});
	}

}
