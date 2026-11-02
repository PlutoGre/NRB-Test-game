package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class Round {
	private List<Characters> characters;//回合开始的阵容属性
	private List<Actions> actions;//回合里面的操作
	private boolean gotTo;//是否到了这个回合
	
	public Round(){
		this.characters = new ArrayList<Characters>();
		this.actions = new ArrayList<Actions>();
		this.gotTo = false;
	}
	
	public List<Characters> getCharacters(){
		return characters;
	}
	
	public void setCharacters(List<Characters>characters){
		this.characters = characters;
	}
	
	public List<Actions> getActions(){
		return actions;
	}
	
	public void setActions(List<Actions> actions){
		this.actions = actions;
	}
	
	public boolean getGotTo(){
		return gotTo;
	}
	
	public void setGotTO(boolean gotto){
		this.gotTo = gotto;
	}
}
