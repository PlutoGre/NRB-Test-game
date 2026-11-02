package com.oldmountain.dzm.entity;

public class Soul{
	private int id;
	private int itemid;
	private int playerid;
	private int count;
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getItemid(){
		return itemid;
	}
	
	public void setItemid(int itemid){
		this.itemid = itemid;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerId){
		this.playerid = playerId;
	}
	
	public int getCount(){
		return count;
	}
	
	public void setCount(int count){
		this.count = count;
	}
}
