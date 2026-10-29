package com.oldmountain.dzm.services;

import java.util.Iterator;
import java.util.List;
import java.util.Map;

import net.sf.ehcache.search.expression.And;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.message.OP;
import com.oldmountain.dzm.message.UserRewardInfo;
import com.oldmountain.dzm.message.UserSysMsgInfo;
import com.oldmountain.dzm.message.UserSysMsgInfo.OPUserSysMsgInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.util.TimeUtil;
import com.oldmountain.dzm.util.Util;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-8
 * Time: 下午5:06
 * To change this template use File | Settings | File Templates.
 */
public class RewardServices
{

    public static final int FIGHTEND_REWARD=6;
    public static final int COMPENSATE_REWARD=7;
    public static final int WORLDBOSSRANK_REWARD=8;
    private static final Logger log = LoggerFactory.getLogger(RewardServices.class);
    //下一次数据清理时间
    private static int _nextClearTime;

    /**
     * 添加补偿奖励到rewards表
     * @param msg
     * @param content
     * @param lastLogin
     * @param stats
     * @param playerId
     */
    public synchronized static void createCompensateReward(String msg,JSONObject rewardInfo,JSONArray content,int lastLogin,int levelStart,int levelEnd,int rankStart,int rankEnd,int stats,int playerId,String channel)
    {
        log.info("createCompensateReward rewards begin ！");
        int starTime= Util.getServerTime();
        DaoManager.getRewardDao().createCompensateReward(msg,rewardInfo,content,stats,playerId,lastLogin,levelStart,levelEnd,rankStart,rankEnd,channel);
        //全服补偿
        if(playerId==-1)
        {
            Iterator iter= ObjectAccessor.players.entrySet().iterator();
            while (iter.hasNext())
            {
                Map.Entry entry = (Map.Entry) iter.next();
                Player _player = (Player)entry.getValue();
                if(_player.getLastlogin()<lastLogin)
                {
                    continue;
                }
                if (levelStart != 0 && _player.getLevel() < levelStart) 
                {
					continue;
				}
                if (levelEnd != 0 && _player.getLevel() > levelEnd) {
					continue;
				}
                if (rankStart != 0 && _player.getChallenge() != null && _player.getChallenge().getRank() < rankStart) {
					continue;
				}
                if (rankEnd != 0 && _player.getChallenge() != null && _player.getChallenge().getRank() > rankEnd) {
					continue;
				}
                if(_player.getPuid().indexOf(channel) == -1){
                	continue;
                }
                _player.setNeedSyncRewards(true);
            }
        }
        else
        {
            //单个玩家补偿
            if(ObjectAccessor.players.containsKey(playerId))
            {
                Player _player=ObjectAccessor.getOnlineUser(playerId);
                if(_player.getPuid().indexOf(channel) == -1){
                	return;
                }
                if(null!=_player)
                {
                    _player.setNeedSyncRewards(true);
                    //如果玩家在线 推送数据
                    if(null!=_player.getSession())
                    {
                        UserSysMsgInfo.OPUserSysMsgInfoRet.Builder builder = RewardServices.getSysReward(_player);
                        builder.setServertime(starTime);
                        _player.sendPacket(OP.code.OPCODE_GET_USER_SYSMSG_INFORET_S_VALUE, builder);// 发送返回包
                    }
                }
            }
        }
        int endTime=Util.getServerTime();
        log.info("createCompensateReward rewards over!，Cost Time："+(endTime-starTime)+" second！");
    }

