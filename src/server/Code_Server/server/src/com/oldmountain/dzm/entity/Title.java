package com.oldmountain.dzm.entity;

public class Title {
	private int playerid;//用户ID
	private int careertypeid;//江湖类型称号ID
	private int careertypenum;//江湖类型称号提升额度
	private int viptypeid;//vip等级类型称号ID
	private float viptypenum;//vip等级称号提升额度
	private int challengetypeid;//论剑类型称号ID
	private int challengetypenum;//论剑类型称号提升额度
	private int chiptypeid;//拼合残障类型称号ID
	private int chiptypenum;//拼合残障类型称号提升额度
	private int fighttypeid;//血战次数类型称号ID
	private int fighttypenum;//血战次数类型称号提升额度
	private int battletypeid;//群雄破阵次数类型称号ID
	private int battletypenum;//群雄破阵次数称号提升额度
	private int friendtypeid;//好友数类型称号ID
	private int friendtypenum;//好友数类型称号提升额度
	private int fightrankid;//血战排名类型称号ID
	private float fightranknum;//血战排名类型称号提升额度
	private int fightranktime;//血战排名称号生成时间
	private int battlerankid;//群雄破阵排名类型称号ID
	private float battleranknum;//群雄破阵排名称号提升额度
	private int battleranktime;//群雄破阵排名称号生成时间
	
	public Title(){
		this.careertypeid = 0;
		this.careertypenum = 0;
		this.viptypeid = 0;
		this.viptypenum = 0;
		this.challengetypeid = 0;
		this.challengetypenum = 0;
		this.chiptypeid = 0;
		this.chiptypenum = 0;
		this.fighttypeid = 0;
		this.fighttypenum = 0;
		this.battletypeid = 0;
		this.battletypenum = 0;
		this.friendtypeid = 0;
		this.friendtypenum = 0;
		this.fightrankid = 0;
		this.fightranknum = 0;
		this.fightranktime = 0;
		this.battlerankid = 0;
		this.battleranknum = 0;
		this.battleranktime = 0;
	}
	
	public int getPlayerid(){
		return playerid;
	}
	
	public void setPlayerid(int playerid){
		this.playerid = playerid;
	}
	
	public int getCareertypeid(){
		return careertypeid;
	}
	
	public void setCareertypeid(int type){
		this.careertypeid = type;
	}
	
	public int getCareertypenum(){
		return careertypenum;
	}
	
	public void setCareertypenum(int num){
		this.careertypenum = num;
	}
	
	public int getViptypeid(){
		return viptypeid;
	}
	
	public void setViptypeid(int type){
		this.viptypeid = type;
	}
	
	public float getViptypenum() {
		return viptypenum;
	}

	public void setViptypenum(float viptypenum) {
		this.viptypenum = viptypenum;
	}

	public int getChallengetypeid(){
		return challengetypeid;
	}
	
	public void setChallengetypeid(int challengetypeid){
		this.challengetypeid = challengetypeid;
	}
	
	public int getChallengetypenum(){
		return challengetypenum;
	}
	
	public void setChallengetypenum(int challengetypenum){
		this.challengetypenum = challengetypenum;
	}
	
	public int getChiptypeid(){
		return chiptypeid;
	}
	
	public void setChiptypeid(int chiptypeid){
		this.chiptypeid = chiptypeid;
	}
	
	public int getChiptypenum(){
		return chiptypenum;
	}
	
	public void setChiptypenum(int chiptypenum){
		this.chiptypenum = chiptypenum;
	}
	
	public int getFighttypeid(){
		return fighttypeid;
	}
	
	public void setFighttypeid(int fighttypeid){
		this.fighttypeid = fighttypeid;
	}
	
	public int getFighttypenum(){
		return fighttypenum;
	}
	
	public void setFighttypenum(int fighttypenum){
		this.fighttypenum = fighttypenum;
	}
	
	public int getBattletypeid(){
		return battletypeid;
	}
	
	public void setBattletypeid(int battletypeid){
		this.battletypeid = battletypeid;
	}
	
	public int getBattletypenum(){
		return battletypenum;
	}
	
	public void setBattletypenum(int battletypenum){
		this.battletypenum = battletypenum;
	}
	
	public int getFriendtypeid(){
		return friendtypeid;
	}
	
	public void setFriendtypeid(int friendtypeid){
		this.friendtypeid = friendtypeid;
	}
	
	public int getFriendtypenum(){
		return friendtypenum;
	}
	
	public void setFriendtypenum(int friendtypenum){
		this.friendtypenum = friendtypenum;
	}
	
	public int getFightrankid(){
		return fightrankid;
	}
	
	public void setFightrankid(int fightrankid){
		this.fightrankid = fightrankid;
	}
	
	public float getFightranknum(){
		return fightranknum;
	}
	
	public void setFightranknum(float fightranknum){
		this.fightranknum = fightranknum;
	}
	
	public int getFightranktime(){
		return fightranktime;
	}
	
	public void setFightranktime(int time){
		this.fightranktime = time;
	}
	
	public int getBattlerankid(){
		return battlerankid;
	}
	
	public void setBattlerankid(int battlerankid){
		this.battlerankid = battlerankid;
	}
	
	public float getBattleranknum(){
		return battleranknum;
	}
	
	public void setBattleranknum(float battleranknum){
		this.battleranknum = battleranknum;
	}
	
	public int getBattleranktime(){
		return battleranktime;
	}
	
	public void setBattleranktime(int battleranktime){
		this.battleranktime = battleranktime;
	}
	//获取总的加成百分比
	public float getTotalRate(){
		float totalRate=this.viptypenum+this.battleranknum+this.fightranknum;
		return totalRate;
	}
	//获取除百分比外总的气势数值
	public int getTotalAmount(){
		int totalAmount=this.battletypenum+this.careertypenum+this.challengetypenum+this.chiptypenum+this.fighttypenum+this.friendtypenum;
		return totalAmount;
	}
}
