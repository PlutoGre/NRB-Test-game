package com.oldmountain.dzm.configEntity;

import java.util.HashMap;
import com.oldmountain.dzm.entity.ToolContainReward;

public class WorldBossConfig {
	/**
	 * baseHealth:Boss基础血量	healthRatio:BOSS血量成长系数	maxLevel:BOSS最大等级	timeLimitMax:最长攻击间隔	timeLimitMin:最短攻击间隔	
	 * reliveCost:普通复活花费	ashesCost:浴火重生花费  	ashesAddType:浴火重生加成类型	ashesAddQuantity:浴火重生加成额度	silverCoinsBase:（排名）伤害得贝里系数	
	 * rumRewardRatio:排名得培养丹系数		bossKillRatio:最后击杀BOSS得奖励
	 * perAttackRewardNum:每次攻击获得培养药剂数量 	perAttackRewardProbability：每次攻击获得培养药剂奖励的概率 	totalRewardMax：每次活动可获得培养药剂数量上限
	 */
	private int startTime;
	private int endTime;
	private int baseHealth;
	private float healthRatio;
	private int maxLevel;
	private int timeLimitMax;
	private int timeLimitMin;
	private int reliveCost;
	private int ashesCost;
	private int ashesIncrease;
	private int ashesAddType;
	private float ashesAddQuantity;
	private float silverCoinsBase;
	private float rumRewardRatio;
	private float bossKillRatio;
	private int perAttackRewardNum;
	private int perAttackRewardProbability;
	private int totalRewardMax;
	private int startLevel;
	private int npcItemId;
	
	public int getNpcItemId(){
		return npcItemId;
	}
	
	public void setNpcItemId(int itemid){
		this.npcItemId = itemid;
	}
	
	public int getStartLevel(){
		return startLevel;
	}
	
	public void setStartLevel(int level){
		this.startLevel = level;
	}
	
	public int getTotalRewardMax(){
		return totalRewardMax;
	}
	
	public void setTotalRewardMax(int max){
		this.totalRewardMax = max;
	}
	
	public int getPerAttackRewardProbability(){
		return perAttackRewardProbability;
	}
	
	public void setPerAttackRewardProbability(int pro){
		this.perAttackRewardProbability = pro;
	}
	
	public int getPerAttackRewardNum(){
		return perAttackRewardNum;
	}
	
	public void setPerAttackRewardNum(int num){
		this.perAttackRewardNum = num;
	}
	
	public int getStartTime(){
		return startTime;
	}
	
	public void setStartTime(int time){
		this.startTime = time;
	}
	
	public int getEndTime(){
		return endTime;
	}
	
	public void setEndTime(int time){
		this.endTime = time;
	}
	
	public int getBaseHealth(){
		return baseHealth;
	}
	
	public void setBaseHealth(int baseHealth){
		this.baseHealth = baseHealth;
	}
	
	public float getHealthRatio(){
		return healthRatio;
	}
	
	public void setHealthRatio(float healthRatio){
		this.healthRatio = healthRatio;
	}
	
	public int getMaxLevel(){
		return maxLevel;
	}
	
	public void setMaxLevel(int maxLevel){
		this.maxLevel = maxLevel;
	}
	
	public int getTimeLimitMax(){
		return timeLimitMax;
	}
	
	public void setTimeLimitMax(int timeLimit){
		this.timeLimitMax = timeLimit;
	}
	
	public int getTimeLimitMin(){
		return timeLimitMin;
	}
	
	public void setTimeLimitMin(int timeLimit){
		this.timeLimitMin = timeLimit;
	}
	
	public int getReliveCost(){
		return reliveCost;
	}
	
	public void setReliveCost(int cost){
		this.reliveCost = cost;
	}
	
	public int getAshesIncrease(){
		return ashesIncrease;
	}
	
	public void setAshesIncrease(int crease){
		this.ashesIncrease = crease;
	}
	
	public int getAshesCost(){
		return ashesCost;
	}
	
	public void setAshesCost(int cost){
		this.ashesCost = cost;
	}
	
	public int getAshesAddType(){
		return ashesAddType;
	}
	
	public void setAshesAddType(int type){
		this.ashesAddType = type;
	}
	
	public float getAshesAddQuantity(){
		return ashesAddQuantity;
	}
	
	public void setAshesAddQuantity(float quantity){
		this.ashesAddQuantity = quantity;
	}
	
	public float getSilverCoinsBase(){
		return silverCoinsBase;
	}
	
	public void setSilverCoinsBase(float silverCoinsBase){
		this.silverCoinsBase = silverCoinsBase;
	}
	
	public float getRumRewardRatio(){
		return rumRewardRatio;
	}
	
	public void setRumRewardRatio(float rumRewardRatio){
		this.rumRewardRatio = rumRewardRatio;
	}
	
	public float getBossKillRatio(){
		return bossKillRatio;
	}
	
	public void setBossKillRatio(float bossKillRatio){
		this.bossKillRatio = bossKillRatio;
	}
}
