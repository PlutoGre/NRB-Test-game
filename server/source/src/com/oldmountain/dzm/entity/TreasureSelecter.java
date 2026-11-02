package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-3-30
 * Time: 下午5:40
 * To change this template use File | Settings | File Templates.
 */
public class TreasureSelecter {
    private List<Integer> id;
    private List<Integer> type;
    private List<Integer> itemType;
    private List<Integer> propId;
    private List<Integer> count;
    private List<Integer> worth;
    private List<Float> rate1;
    private List<Float> rate2;
    private List<Float> rate3;
    private List<Float> rate4;
    public TreasureSelecter(){
        this.id =new ArrayList<Integer>();
        this.type = new ArrayList<Integer>();
        this.itemType = new ArrayList<Integer>();
        this.propId = new ArrayList<Integer>();
        this.count = new ArrayList<Integer>();
        this.worth= new ArrayList<Integer>();
        this.rate1 = new ArrayList<Float>();
        this.rate2 = new ArrayList<Float>();
        this.rate3 = new ArrayList<Float>();
        this.rate4=new ArrayList<Float>();
    }
    public List<Integer> getId(){
        return id;
    }

    public void setId(List<Integer> id){
        this.id = id;
    }
    public List<Integer> getType(){
        return type;
    }

    public void setType(List<Integer> type){
        this.type = type;
    }
    public List<Integer> getItemType(){
        return itemType;
    }

    public void setItemType(List<Integer> itemType){
        this.itemType = itemType;
    }

    public List<Integer> getPropId(){
        return propId;
    }

    public void setPropId(List<Integer> propId){
        this.propId = propId;
    }

    public List<Integer> getCount(){
        return count;
    }

    public void setCount(List<Integer> count){
        this.count = count;
    }

    public List<Integer> getWorth(){
        return worth;
    }

    public void setWorth(List<Integer> worth){
        this.worth = worth;
    }

    public List<Float> getRate1(){
        return rate1;
    }

    public void setRate1(List<Float> rate1){
        this.rate1 = rate1;
    }

    public List<Float> getRate2(){
        return rate2;
    }

    public void setRate2(List<Float> rate2){
        this.rate2 = rate2;
    }

    public List<Float> getRate3(){
        return rate3;
    }

    public void setRate3(List<Float> rate3){
        this.rate3 = rate3;
    }

    public List<Float> getRate4(){
        return rate4;
    }

    public void setRate4(List<Float> rate4){
        this.rate4 = rate4;
    }
}
