package com.oldmountain.dzm.entity;

import net.sf.json.JSONObject;

public class InviteStatus {
	private int playerid;//用户id
	private JSONObject invitestatus;//各个条件的完成情况
	private JSONObject rewardstatus;//领取的奖励情况
	
	public InviteStatus(){
		this.invitestatus = new JSONObject();
		this.rewardstatus = new JSONObject();
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public String getInvitestatus(){
		return invitestatus.toString();
	}
	
	public JSONObject getInvitestatusInObject(){
		return invitestatus;
	}
	
	public void setInvitestatus(String invitestatus){
		this.invitestatus = JSONObject.fromObject(invitestatus);
	}
	
	public void addInvitestatusByType(int type,int addnum){
		int originnum = 0;
		if (this.invitestatus != null && this.invitestatus.containsKey(String.valueOf(type))) {
			originnum = this.invitestatus.getInt(String.valueOf(type));
		}
		this.invitestatus.put(String.valueOf(type), originnum + addnum);
	}
	
	public String getRewardstatus(){
		return rewardstatus.toString();
	}
	
	public JSONObject getRewardstatusInObject(){
		return rewardstatus;
	}
	
	public void setRewardstatus(String rewardstatus){
		this.rewardstatus = JSONObject.fromObject(rewardstatus);
	}
	
	public void setRewardstatusByType(int type,int num){
		this.rewardstatus.put(String.valueOf(type), num);
	}
}
