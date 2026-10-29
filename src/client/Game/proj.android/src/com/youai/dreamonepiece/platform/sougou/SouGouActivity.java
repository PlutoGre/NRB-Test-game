package com.youai.dreamonepiece.platform.sougou;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class SouGouActivity extends GameActivity {
	public SouGouActivity()
	{
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_SouGou);
	}
}
