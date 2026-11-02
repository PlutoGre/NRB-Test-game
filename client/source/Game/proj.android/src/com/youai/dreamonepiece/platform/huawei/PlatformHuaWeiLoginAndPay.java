package com.youai.dreamonepiece.platform.huawei;

import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import com.hianalytics.android.v1.HiAnalytics;
import com.huawei.hwid.openapi.OpenHwID;
import com.huawei.hwid.openapi.out.OutReturn;
import com.huawei.hwid.openapi.out.ResReqHandler;
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

public class PlatformHuaWeiLoginAndPay implements IPlatformLoginAndPay {

	public static final String TAG = PlatformHuaWeiLoginAndPay.class.getSimpleName();
	
	private IGameActivity                            	mGameActivity;
	private IPlatformSDKStateCallback                   mCallback1;
	private IGameUpdateStateCallback                    mCallback2;
	private IGameAppStateCallback                       mCallback3;
	
	private Activity                                    game_ctx = null;
	private GameInfo                                    game_info = null;
	private LoginInfo                                   login_info = null;
	private VersionInfo                              	version_info = null;
	private PayInfo                                     pay_info = null;
	private boolean                                     isLogin = false;
	
	private static JSONObject jsonObject;
	// 正式环境
	String environment = "ENV_LIVE";
	
	private static PlatformHuaWeiLoginAndPay            sInstance = null;
	public static PlatformHuaWeiLoginAndPay getInstance(){
		if(sInstance == null){
			sInstance = new PlatformHuaWeiLoginAndPay();
		}
		return sInstance;
	}
	
	@Override
	public void init(IGameActivity game_ctx, GameInfo game_info) {
		// TODO Auto-generated method stub
		this.mGameActivity = game_ctx;
		this.game_ctx = game_ctx.getActivity();
		this.game_info = game_info;
		 
		game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		this.game_info.screen_orientation = PlatformAndGameInfo.enScreenOrientation_Portrait;
		this.game_info.platform_type_str = PlatformAndGameInfo.enPlatformName_HuaWei;
		this.game_info.platform_type = PlatformAndGameInfo.enPlatform_HuaWei;
		
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
		return PlatformAndGameInfo.DoNotSupportUpdate;
	}

	@Override
	public int getPlatformLogoLayoutId() {
		// TODO Auto-generated method stub
		return -1;
	}

	@Override
	public void unInit() {
		// TODO Auto-generated method stub
		OpenHwID.logOut(this.game_ctx, this.game_info.app_id_str, null, null);

	}

	@Override
	public GameInfo getGameInfo() {
		// TODO Auto-generated method stub
		return this.game_info;
	}

