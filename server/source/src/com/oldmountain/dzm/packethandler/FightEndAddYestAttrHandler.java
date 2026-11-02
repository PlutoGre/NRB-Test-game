package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.FightEndAddYestAttr.OPFightEndAddYestAttr;
import com.oldmountain.dzm.message.FightEndAddYestAttr.OPFightEndAddYestAttrRet;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.AdventureFightStatsServices;
import com.oldmountain.dzm.util.AdventureFightEndUtil;
import com.oldmountain.dzm.util.TitleUtil;
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
public class FightEndAddYestAttrHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FightEndInfo");
	public static void addAttr(Packet packet, IoSession session)
    {
        OPFightEndAddYestAttr opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPFightEndAddYestAttr.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
		int additionType = opDisposeFriendsInfo.getId();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        int id=player.getId();
        logger2.info("addAttr received PlayerId:{},Version:{},AdditionType:{}", new Object[]{id , version , additionType});
        long current_time = Util.getServerMillTime();
        OPFightEndAddYestAttrRet.Builder yestBuilder=OPFightEndAddYestAttrRet.newBuilder();
        if(player.getFightStats().checkIsAcrossDay())
        {//如果跨天，展现面板为主面板
            yestBuilder.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player, OPFightEndInfoRet.PanelType.MainPanel_VALUE, false, false));
        }
        else
        {//否则为当前面板,判断上一个面板是否为主面板
            if(player.getFightStats().getBattlePanelType()!=OPFightEndInfoRet.PanelType.MainPanel_VALUE)
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S_VALUE);
                eBuilder.setErrormsg("PlayerId:"+id+" Current Panel is not Main Panel!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                logger2.info("Error Reporter send: fight end add yest attr handler: {}, {}, {}", new Object[]{id});
                return;
            }
            else
            {
                if(player.getFightStats().getBattleAmount()>= AdventureFightEndUtil.DAILY_BARRIER_COUNT)
                {
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S_VALUE);
                    eBuilder.setErrormsg("PlayerId:"+id+" Daily Barrier Count Is Max!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Error Reporter send: fight end add yest attr handler: {}, {}, {}", new Object[]{id});
                    return;
                }
                else
                {
                    player.getFightStats().resetBarrierConf();
                    UserStats userStats=player.getUserStats();
                    userStats.setFightEndTimes(userStats.getFightEndTimes()+1);
                    player.setUserStats(userStats);
                    TitleUtil.checkNewTitle(player, 5, userStats.getFightEndTimes());
                    //昨日已在排行榜内，则不进行昨日加成
                    if(player.getFightStats().getContinueInRankAmount()<=0)
                    {
                        //添加昨天血战信息加成
                        player.getFightStats().increaseYestAdditionByType(additionType);
                    }
                    else
                    {
                        player.getFightStats().increaseBattleAmount();
                    }
                    //设置下一次要显示的关卡信息
                    player.getFightStats().setBattlePanelType(OPFightEndInfoRet.PanelType.TeamPanel_VALUE);
                    player.getFightStats().setCurrBarrierLV(1);
                    //生成血战基本信息以及战斗信息
                    yestBuilder.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player,OPFightEndInfoRet.PanelType.TeamPanel_VALUE,true,false));
                }
            }
        }

        yestBuilder.setStatus(1);
        yestBuilder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S_VALUE, yestBuilder);// 发送返回包
		player.setLastVerifyTime(Util.getServerTime());
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("addAttr Send PlayerId:{},Cost:{}", new Object[]{id ,taken_time});
	}

}
