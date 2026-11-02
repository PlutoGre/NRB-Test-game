package com.youai.dreamonepiece.platform.xunlei;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class XunLeiActivity extends GameActivity {
	public XunLeiActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_XunLei);
	}
}
