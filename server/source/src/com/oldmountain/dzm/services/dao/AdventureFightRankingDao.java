package com.oldmountain.dzm.services.dao;

import com.oldmountain.dzm.entity.AdventureFightRank;
import com.oldmountain.dzm.entity.AdventureFightStats;
import com.oldmountain.dzm.entity.Player;

import java.util.List;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-3
 * Time: 下午12:23
 * To change this template use File | Settings | File Templates.
 */
public interface AdventureFightRankingDao
{
    public List<AdventureFightRank> getFightRanking(int time);
    public int addFightRanking(AdventureFightRank rank);
    public int delFightRanking(int barrierCount,int time);
    public boolean syncStatsToRank(int barrierCount,int startTime,int endTime,int rankTime);
    public List<AdventureFightRank> getFightRankingFromStats(int barrierCount,int startTime,int endTime,int rankTime);
}
