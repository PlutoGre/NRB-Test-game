package com.oldmountain.dzm.configEntity;

import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-5
 * Time: 下午12:07
 * To change this template use File | Settings | File Templates.
 */
public class FightEndNpc
{
    private int id;
    private int battle;
    private int pos_index;
    private JSONObject addition=new JSONObject();
    private int skill_rate;
    private String[] npcArr;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getBattle() {
        return battle;
    }

    public void setBattle(int battle) {
        this.battle = battle;
    }

    public int getPos_index() {
        return pos_index;
    }

    public void setPos_index(int pos_index) {
        this.pos_index = pos_index;
    }


    public void setAddition(int key,float v)
    {
        addition.put(key,v);
    }

    public float getAdditionByType(int key)
    {
        float _v=0.0f;
        try
        {
            _v= (float) addition.getDouble(String.valueOf(key));
        }
        catch (Exception e)
        {
            _v=0.0f;
        }
        return _v;
    }

    public int getSkill_rate() {
        return skill_rate;
    }

    public void setSkill_rate(int skill_rate) {
        this.skill_rate = skill_rate;
    }

    public String[] getNpcArr()
    {
        return npcArr;
    }

    public void setNpcArr(String npcStr)
    {
        this.npcArr = npcStr.split(",");
    }

    public String[] getNpcByLimit(int limit)
    {
        String[] _arr=new String[limit];
        System.arraycopy(this.npcArr,0,_arr,0,limit);
        return _arr;
    }
}
