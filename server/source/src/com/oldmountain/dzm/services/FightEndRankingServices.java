package com.oldmountain.dzm.services;

import com.oldmountain.dzm.DZM;
import com.oldmountain.dzm.entity.AdventureFightRank;
import com.oldmountain.dzm.entity.AdventureFightStats;
import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;
import com.oldmountain.dzm.message.FightEndRankInfo.OPFightEndRankInfoRet;
import com.oldmountain.dzm.net.ObjectAccessor;
import com.oldmountain.dzm.util.TimeUtil;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-8
 * Time: 下午5:06
 * To change this template use File | Settings | File Templates.
 */
public class FightEndRankingServices
{
    //昨日血战排行数据
    private static final ConcurrentHashMap<Integer, ConcurrentHashMap<Integer,AdventureFightRank>> yestRanking = new ConcurrentHashMap<Integer, ConcurrentHashMap<Integer,AdventureFightRank>>(25);
    //今日血战排行榜数据
    private static final ConcurrentHashMap<Integer, ConcurrentHashMap<Integer,AdventureFightRank>> todayRanking = new ConcurrentHashMap<Integer, ConcurrentHashMap<Integer,AdventureFightRank>>(100);
    //今日排行榜中最小得星数
    private static final ConcurrentHashMap<Integer,Integer> todayMinStar=new ConcurrentHashMap<Integer, Integer>(4);
    private static final int FIVE=5;
    private static final int SIX=6;
    private static final int SEVEN=7;
    private static final int EIGHT=8;
    private static final Logger log = LoggerFactory.getLogger(FightEndRankingServices.class);
    private static final int RANKING_LIMIT=20;
    //当前日期YYYYMMDD
    private static int CurrDay=-1;
    //昨日时间YYYYMMDD
    private static int YestDay=-1;
    //Map达到最大数时进行排序剔除前20名以外玩家
    private static int MapSortSize =100;
    //下一次排行数据同步时间
    private static int _nextSyncTime;

    private static final ConcurrentHashMap<Integer,OPFightEndRankInfoRet.Builder> OPFightEndRankInfoRetMap = new ConcurrentHashMap<Integer,OPFightEndRankInfoRet.Builder>(4);

    private static final ConcurrentHashMap<Integer,ArrayList<AdventureFightRank>> todayRankingArr = new ConcurrentHashMap<Integer,ArrayList<AdventureFightRank>>(100);
    /**
     * 获取当前阵容昨日排行榜数据信息
     * @param battleCount
     * @return
     */
    public static OPFightEndRankInfoRet.Builder getFightEndRankInfoByBattleCount(int battleCount)
    {
        int serverTime=Util.getServerTime();
        checkNextDay(serverTime);
        OPFightEndRankInfoRet.Builder ret;
        if(OPFightEndRankInfoRetMap.containsKey(battleCount))
        {
            return OPFightEndRankInfoRetMap.get(battleCount);
        }
        else
        {
            ret=OPFightEndRankInfoRet.newBuilder();
            List<AdventureFightRank> _arr=sortYestRankingByBattle(battleCount);
            int i=1;
            for(AdventureFightRank ar:_arr)
            {
                OPFightEndRankInfoRet.RankItem.Builder rankItem=OPFightEndRankInfoRet.RankItem.newBuilder();
                rankItem.setId(i);
                rankItem.setPlayerId(ar.getPlayerId());
                rankItem.setPlayerName(ar.getPlayerName());
                rankItem.setLevel(ar.getPlayerLevel());
                rankItem.setObtainStar(ar.getObtainStar());
                rankItem.setPassBattle(ar.getPassBarrier());
                rankItem.setSussessionCount(ar.getContinueInRankAmount());
                ret.addRankItem(rankItem);
                i++;
            }
            ret.setStatus(1);
            ret.setRankType(OPFightEndRankInfoRet.Fight_Rank_Type.valueOf(battleCount-4));
            ret.setServertime(serverTime);
            OPFightEndRankInfoRetMap.put(battleCount,ret);
            return ret;
        }
    }