	@Override
	public void callLogin() {
		// TODO Auto-generated method stub
		if(isLogin){
			return;
		}
		
  		final IGameAppStateCallback callback = mCallback3;
		callback.showWaitingViewImp(true, -1, "正在登录");
		
		OpenHwID.authorize(this.game_ctx, 
				"https://www.huawei.com/auth/account",
				new ResReqHandler() {
					
					@Override
					public void onComplete(Bundle values) {
						// TODO Auto-generated method stub
						if(OutReturn.isRequestSuccess(values)){
							String accessToken = OutReturn.getAccessToken(values); 
							OpenHwID.storeAccessToken(PlatformHuaWeiLoginAndPay.sInstance.game_ctx.getBaseContext(), game_info.app_id_str, null, accessToken, null);
							Log.i(TAG, "onComplete: token(" + accessToken + ")");
							
							OpenHwID.userInfoRequest(PlatformHuaWeiLoginAndPay.sInstance.game_ctx.getBaseContext(), 
							new ResReqHandler() {
								@Override
								public void onComplete(Bundle bd) {
									String content = OutReturn.getRetContent(bd);
									try {
										jsonObject = new JSONObject(content);
									} catch (JSONException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									
									if(OutReturn.isRequestSuccess(bd)){
										//登陆成功 可以获取到 userInfo信息
										callback.showWaitingViewImp(false, -1, "");
										
										LoginInfo login_info = new LoginInfo();
										login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
										login_info.login_session = jsonObject.optString("userID");
										login_info.account_uid_str = jsonObject.optString("userName");
										login_info.account_nick_name = jsonObject.optString("userName");
										
										isLogin = true;
										PlatformHuaWeiLoginAndPay.getInstance().notifyLoginResult(login_info);
										
									} else {
										callback.showWaitingViewImp(false, -1, "");
										Log.i(TAG, "bd IS NULL");
										Toast.makeText(game_ctx,"登陆失败", Toast.LENGTH_SHORT).show();
									}
								}
						}, 
						accessToken);
						} else {
							callback.showWaitingViewImp(false, -1, "");
							Log.i(TAG, "onComplete get Token failed!!" +
									" err:" + OutReturn.getErrInfo(values));
							Toast.makeText(game_ctx,"取消登陆", Toast.LENGTH_SHORT).show();
						}
					}
				}, this.game_info.app_id_str, null);
			callback.showWaitingViewImp(false, -1, "");
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		// TODO Auto-generated method stub
		login_info = null;
		login_info = login_result;
		
		if(login_result != null){
			login_info.account_uid_str = PlatformAndGameInfo.enPlatformShort_HuaWei+login_info.account_uid_str;
			Toast.makeText(game_ctx,"登录成功，点击进入游戏", Toast.LENGTH_SHORT).show();
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
		this.pay_info = null;
		this.pay_info = pay_info;

		String requestId = pay_info.description + "-" + pay_info.product_id + "-" + this.login_info.account_uid_str;//区号-物品ID-ouruserid
		String price = this.pay_info.price+"";
		price = price+"0";
		
		Map<String, String> params = new HashMap<String, String>();
		params.put("userID", this.game_info.pay_id_str);
		params.put("applicationID", this.game_info.app_id_str);
		params.put("amount", price);
		params.put("productName", this.pay_info.product_name);
		params.put("productDesc", this.pay_info.product_name);
		params.put("requestId", requestId);
		
		String noSign = HuaweiPayUtil.getSignData(params);
		String	sign = Rsa.sign(noSign, this.game_info.private_str);
		
		Log.e(TAG, "pre noSign: "+noSign + "  sign: "+sign);
		
		MobileSecurePayHelper payHelper = new MobileSecurePayHelper(this.game_ctx);
		Map<String, Object> payInfo = new HashMap<String, Object>();
		payInfo.put("amount", price);
		payInfo.put("productName", this.pay_info.product_name);
		payInfo.put("requestId", requestId);
		payInfo.put("productDesc", this.pay_info.product_name);
		payInfo.put("userName", "华为");
		payInfo.put("applicationID", this.game_info.app_id_str);
		payInfo.put("userID", this.game_info.pay_id_str);
		payInfo.put("sign", sign);
		payInfo.put("notifyUrl", this.game_info.pay_addr);//正式服
		payInfo.put("environment", environment);
		
		Log.e(TAG , "all parameters : "+payInfo.toString());
	
		payHelper.startPay(payInfo, handler, HuaweiPayUtil.RQF_PAY_INFO);
		
		
		return 0;
	}
	
	private Handler handler = new Handler(){
		public void handleMessage(Message msg) {
			try {
				switch (msg.what) {
				case HuaweiPayUtil.RQF_PAY_INFO: {

					//处理支付结果
					String pay = "支付失败！";
					String payResult = (String)msg.obj;
					Log.e(TAG, "GET PAY RESULT "+ payResult);
					JSONObject jsonObject = new JSONObject(payResult);
					String returnCode = jsonObject.getString("returnCode");
					if(returnCode.equals("0"))
					{
						String errMsg = jsonObject.getString("errMsg");
						if(errMsg.equals("success"))
						{
						
							//支付成功，验证信息的安全性
							String amount = jsonObject.getString("amount");
							String sign = jsonObject.getString("sign");
							String orderID = jsonObject.getString("orderID");
							String requestId = jsonObject.getString("requestId");
							String userName = jsonObject.getString("userName");
							String time = jsonObject.getString("time");
							Map<String, String> paramsa = new HashMap<String, String>();
							paramsa.put("userName", userName);
							paramsa.put("orderID",orderID);
							paramsa.put("amount", amount);
							paramsa.put("errMsg", errMsg);
							paramsa.put("time", time);
							paramsa.put("requestId", requestId);
							String noSigna = HuaweiPayUtil.getSignData(paramsa);
							boolean s = Rsa.doCheck(noSigna, sign, game_info.public_str);
							
							if(s)
							{
								pay = "支付成功！";
							}else
							{
								pay = "支付成功，但验签失败！";
							}
							
							Log.e(TAG, "Rsa.doChec = " + s );
						}
					}else if(returnCode.equals("30002"))
					{
						pay = "支付结果查询超时！";
					}
					Toast.makeText(game_ctx, pay, Toast.LENGTH_SHORT).show();
				}
				}
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		}
	};

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) {
		// TODO Auto-generated method stub

	}

	@Override
	public void callAccountManage() {
		// TODO Auto-generated method stub
		if (Cocos2dxHelper.nativeHasEnterMainFrame())
			return;
		if (PlatformHuaWeiLoginAndPay.getInstance().isLogin)
			callLogout();
		callLogin();
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
