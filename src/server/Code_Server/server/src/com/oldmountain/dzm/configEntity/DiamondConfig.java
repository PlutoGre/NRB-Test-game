package com.oldmountain.dzm.configEntity;

import net.sf.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;

/*
 * Created with IntelliJ IDEA.
 * User: dylan
 * Date: 13-7-9
 * Time: 下午7:30
 * To change this template use File | Settings | File Templates.
 */
public class DiamondConfig
{
    public static final int[] PundConsume=new int[4];
    public static int DiamondOpenLevel=20;
    public static int DiamondOpenToolsId=20;
    public static int DiamondRemoveToolsId=20;
    public static int DiamondOpenStatus=0;
    private static JSONObject diamondAdditionConfig=new JSONObject();
    private static HashMap<String,ArrayList<Integer>> weightMap=new HashMap<String,ArrayList<Integer>>();
    private static HashMap<String,ArrayList<String>> attAdditionMap=new HashMap<String,ArrayList<String>>();
    public static int getOpenHoleConsumeByIndex(int index)
    {
        if(index<0||index>4)
        {
            return 999999;
        }
        if(index<=PundConsume.length)
        {
            return PundConsume[index-1];
        }
        return 999999;
    }

    public static void setDiamondAdditionConfig(JSONObject _diamondAdditionConfig)
    {
        diamondAdditionConfig = _diamondAdditionConfig;
        weightMap.clear();
        attAdditionMap.clear();
    }

    public static  ArrayList<Integer> getWeightByType(String key)
    {
        if (!weightMap.containsKey(key))
        {
            if(diamondAdditionConfig.containsKey(String.valueOf(key)))
            {
                ArrayList<Integer> _weightArr=new ArrayList<Integer>();
                _weightArr.addAll(diamondAdditionConfig.getJSONObject(String.valueOf(key)).getJSONArray("w").subList(0,diamondAdditionConfig.getJSONObject(String.valueOf(key)).getJSONArray("w").size()));
                weightMap.put(key,_weightArr);
            }
            else
            {
                new ArrayList<Integer>();
            }
        }
        return weightMap.get(key);
    }

    public static  ArrayList<String> getAttAdditionByType(String key)
    {
        if(!attAdditionMap.containsKey(key))
        {
            if(diamondAdditionConfig.containsKey(String.valueOf(key)))
            {
                ArrayList<String> _attrAddArr=new ArrayList<String>();
                _attrAddArr.addAll(diamondAdditionConfig.getJSONObject(String.valueOf(key)).getJSONArray("v").subList(0, diamondAdditionConfig.getJSONObject(String.valueOf(key)).getJSONArray("v").size()));
                attAdditionMap.put(key,_attrAddArr);
            }
            else
            {
                new ArrayList<String>();
            }
        }
        return attAdditionMap.get(key);
    }

    public static boolean DiamondOpen()
    {
        return DiamondOpenStatus==1;
    }
}
