

package com.youai.dreamonepiece;

import com.youai.IGameActivityState;
import com.youai.IStateManager;
import com.youai.dreamonepiece.GameInterface.IDreamOnePiece;





public class GameStateManager implements IStateManager
{

	@Override
	public void changeState(int stateID)
	{
		if (stateID < GameInterface.GameStateIDMin || 
				stateID >= GameInterface.GameStateIDMax)
		{
			return;
		}
		
		if (mStates[stateID] == null)
		{
			mStates[stateID] = createStateByID(stateID);
		}
		
		if (mStates[mCurrentStateID] != null)
		{
			mStates[mCurrentStateID].exit();
			/*
			 * 顺序切换，可以置null
			 * 不能置null了，因为update没等回调而直接切换了
			 * */
			//mStates[mCurrentStateID] = null;
		}
		
		mCurrentStateID = stateID;
		if (mStates[stateID] != null)
		{
			mStates[stateID].enter();
		}
	}
	
	/*
	 * 
	 * */
	public GameStateManager(int count, IDreamOnePiece pGameActivity)
	{
		mStates = new IGameActivityState[count];
		mGameActivity = pGameActivity;
	}
	/*
	 * 
	 * */
	private IGameActivityState createStateByID(int stateID)
	{
		IGameActivityState pState = null;
		switch (stateID)
		{
			case GameInterface.GameLogoStateID:
			{
				pState = new GameLogoState(this, mGameActivity, mGameActivity);
				break;
			}
			case GameInterface.YouaiUpdateStateID:
			{
				pState = new YouaiUpdateState(this, mGameActivity, mGameActivity);
				break;
			}
			case GameInterface.PlatformSDKStateID:
			{
				pState = new PlatformSDKState(this, mGameActivity, mGameActivity);
				break;
			}
			case GameInterface.GameUpdateStateID:
			{
				pState = new GameUpdateState(this, mGameActivity, mGameActivity);
				break;
			}
			case GameInterface.GameContextStateID:
			{
				pState = new GameContextState(this, mGameActivity, mGameActivity);
				break;
			}
			case GameInterface.GameAppStateID:
			{
				pState = new GameAppState(this, mGameActivity, mGameActivity);
				break;
			}
		}
		return pState;
	}
	/*
	 * 
	 * */
	private IGameActivityState[] mStates = null;
	/*
	 * 
	 * */
	private int mCurrentStateID = 0;
	/*
	 * 
	 * */
	private IDreamOnePiece mGameActivity = null;
}

