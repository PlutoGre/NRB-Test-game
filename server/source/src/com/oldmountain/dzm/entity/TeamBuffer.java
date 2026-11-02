package com.oldmountain.dzm.entity;

public class TeamBuffer {
	private int playerid;//玩家的游戏ID
	private int healthbufferlevel;//血字诀等级
	private int healthbufferexp;//血字诀经验
	private int attackbufferlevel;//攻字诀等级
	private int attackbufferexp;//攻字诀经验
	private int defencebufferlevel;//防字诀等级
	private int defencebufferexp;//防字诀经验
	private int rpbufferlevel;//内字诀等级
	private int rpbufferexp;//内字诀经验
	
	public TeamBuffer(){
		this.healthbufferlevel = 0;
		this.healthbufferexp = 0;
		this.attackbufferlevel = 0;
		this.attackbufferexp = 0;
		this.defencebufferlevel = 0;
		this.defencebufferexp = 0;
		this.rpbufferlevel = 0;
		this.rpbufferexp = 0;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getHealthbufferlevel(){
		return healthbufferlevel;
	}
	
	public void setHealthbufferlevel(int level){
		this.healthbufferlevel = level;
	}
	
	public int getHealthbufferexp(){
		return healthbufferexp;
	}
	
	public void setHealthbufferexp(int exp){
		this.healthbufferexp = exp;
	}
	
	public int getAttackbufferlevel(){
		return attackbufferlevel;
	}
	
	public void setAttackbufferlevel(int level){
		this.attackbufferlevel = level;
	}
	
	public int getAttackbufferexp(){
		return attackbufferexp;
	}
	
	public void setAttackbufferexp(int exp){
		this.attackbufferexp = exp;
	}
	
	public int getDefencebufferlevel(){
		return defencebufferlevel;
	}
	
	public void setDefencebufferlevel(int level){
		this.defencebufferlevel = level;
	}
	
	public int getDefencebufferexp(){
		return defencebufferexp;
	}
	
	public void setDefencebufferexp(int exp){
		this.defencebufferexp = exp;
	}
	
	public int getRpbufferlevel(){
		return rpbufferlevel;
	}
	
	public void setRpbufferlevel(int level){
		this.rpbufferlevel = level;
	}
	
	public int getRpbufferexp(){
		return rpbufferexp;
	}
	
	public void setRpbufferexp(int exp){
		this.rpbufferexp = exp;
	}
}
