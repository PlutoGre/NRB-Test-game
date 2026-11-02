package com.youai.dreamonepiece.platform.ld;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class LvDouGameActivity extends GameActivity {
	public LvDouGameActivity(){
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_LvDouGame);
	}
}
