/*
 * 
 * 
 * 
 * 
 * 
 * */

package com.youai.dreamonepiece;

import java.io.File;
import java.util.ArrayList;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHandler;
import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Toast;

import com.youai.DeviceUtil;
import com.youai.IPlatformLoginAndPay;
import com.youai.IStateManager;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.GameInfo;
import com.youai.PlatformAndGameInfo.LoginInfo;
import com.youai.PlatformAndGameInfo.PayInfo;
import com.youai.PlatformAndGameInfo.ShareInfo;
import com.youai.PlatformAndGameInfo.VersionInfo;
import com.youai.dreamonepiece.GameInterface.IDreamOnePiece;

//

public class GameActivity extends Cocos2dxActivity implements IDreamOnePiece {
	// ////////////////////////////////////////////////////////////////////////
	static {

		System.loadLibrary("DreamOnePiece");

	}
	// ////////////////////////////////////////////////////////////////////////

	private static final String TAG = GameActivity.class.getSimpleName();

	//
	protected GameConfig mGameCfg = null;
	private IStateManager mStateMgr = new GameStateManager(
			GameInterface.GameStateIDMax, this);
	protected IPlatformLoginAndPay mPlatform = null;
	//
	private Handler mGameContextStateHandler;
	//
	private long mLastMenuKeyDownTimeMillis = 0;
	private long mRecentPressMenuKeyDownCount = 0;
	//
	private boolean mHaveEnteredGameAppState = false;
	private VersionInfo mVersionResult;
	private DownloadApk updateApk;
	
