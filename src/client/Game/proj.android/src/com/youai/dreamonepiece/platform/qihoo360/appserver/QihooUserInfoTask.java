
package com.youai.dreamonepiece.platform.qihoo360.appserver;

import android.content.Context;
import android.util.Log;

import com.youai.dreamonepiece.platform.qihoo360.Constants;
import com.youai.dreamonepiece.platform.qihoo360.common.SdkHttpListener;
import com.youai.dreamonepiece.platform.qihoo360.common.SdkHttpTask;

/***
 * 此类使用Access Token，请求您的应用服务器*/
public class QihooUserInfoTask {

    private static final String TAG = "QihooUserInfoTask";

    private SdkHttpTask sSdkHttpTask;
    
    public static QihooUserInfoTask newInstance(){
        return new QihooUserInfoTask();
     }

    public void doRequest(Context context, String accessToken, String appKey,
            final QihooUserInfoListener listener) {

        // DEMO使用的应用服务器url仅限DEMO示范使用，禁止正式上线游戏把DEMO应用服务器当做正式应用服务器使用，请使用方自己搭建自己的应用服务器。
       // String url = Constants.DEMO_APP_SERVER_URL_GET_USER + accessToken + "&appkey=" + appKey;
		String url = Constants.DEMO_APP_SERVER_URL_GET_USER + "?access_token=" + accessToken + 
        		"&fields=id,name,avatar,sex,area";

        // 如果存在，取消上一次请求
        if (sSdkHttpTask != null) {
            sSdkHttpTask.cancel(true);
        }
        // 新请求
        sSdkHttpTask = new SdkHttpTask(context);
        sSdkHttpTask.doGet(new SdkHttpListener() {

            @Override
            public void onResponse(String response) {
                Log.d(TAG, "onResponse=" + response);
                QihooUserInfo userInfo = QihooUserInfo.parseJson(response);
                listener.onGotUserInfo(userInfo);
                sSdkHttpTask = null;
            }

            @Override
            public void onCancelled() {
                listener.onGotUserInfo(null);
                sSdkHttpTask = null;
            }

        }, url);
        
        Log.d(TAG, "url=" + url);
    }

    public boolean doCancel() {
        return (sSdkHttpTask != null) ? sSdkHttpTask.cancel(true) : false;
    }

}