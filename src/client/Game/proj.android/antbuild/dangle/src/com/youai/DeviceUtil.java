

package com.youai;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Properties;
import java.util.UUID;

import android.content.Context;
import android.os.Build;
import android.telephony.TelephonyManager;


public class DeviceUtil
{
	/*
	 * 
	 * */
	public static String generateUUID()
	{
		return UUID.randomUUID().toString();//字符串带横杠字符-
	}
	/*
	 * 
	 * */
	public static String getDeviceProductName(Context context)
	{
		
		String temp = Build.MODEL.replaceAll(" ", "-");
		return Build.MANUFACTURER+"_"+temp;
		
		//String productName = "DeviceProductName";
	}
	/*
	 * 
	 * */
	public static String getDeviceUUID(Context context)
	{
		
		String uuid = "";//getExternalFilesDir(null)//不能用这个，在没有外部存储的设备会有问题！Android的外部存储不狭隘的指SD卡，设备厂商可内置部分存储时即为默认外部存储
		File uFile = new File(context.getFilesDir().getAbsolutePath()+"/uuid.properties");
		if (uFile.exists()) {
			Properties cfgIni = new Properties();
			try {
				cfgIni.load(new FileInputStream(uFile));
				uuid = cfgIni.getProperty("uuid", null);
			} catch (FileNotFoundException e) {
				
			} catch (IOException e) {
				
			}
			cfgIni = null;
			if(uuid!=null && !("".equals(uuid)))
			{
				uFile = null;
				return uuid;
			}
		}
		else
		{
			Properties cfgIni = new Properties();
			cfgIni.setProperty("uuid", "");
			try {
				cfgIni.store(new FileOutputStream(uFile), "auto save, default none str");
			} catch (FileNotFoundException e) {
				
			} catch (IOException e) {
				
			}
			//
			cfgIni = null;
		}
		try
		{
			TelephonyManager tmsvc = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
			if (tmsvc != null)
			{
				uuid = tmsvc.getDeviceId();
				if (uuid == null)
					uuid = tmsvc.getSubscriberId();
				if (uuid == null)
					uuid = DeviceUtil.getDeviceProductName(context);
			}
		}
		catch(Exception e)
		{
			
		}
		if(uuid==null||"".equals(uuid)||"0".equals(uuid))
		{
			uuid = "uuid_"+generateUUID();
		}
		Properties cfgIni = new Properties();
		cfgIni.setProperty("uuid", uuid);
		try {
			cfgIni.store(new FileOutputStream(uFile), "auto save, generateUUID");
		} catch (FileNotFoundException e) {
			
		} catch (IOException e) {
			
		}
		uFile = null;
		cfgIni = null;
		return uuid;
	}
}

