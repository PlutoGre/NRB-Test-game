package com.youai.dreamonepiece;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Properties;

import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.youai.IGameActivity;
import com.youai.IGameActivityState;
import com.youai.IStateManager;
import com.youai.StorageUtil;
import com.youai.dreamonepiece.GameInterface.IGameLogoStateCallback;
import com.youai.dreamonepiece.platform.dangle.R;

public class GameLogoState implements IGameActivityState
{
	
	public static final String	TAG	= GameLogoState.class.getSimpleName();
	
	@Override
	public void enter()
	{
		Log.d(TAG, "enter GameLogoState");
		/*
		 * 
		 * */
		mGameActivity.getActivity().setContentView(R.layout.logo_layout);
		
		mAssetsUnzipProgressBar = (ProgressBar) mGameActivity.getActivity()
				.findViewById(R.id.assetsUnzipProgress);
		mAssetsUnzipProgressBar.setVisibility(View.INVISIBLE);
		mAssetsUnzipTextView = (TextView) mGameActivity.getActivity()
				.findViewById(R.id.assets_unzip_textView);
		
		checkNetworkStatus();
		checkStorageStatus();
		
		mHandler = new GameLogoStateHandler();
		
		if (mUnzipedAssets == false
				|| checkExternalStorageResourcesStatus() == false)
		{
			if (mUnzipedAssets == false)
			{
				saveConfigFileFirstTime();
			}
			mAssetsUnzipTextView.setText("解压资源，不需流量");
			requestUnzipAssetToExternalStorageResources();
		}
		else
		{
			makeSureUnzipMusicSoundFiles();
			
			if (mNetworkOK)
			{
				mHandler.sendEmptyMessageDelayed(HANDLER_MSG_TO_MAINTHREAD_ShowLogoViewDelay, 2000);
			}
			else
			{
				DialogMessage dlgmsg = new DialogMessage(
						mGameActivity.getActivity().getResources()
						.getString(R.string.app_dlg_title),
						mGameActivity.getActivity().getResources()
						.getString(R.string.app_dlg_newwork_notok_msg),
						HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK);
				
				showDialog(dlgmsg);
			}
		}
		
	}
	
	@Override
	public void exit()
	{
		mAssetsUnzipProgressBar.setVisibility(View.INVISIBLE);
		mAssetsUnzipTextView.setVisibility(View.INVISIBLE);
		
		mAssetsUnzipProgressBar = null;
		mAssetsUnzipTextView = null;
		
		mStateMgr = null;
		mGameActivity = null;
		mCallback = null;
		
		mHandler.removeMessages(HANDLER_MSG_TO_MAINTHREAD_ShowLogoViewDelay);
		mHandler.removeMessages(HANDLER_MSG_TO_MAINTHREAD_UpdateMoveAssetResProgress);
		mHandler.removeMessages(HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK);
		mHandler = null;
		
		Log.d(TAG, "exit GameLogoState");
	}
	
	/*
	 * 
	 * */
	public GameLogoState(IStateManager pStateMgr, IGameActivity pGameActivity,
			IGameLogoStateCallback pCallback)
	{
		mStateMgr = pStateMgr;
		mGameActivity = pGameActivity;
		mCallback = pCallback;
	}
	
