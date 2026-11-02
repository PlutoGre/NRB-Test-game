package com.oldmountain.dzm.services.dao;

import java.util.List;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import com.oldmountain.dzm.entity.Player;
import com.oldmountain.dzm.entity.Reward;

/*
 * Created with IntelliJ IDEA.
 * User: lvjc
 * Date: 13-4-4
 * Time: 下午5:01
 * To change this template use File | Settings | File Templates.
 */
public interface RewardDao
{
    public List<Reward> getReward(Player player,int currTime);
    public List<Reward> getRewardNotDeal(Player player);
    public int addReward(Reward reward);
    public boolean deleteRewardInvalid(int currTime);
    public boolean createCompensateReward(String rewardMsg, JSONObject rewardInfo,JSONArray rewardContent,int status,int playerId,int lastLogin,int levelStart,int levelEnd,int rankStart,int rankEnd,String channel);
}
