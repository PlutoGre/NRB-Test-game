package com.oldmountain.dzm.entity;

public class LeaguaApply {
	private int id;//服务器id
	private int playerId;//发送申请的用户ID
	private String playerName;//发送申请的用户姓名
	private int playerLevel;//发送申请的用户等级
	private int applyTime;//发送申请的时间
	private int applyLeaguaId;//发送给的联盟ID
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getPlayerId(){
		return playerId;
	}
	
	public void setPlayerId(int id){
		this.playerId = id;
	}
	
	public String getPlayerName(){
		return playerName;
	}
	
	public void setPlayerName(String name){
		this.playerName = name;
	}
	
	public int getPlayerLevel(){
		return playerLevel;
	}
	
	public void setPlayerLevel(int level){
		this.playerLevel = level;
	}
	
	public int getApplyTime(){
		return applyTime;
	}
	
	public void setApplyTime(int time){
		this.applyTime = time;
	}
	
	public int getApplyLeaguaId(){
		return applyLeaguaId;
	}
	
	public void setApplyLeaguaId(int id){
		this.applyLeaguaId = id;
	}
}
