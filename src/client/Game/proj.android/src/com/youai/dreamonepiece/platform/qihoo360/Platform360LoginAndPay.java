
/*
 * utf-8
 * */

package com.youai.dreamonepiece.platform.qihoo360;

import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.graphics.BitmapFactory;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.qihoopay.insdk.matrix.Matrix;
import com.qihoo.gamecenter.sdk.protocols.pay.ProtocolKeys;
import com.qihoopay.insdk.activity.ContainerActivity;
import com.youai.IGameActivity;
import com.youai.IPlatformLoginAndPay;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.GameInfo;
import com.youai.PlatformAndGameInfo.LoginInfo;
import com.youai.PlatformAndGameInfo.PayInfo;
import com.youai.PlatformAndGameInfo.ShareInfo;
import com.youai.PlatformAndGameInfo.VersionInfo;
import com.youai.dreamonepiece.GameConfig;
import com.youai.dreamonepiece.GameInterface.IGameAppStateCallback;
import com.youai.dreamonepiece.GameInterface.IGameUpdateStateCallback;
import com.youai.dreamonepiece.GameInterface.IPlatformSDKStateCallback;
import com.youai.dreamonepiece.platform.qihoo360.appserver.QihooUserInfo;
import com.youai.dreamonepiece.platform.qihoo360.appserver.QihooUserInfoListener;
import com.youai.dreamonepiece.platform.qihoo360.appserver.QihooUserInfoTask;
import com.youai.dreamonepiece.platform.qihoo360.appserver.TokenInfo;
import com.youai.dreamonepiece.platform.qihoo360.appserver.TokenInfoListener;
import com.youai.dreamonepiece.platform.qihoo360.appserver.TokenInfoTask;
import com.youai.dreamonepiece.platform.qihoo360.common.QihooPayInfo;
import com.youai.dreamonepiece.platform.qihoo360.common.SdkUserBaseActivity;
import com.youai.dreamonepiece.platform.qihoo360.common.TryAccount;
import com.youai.dreamonepiece.platform.qihoo360.utils.ProgressUtil;
import com.youai.dreamonepiece.platform.nd91.R;



