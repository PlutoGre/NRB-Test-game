package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.RecordReadMsgTime.*;
import com.oldmountain.dzm.services.RewardServices;
import com.oldmountain.dzm.services.SysMsgInfoServices;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserSysMsgInfo.OPUserSysMsgInfo;
import com.oldmountain.dzm.message.UserSysMsgInfo.OPUserSysMsgInfoRet;
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
 *
 * @author lvjc
 */
public class UserSysMsgInfoHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("SysMsgInfo");

    /**
     * 获取系统消息
     * @param packet
     * @param session
     */
	public static void getSysMsgInfo(Packet packet, IoSession session)
    {
        OPUserSysMsgInfo selFriendsInfoArray = null;
		try
        {
			selFriendsInfoArray = OPUserSysMsgInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = selFriendsInfoArray.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        logger2.info("getSysMsgInfo handler received: {}, {}, {}", new Object[]{playerId , version});
        long startTime = Util.getServerMillTime();
        
        OPUserSysMsgInfoRet.Builder builder = RewardServices.getSysReward(player);
        builder= SysMsgInfoServices.getUserSysMsgRet(player,builder);
        long endTime = Util.getServerMillTime();
        builder.setServertime(Util.getServerTime());
        UserStats _stats=player.getUserStats();
        _stats.setSysMsgTime(Util.getServerTime());
        player.setUserStats(_stats);
        builder.setReadTime(player.getUserStats().getSysMsgTime());
		player.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, builder);// 发送返回包
		long costTime=endTime-startTime;
		logger2.info("getSysMsgInfo handler ret send: {}, Cost:{}, {}", new Object[]{playerId ,costTime});
	}

    /**
     * 记录上次阅读的传书的时间
     * @param packet
     * @param session
     */
    public static void recordReadMsgTime(Packet packet,IoSession session)
    {
        OPRecordReadMsgTime recordReadMsgTime = null;
        try
        {
            recordReadMsgTime = OPRecordReadMsgTime.parseFrom(packet.data.array());
        }
        catch (InvalidProtocolBufferException e)
        {
            e.printStackTrace();
            return;
        }
        int version = recordReadMsgTime.getVersion();//version default = 1;
        int playerId = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(playerId);
        logger2.info("recordReadMsgTime handler received: {}, {}, {}", new Object[]{playerId , version});
        long startTime = Util.getServerMillTime();
        int recordTime=Util.getServerTime();
        UserStats _stats=player.getUserStats();
        _stats.setSysMsgTime(recordTime);
        player.setUserStats(_stats);
        OPRecordReadMsgTimeRet.Builder builder = OPRecordReadMsgTimeRet.newBuilder();
        builder.setServertime(1);
        long endTime = Util.getServerMillTime();
        builder.setServertime(recordTime);
        player.sendPacket(OP.code.OPCODE_RECORDREADMSGTIMERET_S_VALUE, builder);// 发送返回包
        long costTime=endTime-startTime;
        logger2.info("recordReadMsgTime handler ret send: {}, Cost:{}, {}", new Object[]{playerId ,costTime});
    }
}
