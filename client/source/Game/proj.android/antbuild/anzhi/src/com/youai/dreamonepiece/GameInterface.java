
package com.youai.dreamonepiece;

import android.os.Handler;
import android.view.View;

import com.youai.IGameActivity;
import com.youai.IPlatformLoginAndPay;
import com.youai.PlatformAndGameInfo.LoginInfo;
import com.youai.PlatformAndGameInfo.PayInfo;
import com.youai.PlatformAndGameInfo.VersionInfo;



public interface GameInterface
{
	public static int GameStateIDMin = 0;
	public static int GameLogoStateID = 1;
	public static int YouaiUpdateStateID = 2;
	public static int PlatformSDKStateID = 3;
	public static int GameUpdateStateID = 4;
	public static int GameContextStateID = 5;
	public static int GameAppStateID = 6;
	public static int GameStateIDMax = 7;
	/*
	 * 
	 * */
	public static interface IGameLogoStateCallback
	{
		/*
		 * */
		public void initAppDataPath(String fullPath);
	}
	/*
	 * 
	 * */
	public static interface IYouaiUpdateStateCallback
	{
		
	}
	/*
	 * 
	 * */
	public static interface IPlatformSDKStateCallback
	{
		/*
		 * */
		public void initPlatformSDK(IPlatformLoginAndPay platform);
		/*
		 * */
		public void notifyInitPlatformSDKComplete();
	}
	/*
	 * 
	 * */
	public static interface IGameUpdateStateCallback
	{
		/*
		 * */
		public void notifyVersionCheckResult(VersionInfo versionInfo);
	}
	/*
	 * 
	 * */
	public static interface IGameContextStateCallback
	{
		/*
		 * */
		public void initCocos2dxAndroidContext(View glView, View editText, Handler handler);
	}
	/*
	 * 
	 * */
	public static interface IGameAppStateCallback
	{
		/*
		 * 
		 * */
		public void notifyEnterGameAppState(Handler handler);
		/*
		 * 
		 * */
		public void notifyOnTempShortPause();
		/*
		 * 
		 * */
		public void notifyLoginResut(LoginInfo result);
		/*
		 * 
		 * */
		public void notifyPayRechargeResult(PayInfo result);
		/*
		 * 
		 * */
		public void showWaitingViewImp(boolean show, int progress, String text);
	}
	/*
	 * 
	 * */
	public static interface IDreamOnePiece extends IGameActivity
													,IGameLogoStateCallback
													,IYouaiUpdateStateCallback
													,IPlatformSDKStateCallback
													,IGameUpdateStateCallback
													,IGameContextStateCallback
													,IGameAppStateCallback
	{
		
	}
}

