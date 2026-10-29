package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class SkillbookSelecter {
	private List<Integer> id;
	private List<Integer> deepWaterRat1;
	private List<Integer> deepWaterRat2;
	private List<Integer> deepWaterRat3;
	public SkillbookSelecter(){
		this.id=new ArrayList<Integer>();
		this.deepWaterRat1=new ArrayList<Integer>();
		this.deepWaterRat2=new ArrayList<Integer>();
		this.deepWaterRat3=new ArrayList<Integer>();
	}
	public List<Integer> getId() {
		return id;
	}
	public void setId(List<Integer> id) {
		this.id = id;
	}
	public List<Integer> getDeepWaterRat1() {
		return deepWaterRat1;
	}
	public void setDeepWaterRat1(List<Integer> deepWaterRat1) {
		this.deepWaterRat1 = deepWaterRat1;
	}
	public List<Integer> getDeepWaterRat2() {
		return deepWaterRat2;
	}
	public void setDeepWaterRat2(List<Integer> deepWaterRat2) {
		this.deepWaterRat2 = deepWaterRat2;
	}
	public List<Integer> getDeepWaterRat3() {
		return deepWaterRat3;
	}
	public void setDeepWaterRat3(List<Integer> deepWaterRat3) {
		this.deepWaterRat3 = deepWaterRat3;
	}
	
}
