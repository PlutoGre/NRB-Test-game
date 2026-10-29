package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.services.FightEndRankingServices;
import com.oldmountain.dzm.util.AdventureFightEndUtil;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.AdventureFightStats;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfo;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.AdventureFightStatsServices;
import com.oldmountain.dzm.services.DaoManager;
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
public class FightEndInfoHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FightEndInfo");
	public static void getFightEndInfo(Packet packet, IoSession session)
    {
        OPFightEndInfo opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPFightEndInfo.parseFrom(packet.data.array());
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
		logger2.info("getFightEndInfo received PlayerId:{},Version:{}", new Object[]{id , version});
		long current_time = Util.getServerMillTime();
        if(player.getBattleCount()< AdventureFightEndUtil.MIN_BATTLE_COUNT)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" player level dissatisfy 5 Exception!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        //检测是否为下一天的开始，若为下一天的开始，重置数据
        int panelType=-1;
        AdventureFightStats _adventureFightStats= DaoManager.getAdventureFightDao().getAdventureFightStats(player);
        boolean isNewStats=false;
        //是否是新用户
        if(_adventureFightStats.getPlayerId()==-1)
        {
            isNewStats=true;
            _adventureFightStats.setNewStatus(0);
            _adventureFightStats.setPlayerId(player.getId());
            _adventureFightStats.setBattlePanelType(OPFightEndInfoRet.PanelType.MainPanel_VALUE);
        }
        OPFightEndInfoRet.Builder builder;

        if(player.getFightStats().checkIsAcrossDay()||isNewStats)
        {//如果跨天或者是新用户，展现面板为主面板，所展现的面板需生成数据
            builder = AdventureFightStatsServices.getFightEndInfo(player, OPFightEndInfoRet.PanelType.MainPanel_VALUE,true,isNewStats);
        }
        else
        {//否则为当前面板，无需生成数据
            builder = AdventureFightStatsServices.getFightEndInfo(player,_adventureFightStats.getBattlePanelType(),false,isNewStats);
        }
        player.setLastVerifyTime(Util.getServerTime());
		builder.setStatus(1);
        builder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_GET_FIGHTEND_INFORET_S_VALUE, builder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("getFightEndInfo ret send PlayerId:{},Cost:{}", new Object[]{id ,taken_time});
	}

}
