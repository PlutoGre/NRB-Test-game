package com.oldmountain.dzm.entity;

public class PlayerFrom {
	private String puid;
	private int playerid;
	private String platform;
	private String deviceid;
	private int createtime;
	
	public String getPuid(){
		return puid;
	}
	
	public void setPuid(String puid){
		this.puid = puid;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public String getPlatform(){
		return platform;
	}
	
	public void setPlatform(String platform){
		this.platform = platform;
	}
	
	public String getDeviceid(){
		return deviceid;
	}
	
	public void setDeviceid(String id){
		this.deviceid = id;
	}
	
	public int getCreatetime(){
		return createtime;
	}
	
	public void setCreatetime(int time){
		this.createtime = time;
	}
}
