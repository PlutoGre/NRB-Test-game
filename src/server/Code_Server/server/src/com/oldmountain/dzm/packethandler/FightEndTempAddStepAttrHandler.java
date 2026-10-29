package com.oldmountain.dzm.packethandler;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.message.FightEndTempAddStepAttr.OPFightEndTempAddStepAttr;
import com.oldmountain.dzm.message.FightEndTempAddStepAttr.OPFightEndTempAddStepAttrRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.AdventureFightStatsServices;
import com.oldmountain.dzm.util.AdventureFightEndUtil;
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
public class FightEndTempAddStepAttrHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FightEndInfo");
	public static void addAttr(Packet packet, IoSession session)
    {
        OPFightEndTempAddStepAttr opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPFightEndTempAddStepAttr.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
        int additionIndex = opDisposeFriendsInfo.getId();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        if(player.getBattleCount()< AdventureFightEndUtil.MIN_BATTLE_COUNT)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" player level dissatisfy 5 Exception!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        int id=player.getId();
        logger2.info("addAttr Temp received: PlayerId:{},Version:{},AdditionIndex:{}", new Object[]{id , version , additionIndex});
        long current_time = Util.getServerMillTime();
        if(additionIndex>3||additionIndex<1)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Select Addition Id:"+additionIndex+" Is Wrong!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            logger2.info("Error Reporter send: fight end temp add step attr handler: {}, {}, {}", new Object[]{id});
            return;
        }
        OPFightEndTempAddStepAttrRet.Builder tempAttrBuilder=OPFightEndTempAddStepAttrRet.newBuilder();
        if(player.getFightStats().checkIsAcrossDay())
        {//已经跨天
            tempAttrBuilder.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player, OPFightEndInfoRet.PanelType.MainPanel_VALUE,false,false));
        }
        else
        {
            if(player.getFightStats().isSelAdditionPanel()||player.getFightStats().getBattlePanelType()==OPFightEndInfoRet.PanelType.StepAddAttr_VALUE)
            {
                int needStar= AdventureFightEndUtil.getDeducteStarByIndex(additionIndex);
                //判断当前得星是否够选择临时加成
                if(player.getFightStats().getCurrSurplusStar()<needStar)
                {
                    ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                    eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S_VALUE);
                    eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Select Index Is Not Enough!");
                    player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                    logger2.info("Error Reporter send: fight end temp add step attr handler: {}, {}, {}", new Object[]{id});
                    return;
                }
                else
                {
                    //进行属性加成并扣除得星
                    player.getFightStats().increaseTempAdditionByIndex(additionIndex);
                    //计算下次展现面板
                    player.getFightStats().getNextPanelType(true);
                    tempAttrBuilder.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player,player.getFightStats().getBattlePanelType(),true,false));
                }

            }
            else
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S_VALUE);
                eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Panel is not Select AdditionType Panel!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                logger2.info("Error Reporter send: fight end temp add step attr handler: {}, {}, {}", new Object[]{id});
                return;
            }
        }
        
        player.setLastVerifyTime(Util.getServerTime());

        tempAttrBuilder.setStatus(1);
        tempAttrBuilder.setServertime((int) System.currentTimeMillis());
		player.sendPacket(OP.code.OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S_VALUE, tempAttrBuilder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("addAttr Temp send  PlayerId:{} Cost:{}", new Object[]{id ,taken_time});
	}

}
