package com.oldmountain.dzm.entity;

public class Damage {
	private int position;//受到伤害的位置
	private int harmValue;//伤害的值
	
	public int getPosition(){
		return position;
	}
	
	public void setPosition(int position){
		this.position = position;
	}
	
	public int getHarmValue(){
		return harmValue;
	}
	
	public void setHarmValue(int value){
		this.harmValue = value;
	}
}
