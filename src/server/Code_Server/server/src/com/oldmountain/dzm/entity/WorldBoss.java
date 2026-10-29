package com.oldmountain.dzm.entity;

import java.util.HashMap;

public class WorldBoss {
	private int id;
	private long totalHealth;//世界boss的总血量
	private long leftHealth;//世界boss的剩余血量
	private int level;//世界boss的等级
	private int killedTime;//世界boss被击杀的时间
	private int lastRefreshTime;//上次刷新时间
	private int killerId;//最终击杀玩家id
	private String killerName;//最终击杀玩家姓名
	private int killerLevel;//最终击杀玩家等级
	private int killerDamage;//最终击杀玩家总伤害
	private int date;//世界boss的日期
	private HashMap<Integer, WorldBossRank> currentRank;//当前破阵排名
	
	public WorldBoss(){
		this.totalHealth = 0;
		this.leftHealth = 0;
		this.level = 0;
		this.killedTime = 0;
		this.lastRefreshTime = 0;
		this.currentRank = new HashMap<Integer, WorldBossRank>();
		this.killerId = 0;
		this.killerName = "";
		this.killerLevel = 0;
		this.killerDamage = 0;
	}
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public long getTotalHealth(){
		return totalHealth;
	}
	
	public void setTotalHealth(long health){
		this.totalHealth = health;
	}
	
	public long getleftHealth(){
		return leftHealth;
	}
	
	synchronized public void setleftHealth(long health){
		this.leftHealth = health;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getKilledTime(){
		return killedTime;
	}
	
	public void setKilledTime(int time){
		this.killedTime = time;
	}
	
	public int getLastRefreshTime(){
		return lastRefreshTime;
	}
	
	public void setLastRefreshTime(int time){
		this.lastRefreshTime = time;
	}
	
	public int getKillerId(){
		return killerId;
	}

	public void setKillerId(int id){
		this.killerId = id;
	}
	
	public String getKillerName(){
		return killerName;
	}
	
	public void setKillerName(String name){
		this.killerName = name;
	}
	
	public int getKillerLevel(){
		return killerLevel;
	}
	
	public void setKillerLevel(int level){
		this.killerLevel = level;
	}
	
	public int getKillerDamage(){
		return killerDamage;
	}
	
	public void setKillerDamage(int damage){
		this.killerDamage = damage;
	}
	
	public int getDate(){
		return date;
	}
	
	public void setDate(int date){
		this.date = date;
	}
		
	public HashMap<Integer, WorldBossRank> getCurrentRank(){
		return currentRank;
	}
	
	public void setCurrentRank(HashMap<Integer, WorldBossRank>rank){
		this.currentRank = rank;
	}
}
