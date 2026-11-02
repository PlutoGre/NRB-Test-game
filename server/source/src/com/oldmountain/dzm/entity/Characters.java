package com.oldmountain.dzm.entity;

public class Characters {
	private int itemid;//人物的配置Id
	private int life;//人物的血量
	private int power;//人物的内力值
	private int position;//人物在阵容中的位置
	private int level;//人物的等级
	
	public int getItemid(){
		return itemid;
	}
	
	public void  setItemid(int itemid) {
		this.itemid = itemid;
	}
	
	public int getLife(){
		return life;
	}
	
	public void setLife(int life){
		this.life = life;
	}
	
	public int getPower(){
		return power;
	}
	
	public void setPower(int power){
		this.power = power;
	}
	
	public int getPosition(){
		return position;
	}
	
	public void setPosition(int position){
		this.position = position;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
}
