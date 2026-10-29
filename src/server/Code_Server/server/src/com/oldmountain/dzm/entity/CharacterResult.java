package com.oldmountain.dzm.entity;

public class CharacterResult {
	private int itemid;//人物在xml中定义的ID
	private int gainExp;//这次战斗获得的经验值
	private int level;//人物等级
	private boolean isUpgarade;//是否升级
	
	public CharacterResult(){
		this.itemid = this.gainExp = this.level = 0;
		this.isUpgarade = false;
	}
	
	public int getItemid(){
		return itemid;
	}
	
	public void setItemid(int itemid){
		this.itemid = itemid;
	}
	
	public int getGainExp(){
		return gainExp;
	}
	
	public void setGainExp(int exp){
		this.gainExp = exp;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public boolean getIsUpgrade(){
		return isUpgarade;
	}
	
	public void setIsUpgrade(boolean upgrade){
		this.isUpgarade = upgrade;
	}
}
