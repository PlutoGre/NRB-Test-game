package com.oldmountain.dzm.configEntity;

import java.util.ArrayList;
import java.util.HashMap;

public class GuildConfig {
	private int createGold;
	private int levelLimit;
	private String defaultBrodcast;
	private int totemRepairNeedSeconds;
	private int dailyMaxAttackTimes;
	private int dailyMaxRewardTimes;
	private int attackCoolSeconds;
	private int attckHp;
	private String shopRefreshCD;
	private int shopWinFreshCost;
	private int memberUpgradeCD;
	private String changeRate;
	private int broadcastRefreshCD;//公告更改冷却时间
	private float getGuildCoinRatio;//每日领取帮贡系数
	private float attackAdjustmentRatio;//攻打公会收益调整系数
	private float attackRankingRatio1;//进攻时目标公会排名系数，计算公式基数
	private float attackRankingRatio2;//进攻时目标公会排名系数，计算公式修正数
	private int leaguaMaxLevel;//公会的最高等级
	private HashMap<Integer, GuildLevelConfig> levelConfigs = new HashMap<Integer, GuildLevelConfig>();
	private HashMap<Integer, ArrayList<Integer>> allMedals = new HashMap<Integer, ArrayList<Integer>>();
	private HashMap<Integer, ArrayList<Integer>> allBuilds = new HashMap<Integer, ArrayList<Integer>>();
	private HashMap<Integer, HashMap<Integer, GuildShopSell>> regularShopSell = new HashMap<Integer,HashMap<Integer,GuildShopSell>>();	
	private HashMap<Integer, HashMap<Integer, GuildShopSell>> occupyShopSell = new HashMap<Integer, HashMap<Integer,GuildShopSell>>();
	private HashMap<Integer, HashMap<Integer, GuildNpcConfig>> totemNpcs = new HashMap<Integer, HashMap<Integer,GuildNpcConfig>>();
	
	public int getCreateGold(){
		return createGold;
	}
	
	public void setCreateGold(int gold){
		this.createGold = gold;
	}
	
	public int getLevelLimit(){
		return levelLimit;
	}
	
	public void setLevelLimit(int level){
		this.levelLimit = level;
	}
	
	public String getDefaultBrodcast(){
		return defaultBrodcast;
	}
	
	public void setDefaultBrodcast(String defaultBrodcast){
		this.defaultBrodcast = defaultBrodcast;
	}
	
	public int getTotemRepairNeedSeconds(){
		return totemRepairNeedSeconds;
	}
	
	public void setTotemRepairNeedSeconds(int seconds){
		this.totemRepairNeedSeconds = seconds;
	}
	
	public HashMap<Integer, GuildLevelConfig> getLevelConfigs (){
		return levelConfigs;
	}
	
	public GuildLevelConfig getLevelConvigByLevel(int level){
		return levelConfigs.get(level);
	}
	
	public void setLevelConfigs(HashMap<Integer, GuildLevelConfig> configs){
		this.levelConfigs = configs;
	}
	
	public HashMap<Integer, ArrayList<Integer>> getAllMedals(){
		return allMedals;
	}
	
	public ArrayList<Integer> getMedalsByType(int type){
		return allMedals.get(type);
	}
	
	public void setAllMedals(HashMap<Integer, ArrayList<Integer>> allMedals){
		this.allMedals = allMedals;
	}
	
	public HashMap<Integer, ArrayList<Integer>> getAllBuilds(){
		return allBuilds;
	}
	
	public ArrayList<Integer> getBuildsByType(int type){
		return allBuilds.get(type);
	}
	
	public void setAllBuilds(HashMap<Integer, ArrayList<Integer>> allBuilds){
		this.allBuilds = allBuilds;
	}
	
	public int getDailyMaxAttackTimes(){
		return dailyMaxAttackTimes;
	}
	
	public void setDailyMaxAttackTimes(int times){
		this.dailyMaxAttackTimes = times;
	}
	