	/*
	 * 
	 * */
	private void checkStorageStatus()
	{
		/*
		 * 
		 * */
		SharedPreferences sp = mGameActivity.getActivity()
				.getSharedPreferences("ResourcesInfo", Context.MODE_PRIVATE);
		String spath = sp.getString("StorageFullPath", "");
		boolean unziped = sp.getBoolean("UnzipedAssets", false);
		if (unziped == false)
		{
			spath = "";
		}
		else
		{
			File dir = new File(spath);
			if (dir.exists() == false)
			{
				spath = "";
				
				Editor edit = sp.edit();
				edit.putBoolean("UnzipedAssets", false);
				edit.apply();
				
				unziped = false;
			}
		}
		
		if (spath.equals(""))
		{
			String appFilesPath = null;
			File appFilesDir = mGameActivity.getActivity().getFilesDir();
			
			if (Environment.getExternalStorageState().equalsIgnoreCase(
					Environment.MEDIA_MOUNTED))
			{
				File externalStorageDir = Environment
						.getExternalStorageDirectory();
				if (!externalStorageDir.canRead()
						|| !externalStorageDir.canWrite()
						|| externalStorageDir.getUsableSpace() < 200 * 1024 * 1024)
				{
					String secondPath = StorageUtil
							.getSecondStorageWithFreeSize(250 * 1024 * 1024);
					if (secondPath != null)
					{
						appFilesPath = secondPath;
					}
					else
					{
						secondPath = StorageUtil
								.getSecondStorageWithFreeSize(externalStorageDir
										.getUsableSpace());
						if (secondPath != null)
						{
							appFilesPath = secondPath;
						}
						else
						{
							appFilesPath = externalStorageDir.getAbsolutePath();
						}
						
						File dir = new File(appFilesPath);
						if (dir.getUsableSpace() < 20 * 1024 * 1024)
						{
							appFilesPath = appFilesDir.getAbsolutePath();
						}
					}
				}
				else
				{
					appFilesPath = externalStorageDir.getAbsolutePath();
				}
			}
			else
			{
				appFilesPath = appFilesDir.getAbsolutePath();
			}
			
			if (appFilesPath.equalsIgnoreCase(appFilesDir.getAbsolutePath()) == false)
			{
				//spath = appFilesPath + "/Data/"
				//		+ mGameActivity.getActivity().getPackageName();
				spath = appFilesPath + "/Android/Data/" +
						mGameActivity.getActivity().getPackageName() + "/files";
			}
			else
			{
				spath = appFilesPath;
			}
			
			Editor edit = sp.edit();
			edit.putString("StorageFullPath", spath);
			edit.apply();
		}
		
		mCallback.initAppDataPath(spath);
		
		mAppFilesPath = spath;
		mUnzipedAssets = unziped;
	}
	
