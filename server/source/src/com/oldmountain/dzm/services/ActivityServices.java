package com.oldmountain.dzm.services;

import gnu.trove.map.TIntObjectMap;

import java.awt.datatransfer.StringSelection;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;

import com.oldmountain.dzm.entity.ActivityInfo;
import com.oldmountain.dzm.entity.ActivityTimeConfig;
import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ActivityServices {
	
	public static final int GODDESS_REBATE=1; //女神回馈活动id
	
	//判断是否进行信息清理
	public static boolean checkNeedToClearup(ActivityInfo activityInfo){
		//如果距上次清理时间超过10天,则进行清理,删除过期失效数据
		if(Util.getServerTime()-activityInfo.getLaseRefreshTime()>=86400*10){
			clearupInfo(activityInfo);
			activityInfo.setLaseRefreshTime(Util.getServerTime());
			return true;
		}
		return false;
	}
	
	//清除失效信息
	public static void clearupInfo(ActivityInfo activityInfo){
		Map<String, String> activityInfos=Util.jsonToMap(activityInfo.getActivityinfo());//全部活动信息
		Iterator<Entry<String, String>> iterator=activityInfos.entrySet().iterator();
		while(iterator.hasNext()){
			Entry<String, String> entry=iterator.next();
			int id=Integer.parseInt(entry.getKey());
			Map<String, Integer> info=Util.jsonToMap(entry.getValue());//活动信息
			if(info==null || !info.containsKey("termId")){
				continue;
			}
			int termId=info.get("termId");
			//通过该活动的id代号,获取它的时间配置信息
			TIntObjectMap<ActivityTimeConfig> configs=getTimeConfigById(id);
			int current_termId=getActivityTermId(Util.getServerMillTime(), configs);
			//如果活动没开启,或者记录的信息的id与当前活动id不符,则清除相关信息
			if(current_termId==0){
				activityInfos.remove(entry.getKey());
			}
			else if(current_termId!=termId){
				activityInfos.remove(entry.getKey());
			}
		}
		activityInfo.setActivityinfo(Util.mapToString(activityInfos));
	}
	
	//获取所有活动信息
	public static Map<String, String> getActivityInfos(ActivityInfo activityInfo){
		Map<String, String> infos=Util.jsonToMap(activityInfo.getActivityinfo());
		return infos;
	}
	
	//根据活动id获取活动信息
	public static Map<String, Integer> getActivityInfoById(ActivityInfo activityInfo,int id){
		Map<String, String> activityInfos=getActivityInfos(activityInfo);
		if(activityInfos==null){
			return null;
		}
		if(StringUtils.isBlank(activityInfos.get(String.valueOf(id)))){
			return null;
		}
		else{
			Map<String, Integer> info=Util.jsonToMap(activityInfos.get(String.valueOf(id)));
			return info;
		}
	}
	
	//根据活动id和活动itemId初始化活动信息
	public static Map<String, Integer> initActivityInfo(int id, int itemId) {
		Map<String, Integer> info=new HashMap<String, Integer>();
		switch (id) {
		case GODDESS_REBATE:
			info.put("termId",itemId);//初始化为当前itemid
			int size=XMLTemplateService.getGoddessConfigsSize();
			for(int i=1;i<=size;i++){
				info.put("intimacy"+i, 0);//初始化亲密度亲密度
			}
			info.put("totalCount",0);//初始化鲜花数量
			break;

		default:
			break;
		}
		
		return info;
	}

	//通过id获取活动的itemId
	public static int getTermIdById(int id,long current_time){
		TIntObjectMap<ActivityTimeConfig>configs=getTimeConfigById(id);
		int termId=getActivityTermId(current_time, configs);
		return termId;
		
	}

	//adventure中相关活动的开启id查询
    public static int getActivityTermId(long current_time,TIntObjectMap<ActivityTimeConfig> configs){
    	SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
    	int termId=0;
    	long beginTime=0;
    	long endTime=0;
    	for(int i=1;i<=configs.size();i++){
    		ActivityTimeConfig config=configs.get(i);
    		try {
				beginTime=df.parse(config.getBeginTime()).getTime();
				endTime=df.parse(config.getEndTime()).getTime();
				if(current_time>=beginTime && current_time<=endTime){
					termId=config.getId();
					break;
				}
			} catch (ParseException e) {
				e.printStackTrace();
			}
    	}
    	return termId;
    }
    //通过id和itemId获取adventure中相关活动的起止时间
    public static int[] getActivityTimeByTermid(int id,int termid){
    	TIntObjectMap<ActivityTimeConfig> configs=getTimeConfigById(id);
    	SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH:mm:ss");
    	String beginTime=configs.get(termid).getBeginTime();
    	String endTime=configs.get(termid).getEndTime();
    	int[]times={0,0};
    	try {
			times[0]=(int)(Math.floor(df.parse(beginTime).getTime()/1000));
			times[1]=(int)(Math.floor(df.parse(endTime).getTime()/1000));
		} catch (ParseException e) {
			e.printStackTrace();
		}
    	return times;
    }
   
    
    //根据id获取相应活动的时间配置
    public static TIntObjectMap<ActivityTimeConfig> getTimeConfigById(int id){
    	TIntObjectMap<ActivityTimeConfig> configs=null;
    	switch (id) {
		case GODDESS_REBATE:
			configs=XMLTemplateService.getGoddesRebateTimeConfigs();
			break;

		default:
			break;
		}
    	return configs;
    }
    //修改玩家活动信息
	public static ActivityInfo updateActivityInfos( ActivityInfo activityInfo,int id, Map<String, Integer> info) {
		Map<String, String> activityInfos=getActivityInfos(activityInfo);
		if(activityInfos==null){
			activityInfos=new HashMap<String, String>(); 
		}
		activityInfos.put(String.valueOf(id), Util.mapToString(info));
		activityInfo.setActivityinfo(Util.mapToString(activityInfos));
		return activityInfo;
	}
}
