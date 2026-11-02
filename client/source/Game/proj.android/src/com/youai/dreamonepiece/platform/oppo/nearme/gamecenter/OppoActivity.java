package com.youai.dreamonepiece.platform.oppo.nearme.gamecenter;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class OppoActivity extends GameActivity {
	public OppoActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_Oppo);
	}
}
