package com.oldmountain.dzm.configEntity;

import java.util.ArrayList;

public class GuildNpcConfig {
	private ArrayList<Integer> npcIds = new ArrayList<Integer>();
	private int npcLevel;
	private float skillRatios;
	private float hpBuff;
	private float attackBuff;
	private float defenceBuff;
	private float willBuff;
	private int npcRate;
	private int memberRate;
	
	public ArrayList<Integer> getNpcIds(){
		return npcIds;
	}
	
	public void setNpcIds(String idsString){
		String[] ids = idsString.split(",");
		for (int i = 0; i < ids.length; i++) {
			npcIds.add(Integer.valueOf(ids[i]));
		}
	}
	
	public int getNpcLevel(){
		return npcLevel;
	}
	
	public void setNpcLevel(int level){
		this.npcLevel = level;
	}
	
	public float getSkillRatios(){
		return skillRatios;
	}
	
	public void setSkillRatios(float ratio){
		this.skillRatios = ratio;
	}
	
	public float getHpBuff(){
		return hpBuff;
	}
	
	public void setHpBuff(float hpBuff){
		this.hpBuff = hpBuff;
	}
	
	public float getAttackBuff(){
		return attackBuff;
	}
	
	public void setAttackBuff(float buff){
		this.attackBuff = buff;
	}
	
	public float getDefenceBuff(){
		return defenceBuff;
	}
	
	public void setDefenceBuff(float buff){
		this.defenceBuff = buff;
	}
	
	public float getWillBuff(){
		return willBuff;
	}
	
	public void setWillBuff(float buff){
		this.willBuff = buff;
	}
	
	public int getNpcRate(){
		return npcRate;
	}
	
	public void setNpcRate(int rate){
		this.npcRate = rate;
	}
	
	public int getMemberRate(){
		return memberRate;
	}
	
	public void setmemberRate(int rate){
		this.memberRate = rate;
	}
}
