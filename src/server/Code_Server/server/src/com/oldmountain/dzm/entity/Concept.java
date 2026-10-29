package com.oldmountain.dzm.entity;

/**
 * 掌门决信息
 */
public class Concept {
	private int id;
	private int userId;
	private int teamId;
	private int teamLevel;
	private int currBoldness;
	
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}
	public int getUserId() {
		return userId;
	}
	public void setUserId(int userId) {
		this.userId = userId;
	}
	public int getTeamId() {
		return teamId;
	}
	public void setTeamId(int teamId) {
		this.teamId = teamId;
	}
	public int getTeamLevel() {
		return teamLevel;
	}
	public void setTeamLevel(int teamLevel) {
		this.teamLevel = teamLevel;
	}
	public int getCurrBoldness() {
		return currBoldness;
	}
	public void setCurrBoldness(int currBoldness) {
		this.currBoldness = currBoldness;
	}
	
}
