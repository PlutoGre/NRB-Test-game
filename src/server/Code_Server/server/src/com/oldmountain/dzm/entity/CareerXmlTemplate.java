package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class CareerXmlTemplate {
	private int id;//关卡在配置文件中的id
	private int chapterId;//关卡所属章节ID
	private Boolean isOpen;//关卡是否开启
	private int boosType;//boss类型
	private int costPower;//消耗体力
	private int countLimit;//每日攻打次数
	private int gainExp;//胜利获得的经验
	private int gainCoins;//胜利获得的银币
	private int unlockId;//解锁下一关的配置ID
	ArrayList<int[]> dropItems;//过关奖励物品；Integer【】，0为奖励物品类型：1为技能，2为装备；1为物品ID；2为奖励数量
	ArrayList<Integer> dropRates;//掉落物品的概率
	ArrayList<Float> skillRates;//关卡NPC释放技能倍数
	
	private BattleArrayObject battleInfo;//关卡阵容配置
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getChapterId(){
		return chapterId;
	}
	
	public void setChapterId(int id){
		this.chapterId = id;
	}
	
	public boolean getIsOpen(){
		return isOpen;
	}
	
	public void setIsOpen(boolean isOpen){
		this.isOpen = isOpen;
	}
	
	public int getBoosType(){
		return boosType;
	}
	
	public void setBoosType(int type){
		this.boosType = type;
	}
	
	public int getCostPower(){
		return costPower;
	}
	
	public void setCostPower(int costPower){
		this.costPower = costPower;
	}
	
	public int getCountLimit(){
		return countLimit;
	}
	
	public void setCountLimit(int limit){
		this.countLimit = limit;
	}
	
	public int getGainExp(){
		return gainExp;
	}
	
	public void setGainExp(int gainExp){
		this.gainExp = gainExp;
	}
	
	public int getGainCoins(){
		return gainCoins;
	}
	
	public void setGainCoins(int gainCoins){
		this.gainCoins = gainCoins;
	}
	
	public int getUnlockId(){
		return unlockId;
	}
	
	public void setUnlockId(int unlockId){
		this.unlockId = unlockId;
	}
	
	public BattleArrayObject getBattleInfo(){
		return battleInfo;
	}
	
	public void setBattleInfo(BattleArrayObject battleInfo){
		this.battleInfo = battleInfo;
	}
	
	public ArrayList<int[]> getDropItems(){
		return dropItems;
	}
	
	public void setDropItems(ArrayList<int[]> dropItems){
		this.dropItems = dropItems;
	}
	
	public ArrayList<Integer> getDropRates(){
		return dropRates;
	}
	
	public void setDropRates(ArrayList<Integer> dropRates){
		this.dropRates = dropRates;
	}
	
	public ArrayList<Float> getSkillRates(){
		return skillRates;
	}
	
	public void setSkillRates(ArrayList<Float> skillRates){
		this.skillRates = skillRates;
	}
}
