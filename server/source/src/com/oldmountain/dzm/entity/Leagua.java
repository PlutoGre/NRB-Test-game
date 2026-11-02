package com.oldmountain.dzm.entity;

import net.sf.json.JSONObject;

public class Leagua implements Comparable<Leagua> {
	private int leaguaId;//公会id
	private String leaguaName;//公会名称
	private int leaguaLevel;//公会等级
	private int leaguaMaxMemberCount;
	private int leaguaCurMemberCount;//公会当前成员数
	private long leaguaFunds;//公会资金
	private int ownerPlayerId;//公会创建者ID
	private String ownerPlayerName;//公会创建者姓名
	private int leaguaRank;//公会排名
	private int leaguaActiveMedal;//公会当前激活徽章
	private String leaguaBroadcast;//公会公告
	private JSONObject leaguaTotemInfo;//公会的图腾信息
	private JSONObject leaguaBuildingInfo;//公会建筑信息
	private JSONObject leaguaShopInfo;//公会商店出售的信息
	private int leaguaShopRefreshTime;//公会商店上次刷新时间
	private float leaguaWinRate;//公会盟站胜率
	private int createTime;//公会创建时间
	private int broadcastRefreshTime;//公会上次更改公告的时间
	private int contributionCount;//今日可领取的贡献数
	private int contributionTime;//上次更新可领取贡献的日期
	private int receiveMedalTime;//领取徽章的时间
	private int transLeaderTime;//上次转让盟主的时间
	private JSONObject totemRepairCosts = new JSONObject();//图腾上次请求修复的状态
	
	public Leagua(){
		this.leaguaTotemInfo = new JSONObject();
		this.leaguaBuildingInfo = new JSONObject();
		this.leaguaShopInfo = new JSONObject();
	}
	
	public int getLeaguaId(){
		return leaguaId;
	}
	
	public void setLeaguaId(int id){
		this.leaguaId = id;
	}
	
	public String getLeaguaName(){
		return leaguaName;
	}
	
	public void setLeaguaName(String name){
		this.leaguaName = name;
	}
	
	public int getLeaguaLevel(){
		return leaguaLevel;
	}
	
	public void setLeaguaLevel(int level){
		this.leaguaLevel = level;
	}
	
	public int getLeaguaMaxMemberCount(){
		return leaguaMaxMemberCount;
	}
	
	public void setLeaguaMaxMemberCount(int leaguaMaxMemberCount){
		this.leaguaMaxMemberCount = leaguaMaxMemberCount;
	}
	
	public int getLeaguaCurMemberCount(){
		return leaguaCurMemberCount;
	}
	
	public void setLeaguaCurMemberCount(int count){
		this.leaguaCurMemberCount = count;
	}
	
	public long getLeaguaFunds(){
		return leaguaFunds;
	}
	
	public void setLeaguaFunds(long funds){
		this.leaguaFunds = funds;
	}
	
	public String getOwnerPlayerName(){
		return ownerPlayerName;
	}
	
	public void setOwnerPlayerName(String ownerPlayerName){
		this.ownerPlayerName = ownerPlayerName;
	}
	
	public int getOwnerPlayerId(){
		return ownerPlayerId;
	}
	
	public void setOwnerPlayerId(int ownerId){
		this.ownerPlayerId = ownerId;
	}
	
	public int getLeaguaRank(){
		return leaguaRank;
	}
	
	public void setLeaguaRank(int rank){
		this.leaguaRank = rank;
	}
	
	public int getLeaguaActiveMedal(){
		return leaguaActiveMedal;
	}
	
	public void setLeaguaActiveMedal(int medalId){
		this.leaguaActiveMedal = medalId;
	}
	
	public String getLeaguaBroadcast(){
		return leaguaBroadcast;
	}
	
	public void setLeaguaBroadcast(String broadcast){
		this.leaguaBroadcast = broadcast;
	}
	
