package com.oldmountain.dzm.configEntity;

import com.oldmountain.dzm.entity.ToolContainReward;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-5
 * Time: 下午12:07
 * To change this template use File | Settings | File Templates.
 */
public class FightEndGeneral
{
    private int barrier;
    private JSONObject battleSilver=new JSONObject();
    private JSONObject battlerReward=new JSONObject();

    public int getBarrier() {
        return barrier;
    }

    public void setBarrier(int barrier) {
        this.barrier = barrier;
    }

    public void setBattleSilver(int key,int v)
    {
        this.battleSilver.put(key,v);
    }

    public int getBattlerSilverByBattle(int battle)
    {
        int _v=0;
        if(battleSilver.containsKey(String.valueOf(battle)))
        {
            _v=battleSilver.getInt(String.valueOf(battle));
        }
        return _v;
    }

    public JSONArray getExtraBattleReward(int star)
    {
        return battlerReward.getJSONArray(String.valueOf(star));
    }

    public void setBattlerReward(int key,String reward)
    {
        this.battlerReward.put(key,setContainTools(reward));
    }

    public ToolContainReward[] setContainTools(String s)
    {
        ToolContainReward [] rewards=new ToolContainReward[0];
        if(!("none").equals(s))
        {
            String[] arrTools = s.split(",");
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
        return rewards.clone();
    }
}
