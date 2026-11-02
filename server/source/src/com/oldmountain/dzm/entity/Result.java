package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class Result {
	private int winlose;//输赢情况：BIGWIN = 0;WIN = 1;JUSTWIN = 2;JUSTLOSE = 3;LOSE = 4;BIGLOSE = 5;
	private int gotCoin;//获得的银币
	private int gotExp;//获得的经验
	private int gotGold;//获得的元宝
	private int gotItemType;//获得物品的类型
	private int gotItemId;//获得的物品的Id
	private int gotItemCount;//获得物品的个数
	private ArrayList<CharacterResult> characterResults;//人物结果列表
	
	private boolean canViewTeam;//是否可以查看阵容
	
	public Result(){
		this.winlose = this.gotCoin = this.gotExp = this.gotGold = this.gotItemId = this.gotItemCount = 0;
		this.gotItemType = -1;
		this.characterResults = new ArrayList<CharacterResult>();
		this.canViewTeam = false;
		
	}
	
	public int getWinlose(){
		return winlose;
	}
	
	public void setWinlose(int winlose){
		this.winlose = winlose;
	}
	
	public int getGotCoin(){
		return gotCoin;
	}
	
	public void setGotCoin(int gotCoin){
		this.gotCoin = gotCoin;
	}
	
	public int getGotExp(){
		return gotExp;
	}
	
	public void setGotExp(int gotExp){
		this.gotExp = gotExp;
	}
	
	public int getGotGold(){
		return gotGold;
	}
	
	public void setGotGold(int gotGold){
		this.gotGold = gotGold;
	}
	
	public int getGotItemId(){
		return gotItemId;
	}
	
	public void setGotItemId(int id){
		this.gotItemId = id;
	}
	
	public ArrayList<CharacterResult> getCharacterResults(){
		return characterResults;
	}
	
	public void setCharacterResults(ArrayList<CharacterResult> characterResults){
		this.characterResults = characterResults;
	}
	
	public boolean getCanViewTeam(){
		return canViewTeam;
	}
	
	public void setCanViewTeam(boolean canView){
		this.canViewTeam = canView;
	}
	
	public int getGotItemType(){
		return gotItemType;
	}
	
	public void setGotItemType(int type){
		this.gotItemType = type;
	}
	
	public int getGotItemCount(){
		return gotItemCount;
	}
	
	public void setGotItemCount(int count){
		this.gotItemCount = count;
	}
}
