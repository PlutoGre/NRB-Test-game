package com.oldmountain.dzm.entity;

import java.util.ArrayList;
import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Administrator
 * Date: 13-4-2
 * Time: 上午11:56
 * To change this template use File | Settings | File Templates.
 */
public class LevelSilversXmlTemplate {
    private List<Integer> silvers;
    public LevelSilversXmlTemplate(){
        this.silvers = new ArrayList<Integer>();
    }
    public List<Integer> getSilvers(){
        return silvers;
    }

    public void setSilvers(List<Integer> silvers){
        this.silvers = silvers;
    }
}
