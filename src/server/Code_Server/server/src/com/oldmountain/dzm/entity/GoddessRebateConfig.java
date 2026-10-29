package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class GoddessRebateConfig {
	private int id;	//女神id
	private int price;	//亲密度价格
	private int needTimes; //女神回馈亲密度要求
	private ArrayList<int[]> rewardList;	//奖励
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}
	public int getPrice() {
		return price;
	}
	public void setPrice(int price) {
		this.price = price;
	}
	public int getNeedTimes() {
		return needTimes;
	}
	public void setNeedTimes(int needTimes) {
		this.needTimes = needTimes;
	}
	public ArrayList<int[]> getRewardList() {
		return rewardList;
	}
	public void setRewardList(ArrayList<int[]> rewardList) {
		this.rewardList = rewardList;
	}
	
	
}
