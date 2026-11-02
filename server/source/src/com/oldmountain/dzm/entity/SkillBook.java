package com.oldmountain.dzm.entity;

import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-8
 * Time: 下午2:00
 * To change this template use File | Settings | File Templates.
 */
public class SkillBook
{
    private int id;
    private int playerId;
    private int skillId;
    private int pieceChance;
    private JSONObject skillBookDetail;

    public SkillBook()
    {
        this.pieceChance = 0;
        this.skillBookDetail = new JSONObject();
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getPlayerId() {
        return playerId;
    }

    public void setPlayerId(int playerId) {
        this.playerId = playerId;
    }

    public int getSkillId() {
        return skillId;
    }

    public void setSkillId(int skillId) {
        this.skillId = skillId;
    }

    public int getPieceChance() {
        return pieceChance;
    }

    public void setPieceChance(int pieceChance)
    {
        this.pieceChance = pieceChance;
    }

    public JSONObject getSkillBookDetailForJson()
    {
        return skillBookDetail;
    }

    public String getSkillBookDetail()
    {
        return skillBookDetail.toString();
    }

    public void setSkillBookDetail(String skillBookDetail)
    {
        this.skillBookDetail = JSONObject.fromObject(skillBookDetail);
    }

    public void setSkillBookDetailFromJson(JSONObject skillBookDetail)
    {
        this.skillBookDetail = skillBookDetail;
    }

    /**
     * 根据残章编号获取数量
     * @param skillBookId
     */
    public int getSkillBookCountBySkillBookId(int skillBookId)
    {
        int count=0;
        try
        {
            if(this.skillBookDetail.containsKey(String.valueOf(skillBookId)))
            {
                count=this.skillBookDetail.getInt(String.valueOf(skillBookId));
            }
        }
        catch (Exception e)
        {
            count=0;
        }
        return count;
    }

    public void changePartCountByBookId(int skillBookId, int _count)
    {
        int count=0;
        try
        {
            if(this.skillBookDetail.containsKey(String.valueOf(skillBookId)))
            {
                count=this.skillBookDetail.getInt(String.valueOf(skillBookId));
            }
        }
        catch (Exception e)
        {
            count=0;
        }
        count+=_count;
        if(count<0)
        {
            count=0;
        }
        else
        {
           if(count>this.pieceChance)
           {
               this.pieceChance=count;
           }
        }
        this.skillBookDetail.put(skillBookId,count);
    }

    public void changePieceChanceByCount(int count)
    {
        this.pieceChance+=count;
        this.pieceChance=this.pieceChance<0?0:this.pieceChance;
    }

}