	/*
	 * */
	private void requestUnzipAssetToExternalStorageResources()
	{
		Log.d(TAG, "requestUnzipAssetToExternalStorageResources");
		//
		final String appFilesResourcesPath = mGameActivity.getAppFilesResourcesPath();
		{
			/*
			 * 存储空间不足，维持不解压缩玩
			 */
			File dir = new File(appFilesResourcesPath);
			if (dir.getUsableSpace() < 200 * 1024 * 1024)
			{
				Log.d(TAG, "not enough usable storage space");
				
				makeSureUnzipMusicSoundFiles();
				
				{
					Message msg = new Message();
					msg.obj = new ProgressMessage(100, ", starting game");
					msg.what = HANDLER_MSG_TO_MAINTHREAD_UpdateMoveAssetResProgress;
					mHandler.sendMessageDelayed(msg, 2000);
				}
				
				dir = null;
				return;
			}
			dir = null;
		}
		//
		final Activity theActivity = mGameActivity.getActivity();
		final Handler theHandler = mHandler;// another realize to use
											// HandlerThread
		//final View theProgress = mAssetsUnzipProgressBar;
		//
		new Thread("MoveAssetToExternalStorageThread")
		{
			
			//
			private ArrayList<String>	resFilesPath	= new ArrayList<String>();
			
			//
			private int recursionSumAssetsFileNum(String path)
			{
				int num = 0;
				try
				{
					AssetManager assetMgr = theActivity.getAssets();
					String[] assets = assetMgr.list(path);
					//
					for (String filepath : assets)
					{
						if (filepath.isEmpty())
							continue;
						//
						if (!path.isEmpty())
							filepath = path + "/" + filepath;
						//
						int idx = filepath.lastIndexOf('/');
						int idy = filepath.lastIndexOf('.');
						if (idx == -1 && idy == -1)
						{
							num += recursionSumAssetsFileNum(filepath);
							File temp = new File(appFilesResourcesPath + "/"
									+ filepath);
							if (!temp.exists())
								temp.mkdirs();
							// temp = null;
						}
						else if (idx > 0 && idy > idx)
						{
							num++;
							resFilesPath.add(filepath);
						}
						else if (idx == -1 && idy > 0)
						{
							num++;
							resFilesPath.add(filepath);
						}
						else if (idx > 0 && idy < idx)
						{
							num += recursionSumAssetsFileNum(filepath);
							File temp = new File(appFilesResourcesPath + "/"
									+ filepath);
							if (!temp.exists())
								temp.mkdirs();
							// temp = null;
						}
					}
					//
				}
				catch (IOException e)
				{
					
				}
				finally
				{
					
				}
				return num;
			}
			
			//
			@Override
			public void run()
			{
				//
				long start = System.currentTimeMillis();
				//
				AssetManager assetMgr = theActivity.getAssets();
				int idx = 0;
				BufferedInputStream in = null;
				BufferedOutputStream out = null;
				//
				// try {
				//
				int assetsFileNum = recursionSumAssetsFileNum("");
				//if (assetsFileNum > 20)
				//	theProgress.setVisibility(View.VISIBLE);
				//
				byte[] buf = null;
				if (resFilesPath.size() > 0)
					buf = new byte[10240];
				//
				for (String filepath : resFilesPath)
				{
					idx++;
					//
					try
					{
						in = new BufferedInputStream(assetMgr.open(filepath,
								AssetManager.ACCESS_STREAMING), 40960);
						out = new BufferedOutputStream(new FileOutputStream(
								appFilesResourcesPath + "/" + filepath), 40960);
						//
						int readNum = 0;
						while (true)
						{
							
							readNum = in.read(buf, 0, buf.length);
							if (readNum <= 0)
							{
								break;
							}
							
							out.write(buf, 0, readNum);
							
						}
						//
						out.flush();
					}
					catch (IOException e)
					{
						File tmp = new File(appFilesResourcesPath + "/" + filepath);
						if (tmp.exists())
							tmp.delete();
						tmp = null;
					}
					catch (OutOfMemoryError omm)
					{
						File tmp = new File(appFilesResourcesPath + "/" + filepath);
						if (tmp.exists())
							tmp.delete();
						tmp = null;
					}
					finally
					{
						try
						{
							if (in != null)
								in.close();
							if (out != null)
								out.close();
							//
							// in = null;
							// out = null;
							//
						}
						catch (IOException e)
						{
							
						}
					}
					//
					int itmp = idx % 20;
					if (itmp == 0)
					{
						int progress = idx * 100 / resFilesPath.size();
						String text = String.valueOf(idx) + "/"
								+ String.valueOf(resFilesPath.size());
						
						Message msg = new Message();
						msg.obj = new ProgressMessage(progress, text);
						msg.what = HANDLER_MSG_TO_MAINTHREAD_UpdateMoveAssetResProgress;
						theHandler.sendMessage(msg);
						//
					}
					if (idx >= resFilesPath.size() && itmp > 0)
					{
						
						Message msg = new Message();
						msg.obj = new ProgressMessage(100, ", starting game");
						msg.what = HANDLER_MSG_TO_MAINTHREAD_UpdateMoveAssetResProgress;
						theHandler.sendMessage(msg);
						//
						long end = System.currentTimeMillis();
						long span = end - start;
						Log.d(TAG,
								"UnzipAssetToExternalStorageResources cost time: "
										+ span + " millis");
					}
					//
				}
				//
				buf = null;
				//
				// } catch (IOException e) {
				
				// }
				//
			}
			//
		}.start();
		//
		SharedPreferences sp = mGameActivity.getActivity()
				.getSharedPreferences("ResourcesInfo", Context.MODE_PRIVATE);
		Editor edit = sp.edit();
		edit.putBoolean("UnzipedAssets", true);
		edit.apply();
		
		mUnzipedAssets = true;
		//
	}
	