    /**
     * 获取当前阵容昨日排行榜数据信息
     * @param player
     * @return
     */
    public static OPUserSysMsgInfoRet.Builder getSysReward(Player player)
    {
        int serverTime=Util.getServerTime();

        checkNextDay(serverTime);

        OPUserSysMsgInfoRet.Builder ret=OPUserSysMsgInfoRet.newBuilder();
        if(player.isNeedSyncRewardsToMap())
        {
            List<Reward> _rewardArr=DaoManager.getRewardDao().getReward(player,serverTime);
            List<Reward> _rewardNotGet=DaoManager.getRewardDao().getRewardNotDeal(player);
            if(_rewardNotGet!=null && _rewardNotGet.size()>0){
            	for(int i=0; i<_rewardNotGet.size();i++){
            		_rewardArr.add(_rewardNotGet.get(i));
            	}
            }
            int count=0;
            for(Reward _item:_rewardArr)
            {
                Reward _pItem= null;
                if(player.getRewards().containsKey(_item.getId()))
                {
                    _pItem=player.getRewards().get(_item.getId());
                }
                else
                {
                    player.getRewards().put(_item.getId(),_item);
                }
                OPUserSysMsgInfoRet.SysMsgInfo.Builder msgInfo=getSysMsgInfo(_item,_pItem,count);
                msgInfo.setEventTime(_item.getAddTime());
                ret.addSysMsg(msgInfo);
                count++;
            }
            player.setNeedSyncRewards(false);
        }
        else
        {
            Iterator<Map.Entry<Integer, Reward>> iter = player.getRewards().entrySet().iterator();
            int count=0;
            while (iter.hasNext())
            {
                Map.Entry<Integer,Reward> entry = iter.next();
                Reward _item = (Reward)entry.getValue();
                OPUserSysMsgInfoRet.SysMsgInfo.Builder msgInfo=getSysMsgInfo(_item,_item,count);
                msgInfo.setEventTime(_item.getAddTime());
                ret.addSysMsg(msgInfo);
                count++;
            }
        }
        return ret;
    }


    /**
     * 获取玩家奖励信息消息
     * @param _item
     * @param id
     * @return
     */
    public static OPUserSysMsgInfoRet.SysMsgInfo.Builder getSysMsgInfo(Reward _item ,Reward _pItem,int id)
    {
        OPUserSysMsgInfoRet.SysMsgInfo.Builder sysMsgInfo=OPUserSysMsgInfoRet.SysMsgInfo.newBuilder();

        sysMsgInfo.setId(id);
        sysMsgInfo.setRelationId(_item.getId());
        sysMsgInfo.setMsgInfo(_item.getRewardInfo());
        sysMsgInfo.setMsgContent(_item.getRewardMsg());
        if(_pItem!=null)
        {
            sysMsgInfo.setOprType(_pItem.getStatus());
        }
        else
        {
            sysMsgInfo.setOprType(_item.getStatus());
        }
        sysMsgInfo.setMsgType(_item.getRewardType());
        int len=_item.getRewardContentJson().size();
        for(int i=0;i<len;++i)
        {
            JSONObject rewardItem  =_item.getRewardContentJson().getJSONObject(i);
            UserRewardInfo.OPRewardInfoRet.RewardInfo.Builder reward=UserRewardInfo.OPRewardInfoRet.RewardInfo.newBuilder();
            reward.setItemType(rewardItem.getInt("t"));
            reward.setItemId(rewardItem.getInt("i"));
            reward.setCount(rewardItem.getInt("q"));
            sysMsgInfo.addReward(reward);
        }
        return sysMsgInfo;
    }

    /**
     * 生成下次日期
     */
    private synchronized  static void getCurrDay()
    {
        _nextClearTime =TimeUtil.getNextDayMinTime();
    }

    /**
     * 服务器数据清理时间,每日仅清理一次，且由第一个操作奖励的玩家来触发
     * @param currTime
     */
    public synchronized static boolean checkNextDay(int currTime)
    {
         if(currTime> _nextClearTime)
         {
            log.info("clear rewards  begin ！");
            int starTime= Util.getServerTime();
            DaoManager.getRewardDao().deleteRewardInvalid(currTime);
            getCurrDay();
            int endTime=Util.getServerTime();
            log.info("clear rewards over!，Cost Time："+(endTime-starTime)+" second！");
             return true;
         }
        return false;
    }
}