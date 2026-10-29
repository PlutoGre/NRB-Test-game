package com.oldmountain.dzm.entity;

public class QuickCareerConfig {
	private int basecoldTime;	//连闯功能每次挑战的基础冷却时间
	private int basePrice;		//连闯功能cd重置的最大消耗钻石数
	private int vipLimit;		//连闯功能的vip等级限制
	private int vipResetLimit;	//连闯功能cd重置的vip等级显示
	private int divisor;
	
	public QuickCareerConfig(){
		this.basecoldTime=0;
		this.basePrice=0;
		this.vipLimit=0;
		this.vipResetLimit=0;
		this.divisor=0;
	}
	public int getBasecoldTime() {
		return basecoldTime;
	}
	public void setBasecoldTime(int basecoldTime) {
		this.basecoldTime = basecoldTime;
	}
	public int getBasePrice() {
		return basePrice;
	}
	public void setBasePrice(int basePrice) {
		this.basePrice = basePrice;
	}
	public int getVipLimit() {
		return vipLimit;
	}
	public void setVipLimit(int vipLimit) {
		this.vipLimit = vipLimit;
	}
	public int getVipResetLimit() {
		return vipResetLimit;
	}
	public void setVipResetLimit(int vipResetLimit) {
		this.vipResetLimit = vipResetLimit;
	}
	public int getDivisor() {
		return divisor;
	}
	public void setDivisor(int divisor) {
		this.divisor = divisor;
	}
	
}
