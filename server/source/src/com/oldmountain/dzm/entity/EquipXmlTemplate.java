package com.oldmountain.dzm.entity;

public class EquipXmlTemplate {
	private int id;
	private int type;//装备的类型:1是武器，2是防具，3是饰品
	private int quality;//装备的品质:1是甲，2是乙，3是丙，4是丁
	private float quantity;//装备提供的初始加成值
	private float growth;//装备的成长值
	
	public int getId() {
		return id;
	}
	
	public void setId(int id) {
		this.id = id;
	}
	
	public int getType(){
		return type;
	}
	
	public void setType(int type){
		this.type = type;
	}
	
	public int getQuality() {
		return quality;
	}
	
	public void setQuality(int quality) {
		this.quality = quality;
	}
	
	public float getQuantity(){
		return quantity;
	}
	
	public void setQuantity(float quantity){
		this.quantity = quantity;
	}
	
	public float getGrowth(){
		return growth;
	}
	
	public void setGrowth(float growth){
		this.growth = growth;
	}
}
