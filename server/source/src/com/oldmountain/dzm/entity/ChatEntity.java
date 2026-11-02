package com.oldmountain.dzm.entity;

public class ChatEntity {
	private int playerId;
	private int vipLevel;
	private String name;
	private String msg;
	private int sendTime;
	private int id;
	private int type;
	
	public int getPlayerId(){
		return playerId;
	}
	
	public void setPlayerId(int playerid){
		this.playerId = playerid;
	}
	
	public int getVipLevel(){
		return vipLevel;
	}
	
	public void setVipLevel(int level){
		this.vipLevel = level;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name = name;
	}
	
	public String getMsg(){
		return msg;
	}
	
	public void setMsg(String msg){
		this.msg = msg;
	}
	
	public int getSendTime(){
		return sendTime;
	}
	
	public void setSendTime(int time){
		this.sendTime = time;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getType() {
		return type;
	}

	public void setType(int type) {
		this.type = type;
	}
	
}
