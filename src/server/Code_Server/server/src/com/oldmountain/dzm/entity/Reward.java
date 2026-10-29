package com.oldmountain.dzm.entity;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-23
 * Time: 上午11:30
 * To change this template use File | Settings | File Templates.
 */
public class Reward
{
    private int id;
    private int playerId;
    private int rewardType;
    private StringBuffer rewardMsg;
    private JSONObject rewardInfo;
    private JSONArray rewardContent;
    private int status;
    private int addTime;
    private int rewardTime;

    public Reward()
    {
        this.rewardMsg=new StringBuffer();
        this.rewardInfo=new JSONObject();
        this.rewardContent=new JSONArray();
        this.status=0;
    }
    public int getId()
    {
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

    public int getRewardType() {
        return rewardType;
    }

    public void setRewardType(int rewardType) {
        this.rewardType = rewardType;
    }

    public String getRewardMsg()
    {
        return rewardMsg.toString();
    }

    public void setRewardMsg(String rewardMsg)
    {
        this.rewardMsg =new StringBuffer(rewardMsg);
    }

    public String getRewardInfo()
    {
        return rewardInfo.toString();
    }

    public void setRewardInfo(String rewardInfo)
    {
        this.rewardInfo =JSONObject.fromObject(rewardInfo);
    }

    public JSONObject getRewardInfoJson()
    {
        return this.rewardInfo;
    }

    public void setRewardInfoFromJson(JSONObject rewardInfo)
    {
        this.rewardInfo =rewardInfo;
    }

    public String getRewardContent()
    {
        return rewardContent.toString();
    }

    public JSONArray getRewardContentJson()
    {
        return rewardContent;
    }

    public void setRewardContent(String rewardContent)
    {
        this.rewardContent = JSONArray.fromObject(rewardContent);
    }

    public void setRewardContentFromJson(JSONArray content)
    {
        this.rewardContent=content;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public int getAddTime() {
        return addTime;
    }

    public void setAddTime(int addTime) {
        this.addTime = addTime;
    }

    public int getRewardTime() {
        return rewardTime;
    }

    public void setRewardTime(int rewardTime) {
        this.rewardTime = rewardTime;
    }
}
