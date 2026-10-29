package com.oldmountain.dzm.entity;

public class Recharge {
	private String cooorderserial;
	private String consumestreamid;
	private String uid;
	private int playerid;
	private int goodsid;
	private int goodscount;
	private int goodscost;
	private int amount;
	private int addnum;
	private int isfirstpay;
	private String requesttime;
	private int createtime;
	
	public String getCooorderserial(){
		return cooorderserial;
	}
	
	public void setCooorderserial(String serial){
		this.cooorderserial = serial;
	}
	
	public String getConsumestreamid(){
		return consumestreamid;
	}
	
	public void setConsumestreamid(String id){
		this.consumestreamid = id;
	}
	
	public String getUid(){
		return uid;
	}
	
	public void setUid(String uid){
		this.uid = uid;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getGoodsid(){
		return goodsid;
	}
	
	public void setGoodsid(int goodsid){
		this.goodsid = goodsid;
	}
	
	public int getGoodscount(){
		return goodscount;
	}
	
	public void setGoodscount(int count){
		this.goodscount = count;
	}
	
	public int getGoodscost(){
		return goodscost;
	}
	
	public void setGoodscost(int cost){
		this.goodscost = cost;
	}
	
	public int getAmount(){
		return amount;
	}
	
	public void setAmount(int amount){
		this.amount = amount;
	}
	
	public int getAddnum(){
		return addnum;
	}
	
	public void setAddnum(int addnum){
		this.addnum = addnum;
	}
	
	public int getIsfirstpay(){
		return isfirstpay;
	}
	
	public void setIsfirstpay(int firstpay){
		this.isfirstpay = firstpay;
	}
	
	public String getRequesttime(){
		return requesttime;
	}
	
	public void setRequesttime(String requesttime){
		this.requesttime = requesttime;
	}
	
	public int getCreatetime(){
		return createtime;
	}
	
	public void setCreatetime(int createtime){
		this.createtime = createtime;
	}
}