	//
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);

		mStateMgr.changeState(GameInterface.GameLogoStateID);
		startService(new Intent(GameActivity.this, NotificationService.class));
	}

	@Override
	protected void onDestroy() {
		//
		super.onDestroy();
		if(updateApk!=null){
			updateApk.onDestroy();
			updateApk = null;
		}
		//
		Log.d(TAG, "call onDestroy and System exit");
		/*
		 * OpenGL ES Context 没有能正确释放，只得System.exit了！不然影响重启
		 */
		System.exit(0);
	}

	@Override
	protected void destroy() {

		Log.d(TAG, "call destroy");
		//
		if (mPlatform != null) {
			Log.d(TAG, "mPlatform.unInit");
			// 要求mPlatform里的Activities进行finish
			mPlatform.unInit();
			mPlatform = null;
		}
		//
		super.destroy();
		//
		mStateMgr.changeState(GameInterface.GameStateIDMin);
		mStateMgr = null;
	}

	@Override
	protected void onStart() {
		// home键会调游戏程序进入后台，渲染跳过、网络断�?��但程序并未unInitiated，再次点击应用图标会立即调出进入后台前的界面，并尝试连网登录
		super.onStart();
		//
		Log.d(TAG, "call onStart");
		//

	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		Log.d(TAG, "onKeyDown, keyCode: " + String.valueOf(keyCode)
				+ " getDownTime: " + String.valueOf(event.getDownTime()));

		if (keyCode == KeyEvent.KEYCODE_POWER) {
			if (event.isLongPress()) {
				Log.d(TAG, "KEYCODE_POWER isLongPress");

				super.onLowMemory();

				return true;
			}
		} else if (keyCode == KeyEvent.KEYCODE_MENU) {
				 
			if (mIsTempShortPause || mIsOnPause)
				return super.onKeyDown(keyCode, event);

			final long nowtime = android.os.SystemClock.elapsedRealtime();

			if ((nowtime - mLastMenuKeyDownTimeMillis) < 3000 // 3秒之内再按第2�?
					&& mRecentPressMenuKeyDownCount > 0) {

				mRecentPressMenuKeyDownCount = 0;
				Toast.makeText(this, "正在截屏", Toast.LENGTH_SHORT).show();

				final GameActivity theActivity = this;
				//
				// if (repeatCount == 1) {
				super.runOnGLThread(new Runnable() {

					@Override
					public void run() {

						String png_file = Cocos2dxHelper.nativeGameSnapshot();
						//
						ShareInfo share = new ShareInfo();
						share.content = "#梦想海贼王##DreamOnePiece#@梦想海贼王Online";
						share.img_path = png_file;
						// share.bitmap = bmp;
						theActivity.callSystemShare(share);
					}

				});
				// }
				//
				return true;
			}

			if ((nowtime - mLastMenuKeyDownTimeMillis) < 3000)// 两次截屏之间�?�?
				return super.onKeyDown(keyCode, event);

			mLastMenuKeyDownTimeMillis = nowtime;

			if (mRecentPressMenuKeyDownCount < 1) {

				Toast.makeText(this, "再按一次截屏分享", Toast.LENGTH_SHORT).show();
				// mLastMenuKeyDownTimeMillis -= 2500;
				mRecentPressMenuKeyDownCount++;
				//
				return super.onKeyDown(keyCode, event);
			} else {
				Toast.makeText(this, "再按一次截屏分享", Toast.LENGTH_SHORT).show();
				return super.onKeyDown(keyCode, event);
			}
		} else if (keyCode == KeyEvent.KEYCODE_BACK) {

			if (mGameCfg.mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_91) {
				//
				if (mGameCfg.mGameInfo.use_platform_sdk_type == 0)
					super.onKeyDown(keyCode, event);
				else if (mGameCfg.mGameInfo.use_platform_sdk_type == 1) {
					if (mPlatform != null) {
						super.setOnTempShortPause(true);
						mPlatform.onGameExit();
					}
				}
				//
			} else if (mGameCfg.mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_360) {
				if (mPlatform != null) {
					super.setOnTempShortPause(true);
					mPlatform.onGameExit();
					return true;
				} else {
					super.onKeyDown(keyCode, event);
				}
			} else {
				super.onKeyDown(keyCode, event);
			}
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event) {
		Log.d(TAG, "onKeyLongPress, keyCode: " + String.valueOf(keyCode));

		if (mIsTempShortPause || mIsOnPause)
			return super.onKeyDown(keyCode, event);

		if (keyCode == KeyEvent.KEYCODE_MENU) {
			final GameActivity theActivity = (GameActivity) Cocos2dxActivity
					.getContext();
			//
			super.runOnGLThread(new Runnable() {

				@Override
				public void run() {
					{
						String png_file = Cocos2dxHelper.nativeGameSnapshot();
						//
						theActivity.callPlatformSupportThirdShare(
								"#梦想海贼王##DreamOnePiece#@梦想海贼王Online", png_file);
					}
				}

			});
			//
			return true;
		}

		return super.onKeyLongPress(keyCode, event);
	}

	public void callSystemShare(final ShareInfo share) {

		if (share == null)
			return;

		super.setOnTempShortPause(true);

		final GameActivity theActivity = this;

		new Thread(new Runnable() {

			@Override
			public void run() {

				// theActivity.onPause();

				// share.img_path =
				// theActivity.mAppDataExternalStorageCacheFullPath +
				// "/share.png";

				Intent intent1 = new Intent(Intent.ACTION_SEND);// 微信的单张图片分享失败，报读取资源失败，没找到解决方�?
				Intent intent = new Intent(Intent.ACTION_SEND_MULTIPLE);
				intent.setType("image/png");
				intent1.setType("image/png");
				// intent.setType("text/plain");
				intent.putExtra(Intent.EXTRA_SUBJECT, "分享梦想海贼王");
				intent.putExtra(Intent.EXTRA_TEXT, share.content);
				//
				intent1.putExtra(Intent.EXTRA_SUBJECT, "分享梦想海贼王");
				intent1.putExtra(Intent.EXTRA_TEXT, share.content);

				intent1.putExtra(Intent.EXTRA_STREAM,
						Uri.parse("file:///" + share.img_path));

				{
					ArrayList<Uri> arrayUri = new ArrayList<Uri>();
					arrayUri.add(Uri.parse("file:///" + share.img_path));

					intent.putExtra(Intent.EXTRA_STREAM, arrayUri);
				}

				intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				intent1.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				/*
				 * { ArrayList<Intent> arrayIntent = new ArrayList<Intent>();
				 * 
				 * //arrayIntent.add(intent); arrayIntent.add(intent1);
				 * 
				 * Intent intent3 =Intent.createChooser(intent, "分享");
				 * intent3.putExtra(Intent.EXTRA_INITIAL_INTENTS,
				 * arrayIntent.toArray(new Parcelable[]{}));
				 * intent3.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				 * 
				 * Cocos2dxActivity.getContext().startActivity(intent3);
				 * 
				 * { //arrayIntent.add(intent);
				 * //Cocos2dxActivity.getContext().startActivity( //
				 * GameActivity.myCreateChooser(arrayIntent, "分享")); } }
				 */
				// Cocos2dxActivity.getContext().startActivity(
				// Intent.createChooser(intent, "分享至微�?));
				//
				Cocos2dxActivity.getContext().startActivity(
						Intent.createChooser(intent1, "分享"));
				//
				Log.d("callSystemShare", "android.intent.action.SEND");

				// theActivity.onResume();
			}

		}).start();
	}

	@Override
	public boolean getPlatformLoginStatus() {

		if (mPlatform == null)
			return false;
		//
		// return true;//for testin
		//
		LoginInfo login_info = mPlatform.getLoginInfo();
		if (login_info == null)
			return false;

		if (login_info.login_result == PlatformAndGameInfo.enLoginResult_Success)
			return true;
		else
			return false;
	}

	@Override
	public String getPlatformLoginUin() {

		if (mPlatform == null) {
			return DeviceUtil.getDeviceUUID(this);
		}

		LoginInfo login_info = mPlatform.getLoginInfo();
		if (login_info == null) {
			return DeviceUtil.getDeviceUUID(this);
		}

		return login_info.account_uid_str;
	}

	@Override
	public String getPlatformLoginSessionId() {

		if (mPlatform == null) {
			return DeviceUtil.generateUUID();
		}

		LoginInfo login_info = mPlatform.getLoginInfo();
		if (login_info == null) {
			return DeviceUtil.generateUUID();
		}

		return login_info.login_session;
	}

	@Override
	public String getPlatformUserNickName() {

		if (mPlatform == null) {
			return DeviceUtil.getDeviceProductName(this);
		}

		LoginInfo login_info = mPlatform.getLoginInfo();
		if (login_info == null) {
			return DeviceUtil.getDeviceProductName(this);
		}

		return login_info.account_nick_name;
	}

	@Override
	public String generateNewOrderSerial() {

		return mPlatform.generateNewOrderSerial();
	}

	@Override
	public void openUrlOutside(String url) {

 
		if (url.isEmpty())
			return;
		//
		if (url.endsWith(".apk"))
		{
			updateApk = new DownloadApk(getActivity(), url);
		}
		else
		{
			super.openUrlOutside(url);
		}
	}

	@Override
	public void onTimeToShowCocos2dxContentView() {
		mGameContextStateHandler.sendEmptyMessageDelayed(
				Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_ShowCocos2dx, 0);
	}

	@Override
	public Activity getActivity() {

		return this;
	}

	@Override
	public IPlatformLoginAndPay getPlatformSDK() {

		return mPlatform;
	}

	@Override
	public void initAppDataPath(String fullPath) {

		mAppDataExternalStorageFullPath = fullPath;
		mAppDataExternalStorageResourcesFullPath = mAppDataExternalStorageFullPath
				+ "/Assets";
		mAppDataExternalStorageCacheFullPath = mAppDataExternalStorageFullPath
				+ "/Cache";
		//

		// xiaomi
		// 2S上getExternalCacheDir指向/storage/sdcard0/Android/data/PackageName/cache
		//
		File tempDir = new File(mAppDataExternalStorageCacheFullPath);
		if (!tempDir.exists())
			tempDir.mkdirs();
		//
		if (!tempDir.exists())
			Log.e(TAG,
					"AppDataExternalStorageCacheFullPath: "
							+ tempDir.getAbsolutePath() + " is not OK!");
		else
			Log.d(TAG,
					"AppDataExternalStorageCacheFullPath: "
							+ tempDir.getAbsolutePath());
		//
		tempDir = null;
		//
		tempDir = new File(mAppDataExternalStorageFullPath);
		if (!tempDir.exists())
			tempDir.mkdirs();
		//
		if (!tempDir.exists())
			Log.e(TAG,
					"mAppDataExternalStorageFullPath: "
							+ tempDir.getAbsolutePath() + " is not OK!");
		else
			Log.d(TAG,
					"mAppDataExternalStorageFullPath: "
							+ tempDir.getAbsolutePath());
		//
		tempDir = null;
		//
		tempDir = new File(mAppDataExternalStorageResourcesFullPath);
		if (!tempDir.exists())
			tempDir.mkdirs();
		//
		if (!tempDir.exists())
			Log.e(TAG,
					"AppDataExternalStorageResourcesFullPath: "
							+ tempDir.getAbsolutePath() + " is not OK!");
		else
			Log.d(TAG,
					"AppDataExternalStorageResourcesFullPath: "
							+ tempDir.getAbsolutePath());
		//
		tempDir = null;
		//
	}

	@Override
	public String getAppFilesRootPath() {
		return this.mAppDataExternalStorageFullPath;
	}

	@Override
	public String getAppFilesResourcesPath() {
		return this.mAppDataExternalStorageResourcesFullPath;
	}

	@Override
	public String getAppFilesCachePath() {
		return this.mAppDataExternalStorageCacheFullPath;
	}

	@Override
	public void requestDestroy() {
		/*
		 * 去onDestroy干了其他destroy事情
		 */
		this.destroy();
	}

	@Override
	public GameInfo getGameInfo() {

		return mGameCfg.mGameInfo;
	}

	@Override
	public void initPlatformSDK(IPlatformLoginAndPay platform) {

		mPlatform = platform;
	}

	@Override
	public void notifyInitPlatformSDKComplete() {
		Log.d(TAG, "notifyInitPlatformSDKComplete");
	}

	@Override
	public void notifyVersionCheckResult(VersionInfo versionInfo) {
		Log.d(TAG, "notifyVersionCheckResult");

		if (mHaveEnteredGameAppState) {
			if (versionInfo.update_info == PlatformAndGameInfo.enUpdateInfo_No) {
				Cocos2dxHelper.nativeNotifyPlatformGameUpdateResult(
						versionInfo.update_info, versionInfo.max_version_code,
						versionInfo.local_version_code,
						versionInfo.download_url);
			} else if (versionInfo.update_info == PlatformAndGameInfo.enUpdateInfo_Suggest) {
				Cocos2dxHelper.nativeNotifyPlatformGameUpdateResult(
						versionInfo.update_info, versionInfo.max_version_code,
						versionInfo.local_version_code,
						versionInfo.download_url);
			} else if (versionInfo.update_info == PlatformAndGameInfo.enUpdateInfo_Force) {
				Log.w(TAG, "notifyVersionCheckResult: enUpdateInfo_Force");

				showWaitingView(true, -1, "需要强制版本更新，请下载新版本重新安装");
			}
		} else {
			/*
			 * 等进入GameAppState再处�?
			 */
			mVersionResult = versionInfo;
		}
	}

	@Override
	public void initCocos2dxAndroidContext(View glView, View editText,
			Handler handler) {

		mGameContextStateHandler = handler;
		super.initAndroidContext(glView, editText);
	}

	@Override
	public Handler getMainHandler() {

		return super.getMainThreadHandler();
	}

	/*
	 * 总有�?��特殊处理! 正常情况�?显示平台登录界面�?Cocos2dx被onPause�?
	 * 360的登录框背景透明,挡不住处于不渲染状�?的Cocos2dxGLSurfaceView,�?��特殊处理让其不onPause
	 */
	@Override
	public void callPlatformLogin() {
		super.callPlatformLogin();

		if (mPlatform.getGameInfo().platform_type == PlatformAndGameInfo.enPlatform_RenRen) {
			super.mIsRenderCocos2dxView = false;
		}

	}

	@Override
	public void notifyEnterGameAppState(Handler handler) {
		Log.d(TAG, "notifyEnterGameAppState");

		mHaveEnteredGameAppState = true;
		if (mVersionResult != null) {
			notifyVersionCheckResult(mVersionResult);
			mVersionResult = null;
		}

		super.setGameAppStateHandler(handler);

	}

	@Override
	public void notifyOnTempShortPause() {

		super.setOnTempShortPause(true);
	}

	@Override
	public void notifyLoginResut(LoginInfo result) {
		Log.d(TAG, "notifyLoginResut");

		super.setOnTempShortPause(false);

		/*
		 * 平台账号登录成功�?允许玩家进入游戏
		 */
		Cocos2dxHelper.nativeNotifyPlatformLoginResult(
				PlatformAndGameInfo.enLoginResult_Success,
				String.valueOf(result.account_uid), result.login_session,
				result.account_nick_name);
	}

	@Override
	public void notifyPayRechargeResult(PayInfo result) {
		Log.d(TAG, "notifyPayRechargeResult");

		// super.setOnTempShortPause(false);

		/*
		 * 订单参数正确,result.result�?标示提交成功
		 */
		// 通知进去没用，最多提示下“正在打开平台充值页面”
		Cocos2dxHelper.nativeNotifyPlatformPayResult(result.result,
				result.order_serial, result.product_id, result.product_name,
				result.price, result.orignal_price, result.count,
				result.description);

	}

	@Override
	public void showWaitingViewImp(boolean show, int progress, String text) {
		super.showWaitingView(show, progress, text);
	}

	@Override
	public String getDeviceID() {

		return DeviceUtil.getDeviceUUID(this);
	}

	@Override
	public String getPlatformInfo() {
		if (mPlatform != null) {
			String temp = Build.MANUFACTURER + Build.MODEL;
			temp = temp.replaceAll(" ", "-");
			String ret = temp + "_" + Build.VERSION.SDK_INT;
			// ret = ret + "_" + mPlatform.getGameInfo().platform_type_str;
			return ret;
		}

		return "AndroidPlatform";
	}

	@Override
	public String getClientChannel() {

		if (mPlatform != null) {

			return mPlatform.getGameInfo().platform_type_str;
		}

		return "Android";
	}
	@Override
	public void notifyEnterGame() {
		
	}

	@Override
	public void showToastMsg(String str) {

		Message msg = new Message();
		msg.obj = str;
		msg.what = Cocos2dxHandler.HANDLER_MSG_TO_MAINTHREAD_ShowToastMsg;

		super.getMainThreadHandler().sendMessage(msg);
	}

	@Override
	public void showToastMsgImp(String msg) {
		Toast.makeText(this, msg, Toast.LENGTH_SHORT * 5).show();
	}

	@Override
	public int getPlatformId() {
		// TODO Auto-generated method stub
		if (mPlatform != null) {
			return mPlatform.getGameInfo().platform_type;
		}
		return 0;
	}

	/**
	 * 推送广告通知到安卓的通知消息 pTitle 标题 msg 消息内容 pInstantMinite 延迟分钟
	 * */
	@Override
	public void pushSysNotification(final String pTitle, final String msg,
			final int pInstantMinite) {
		
		final String strTitle = "梦想海贼王";
		
		if (!isWorked()) {
			startService(new Intent(GameActivity.this,
					NotificationService.class));

			Runnable pushRunable = new Runnable() {
				@Override
				public void run() {
					Intent myIntent = new Intent();// 创建Intent对象
					myIntent.setAction("com.youai.dreamonepiece.notificationservice");
					myIntent.putExtra("message", msg);
					myIntent.putExtra("title", strTitle);
					myIntent.putExtra("delayminite", pInstantMinite);
					sendBroadcast(myIntent);// 发送广播
					Log.i("GameActivity", "pushSysNotification");
				}
			};
			getMainHandler().post(pushRunable);
		} else {
			Intent myIntent = new Intent();// 创建Intent对象
			myIntent.setAction("com.youai.dreamonepiece.notificationservice");
			myIntent.putExtra("message", msg);
			myIntent.putExtra("title", strTitle);
			myIntent.putExtra("delayminite", pInstantMinite);
			sendBroadcast(myIntent);// 发送广播
			Log.i("GameActivity", "pushSysNotification");
		}
		/*
		 * Timer timer = new Timer(); timer.schedule(new NotificationTask(msg,
		 * pTitle),new Date());
		 */
	}

	/** service是否已经启动 */
	public boolean isWorked() {
		ActivityManager myManager = (ActivityManager) GameActivity.this
				.getSystemService(Context.ACTIVITY_SERVICE);
		ArrayList<RunningServiceInfo> runningService = (ArrayList<RunningServiceInfo>) myManager
				.getRunningServices(30);
		for (int i = 0; i < runningService.size(); i++) {
			if (runningService.get(i).service.getClassName().toString()
					.equals("com.youai.dreamonepiece.NotificationService")) {
				return true;
			}
		}
		return false;
	}

	/**
	 * 推送广告通知到安卓的通知消息 pAnnounceUrl 显示公告地址
	 * */
	@Override
	public void ShowAnnounce( final String pAnnounceUrl) {
		Runnable dialogRun = new Runnable() {
			@Override
			public void run() {
				GameAnnounceDialog dialog = new GameAnnounceDialog(
						getActivity(), pAnnounceUrl);
				  
			}
		};
		getMainHandler().post(dialogRun);

	}

	@Override
	public void clearSysNotification() {
		Log.i("GameActivity", "clearSysNotification");
		if (!isWorked()) {
			startService(new Intent(GameActivity.this,NotificationService.class));

			Runnable pushRunable = new Runnable() {
				@Override
				public void run() {
					Intent myIntent = new Intent();// 创建Intent对象
					myIntent.setAction("com.youai.dreamonepiece.notificationservice");
					myIntent.putExtra("clear", true);
					sendBroadcast(myIntent);// 发送广播
					Log.i("GameActivity", "clearSysNotification");
				}
			};
			getMainHandler().post(pushRunable);
		} else {
			Intent myIntent = new Intent();// 创建Intent对象
			myIntent.setAction("com.youai.dreamonepiece.notificationservice");
			myIntent.putExtra("clear", true);
			sendBroadcast(myIntent);// 发送广播
			Log.i("GameActivity", "clearSysNotification");
		}
	}
}