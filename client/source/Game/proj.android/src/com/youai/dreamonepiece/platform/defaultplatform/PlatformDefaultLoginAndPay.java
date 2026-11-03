package com.youai.dreamonepiece.platform.defaultplatform;

import android.app.Activity;

import com.youai.LogUtils;
import com.youai.IGameActivity;
import com.youai.IPlatformLoginAndPay;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.GameInfo;
import com.youai.PlatformAndGameInfo.LoginInfo;
import com.youai.PlatformAndGameInfo.PayInfo;
import com.youai.PlatformAndGameInfo.ShareInfo;
import com.youai.PlatformAndGameInfo.VersionInfo;
import com.youai.dreamonepiece.GameInterface.IGameAppStateCallback;
import com.youai.dreamonepiece.GameInterface.IGameUpdateStateCallback;
import com.youai.dreamonepiece.GameInterface.IPlatformSDKStateCallback;

/** @brief 自建服务端账号渠道，不依赖第三方账号或设备游客身份。 */
public class PlatformDefaultLoginAndPay implements IPlatformLoginAndPay {

	
private static final String TAG = "PlatformDefaultLoginAndPay";
	
	private IGameActivity 				mGameActivity;
	private IPlatformSDKStateCallback	mCallback1;
	private IGameUpdateStateCallback	mCallback2;
	private IGameAppStateCallback		mCallback3;
	
	private Activity 					game_ctx = null;
	private GameInfo 					game_info = null;
	private LoginInfo 					login_info = null;
    private AccountLogin accounts;
	private VersionInfo 				version_info = null;
	private PayInfo 					pay_info = null;
	
	private int 						auto_recalllogin_count = 0;
	
	
	private static PlatformDefaultLoginAndPay sInstance = null;
	/** 获取本地登录实例。 */
	public static PlatformDefaultLoginAndPay getInstance() {
		if (sInstance == null) {
			sInstance = new PlatformDefaultLoginAndPay();
		}
		return sInstance;
	}
	
	private PlatformDefaultLoginAndPay() {
		
	}
	
	/** 初始化本地登录上下文并通知平台准备完成。 */
	@Override
	public void init(IGameActivity game_acitivity, GameInfo game_info) {
        LogUtils.i(TAG, "init: 初始化本地登录");
		

		mGameActivity = game_acitivity;
		this.game_ctx = game_acitivity.getActivity();
		this.game_info = game_info;
		game_info.use_platform_sdk_type = 1;//0逻辑需要再调
		game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		 	
		//
		mCallback1.notifyInitPlatformSDKComplete();
		//
		
	}

	@Override
	public void setPlatformSDKStateCallback(IPlatformSDKStateCallback callback1) {
		// TODO Auto-generated method stub

		this.mCallback1 = callback1;
		
	}

	@Override
	public void setGameUpdateStateCallback(IGameUpdateStateCallback callback2) {
		// TODO Auto-generated method stub
		this.mCallback2 = callback2;
	}

	@Override
	public void setGameAppStateCallback(IGameAppStateCallback callback3) {
		// TODO Auto-generated method stub
		this.mCallback3 = callback3;
	}

	@Override
	public int isSupportInSDKGameUpdate() {
		// TODO Auto-generated method stub
		return PlatformAndGameInfo.DoNotSupportUpdate;
	}

	@Override
	public int getPlatformLogoLayoutId() {
		// TODO Auto-generated method stub
		return 0;
	}

    /** 释放账号窗口并使在途回调失效。 */
	@Override
	public void unInit() {
        LogUtils.d(TAG, "unInit: 释放账号窗口");
        if (accounts != null) accounts.close();
        accounts = null;
		// TODO Auto-generated method stub
		mGameActivity = null;
		mCallback1 = null;
		mCallback2 = null;
		mCallback3 = null;
		//
		this.game_ctx = null;
		this.game_info = null;
		this.login_info = null;
		this.version_info = null;
		this.pay_info = null;

	}

	@Override
	public GameInfo getGameInfo() {
		// TODO Auto-generated method stub
		return this.game_info;
	}

    /** 使用缓存凭证恢复登录，首次登录时显示服务端账号窗口。 */
    @Override
    public void callLogin() {
        LogUtils.d(TAG, "callLogin: 开始服务端账号认证");
        if (mCallback3 == null || game_ctx == null) {
            LogUtils.w(TAG, "callLogin: 登录回调或上下文尚未就绪");
            return;
        }
        mCallback3.showWaitingViewImp(false, -1, "账号登录");
        if (accounts == null) {
            accounts = new AccountLogin(game_ctx, new AccountLogin.Listener() {
                public void accepted(String uid, String username, String token) {
                    if (mCallback3 == null) return;
                    LoginInfo result = new LoginInfo();
                    result.login_result = PlatformAndGameInfo.enLoginResult_Success;
                    result.login_session = token;
                    result.account_uid_str = uid;
                    result.account_uid = 0L;
                    result.account_nick_name = username;
                    notifyLoginResult(result);
                }
            });
        }
        accounts.login();
    }

    /** 保存服务器身份并通知游戏层，三端使用同一个 UID，不追加渠道前缀。 */
    @Override
    public void notifyLoginResult(LoginInfo result) {
        if (result == null || result.account_uid_str == null || mCallback3 == null) {
            LogUtils.w(TAG, "notifyLoginResult: 登录结果或回调为空");
            return;
        }
        login_info = result;
        mCallback3.notifyLoginResut(result);
    }

	@Override
	public LoginInfo getLoginInfo() {
		// TODO Auto-generated method stub
		return login_info;
	}

	/** 清除当前本地登录状态。 */
	@Override
	public void callLogout() {
        login_info = null;
        if (accounts != null) accounts.logout();
        LogUtils.i(TAG, "callLogout: 已清除本地登录状态");

	}

	@Override
	public void callCheckVersionUpate() {
		// TODO Auto-generated method stub

	}

	@Override
	public void notifyVersionUpateInfo(VersionInfo version_info) {
		// TODO Auto-generated method stub

	}

	@Override
	public int callPayRecharge(PayInfo pay_info) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) {
		// TODO Auto-generated method stub

	}

    /** 登录页切换账号，清除当前平台状态后重新输入账号密码。 */
	@Override
	public void callAccountManage() {
        LogUtils.d(TAG, "callAccountManage: 切换服务端账号");
        login_info = null;
        if (accounts == null) callLogin(); else accounts.switchAccount();
	}

	@Override
	public String generateNewOrderSerial() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void callPlatformFeedback() {
		// TODO Auto-generated method stub

	}

	@Override
	public void callPlatformSupportThirdShare(ShareInfo share_info) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void callPlatformGameBBS() {
		// TODO Auto-generated method stub

	}

	@Override
	public void onGamePause() {
		// TODO Auto-generated method stub

	}

	@Override
	public void onGameResume() {
		// TODO Auto-generated method stub

	}

	@Override
	public void onGameExit() {
		// TODO Auto-generated method stub

	}

}
