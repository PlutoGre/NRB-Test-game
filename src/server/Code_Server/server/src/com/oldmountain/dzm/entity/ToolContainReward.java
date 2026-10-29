package com.oldmountain.dzm.entity;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-1
 * Time: 下午5:38
 * To change this template use File | Settings | File Templates.
 */
public class ToolContainReward {
    private int type;
    private int itemId;
    private int quantity;
    public int getType(){
        return  type;
    }
    public void setType(int type){
        this.type = type;
    }

    public int getItemId(){
        return  itemId;
    }
    public void setItemId(int itemId){
        this.itemId = itemId;
    }

    public int getQuantity(){
        return  quantity;
    }
    public void setQuantity(int quantity){
        this.quantity = quantity;
    }
}
