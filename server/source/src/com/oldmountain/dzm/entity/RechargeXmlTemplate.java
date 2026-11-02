package com.oldmountain.dzm.entity;

public class RechargeXmlTemplate {
	private int id;
	private int amount;
	private int addNum;
	private int costMoney;
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getAmount(){
		return amount;
	}
	
	public void setAmount(int amount){
		this.amount = amount;
	}
	
	public int getAddNum(){
		return addNum;
	}
	
	public void setAddNum(int num){
		this.addNum = num;
	}
	
	public int getCostMoney(){
		return costMoney;
	}
	
	public void setCostMoney(int money){
		this.costMoney = money;
	}
}

