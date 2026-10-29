package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class BattleArrayObject {
	private int playerid;//用户的游戏ID
	private int momentum;//整容的气势
	ArrayList<Battle> battlearrays;//玩家的整容
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getMomentum(){
		return momentum;
	}
	
	public void setMomentum(int momentum){
		this.momentum = momentum;
	}
	
	public ArrayList<Battle> getBattlearrays(){
		return battlearrays;
	}
	
	public void setBattlearrays(ArrayList<Battle>battlearrays ){
		this.battlearrays = battlearrays;
	}
}
