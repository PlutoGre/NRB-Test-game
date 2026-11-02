package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.GetAdventurePowerInfo.OPGetAdventurePower;
import com.oldmountain.dzm.message.GetAdventurePowerInfo.OPGetAdventurePowerRet;
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
public class GetAdventurePowerInfoHandler
{
	private static final Logger log = LoggerFactory.getLogger(GetAdventurePowerInfoHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
	public static void getInfo(Packet packet, IoSession session)
    {
        OPGetAdventurePower opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPGetAdventurePower.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        log.info("get adventure power info handler received: {}, {}, {}", new Object[]{id , version});
        logger2.info("get adventure power info handler received: {}, {}, {}", new Object[]{id , version});
        int current_time = Util.getServerTime();
        OPGetAdventurePowerRet.Builder builder = OPGetAdventurePowerRet.newBuilder();
		builder.setStatus(1);
        builder.setServertime((int) System.currentTimeMillis());
        builder.setAmStatus(0);
        builder.setPmStatus(0);
		player.sendPacket(OP.code.OPCODE_GET_ADVENTURE_POWERINFORET_S_VALUE, builder);// 发送返回包
		int current_time_2 = Util.getServerTime();
		int taken_time=current_time_2-current_time;
		log.info("get adventure power info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
		logger2.info("get adventure power info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}
