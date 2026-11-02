package com.oldmountain.dzm.entity;

import java.text.SimpleDateFormat;
import java.util.Date;

public class WorldBossRank implements Comparable<WorldBossRank>{
	private int id;
	private int playerid;
	private int rank;
	private String name;
	private int level;
	private int damage;
	private int ranktime;
	
	public WorldBossRank(){
		SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");
		Date today = new Date();
		ranktime = Integer.valueOf(sdf.format(today));
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
	
	public void setPlayerid(int id){
		this.playerid = id;
	}
	
	public int getRank(){
		return rank;
	}
	
	public void setRank(int rank){
		this.rank = rank;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name = name;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getDamage(){
		return damage;
	}
	
	public void setDamage(int damage){
		this.damage = damage;
	}
	
	public int getRanktime(){
		return ranktime;
	}
	
	public void setRanktime(int time){
		this.ranktime = time;
	}
	
	public int compareTo(WorldBossRank o){
		return this.damage - o.damage;
	}
}
