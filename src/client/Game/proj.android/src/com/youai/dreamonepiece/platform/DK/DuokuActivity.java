package com.youai.dreamonepiece.platform.DK;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class DuokuActivity extends GameActivity {
	public DuokuActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_BaiduDuoKu);
	}
}
