package com.oldmountain.dzm.entity;

public class LeaguaMedal {
	private int id;//数据库存储ID
	private int medalId;//徽章id
	private int ownerLeaguaID;//徽章拥有公会的id
	private String ownerLeaguaName;//徽章拥有公会的名称
	private int count;//个数
	
	public LeaguaMedal(){
		this.ownerLeaguaID = 0;
		this.ownerLeaguaName="";
		this.count = 1;
	}
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getMedalId(){
		return medalId;
	}
	
	public void setMedalId(int id){
		this.medalId = id;
	}
	
	public int getOwnerLeaguaID(){
		return ownerLeaguaID;
	}
	
	public void setOwnerLeaguaID(int id){
		this.ownerLeaguaID = id;
	}
	
	public String getOwnerLeaguaName(){
		return ownerLeaguaName;
	}
	
	public void setOwnerLeaguaName(String name){
		this.ownerLeaguaName = name;
	}
	
	public int getCount(){
		return count;
	}
	
	public void setCount(int count){
		this.count = count;
	}
}
