
/*
 * utf-8 encoding
 * */

package com.youai.dreamonepiece.platform.wandoujia;

import java.net.URLEncoder;
import java.util.UUID;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.widget.Toast;

import com.wandoujia.paysdkimpl.WandouPayImpl;
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
import com.wandoujia.login.AccountHelper;
import com.wandoujia.paydef.LoginCallBack;
import com.wandoujia.paydef.MSG;
import com.wandoujia.paydef.OrderCallBack;
import com.wandoujia.paydef.PayCallBack;
import com.wandoujia.paydef.User;
import com.wandoujia.paydef.WandouAccount;
import com.wandoujia.paydef.WandouOrder;
import com.wandoujia.paydef.WandouPay;
import com.wandoujia.paysdk.PayConfig;

public class PlatformWanDouJiaLoginAndPay implements IPlatformLoginAndPay {
	
	private static final String TAG = PlatformWanDouJiaLoginAndPay.class.getSimpleName();
	
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
	private boolean						is_logined = false;
	
	 // 需要配置的部分- 结束
    private WandouPay _wandoupay ;
    private static User _currUser;
    private WandouOrder _wandoujiaOrder;
	private static PlatformWanDouJiaLoginAndPay sInstance = null;
	private final static String checkTokenUrl = "https://pay.wandoujia.com/api/uid/check";
	private long _startTime=0;
	public static PlatformWanDouJiaLoginAndPay getInstance()
	{
		if (sInstance == null) 
		{
			sInstance = new PlatformWanDouJiaLoginAndPay();
			_currUser=null;
		}
		return sInstance;
	}
	
	private PlatformWanDouJiaLoginAndPay() 
	{
		
	}
	
	public int getPlatformLogoLayoutId()
	{
		//return R.layout.logo_youai;
		return -1;
	}

	@Override
	public void init(IGameActivity game_acitivity, GameInfo game_info)
	{
		
		mGameActivity = game_acitivity;
		this.game_ctx = game_acitivity.getActivity();
		this.game_info = game_info;

		game_info.debug_mode = PlatformAndGameInfo.enDebugMode_Release;
		this.game_info.screen_orientation  = PlatformAndGameInfo.enScreenOrientation_Portrait;
		
		_wandoupay= new WandouPayImpl();
		PayConfig.init(game_ctx,(long)game_info.app_id,game_info.app_key);
		
		final IPlatformSDKStateCallback callback1 = mCallback1;
		callback1.notifyInitPlatformSDKComplete();
		
	}

	@Override
	public void unInit() {
		
		mGameActivity = null;
		mCallback1 = null;
		mCallback2 = null;
		mCallback3 = null;
		//
		WandouAccount account = new AccountHelper();
        account.doLogout(this.game_ctx, null);
		this.game_ctx = null;
		this.game_info = null;
		this.login_info = null;
		this.version_info = null;
		this.pay_info = null;
		this._currUser=null;
		//
		PlatformWanDouJiaLoginAndPay.sInstance = null;
	}

	@Override
	public GameInfo getGameInfo() {
		
		return this.game_info;
	}

	@Override
	public void callLogin() {
		
		Log.d(TAG, "WandouAccount.doLogin");
		
		final IGameAppStateCallback callback = mCallback3;
		
		callback.showWaitingViewImp(true, -1, "正在登录");
		
        WandouAccount account = new AccountHelper();
        Log.w(TAG, "doLogin!");
        
        account.doLogin(this.game_ctx, new LoginCallBack() {
        	String tip = "";
            @Override
            public void onError(int code, String info) {
            	tip = "豌豆荚登录失败，错误代码：" + code;
            	if (code == 0)
            		tip = "请点击进入游戏重新登录";
				//tip = tip + "，正在重试，请稍后";
				if (PlatformWanDouJiaLoginAndPay.getInstance().auto_recalllogin_count < 5) {
					PlatformWanDouJiaLoginAndPay.getInstance().auto_recalllogin_count++;
					
					//try {
						//Thread.sleep(2000);
					//} catch (InterruptedException e) {
						//
					//	e.printStackTrace();
					//}
					
					//PlatformWanDouJiaLoginAndPay.getInstance().callLogin();
				}
				else {
					tip = "豌豆荚登录失败，请检查手机网络，并重新启动游戏";
				}
				_currUser=null;
				Toast.makeText(PlatformWanDouJiaLoginAndPay.getInstance().game_ctx, tip, Toast.LENGTH_SHORT).show();
				callback.showWaitingViewImp(false, -1, "重新登录");
            }

            @Override
            public void onSuccess(User user, int type)
            {
            	_currUser=user;
                Log.w("login", "success:+" + user);
                Log.w("checkTokenUrl",
                        checkTokenUrl + "?uid=" + user.getUid() + "&token="
                                + URLEncoder.encode(user.getToken()));
                tip = "登录成功，点击进入游戏";
                LoginInfo login_info = new LoginInfo();
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
				login_info.login_session = user.getToken();
				login_info.account_uid_str =PlatformAndGameInfo.enPlatformShort_WanDouJia+user.getUid().toString();
				login_info.account_uid = Long.parseLong(user.getUid().toString());
				login_info.account_nick_name = user.getNick();
				//
				PlatformWanDouJiaLoginAndPay.getInstance().notifyLoginResult(login_info);
                Toast.makeText(PlatformWanDouJiaLoginAndPay.getInstance().game_ctx, tip, Toast.LENGTH_SHORT).show(); 
            }
        });
	}

	@Override
	public void notifyLoginResult(LoginInfo login_result) {
		
		login_info = null;
		login_info = login_result;
		//
		if (login_result != null) {
			
			is_logined = true;
			
			mCallback3.notifyLoginResut(login_result);
		}
	}

