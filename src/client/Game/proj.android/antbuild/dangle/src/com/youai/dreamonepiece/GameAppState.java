

package com.youai.dreamonepiece;

import org.cocos2dx.lib.Cocos2dxHandler;
import org.cocos2dx.lib.Cocos2dxHandler.PayRechargeMessage;
import org.cocos2dx.lib.Cocos2dxHandler.ShareMessage;
import org.cocos2dx.lib.Cocos2dxHandler.ShowWaitingViewMessage;

import android.app.Activity;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.TouchDelegate;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.youai.IGameActivity;
import com.youai.IGameActivityState;
import com.youai.IPlatformLoginAndPay;
import com.youai.IStateManager;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.LoginInfo;
import com.youai.PlatformAndGameInfo.PayInfo;
import com.youai.PlatformAndGameInfo.ShareInfo;
import com.youai.dreamonepiece.GameInterface.IGameAppStateCallback;
import com.youai.dreamonepiece.platform.dangle.R;




public class GameAppState implements IGameActivityState
{
	public static final String	TAG	= GameAppState.class.getSimpleName();

	@Override
	public void enter()
	{
		Log.d(TAG, "enter GameAppState");
		/*
		 * 这时的ContentView是R.layout.activity_game
		 * */
		mWaitingView = mActivity.findViewById(R.id.GameApp_WaitingFrameLayout);
		mWaitingView.setBackgroundColor(0x400f0f0f);
		
		mProgress = (ProgressBar) mActivity.findViewById(R.id.waiting_progressBar);
		mWaitingText = (TextView) mActivity.findViewById(R.id.waiting_text);
		mWaitingText.setTextColor(0xfffdfdfd);
		
		View logoImg = mActivity.findViewById(R.id.GameApp_LogoRelativeLayout);
		logoImg.setVisibility(View.INVISIBLE);
		logoImg = null;
		
		mCallback.notifyEnterGameAppState(new GameAppStateHandler());
		
		mPlatform.setGameAppStateCallback(new GameAppStateCallback());
		
		showWaitingViewImp(true, -1, "检查版本更新");
	}

	@Override
	public void exit()
	{
		
		mStateMgr = null;
		mGameActivity = null;
		mCallback = null;
		
		mActivity = null;
		mPlatform.setGameAppStateCallback(null);
		mPlatform = null;
		
		mWaitingView = null;
		mProgress = null;
		mWaitingText = null;
		
		mOwnTouchDelegate = null;
		mOurTouchDelegate = null;
		
		Log.d(TAG, "exit GameAppState");
	}
	
