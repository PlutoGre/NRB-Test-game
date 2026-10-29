package com.youai.dreamonepiece.platform.huawei;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class HuaWeiActivity extends GameActivity {
	public HuaWeiActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_HuaWei);
	}
}
