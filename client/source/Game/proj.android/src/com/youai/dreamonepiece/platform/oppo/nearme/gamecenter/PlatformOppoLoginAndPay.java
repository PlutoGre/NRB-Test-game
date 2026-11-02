package com.youai.dreamonepiece.platform.oppo.nearme.gamecenter;

import java.util.Date;
import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONObject;

import android.app.Activity;
import android.widget.Toast;

import com.nearme.gamecenter.open.api.ApiCallback;
import com.nearme.gamecenter.open.api.ApiParams;
import com.nearme.gamecenter.open.api.GameCenterSDK;
import com.nearme.gamecenter.open.api.GameCenterSettings;
import com.nearme.gamecenter.open.api.ProductInfo;
import com.nearme.gamecenter.open.api.UserInfo;
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
import com.youai.dreamonepiece.platform.nd91.R;

public class PlatformOppoLoginAndPay implements IPlatformLoginAndPay {
	private static final String TAG = PlatformOppoLoginAndPay.class.getSimpleName();

	private IGameActivity 				mGameActivity;
	private IPlatformSDKStateCallback	mCallback1;
	private IGameUpdateStateCallback	mCallback2;
	private IGameAppStateCallback		mCallback3;
	
	private Activity 					game_ctx = null;
	private GameInfo 					game_info = null;
	private LoginInfo 					login_info = null;
	private VersionInfo 				version_info = null;
	private PayInfo 					pay_info = null;
	private boolean						isLogined = false;
	
	
	private static PlatformOppoLoginAndPay sInstance = null;
	public static PlatformOppoLoginAndPay getInstance() {
		if (sInstance == null) {
			sInstance = new PlatformOppoLoginAndPay();
		}
		return sInstance;
	}
	
	@Override
	public void init(IGameActivity game_ctx, GameInfo game_info) {
		// TODO Auto-generated method stub
		mGameActivity = game_ctx;
		this.game_ctx = game_ctx.getActivity();
		this.game_info = game_info;

		game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		this.game_info.screen_orientation  = PlatformAndGameInfo.enScreenOrientation_Portrait;
		this.game_info.platform_type_str = PlatformAndGameInfo.enPlatformName_Oppo;
		this.game_info.platform_type = PlatformAndGameInfo.enPlatform_Oppo;
		GameCenterSettings gameCenterSettings = new GameCenterSettings(
				game_info.app_key, game_info.app_secret) {

			@Override
			public void onForceReLogin() {
				// sdk由于某些原因登出,此方法通知cp,cp需要在此处清理当前的登录状态并重新请求登录.
				// 可以发广播通知页面重新登录
			}
			
			@Override 
			public void onForceUpgradeCancel() {
				// 游戏自升级，后台有设置为强制升级，用户点击取消时的回调函数。
				// 若开启强制升级模式 ，  一般要求不更新则强制退出游戏并杀掉进程。
				// System.exit(0) or kill this process
				System.exit(0);
			}

		};
		GameCenterSettings.isDebugModel = true;// 测试log开关
		GameCenterSettings.isOritationPort = true;// 控制SDK activity的横竖屏 true为竖屏
		// 游戏自身 虚拟货币和人民币的 汇率  当申请的支付类型为人民币直冲时将起作用
		//GameCenterSettings.rate = "100"; // 1元人民币兑换1000虚拟货币
		GameCenterSDK.init(gameCenterSettings, PlatformOppoLoginAndPay.getInstance().game_ctx);
		 mCallback1.notifyInitPlatformSDKComplete();
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
		return R.layout.nmgc_logo_ad;
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
		this.isLogined = false;
		
		PlatformOppoLoginAndPay.sInstance = null;
	}

	@Override
	public GameInfo getGameInfo() {
		// TODO Auto-generated method stub
		return this.game_info;
	}

