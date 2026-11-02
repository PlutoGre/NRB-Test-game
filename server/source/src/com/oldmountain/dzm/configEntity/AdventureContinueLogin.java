package com.oldmountain.dzm.configEntity;

import com.oldmountain.dzm.entity.ToolContainReward;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-5
 * Time: 下午12:20
 * To change this template use File | Settings | File Templates.
 */
public class AdventureContinueLogin
{
    private int id;
    private int dayCount;
    private ToolContainReward[] rewards;

    public int getId()
    {
        return id;
    }

    public void setId(int id)
    {
        this.id = id;
    }

    public int getDayCount() {
        return dayCount;
    }

    public void setDayCount(int dayCount) {
        this.dayCount = dayCount;
    }

    public ToolContainReward[] getRewards()
    {
        return rewards.clone();
    }

    public void setRewards(String rewardsStr)
    {
        rewards=new ToolContainReward[0];
        if(!("none").equals(rewardsStr))
        {
            String[] arrTools = rewardsStr.split(",");
            int arrLen = arrTools.length;
            rewards = new ToolContainReward[arrLen];
            try
            {
                for(int i=0;i<arrLen;i++)
                {
                    String[] arrTmp =  arrTools[i].split(":");
                    rewards[i] = new ToolContainReward();
                    rewards[i].setType(Integer.parseInt(arrTmp[0]));
                    rewards[i].setItemId(Integer.parseInt(arrTmp[1]));
                    rewards[i].setQuantity(Integer.parseInt(arrTmp[2]));
                }
            }
            catch (Exception e)
            {
                System.out.println(e.getMessage());
            }
        }
    }
}
