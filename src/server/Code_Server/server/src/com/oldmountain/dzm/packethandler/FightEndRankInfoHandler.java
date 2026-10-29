package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.util.AdventureFightEndUtil;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.FightEndRankInfo.OPFightEndRankInfo;
import com.oldmountain.dzm.message.FightEndRankInfo.OPFightEndRankInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.FightEndRankingServices;
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
public class FightEndRankInfoHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FightEndInfo");
	public static void getFightEndInfo(Packet packet, IoSession session)
    {
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        OPFightEndRankInfo opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPFightEndRankInfo.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
		int index = opDisposeFriendsInfo.getIndex();//version default = 1;
		int id=player.getId();
        if(player.getBattleCount()< AdventureFightEndUtil.MIN_BATTLE_COUNT)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_RANK_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" player level dissatisfy 5 Exception!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
		logger2.info("fight end rank info handler received: {}, {}, {}", new Object[]{id , version , index});
		long current_time = Util.getServerMillTime();

        OPFightEndRankInfoRet.Builder builder = FightEndRankingServices.getFightEndRankInfoByBattleCount(index+4);
        builder.setPlayerId(player.getId());
		player.sendPacket(OP.code.OPCODE_GET_FIGHTEND_RANK_INFORET_S_VALUE, builder);// 发送返回包
		player.setLastVerifyTime(Util.getServerTime());
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("fight end rank info handler ret send: {}, {}, {}", new Object[]{id ,taken_time});
	}

}
