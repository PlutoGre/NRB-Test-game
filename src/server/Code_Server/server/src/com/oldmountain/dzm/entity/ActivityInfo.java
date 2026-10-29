package com.oldmountain.dzm.entity;

public class ActivityInfo {
	private int playerid;
	private String activityinfo;//活动相关数据信息
	private int laseRefreshTime;//上次刷新时间
	public int getPlayerid() {
		return playerid;
	}
	public ActivityInfo(){
		this.activityinfo="{}";
		this.laseRefreshTime=0;
	}
	public void setPlayerid(int playerid) {
		this.playerid = playerid;
	}
	public String getActivityinfo() {
		return activityinfo;
	}
	public void setActivityinfo(String activityinfo) {
		this.activityinfo = activityinfo;
	}
	public int getLaseRefreshTime() {
		return laseRefreshTime;
	}
	public void setLaseRefreshTime(int laseRefreshTime) {
		this.laseRefreshTime = laseRefreshTime;
	}
	
}