public class Platform360LoginAndPay extends SdkUserBaseActivity 
					implements IPlatformLoginAndPay, TokenInfoListener, QihooUserInfoListener {
	
	private static final String TAG = Platform360LoginAndPay.class.getSimpleName();
	
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
	
	//登录返回解析得到的信息数�?
	private TokenInfo mTokenInfo;
    private QihooUserInfo mQihooUserInfo;
    private ProgressDialog mProgress;
    private TokenInfoTask mTokenTask;
    private QihooUserInfoTask mUserInfoTask;
	
	
	private static Platform360LoginAndPay sInstance = null;
	public static Platform360LoginAndPay getInstance() {
		if (sInstance == null) {
			sInstance = new Platform360LoginAndPay();
		}
		return sInstance;
	}
	
	private Platform360LoginAndPay() {
		
	}
	
	/*
	 * 返回值：
	 * -1：标示本SDK的init内部会切换view
	 * 0：标示本SDK的init内部不会切换view，也不需要单独指定显示平台logo的view，沿用GameLogo即可
	 * 其他值：是一个R.layout.value，标示本SDK的init内部不会切换view，但�?��单独指定显示平台logo的view layout
	 * */
	public int getPlatformLogoLayoutId()
	{
		//return R.layout.logo_360;
		return 0;
	}

	@Override
	public void init(IGameActivity game_acitivity, GameInfo game_info) {
		
		mGameActivity = game_acitivity;
		this.game_ctx = game_acitivity.getActivity();
		this.game_info = game_info;
		this.game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		this.game_info.screen_orientation  = PlatformAndGameInfo.enScreenOrientation_Portrait;
		//
		super.setActivityContext(game_ctx);
		mCallback1.notifyInitPlatformSDKComplete();
		//
	}

	@Override
	public void unInit() {
		
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
		super.doSdkQuit(false);
		//
		Platform360LoginAndPay.sInstance = null;
	}

	@Override
	public GameInfo getGameInfo() {
		
		return this.game_info;
	}

	@Override
	public void callLogin() {
		
		Log.d(TAG, "callLogin");
		
		mCallback3.showWaitingViewImp(true, -1, "正在登录");
		
		super.doSdkLogin(false, true);
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		
		login_info = null;
		login_info = login_result;
		//
		if (login_result != null) {
			
			login_info.account_uid_str = PlatformAndGameInfo.enPlatformShort_360+login_info.account_uid_str;
			
			Toast.makeText(game_ctx, "登录成功，点击进入游戏", Toast.LENGTH_SHORT)
			.show();
			
			mCallback3.notifyLoginResut(login_result);
		}
	}

	@Override
	public LoginInfo getLoginInfo() {
		
		if (login_info != null) {
			
		}
		
		return login_info;
	}

	@Override
	public void callLogout() {
		
		
	}

	@Override
	public void callCheckVersionUpate() {
		
		
	}

	@Override
	public void notifyVersionUpateInfo(VersionInfo version_info) {
		
		this.version_info = null;
		this.version_info = version_info;
		if (version_info != null) {
			
			mCallback2.notifyVersionCheckResult(version_info);
		}
		
	}
	
	/**
     * 钩子方法，留给使用支付的子类实现getQihooPayInfo
     * 
     * @param isFixed
     * @return
     */
    protected QihooPayInfo getQihooPayInfo(boolean isFixed) {
        
    	QihooPayInfo qPayInfo = new QihooPayInfo();
    	
    	qPayInfo.setAccessToken(this.login_info.login_session);
    	qPayInfo.setAppExt1(this.pay_info.description);//区号
    	qPayInfo.setAppExt2(this.pay_info.product_name);
    	qPayInfo.setAppName(this.game_ctx.getResources().getString(R.string.app_name));
    	qPayInfo.setAppOrderId(this.pay_info.order_serial);
    	qPayInfo.setAppUserId(this.login_info.account_uid_str);
    	qPayInfo.setAppUserName(this.login_info.account_nick_name);
    	qPayInfo.setExchangeRate("10");
    	qPayInfo.setMoneyAmount(String.valueOf((int)this.pay_info.price*100));//单位 *100
    	qPayInfo.setNotifyUri(Constants.DEMO_APP_SERVER_NOTIFY_URI);
    	//qPayInfo.setPayTypes(null);//必需添加快捷支付
    	qPayInfo.setProductId(this.pay_info.product_id);
    	qPayInfo.setProductName(this.pay_info.product_name);
    	qPayInfo.setQihooUserId(String.valueOf(this.login_info.account_uid));
    	
    	
    	return qPayInfo;
    }

	@Override
	public int callPayRecharge(PayInfo pay_info) {
		
		//
		this.pay_info = null;
		this.pay_info = pay_info;
		//
		//super.doSdkPay(false, true);
		super.mPayTypeList.add(ProtocolKeys.PayType.ALIPAY);
		super.mPayTypeList.add(ProtocolKeys.PayType.JCARD);
		super.mPayTypeList.add(ProtocolKeys.PayType.MOBILE_CARD);
		super.mPayTypeList.add(ProtocolKeys.PayType.QIHOO_CARD);
		super.mPayTypeList.add(ProtocolKeys.PayType.QUICKLY_PAY);
		super.mPayTypeList.add(ProtocolKeys.PayType.SAVINGS_CARD_PAY);
		super.mPayTypeList.add(ProtocolKeys.PayType.UNION_PAY);
		//
		super.toSdkPay(false, true);
		//
		return 0;
	}
	
	@Override
	protected void onGotPayCallback(int err, String msg)
    {
    	this.pay_info.result = err;
    	//
    	this.notifyPayRechargeRequestResult(this.pay_info);
    }

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) {
		
		mCallback3.notifyPayRechargeResult(pay_info);
		//
	}
	
	@Override
	public void callAccountManage() {
		
		if (Cocos2dxHelper.nativeHasEnterMainFrame())
			return;
		
		super.doSdkSwitchAccount(false, true);
	}

	@Override
	public String generateNewOrderSerial() {
		
		return UUID.randomUUID().toString();
	}

	@Override
	public void callPlatformFeedback() {
		
		super.doSdkCustomerService(false);
	}

	@Override
	public void callPlatformSupportThirdShare(ShareInfo share_info) {
		
		if (share_info.bitmap == null) {
			share_info.bitmap = BitmapFactory.decodeFile(share_info.img_path);
		}
		
		
	}
	
	@Override
	public void callPlatformGameBBS() {
		
		super.doSdkBBS(false);
	}

	@Override
	public void setPlatformSDKStateCallback(IPlatformSDKStateCallback callback1)
	{
		
		mCallback1 = callback1;
	}

	@Override
	public void setGameUpdateStateCallback(IGameUpdateStateCallback callback2)
	{
		
		mCallback2 = callback2;
	}

	@Override
	public void setGameAppStateCallback(IGameAppStateCallback callback3)
	{
		
		mCallback3 = callback3;
	}

	@Override
	public int isSupportInSDKGameUpdate()
	{
		/*
		 *
		 * */
		return PlatformAndGameInfo.DoNotSupportUpdate;
	}

	private void clearLoginResult() {
        mTokenInfo = null;
        mQihooUserInfo = null;
    }
	
	private String getAppSecret()
	{
		return GameConfig.nativeReadGameAppSecret(PlatformAndGameInfo.enPlatform_360);
	}
	
	@Override
	public void onGotAuthorizationCode(String code)
	{
		mCallback3.showWaitingViewImp(false, -1, "点击进入游戏发起重新登录");
		// 这里是登录的回调
		if (TextUtils.isEmpty(code)) {
            Toast.makeText(game_ctx, R.string.get_code_fail, Toast.LENGTH_LONG).show();
        } else {
            clearLoginResult();
            mTokenTask = TokenInfoTask.newInstance();
            // 提示用户进度
            mProgress = ProgressUtil.show(game_ctx, R.string.get_token_title, R.string.get_token_message, new OnCancelListener() {
                
                @Override
                public void onCancel(DialogInterface dialog) {
                	mTokenTask.doCancel();
                }
            });

            // 请求应用服务器，用AuthorizationCode换取AccessToken
            mTokenTask.doRequest(game_ctx, code, Matrix.getAppKey(game_ctx), getAppSecret(), this);
        }
	}

	@Override
	public void onGotError(int errCode)
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onGotUserInfo(QihooUserInfo userInfo)
	{
		// 这里是QihooUserInfoTask.doRequest的回�?
		ProgressUtil.dismiss(mProgress);

        if (userInfo != null && userInfo.isValid()) {
            // 保存QihooUserInfo
            mQihooUserInfo = userInfo;

            LoginInfo login_info = new LoginInfo();
            login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
            login_info.account_uid = Integer.parseInt(mQihooUserInfo.getId());//qihoo user id
            login_info.account_uid_str = mQihooUserInfo.getId();//will change at notifyLoginResult
            login_info.account_nick_name = mQihooUserInfo.getName();
            login_info.account_user_name = mQihooUserInfo.getNick();
            login_info.login_session = mTokenInfo.getAccessToken();
            
            notifyLoginResult(login_info);

        } else {
            Toast.makeText(game_ctx, R.string.get_user_fail, Toast.LENGTH_LONG).show();
        }
	}

	@Override
	public void onGotTokenInfo(TokenInfo tokenInfo)
	{
		// 这里是TokenInfoTask.doRequest的回�?
		if (tokenInfo == null || TextUtils.isEmpty(tokenInfo.getAccessToken())) {
            ProgressUtil.dismiss(mProgress);
            Toast.makeText(game_ctx, R.string.get_token_fail, Toast.LENGTH_LONG).show();
        } else {
            // 保存TokenInfo
            mTokenInfo = tokenInfo;
            mUserInfoTask = QihooUserInfoTask.newInstance();
            // 提示用户进度
            ProgressUtil.setText(mProgress, game_ctx.getString(R.string.get_user_title), 
            		game_ctx.getString(R.string.get_user_message), new OnCancelListener() {
                
                @Override
                public void onCancel(DialogInterface dialog) {
                	mUserInfoTask.doCancel();
                }
            });

            // 请求应用服务器，用AccessToken换取UserInfo
            mUserInfoTask.doRequest(game_ctx, tokenInfo.getAccessToken(), Matrix.getAppKey(game_ctx), this);
        }
	}

	@Override
	public void onGameExit()
	{
		
		super.doSdkQuit(false);
	}

	@Override
	public void onGamePause() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onGameResume() {
		// TODO Auto-generated method stub
		
	}
}
