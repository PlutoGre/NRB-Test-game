package com.oldmountain.dzm.entity;

public class ChargeRebateXmlTemplate {
	private int id;		//活动ID
	private int activeTimeInterval;		//活动开启周期间隔
	private int chargeDuration;		//储值累积时间
	private int rebateDuration;		//返利领取时间
	private int minRecharge;	//最小充值额度
	private int maxRecharge;	//最大充值额度
	private float refundRate;	//返利比率
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}
	public int getActiveTimeInterval() {
		return activeTimeInterval;
	}
	public void setActiveTimeInterval(int activeTimeInterval) {
		this.activeTimeInterval = activeTimeInterval;
	}
	public int getChargeDuration() {
		return chargeDuration;
	}
	public void setChargeDuration(int chargeDuration) {
		this.chargeDuration = chargeDuration;
	}
	public int getRebateDuration() {
		return rebateDuration;
	}
	public void setRebateDuration(int rebateDuration) {
		this.rebateDuration = rebateDuration;
	}
	public int getMinRecharge() {
		return minRecharge;
	}
	public void setMinRecharge(int minRecharge) {
		this.minRecharge = minRecharge;
	}
	public int getMaxRecharge() {
		return maxRecharge;
	}
	public void setMaxRecharge(int maxRecharge) {
		this.maxRecharge = maxRecharge;
	}
	public float getRefundRate() {
		return refundRate;
	}
	public void setRefundRate(float refundRate) {
		this.refundRate = refundRate;
	}
	
}
