package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

import com.oldmountain.dzm.services.LeaguaServices;

public class LeaguaObject {
	private Leagua leaguaInfo;
	private HashMap<Integer, LeaguaMembers> leaguaMembersInfo;
	private HashMap<Integer,LeaguaMedal> leaguaMedals;
	private HashMap<Integer, LeaguaApply> leaguaApplys;
	private boolean isDirty;
	private HashMap<String, HashSet<Integer>> needUpdateObj = new HashMap<String, HashSet<Integer>>();
	
	public LeaguaObject() {
		this.leaguaInfo = null;
		this.leaguaMembersInfo = new HashMap<Integer, LeaguaMembers>();
		this.leaguaMedals = new HashMap<Integer,LeaguaMedal>();
		this.leaguaApplys = new HashMap<Integer, LeaguaApply>();
		this.isDirty = true;
	}
	
	public Leagua getLeaguaInfo(){
		return leaguaInfo;
	}
	
	public void setLeaguaInfo(Leagua leagua){
		this.leaguaInfo = leagua;
	}
	
	public void updateLeaguaInfo(Leagua leagua){
		this.leaguaInfo = leagua;
		LeaguaServices.allLeagua.put(leagua.getLeaguaId(), leagua);
		this.isDirty = true;
	}
	
	public HashMap<Integer, LeaguaMembers> getLeaguaMembersInfo(){
		return leaguaMembersInfo;
	}
	
	public void setLeaguaMembersInfo(HashMap<Integer, LeaguaMembers> leaguaMembersInfo){
		this.leaguaMembersInfo = leaguaMembersInfo;
	}
	
	public void updateOneMember(LeaguaMembers member){
		this.leaguaMembersInfo.put(member.getPlayerId(), member);
		addNeedUpdate("members", member.getPlayerId());
		
	}
	
	public HashMap<Integer,LeaguaMedal> getLeaguaMedals(){
		return leaguaMedals;
	}
	
	public void setLeaguaMedals(HashMap<Integer,LeaguaMedal> leaguaMedals){
		this.leaguaMedals = leaguaMedals;
	}
	
	public void updateOneMedal(LeaguaMedal medal){
		this.leaguaMedals.put(medal.getMedalId(), medal);
		addNeedUpdate("medals", medal.getMedalId());
	}
	
	public HashMap<Integer, LeaguaApply> getLeaguaApplys(){
		return leaguaApplys;
	}
	
	public void setLeaguaApplys(HashMap<Integer, LeaguaApply> leaguaApplys){
		this.leaguaApplys = leaguaApplys;
	}
	
	public void addNeedUpdate(String type,int id){
    	if (needUpdateObj.containsKey(type)) {
			needUpdateObj.get(type).add(id);
		}
    	else {
			HashSet<Integer> needUpdateHashSet = new HashSet<Integer>();
			needUpdateHashSet.add(id);
			needUpdateObj.put(type, needUpdateHashSet);
		}
    	this.isDirty = true;
    }
	
	public void clearNeedUpdate(){
    	this.isDirty = false;
    	this.needUpdateObj = new HashMap<String, HashSet<Integer>>();
    }
	
	public boolean getIsDirty(){
    	return isDirty;
    }
    
    public void setIsDirty(boolean isDirty){
    	this.isDirty = isDirty;
    }
    
    public HashMap<String, HashSet<Integer>> getNeedUpdate(){
    	return needUpdateObj;
    }
}
