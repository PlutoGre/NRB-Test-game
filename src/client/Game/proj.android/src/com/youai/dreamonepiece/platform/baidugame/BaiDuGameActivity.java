package com.youai.dreamonepiece.platform.baidugame;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class BaiDuGameActivity extends GameActivity {
	public BaiDuGameActivity(){
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_BaiDuGame);
	}
}
