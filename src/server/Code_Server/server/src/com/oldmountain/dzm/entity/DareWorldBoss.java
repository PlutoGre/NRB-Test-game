package com.oldmountain.dzm.entity;

public class DareWorldBoss {
	private int playerid;//玩家id
	private int attackcount;//玩家攻击次数
	private int attackamount;//玩家攻击总血量
	private int ashestimes;//浴火重生次数
	private int gainsilvercoins;//总获得的银两
	private int lastverifytime;//上次更新时间
	private int hasGotRewardCount;//已经获取的培养药剂的个数
	
	
	public DareWorldBoss(){
		this.attackcount = 0;
		this.attackamount = 0;
		this.ashestimes = 0;
		this.gainsilvercoins = 0;
		this.hasGotRewardCount = 0;
	}
	
	public int getHasGotRewardCount(){
		return hasGotRewardCount;
	}
	
	public void setHasGotRewardCount(int count){
		this.hasGotRewardCount = count;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int id){
		this.playerid = id;
	}
	
	public int getAttackcount(){
		return attackcount;
	}
	
	public void setAttackcount(int count){
		this.attackcount = count;
	}
	
	public void addAttackCount(int count){
		this.attackcount += count;
	}
	
	public int getAttackamount(){
		return attackamount;
	}
	
	public void setAttackamount(int amount){
		this.attackamount = amount;
	}
	
	public void addAttackAmount(int amount){
		this.attackamount += amount;
	}
	
	public int getAshestimes(){
		return ashestimes;
	}
	
	public void setAshestimes(int times){
		this.ashestimes = times;
	}
	
	public void addAshestimes(int times){
		this.ashestimes += times;
	}
	
	public int getGainsilvercoins(){
		return gainsilvercoins;
	}
	
	public void setGainsilvercoins(int gaincoins){
		this.gainsilvercoins = gaincoins;
	}
	
	public void addGainSilverCoins(int coins){
		this.gainsilvercoins += coins;
	}
	
	public int getLastverifytime(){
		return lastverifytime;
	}
	
	public void setLastverifytime(int time){
		this.lastverifytime = time;
	}
}
