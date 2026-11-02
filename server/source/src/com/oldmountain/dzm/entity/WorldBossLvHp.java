package com.oldmountain.dzm.entity;

public class WorldBossLvHp {
	private int level;
	private long hp;
	public WorldBossLvHp(){
		this.level=0;
		this.hp=0;
	}
	public int getLevel() {
		return level;
	}
	public void setLevel(int level) {
		this.level = level;
	}
	public long getHp() {
		return hp;
	}
	public void setHp(long hp) {
		this.hp = hp;
	}
	
}