	@Override
	public LoginInfo getLoginInfo() {
		
		if (login_info != null) {
			if(_currUser!=null)
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Success;
			else
				login_info.login_result = PlatformAndGameInfo.enLoginResult_Failed;
		}
		
		return login_info;
	}

	@Override
	public void callLogout() 
	{
		WandouAccount account = new AccountHelper();
        account.doLogout(this.game_ctx, null);
        //
        is_logined = false;
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

	@Override
	public int callPayRecharge(PayInfo pay_info)
	{
		int aError=0;
		this.pay_info = null;
		this.pay_info = pay_info;
		// 三个参数分别是 游戏名(String)，商品(String)，价格(Long)单位是分
        _wandoujiaOrder = new WandouOrder("梦想海贼王", this.pay_info.product_name, Long.valueOf((int)(this.pay_info.price*100)));//单位是分

        _startTime = System.currentTimeMillis();

        _wandoujiaOrder.out_trade_no = pay_info.description + "-" + pay_info.product_id + "-" + this.login_info.account_uid_str;// 这里设置外部订单号，不要超过50个字符
        PlatformWanDouJiaLoginAndPay.getInstance().pay_info.result = -1;
        _wandoupay.createOrder(this.game_ctx, _wandoujiaOrder, new OrderCallBack() 
        {
            @Override
            public void onError(WandouOrder order)
            {
				Toast.makeText(
				PlatformWanDouJiaLoginAndPay.getInstance().game_ctx,
				"订单生成失败", Toast.LENGTH_SHORT).show();
				Log.w(TAG, "success:" + order.toJSONString());
				PlatformWanDouJiaLoginAndPay.getInstance().pay_info.result = -1;
    			PlatformWanDouJiaLoginAndPay.getInstance()
    				.notifyPayRechargeRequestResult(PlatformWanDouJiaLoginAndPay.getInstance().pay_info);
            }

            @Override
            public void onSuccess(WandouOrder order) {
				Toast.makeText(
				PlatformWanDouJiaLoginAndPay.getInstance().game_ctx,
				"请支付订单", Toast.LENGTH_SHORT).show();
                Log.w(TAG, "success:" + order.toJSONString());
                PlatformWanDouJiaLoginAndPay.getInstance().pay_info.result = 0;
    			PlatformWanDouJiaLoginAndPay.getInstance()
    				.notifyPayRechargeRequestResult(PlatformWanDouJiaLoginAndPay.getInstance().pay_info);
            }
			
        });

		return aError;
	}

	@Override
	public void notifyPayRechargeRequestResult(PayInfo pay_info) 
	{
		final PayInfo the_pay_info = pay_info;
		//
		if(PlatformWanDouJiaLoginAndPay.getInstance().pay_info.result == 0)
		{
			if (_wandoujiaOrder == null || _wandoujiaOrder.order_no == 0L || pay_info.order_serial.length() < 2)
			{
				Toast.makeText(
						PlatformWanDouJiaLoginAndPay.getInstance().game_ctx,
						"请先生成订单", Toast.LENGTH_SHORT).show();
	        } 
			else 
			{
	            Log.w("Pay", "pay!");
	            _wandoupay.pay(PlatformWanDouJiaLoginAndPay.getInstance().game_ctx, _wandoujiaOrder, new PayCallBack()
	            {

	                @Override
	                public void onSuccess(User user, WandouOrder order) 
	                {
	                    Log.w("DemoPay", "onSuccess:" + order + " status:" + order.status(order.TRADE_SUCCESS));
	                    String showMsg = "支付成功！";
	                    Toast.makeText(
	            				PlatformWanDouJiaLoginAndPay.getInstance().game_ctx,
	            				showMsg, Toast.LENGTH_SHORT).show();
	                    //
	                    mCallback3.notifyPayRechargeResult(the_pay_info);
	                }

	                @Override
	                public void onError(User user, WandouOrder order)
	                {
	                    Log.w("DemoPay", "onError:" + order);
	                    String showMsg = "支付失败！";
	                    Toast.makeText(
	            				PlatformWanDouJiaLoginAndPay.getInstance().game_ctx,
	            				showMsg, Toast.LENGTH_SHORT).show();
	                    //
	                    
	                }
	            });
			}
		}
		//
	}
	
	@Override
	public void callAccountManage() 
	{
		if (Cocos2dxHelper.nativeHasEnterMainFrame())
			return;
		//if (!is_logined)
		if (is_logined)
			callLogout();
		//
		callLogin();
	}

	@Override
	public String generateNewOrderSerial() {
		
		return UUID.randomUUID().toString();//字符串带横杠字符-
	}

	@Override
	public void callPlatformFeedback() {
		
		//NdCommplatform.getInstance().ndUserFeedback(this.game_ctx); 
	}

	@Override
	public void callPlatformSupportThirdShare(ShareInfo share_info) {
		
		if (share_info.bitmap == null) {
			share_info.bitmap = BitmapFactory.decodeFile(share_info.img_path);
		}
		
		//NdCommplatform.getInstance().ndShareToThirdPlatform(game_ctx, share_info.content, share_info.bitmap);
	}
	
	@Override
	public void callPlatformGameBBS() {
		
		//NdCommplatform.getInstance().ndEnterAppBBS(game_ctx, 0);
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
		 * 2013-06-23 Ver3.2.5.1SDK的init自动发起了更新检查，但没有单独的回调，所以我们仍�?��己请求一�?
		 * */
		return PlatformAndGameInfo.DoNotSupportUpdate;
	}

	@Override
	public void onGameExit()
	{
		WandouAccount account = new AccountHelper();
        account.doLogout(this.game_ctx, null);
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
