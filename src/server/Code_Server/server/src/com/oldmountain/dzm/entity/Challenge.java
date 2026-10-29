package com.oldmountain.dzm.entity;

import com.oldmountain.dzm.util.Util;

public class Challenge {
	private int playerid;
	private int level;//等级
	private String name;//名称
	private int isnpc;//是否是NPC
	private int rank;//排名
	private int highestrank;//最高排名
	private int score;//剩余积分
	private int lastsettletime;//上次积分结算时间
	private String hasgotreward;//已经领取过的只能领一次奖励的id
	private int lastrefreshtime;//上次刷新时间
	
	public Challenge(){
		this.isnpc = 0;
		this.score = 0;
		this.lastsettletime = 0;
		this.hasgotreward = "";
		this.lastrefreshtime = Util.getServerTime();
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerId){
		this.playerid = playerId;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name = name;
	}
	
	public int getIsnpc(){
		return isnpc;
	}
	
	public void setIsnpc(int isNpc){
		this.isnpc = isNpc;
	}
	
	public int getRank(){
		return rank;
	}
	
	public void setRank(int rank){
		this.rank = rank;
	}
	
	public int getHighestrank(){
		return highestrank;
	}
	
	public void setHighestrank(int rank){
		this.highestrank = rank;
	}
	
	public int getScore(){
		return score;
	}
	
	public void setScore(int score){
		this.score = score;
	}
	
	public int getLastsettletime(){
		return lastsettletime;
	}
	
	public void setLastsettletime(int time){
		this.lastsettletime = time;
	}
	
	public String getHasgotreward(){
		return hasgotreward;
	}
	
	public void setHasgotreward(String rewards){
		this.hasgotreward = rewards;
	}
	
	public int getLastrefreshtime(){
		return lastrefreshtime;
	}
	
	public void setLastrefreshtime(int time){
		this.lastrefreshtime = time;
	}
}
