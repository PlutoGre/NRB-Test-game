package com.oldmountain.dzm.entity;

import java.util.ArrayList;

public class BroadCast {
	private int type;//广播类型 (1为拼合残章，2为开箱子开出甲等武功或装备，3为参拜出甲等装备或武功,4为集市招募出甲等弟子,5为参悟武功到指定等级，6为装备精练到指定等级)
	private String name;//广播的发出者姓名
	private ArrayList<Integer> params = new ArrayList<Integer>(5);//拼合广播需要的各种参数
	
	public int getType(){
		return type;
	}
	
	public void setType(int type){
		this.type = type;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String name){
		this.name = name;
	}
	
	public ArrayList<Integer> getParams(){
		return params;
	}
	
	public void setParams(ArrayList<Integer> params){
		this.params = params;
	}
}
