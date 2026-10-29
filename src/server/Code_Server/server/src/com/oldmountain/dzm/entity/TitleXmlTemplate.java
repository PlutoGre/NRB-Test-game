package com.oldmountain.dzm.entity;

public class TitleXmlTemplate {
	private int id;
	private int type;
	private int addType;
	private int timeLine;
	private int require;
	private float quantity;
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getType(){
		return type;
	}
	
	public void setType(int type){
		this.type = type;
	}
	
	public int getAddType(){
		return addType;
	}
	
	public void setAddType(int type){
		this.addType = type;
	}
	
	public int getTimeLine(){
		return timeLine;
	}
	
	public void setTimeLine(int time){
		this.timeLine = time;
	}
	
	public int getRequire(){
		return require;
	}
	
	public void setRequire(int require){
		this.require = require;
	}
	
	public float getQuantity(){
		return quantity;
	}
	
	public void setQuantity(float quantity){
		this.quantity = quantity;
	}
}
