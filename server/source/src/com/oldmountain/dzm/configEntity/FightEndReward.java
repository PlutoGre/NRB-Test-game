package com.oldmountain.dzm.configEntity;

import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-23
 * Time: 下午12:53
 * To change this template use File | Settings | File Templates.
 */
public class FightEndReward
{
    private int level;
    private JSONObject rewardContent;

    public int getLevel()
    {
        return level;
    }

    public void setLevel(int level)
    {
        this.level = level;
    }

    public JSONObject getRewardContent()
    {
        return rewardContent;
    }

    public void setRewardContent(JSONObject rewardContent)
    {
        this.rewardContent = rewardContent;
    }
}
