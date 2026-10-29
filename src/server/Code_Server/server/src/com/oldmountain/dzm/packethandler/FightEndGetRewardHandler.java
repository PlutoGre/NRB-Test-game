package com.oldmountain.dzm.packethandler;

import com.oldmountain.dzm.entity.UserStats;
import com.oldmountain.dzm.util.*;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.apache.mina.common.IoSession;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.message.ErrorReporter;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.FightEndGetReward.OPFightEndGetReward;
import com.oldmountain.dzm.message.FightEndGetReward.OPFightEndGetRewardRet;
import com.oldmountain.dzm.message.FightEndInfo.OPFightEndInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.net.packet.Packet;
import com.oldmountain.dzm.services.AdventureFightStatsServices;
import com.oldmountain.dzm.services.PayToolServices;
import com.oldmountain.dzm.services.UserPropertyServices;

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
public class FightEndGetRewardHandler
{
	private static final Logger logger2 = LoggerFactory.getLogger("FightEndInfo");
	public static void gerReward(Packet packet, IoSession session)
    {
        OPFightEndGetReward opDisposeFriendsInfo = null;
		try
        {
			opDisposeFriendsInfo = OPFightEndGetReward.parseFrom(packet.data.array());
		}
        catch (InvalidProtocolBufferException e)
        {
			e.printStackTrace();
			return;
		}
		
		int version = opDisposeFriendsInfo.getVersion();//version default = 1;
        int uuid = (Integer)session.getAttribute("playerid");
        Player player = ObjectAccessor.getOnlineUser(uuid);
        if(player.getBattleCount()< AdventureFightEndUtil.MIN_BATTLE_COUNT)
        {
            ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
            eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
            eBuilder.setErrormsg("PlayerId:"+player.getId()+" player level dissatisfy 5 Exception!");
            player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
            return;
        }
        int id=player.getId();
        logger2.info("gerReward received: PlayerId:{}, Version:{}", new Object[]{id , version});
        long current_time = Util.getServerMillTime();
        OPFightEndGetRewardRet.Builder rewardBuilder=OPFightEndGetRewardRet.newBuilder();
        if(player.getFightStats().checkIsAcrossDay())
        {//已经跨天
            rewardBuilder.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player, OPFightEndInfoRet.PanelType.MainPanel_VALUE,false,false));
        }
        else
        {
            if(player.getFightStats().isRewardPanel()||player.getFightStats().getBattlePanelType()==OPFightEndInfoRet.PanelType.RewardPanel_VALUE)
            {//判断是否为领取奖励的关卡，
                JSONArray ja=player.getFightStats().getExtraRewardConfJsonArray();
                //领取奖励，将奖励信息添加到用户对应表中
                for(int i=0;i<ja.size();i++)
                {
                    JSONObject item=ja.getJSONObject(i);
                    switch (UserToolConst.getType(item.getInt("type")))
                    {
                        case UserToolConst.TOOLS_TYPE:
                            rewardBuilder.addToolInfo(PayToolServices.BuyToolsAlone(player, item.getInt("itemId"), item.getInt("quantity")));
                            //添加宝箱运气池的运气值
                            if(UserToolConst.getType(item.getInt("itemId"))==UserToolConst.TREASURE_TOOL)
                            {
                                BagUtil.getInstance().addLuckPool(player,item.getInt("itemId"),item.getInt("quantity"), XMLTemplateService.getSysBasicConfig().getFightEndLuckyProportion());
                            }
                            break;
                        case UserToolConst.USER_PROPERTY_TYPE:
                            UserPropertyServices.changeProperty(player,item.getInt("itemId"), item.getInt("quantity"));
                            break;
                    }
                }
                //生成下一次领取奖励关卡，记录本次得星最大值并清空阶段性奖励得星信息
                player.getFightStats().calcStepReward();
                rewardBuilder.setFightEndInfo(AdventureFightStatsServices.getFightEndInfo(player, OPFightEndInfoRet.PanelType.TeamPanel_VALUE, true,false));
            }
            else
            {
                ErrorReporter.OPErrorReporter.Builder eBuilder = ErrorReporter.OPErrorReporter.newBuilder();
                eBuilder.setErrorid(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE);
                eBuilder.setErrormsg("PlayerId:"+player.getId()+" Current Barrier Can't Get Reward!");
                player.sendPacket(OP.code.OPCODE_ERROR_RET_S_VALUE, eBuilder);
                logger2.info("Error Reporter send: fight end get reward handler: {}, {}, {}", new Object[]{id});
                return ;
            }
        }
        player.setLastVerifyTime(Util.getServerTime());
        rewardBuilder.setStatus(1);
        rewardBuilder.setServertime((int) System.currentTimeMillis());
        rewardBuilder.setSilvercoins(player.getSilvercoins());
        rewardBuilder.setGoldcoins(player.getGoldCoins());
		player.sendPacket(OP.code.OPCODE_GET_FIGHTEND_REWARD_INFORET_S_VALUE, rewardBuilder);// 发送返回包
		long current_time_2 = Util.getServerMillTime();
		long taken_time=current_time_2-current_time;
		logger2.info("gerReward Send: PlayerId:{},Cost:{}", new Object[]{id ,taken_time});
	}

}
