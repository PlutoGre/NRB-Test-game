package com.youai.dreamonepiece.platform.defaultplatform;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class DefaultActivity extends GameActivity {

	
	/*
	 * 
	 * */
	public DefaultActivity()
	{
		/*
		 * */
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_Default);

	}
	
}
