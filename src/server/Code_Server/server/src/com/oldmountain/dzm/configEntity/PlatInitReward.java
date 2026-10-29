package com.oldmountain.dzm.configEntity;

public class PlatInitReward {
	private int rewardStartTime;//开始时间
	private int rewardEndTime;//结束时间
	private String rewardInfo;//奖励物品
	private String rewardMsg;//奖励信息
	
	public int getRewardStartTime(){
		return rewardStartTime;
	}
	
	public void setRewardStartTime(int rewardStartTime){
		this.rewardStartTime = rewardStartTime;
	}
	
	public int getRewardEndTime(){
		return rewardEndTime;
	}
	
	public void setRewardEndTime(int rewardEndTime){
		this.rewardEndTime = rewardEndTime;
	}
	
	public String getRewardInfo(){
		return rewardInfo;
	}
	
	public void setRewardInfo(String rewardInfo){
		this.rewardInfo = rewardInfo;
	}
	
	public String getRewardMsg(){
		return rewardMsg;
	}
	
	public void setRewardMsg(String rewardMsg){
		this.rewardMsg = rewardMsg;
	}
}