	/*
	 * 普�?的APK文件是存储压缩率（低压缩率）的zip文件�?
	 * 我们发布时的APK是标准压缩率（高压缩率）的zip文件，此时Cocos2dxMusic&Sound里面的MediaPlayer
	 * &SoundPool都不能使用压缩的声音文件�?故此保证声音文件解压缩，其他的可不解（暂时未发现问题�?
	 */
	private void makeSureUnzipMusicSoundFiles()
	{
		Log.d(TAG, "makeSureUnzipMusicSoundFiles");
		//
		{
			File cfg = new File(mAppFilesPath + "/config.properties");
			if (cfg.exists())
			{
				Properties cfgIni = new Properties();
				String hasUnzipSound = null;
				try
				{
					cfgIni.load(new FileInputStream(cfg));
					hasUnzipSound = cfgIni.getProperty("hasUnzipSound", null);
					//
				}
				catch (FileNotFoundException e)
				{
					
				}
				catch (IOException e)
				{
					
				}
				//
				cfg = null;
				cfgIni = null;
				//
				if (hasUnzipSound != null
						&& hasUnzipSound.equalsIgnoreCase("true"))
					return;
			}
		}
		//
		final Activity theActivity = mGameActivity.getActivity();
		final String appFilesPath = mAppFilesPath;
		final String appFilesResourcesPath = mGameActivity
				.getAppFilesResourcesPath();
		//
		new Thread("MoveSoundFilesToExternalStorageThread")
		{
			
			//
			private ArrayList<String>	soundFilesPath	= new ArrayList<String>();
			
			//
			private int recursionSumAssetsFileNum(String path)
			{
				int num = 0;
				try
				{
					AssetManager assetMgr = theActivity.getAssets();
					String[] assets = assetMgr.list(path);
					//
					for (String filepath : assets)
					{
						if (filepath.isEmpty())
							continue;
						String strTmp = filepath.toUpperCase();
						//
						if (!path.isEmpty())
							filepath = path + "/" + filepath;
						//
						int idx = filepath.lastIndexOf('/');
						int idy = filepath.lastIndexOf('.');
						if (idx == -1 && idy == -1)
						{
							
						}
						else if (idx > 0 && idy > idx)
						{
							if (strTmp.endsWith(".MP3")
									|| strTmp.endsWith(".WAV"))
							{
								num++;
								soundFilesPath.add(filepath);
							}
						}
						else if (idx == -1 && idy > 0)
						{
							if (strTmp.endsWith(".MP3")
									|| strTmp.endsWith(".WAV"))
							{
								num++;
								soundFilesPath.add(filepath);
							}
						}
						else if (idx > 0 && idy < idx)
						{
							
						}
					}
					//
				}
				catch (IOException e)
				{
					
				}
				finally
				{
					
				}
				return num;
			}
			
			//
			@Override
			public void run()
			{
				//
				AssetManager assetMgr = theActivity.getAssets();
				int idx = 0;
				BufferedInputStream in = null;
				BufferedOutputStream out = null;
				//
				// try {
				//
				int assetsFileNum = recursionSumAssetsFileNum("");
				//
				byte[] buf = null;
				if (soundFilesPath.size() > 0)
					buf = new byte[10240];
				//
				for (String filepath : soundFilesPath)
				{
					idx++;
					//
					File file_tmp = new File(appFilesResourcesPath + "/"
							+ filepath);
					if (!file_tmp.exists())
					{
						try
						{
							in = new BufferedInputStream(assetMgr.open(
									filepath, AssetManager.ACCESS_STREAMING),
									40960);
							out = new BufferedOutputStream(
									new FileOutputStream(appFilesResourcesPath
											+ "/" + filepath), 40960);
							//
							int readNum = 0;
							while (true)
							{
								
								readNum = in.read(buf, 0, buf.length);
								if (readNum <= 0)
								{
									break;
								}
								
								out.write(buf, 0, readNum);
								
							}
							//
							out.flush();
						}
						catch (IOException e)
						{
							
						}
						catch (OutOfMemoryError omm)
						{
							
						}
						finally
						{
							try
							{
								if (in != null)
									in.close();
								if (out != null)
									out.close();
								//
								// in = null;
								// out = null;
								//
							}
							catch (IOException e)
							{
								
							}
						}
						//
					}
					file_tmp = null;
				}
				//
				buf = null;
				//
				if (idx >= assetsFileNum)
				{
					File cfg = new File(appFilesPath + "/config.properties");
					if (cfg.exists())
					{
						Properties cfgIni = new Properties();
						try
						{
							cfgIni.load(new FileInputStream(cfg));
							cfgIni.setProperty("hasUnzipSound", "true");
							//
							cfgIni.store(new FileOutputStream(cfg),
									"last upzip sound files");
						}
						catch (FileNotFoundException e)
						{
							
						}
						catch (IOException e)
						{
							
						}
						//
						cfg = null;
						cfgIni = null;
					}
					
					Log.d(TAG, "UnzipedMusicSoundFiles");
				}
				//
				// } catch (IOException e) {
				
				// }
				//
			}
			//
		}.start();
	}
	
