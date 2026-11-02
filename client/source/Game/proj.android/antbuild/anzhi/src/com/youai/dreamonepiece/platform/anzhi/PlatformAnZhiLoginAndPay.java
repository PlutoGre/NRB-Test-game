package com.youai.dreamonepiece.platform.anzhi;

import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.util.Log;
import android.widget.Toast;

import com.anzhi.payments.model.InitParams;
import com.anzhi.payments.model.PayParams;
import com.anzhi.usercenter.AnBaseActivity;
import com.anzhi.usercenter.cb.LoginListener;
import com.anzhi.usercenter.cb.PayListener;
import com.anzhi.usercenter.manager.AdManager;
import com.anzhi.usercenter.manager.GameManager;
import com.anzhi.usercenter.manager.LoginManager;
import com.anzhi.usercenter.manager.PayManager;
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

public class PlatformAnZhiLoginAndPay implements IPlatformLoginAndPay {
	
	private static String TAG=PlatformAnZhiLoginAndPay.class.getSimpleName();
	
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
	
	private static PlatformAnZhiLoginAndPay sInstance = null;
	public static PlatformAnZhiLoginAndPay getInstance(){
		if(sInstance == null){
			sInstance = new PlatformAnZhiLoginAndPay();
		}
		return sInstance;
	}
	
	@Override
	public void init(IGameActivity game_ctx, GameInfo game_info) {
		// TODO Auto-generated method stub
		mGameActivity = game_ctx;
		this.game_ctx = game_ctx.getActivity();
		this.game_info = game_info;
		
		//game_info.app_key = "1373855086b74VGQb3C0O1L3x3zpez";
		//game_info.app_secret = "Mqw5Jx3LkGELDUMD87U576eG";
		
		game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		this.game_info.screen_orientation = PlatformAndGameInfo.enScreenOrientation_Portrait;
		this.game_info.platform_type_str = PlatformAndGameInfo.enPlatformName_AnZhi;
		this.game_info.platform_type = PlatformAndGameInfo.enPlatform_AnZhi;
		
		InitParams params = new InitParams();
		params.appKey = game_info.app_key;
 		params.appSecret = game_info.app_secret;
		params.channel = PlatformAndGameInfo.enPlatformName_AnZhi;
		AdManager.init(params);
		// 设置竖屏，默认值：ActivityInfo.SCREEN_ORIENTATION_SENSOR
		AnBaseActivity.setScreenOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		
		isLogin = false;
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
		return PlatformAndGameInfo.DoNotSupportUpdate;
	}

	@Override
	public int getPlatformLogoLayoutId() {
		// TODO Auto-generated method stub
		return R.layout.logo_anzhi;
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
		
		this.isLogin = false;
		
		
		//
		//
		PlatformAnZhiLoginAndPay.sInstance = null;
	}

	@Override
	public GameInfo getGameInfo() {
		// TODO Auto-generated method stub
		return this.game_info;
	}

	@Override
	public void callLogin() {
		// TODO Auto-generated method stub
		
		
		/**
		 * 跳转到sdk登陆界面
		 */
		
			if(!isLogin){
				GameManager.returnAnSdkLogin(this.game_ctx);
			}
			
			final IGameAppStateCallback callback = mCallback3;
			callback.showWaitingViewImp(false, -1, "");

			// 登陆回调（登陆，注销，切换账号，老账号登陆，所有状态）
			LoginManager.getInstance(this.game_ctx).addLoginListener(loginCallBack);
		
		
	}
	
