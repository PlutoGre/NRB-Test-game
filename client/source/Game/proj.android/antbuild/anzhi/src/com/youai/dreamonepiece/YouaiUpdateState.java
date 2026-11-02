

package com.youai.dreamonepiece;

import android.app.Activity;
import android.util.Log;

import com.youai.IGameActivity;
import com.youai.IGameActivityState;
import com.youai.IStateManager;
import com.youai.dreamonepiece.GameInterface.IYouaiUpdateStateCallback;




public class YouaiUpdateState implements IGameActivityState
{
	public static final String	TAG	= YouaiUpdateState.class.getSimpleName();
	
	@Override
	public void enter()
	{
		Log.d(TAG, "enter YouaiUpdateState");
		// TODO Auto-generated method stub
		mStateMgr.changeState(GameInterface.PlatformSDKStateID);
		
	}

	@Override
	public void exit()
	{
		// TODO Auto-generated method stub
		mStateMgr = null;
		mGameActivity = null;
		mCallback = null;
		
		Log.d(TAG, "exit YouaiUpdateState");
	}
	
	/*
	 * 
	 * */
	public YouaiUpdateState(IStateManager pStateMgr, IGameActivity pGameActivity, IYouaiUpdateStateCallback pCallback)
	{
		mStateMgr = pStateMgr;
		mGameActivity = pGameActivity;
		mCallback = pCallback;
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
	private IYouaiUpdateStateCallback mCallback;
}


