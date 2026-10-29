package com.oldmountain.dzm.entity;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-4-2
 * Time: 上午10:06
 * To change this template use File | Settings | File Templates.
 */
public class PayInfo
{
    private int id;
    private int playerId;
    private int propId;
    private int payType;
    private int buyNum;
    private int originalCash;
    private int consumeCash;
    private int surplusCash;
    private int oprTime;

    public int getPayType()
    {
        return payType;
    }

    public void setPayType(int payType) {
        this.payType = payType;
    }

    public int getOriginalCash() {
        return originalCash;

    }

    public void setOriginalCash(int originalCash) {
        this.originalCash = originalCash;
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

    public int getPropId() {
        return propId;
    }

    public void setPropId(int propId) {
        this.propId = propId;
    }

    public int getBuyNum() {
        return buyNum;
    }

    public void setBuyNum(int buyNum) {
        this.buyNum = buyNum;
    }

    public int getConsumeCash() {
        return consumeCash;
    }

    public void setConsumeCash(int consumeCash) {
        this.consumeCash = consumeCash;
    }

    public int getSurplusCash() {
        return surplusCash;
    }

    public void setSurplusCash(int surplusCash) {
        this.surplusCash = surplusCash;
    }

    public int getOprTime() {
        return oprTime;
    }

    public void setOprTime(int oprTime) {
        this.oprTime = oprTime;
    }
}