	/*
	 * 
	 * */
	public GameAppState(IStateManager pStateMgr, IGameActivity pGameActivity, IGameAppStateCallback pCallback)
	{
		mStateMgr = pStateMgr;
		mGameActivity = pGameActivity;
		mCallback = pCallback;
		
		mActivity = mGameActivity.getActivity();
		mPlatform = mGameActivity.getPlatformSDK();
	}
	/*
	 * 
	 * */
	private IStateManager mStateMgr;
	/*
	 * 
	 * */
	private IGameActivity mGameActivity;
	/*
	 * 
	 * */
	private Activity mActivity;
	/*
	 * 
	 * */
	private IGameAppStateCallback mCallback;
	/*
	 * 
	 * */
	private IPlatformLoginAndPay mPlatform;
	/*
	 * */
	private View mWaitingView;
	/*
	 * */
	private ProgressBar mProgress;
	/*
	 * */
	private TextView mWaitingText;
	/*
	 * 
	 * */
	private class GameAppStateHandler extends Handler
	{
		public void handleMessage(Message msg)
		{
			//
			if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformLogin)
			{
				mCallback.notifyOnTempShortPause();
				
				mPlatform.callLogin();
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformLogout)
			{
				mPlatform.callLogout();
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformAccountManage)
			{
				mCallback.notifyOnTempShortPause();
				mPlatform.callAccountManage();
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformPayRecharge)
			{
				mCallback.notifyOnTempShortPause();//支付有的activity跳activity，容易把我们的游戏activity搞的自动重启！
				//
				PayRechargeMessage obj = (PayRechargeMessage)msg.obj;
				//
				PayInfo pay_info = new PayInfo();
				pay_info.order_serial = obj.serial;
				pay_info.product_id = obj.productId;
				pay_info.product_name = obj.productName;
				pay_info.price = obj.price;
				pay_info.orignal_price = obj.orignalPrice;
				pay_info.count = obj.count;
				pay_info.description = obj.description;
				
				if (pay_info.order_serial.isEmpty())
					pay_info.order_serial = mPlatform.generateNewOrderSerial();
				//
				mPlatform.callPayRecharge(pay_info);
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformFeedback)
			{
				mCallback.notifyOnTempShortPause();
				mPlatform.callPlatformFeedback();
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformThirdShare)
			{
				mCallback.notifyOnTempShortPause();
				//
				ShareMessage obj = (ShareMessage)msg.obj;
				//
				ShareInfo share_info = new ShareInfo();
				share_info.img_path = obj.imgPath;
				share_info.content = obj.content;
				//
				mPlatform.callPlatformSupportThirdShare(share_info);
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_CallPlatformBBS)
			{
				mCallback.notifyOnTempShortPause();
				mPlatform.callPlatformGameBBS();
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_ShowWaitingView)
			{
				ShowWaitingViewMessage obj = (ShowWaitingViewMessage)msg.obj;
				showWaitingViewImp(obj.show, obj.progress, obj.text);
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_OnActivityPause)
			{
				mPlatform.onGamePause();
			}
			else if (msg.what == Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_OnActivityResume)
			{
				//mCallback.notifyOnTempShortPause();
				mPlatform.onGameResume();
			}
		}
	}
	/*
	 * 
	 * */
	private class GameAppStateCallback implements IGameAppStateCallback
	{

		@Override
		public void notifyEnterGameAppState(Handler handler)
		{
			// none
			
		}

		@Override
		public void notifyOnTempShortPause()
		{
			
			mCallback.notifyOnTempShortPause();
		}

		@Override
		public void notifyLoginResut(LoginInfo result)
		{
			showWaitingViewImp(false, -1, "");
			
			if (result.login_result == PlatformAndGameInfo.enLoginResult_Success)
			{
				mCallback.notifyLoginResut(result);
			}
		}

		@Override
		public void notifyPayRechargeResult(PayInfo result)
		{
			
			mCallback.notifyPayRechargeResult(result);
		}

		@Override
		public void showWaitingViewImp(boolean show, int progress, String text)
		{
			/*
			 * SendMessage
			 * */
			mCallback.showWaitingViewImp(show, progress, text);
		}
		
	}
	/*
	 * 
	 * */
	private TouchDelegate			mOwnTouchDelegate;
	private TouchDelegate			mOurTouchDelegate;
	/*
	 * 
	 * */
	private void showWaitingViewImp(boolean show, int progress, String text)
	{
		if (mOwnTouchDelegate == null)
			mOwnTouchDelegate = mWaitingView.getTouchDelegate();
		if (mOurTouchDelegate == null) {
			Rect rt = new Rect();
			rt.top = rt.left = 0;
			rt.bottom = mWaitingView.getHeight();
			rt.right = mWaitingView.getWidth();
			
			mOurTouchDelegate = new TouchDelegate(rt, mWaitingView) {
				
				@Override
				public boolean onTouchEvent(MotionEvent event) {
					return true;	
				}
			};
		}
		//
		if (show) {
			
			mWaitingView.setVisibility(View.VISIBLE);
			mWaitingText.setText(text);
			//
			mWaitingView.setTouchDelegate(mOurTouchDelegate);
		}
		else {
			
			mWaitingView.setVisibility(View.INVISIBLE);
			mWaitingText.setText("");
			//
			mWaitingView.setTouchDelegate(mOwnTouchDelegate);
		}
	}
}


