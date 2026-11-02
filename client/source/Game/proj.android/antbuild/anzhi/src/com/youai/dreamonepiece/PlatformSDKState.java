

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
//import com.youai.dreamonepiece.platform.nd91.Platform91LoginAndPay;
//import com.youai.dreamonepiece.platform.qihoo360.Platform360LoginAndPay;
//import com.youai.dreamonepiece.platform.sina_wyx.PlatformSinaLoginAndPay;
//import com.youai.dreamonepiece.platform.renren.PlatformRenRenLoginAndPay;
//import com.youai.dreamonepiece.platform.thirdlogin.PlatformThirdLoginAndPay;
//import com.youai.dreamonepiece.platform.uc.PlatformUCLoginAndPay;
//import com.youai.dreamonepiece.platform.wandoujia.PlatformWanDouJiaLoginAndPay;
//import com.youai.dreamonepiece.platform.yingyonghui.PlatformYingYongHuiLoginAndPay;
//import com.youai.dreamonepiece.platform.jifeng.PlatformJifengLoginAndPay;
//import com.youai.dreamonepiece.platform.kingsoft.PlatformJinShanLoginAndPay;
//import com.youai.dreamonepiece.platform.mi.PlatformXiaoMiLoginAndPay;
import com.youai.dreamonepiece.platform.anzhi.PlatformAnZhiLoginAndPay;
//import com.youai.dreamonepiece.platform.dangle.PlatformDangleLoginAndPay;
//import com.youai.dreamonepiece.platform.defaultplatform.PlatformDefaultLoginAndPay;
//import com.youai.dreamonepiece.platform.feiliu.PlatformFeiLiuLoginAndPay;
//import com.youai.dreamonepiece.platform.DK.PlatformDuokuLoginAndPay;




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
		IPlatformLoginAndPay platform = null;
		
		switch (type)
		{
		case PlatformAndGameInfo.enPlatform_91:
			//platform = Platform91LoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_360:
			//platform = Platform360LoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_UC:
			//platform = PlatformUCLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_WanDouJia:
			//platform = PlatformWanDouJiaLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_XiaoMi:
			//platform = PlatformXiaoMiLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_DangLe:
			//platform = PlatformDangleLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_BaiduDuoKu:
			//platform = PlatformDuokuLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_Sina:
			//platform = PlatformSinaLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_RenRen:
			//platform = PlatformRenRenLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_Default:
			//platform = PlatformDefaultLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_ThirdLogin:
			//platform = PlatformThirdLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_JiFeng:
			//platform = PlatformJifengLoginAndPay.getInstance();
			break;	
		case PlatformAndGameInfo.enPlatform_YingYongHui:
			//platform = PlatformYingYongHuiLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_FeiLiu:
			//platform = PlatformFeiLiuLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_AnZhi:
			platform = PlatformAnZhiLoginAndPay.getInstance();
			break;
		case PlatformAndGameInfo.enPlatform_JinShan:
		    //platform = PlatformJinShanLoginAndPay.getInstance();
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


