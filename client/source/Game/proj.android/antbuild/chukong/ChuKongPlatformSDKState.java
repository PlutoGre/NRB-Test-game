package com.youai.dreamonepiece;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.youai.IGameActivity;
import com.youai.IGameActivityState;
import com.youai.IPlatformLoginAndPay;
import com.youai.IStateManager;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.GameInfo;
import com.youai.dreamonepiece.GameInterface.IPlatformSDKStateCallback;
import com.youai.dreamonepiece.platform.punchbox.PlatformChuKongLoginAndPay;





public class PlatformSDKState implements IGameActivityState
{
	public static final String	TAG	= PlatformSDKState.class.getSimpleName();

	@Override
	public void enter()
	{
		Log.d(TAG, "enter PlatformSDKState");
		
		mGameInfo = mGameActivity.getGameInfo();
		
		mPlatform = createPlatformSDKByType(mGameInfo.platform_type);
		
		mCallback.initPlatformSDK(mPlatform);
		
		mPlatform.setPlatformSDKStateCallback(new PlatformSDKStateCallback());
		
		mHandler = new PlatformSDKStateHandler();
		
		int layoutId = mPlatform.getPlatformLogoLayoutId();
		if (layoutId == -1)
		{
			mPlatform.init(mGameActivity, mGameInfo);
		}
		else if (layoutId == 0)
		{
			mPlatform.init(mGameActivity, mGameInfo);
		}
		else
		{
			mGameActivity.getActivity().setContentView(layoutId);
			
			mHandler.sendEmptyMessageDelayed(PlatformSDKState_MainThreadMsg_DoInitAfterSetView, 1500);
		}
		
		
	}

	@Override
	public void exit()
	{
		
		mStateMgr = null;
		mGameActivity = null;
		mCallback = null;
		
		mPlatform.setPlatformSDKStateCallback(null);
		mPlatform = null;
		
		mGameInfo = null;
		mHandler.removeMessages(PlatformSDKState_MainThreadMsg_DoInitAfterSetView);
		mHandler = null;
		
		Log.d(TAG, "exit PlatformSDKState");
	}
	
	/*
	 * 
	 * */
	public PlatformSDKState(IStateManager pStateMgr, IGameActivity pGameActivity, IPlatformSDKStateCallback pCallback)
	{
		mStateMgr = pStateMgr;
		mGameActivity = pGameActivity;
		mCallback = pCallback;
	}
	/*
	 * 
	 * */
	private IPlatformLoginAndPay createPlatformSDKByType(int type)
	{
        // 仅显式选择本地渠道时启用；类由公共打包准备步骤一同复制。
        if (type == PlatformAndGameInfo.enPlatform_Default) {
            return com.youai.dreamonepiece.platform.defaultplatform.PlatformDefaultLoginAndPay.getInstance();
        }
		IPlatformLoginAndPay platform = null;
		
		switch (type)
		{
		case PlatformAndGameInfo.enPlatform_ChuKong:
			platform = PlatformChuKongLoginAndPay.getInstance();
			break;
		}
		
		return platform;
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
	private IPlatformSDKStateCallback mCallback;
	/*
	 * */
	private IPlatformLoginAndPay mPlatform;
	/*
	 * 
	 * */
	private GameInfo mGameInfo;
	/*
	 * SDK的init操作耗时，先切换view，再执行init
	 * */
	private static final int PlatformSDKState_MainThreadMsg_DoInitAfterSetView = 0;
	private class PlatformSDKStateHandler extends Handler
	{
		/*
		 * 在主线程
		 * */
		public void handleMessage(Message msg)
		{
			if (msg.what == PlatformSDKState_MainThreadMsg_DoInitAfterSetView)
			{
				mPlatform.init(mGameActivity, mGameInfo);
			}
		}
	}
	private PlatformSDKStateHandler mHandler;
	/*
	 * */
	private class PlatformSDKStateCallback implements IPlatformSDKStateCallback
	{

		@Override
		public void initPlatformSDK(IPlatformLoginAndPay platform)
		{
			// none
			
		}

		@Override
		public void notifyInitPlatformSDKComplete()
		{
			
			mCallback.notifyInitPlatformSDKComplete();
			
			mStateMgr.changeState(GameInterface.GameUpdateStateID);
		}
		
	}
}


