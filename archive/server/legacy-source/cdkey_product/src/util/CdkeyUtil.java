package util;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Random;

import org.apache.commons.lang.StringUtils;

import entity.Cdkey;

public class CdkeyUtil {
	//获取cdkey
	public static String createCdkey(String type, int channel) { // length表示生成字符串的长度
		String base1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";//abcdefghijklmnopqrstuvwxyz
		String base2 = "0123456789";
		Random random = new Random();
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < 6; i++) {
			int number = random.nextInt(base1.length());
			sb.append(base1.charAt(number));
		}
		for (int i = 0; i < 4; i++) {
			int number = random.nextInt(base2.length());
			sb.append(base2.charAt(number));
		}
		String[] strs = sb.toString().split("");
		List<String> str_list = Arrays.asList(strs);
		Collections.shuffle(str_list);
		String cdkey = type;
		for (String s : str_list) {
			cdkey += s;
		}
		cdkey += channel;
		return cdkey;
	}
	//批量获取cdkey
	public static List<Cdkey> createCdkeysList(int count, String type, int channel) {
		List<String> list = new ArrayList<String>();
		List<Cdkey> cdkeys=new ArrayList<Cdkey>();
		for (int i = 0; i < count; i++) {
			String cd_key = CdkeyUtil.createCdkey(type, channel);
			if (list.contains(cd_key.toUpperCase())) {
				i--;
			} 
			else 
			{
				Cdkey cdkey=new Cdkey();
				cdkey.setCd_key(cd_key);
				cdkey.setCreate_time((int)(Math.floor(System.currentTimeMillis()/1000)));
				cdkeys.add(cdkey);
				list.add(cd_key.toUpperCase());
			}
		}
		return cdkeys;
	}
	//组织条件语句
	public static String createCondition(String type,String channel, String status){
		String condition="";
		if(StringUtils.isNotBlank(type)){
			condition+=" and c.cd_key like '"+type+"%' ";
		}
		if(StringUtils.isNotBlank(channel)){
			condition+=" and c.cd_key like '%"+channel+"' ";
		}
		if(StringUtils.isNotBlank(status)){
			condition+=" and c.status="+status+" ";
		}
		return condition;
	}
	
}