	@Override
	public void callLogin() {
		// TODO Auto-generated method stub
		if(isLogined)
		{
			return;
		}
		final IGameAppStateCallback callback = mCallback3;
		callback.showWaitingViewImp(true, -1, "正在登录");
		GameCenterSDK.setmCurrentContext(PlatformOppoLoginAndPay.getInstance().game_ctx);
		GameCenterSDK.getInstance().doReLogin(new ApiCallback() {
		@Override
		public void onSuccess(String content, int code){
			if (code == ApiParams.GLOBAL_CODE_OK || code == ApiParams.LOGIN_CODE_ALREADY_LOGIN) {
				GameCenterSDK.getInstance().doGetUserInfo(new ApiCallback() {
					@Override
					public void onSuccess(String content, int arg1) {
						// GameCenterSDK.getInstance().removeLoginPre();
						UserInfo ui = new UserInfo(content);
						
						Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(),
								"登录成功 ", Toast.LENGTH_SHORT)
								.show();

							LoginInfo login_info = new LoginInfo();
							login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
						//	login_info.login_session = ;
							login_info.account_uid_str = ui.id;
							login_info.account_nick_name = ui.nickname;
						//
						isLogined = true;
						PlatformOppoLoginAndPay.getInstance().notifyLoginResult(login_info);
					}

					@Override
					public void onFailure(String arg0, int code) {
						Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(),
								"获取用户信息失败:" + arg0, Toast.LENGTH_SHORT)
								.show();

					}
				});
			}
			callback.showWaitingViewImp(false, -1, "");
		}
		@Override
		public void onFailure(String content, int code){
			Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(), "登录失败",
					Toast.LENGTH_SHORT).show();
			callback.showWaitingViewImp(false, -1, "");
		}});
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		// TODO Auto-generated method stub
		login_info = null;
		login_info = login_result;
		//
		if (login_result != null) {
			
			login_info.account_uid_str = PlatformAndGameInfo.enPlatformShort_Oppo+login_info.account_uid_str;
			
			mCallback3.notifyLoginResut(login_result);
		}
	}

	@Override
	public LoginInfo getLoginInfo() {
		// TODO Auto-generated method stub
		if (login_info != null) {
			if(isLogined)
			{
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
			}
			else
			{
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Failed;
			}
		}
		
		return login_info;
	}

	@Override
	public void callLogout() {
		// TODO Auto-generated method stub
		//if (Cocos2dxHelper.nativeHasEnterMainFrame())
		//	return;
		
		isLogined = false;
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
		String temp = pay_info.description + "-" + pay_info.product_id.substring(0, pay_info.product_id.indexOf("."))+"-"+this.login_info.account_uid_str+"-"+(int)((new Date()).getTime()/1000);//区号-物品ID-orderserial
		final ProductInfo p2 = new ProductInfo(
				(double)pay_info.price*100,
				PlatformOppoLoginAndPay.getInstance().game_info.pay_addr,
				"",
				temp,pay_info.product_name, "钻石", 1, 1);
		GameCenterSDK.getInstance().doPaymentForNBAO(new ApiCallback() {
			@Override
			public void onSuccess(String content, int code) {
				Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(), "亲,道具购买成功了哦,请安全使用.",
						Toast.LENGTH_SHORT).show();
				if (PlatformOppoLoginAndPay.getInstance().pay_info != null)
				{
					PlatformOppoLoginAndPay.getInstance().pay_info.result = 0;
					
					notifyPayRechargeRequestResult(PlatformOppoLoginAndPay.getInstance().pay_info);
				}
			}

			@Override
			public void onFailure(String content, int code) {
				Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(),
						"购买失败:" + content,
						Toast.LENGTH_SHORT).show();
			}
		}, p2);
		return 0;
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
		{
			GameCenterSDK.getInstance().doShowForum(PlatformOppoLoginAndPay.getInstance().game_ctx);
			return;
		}
		if (PlatformOppoLoginAndPay.getInstance().isLogined)
			callLogout();
		GameCenterSDK.setmCurrentContext(PlatformOppoLoginAndPay.getInstance().game_ctx);
		GameCenterSDK.getInstance().doReLogin(new ApiCallback() {
					@Override
					public void onSuccess(String content, int code) {
						// 登录成功的判断
						GameCenterSDK.getInstance().doGetUserInfo(new ApiCallback() {
							@Override
							public void onSuccess(String content, int arg1) {
								// GameCenterSDK.getInstance().removeLoginPre();
								UserInfo ui = new UserInfo(content);
								
								Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(),
										"登录成功 ", Toast.LENGTH_SHORT)
										.show();

									LoginInfo login_info = new LoginInfo();
									login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
								//	login_info.login_session = ;
									login_info.account_uid_str = ui.id;
									login_info.account_nick_name = ui.nickname;
								//
								isLogined = true;
								PlatformOppoLoginAndPay.getInstance().notifyLoginResult(login_info);
							}

							@Override
							public void onFailure(String arg0, int code) {
								Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(),
										"获取用户信息失败:" + arg0, Toast.LENGTH_SHORT)
										.show();

							}
						});

					}

					@Override
					public void onFailure(String content, int code) {
						Toast.makeText(PlatformOppoLoginAndPay.getInstance().game_ctx.getApplicationContext(), "登录失败",
								Toast.LENGTH_SHORT).show();
					}
				}
				);
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
		GameCenterSDK.getInstance().doShowForum(PlatformOppoLoginAndPay.getInstance().game_ctx);
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
