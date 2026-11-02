package com.oldmountain.dzm.entity;

import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

import net.sf.json.JSONObject;

public class LeaguaMembers implements Comparable<LeaguaMembers> {
	private int playerId;//成员的游戏id
	private String playerName;//成员名
	private int playerlevel;//成员的等级
	private int playerGrade;//成员的公会等级,1为普通成员；2为精英队长；3为副会长；4为会长
	private int playerLeftContribution;//成员剩余的公会贡献
	private int playerTotalContribution;//成员的公会贡献
	private int playerKillCount;//成员在盟战中的总杀敌数
	private int guildId;//成员所属的公会ID
	private JSONObject shopPurchaseInfo;//成员在公会商店中购买物品的信息
	private int lastBuyTime;//成员上次够买时间
	private int time;//成员入盟时间
	private int nextCanAttackTime;//玩家下次可攻击事件
	private int leftAttackTimes;//玩家剩余攻击次数
	private int leftGetAttackRewardTimes;//玩家剩余可获得收益次数
	private int lastUpgradeTime;//上次提升时间
	private int reccontributiontime;//上次领取帮贡的时间
	private int lastRefreshTime;//上次更新事件
	private int lastShopRefreshTime;//上次更新商店时间
	
	public LeaguaMembers(){
		this.lastRefreshTime = 0;
		this.shopPurchaseInfo = new JSONObject();
	}
	
	public int getPlayerId(){
		return playerId;
	}
	
	public void setPlayerId(int id){
		this.playerId = id;
	}
	
	public String getPlayerName(){
		return playerName;
	}
	
	public void setPlayerName(String name){
		this.playerName = name;
	}
	
	public int getPlayerlevel(){
		return playerlevel;
	}
	
	public void setPlayerlevel(int level){
		this.playerlevel = level;
	}
	
	public int getPlayerGrade(){
		return playerGrade;
	}
	
	public void setPlayerGrade(int grade){
		this.playerGrade = grade;
	}
	
	public int getPlayerLeftContribution(){
		return playerLeftContribution;
	}
	
	public void setPlayerLeftContribution(int contribution){
		this.playerLeftContribution = contribution;
	}
	
	public void increasePlayerLeftContribution(int contribution){
		this.playerLeftContribution += contribution;
	}
	
	public void decreasePlayerLeftContribution(int contribution){
		this.playerLeftContribution -= contribution;
	}
	
	public int getPlayerTotalContribution(){
		return playerTotalContribution;
	}
	
	public void setPlayerTotalContribution(int constribution){
		this.playerTotalContribution = constribution;
	}
	
	public void increasePlayerTotalContribution(int constribution){
		this.playerTotalContribution += constribution;
	}
	
	public int getPlayerKillCount(){
		return playerKillCount;
	}
	
	public void setPlayerKillCount(int count){
		this.playerKillCount = count;
	}
	
	public int getGuildId(){
		return guildId;
	}
	
	public void setGuildId(int guildId){
		this.guildId = guildId;
	}
	
	public String getShopPurchaseInfo(){
		return shopPurchaseInfo.toString();
	}
	
	public void setShopPurchaseInfo(String shopPurchaseInfo){
		this.shopPurchaseInfo = JSONObject.fromObject(shopPurchaseInfo);
	}
	
	public JSONObject getShopPurchaseInfoInJson(){
		return shopPurchaseInfo;
	}
	
	public void setShopPurchaseInfo(JSONObject shopPurchaseInfo){
		this.shopPurchaseInfo = shopPurchaseInfo;
	}
	
	public int getLastBuyTime(){
		return lastBuyTime;
	}
	
	public void setLastBuyTime(int time){
		this.lastBuyTime = time;
	}
	
	public int getTime(){
		return time;
	}
	
	public void setTime(int time){
		this.time = time;
	}
	
	public int getNextCanAttackTime(){
		return nextCanAttackTime;
	}
	
	public void setNextCanAttackTime(int time){
		this.nextCanAttackTime = time;
	}
	
	private void checkCrossDay(){
		if (this.lastRefreshTime < Util.getTodayZeroTimeStamp()) {
			this.leftAttackTimes = XMLTemplateService.getGuildConfig().getDailyMaxAttackTimes();
			this.leftGetAttackRewardTimes = XMLTemplateService.getGuildConfig().getDailyMaxRewardTimes();
			this.lastRefreshTime = Util.getServerTime();
		}
	}
	
	public int checkAndGetLeftAttackTimes(){
		checkCrossDay();
		return leftAttackTimes;
	}
	
	public int getLeftAttackTimes(){
		return leftAttackTimes;
	}
	
	public void setLeftAttackTimes(int times){
		this.leftAttackTimes = times;
	}
	
	public int decreseLeftAttackTimes(){
		return this.leftAttackTimes --;
	}
	
	public int checkAndGetLeftGetAttackRewardTimes(){
		checkCrossDay();
		return leftGetAttackRewardTimes;
	}
	
	public int getLeftGetAttackRewardTimes(){
		return leftGetAttackRewardTimes;
	}
	
	public void setLeftGetAttackRewardTimes(int times){
		this.leftGetAttackRewardTimes = times;
	}
	
	public int decreseLeftGetAttackRewardTimes(){
		return leftGetAttackRewardTimes --;
	}
	
	public int getLastUpgradeTime(){
		return lastUpgradeTime;
	}
	
	public void setLastUpgradeTime(int time){
		this.lastUpgradeTime = time;
	}
	
	public int getLastRefreshTime(){
		return lastRefreshTime;
	}
	
	public void setLastRefreshTime(int time){
		this.lastRefreshTime = time;
	}
	
	public int getReccontributiontime(){
		return reccontributiontime;
	}
	
	public void setReccontributiontime(int time){
		this.reccontributiontime = time;
	}
	
	public int getLastShopRefreshTime(){
		return lastShopRefreshTime;
	}
	
	public void setLastShopRefreshTime(int time){
		this.lastShopRefreshTime = time;
	}
	
	public int compareTo(LeaguaMembers member){
		int result = 0;
		if (this.playerTotalContribution > member.playerTotalContribution) {
			result = 1;
		}
		else if(this.playerTotalContribution < member.playerTotalContribution){
			result = -1;
		}
		return result;
	}
}
