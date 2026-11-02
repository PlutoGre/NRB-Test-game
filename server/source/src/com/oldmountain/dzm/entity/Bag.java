package com.oldmountain.dzm.entity;

public class Bag{
	private int id;//物品在包裹中的id
	private int playerid;//所属的玩家id
	private int itemid;//物品的类型id
	private int count;//物品的个数
	private int source;//物品的来源
	private int createtime;//物品的创建时间
	
	public Bag(){
		this.count = 0;
		this.source = 0;
	}
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getItemid(){
		return itemid;
	}
	
	public void setItemid(int itemid){
		this.itemid = itemid;
	}
	
	public int getCount(){
		return count;
	}
	
	public void setCount(int count){
		this.count = count;
	}
	
	public int getSource(){
		return source;
	}
	
	public void setSource(int source){
		this.source = source;
	}
	
	public int getCreatetime(){
		return createtime;
	}
	
	public void setCreatetime(int time){
		this.createtime = time;
	}
}
