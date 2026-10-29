package com.oldmountain.dzm.configEntity;

public class GuildLevelConfig{
	private int level;
	private int guildMaxMember;
	private int guildXp;
	private int shopXp;
	private int catalogCount;
	private int shoppingCD;
	private int bankXp;
	private int bankStorage;
	private float totemBuff;
	private int totemHp;
	private int totemXp;
	private int hpAltar;
	private int atcAltar;
	private int defAltar;
	private int willAltar;
	private int altarXp;
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getGuildMaxMember(){
		return guildMaxMember;
	}
	
	public void setGuildMaxMember(int maxCount){
		this.guildMaxMember = maxCount;
	}
	
	public int getGuildXp(){
		return guildXp;
	}
	
	public void setGuildXp(int xp){
		this.guildXp = xp;
	}
	
	public int getShopXp(){
		return shopXp;
	}
	
	public void setShopXp(int shopXp){
		this.shopXp = shopXp;
	}
	
	public int getCatalogCount(){
		return catalogCount;
	}
	
	public void setCatalogCount(int catalogCount){
		this.catalogCount = catalogCount;
	}
	
	public int getShoppingCD(){
		return shoppingCD;
	}
	
	public void setShoppingCD(int shoppingCD){
		this.shoppingCD = shoppingCD;
	}
	
	public int getBankXp(){
		return bankXp;
	}
	
	public void setBankXp(int bankXp){
		this.bankXp = bankXp;
	}
	
	public int getBankStorage(){
		return bankStorage;
	}
	
	public void setBankStorage(int bankStorage){
		this.bankStorage = bankStorage;
	}
	
	public float getTotemBuff(){
		return totemBuff;
	}
	
	public void setTotemBuff(Float totemBuff){
		this.totemBuff = totemBuff;
	}
	
	public int getTotemHp(){
		return totemHp;
	}
	
	public void setTotemHp(int totemHp){
		this.totemHp = totemHp;
	}
	
	public int getTotemXp(){
		return totemXp;
	}
	
	public void setTotemXp(int totemXp){
		this.totemXp = totemXp;
	}
	
	public int getHpAltar(){
		return hpAltar;
	}
	
	public void setHpAltar(int hpAltar){
		this.hpAltar = hpAltar;
	}
	
	public int getAtcAltar(){
		return atcAltar;
	}
	
	public void setAtcAltar(int atcAltar){
		this.atcAltar = atcAltar;
	}
	
	public int getDefAltar(){
		return defAltar;
	}
	
	public void setDefAltar(int defAltar){
		this.defAltar = defAltar;
	}
	
	public int getWillAltar(){
		return willAltar;
	}
	
	public void setWillAltar(int willAltar){
		this.willAltar = willAltar;
	}
	
	public int getAltarXp(){
		return altarXp;
	}
	
	public void setAltarXp(int altarXp){
		this.altarXp = altarXp;
	}
}