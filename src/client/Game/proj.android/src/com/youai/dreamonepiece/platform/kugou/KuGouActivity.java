package com.youai.dreamonepiece.platform.kugou;

import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

public class KuGouActivity extends GameActivity {
	public KuGouActivity(){
		super.mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_KuGou);
	}
}
