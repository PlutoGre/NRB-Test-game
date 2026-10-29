package com.oldmountain.dzm.services;

import net.sf.json.JSONObject;

import com.oldmountain.dzm.util.Util;
import com.oldmountain.dzm.util.XMLTemplateService;

public class ActivateVersionService {
	public static int adventure_version=1;
	public static int market_givesoul_version=0;
	public static int tool_market_version=0;
	public static JSONObject getActivateVersion(){
		long current_time=Util.getServerMillTime();
		int givesoul_id=Util.getGiveSoulTermId(current_time);
		//商场活动id
		int tool_id=XMLTemplateService.getMarketToolVersion();
		int adventure_id=0;
		/****************奇遇活动版本检测*****************/
		int deepWater_id=Util.getActivityTermId(current_time, XMLTemplateService.getDeepWaterTimeConfigs());
		adventure_id=(adventure_id>deepWater_id)?adventure_id:deepWater_id;
		
		/****************奇遇活动版本检测*****************/
		if(tool_id>tool_market_version){
			tool_market_version=tool_id;
		}
		if(adventure_id>adventure_version){
			adventure_version=adventure_id;
		}
		if(givesoul_id>market_givesoul_version){
			market_givesoul_version=givesoul_id;
		}
		
		 JSONObject _obj=new JSONObject();
	     _obj.put("m",market_givesoul_version);
	     _obj.put("t",tool_market_version);
	     _obj.put("ad",adventure_version);
	     return _obj;
		
	}
}
