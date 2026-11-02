package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-2
 * Time: 下午2:29
 * To change this template use File | Settings | File Templates.
 */
public class EquipSelecter {
    private List<Integer> id;
    private List<Integer> quality;
    private List<Integer> equipType;
    private List<Integer> rate1;
    private List<Integer> rate2;
    private List<Integer> rate3;
    private List<Integer> rate4;
    private List<Integer> continuesLoginRate;
    private List<Integer> visitRate;

    public EquipSelecter(){
        this.id = new ArrayList<Integer>();
        this.quality = new ArrayList<Integer>();
        this.equipType = new ArrayList<Integer>();
        this.rate1 = new ArrayList<Integer>();
        this.rate2 = new ArrayList<Integer>();
        this.rate3 = new ArrayList<Integer>();
        this.rate4 = new ArrayList<Integer>();
        this.continuesLoginRate = new ArrayList<Integer>();
        this.visitRate =  new ArrayList<Integer>();
    }

    public void setId(List<Integer> id) {
        this.id = id;
    }

    public void setQuality(List<Integer> quality) {
        this.quality = quality;
    }

    public void setEquipType(List<Integer> equipType) {
        this.equipType = equipType;
    }

    public void setRate1(List<Integer> rate1) {
        this.rate1 = rate1;
    }

    public void setRate2(List<Integer> rate2) {
        this.rate2 = rate2;
    }

    public void setRate3(List<Integer> rate3) {
        this.rate3 = rate3;
    }

    public void setRate4(List<Integer> rate4) {
        this.rate4 = rate4;
    }

    public void setContinuesLoginRate(List<Integer> continuesLoginRate) {
        this.continuesLoginRate = continuesLoginRate;
    }

    public void setVisitRate(List<Integer> visitRate) {
        this.visitRate = visitRate;
    }

    public List<Integer> getId() {
        return id;
    }

    public List<Integer> getQuality() {
        return quality;
    }

    public List<Integer> getEquipType() {
        return equipType;
    }

    public List<Integer> getRate1() {
        return rate1;
    }

    public List<Integer> getRate2() {
        return rate2;
    }

    public List<Integer> getRate3() {
        return rate3;
    }

    public List<Integer> getRate4() {
        return rate4;
    }

    public List<Integer> getContinuesLoginRate() {
        return continuesLoginRate;
    }

    public List<Integer> getVisitRate() {
        return visitRate;
    }
}
