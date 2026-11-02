package com.youai.dreamonepiece.platform.defaultplatform;

import android.app.Activity;

import com.youai.DeviceUtil;
import com.youai.LogUtils;
import java.util.UUID;
import android.content.SharedPreferences;
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

/** @brief 本地部署使用的设备身份登录，不连接第三方账号或支付服务。 */
public class PlatformDefaultLoginAndPay implements IPlatformLoginAndPay {

	
private static final String TAG = "PlatformDefaultLoginAndPay";
	
	private IGameActivity 				mGameActivity;
	private IPlatformSDKStateCallback	mCallback1;
	private IGameUpdateStateCallback	mCallback2;
	private IGameAppStateCallback		mCallback3;
	
	private Activity 					game_ctx = null;
	private GameInfo 					game_info = null;
	private LoginInfo 					login_info = null;
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

	@Override
	public void unInit() {
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

    /** 使用稳定的本地设备身份登录，读取设备信息失败时使用持久化随机标识。 */
    @Override
    public void callLogin() {
        LogUtils.d(TAG, "callLogin: 开始本地登录");
        if (mCallback3 == null || game_ctx == null) {
            LogUtils.w(TAG, "callLogin: 登录回调或上下文尚未就绪");
            return;
        }
        LoginInfo result = new LoginInfo();
        result.login_result = PlatformAndGameInfo.enLoginResult_Success;
        result.login_session = "";
        result.account_uid_str = localIdentity();
        result.account_uid = 0L;
        result.account_nick_name = "Local";
        notifyLoginResult(result);
        mCallback3.showWaitingViewImp(false, -1, "已登录");
        LogUtils.i(TAG, "callLogin: 本地登录完成");
    }

    private String localIdentity() {
        // 不以固定账号代替设备身份，避免多台手机共用同一个角色。
        String id = null;
        try {
            id = DeviceUtil.getDeviceUUID(game_ctx);
        } catch (RuntimeException error) {
            LogUtils.e(TAG, "localIdentity: 设备标识读取失败，使用本地标识", error);
        }
        if (id == null || id.length() == 0) {
            SharedPreferences preferences = game_ctx.getSharedPreferences("local_login", 0);
            id = preferences.getString("identity", "");
            if (id.length() == 0) {
                id = UUID.randomUUID().toString();
                preferences.edit().putString("identity", id).commit();
            }
        }
        return id;
    }

    /** 保存登录结果并通知游戏层，重复通知不重复追加渠道前缀。 */
    @Override
    public void notifyLoginResult(LoginInfo result) {
        if (result == null || result.account_uid_str == null || mCallback3 == null) {
            LogUtils.w(TAG, "notifyLoginResult: 登录结果或回调为空");
            return;
        }
        String prefix = PlatformAndGameInfo.enPlatformShort_Default;
        if (!result.account_uid_str.startsWith(prefix)) {
            result.account_uid_str = prefix + result.account_uid_str;
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

	@Override
	public void callAccountManage() {
		// TODO Auto-generated method stub

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
