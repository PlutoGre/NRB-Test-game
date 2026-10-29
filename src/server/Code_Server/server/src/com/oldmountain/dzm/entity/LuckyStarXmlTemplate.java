package com.oldmountain.dzm.entity;

public class LuckyStarXmlTemplate {
	private int id;		
	private int itemId;	//幸运星的itemId
	private int maxDropCount;	//航行中最大掉落次数
	private int balanceTimes;	//保底必中的抽取次数
	private int balanceType;	//保底必中的物品类型
	private int balanceId;		//保底必中的物品id
	private int treasureType;	//宝箱抽取的类型
	
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}
	public int getItemId() {
		return itemId;
	}
	public void setItemId(int itemId) {
		this.itemId = itemId;
	}
	public int getMaxDropCount() {
		return maxDropCount;
	}
	public void setMaxDropCount(int maxDropCount) {
		this.maxDropCount = maxDropCount;
	}
	public int getBalanceTimes() {
		return balanceTimes;
	}
	public void setBalanceTimes(int balanceTimes) {
		this.balanceTimes = balanceTimes;
	}
	public int getBalanceType() {
		return balanceType;
	}
	public void setBalanceType(int balanceType) {
		this.balanceType = balanceType;
	}
	public int getBalanceId() {
		return balanceId;
	}
	public void setBalanceId(int balanceId) {
		this.balanceId = balanceId;
	}
	public int getTreasureType() {
		return treasureType;
	}
	public void setTreasureType(int treasureType) {
		this.treasureType = treasureType;
	}
	
}