	public String getLeaguaTotemInfo(){
		return leaguaTotemInfo.toString();
	}
	
	public void setLeaguaTotemInfo(String totemInfo){
		this.leaguaTotemInfo = JSONObject.fromObject(totemInfo);
	}
	
	public JSONObject getLeaguaTotemInfoInJson(){
		return leaguaTotemInfo;
	}
	
	public void setLeaguaTotemInfoFromJson(JSONObject totemInfo){
		this.leaguaTotemInfo = totemInfo;
	}
	
	public String getLeaguaBuildingInfo(){
		return leaguaBuildingInfo.toString();
	}
	
	public void setLeaguaBuildingInfo(String leaguaBuildingInfo){
		this.leaguaBuildingInfo = JSONObject.fromObject(leaguaBuildingInfo);
	}
	
	public JSONObject getLeaguaBuildingInfoInJson(){
		return this.leaguaBuildingInfo;
	}
	
	public void setLeaguaBuildingInfoFromJson(JSONObject leaguaBuildingInfo){
		this.leaguaBuildingInfo = leaguaBuildingInfo;
	}
	
	public String getLeaguaShopInfo(){
		return leaguaShopInfo.toString();
	}
	
	public void setLeaguaShopInfo(String leaguaShopInfo){
		this.leaguaShopInfo = JSONObject.fromObject(leaguaShopInfo);
	}
	
	public JSONObject getLeaguaShopInfoInJson(){
		return leaguaShopInfo;
	}
	
	public void setLeaguaShopInfoFromJson(JSONObject leaguaShopInfo){
		this.leaguaShopInfo = leaguaShopInfo;
	}
	
	public int getLeaguaShopRefreshTime(){
		return leaguaShopRefreshTime;
	}
	
	public void setLeaguaShopRefreshTime(int time){
		this.leaguaShopRefreshTime = time;
	}
	
	public float getLeaguaWinRate(){
		return leaguaWinRate;
	}
	
	public void setLeaguaWinRate(float leaguaWinRate){
		this.leaguaWinRate = leaguaWinRate;
	}
	
	public int getCreateTime(){
		return createTime;
	}
	
	public void setCreateTime(int time){
		this.createTime = time;
	}
	
	public int compareTo(Leagua leagua){
		int result = 0;
		if (this.leaguaLevel > leagua.leaguaLevel) {
			result = 1;
		}
		else if(this.leaguaLevel < leagua.leaguaLevel){
			result = -1;
		}
		else {
			if (this.leaguaWinRate > leagua.leaguaWinRate) {
				result = 1;
			}
			else if (this.leaguaWinRate > leagua.leaguaWinRate) {
				result = -1;
			}
		}
		return result;
	}

	public int getBroadcastRefreshTime() {
		return broadcastRefreshTime;
	}

	public void setBroadcastRefreshTime(int broadcastRefreshTime) {
		this.broadcastRefreshTime = broadcastRefreshTime;
	}
	
	public JSONObject getTotemRepairCosts(){
		return totemRepairCosts;
	}
	
	public void setTotemRepairCosts(JSONObject totemRepairCosts){
		this.totemRepairCosts = totemRepairCosts;
	}
	
	public int getContributionCount(){
		return contributionCount;
	}
	
	public void setContributionCount(int contributionCount){
		this.contributionCount = contributionCount;
	}
	
	public int getContributionTime(){
		return contributionTime;
	}
	
	public void setContributionTime(int contributionTime){
		this.contributionTime = contributionTime;
	}
	
	public int getReceiveMedalTime(){
		return receiveMedalTime;
	}
	
	public void setReceiveMedalTime(int time){
		this.receiveMedalTime = time;
	}
	
	public int getTransLeaderTime(){
		return transLeaderTime;
	}
	
	public void setTransLeaderTime(int transLeaderTime){
		this.transLeaderTime = transLeaderTime;
	}
}