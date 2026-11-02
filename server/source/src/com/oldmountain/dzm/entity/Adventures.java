package com.oldmountain.dzm.entity;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-11
 * Time: 下午12:22
 * To change this template use File | Settings | File Templates.
 */
public class Adventures {
    private int id;
    private int timeType;
    private int levelLimit;
    private int lastTime;
    private String beginTime;
    private String endTime;
    private int version;
    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getTimeType() {
        return timeType;
    }

    public void setTimeType(int timeType) {
        this.timeType = timeType;
    }

    public int getLevelLimit() {
        return levelLimit;
    }

    public void setLevelLimit(int levelLimit) {
        this.levelLimit = levelLimit;
    }

    public int getLastTime() {
        return lastTime;
    }

    public void setLastTime(int lastTime) {
        this.lastTime = lastTime;
    }

    public String getBeginTime() {
        return beginTime;
    }

    public void setBeginTime(String beginTime) {
        this.beginTime = beginTime;
    }

    public String getEndTime() {
        return endTime;
    }

    public void setEndTime(String endTime) {
        this.endTime = endTime;
    }

    public int getVersion() {
        return version;
    }

    public void setVersion(int version) {
        this.version = version;
    }
}
