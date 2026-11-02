package com.youai.dreamonepiece.platform.punchbox;

import joy.JoyGL;
import joy.JoyInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;

import com.punchbox.hound.Hound;
import com.youai.IPlatformLoginAndPay;
import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class ChuKongActivity extends GameActivity implements JoyGL {
	
	
	PlatformChuKongLoginAndPay mChuKongPlatform = null;
	
	
	public ChuKongActivity(){
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_ChuKong);
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
	
		ApplicationInfo appInfo = null;
		try {
			appInfo = getPackageManager().getApplicationInfo(getPackageName(),PackageManager.GET_META_DATA);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
		}
		
		String channel=String.valueOf(appInfo.metaData.getInt("chukong_channel", 0));
		String boxid=appInfo.metaData.getString("chukong_boxid","");
		if(boxid.equals("")||boxid==null)
		{
			boxid=String.valueOf(appInfo.metaData.getInt("chukong_boxid", 0));
		}
		
		Hound.init(this,boxid,channel);
	}
	
	@Override
	protected void onDestroy() {
		//
		JoyInterface.onDestroy();
		Hound.release(this);
		super.onDestroy();
		
	}
	
	@Override
	public void initPlatformSDK(IPlatformLoginAndPay platform) {

		super.initPlatformSDK(platform);
		
		mChuKongPlatform = (PlatformChuKongLoginAndPay)mPlatform;
		
		mChuKongPlatform.initChuKong(this);
		
	}
	
	@Override
	public void onPause() {
		super.onPause();
		JoyInterface.onPause(); 
	}
	
	@Override
	public void onResume() {
		super.onResume();
		JoyInterface.onResume();
	}
	
	static {
		// 见文档1.3.3
		System.loadLibrary("joygamesdk");
	}
	
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		JoyInterface.onActivityResult(requestCode, resultCode, data);
		super.onActivityResult(requestCode, resultCode, data);
	}
}
