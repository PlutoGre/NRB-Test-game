package com.oldmountain.dzm.entity;
import java.util.ArrayList;
import java.util.List;

public class KindSelecter {
	private List<Integer> kinds;
	private List<Integer> rate1;
	private List<Integer> rate2;
	private List<Integer> rate3;
	private List<Integer> rate4;
	private List<Integer> rate5;
	private List<Integer> rate6;
	
	public KindSelecter(){
		this.kinds = new ArrayList<Integer>();
		this.rate1 = new ArrayList<Integer>();
		this.rate2 = new ArrayList<Integer>();
		this.rate3 = new ArrayList<Integer>();
		this.rate4 = new ArrayList<Integer>();
		this.rate5 = new ArrayList<Integer>();
		this.rate6 = new ArrayList<Integer>();
	}
	
	public List<Integer> getKinds(){
		return kinds;
	}
	
	public void setKinds(List<Integer> kinds){
		this.kinds = kinds;
	}
	
	public List<Integer> getRate1(){
		return rate1;
	}
	
	public void setRate1(List<Integer> rate){
		this.rate1 = rate;
	}
	
	public List<Integer> getRate2(){
		return rate2;
	}
	
	public void setRate2(List<Integer> rate){
		this.rate2 = rate;
	}
	
	public List<Integer> getRate3(){
		return rate3;
	}
	
	public void setRate3(List<Integer> rate){
		this.rate3 = rate;
	}
	
	public List<Integer> getRate4(){
		return rate4;
	}
	
	public void setRate4(List<Integer> rate){
		this.rate4 = rate;
	}
	
	public List<Integer> getRate5(){
		return rate5;
	}
	
	public void setRate5(List<Integer> rate){
		this.rate5 = rate;
	}
	
	public List<Integer> getRate6(){
		return rate6;
	}
	
	public void setRate6(List<Integer> rate){
		this.rate6 = rate;
	}
}