	// 登陆回调（登陆，注销，切换账号，老账号登陆，所有状态）
	LoginListener loginCallBack = new LoginListener() {
		@Override
		public void LoginResult(int code, String desc) {
			// TODO Auto-generated method stub 
			Log.e("loginCallBack", "code:" + code);
			final IGameAppStateCallback callback = mCallback3;
			callback.showWaitingViewImp(true, -1, "正在登录");
			LoginInfo login_info = new LoginInfo();
			switch (code) {
			case LoginListener.LOGIN_IN_SUCCESS:  //登陆成功
				callback.showWaitingViewImp(false, -1, "");
				Log.e("loginCallBack", "LoginCallBack.LOGIN_IN_SUCCESS");
				Log.e("回话ID：", GameManager.getSessionId());
				Log.e("登陆名：", GameManager.getLoginName());
				
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
				login_info.login_session = GameManager.getSessionId();
				login_info.account_uid_str = GameManager.getUid();
				login_info.account_nick_name =GameManager.getLoginName();
				
				Toast.makeText(game_ctx, "登录成功，点击进入游戏", Toast.LENGTH_SHORT).show();
				isLogin = true;
				PlatformAnZhiLoginAndPay.getInstance().notifyLoginResult(login_info);
				break;
			case LoginListener.LOGIN_IN_FAIL:  //登陆失败
				callback.showWaitingViewImp(false, -1, "");
				Log.e("loginCallBack", "LoginCallBack.LOGIN_IN_FAIL");
				Toast.makeText(game_ctx, "登陆失败！ ", Toast.LENGTH_SHORT).show();
				break;
			case LoginListener.LOGIN_OUT_FAIL: //登出，注销，重启成功
				callback.showWaitingViewImp(false, -1, "");
				Log.e("loginCallBack", "LoginCallBack.LOGIN_OUT_FAIL");
//				Toast.makeText(game_ctx, "登出，注销，重启成功！ ", Toast.LENGTH_SHORT).show();
				login_info.account_nick_name = "Login";
				isLogin = false;
				break;
			case LoginListener.LOGIN_OUT_SUCCESS: //登出，注销，重启失败
				callback.showWaitingViewImp(false, -1, "");
				Log.e("loginCallBack", "LoginCallBack.LOGIN_OUT_SUCCESS");
				login_info.account_nick_name="Login";
				isLogin = false;
				
				if(Cocos2dxHelper.nativeHasEnterMainFrame())
				{
					AlertDialog dlg = new AlertDialog.Builder(PlatformAnZhiLoginAndPay.getInstance().game_ctx)
					.setTitle("提示")
					.setMessage("您已退出账号登录，请重新进入游戏!")
					.setPositiveButton("确定", new DialogInterface.OnClickListener()
					{
						@Override
						public void onClick(DialogInterface dialog, int which)
						{
							System.exit(0);
							
						}
					}).setOnCancelListener(new DialogInterface.OnCancelListener()
					{

						@Override
						public void onCancel(DialogInterface dialog)
						{
							System.exit(0);
						}
				
					}).create();
					dlg.show();
				}
//				Toast.makeText(game_ctx, "登出，注销，重启失败!", Toast.LENGTH_SHORT)	.show();
				break;
			case LoginListener.REGISTER_AND_LOGIN_IN_SUCCESS: //APP 官方账号登录成功 
				Log.e("loginCallBack","LoginCallBack.REGISTER_AND_LOGIN_IN_SUCCESS");
				callback.showWaitingViewImp(false, -1, "");
				Toast.makeText(game_ctx, "APP 官方账号注册成功,点击进入游戏", Toast.LENGTH_SHORT).show();
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
				login_info.login_session = GameManager.getSessionId();
				login_info.account_uid_str = GameManager.getUid();
				login_info.account_nick_name =GameManager.getLoginName();
				
				isLogin = true;
				PlatformAnZhiLoginAndPay.getInstance().notifyLoginResult(login_info);
				break;
			case LoginListener.OFFICIAL_ACCOUNT_LOGIN_IN_SUCCESS: //注册并自动登录成功
				callback.showWaitingViewImp(false, -1, "");
				Log.e("loginCallBack","LoginCallBack.OFFICIAL_ACCOUNT_LOGIN_IN_SUCCESS");
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
				login_info.login_session = GameManager.getSessionId();
				login_info.account_uid_str = GameManager.getSessionId();
				login_info.account_uid_str = GameManager.getUid();
				
				login_info.account_nick_name =GameManager.getLoginName();
				
				Toast.makeText(game_ctx, "登录成功，点击进入游戏", Toast.LENGTH_SHORT).show();
				isLogin = true;
				PlatformAnZhiLoginAndPay.getInstance().notifyLoginResult(login_info);
				break;
			}
		}
	};

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		// TODO Auto-generated method stub
		login_info = null;
		login_info = login_result;
		//
		if (login_result != null) {
			login_info.account_uid_str = PlatformAndGameInfo.enPlatformShort_AnZhi+login_info.account_uid_str;
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
		int aError = 0;
		this.pay_info = null;
		this.pay_info = pay_info;
		
	    PayParams payParams = new PayParams();
		payParams.which = Integer.parseInt(pay_info.description);  //服务器分区
//		payParams.desc = "你需要支付" + pay_info.price + "元，可以兑换" + (pay_info.price * 10) +"个钻石。";
		payParams.desc =pay_info.product_name;
		payParams.money = pay_info.price;
		payParams.isCallBack = true;
		payParams.callBackInfo = pay_info.description + "-" + pay_info.product_id + "-" + this.login_info.account_uid_str;//区号-物品ID-ouruserid
//		payParams.orderId =  pay_info.order_serial;
		PayManager.pay(this.game_ctx, payParams);// 充值
		PayManager.addPayListener(new PayListener() {
			
			@Override
			public void PayResult(int payType, String orderId, float price, int successCode,
					String desc, String time) { //successCode   支付状态：（0 支付成功，1 支付失败，2 待支付）
				// TODO Auto-generated method stub
				switch (successCode) {//
				case PayListener.PAY_SUCCESS: //1
					//payParams.orderId = orderId;
					Toast.makeText(game_ctx, "支付成功", Toast.LENGTH_SHORT).show();
					PlatformAnZhiLoginAndPay.getInstance().pay_info.result = 0;
					PlatformAnZhiLoginAndPay.getInstance().notifyPayRechargeRequestResult(PlatformAnZhiLoginAndPay.getInstance().pay_info);
					break;
				case PayListener.PAY_FAIL: //2
					Toast.makeText(game_ctx, "支付失败", Toast.LENGTH_SHORT).show();
					break;
				case PayListener.PAY_WAIT: //3
					Toast.makeText(game_ctx, "待支付", Toast.LENGTH_SHORT).show();
					break;
				default:
					break;
				}
			}

			@Override
			public void PayEnd() {
				// TODO Auto-generated method stub
			}
		});
		return aError;
	}

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) {
		// TODO Auto-generated method stub
		mCallback3.notifyPayRechargeResult(pay_info);
	}

	@Override
	public void callAccountManage() {
		// TODO Auto-generated method stub
		GameManager.returnAnSdkCenter(game_ctx); //切换账号(用户管理中心)
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
