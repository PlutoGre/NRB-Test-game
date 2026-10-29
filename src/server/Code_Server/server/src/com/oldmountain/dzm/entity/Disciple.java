package com.oldmountain.dzm.entity;

import java.util.HashMap;

public class Disciple {
	private int id;//弟子的ID
	private int playerid;//属于的玩家id
	private int itemid;//弟子类型
	private int level;//弟子等级
	private int exp;//弟子当前经验
	private float health;//弟子的生命值
	private float attack;//弟子的攻击值
	private float defence;//弟子的防御值
	private float rp;//弟子的内力值
	private int upgradelevel;//弟子的突破等级
	private int potentiality;//弟子的剩余潜力值
	private int skill1;//弟子的技能1
	private int battleid;//是否上阵，及阵容ID
	private int expiretime;//弟子的删除时间
	private HashMap<String, HashMap<String, Integer>> trainstatus;//弟子上次培养的临时状态保存
	
	public Disciple(){
		this.level = 1;
		this.exp = 0;
		this.upgradelevel = 0;
		this.battleid = this.expiretime = 0;
	}
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getItemid(){
		return itemid;
	}
	
	public void setItemid(int itemid){
		this.itemid = itemid;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getExp(){
		return exp;
	}
	
	public void setExp(int exp){
		this.exp = exp;
	}
	
	public float getHealth(){
		return health;
	}
	
	public int getHealthToInt(){
		return Math.round(health);
	}
	
	public void setHealth(float health){
		this.health = health;
	}
	
	public float getAttack(){
		return attack;
	}
	
	public int getAttackToInt(){
		return Math.round(attack);
	}
	
	public void setAttack(float attack){
		this.attack = attack;
	}
	
	public float getDefence(){
		return defence;
	}
	
	public int getDefenceToInt(){
		return Math.round(defence);
	}
	
	public void setDefence(float defence){
		this.defence = defence;
	}
	
	public float getRp(){
		return rp;
	}
	
	public int getRpToInt(){
		return Math.round(rp);
	}
	
	public void setRp(float rp){
		this.rp = rp;
	}
	
	public int getUpgradelevel(){
		return upgradelevel;
	}
	
	public void setUpgradelevel(int upgradelevel){
		this.upgradelevel = upgradelevel;
	}
	
	public int getPotentiality(){
		return potentiality;
	}
	
	public void setPotentiality(int potentiality){
		this.potentiality = potentiality;
	}
	
	public int getSkill1(){
		return skill1;
	}
	
	public void setSkill1(int skill1){
		this.skill1 = skill1;
	}
	
	public int getBattleid(){
		return battleid;
	}
	
	public void setBattleid(int id){
		this.battleid = id;
	}
	
	public int getExpiretime(){
		return expiretime;
	}
	
	public void setExpiretime(int time){
		this.expiretime = time;
	}
	
	public HashMap<String, HashMap<String, Integer>> getTrainstatus(){
		return trainstatus;
	}
	
	public void setTrainstatus(HashMap<String, HashMap<String, Integer>> trainstatus){
		this.trainstatus = trainstatus;
	}
	public void increaseExp(int _exp){
		if(_exp>0){
			this.exp+=_exp;
		}
	}
}
