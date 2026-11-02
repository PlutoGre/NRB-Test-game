package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.GetAdventureBoundlessHole.OPGetBoundlessHole;
import com.oldmountain.dzm.message.GetAdventureBoundlessHole.OPGetBoundlessHoleRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

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
public class GetAdventureBoundlessHoleHandler
{
	private static final Logger log = LoggerFactory.getLogger(GetAdventureBoundlessHoleHandler.class);
	private static final Logger logger2 = LoggerFactory.getLogger("AdventureInfo");
	public static void getBoundlessHole(Packet packet, IoSession session)
    {
        OPGetBoundlessHole opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPGetBoundlessHole.parseFrom(packet.data.array());
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
        log.info("get adventure boundless hole handler received: {}, {}, {}", new Object[]{id , version});
        logger2.info("get adventure boundless hole handler received: {}, {}, {}", new Object[]{id , version});
        long current_time = Util.getServerMillTime();
        UserStats stats = player.getUserStats();
        OPGetBoundlessHoleRet.Builder builder = OPGetBoundlessHoleRet.newBuilder();

        int currentTime = (int) (System.currentTimeMillis()/1000);

        builder.setStatus(1);
        builder.setServertime(currentTime);
        int continueDay = XMLTemplateService.getVisitRewardSelecter(stats.getVisitRewardCount()).getContinueDay();
        builder.setAccumulateCount(continueDay);
        builder.setCurrCount(stats.getVisitTimes());
        if(stats.getHasVisited()==1)
        {
           builder.setTodayStatus(1);

        }else
        {
            builder.setTodayStatus(0);
        }
        player.setLastVerifyTime(Util.getServerTime());

		player.sendPacket(OP.code.OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		log.info("get adventure boundless hole handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
		logger2.info("get adventure boundless hole handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}
