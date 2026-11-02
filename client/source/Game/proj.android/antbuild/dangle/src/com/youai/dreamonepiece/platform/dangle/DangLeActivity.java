package com.youai.dreamonepiece.platform.dangle;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class DangLeActivity extends GameActivity {
	public DangLeActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_DangLe);
	}
}
