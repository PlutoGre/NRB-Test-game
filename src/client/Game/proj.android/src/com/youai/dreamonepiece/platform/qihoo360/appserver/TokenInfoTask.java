
package com.youai.dreamonepiece.platform.qihoo360.appserver;

import com.youai.dreamonepiece.platform.qihoo360.Constants;
import com.youai.dreamonepiece.platform.qihoo360.common.SdkHttpListener;
import com.youai.dreamonepiece.platform.qihoo360.common.SdkHttpTask;

import android.content.Context;
import android.util.Log;

/***
 * 此类使用AuthorizationCode，请求您的应用服务器*/
public class TokenInfoTask {

    private static final String TAG = "TokenInfoTask";

    private  SdkHttpTask sSdkHttpTask;
    public static TokenInfoTask newInstance(){
        return new TokenInfoTask();
     }
    public void doRequest(Context context, String authorizationCode,
            String appKey, String appSecret, 
            final TokenInfoListener listener) {

        // DEMO使用的应用服务器url仅限DEMO示范使用，禁止正式上线游戏把DEMO应用服务器当做正式应用服务器使用，请使用方自己搭建自己的应用服务器
        String url = Constants.DEMO_APP_SERVER_URL_GET_TOKEN + "&code=" + authorizationCode 
        		+ "&client_id=" + appKey + "&client_secret=" + appSecret 
        		+ "&redirect_uri=oob";
	//	String url = Constants.DEMO_APP_SERVER_URL_GET_TOKEN + authorizationCode + "&appkey=" + appKey;

        // 如果存在，取消
        if (sSdkHttpTask != null) {
            sSdkHttpTask.cancel(true);
        }
        // 新请
        sSdkHttpTask = new SdkHttpTask(context);
        sSdkHttpTask.doGet(new SdkHttpListener() {

            @Override
            public void onResponse(String response) {
                Log.d(TAG, "onResponse=" + response);
                TokenInfo tokenInfo = TokenInfo.parseJson(response);
                listener.onGotTokenInfo(tokenInfo);
                sSdkHttpTask = null;
            }

            @Override
            public void onCancelled() {
                listener.onGotTokenInfo(null);
                sSdkHttpTask = null;
            }

        }, url);
        
        Log.d(TAG, "doRequest completed, url=" + url);
    }

    public boolean doCancel() {
        return (sSdkHttpTask != null) ? sSdkHttpTask.cancel(true) : false;
    }

}