	/*
	 * */
	private void checkNetworkStatus()
	{
		
		ConnectivityManager cm = (ConnectivityManager) mGameActivity
				.getActivity().getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo niWiFi = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		NetworkInfo niMobile = cm
				.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
		//
		if ((!niWiFi.isAvailable() && !niMobile.isAvailable())
				|| (!niWiFi.isConnected() && !niMobile.isConnected()))
		{
			/*

		   */
			mNetworkOK = false;
		}
		else
		{
			mNetworkOK = true;
		}
	}
	
	/*
	 * 
	 * */
	/*
	 * 返回true将要求解压缩资源覆盖 安装包版本高返回true ExternalStorageResources版本高返回false
	 */
	private boolean checkExternalStorageResourcesVersion()
	{
		
		File version_cfg = new File(mGameActivity.getAppFilesResourcesPath()
				+ "/version_android.cfg");
		if (!version_cfg.exists())
		{
			version_cfg = null;
			return true;
		}
		
		byte[] buf1 = new byte[2048];// 会自动初始化0
		try
		{
			FileInputStream inp1 = new FileInputStream(version_cfg);
			inp1.read(buf1);// utf8文本
			inp1.close();
			inp1 = null;
		}
		catch (Exception e)
		{
			//e.printStackTrace();
			Log.d(TAG, "local unzip storage version_android.cfg file not exist!");
			return true;
		}
		
		String str1 = new String(buf1);
		String local_version = "";
		
		{
			JSONTokener jsonParser = new JSONTokener(str1);
			try
			{
				JSONObject version = (JSONObject) jsonParser.nextValue();
				local_version = version.getString("localVerson");
			}
			catch (JSONException e)
			{
				//e.printStackTrace();
				Log.d(TAG, "local unzip storage version.cfg file json parse failed!");
				return true;
			}
			jsonParser = null;
		}
		
		str1 = null;
		buf1 = null;
		
		byte[] buf2 = new byte[1024];
		AssetManager assetMgr = mGameActivity.getActivity().getAssets();
		try
		{
			InputStream inp2 = assetMgr.open("version_android.cfg");
			inp2.read(buf2);
			inp2.close();
			inp2 = null;
		}
		catch (IOException e)
		{
			//e.printStackTrace();
			Log.d(TAG, "apk assets version_android.cfg file not exist!");
			return false;
		}
		
		String str2 = new String(buf2);
		String apk_version = "";
		
		{
			JSONTokener jsonParser = new JSONTokener(str2);
			try
			{
				JSONObject version = (JSONObject) jsonParser.nextValue();
				apk_version = version.getString("localVerson");
			}
			catch (JSONException e)
			{
				//e.printStackTrace();
				Log.d(TAG, "apk assets version_android.cfg file parse failed!");
				return false;
			}
			jsonParser = null;
		}
		
		str2 = null;
		buf2 = null;
		String apkVer[] = apk_version.split("\\.");
		String localVer[] = local_version.split("\\.");
		try
		{
			if(Integer.valueOf(apkVer[0])>Integer.valueOf(localVer[0]))
			{
				return true;
			}
			if(Integer.valueOf(apkVer[1])>Integer.valueOf(localVer[1]))
			{
				return true;
			}
			if(Integer.valueOf(apkVer[2])>Integer.valueOf(localVer[2]))
			{
				return true;
			}
		}catch(Exception e)
		{
			Log.d(TAG, "compare apkversion and localversion failed");
			return false;
		}
		//if (apk_version.compareTo(local_version) > 0)
		//	return true;
		
		return false;
	}
	
	private void saveConfigFileFirstTime()
	{
		File cfg = new File(mAppFilesPath + "/config.properties");
		Properties cfgIni = new Properties();
		cfgIni.setProperty("forceUnzipAssets", "false");
		try
		{
			cfgIni.store(new FileOutputStream(cfg),
					"auto save, default false");
		}
		catch (FileNotFoundException e)
		{
			
		}
		catch (IOException e)
		{
			
		}
		//
		cfg = null;
		cfgIni = null;
	}
	
