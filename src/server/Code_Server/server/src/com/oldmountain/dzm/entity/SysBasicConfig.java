package com.oldmountain.dzm.entity;

import java.util.List;

public class SysBasicConfig {
	private int tenCoolTime;//十里冷却时间
	private int tenCostGold;//十里消耗元宝数
	private int hundredCoolTime;//百里冷却时间
	private int hundredCostGold;//百里消耗元宝数
	private int wanliCoolTime;//万里冷却时间
	private int wanliCostGold;//万里消耗元宝数
	private List<Integer> poolLimits;//弟子下次能进入的高级奖池Id
	private List<Integer> soulCounts;//抽取魂魄的数量
    private List<Integer> treasurePoolNeed;//进入高级奖池所需积分
    private List<Integer> treasureClearCount;//对应高级奖池积分清零累积额度
    private float careerLuckyProportion;//江湖宝箱累计积分比例
    private float fightEndLuckyProportion;//血战宝箱累计积分比例
    private float emptyLuckPoolProportion;//运气池为空时累计运气值比例


    public int getTenCoolTime(){
		return tenCoolTime;
	}

    public void setTenCoolTime(int time){
		this.tenCoolTime = time;
	}
	
	public int getTenCostGold(){
		return tenCostGold;
	}
	
	public void setTenCostGold(int gold){
		this.tenCostGold = gold;
	}
	
	public int getHundredCoolTime(){
		return hundredCoolTime;
	}
	
	public void setHundredCoolTime(int time){
		this.hundredCoolTime = time;
	}
	
	public int getHundredCostGold(){
		return hundredCostGold;
	}
	
	public void setHundredCostGold(int gold){
		this.hundredCostGold = gold;
	}
	
	public int getWanliCoolTime(){
		return wanliCoolTime;
	}
	
	public void setWanliCoolTime(int time){
		this.wanliCoolTime = time;
	}
	
	public int getWanliCostGold(){
		return wanliCostGold;
	}
	
	public void setWanliCostGold(int gold){
		this.wanliCostGold = gold;
	}
	
	public List<Integer> getPoolLimits(){
		return poolLimits;
	}
	
	public void setPoolLimits(List<Integer> limits){
		this.poolLimits = limits;
	}
	
	public List<Integer> getSoulCounts(){
		return soulCounts;
	}
	
	public void setSoulCounts(List<Integer> limits){
		this.soulCounts = limits;
	}
    public List<Integer> getTreasureClearCount() {
        return treasureClearCount;
    }

    public void setTreasureClearCount(List<Integer> treasureClearCount) {
        this.treasureClearCount = treasureClearCount;
    }

    public List<Integer> getTreasurePoolNeed() {
        return treasurePoolNeed;
    }

    public void setTreasurePoolNeed(List<Integer> treasurePoolNeed) {
        this.treasurePoolNeed = treasurePoolNeed;
    }

    public float getCareerLuckyProportion() {
        return careerLuckyProportion;
    }

    public void setCareerLuckyProportion(float careerLuckyProportion) {
        this.careerLuckyProportion = careerLuckyProportion;
    }

    public float getFightEndLuckyProportion() {
        return fightEndLuckyProportion;
    }

    public void setFightEndLuckyProportion(float fightEndLuckyProportion) {
        this.fightEndLuckyProportion = fightEndLuckyProportion;
    }

    public float getEmptyLuckPoolProportion() {
        return emptyLuckPoolProportion;
    }

    public void setEmptyLuckPoolProportion(float emptyLuckPoolProportion) {
        this.emptyLuckPoolProportion = emptyLuckPoolProportion;
    }
}
