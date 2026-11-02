package com.oldmountain.dzm.entity;


public class DiscipleXmlTemplate {
	private int id;//弟子配置Id
	private int quality;//弟子的品质
	private int group;//弟子的阵营
	private int defaultSkill;//默认技能
	private int defaultHealth;//默认血量
	private int defaultAttack;//默认攻击值
	private int defaultDefence;//默认防御值
	private int defaultRp;//默认内力
	private int defaultPotentiality;//默认潜力值
	private int defaultExp;//默认经验
	private float growHealth;//血量成长值
	private float growAttack;//攻击成长值
	private float growDefence;//防御成长值
	private float growRp;//内力成长值
	private String fates;//弟子的缘
	
	
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}
	public int getQuality() {
		return quality;
	}
	public void setQuality(int quality) {
		this.quality = quality;
	}
	
	public int getGroup(){
		return group;
	}
	
	public void setGroup(int group){
		this.group = group;
	}
	
	public int getDefaultSkill(){
		return defaultSkill;
	}
	
	public void setDefaultSkill(int skillId){
		this.defaultSkill = skillId;
	}
	
	public int getDefaultHealth(){
		return defaultHealth;
	}
	
	public void setDefaultHealth(int health){
		this.defaultHealth = health;
	}
	
	public int getDefaultAttack(){
		return defaultAttack;
	}
	
	public void setDefaultAttack(int attack){
		this.defaultAttack = attack;
	}
	
	public int getDefaultDefence(){
		return defaultDefence;
	}
	
	public void setDefaultDefence(int defence){
		this.defaultDefence = defence;
	}
	
	public int getDefaultRp(){
		return defaultRp;
	}
	
	public void setDefaultRp(int rp){
		this.defaultRp = rp;
	}
	
	public int getDefaultPotentiality(){
		return defaultPotentiality;
	}
	
	public void setDefaultPotentiality(int p){
		this.defaultPotentiality = p;
	}
	
	public int getDefaultExp(){
		return defaultExp;
	}
	
	public void setDefaultExp(int exp){
		this.defaultExp = exp;
	}
	
	public float getGrowHealth(){
		return growHealth;
	}
	
	public void setGrowHealth(float health){
		this.growHealth = health;
	}
	
	public float getGrowAttack(){
		return growAttack;
	}
	
	public void setGrowAttack(float attack){
		this.growAttack = attack;
	}
	
	public float getGrowDefence(){
		return growDefence;
	}
	
	public void setGrowDefence(float defence){
		this.growDefence = defence;
	}
	
	public float getGrowRp(){
		return growRp;
	}
	
	public void setGrowRp(float rp){
		this.growRp = rp;
	}
	
	public String getFates(){
		return fates;
	}
	
	public void setFates(String fates){
		this.fates = fates;
	}
}
