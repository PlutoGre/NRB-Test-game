package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

public class Battle implements Cloneable{
	private int id;//阵容在数据库里面的ID
	private int playerid;//阵容所属的玩家ID
	private int discipleid;//阵容对应的弟子ID
	private int itemid;//弟子的xml配置ID
	private int level;//弟子的等级
	private int attack;//阵容的攻击力
	private int defence;//阵容的防御力
	private int health;//阵容的血量
	private int rp;//阵容的内力值
	private int defaultskill;//阵容弟子自带的默认技能
	private int defaultskillitem;//阵容弟子自带的默认技能itemid
	private int defaultskilltype;//默认技能类型
	private float defaultskillquantity;//默认技能的强度
	private int skill1;//第二个位置的技能
	private int skill1item;//第二个位置的技能itemid
	private int skill1type;//第二个位置的技能类型
	private float skill1quantity;//第二个位置的技能强度
	private int skill2;//第三个位置的技能
	private int skill2item;//第三个位置的技能itemid
	private int skill2type;//第三个位置的技能类型
	private float skill2quantity;//第三个位置的技能强度
	private int equip1;//阵容上的武器
	private int equip1item;//阵容上的武器itemid
	private int equip2;//阵容上的防具
	private int equip2item;//阵容上的防具itemid
	private int equip3;//阵容上的饰品
	private int equip3item;//阵容上的饰品itemid
	private String fates;//阵容的缘
	private String fatesstatus;//阵容缘的激活情况
	
	public Battle(){
		this.discipleid = this.itemid = this.level = this.attack = this.defence = this.health = this.rp = 0;
		this.defaultskill = this.defaultskilltype = this.skill1  = this.skill1type = this.skill2 =  this.skill2type = 0;
		this.defaultskillquantity = this.skill1quantity = this.skill2quantity = 0;
		this.equip1 = this.equip2 = this.equip3 = 0;
		this.fates = this.fatesstatus = "";
	}
	
	public Battle clone(){
		Battle copy = new Battle();
		try {
			copy = (Battle)super.clone();
		} catch (Exception e) {
			// TODO: handle exception
		}
		return copy;
	}
	
	public int getId(){
		return id;
	}
	
	public void setId(int id){
		this.id = id;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerId){
		this.playerid = playerId;
	}
	
	public int getDiscipleid(){
		return discipleid;
	}
	
	public void setDiscipleid(int discipleid){
		this.discipleid = discipleid;
	}
	
	public int getItemid(){
		return itemid;
	}
	
	public void setItemid(int itemid){
		this.itemid = itemid;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getAttack(){
		return attack;
	}
	
	public void setAttack(int attack){
		this.attack = attack;
	}
	
	public int getDefence(){
		return defence;
	}
	
	public void setDefence(int defence){
		this.defence = defence;
	}
	
	public int getHealth(){
		return health;
	}
	
	public void setHealth(int health){
		this.health = health;
	}
	
	public int getRp(){
		return rp;
	}
	
	public void setRp(int rp){
		this.rp = rp;
	}
	
	public int getDefaultskill(){
		return defaultskill;
	}
	
	public void setDefaultskill(int defaultskill){
		this.defaultskill = defaultskill;
	}
	
	public int getDefaultskilltype(){
		return defaultskilltype;
	}
	
	public void setDefaultskilltype(int type){
		this.defaultskilltype = type;
	}
	
	public float getDefaultskillquantity(){
		return defaultskillquantity;
	}
	
	public void setDefaultskillquantity(float quantity){
		this.defaultskillquantity = quantity;
	}
	
	public int getSkill1(){
		return skill1;
	}
	
	public void setSkill1(int skill1){
		this.skill1 = skill1;
	}
	
	public int getSkill1type(){
		return skill1type;
	}
	
	public void setSkill1type(int type){
		this.skill1type = type;
	}
	
	public float getSkill1quantity(){
		return skill1quantity;
	}
	
	public void setSkill1quantity(float quantity){
		this.skill1quantity = quantity;
	}
	
	public int getSkill2(){
		return skill2;
	}
	
	public void setSkill2(int skill2){
		this.skill2 = skill2;
	}
	
	public int getSkill2type(){
		return skill2type;
	}
	
	public void setSkill2type(int type){
		this.skill2type = type;
	}
	
	public float getSkill2quantity(){
		return skill2quantity;
	}
	
	public void setSkill2quantity(float quantity){
		this.skill2quantity = quantity;
	}
	
	public int getEquip1(){
		return equip1;
	}
	
	public void setEquip1(int equip){
		this.equip1 = equip;
	}
	
	public int getEquip2(){
		return equip2;
	}
	
	public void setEquip2(int equip){
		this.equip2 = equip;
	}
	
	public int getEquip3(){
		return equip3;
	}
	
	public void setEquip3(int equip){
		this.equip3 = equip;
	}
	
	public String getFates(){
		return fates;
	}
	
	public List<Integer> getFatesToList(){
		String[] fatesArray = this.fates.split(",");
		List<Integer> returnFates = new ArrayList<Integer>();
		for (int i = 0; i < fatesArray.length; i++) {
			returnFates.add(Integer.parseInt(fatesArray[i]));
		}
		return returnFates;
	}
	
	public void setFates(String fates){
		this.fates = fates;
	}
	
	public String getFatesstatus(){
		return fatesstatus;
	}
	
	public List<Integer> getFatesstatusToList(){
		String[] fatestatusArray = this.fatesstatus.split(",");
		List<Integer> returnFatestatus = new ArrayList<Integer>();
		for (int i = 0; i < fatestatusArray.length; i++) {
			returnFatestatus.add(Integer.parseInt(fatestatusArray[i]));
		}
		return returnFatestatus;
	}
	
	public void setFatesstatus(String status){
		this.fatesstatus = status;
	}
	
	public int getSkill1item(){
		return skill1item;
	}
	
	public void setSkill1item(int itemid){
		this.skill1item = itemid;
	}
	
	public int getSkill2item(){
		return skill2item;
	}
	
	public void setSkill2item(int itemid){
		this.skill2item = itemid;
	}
	
	public int getEquip1item(){
		return equip1item;
	}
	
	public void setEquip1item(int item){
		this.equip1item = item;
	}
	
	public int getEquip2item(){
		return equip2item;
	}
	
	public void setEquip2item(int item){
		this.equip2item = item;
	}
	
	public int getEquip3item(){
		return equip3item;
	}
	
	public void setEquip3item(int item){
		this.equip3item = item;
	}
	
	public int getDefaultskillitem(){
		return defaultskillitem;
	}
	
	public void setDefaultskillitem(int itemid){
		this.defaultskillitem = itemid;
	}
}