	/*
	 * return false将要求解压缩apk资源覆盖本地资源目录
	 */
	private boolean checkExternalStorageResourcesStatus()
	{
		// �?��config.properties
		// true默认首次运行解压缩Assets资源到外部存�?
		boolean isForceUnzipAssets = true;
		File cfg = new File(mAppFilesPath + "/config.properties");
		if (cfg.exists())
		{
			Properties cfgIni = new Properties();
			String forceUnzip = null;
			try
			{
				cfgIni.load(new FileInputStream(cfg));
				forceUnzip = cfgIni.getProperty("forceUnzipAssets", null);
				//
				if (forceUnzip != null && forceUnzip.equalsIgnoreCase("true"))
				{
					cfgIni.setProperty("forceUnzipAssets", "false");
					cfgIni.store(new FileOutputStream(cfg),
							"auto save, change from true to false");
				}
				//
			}
			catch (FileNotFoundException e)
			{
				
			}
			catch (IOException e)
			{
				
			}
			//
			cfg = null;
			cfgIni = null;
			//
			if (forceUnzip != null && forceUnzip.equalsIgnoreCase("true"))
			{
				isForceUnzipAssets = true;
			}
			else
			{
				isForceUnzipAssets = false;
			}
		}
		else
		{
			Properties cfgIni = new Properties();
			cfgIni.setProperty("forceUnzipAssets", "false");
			try
			{
				cfgIni.store(new FileOutputStream(cfg),
						"auto save, default false");
			}
			catch (FileNotFoundException e)
			{
				
			}
			catch (IOException e)
			{
				
			}
			//
			cfg = null;
			cfgIni = null;
		}
		//
		//
		File resources = new File(mGameActivity.getAppFilesResourcesPath());
		if (!resources.exists() || !resources.canRead()
				|| !resources.canWrite())
		{
			Log.e(TAG, "AppDataExternalStorageResourcesFullPath: "
					+ mGameActivity.getAppFilesResourcesPath() + " is not OK!");
			resources.mkdirs();
			resources = null;
			if (isForceUnzipAssets)
				return false;
			else
				return true;
		}
		//
		// 确保高版本资源能覆盖
		if (isForceUnzipAssets == false /*&& mUnzipedAssets == false*/)
		{
			isForceUnzipAssets = checkExternalStorageResourcesVersion();
		
		}
		//
		AssetManager assetMgr = mGameActivity.getActivity().getAssets();
		//
		try
		{
			
			String[] assets = assetMgr.list("");// "/"apk根目�?""assets根目�?
			String[] files = resources.list();
			//
			resources = null;
			//
			Log.d(TAG, "assets.length: " + String.valueOf(assets.length)
					+ " resources.length: " + String.valueOf(files.length));
			//
			if (files.length < assets.length)
			{
				if (isForceUnzipAssets)
					return false;
				else
					return true;
			}
			// TODO �?��换检查二者不�?��的策�?
			// �?��策略：资源目录放版本号文件resver.txt，版本号+文件MD5(CRC32)列表
			// asset版本大时，返回false，重新解压缩覆盖
			// 版本号相等时，可�?��比较文件个数，或者�?�?��较MD5(CRC32)
			// asset版本小时，返回true，相信游戏内更新了资�?
			// 内更新如果检查资源版本时做校验的�?
		}
		catch (IOException e)
		{
			
			// e.printStackTrace();
			if (isForceUnzipAssets)
				return false;
			else
				return true;
		}
		//
		if (isForceUnzipAssets)
			return false;
		else
			return true;
	}
	
	/*
	 * 
	 * */
	private IStateManager			mStateMgr;
	/*
	 * 
	 * */
	private IGameActivity			mGameActivity;
	/*
	 * 
	 * */
	private IGameLogoStateCallback	mCallback;
	/*
	 * */
	private String					mAppFilesPath											= null;
	/*
	 * */
	private boolean					mUnzipedAssets											= false;
	/*
	 * */
	private boolean					mNetworkOK												= false;
	/*
	 * */
	private ProgressBar				mAssetsUnzipProgressBar									= null;
	private TextView				mAssetsUnzipTextView									= null;
	/*
	 * 
	 * */
	private static final int		HANDLER_MSG_TO_MAINTHREAD_UpdateMoveAssetResProgress	= 0;
	private static final int		HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK					= 1;
	private static final int		HANDLER_MSG_TO_MAINTHREAD_ShowLogoViewDelay				= 3;
	
	/*
	 * 在主线程创建，实际默认就是MainLooper
	 */
	private class GameLogoStateHandler extends Handler
	{
		
