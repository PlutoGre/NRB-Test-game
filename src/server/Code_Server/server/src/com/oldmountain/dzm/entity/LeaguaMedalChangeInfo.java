package com.oldmountain.dzm.entity;

public class LeaguaMedalChangeInfo {
	private int id;//数据库Id
	private int medalId;//发生变化的徽章id
	private String beforeOwnerLeaguaName;//被掠夺的公会名称
	private String afterOwnerLeaguaName;//进行掠夺的公会名称
	private int changeTime;//掠夺的时间
	
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
	
	public String getBeforeOwnerLeaguaName(){
		return beforeOwnerLeaguaName;
	}
	
	public void setBeforeOwnerLeaguaName(String name){
		this.beforeOwnerLeaguaName = name;
	}
	
	public String getAfterOwnerLeaguaName(){
		return afterOwnerLeaguaName;
	}
	
	public void setAfterOwnerLeaguaName(String name){
		this.afterOwnerLeaguaName = name;
	}
	
	public int getChangeTime(){
		return changeTime;
	}
	
	public void setChangeTime(int time){
		this.changeTime = time;
	}
}