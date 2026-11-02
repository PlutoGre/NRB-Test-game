package com.oldmountain.dzm.entity;

public class FateXmlTemplate {
	private int id;//机缘的id
	private int type;//机缘的触发类型：1与他人同上阵，2为佩戴指定装备，3为装配指定技能
	private String require;//触发所需的条件
	private int upgradeType;//触发提升的类型,1提升血量，2提升攻击力，3提升防御力，4提升内力
	private float quantity;//触发提升的比率
	
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
	
	public String getRequire(){
		return require;
	}
	
	public void setRequire(String require){
		this.require = require;
	}
	
	public int getUpgradeType(){
		return upgradeType;
	}
	
	public void setUpgradeType(int type){
		this.upgradeType = type;
	}
	
	public float getQuantity(){
		return quantity;
	}
	
	public void setQuantity(float quantity){
		this.quantity = quantity;
	}
}
