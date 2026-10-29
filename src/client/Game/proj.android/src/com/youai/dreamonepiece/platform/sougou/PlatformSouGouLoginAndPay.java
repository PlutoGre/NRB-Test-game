package com.youai.dreamonepiece.platform.sougou;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.util.Log;
import android.widget.Toast;

import com.sogou.gamecenter.sdk.SogouGamePlatform;
import com.sogou.gamecenter.sdk.bean.SogouAppInfo;
import com.sogou.gamecenter.sdk.bean.UserInfo;
import com.sogou.gamecenter.sdk.listener.LoginCallbackListener;
import com.sogou.gamecenter.sdk.listener.PayCallbackListener;
import com.sogou.gamecenter.sdk.listener.VersionCallbackListener;
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

public class PlatformSouGouLoginAndPay implements IPlatformLoginAndPay {

	private static final String TAG = PlatformSouGouLoginAndPay.class.getSimpleName();
	
	private IGameActivity 				mGameActivity;
	private IPlatformSDKStateCallback	mCallback1;
	private IGameUpdateStateCallback	mCallback2;
	private IGameAppStateCallback		mCallback3;
	
	private Activity 					game_ctx = null;
	private GameInfo 					game_info = null;
	private LoginInfo 					login_info = null;
	private VersionInfo 				version_info = null;
	private PayInfo 					pay_info = null;
	private boolean                     isLogin = false;
	public UserInfo                     info = null;
	
	
	private static PlatformSouGouLoginAndPay sInstance = null;
	public PlatformSouGouLoginAndPay (){
		
	}
	
