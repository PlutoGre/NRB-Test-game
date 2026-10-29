package com.oldmountain.dzm.configEntity;

public class GuildShopSell {
	private int id;//商品id
	private int itemType;//对应物品类型
	private int propId;//物品的itemid
	private int count;//物品可买个数
	private int price;//每个商品的价格
	private int shopType;//商店等级出现还是盟战占领出现
	private int dropPriority;//掉落优先级
	private int shopLvl;//对应的商店等级
	private int buyPermission;//需要的成员等级
	private int rate;//出现的概率
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getItemType(){
		return itemType;
	}
	
	public void setItemType(int type){
		this.itemType = type;
	}
	
	public int getPropId(){
		return propId;
	}
	
	public void setPropId(int id){
		this.propId = id;
	}
	
	public int getCount(){
		return count;
	}
	
	public void setCount(int count){
		this.count = count;
	}
	
	public int getPrice(){
		return price;
	}
	
	public void setPrice(int price){
		this.price = price;
	}
	
	public int getShopType(){
		return shopType;
	}
	
	public void setShopType(int type){
		this.shopType = type;
	}
	
	public int getDropPriority(){
		return dropPriority;
	}
	
	public void setDropPriority(int priority){
		this.dropPriority = priority;
	}
	
	public int getShopLvl(){
		return shopLvl;
	}
	
	public void setShopLvl(int shopLvl){
		this.shopLvl = shopLvl;
	}
	
	public int getBuyPermission(){
		return buyPermission;
	}
	
	public void setBuyPermission(int permission){
		this.buyPermission = permission;
	}
	
	public int getRate(){
		return rate;
	}
	
	public void setRate(int rate){
		this.rate = rate;
	}
}