package com.oldmountain.dzm.entity;

public class Skill {
    private int id;
    private int playerid;
    private int itemid;
    private int type;
    private int level;
    private float quantity;
    private int consume;
    private int battleid;
    private int isdefaultskill;
    private int expiretime;

    public Skill() {
        level = 1;
        consume = 0;
        expiretime = 0;
        battleid = 0;
        isdefaultskill = 0;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getPlayerid() {
        return playerid;
    }

    public void setPlayerid(int playerid) {
        this.playerid = playerid;
    }

    public int getItemid() {
        return itemid;
    }

    public void setItemid(int itemid) {
        this.itemid = itemid;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getLevel() {
        return level;
    }

    public void setLevel(int level) {
        this.level = level;
    }

    public float getQuantity() {
        return quantity;
    }

    public void setQuantity(float quantity) {
        this.quantity = quantity;
    }
    
    public int getConsume(){
    	return consume;
    }
    
    public void setConsume(int consume){
    	this.consume = consume;
    }
    
    public int getBattleid(){
    	return battleid;
    }
    
    public void setBattleid(int id){
    	this.battleid = id;
    }
    
    public int getIsdefaultskill(){
    	return isdefaultskill;
    }
    
    public void setIsdefaultskill(int isdefaultskill){
    	this.isdefaultskill = isdefaultskill;
    }
    
    public int getExpiretime(){
        return expiretime;
    }

    public void setExpiretime(int time){
        this.expiretime = time;
    }
}
