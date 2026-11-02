package com.oldmountain.dzm.entity;

public class Cdkey {
	private String cd_key;	//cdkey
	private int status;		//使用状态 0:未使用  1:已使用
	private int create_time;
	private int used_time;
	private int playerid;
	public String getCd_key() {
		return cd_key;
	}
	public void setCd_key(String cdKey) {
		cd_key = cdKey;
	}
	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	public int getCreate_time() {
		return create_time;
	}
	public void setCreate_time(int createTime) {
		create_time = createTime;
	}
	public int getUsed_time() {
		return used_time;
	}
	public void setUsed_time(int usedTime) {
		used_time = usedTime;
	}
	public int getPlayerid() {
		return playerid;
	}
	public void setPlayerid(int playerid) {
		this.playerid = playerid;
	}
	
	
}
