package com.youai.dreamonepiece.platform.anzhi;

import android.os.Bundle;

import com.youai.LogUtils;
import com.youai.PlatformAndGameInfo;
import com.youai.dreamonepiece.GameActivity;
import com.youai.dreamonepiece.GameConfig;

/**
 * @brief 安智渠道入口，在 Context 挂接完成后读取游戏配置。
 */
public class AnZhiActivity extends GameActivity {
    private static final String TAG = "AnZhiActivity";

    /**
     * 初始化渠道配置，再进入父类的游戏启动流程。
     *
     * @param savedInstanceState 系统保存的 Activity 状态
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        LogUtils.d(TAG, "onCreate: 初始化渠道配置");
        // 系统调用 onCreate 前已经挂接 Context；父类启动状态机前必须准备好配置。
        mGameCfg = new GameConfig(this, PlatformAndGameInfo.enPlatform_AnZhi);
        LogUtils.i(TAG, "onCreate: 渠道配置已就绪");
        super.onCreate(savedInstanceState);
    }
}
