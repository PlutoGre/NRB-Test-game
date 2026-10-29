package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class DeepWaterSelecter {
	private List<Integer> id;
	private List<Integer> typeId;
	private List<Integer> rewardType;
	private List<Integer> itemId;
	private List<Integer> count;
	private List<Integer> rate;
	private List<Integer> lvl;
	public DeepWaterSelecter(){
		this.id=new ArrayList<Integer>();
		this.typeId=new ArrayList<Integer>();
		this.rewardType=new ArrayList<Integer>();
		this.itemId=new ArrayList<Integer>();
		this.count=new ArrayList<Integer>();
		this.rate=new ArrayList<Integer>();
		this.lvl=new ArrayList<Integer>();
	}
	
	public List<Integer> getId() {
		return id;
	}
	public void setId(List<Integer> id) {
		this.id = id;
	}
	public List<Integer> getRewardType() {
		return rewardType;
	}
	public void setRewardType(List<Integer> rewardType) {
		this.rewardType = rewardType;
	}
	public List<Integer> getItemId() {
		return itemId;
	}
	public void setItemId(List<Integer> itemId) {
		this.itemId = itemId;
	}
	public List<Integer> getCount() {
		return count;
	}
	public void setCount(List<Integer> count) {
		this.count = count;
	}
	public List<Integer> getRate() {
		return rate;
	}
	public void setRate(List<Integer> rate) {
		this.rate = rate;
	}

	public List<Integer> getTypeId() {
		return typeId;
	}

	public void setTypeId(List<Integer> typeId) {
		this.typeId = typeId;
	}

	public List<Integer> getLvl() {
		return lvl;
	}

	public void setLvl(List<Integer> lvl) {
		this.lvl = lvl;
	}
	
	
}
