package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class GiveSoulStage {
	private List<Integer> id;
	private List<Integer> typeid;			//抽取类型
	private List<Integer> stage;			//阶段
	private List<Integer> condition_soulId;		//阶段判定魂魄id
	private List<Integer> condition_count;	//阶段判定次数
	private List<Integer> balanceSoulId;	//平衡判定魂魄id
	private List<Integer> balanceTimes;		//平衡累计次数条件
	private List<Integer> balanceId;		//平衡必中id
	
	public GiveSoulStage(){
        this.id = new ArrayList<Integer>();
        this.typeid = new ArrayList<Integer>();
        this.stage = new ArrayList<Integer>();
        this.condition_soulId = new ArrayList<Integer>();
        this.condition_count = new ArrayList<Integer>();
        this.balanceSoulId = new ArrayList<Integer>();
        this.balanceTimes = new ArrayList<Integer>();
        this.balanceId = new ArrayList<Integer>();
        //将有用数据起始角标置为1,从1开始遍历
        this.id.add(0);
        this.typeid.add(0);
        this.stage.add(0);
        this.condition_soulId.add(0);
        this.condition_count.add(0);
        this.balanceSoulId.add(0);
        this.balanceTimes.add(0);
        this.balanceId.add(0);
    }
	public List<Integer> getId() {
		return id;
	}
	public void setId(List<Integer> id) {
		this.id = id;
	}
	public List<Integer> getTypeid() {
		return typeid;
	}
	public void setTypeid(List<Integer> typeid) {
		this.typeid = typeid;
	}
	public List<Integer> getStage() {
		return stage;
	}
	public void setStage(List<Integer> stage) {
		this.stage = stage;
	}
	public List<Integer> getCondition_soulId() {
		return condition_soulId;
	}
	public void setCondition_id(List<Integer> condition_soulId) {
		this.condition_soulId = condition_soulId;
	}
	public List<Integer> getCondition_count() {
		return condition_count;
	}
	public void setCondition_count(List<Integer> condition_count) {
		this.condition_count = condition_count;
	}
	public List<Integer> getBalanceTimes() {
		return balanceTimes;
	}
	public void setBalanceTimes(List<Integer> balanceTimes) {
		this.balanceTimes = balanceTimes;
	}
	public List<Integer> getBalanceId() {
		return balanceId;
	}
	public void setBalanceId(List<Integer> balanceId) {
		this.balanceId = balanceId;
	}
	public List<Integer> getBalanceSoulId() {
		return balanceSoulId;
	}
	public void setBalanceSoulId(List<Integer> balanceSoulId) {
		this.balanceSoulId = balanceSoulId;
	}
	
	
}