    /**
     * 重启服务器后初始化血战排行榜数据
     */
    public synchronized static void initRanking()
    {
        MapSortSize= DZM.conf.getInt("FightEndRankSortSize");
        getCurrDay();
        log.info("=======Init FightEnd Date ["+YestDay+"] Ranking Info Start!=======");
        int time=Util.getServerTime();
        ArrayList<AdventureFightRank> list= (ArrayList<AdventureFightRank>) DaoManager.getAdventureFightRankingDao().getFightRanking(YestDay);
        //用于处理昨日排行榜数据未同步
        boolean  isYestSyncRank=false;
        if(list.size()<=0)
        {
            //如果昨日排行没有数据
            //查看昨日战斗信息在排行榜内的玩家,并将需要同步放在今日排行榜内
            //如果仍旧没有数据,则昨日没有战斗信息
            for(int i=5;i<=8;++i)
            {
                int todayMinTime=TimeUtil.getNextDayMinTime()-24*60*60;
                int yestMinTime=todayMinTime-24*60*60;
                DaoManager.getAdventureFightRankingDao().syncStatsToRank(i,yestMinTime,todayMinTime,YestDay);
            }
            isYestSyncRank=true;
        }
        list= (ArrayList<AdventureFightRank>) DaoManager.getAdventureFightRankingDao().getFightRanking(YestDay);
        for(AdventureFightRank ad:list)
        {

            if(yestRanking.containsKey(ad.getBarrierCount()))
            {
                yestRanking.get(ad.getBarrierCount()).put(ad.getPlayerId(),ad);
            }
            else
            {
                ConcurrentHashMap<Integer,AdventureFightRank> fetch=new ConcurrentHashMap<Integer,AdventureFightRank>(20);
                fetch.put(ad.getPlayerId(),ad);
                yestRanking.put(ad.getBarrierCount(),fetch);
            }
        }
        log.info("=======Init FightEnd Date ["+YestDay+"] Ranking Info Over!=======");

        if(isYestSyncRank)
        {
            for(int i=5;i<=8;++i)
            {
                List<AdventureFightRank> _arr=sortYestRankingByBattle(i);
                int level=1;
                for(AdventureFightRank ar:_arr)
                {
                    addRewardToDb(ar,time,level,i,YestDay);
                    level++;
                }
            }
        }

        log.info("=======Init FightEnd Date ["+CurrDay+"] Ranking Info Start!=======");
        for(int i=5;i<=8;++i)
        {//处理当天停服后今日排行榜内的玩家信息,并将需要同步放在今日排行榜内,并将排行榜数据需要同步时间定在昨日一早
            int nextMinTime=TimeUtil.getNextDayMinTime();
            int todayMinTime=nextMinTime-24*60*60;
            list= (ArrayList<AdventureFightRank>) DaoManager.getAdventureFightRankingDao().getFightRankingFromStats(i,todayMinTime,nextMinTime,CurrDay);
            if(list.size()>0)
            {
                for(AdventureFightRank ad:list)
                {
                    int battleCount=ad.getBarrierCount();
                    if(todayRanking.containsKey(battleCount))
                    {
                        todayRanking.get(battleCount).put(ad.getPlayerId(), ad);
                    }
                    else
                    {
                        ConcurrentHashMap<Integer,AdventureFightRank> fetch=new ConcurrentHashMap<Integer,AdventureFightRank>(1);
                        fetch.put(ad.getPlayerId(),ad);
                        todayRanking.put(battleCount,fetch);
                    }
                }
            }
            else
            {
                todayRanking.put(i,new ConcurrentHashMap<Integer,AdventureFightRank>(20));
            }
            sortAndRejectByBattleCount(i);
        }
        log.info("=======Init FightEnd Date ["+CurrDay+"] Ranking Info Over!=======");

    }

