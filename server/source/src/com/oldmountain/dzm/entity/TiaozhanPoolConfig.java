package com.oldmountain.dzm.entity;

public class TiaozhanPoolConfig {
	private int type;//积分兑换类型 1:可重复兑换,2:达到条件后可兑换一次
	private int arenaRewardIndex; //对应奖励序列
	private int arenaRewardType; //奖励类型
	private int arenaRewardId; //奖励id
	private int arenaCost; //兑换积分消耗
	private int rankLimit; //兑换排名限制
	private int rewardCount; //奖励个数
	public int getType() {
		return type;
	}
	public void setType(int type) {
		this.type = type;
	}
	public int getArenaRewardIndex() {
		return arenaRewardIndex;
	}
	public void setArenaRewardIndex(int arenaRewardIndex) {
		this.arenaRewardIndex = arenaRewardIndex;
	}
	public int getArenaRewardType() {
		return arenaRewardType;
	}
	public void setArenaRewardType(int arenaRewardType) {
		this.arenaRewardType = arenaRewardType;
	}
	public int getArenaRewardId() {
		return arenaRewardId;
	}
	public void setArenaRewardId(int arenaRewardId) {
		this.arenaRewardId = arenaRewardId;
	}
	public int getArenaCost() {
		return arenaCost;
	}
	public void setArenaCost(int arenaCost) {
		this.arenaCost = arenaCost;
	}
	public int getRankLimit() {
		return rankLimit;
	}
	public void setRankLimit(int rankLimit) {
		this.rankLimit = rankLimit;
	}
	public int getRewardCount() {
		return rewardCount;
	}
	public void setRewardCount(int rewardCount) {
		this.rewardCount = rewardCount;
	}
	 
}
