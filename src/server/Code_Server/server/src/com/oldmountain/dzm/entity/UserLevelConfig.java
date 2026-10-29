package com.oldmountain.dzm.entity;

public class UserLevelConfig {
	private int level;
	private int careerXp;
	private int challengeUserXp;
	private int challengeDiscipleXp;
	private int challengeCoins;
	private int coins1;
	private int worldBossRumCount;
	private int worldBossRumProbability;
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getCareerXp(){
		return careerXp;
	}
	
	public void setCareerXp(int xp){
		this.careerXp = xp;
	}
	
	public int getChallengeUserXp(){
		return challengeUserXp;
	}
	
	public void setChallengeUserXp(int xp){
		this.challengeUserXp = xp;
	}
	
	public int getChallengeDiscipleXp(){
		return challengeDiscipleXp;
	}
	
	public void setChallengeDiscipleXp(int xp){
		this.challengeDiscipleXp = xp;
	}
	
	public int getChallengeCoins(){
		return challengeCoins;
	}
	
	public void setChallengeCoins(int coins){
		this.challengeCoins = coins; 
	}
	
	public int getCoins1(){
		return coins1;
	}
	
	public void setCoins1(int coins){
		this.coins1 = coins;
	}
	
	public int getWorldBossRumCount(){
		return worldBossRumCount;
	}
	
	public void setWorldBossRumCount(int count){
		this.worldBossRumCount = count;
	}
	
	public int getWorldBossRumProbability(){
		return worldBossRumProbability;
	}
	
	public void setWorldBossRumProbability(int probo){
		this.worldBossRumProbability = probo;
	}
}