		public GameLogoStateHandler()
		{
			super(mGameActivity.getActivity().getMainLooper());
		}
		
		public void handleMessage(Message msg)
		{
			if (msg.what == HANDLER_MSG_TO_MAINTHREAD_UpdateMoveAssetResProgress)
			{
				ProgressMessage obj = (ProgressMessage) msg.obj;
				if (obj.progress < 100)
				{
					Log.d(TAG, "uncompressing resources to external storage "
							+ obj.text);
					
					mAssetsUnzipProgressBar.setVisibility(View.VISIBLE);
					mAssetsUnzipProgressBar.setProgress(obj.progress);
					if (obj.progress < 92)
					{
						String str = mGameActivity.getActivity().getResources()
								.getString(R.string.assets_unzip_msg)
								+ obj.text;
						mAssetsUnzipTextView.setText(str);
					}
					else
					{
						mAssetsUnzipTextView.setText(mGameActivity
								.getActivity().getResources()
								.getString(R.string.starting_game));
					}
				}
				else
				{
					if (mNetworkOK)
					{
						mStateMgr.changeState(GameInterface.YouaiUpdateStateID);
					}
					else
					{
						/*
						Message msg1 = new Message();
						msg1.obj = new DialogMessage(
								mGameActivity.getActivity().getResources()
										.getString(R.string.app_dlg_title),
								mGameActivity
										.getActivity()
										.getResources()
										.getString(
												R.string.app_dlg_newwork_notok_msg),
								HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK);
						msg1.what = HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK;
						
						mHandler.sendMessage(msg1);
						*/
						DialogMessage dlgmsg = new DialogMessage(
								mGameActivity.getActivity().getResources()
								.getString(R.string.app_dlg_title),
								mGameActivity.getActivity().getResources()
								.getString(R.string.app_dlg_newwork_notok_msg),
								HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK);
						
						showDialog(dlgmsg);
					}
				}
			}
			else if (msg.what == HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK)
			{
				//showDialog(msg);
			}
			else if (msg.what == HANDLER_MSG_TO_MAINTHREAD_ShowLogoViewDelay)
			{
				mStateMgr.changeState(GameInterface.YouaiUpdateStateID);
			}
		}
	}
	
	private GameLogoStateHandler	mHandler	= null;
	
	/*
	 * 
	 * */
	private void showDialog(DialogMessage msg)
	{
		final IGameActivity theActivity = mGameActivity;
		final int tag = msg.msgId;
		
		AlertDialog dlg = new AlertDialog.Builder(theActivity.getActivity())
				.setTitle(msg.titile)
				.setMessage(msg.message)
				.setPositiveButton("确定", new DialogInterface.OnClickListener()
				{
					
					@Override
					public void onClick(DialogInterface dialog, int which)
					{
						
						if (tag == HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK)
						{
							theActivity.requestDestroy();
						}
						
					}
				}).setOnCancelListener(new DialogInterface.OnCancelListener()
				{

					@Override
					public void onCancel(DialogInterface dialog)
					{
						if (tag == HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK)
						{
							theActivity.requestDestroy();
						}
					}
			
				})
				/*.setOnDismissListener(new DialogInterface.OnDismissListener()
				{
					// need api level 17
					@Override
					public void onDismiss(DialogInterface dialog)
					{
						if (tag == HANDLER_MSG_TO_MAINTHREAD_NetwrokNotOK)
						{
							theActivity.requestDestroy();
						}
					}
					
				})*/.create();
		WindowManager.LayoutParams lp = dlg.getWindow().getAttributes();
		//lp.alpha = 0.6f;
		dlg.getWindow().setAttributes(lp);
		dlg.show();
	}
	
	/*
	 * 
	 * */
	private static class ProgressMessage
	{
		
		public int		progress;	// 百分之几�?
		public String	text;
		
		public ProgressMessage(int progress, String text)
		{
			this.progress = progress;
			this.text = text;
		}
	}
	
	/*
	 * 
	 * */
	private static class DialogMessage
	{
		
		public String	titile;
		public String	message;
		public int		msgId;
		
		public DialogMessage(String title, String message, int msgId)
		{
			this.titile = title;
			this.message = message;
			this.msgId = msgId;
		}
	}
}
