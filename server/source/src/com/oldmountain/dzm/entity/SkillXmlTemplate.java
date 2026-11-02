package com.oldmountain.dzm.entity;

public class SkillXmlTemplate {
	private int id;
	private int quality;
    private int type;
    private float rate;
    private float quantity;
    private float growth;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getQuality(){
    	return quality;
    }
    
    public void setQuality(int quality){
    	this.quality = quality;
    }
    
    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }
    
    public float getRate(){
    	return rate;
    }
    
    public void setRate(float rate){
    	this.rate = rate;
    }

    public float getQuantity() {
        return quantity;
    }

    public void setQuantity(float quantity) {
        this.quantity = quantity;
    }
    
    public float getGrowth(){
    	return growth;
    }
    
    public void setGrowth(float growth){
    	this.growth = growth;
    }
}
