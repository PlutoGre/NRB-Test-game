
package com.youai.dreamonepiece;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;
import java.io.InputStream;

import com.youai.LogUtils;

import org.json.JSONException;
import org.json.JSONObject;

import android.text.TextUtils;

import com.youai.IGameActivity;
import com.youai.PlatformAndGameInfo;
import com.youai.PlatformAndGameInfo.GameInfo;



/** @brief 渠道与本地调试登录配置；外部配置覆盖安装包默认配置。 */
public class GameConfig
{
    private static final String TAG = "GameConfig";
	/*
	 * 
	 * */
	/** 创建当前渠道配置，并应用显式的本地部署选项。 */
	public GameConfig(IGameActivity gameActivity, final int platform_type)
	{
		mGameActivity = gameActivity;
		mGameInfo = new GameInfo();
		
		mGameInfo.platform_type = platform_type;
		mGameInfo.platform_type_str = PlatformAndGameInfo.getPlatformTypeStr(platform_type);
		
		loadGameConfig();
		
		// 本地默认登录不读取渠道密钥，避免继续依赖原渠道配置。
        mGameInfo.platform_type_str = PlatformAndGameInfo.getPlatformTypeStr(mGameInfo.platform_type);
        if (mGameInfo.platform_type != PlatformAndGameInfo.enPlatform_Default) {
            parseGamePlatformInfo(GameConfig.nativeReadGamePlatformInfo(mGameInfo.platform_type));
        }
	}
	/*
	 * not use
	 * */
	public static native int nativeReadGameAppID(int platformType);
	public static native String nativeReadGameAppKey(int platformType);
	public static native String nativeReadGameAppSecret(int platformType);
	/*
	 * using
	 * 返回json格式的字符串，具体解析各个platformType不同
	 * */
	private static native String nativeReadGamePlatformInfo(int platformType);
	/*
	 * 
	 * */
	private void parseGamePlatformInfo(String infoStr)
	{
		if (!TextUtils.isEmpty(infoStr)) {
			
			try {
				
                JSONObject dataJsonObj = new JSONObject(infoStr);
			
				if (mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_91 || 
						mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_XiaoMi || 
						mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_WanDouJia)
				{
	                mGameInfo.app_id = dataJsonObj.getInt("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
				}
				else if (mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_UC || 
							mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_DangLe)
				{
					mGameInfo.app_id = dataJsonObj.getInt("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
	                mGameInfo.cp_id = dataJsonObj.getInt("cpid");
	                mGameInfo.svr_id = dataJsonObj.getInt("svrid");
				}
				else if (mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_BaiduDuoKu || 
						mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_RenRen )
				{
					mGameInfo.app_id = dataJsonObj.getInt("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
	                mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_YingYongHui)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_AnZhi)
				{
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_JinShan)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_Game4399)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_KuWo)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_BaiDuGame)
				{
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_Oppo)
				{
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
					mGameInfo.pay_addr = dataJsonObj.getString("payaddr");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_LvDouGame)
				{
					mGameInfo.app_id = dataJsonObj.getInt("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
	                mGameInfo.pay_addr = dataJsonObj.getString("payaddr");
				}else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_ChuKong)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
	                mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_GTV)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
	                mGameInfo.app_secret = dataJsonObj.getString("appsecret");
	                mGameInfo.svr_id = dataJsonObj.getInt("svrid");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_ChuKong)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
	                mGameInfo.app_key = dataJsonObj.getString("appkey");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_XunLei)
				{
					mGameInfo.app_id = dataJsonObj.getInt("appid");
	                mGameInfo.app_id_str = dataJsonObj.getString("cpid");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_KuGou)
				{
					mGameInfo.app_id = dataJsonObj.getInt("appid");
					mGameInfo.gameid = dataJsonObj.getInt("gameid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_HuaWei)
				{
					mGameInfo.app_id_str = dataJsonObj.getString("appid");
					mGameInfo.pay_id_str = dataJsonObj.getString("payid");
					mGameInfo.private_str = dataJsonObj.getString("private");
					mGameInfo.public_str = dataJsonObj.getString("public");
					mGameInfo.pay_addr = dataJsonObj.getString("payaddr");
				}else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_Youai){
					mGameInfo.app_id = dataJsonObj.getInt("appid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
				}
				else if(mGameInfo.platform_type == PlatformAndGameInfo.enPlatform_SouGou)
				{
					mGameInfo.app_id = dataJsonObj.getInt("appid");
					mGameInfo.app_key = dataJsonObj.getString("appkey");
					mGameInfo.app_secret = dataJsonObj.getString("appsecret");
					mGameInfo.pay_addr = dataJsonObj.getString("payaddr");
				}
			
			} catch (JSONException e) {
                LogUtils.e(TAG, "parseGamePlatformInfo: 渠道配置格式错误", e);
            }
        }
	}
	/*
	 * 
	 * */
	private IGameActivity mGameActivity;
	/*
	 * 
	 * */
	public GameInfo mGameInfo;
    /** 读取包内配置，再应用外部 game.properties 覆盖。 */
    public void loadGameConfig() {
        LogUtils.d(TAG, "loadGameConfig: 读取登录配置");
        Properties settings = new Properties();
        InputStream input = null;
        try {
            input = mGameActivity.getActivity().getAssets().open("game.properties");
            settings.load(input);
        } catch (FileNotFoundException optionalConfigMissing) {
            LogUtils.d(TAG, "loadGameConfig: 包内未配置 game.properties");
        } catch (IOException error) {
            LogUtils.e(TAG, "loadGameConfig: 包内配置读取失败", error);
        } finally {
            closeConfig(input);
        }
        input = null;
        File file = new File(mGameActivity.getAppFilesRootPath(), "game.properties");
        if (file.isFile()) {
            try {
                input = new FileInputStream(file);
                settings.load(input);
            } catch (IOException error) {
                LogUtils.e(TAG, "loadGameConfig: 外部配置读取失败", error);
            } finally {
                closeConfig(input);
            }
        }
        mGameInfo.platform_type = PlatformAndGameInfo.readGameInfoPlatformType(
                settings.getProperty("platformType"), mGameInfo.platform_type);
        if (Boolean.parseBoolean(settings.getProperty("localDebugLogin", "false"))) {
            mGameInfo.platform_type = PlatformAndGameInfo.enPlatform_Default;
            LogUtils.w(TAG, "loadGameConfig: 已显式启用本地调试登录");
        }
        mGameInfo.platform_type_str = PlatformAndGameInfo.getPlatformTypeStr(mGameInfo.platform_type);
        mGameInfo.use_platform_sdk_type = PlatformAndGameInfo.readGameInfoUsePlatformSdkType(
                settings.getProperty("usePlatformSdkType"), mGameInfo.platform_type,
                mGameInfo.use_platform_sdk_type);
        LogUtils.i(TAG, "loadGameConfig: platformType=" + mGameInfo.platform_type);
    }

    private void closeConfig(InputStream input) {
        if (input != null) {
            try {
                input.close();
            } catch (IOException error) {
                LogUtils.e(TAG, "closeConfig: 关闭配置文件失败", error);
            }
        }
    }
}