	public static PlatformSouGouLoginAndPay getInstance() {
		if (sInstance == null) {
			sInstance = new PlatformSouGouLoginAndPay();
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
		this.game_info.platform_type_str = PlatformAndGameInfo.enPlatformName_SouGou;
		this.game_info.platform_type =PlatformAndGameInfo.enPlatform_SouGou;
		
		SogouAppInfo sogouAppInfo = new SogouAppInfo();
		sogouAppInfo.appId = this.game_info.app_id;
		sogouAppInfo.gameName="梦想海贼王(mxhzw)";
		sogouAppInfo.appKey=this.game_info.app_key;
		SogouGamePlatform.getInstance().setDevelopMode(false);
		SogouGamePlatform.getInstance().init(this.game_ctx, sogouAppInfo);
		
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
		return PlatformAndGameInfo.SupportUpdateCheckAndDownload;
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
		//
		PlatformSouGouLoginAndPay.sInstance = null;
	}

	@Override
	public GameInfo getGameInfo() {
		// TODO Auto-generated method stub
		return this.game_info;
	}

	@Override
	public void callLogin() {
		// TODO Auto-generated method stub
		if(this.isLogin){
			Log.w(TAG, "Logined");
			return;
		}
		final IGameAppStateCallback callback = mCallback3;
		callback.showWaitingViewImp(true, -1, "正在登录");
		SogouGamePlatform.getInstance().login(this.game_ctx, new LoginCallbackListener() {
		
			@Override
			public void loginSuccess(int code, UserInfo userInfo) {
				// TODO Auto-generated method stub
				Log.e(TAG, "SCUUESS"+code+"=UserInfo:"+userInfo.toString());
				callback.showWaitingViewImp(false, -1, "");
				
				info = userInfo;
				
				/*post请求*/
				new Thread() {
					public void run() {
						String url = game_info.pay_addr;
						
						// 第一步，创建HttpPost对象 
				        HttpPost httpPost = new HttpPost(url); 
				        
						// 设置HTTP POST请求参数必须用NameValuePair对象 
				        List<NameValuePair> params = new ArrayList<NameValuePair>(); 
				        params.add(new BasicNameValuePair("gid", String.valueOf(game_info.app_id)));
				        params.add(new BasicNameValuePair("session_key", info.getSessionKey())); 
				        params.add(new BasicNameValuePair("user_id", String.valueOf(info.getUserId())));
				        params.add(new BasicNameValuePair("app_secret", game_info.app_secret)); 
						
						org.apache.http.HttpResponse httpResponse = null; 
						try {
							 // 设置httpPost请求参数 
				            httpPost.setEntity(new UrlEncodedFormEntity(params, HTTP.UTF_8)); 
				            httpResponse = new DefaultHttpClient().execute(httpPost); 
				            if (httpResponse.getStatusLine().getStatusCode() == 200) { 
				                // 第三步，使用getEntity方法活得返回结果 
				                String result = EntityUtils.toString(httpResponse.getEntity()); 
				                Log.i("result", result);
				                JSONObject jsonObject = new JSONObject(result);
				                Log.e(TAG, jsonObject.toString());
//				                String bool=jsonObject.optString("result");
				                String str = null;
				                
				                if(jsonObject.optString("result")!=null)
				                {
				                	str = jsonObject.optString("result");
				                	Log.e(TAG, str);
				                	if(str.equals("true"))
					                {
					                	LoginInfo login_info = new LoginInfo();
										login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
										login_info.login_session = info.getSessionKey();
										login_info.account_uid = info.getUserId();
										login_info.account_uid_str = String.valueOf(login_info.account_uid);
										//login_info.account_uid = Long.parseLong(userId);
										login_info.account_nick_name = login_info.account_uid_str;
										isLogin = true;
										PlatformSouGouLoginAndPay.getInstance().notifyLoginResult(login_info);
					                }
					                if(str.equals("false"))
					                {
					                	mGameActivity.showToastMsg("登录失败");
					                }
				                }
				                else if(jsonObject.optString("error")!=null)
				                {
				                	
				                	String error = jsonObject.optString("error");
				                	JSONObject jObject = new JSONObject(error);
				                	int code = jObject.optInt("code");
				                	
				                	if(code == 1)
				                	{
				                		Toast.makeText(game_ctx, "参数错误(缺少必要参数)", Toast.LENGTH_SHORT).show();
				                	}
				                	if(code == 5)
				                	{
				                		Toast.makeText(game_ctx, "签名错误", Toast.LENGTH_SHORT).show();
				                	}
				                	if(code == 2001)
				                	{
				                		Toast.makeText(game_ctx, "无效应用", Toast.LENGTH_SHORT).show();
				                	}
				                	{
				                		Toast.makeText(game_ctx, "未知错误", Toast.LENGTH_SHORT).show();
				                	}
				                }
				            }
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						} catch (JSONException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}

						
					};
				}.start();
			}
			
			@Override
			public void loginFail(int code, String msg) {
				// TODO Auto-generated method stub
				callback.showWaitingViewImp(false, -1, "");
				Log.e(TAG, "Failed"+code+"=msg:"+msg);
			}
		});
		callback.showWaitingViewImp(false, -1, "");
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		// TODO Auto-generated method stub
		login_info = null;
		login_info = login_result;
		
		if(login_result != null){
			login_info.account_uid_str = PlatformAndGameInfo.enPlatformShort_SouGou+login_info.account_uid_str;
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
		SogouGamePlatform.getInstance().checkVersion(this.game_ctx, new VersionCallbackListener() {
			
			@Override
			public void checkVersionProcess(int code, String data) {
				// TODO Auto-generated method stub
				switch (code) {
				case 1001:
					
					break;
				case 1002:
					VersionInfo version_info = new VersionInfo();
					version_info.update_info = PlatformAndGameInfo.enUpdateInfo_No;
					PlatformSouGouLoginAndPay.getInstance().notifyVersionUpateInfo(version_info);
					break;
				case 1003:
	
					break;
				case 1004:
					VersionInfo version_info4 = new VersionInfo();
					version_info4.update_info = PlatformAndGameInfo.enUpdateInfo_No;
					PlatformSouGouLoginAndPay.getInstance().notifyVersionUpateInfo(version_info4);
					break;
				case 1005:
				{
					VersionInfo version_info5 = new VersionInfo();
					version_info5.update_info = PlatformAndGameInfo.enUpdateInfo_No;
					PlatformSouGouLoginAndPay.getInstance().notifyVersionUpateInfo(version_info5);
					break;
				}
				default:
					break;
				}
			}
		});
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
		
		String appData = pay_info.description + "-" + pay_info.product_id + "-" + this.login_info.account_uid_str;//区号-物品ID-ouruserid
		
		Map<String, Object> data = new HashMap<String, Object>();
		data.put("currency", "钻石");
		data.put("rate", 10);
		data.put("amount", (int)this.pay_info.price);
		data.put("product_name", this.pay_info.product_name);
		data.put("app_data", appData);
		
		SogouGamePlatform.getInstance().setReadOnly(true);
		SogouGamePlatform.getInstance().setSid(Cocos2dxHelper.nativeGetServerId());
		SogouGamePlatform.getInstance().pay(this.game_ctx, data, new PayCallbackListener() {
			
			@Override
			public void paySuccess(int code, String appData) {
				// TODO Auto-generated method stub
				if(code == 0)
				{
					PlatformSouGouLoginAndPay.getInstance().pay_info.result = 0;
					PlatformSouGouLoginAndPay.getInstance().notifyPayRechargeRequestResult(PlatformSouGouLoginAndPay.getInstance().pay_info);
					return;
				}
				else if(code == -1)
				{
					Toast.makeText(game_ctx, "失败", Toast.LENGTH_SHORT).show();
				}
				else if(code == 1)
				{
					Toast.makeText(game_ctx, "参数错误", Toast.LENGTH_SHORT).show();
				}
				else if(code == 3001)
				{
					Toast.makeText(game_ctx, "提交订单失败", Toast.LENGTH_SHORT).show();
				}
				else if(code == 3002)
				{
					Toast.makeText(game_ctx, "无效的支付渠道", Toast.LENGTH_SHORT).show();
				}
				
			}
			
			@Override
			public void payFail(int code, String msg) {
				// TODO Auto-generated method stub
				//Toast.makeText(game_ctx, code+":"+msg, Toast.LENGTH_SHORT).show();
			}
		});
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
			return;
		if (PlatformSouGouLoginAndPay.getInstance().isLogin)
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
