package com.oldmountain.dzm.configEntity;

import java.util.ArrayList;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-5-11
 * Time: 上午10:24
 * To change this template use File | Settings | File Templates.
 */
public class SkillBookSelecter
{
    private int id;
    private ArrayList<Integer> weightArr;
    private ArrayList<Integer> bookIdArr;

    public SkillBookSelecter()
    {
        this.weightArr = new ArrayList<Integer>();
        this.bookIdArr = new ArrayList<Integer>();
    }

    public int getId()
    {
        return id;
    }

    public void setId(int id)
    {
        this.id = id;
    }

    public ArrayList<Integer> getWeightArr()
    {
        return weightArr;
    }

    public void setWeightArr(ArrayList<Integer> weightArr)
    {
        this.weightArr = weightArr;
    }

    public ArrayList<Integer> getBookIdArr()
    {
        return bookIdArr;
    }

    public void setBookIdArr(ArrayList<Integer> bookIdArr) {
        this.bookIdArr = bookIdArr;
    }
}
