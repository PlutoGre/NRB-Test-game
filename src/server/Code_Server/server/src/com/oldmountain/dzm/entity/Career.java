package com.oldmountain.dzm.entity;

public class Career{
	private int playerid;//玩家的ID
	private int currentchapter;//玩家当前章节
	private int currentpoint;//玩家当前节点
	private String chapteraddpower;//章节3星通过叫花鸡状态
	private String pointevaluate;//关卡通过评星
	private String pointcount;//关卡当日攻打次数
	private int checktime;//上次检查时间
	
	public Career(){
		this.playerid = 0;
		this.currentchapter = 1;
		this.currentpoint = 103;
		this.chapteraddpower = "{}";
		this.pointevaluate = "{\"101\":3,\"102\":3}";
		this.pointcount = "{}";
		this.checktime = 0;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int id){
		this.playerid = id;
	}
	
	public int getCurrentchapter(){
		return currentchapter;
	}
	
	public void setCurrentchapter(int chapter){
		this.currentchapter = chapter;
	}
	
	public int getCurrentpoint(){
		return currentpoint;
	}
	
	public void setCurrentpoint(int point){
		this.currentpoint = point;
	}
	
	public String getChapteraddpower(){
		return chapteraddpower;
	}
	
	public void setChapteraddpower(String s){
		this.chapteraddpower = s;
	}
	
	public String getPointevaluate(){
		return pointevaluate;
	}
	
	public void setPointevaluate(String s){
		this.pointevaluate = s;
	}
	
	public String getPointcount(){
		return pointcount;
	}
	
	public void setPointcount(String s){
		this.pointcount = s;
	}
	
	public int getChecktime(){
		return checktime;
	}
	
	public void setChecktime(int time){
		this.checktime = time;
	}
}
