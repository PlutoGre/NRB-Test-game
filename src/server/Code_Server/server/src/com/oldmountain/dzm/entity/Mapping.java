package com.oldmountain.dzm.entity;

public class Mapping{
	private String puid;
	private int gameid;
	private String name;
	private String passwd;
	
	public String getPuid(){
		return puid;
	}
	
	public void setPuid(String puid){
		this.puid = puid;
	}
	
	public int getGameid(){
		return gameid;
	}
	
	public void setGameid(int id){
		this.gameid = id;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name = name;
	}
	
	public String getPasswd(){
		return passwd;
	}
	
	public void setPasswd(String passwd){
		this.passwd = passwd;
	}
}
