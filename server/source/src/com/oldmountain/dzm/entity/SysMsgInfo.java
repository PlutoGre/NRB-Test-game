package com.oldmountain.dzm.entity;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-13
 * Time: 下午9:03
 * To change this template use File | Settings | File Templates.
 */
public class SysMsgInfo
{
    private int id;
    private int playerId;
    private int msgType;
    private JSONObject msgInfo;
    private String msgContent;
    private int status;
    private int addTime;

    public SysMsgInfo()
    {
        this.msgContent="";
        this.msgInfo = new JSONObject();
    }

    public int getId()
    {
        return id;
    }

    public void setId(int id)
    {
        this.id = id;
    }

    public int getPlayerId()
    {
        return playerId;
    }

    public void setPlayerId(int playerId)
    {
        this.playerId = playerId;
    }

    public int getMsgType() {
        return msgType;
    }

    public void setMsgType(int msgType) {
        this.msgType = msgType;
    }

    public JSONObject getMsgInfoJson()
    {
        return msgInfo;
    }

    public String getMsgInfo()
    {
        return msgInfo.toString();
    }

    public void setMsgInfo(String msgInfo)
    {
        this.msgInfo = JSONObject.fromObject(msgInfo);
    }

    public void setMsgInfoFromJson(JSONObject msgInfo)
    {
        this.msgInfo = msgInfo;
    }


    public String getMsgContent()
    {
        return msgContent;
    }

    public void setMsgContent(String msgContent)
    {
        this.msgContent = msgContent;
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

    public void setAddTime(int addTime)
    {
        this.addTime = addTime;
    }
}