    /**
     * 生成下次日期
     */
    private synchronized  static void getCurrDay()
    {
        Calendar cl=TimeUtil.getCalendar();
        CurrDay= cl.get(Calendar.YEAR)*10000+(cl.get(Calendar.MONTH)+1)*100+cl.get(Calendar.DAY_OF_MONTH);
        Calendar cYL=TimeUtil.addSystemCurTime(Calendar.DATE,-1);
        YestDay=cYL.get(Calendar.YEAR)*10000+(cYL.get(Calendar.MONTH)+1)*100+cYL.get(Calendar.DAY_OF_MONTH);
        _nextSyncTime=TimeUtil.getNextDayMinTime();
    }

    /**
     * 添加玩家排行信息到今日阵容表内
     * @param player
     */
    @SuppressWarnings("unchecked")
    public synchronized static int addRankingByPlayer(Player player)
    {
        AdventureFightStats fightStats=player.getFightStats();
        int battleCount=player.getBattleCount();
        boolean  isInMap=false;
        int ranking=-1;
        int playerId=player.getId();
        if(fightStats!=null)
        {
            boolean contains=false;
            int rankSize=0;
            if(todayRanking.containsKey(battleCount))
            {
                contains=true;
                rankSize=todayRanking.get(battleCount).size();
            }
            //得星最大值大于当前阵容最小值
            if(fightStats.getMaxObtainStar()>getRankingMinStarByBattleCount(battleCount)||rankSize<20)
            {
               isInMap=true;
               //该排行列表中是否包含这个阵容玩家列表
               if(contains)
               {
                   if(todayRanking.get(battleCount).containsKey(playerId))
                   { //排行中是否包含该玩家
                       if(todayRanking.get(battleCount).get(playerId).getObtainStar()<fightStats.getMaxObtainStar())
                       {
                           todayRanking.get(battleCount).get(playerId).setObtainStar(fightStats.getMaxObtainStar());
                       }
                       //如果闯关数大于上一次闯关信息
                       if(todayRanking.get(battleCount).get(playerId).getPassBarrier()<fightStats.getMaxBarrierLV())
                       {
                           todayRanking.get(battleCount).get(playerId).setPassBarrier(fightStats.getMaxBarrierLV());
                       }
                   }
                   else
                   { //排行中不包含该玩家
                       addRankingToMap(player, fightStats, battleCount);
                   }
               }
               else
               {
                   ConcurrentHashMap<Integer,AdventureFightRank> fetch=new ConcurrentHashMap<Integer,AdventureFightRank>(1);
                   todayRanking.put(battleCount,fetch);
                   addRankingToMap(player,fightStats,battleCount);
               }
            }
        }

        if(isInMap)
        {

            if(todayRankingArr.containsKey(battleCount))
            {
                ranking=todayRankingArr.get(battleCount).indexOf(todayRanking.get(battleCount).get(playerId));
            }

            if(todayRanking.get(battleCount).size()>= MapSortSize || todayRanking.get(battleCount).size() < RANKING_LIMIT)
            {
                sortAndRejectByBattleCount(battleCount);
                if(todayRankingArr.containsKey(battleCount))
                {
                    ranking=todayRankingArr.get(battleCount).indexOf(todayRanking.get(battleCount).get(playerId));
                }
            }
        }
        return ranking==-1?0:(ranking+1);
    }

    /**
     * 昨日排行榜排序列表
     * @param battleCount
     * @return
     */
    private synchronized static List<AdventureFightRank> sortYestRankingByBattle(int battleCount)
    {
        //留前20名玩家
        int starTime= Util.getServerTime();
        List<AdventureFightRank> _arr=new ArrayList<AdventureFightRank>();
        if(yestRanking.containsKey(battleCount))
        {
            _arr.addAll(yestRanking.get(battleCount).values());
        }
        else
        {
            log.info("synchronization FightEndRanking Date ["+YestDay+"] ["+battleCount+"] Battle Ranking Info,Count ["+_arr.size()+"],Total Cost Time:"+(0)+"Second!");
            return _arr;
        }
        Collections.sort(_arr,Collections.reverseOrder());
        int endTime=Util.getServerTime();
        log.info("synchronization FightEndRanking Date ["+YestDay+"] ["+battleCount+"] Battle Ranking Info,Count ["+_arr.size()+"],Total Cost Time:"+(endTime-starTime)+"Second!");
        if(_arr.size()>RANKING_LIMIT)
        {
            return _arr.subList(0,RANKING_LIMIT);
        }
        else
        {
            return _arr;
        }
    }

