package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class Actions {
	private int type;//操作的类型，0是普通攻击，1是技能攻击
	 private int position;//操作者ID
	 private List<Damage> damages;//造成伤害的信息
	 private int skillId;//如果释放了技能，则表示技能ID
	 
	 public Actions(){
		 this.damages = new ArrayList<Damage>();
	 }
	 
	 public int getType(){
		 return type;
	 }
	 
	 public void setType(int type){
		 this.type = type;
	 }
	 
	 public int getPosition(){
		 return position;
	 }
	 
	 public void setPosition(int position){
		 this.position = position;
	 }
	 
	 public List<Damage> getDamages(){
		 return damages;
	 }
	 
	 public void setDamages(List<Damage> damages){
		 this.damages = damages;
	 }
	 
	 public int getSkillId(){
		 return skillId;
	 }
	 
	 public void setSkillId(int id){
		 this.skillId = id;
	 }
}
