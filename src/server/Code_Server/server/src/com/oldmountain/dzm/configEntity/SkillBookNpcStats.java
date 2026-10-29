package com.oldmountain.dzm.configEntity;

import com.oldmountain.dzm.entity.BattleArrayObject;
import com.oldmountain.dzm.services.SkillBookServices;

import java.util.ArrayList;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-10
 * Time: 上午10:09
 * To change this template use File | Settings | File Templates.
 */
public class SkillBookNpcStats
{
    private int ceilLevel;
    private int floorLevel;
    private int currLevel;
    private ArrayList randomNpcArr;
    private BattleArrayObject lastNpcBattle;
    private int lastRandomIndex;
    private int lastSkillBookId;
    public SkillBookNpcStats()
    {
        this.ceilLevel = -1;
        this.floorLevel = -1;
        this.currLevel = -1;
        this.lastSkillBookId=-1;
        this.lastRandomIndex=-1;
        this.randomNpcArr =new ArrayList();
    }

    public int getCeilLevel()
    {
        return ceilLevel;
    }

    public void setCeilLevel(int ceilLevel)
    {
        this.ceilLevel = ceilLevel;
    }

    public int getFloorLevel()
    {
        return floorLevel;
    }

    public void setFloorLevel(int floorLevel)
    {
        this.floorLevel = floorLevel;
    }

    public int getCurrLevel()
    {
        return currLevel;
    }

    public void setCurrLevel(int currLevel)
    {
        this.currLevel = currLevel;
    }

    public ArrayList getRandomNpcArr() {
        return randomNpcArr;
    }

    public BattleArrayObject getLastNpcBattle() {
        return lastNpcBattle;
    }

    public void setLastNpcBattle(BattleArrayObject lastNpcBattle) {
        this.lastNpcBattle = lastNpcBattle;
    }

    public void setRandomNpcArr(ArrayList randomNpcArr)
    {
        this.randomNpcArr = randomNpcArr;
        this.lastNpcBattle=null;
    }

    public int getLastSkillBookId()
    {
        return lastSkillBookId;
    }

    public void setLastSkillBookId(int lastSkillBookId)
    {
        if(this.lastSkillBookId==-1||this.lastSkillBookId!=lastSkillBookId)
        {
            lastRandomIndex=0;
        }
        this.lastSkillBookId = lastSkillBookId;
    }

    public int getLastRandomIndex()
    {
        return lastRandomIndex;
    }

    public void setLastRandomIndex(int lastRandomIndex)
    {
        this.lastRandomIndex = lastRandomIndex;
    }

    private void resetParams(int playerLevel)
    {
        this.currLevel=playerLevel;
        this.ceilLevel=playerLevel+SkillBookServices.getNpcConfig().getCeilCaptainLevel();
        this.ceilLevel=this.ceilLevel<=0?1:this.ceilLevel;
        this.floorLevel=playerLevel+SkillBookServices.getNpcConfig().getFloorCaptainLevel()-1;
        this.floorLevel=this.floorLevel<=0?1:this.floorLevel;
    }

    public void checkLevel(int playerLevel)
    {
        if(this.ceilLevel==-1||this.floorLevel==-1||this.currLevel!=playerLevel)
        {
            resetParams(playerLevel);
        }
    }

    public int getRandomLevel()
    {
        return this.ceilLevel-this.floorLevel;
    }



}
