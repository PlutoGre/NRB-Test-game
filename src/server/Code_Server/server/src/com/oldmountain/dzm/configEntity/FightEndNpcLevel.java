package com.oldmountain.dzm.configEntity;

import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-5
 * Time: 下午12:07
 * To change this template use File | Settings | File Templates.
 */
public class FightEndNpcLevel
{
    private int barrier;
    JSONObject battleLevel=new JSONObject();

    public int getBarrier() {
        return barrier;
    }

    public void setBarrier(int barrier)
    {
        this.barrier = barrier;
    }

    public void setBattleLevel(int key,int level)
    {
        battleLevel.put(key,level);
    }

    public int getBattleLevel(String key)
    {
         return Integer.parseInt(battleLevel.get(key).toString());
    }

}
