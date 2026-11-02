package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-5
 * Time: 下午6:57
 * To change this template use File | Settings | File Templates.
 */
public class VisitRewardSelecter {

    private Integer times;
    private Integer continueDay;
    private List<Integer> rewardTypes;
    private List<Float> rates;
    public VisitRewardSelecter()
    {
        rewardTypes = new ArrayList<Integer>();
        rates = new ArrayList<Float>();
    }

    public Integer getTimes() {
        return times;
    }

    public void setTimes(Integer times) {
        this.times = times;
    }

    public Integer getContinueDay() {
        return continueDay;
    }

    public void setContinueDay(Integer continueDay) {
        this.continueDay = continueDay;
    }

    public List<Integer> getRewardTypes() {
        return rewardTypes;
    }

    public void setRewardTypes(List<Integer> rewardTypes) {
        this.rewardTypes = rewardTypes;
    }

    public List<Float> getRates() {
        return rates;
    }

    public void setRates(List<Float> rates) {
        this.rates = rates;
    }
}
