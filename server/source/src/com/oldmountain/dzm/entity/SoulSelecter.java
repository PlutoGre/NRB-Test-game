package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class SoulSelecter {
	private ArrayList<Integer> soulIds;
	private ArrayList<Integer> loginRates;
	private ArrayList<Integer> careerRates;
	
	public SoulSelecter(){
		this.soulIds = new ArrayList<Integer>();
		this.loginRates = new ArrayList<Integer>();
		this.careerRates = new ArrayList<Integer>();
	}
	
	public ArrayList<Integer>getSoulIds(){
		return soulIds;
	}
	
	public void setSoulIds(ArrayList<Integer>ids){
		this.soulIds = ids;
	}
	
	public ArrayList<Integer>getLoginRates(){
		return loginRates;
	}
	
	public void setLoginRates(ArrayList<Integer>loginRates){
		this.loginRates = loginRates;
	}
	
	public ArrayList<Integer> getCareerRates(){
		return careerRates;
	}
	
	public void setCareerRates(ArrayList<Integer>careerRates){
		this.careerRates = careerRates;
	}
}
