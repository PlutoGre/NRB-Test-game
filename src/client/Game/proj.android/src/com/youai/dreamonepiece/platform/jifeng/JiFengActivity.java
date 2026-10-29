package com.youai.dreamonepiece.platform.jifeng;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class JiFengActivity extends GameActivity {
	public JiFengActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_JiFeng);
	}
}