    /**
     * 今日排行榜排序
     * @param battleCount
     */
    private synchronized static ArrayList<AdventureFightRank> sortAndRejectByBattleCount(int battleCount)
    {
        //排序,留前20名玩家,并记录最小得星
        int starTime= Util.getServerTime();
        ArrayList<AdventureFightRank> _arr=new ArrayList<AdventureFightRank>();
        //取得今日battleCount人阵排行榜数据
        if(todayRanking.containsKey(battleCount))
        {
            _arr.addAll(todayRanking.get(battleCount).values());
        }
        else
        {
            log.info("synchronization FightEndRanking Date ["+CurrDay+"] ["+battleCount+"] Battle Ranking Info,Count ["+_arr.size()+"],Total Cost Time:"+(0)+"Second!");
            return _arr;
        }
        //进行排序
        Collections.sort(_arr,Collections.reverseOrder());
        //取得今日最小得星数
        if(!todayMinStar.containsKey(battleCount))
        {
            todayMinStar.put(battleCount,0);
        }
        int minStar=todayMinStar.containsKey(battleCount)?todayMinStar.get(battleCount):0;
        int currMinStar=0;
        //移除20名以外无用数据
        for(int i=0;i<_arr.size();++i)
        {
            AdventureFightRank _rank=_arr.get(i);
            if(i>=RANKING_LIMIT)
            {
                todayRanking.get(battleCount).remove(_rank.getPlayerId());
            }
            else
            {
                currMinStar=_rank.getObtainStar();
            }
        }
        if(currMinStar>minStar)
        {
            todayMinStar.put(battleCount,currMinStar);
        }

        try
        {
            if(todayRankingArr.containsKey(battleCount))
            {
                todayRankingArr.get(battleCount).clear();
            }
        }
        catch (Exception e)
        {
        }
        int count = Math.min(20, _arr.size());
        List<AdventureFightRank> subList =  _arr.subList(0, count);
        ArrayList<AdventureFightRank>subList2 = new ArrayList<AdventureFightRank>();
        for (int i = 0; i < subList.size(); i++) {
			subList2.add(subList.get(i));
		}
        todayRankingArr.put(battleCount,subList2);
        int endTime=Util.getServerTime();
        log.info("synchronization FightEndRanking Date ["+CurrDay+"] ["+battleCount+"] Battle Ranking Info,Count ["+_arr.size()+"],Total Cost Time:"+(endTime-starTime)+"Second!");
        return subList2;
    }

    /**
     * 排行榜数据同步到服务器,每日仅同步一次,且由第一个操作血战的玩家来触发
     * @param currTime
     */
    public synchronized static void checkNextDay(int currTime)
    {
         if(currTime>_nextSyncTime)
         {
            log.info("synchronization FightEndRanking Date ["+CurrDay+"] Info To DB Start!");
            int starTime= Util.getServerTime();
            exportRanking();
            int endTime=Util.getServerTime();
            log.info("synchronization FightEndRanking Date ["+CurrDay+"] Info To DB Over,Total Cost Time:"+(endTime-starTime)+"Second!");
         }
    }

    /**
     * 获取玩家昨天是否进榜
     * @param playerId
     * @param barrierCount
     * @return
     */
    public static int YestInRankAmountByPlayer(int playerId, int barrierCount)
    {
        int count=0;
        for(int i=5;i<=barrierCount;++i)
        {
            if(yestRanking.containsKey(i))
            {
                 try
                 {
                    if(yestRanking.get(i).containsKey(playerId))
                    {
                        count=1;
                        break;
                    }
                    
                 }
                 catch (Exception e)
                 {
                     count=0;
                 }
            }
        }
        return count;
    }

