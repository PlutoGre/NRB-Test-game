package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class GiveSoulSelecter {
	private List<Integer> id;
	private List<Integer> typeid;
	private List<Integer> soulItemid;
	private List<Integer> count1;
	private List<Integer> count2;
	private List<Integer> rate1;
	private List<Integer> rate2;
	private List<Integer> rate3;
	private List<Integer> rate4;
	private List<Integer> rate5;
	public GiveSoulSelecter(){
        this.id = new ArrayList<Integer>();
        this.typeid = new ArrayList<Integer>();
        this.soulItemid = new ArrayList<Integer>();
        this.count1 = new ArrayList<Integer>();
        this.count2 = new ArrayList<Integer>();
        this.rate1 = new ArrayList<Integer>();
        this.rate2 = new ArrayList<Integer>();
        this.rate3 = new ArrayList<Integer>();
        this.rate4 = new ArrayList<Integer>();
        this.rate5 = new ArrayList<Integer>();
    }
	public List<Integer> getId() {
		return id;
	}
	public void setId(List<Integer> id) {
		this.id = id;
	}
	public List<Integer> getTypeid() {
		return typeid;
	}
	public void setTypeid(List<Integer> typeid) {
		this.typeid = typeid;
	}
	public List<Integer> getSoulItemid() {
		return soulItemid;
	}
	public void setSoulItemid(List<Integer> soulItemid) {
		this.soulItemid = soulItemid;
	}
	public List<Integer> getCount1() {
		return count1;
	}
	public void setCount1(List<Integer> count1) {
		this.count1 = count1;
	}
	public List<Integer> getCount2() {
		return count2;
	}
	public void setCount2(List<Integer> count2) {
		this.count2 = count2;
	}
	public List<Integer> getRate1() {
		return rate1;
	}
	public void setRate1(List<Integer> rate1) {
		this.rate1 = rate1;
	}
	public List<Integer> getRate2() {
		return rate2;
	}
	public void setRate2(List<Integer> rate2) {
		this.rate2 = rate2;
	}
	public List<Integer> getRate3() {
		return rate3;
	}
	public void setRate3(List<Integer> rate3) {
		this.rate3 = rate3;
	}
	public List<Integer> getRate4() {
		return rate4;
	}
	public void setRate4(List<Integer> rate4) {
		this.rate4 = rate4;
	}
	public List<Integer> getRate5() {
		return rate5;
	}
	public void setRate5(List<Integer> rate5) {
		this.rate5 = rate5;
	}
	
}
