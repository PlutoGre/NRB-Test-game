package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class FinalRound {
	private List<Characters>characters;//人物列表
	private boolean win;//输赢情况
	private boolean gotto;//是否到了这个回合
	
	public FinalRound(){
		this.characters = new ArrayList<Characters>();
		this.win = false;
		this.gotto = false;
	}
	
	public List<Characters> getCharacters(){
		return characters;
	}
	
	public void setCharacters(List<Characters>characters){
		this.characters = characters;
	}
	
	public boolean getWin(){
		return win;
	}
	
	public void setWin(boolean win){
		this.win = win;
	}
	
	public boolean getGotto(){
		return gotto;
	}
	
	public void setGotto(boolean gotto){
		this.gotto = gotto;
	}
}