	public int getDailyMaxRewardTimes(){
		return dailyMaxRewardTimes;
	}
	
	public void setDailyMaxRewardTimes(int times){
		this.dailyMaxRewardTimes = times;
	}
	
	public int getAttackCoolSeconds(){
		return attackCoolSeconds;
	}
	
	public void setAttackCoolSeconds(int attackCoolSeconds){
		this.attackCoolSeconds = attackCoolSeconds;
	}
	
	public int getAttckHp(){
		return attckHp;
	}
	
	public void setAttckHp(int attckHp){
		this.attckHp = attckHp;
	}
	
	public String getShopRefreshCD(){
		return shopRefreshCD;
	}
	
	public void setShopRefreshCD(String cd){
		this.shopRefreshCD = cd;
	}
	
	public int getMemberUpgradeCD(){
		return memberUpgradeCD;
	}
	
	public void setMemberUpgradeCD(int cd){
		this.memberUpgradeCD = cd;
	}
	
	public String getChangeRate(){
		return changeRate;
	}
	
	public void setChangeRate(String rate){
		this.changeRate = rate;
	}
	
	public float getGetGuildCoinRatio(){
		return getGuildCoinRatio;
	}
	
	public void setGetGuildCoinRatio(Float getGuildCoinRatio){
		this.getGuildCoinRatio = getGuildCoinRatio;
	}
	
	public HashMap<Integer, HashMap<Integer, GuildShopSell>> getRegularShopSell(){
		return regularShopSell;
	}
	
	public void setRegularShopSell(HashMap<Integer, HashMap<Integer, GuildShopSell>> shopSell){
		this.regularShopSell = shopSell;
	}
	
	public HashMap<Integer, HashMap<Integer, GuildShopSell>> getOccupyShopSell(){
		return occupyShopSell;
	}
	
	public void setOccupyShopSell(HashMap<Integer, HashMap<Integer, GuildShopSell>> shopSell){
		this.occupyShopSell = shopSell;
	}

	public int getBroadcastRefreshCD() {
		return broadcastRefreshCD;
	}

	public void setBroadcastRefreshCD(int broadcastRefreshCD) {
		this.broadcastRefreshCD = broadcastRefreshCD;
	}
	
	public int getShopWinFreshCost(){
		return shopWinFreshCost;
	}
	
	public void setShopWinFreshCost(int shopWinFreshCost){
		this.shopWinFreshCost = shopWinFreshCost;
	}
	
	public float getAttackAdjustmentRatio(){
		return attackAdjustmentRatio;
	}
	
	public void setAttackAdjustmentRatio(float attackAdjustmentRatio){
		this.attackAdjustmentRatio = attackAdjustmentRatio;
	}
	
	public float getAttackRankingRatio1(){
		return attackRankingRatio1;
	}
	
	public void setAttackRankingRatio1(float attackRankingRatio1){
		this.attackRankingRatio1 = attackRankingRatio1;
	}
	
	public float getAttackRankingRatio2(){
		return attackRankingRatio2;
	}
	
	public void setAttackRankingRatio2(float attackRankingRatio2){
		this.attackRankingRatio2 = attackRankingRatio2;
	}
	
	public int getLeaguaMaxLevel(){
		return leaguaMaxLevel;
	}
	
	public void setLeaguaMaxLevel(int level){
		this.leaguaMaxLevel = level;
	}
	
	public HashMap<Integer, HashMap<Integer, GuildNpcConfig>> getTotemNpcConfigs(){
		return totemNpcs;
	}
	
	public GuildNpcConfig getNpcConfigByTotemAndLevel(int totemId,int level){
		return totemNpcs.get(totemId).get(level);
	}
	
	public void setNpcConfigs(HashMap<Integer, HashMap<Integer, GuildNpcConfig>> npcConfigs){
		this.totemNpcs = npcConfigs;
	}
}