    /**
     * 导出数据到数据库,同步今日数据到昨日,并清空今日排行信息以及昨日排行榜的proto协议数据
     */
    private synchronized  static void exportRanking()
    {
        int currTime=Util.getServerTime();
        for(int i=5;i<=8;++i)
        {
            try
            {
                yestRanking.get(i).clear();
                yestRanking.put(i,new ConcurrentHashMap<Integer,AdventureFightRank>(20));
            }
            catch (Exception e)
            {
                yestRanking.put(i,new ConcurrentHashMap<Integer,AdventureFightRank>(20));
            }
            ArrayList<AdventureFightRank> _arr=sortAndRejectByBattleCount(i);
            DaoManager.getAdventureFightRankingDao().delFightRanking(i,CurrDay);
            //将排行数据落地,并添加排行奖励内容到reward表
            int level=1;
            for (AdventureFightRank ar:_arr)
            {
            	if (level > 20) {//容错数组个数大于20的情况
					break;
				}
                yestRanking.get(i).put(ar.getPlayerId(), ar);
                DaoManager.getAdventureFightRankingDao().addFightRanking(ar);
                addRewardToDb(ar,currTime,level,i,CurrDay);
                level++;
            }
            todayMinStar.put(i,0);
            try
            {
                todayRanking.get(i).clear();
                todayRanking.put(i,new ConcurrentHashMap<Integer,AdventureFightRank>(20));
            }
            catch (Exception e)
            {
                todayRanking.put(i,new ConcurrentHashMap<Integer,AdventureFightRank>(20));
            }
            OPFightEndRankInfoRetMap.clear();
        }
        getCurrDay();
    }


    private static void addRewardToDb(AdventureFightRank ar,int currTime,int level,int barrierCount,int day)
    {
        Reward reward=new Reward();
        reward.setPlayerId(ar.getPlayerId());
        reward.setAddTime(currTime);
        reward.setRewardTime(currTime);
        reward.setStatus(0);
        reward.setRewardType(RewardServices.FIGHTEND_REWARD);
        reward.getRewardInfoJson().put("b", barrierCount);
        reward.getRewardInfoJson().put("t", day);
        reward.getRewardInfoJson().put("l", level);
        reward.setRewardContentFromJson(XMLTemplateService.getFightEndRewardByLevel(level).getRewardContent().getJSONArray(String.valueOf(barrierCount)));
        DaoManager.getRewardDao().addReward(reward);
        try
        {
            if(ObjectAccessor.players.containsKey(ar.getPlayerId()))
            {
                ObjectAccessor.getOnlineUser(ar.getPlayerId()).setNeedSyncRewards(true);
            }
        }
        catch (Exception e)
        {
            log.info("exportRanking update needSyncReward fail playerId:"+ar.getPlayerId()+","+e.getMessage());
        }
    }

    /**
     * 添加玩家数据map中
     * @param player
     * @param fightStats
     * @param battleCount
     */
    private static void addRankingToMap(Player player,AdventureFightStats fightStats,int battleCount)
    {
        int playerId=player.getId();
        AdventureFightRank ar=new AdventureFightRank();
        ar.setPassBarrier(fightStats.getMaxBarrierLV());
        ar.setObtainStar(fightStats.getMaxObtainStar());
        ar.setPlayerId(playerId);
        ar.setPlayerLevel(player.getLevel());
        ar.setPlayerName(player.getName());
        ar.setBarrierCount(battleCount);
        int amount=0;
        if(yestRanking.containsKey(battleCount))
        {
            if(yestRanking.get(battleCount).containsKey(playerId))
            {
                amount=yestRanking.get(battleCount).get(playerId).getContinueInRankAmount()+1;
            }
        }
        ar.setContinueInRankAmount(amount);
        ar.setRankTime(CurrDay);
        todayRanking.get(battleCount).put(playerId,ar);
    }

    /**
     * 获取当前阵容最小得星数
     * @param battleCount
     * @return
     */
    public static int getRankingMinStarByBattleCount(int battleCount)
    {
          if(todayMinStar.containsKey(battleCount))
          {
              return todayMinStar.get(battleCount);
          }
          return 0;
    }
}