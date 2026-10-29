package com.oldmountain.dzm.entity;

public class RelationShip {
	private int playerid;//玩家id
	private String friendids;//好友id
	private String enemyids;//仇敌id
	private String confirm;//待确认列表
	public RelationShip(){
		this.playerid=0;
		this.friendids="{}";
		this.enemyids="{}";
		this.confirm="{}";
	}
	
	public int getPlayerid() {
		return playerid;
	}
	public void setPlayerid(int playerid) {
		this.playerid = playerid;
	}
	public String getFriendids() {
		return friendids;
	}
	public void setFriendids(String friendids) {
		this.friendids = friendids;
	}
	public String getEnemyids() {
		return enemyids;
	}
	public void setEnemyids(String enemyids) {
		this.enemyids = enemyids;
	}

	public String getConfirm() {
		return confirm;
	}

	public void setConfirm(String confirm) {
		this.confirm = confirm;
	}
	
}
