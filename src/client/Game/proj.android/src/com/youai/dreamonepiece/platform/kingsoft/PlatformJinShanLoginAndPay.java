package com.youai.dreamonepiece.platform.kingsoft;

import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.util.Log;


import com.ijinshan.ksmglogin.inteface.IKSLoginResultObserver;
import com.ijinshan.ksmglogin.inteface.IKSPayResultObserver;
import com.ijinshan.ksmglogin.util.ViewUtil;
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

public class PlatformJinShanLoginAndPay implements IPlatformLoginAndPay {
	
	private static final String TAG = PlatformJinShanLoginAndPay.class.getSimpleName();
	
	private IGameActivity                               mGameActivity;
	private IPlatformSDKStateCallback                   mCallback1;
	private IGameUpdateStateCallback                    mCallback2;
	private IGameAppStateCallback                       mCallback3;
	
	private Activity                                    game_ctx = null;
	private GameInfo                                    game_info = null;
	private LoginInfo                                   login_info = null;
	private VersionInfo                                 version_info = null;
	private PayInfo                                     pay_info = null;
	private boolean                                     isLogin = false;
	
	private static PlatformJinShanLoginAndPay sInstance = null;
	public static PlatformJinShanLoginAndPay getInstance()
	{
		if(sInstance == null){
			sInstance = new PlatformJinShanLoginAndPay();
		}
		return sInstance;
	}
	
	@Override
	public void init(IGameActivity game_ctx, GameInfo game_info) {
		// TODO Auto-generated method stub
		this.mGameActivity = game_ctx;
		this.game_ctx = game_ctx.getActivity();
		this.game_info = game_info;
		
//		this.game_info.app_id_str = "200005";
//		this.game_info.app_key = "LaL39DyVzLWpXn";
		
		game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		this.game_info.screen_orientation = PlatformAndGameInfo.enScreenOrientation_Portrait;
		this.game_info.platform_type_str = PlatformAndGameInfo.enPlatformName_JinShan;
		this.game_info.platform_type =PlatformAndGameInfo.enPlatform_JinShan;
		
		ViewUtil.initialize(this.game_ctx, this.game_info.app_id_str, this.game_info.app_key);
		
		isLogin = false;
		final IPlatformSDKStateCallback callback1 = mCallback1;
		callback1.notifyInitPlatformSDKComplete();
	}

	@Override
	public void setPlatformSDKStateCallback(IPlatformSDKStateCallback callback1) {
		// TODO Auto-generated method stub
		mCallback1 = callback1;

	}

	@Override
	public void setGameUpdateStateCallback(IGameUpdateStateCallback callback2) {
		// TODO Auto-generated method stub
		mCallback2 = callback2;
	}

	@Override
	public void setGameAppStateCallback(IGameAppStateCallback callback3) {
		// TODO Auto-generated method stub
		mCallback3 = callback3;

	}

	@Override
	public int isSupportInSDKGameUpdate() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int getPlatformLogoLayoutId() {
		// TODO Auto-generated method stub
		return -1;
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
		//
		PlatformJinShanLoginAndPay.sInstance = null;
	}

	@Override
	public GameInfo getGameInfo() {
		// TODO Auto-generated method stub
		return this.game_info;
	}

	@Override
	public void callLogin() {
		// TODO Auto-generated method stub
		if(this.isLogin)
		{
			Log.w(TAG, "Logined");
			return;
		}
		final IGameAppStateCallback callback = mCallback3;
		callback.showWaitingViewImp(true, -1, "正在登录");
		
		ViewUtil.login();
		callback.showWaitingViewImp(false, -1, "");
		ViewUtil.registerLoginResult(new IKSLoginResultObserver() {
			
			@Override
			public void loginResult(boolean loginSuccess, String uid, String session) {
				// TODO Auto-generated method stub
				if(loginSuccess){
					 callback.showWaitingViewImp(false, -1, "");
					 LoginInfo login_info = new LoginInfo();
					 login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
					 login_info.login_session = session;
					 login_info.account_uid_str = uid ;
					 login_info.account_nick_name = login_info.account_uid_str ;
					 
					isLogin = true;
					PlatformJinShanLoginAndPay.getInstance().notifyLoginResult(login_info);
				}else{
					callback.showWaitingViewImp(false, -1, "");
					mGameActivity.showToastMsg("登陆失败");
				}
				
			}
		});
		
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		// TODO Auto-generated method stub
		login_info = null;
		login_info = login_result;
		
		if(login_result != null){
			login_info.account_uid_str = PlatformAndGameInfo.enPlatformShort_JinShan+login_info.account_uid_str;
			mGameActivity.showToastMsg("登录成功，点击进入游戏");
			mCallback3.notifyLoginResut(login_result);
		}
	}

	@Override
	public LoginInfo getLoginInfo() {
		// TODO Auto-generated method stub
		if (login_info != null) {
			if (isLogin)
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
			else
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Failed;
		}
		
		return login_info;
	}

	@Override
	public void callLogout() {
		// TODO Auto-generated method stub
		isLogin = false;
	}

	@Override
	public void callCheckVersionUpate() {
		// TODO Auto-generated method stub

	}

	@Override
	public void notifyVersionUpateInfo(VersionInfo version_info) {
		// TODO Auto-generated method stub
		this.version_info = null;
		this.version_info = version_info;
		if (version_info != null) {
			mCallback2.notifyVersionCheckResult(version_info);
		}
	}

	@Override
	public int callPayRecharge(PayInfo pay_info) {
		// TODO Auto-generated method stub
		int Error = 0;
		this.pay_info = null;
		this.pay_info = pay_info;
		String cpparam = pay_info.description + "-" + pay_info.product_id + "-" + this.login_info.account_uid_str;//区号-物品ID-ouruserid
		
		//调用支付页面(0:商品名称，2:商品价格，3:游戏id(平台提供)，4:服id(1表示1服))this.pay_info.price
		ViewUtil.pay(cpparam,String.valueOf(this.pay_info.price*100), 
				this.game_info.app_secret,pay_info.description);
		
		ViewUtil.registerPayResult(new IKSPayResultObserver() {
			@Override
			public void payResult(boolean isPayFinished) {
				// TODO Auto-generated method stub
				if(isPayFinished)
				{
					
					PlatformJinShanLoginAndPay.getInstance().pay_info.result = 0;
					PlatformJinShanLoginAndPay.getInstance().notifyPayRechargeRequestResult(PlatformJinShanLoginAndPay.getInstance().pay_info);
				}
				else
				{
					mGameActivity.showToastMsg("支付失败！");
				}
			}
		});
		return Error;
	}

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) {
		// TODO Auto-generated method stub
		mCallback3.notifyPayRechargeResult(pay_info);
	}

	@Override
	public void callAccountManage() {
		// TODO Auto-generated method stub
		
		if (Cocos2dxHelper.nativeHasEnterMainFrame())
			return;
		if (PlatformJinShanLoginAndPay.getInstance().isLogin)
			callLogout();
		ViewUtil.login();
	}

	@Override
	public String generateNewOrderSerial() {
		// TODO Auto-generated method stub
		return UUID.randomUUID().toString();
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
