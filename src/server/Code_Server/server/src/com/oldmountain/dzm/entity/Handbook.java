package com.oldmountain.dzm.entity;


public class Handbook {
	private int playerId;
	private String discipleInfo;
	private String equipInfo;
	private String skillInfo;
	//0拥有，1见过，2没见过
	
	public Handbook(){
		
	}
	
	public Handbook(int playerId){
		this.playerId = playerId;
	}
	
	public Handbook(int playerId,String discipleInfo,String equipInfo,String skillInfo ){
		this.playerId = playerId;
		
		this.discipleInfo = discipleInfo;
		this.equipInfo = equipInfo;
		this.skillInfo = skillInfo;
		
	}

	public int getPlayerId() {
		return playerId;
	}

	public void setPlayerId(int playerId) {
		this.playerId = playerId;
	}

	public String getDiscipleInfo() {
		return discipleInfo;
	}

	public void setDiscipleInfo(String discipleInfo) {
		this.discipleInfo = discipleInfo;
	}

	public String getEquipInfo() {
		return equipInfo;
	}

	public void setEquipInfo(String equipInfo) {
		this.equipInfo = equipInfo;
	}

	public String getSkillInfo() {
		return skillInfo;
	}

	public void setSkillInfo(String skillInfo) {
		this.skillInfo = skillInfo;
	}

}
