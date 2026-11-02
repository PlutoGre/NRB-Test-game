package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class ContinueLoginXmlTemplate {
	private ArrayList<Integer> types = new ArrayList<Integer>();
	private ArrayList<Integer> ids = new ArrayList<Integer>();
	private ArrayList<Integer> counts = new ArrayList<Integer>();
	private ArrayList<Integer> rates = new ArrayList<Integer>();
	
	public ArrayList<Integer> getTypes(){
		return types;
	}
	
	public void setTypes(ArrayList<Integer> types){
		this.types = types;
	}
	
	public ArrayList<Integer> getIds(){
		return ids;
	}
	
	public void setIds(ArrayList<Integer>ids){
		this.ids = ids;
	}
	
	public ArrayList<Integer> getCounts(){
		return counts;
	}
	
	public void setCounts(ArrayList<Integer> counts){
		this.counts = counts;
	}
	
	public ArrayList<Integer> getRates(){
		return rates;
	}
	
	public void setRates(ArrayList<Integer>rates){
		this.rates = rates;
	}
}
