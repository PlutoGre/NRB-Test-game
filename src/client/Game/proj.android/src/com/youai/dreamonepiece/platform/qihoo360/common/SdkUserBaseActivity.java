package com.youai.dreamonepiece.platform.qihoo360.common;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.qihoopay.insdk.activity.ContainerActivity;
import com.qihoopay.insdk.matrix.Matrix;
import com.qihoo.gamecenter.sdk.common.IDispatcherCallback;
import com.qihoo.gamecenter.sdk.protocols.pay.ProtocolConfigs;
import com.qihoo.gamecenter.sdk.protocols.pay.ProtocolKeys;
import com.youai.dreamonepiece.platform.nd91.R;

/**
 * xinzheng 2013-06-23 对我们来说，这个类不用作Activity，实际可以改名叫SdkApiBase
 * SdkUserBaseActivity这个基类，处理请求360SDK的登录和支付接口。
 * 使用方的Activity继承SdkUserBaseActivity，调用doSdkLogin接口发起登录请求；调用doSdkPay接口发起支付请求。
 * 父类通过onGotAuthorizationCode通知子类登录获取的授权码
 * ，子类实现onGotAuthorizationCode接口接收授权码，做后续处理。
 */
public abstract class SdkUserBaseActivity /*extends Activity*/ implements SdkAccountListener {
	public List<String> mPayTypeList = new ArrayList<String>();
	//--begin by xinzheng 2013-06-23
	/*
	 * 本来是Activity，使用this，现在必须先传入context
	 * */
	private Activity mContext;
	public void setActivityContext(Activity context)
	{
		mContext = context;
	}
	//--end
	 // 登录响应模式：CODE模式。
    protected static final String RESPONSE_TYPE_CODE = "code";

    private static final String TAG = "SdkUserBaseActivity";

    // ---------------------------------调用360SDK接口------------------------------------

    /**
     * 使用360SDK的登录接口
     * 
     * @param isLandScape 是否横屏显示登录界面
     * @param isBgTransparent 是否以透明背景显示登录界面
     */
    protected void doSdkLogin(boolean isLandScape, boolean isBgTransparent) {
        Intent intent = getLoginIntent(isLandScape, isBgTransparent);
        Matrix.invokeActivity(mContext, intent, mLoginCallback);
    }

    /**
     * 使用360SDK的切换账号接口
     * 
     * @param isLandScape 是否横屏显示登录界面
     * @param isBgTransparent 是否以透明背景显示登录界面
     */
    protected void doSdkSwitchAccount(boolean isLandScape, boolean isBgTransparent) {
        Intent intent = getSwitchAccountIntent(isLandScape, isBgTransparent);
        Matrix.invokeActivity(mContext, intent, mAccountSwitchCallback);
    }

    /**
     * 使用360SDK的支付接口
     * 
     * @param isLandScape 是否横屏显示支付界面
     * @param isFixed 是否定额支付
     */
    protected void doSdkPay(final boolean isLandScape, final boolean isFixed) {
        QihooPayInfo pay = getQihooPayInfo(isFixed);
        Intent intent = getPayIntent(isLandScape, pay);
        Matrix.invokeActivity(mContext, intent, mPayCallback);
    }

    /**
     * 使用360SDK的退出接口
     * 
     * @param isLandScape 是否横屏显示支付界面
     */
    protected void doSdkQuit(boolean isLandScape) {
        Intent intent = getQuitIntent(isLandScape);
        Matrix.invokeActivity(mContext, intent, mQuitCallback);
    }

    /**
     * 使用360SDK的论坛接口
     * 
     * @param isLandScape 是否横屏显示支付界面
     */
    protected void doSdkBBS(boolean isLandScape) {
        Intent intent = getBBSIntent(isLandScape);
        Matrix.invokeActivity(mContext, intent, null);
    }

    /**
     * 使用360SDK客服中心接口
     * 
     * @param isLandScape 是否横屏显示界面
     */
    protected void doSdkCustomerService(boolean isLandScape) {
        Intent intent = getCustomerServiceIntent(isLandScape);
        Matrix.invokeActivity(mContext, intent, null);
    }

    /**
     * 启动实名注册
     * 
     * @param isLandScape 是否横屏显示登录界面
     * @param isBgTransparent 是否以透明背景显示登录界面
     */
    protected void doSdkRealNameRegister(boolean isLandScape, boolean isBgTransparent,
            String qihooUserId) {
        Intent intent = getRealNameRegisterIntent(isLandScape, isBgTransparent, qihooUserId);
        Matrix.invokeActivity(mContext, intent, mRealNameRegisterCallback);
    }

    /**
     * 绑定手机号
     * 
     * @param isLandScape 是否横屏显示登录界面
     * @param isBgTransparent 是否以透明背景显示登录界面
     */
    protected void doSdkBindPhoneNum(boolean isLandScape) {
        Intent intent = getBindPhoneNumIntent(isLandScape);
        Matrix.invokeActivity(mContext, intent, mBindPhoneNumCallback);
    }

    /**
     * 使用360SDK截屏发帖接口
     * 
     * @param isLandScape 是否横屏显示界面
     */
    protected void doSdkBBSPost(boolean isLandScape) {
        Intent intent = getBBSPostIntent(isLandScape);
        Matrix.invokeActivity(mContext, intent, null);
    }

    /**
     * 悬浮窗设置
     */
    protected void doSdkSettings(boolean isLandScape) {
        Intent intent = getSettingIntent(isLandScape);
        Matrix.execute(mContext, intent,
                new IDispatcherCallback() {

                    @Override
                    public void onFinished(String data) {

                    }
                });
    }

    // -----------------------------------参数Intent-------------------------------------

    /***
     * 生成调用360SDK登录接口的Intent
     * 
     * @param isLandScape 是否横屏
     * @param isBgTransparent 是否背景透明
     * @param appKey 应用或游戏的AppKey
     * @param appChannel 应用或游戏的自定义子渠道
     * @return Intent
     */
    private Intent getLoginIntent(boolean isLandScape, boolean isBgTransparent) {

        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);

        // 界面相关参数，360SDK登录界面背景是否透明。
        bundle.putBoolean(ProtocolKeys.IS_LOGIN_BG_TRANSPARENT, isBgTransparent);

        // *** 以下非界面相关参数 ***

        // 必需参数，登录回应模式：CODE模式，即返回Authorization Code的模式。
        bundle.putString(ProtocolKeys.RESPONSE_TYPE, RESPONSE_TYPE_CODE);

        // 必需参数，使用360SDK的登录模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_LOGIN);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成调用360SDK切换账号接口的Intent
     * 
     * @param isLandScape 是否横屏
     * @param isBgTransparent 是否背景透明
     * @return Intent
     */
    private Intent getSwitchAccountIntent(boolean isLandScape, boolean isBgTransparent) {

        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);

        // 界面相关参数，360SDK登录界面背景是否透明。
        bundle.putBoolean(ProtocolKeys.IS_LOGIN_BG_TRANSPARENT, isBgTransparent);

        // *** 以下非界面相关参数 ***

        // 必需参数，登录回应模式：CODE模式，即返回Authorization Code的模式。
        bundle.putString(ProtocolKeys.RESPONSE_TYPE, RESPONSE_TYPE_CODE);

        // 必需参数，使用360SDK的切换账号模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_SWITCH_ACCOUNT);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成调用360SDK支付接口的Intent
     * 
     * @param isLandScape
     * @param pay
     * @return Intent
     */
    protected Intent getPayIntent(boolean isLandScape, QihooPayInfo pay) {
        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);

        // *** 以下非界面相关参数 ***

        // 设置QihooPay中的参数。
        // 必需参数，用户access token，要使用注意过期和刷新问题，最大64字符。
        bundle.putString(ProtocolKeys.ACCESS_TOKEN, pay.getAccessToken());

        // 必需参数，360账号id，整数。
        bundle.putString(ProtocolKeys.QIHOO_USER_ID, pay.getQihooUserId());

        // 必需参数，所购买商品金额, 以分为单位。金额大于等于100分，360SDK运行定额支付流程； 金额数为0，360SDK运行不定额支付流程。
        bundle.putString(ProtocolKeys.AMOUNT, pay.getMoneyAmount());

        // 必需参数，人民币与游戏充值币的默认比例，例如2，代表1元人民币可以兑换2个游戏币，整数。
        bundle.putString(ProtocolKeys.RATE, pay.getExchangeRate());

        // 必需参数，所购买商品名称，应用指定，建议中文，最大10个中文字。
        bundle.putString(ProtocolKeys.PRODUCT_NAME, pay.getProductName());

        // 必需参数，购买商品的商品id，应用指定，最大16字符。
        bundle.putString(ProtocolKeys.PRODUCT_ID, pay.getProductId());

        // 必需参数，应用方提供的支付结果通知uri，最大255字符。360服务器将把支付接口回调给该uri，具体协议请查看文档中，支付结果通知接口–应用服务器提供接口。
        bundle.putString(ProtocolKeys.NOTIFY_URI, pay.getNotifyUri());

        // 必需参数，游戏或应用名称，最大16中文字。
        bundle.putString(ProtocolKeys.APP_NAME, pay.getAppName());

        // 必需参数，应用内的用户名，如游戏角色名。 若应用内绑定360账号和应用账号，则可用360用户名，最大16中文字。（充值不分区服，
        // 充到统一的用户账户，各区服角色均可使用）。
        bundle.putString(ProtocolKeys.APP_USER_NAME, pay.getAppUserName());

        // 必需参数，应用内的用户id。
        // 若应用内绑定360账号和应用账号，充值不分区服，充到统一的用户账户，各区服角色均可使用，则可用360用户ID最大32字符。
        bundle.putString(ProtocolKeys.APP_USER_ID, pay.getAppUserId());

        // 可选参数，应用扩展信息1，原样返回，最大255字符。
        bundle.putString(ProtocolKeys.APP_EXT_1, pay.getAppExt1());

        // 可选参数，应用扩展信息2，原样返回，最大255字符。
        bundle.putString(ProtocolKeys.APP_EXT_2, pay.getAppExt2());

        // 可选参数，应用订单号，应用内必须唯一，最大32字符。
        bundle.putString(ProtocolKeys.APP_ORDER_ID, pay.getAppOrderId());

        // 必需参数，使用360SDK的支付模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_PAY);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /**
     * 钩子方法，留给使用支付的子类实现getQihooPayInfo
     * 
     * @param isFixed
     * @return
     */
    protected QihooPayInfo getQihooPayInfo(boolean isFixed) {
        return null;
    }

    /***
     * 生成调用360SDK退出接口的Intent
     * 
     * @param isLandScape
     * @return Intent
     */
    private Intent getQuitIntent(boolean isLandScape) {
        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);
        // 必需参数，使用360SDK的退出模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_QUIT);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成实名注册登录接口的Intent
     * 
     * @param isLandScape 是否横屏
     * @param isBgTransparent 是否背景透明
     * @param qihooUserId 奇虎UserId
     * @return Intent
     */
    private Intent getRealNameRegisterIntent(boolean isLandScape, boolean isBgTransparent,
            String qihooUserId) {

        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);

        // 背景是否透明
        bundle.putBoolean(ProtocolKeys.IS_LOGIN_BG_TRANSPARENT, isBgTransparent);

        // 必需参数，360账号id，整数。
        bundle.putString(ProtocolKeys.QIHOO_USER_ID, qihooUserId);

        // 必需参数，使用360SDK的实名注册模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_REAL_NAME_REGISTER);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成绑定手机号接口的Intent
     * 
     * @param isLandScape 是否横屏
     * @param isBgTransparent 是否背景透明
     * @return Intent
     */
    private Intent getBindPhoneNumIntent(boolean isLandScape) {
        // Bundle bundle = new Bundle();
        //
        // // 界面相关参数，360SDK界面是否以横屏显示。
        // bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE,
        // isLandScape);
        //
        //
        // // 必需参数，使用360SDK的绑定手机号模块。
        // bundle.putInt(ProtocolKeys.FUNCTION_CODE,
        // ProtocolConfigs.FUNC_CODE_BIND_PHONE_NUM);
        //
        // Intent intent = new Intent(this, ContainerActivity.class);
        // intent.putExtras(bundle);

        Bundle bundle = new Bundle();
        bundle.putInt(ProtocolKeys.FUNCTION_CODE,
                ProtocolConfigs.FUNC_CODE_BIND_PHONE_NUM);
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE,
                isLandScape);
        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成设置接口的Intent
     * 
     * @return Intent
     */
    private Intent getSettingIntent(boolean isLandScape) {
        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);

        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_SETTINGS);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /**
     * 生成截屏发帖的Intent
     * 
     * @param isLandScape
     * @return
     */
    private Intent getBBSPostIntent(boolean isLandScape) {
        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);

        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_BBS_POSTS);

        // 此处可以传入您的截屏路径
        bundle.putString(ProtocolKeys.BBS_POST_EXTRA_SNAP_PATH,
                Environment.getExternalStorageDirectory() + "/DCIM/screenshot/20130621152522.png");

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    // ---------------------------------360SDK接口的回调-----------------------------------

    // 登录、注册的回调
    private IDispatcherCallback mLoginCallback = new IDispatcherCallback() {

        @Override
        public void onFinished(String data) {
            Log.d(TAG, "mLoginCallback, data is " + data);
            String authorizationCode = parseAuthorizationCode(data);
            onGotAuthorizationCode(authorizationCode);
        }
    };

    // 切换账号的回调
    private IDispatcherCallback mAccountSwitchCallback = new IDispatcherCallback() {

        @Override
        public void onFinished(String data) {
            Log.d(TAG, "mAccountSwitchCallback, data is " + data);
            String authorizationCode = parseAuthorizationCode(data);
            onGotAuthorizationCode(authorizationCode);
        }
    };

    /**
     * 从Json字符中获取授权码
     * 
     * @param data Json字符串
     * @return 授权码
     */
    private String parseAuthorizationCode(String data) {
        String authorizationCode = null;
        if (!TextUtils.isEmpty(data)) {
            boolean isCallbackParseOk = false;
            try {
                JSONObject json = new JSONObject(data);
                int errCode = json.getInt("errno");
                if (errCode == 0) {
                    // 只支持code登陆模式
                    JSONObject content = json.getJSONObject("data");
                    if (content != null) {
                        // 360SDK登录返回的Authorization Code（授权码，60秒有效）。
                        authorizationCode = content.getString("code");
                        isCallbackParseOk = true;
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }

            // 用于测试数据格式是否异常。
            if (!isCallbackParseOk) {
                Toast.makeText(mContext, mContext.getString(R.string.data_format_error),
                        Toast.LENGTH_LONG).show();
            }
        }
        Log.d(TAG, "parseAuthorizationCode=" + authorizationCode);
        return authorizationCode;
    }

    // 支付的回调
    protected IDispatcherCallback mPayCallback = new IDispatcherCallback() {

        @Override
        public void onFinished(String data) {
            Log.d(TAG, "mPayCallback, data is " + data);
            boolean isCallbackParseOk = false;
            JSONObject jsonRes;
            try {
                jsonRes = new JSONObject(data);
                // error_code 状态码： 0 支付成功， -1 支付取消， 1 支付失败， -2 支付进行中。
                // error_msg 状态描述
                int errorCode = jsonRes.getInt("error_code");
                switch (errorCode) {
                    case 0:
                    case 1:
                    case -1:
                    case -2: {
                        String errorMsg = jsonRes.getString("error_msg");
                        String text = mContext.getString(R.string.pay_callback_toast, errorCode, errorMsg);
                        Toast.makeText(mContext, text, Toast.LENGTH_SHORT).show();
                        onGotPayCallback(errorCode, errorMsg);
                        isCallbackParseOk = true;
                    }
                        break;
                    default:
                        break;
                }
                
            } catch (JSONException e) {
                e.printStackTrace();
            }

            // 用于测试数据格式是否异常。
            if (!isCallbackParseOk) {
                Toast.makeText(mContext, mContext.getString(R.string.data_format_error),
                        Toast.LENGTH_LONG).show();
            }
        }
    };

    // 实名注册的回调
    private IDispatcherCallback mRealNameRegisterCallback = new IDispatcherCallback() {

        @Override
        public void onFinished(String data) {
            Log.d(TAG, "mRealNameRegisterCallback, data is " + data);
        }
    };

    private Handler mHandler = new Handler();

    // 退出的回调
    private IDispatcherCallback mQuitCallback = new IDispatcherCallback() {

        @Override
        public void onFinished(String data) {
            Log.d(TAG, "mQuitCallback, data is " + data);
            JSONObject json;
            try {
                json = new JSONObject(data);
                int which = json.optInt("which", -1);
                String label = json.optString("label");
                
                Toast.makeText(mContext, "按钮标识：" + which + "，按钮描述:" + label, Toast.LENGTH_LONG).show();
                
                if(which == 0) return;
                
                mHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (mContext.isFinishing()) {
                            return;
                        }
                       mContext. finish();
                    }
                }, 200);
                
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    };

    private IDispatcherCallback mBindPhoneNumCallback = new IDispatcherCallback() {

        @Override
        public void onFinished(String data) {
            Log.d(TAG, "mBindPhoneNumCallback, data is " + data);
        }
    };

    // -----------------------------------------防沉迷查询接口----------------------------------------

    /**
     * 本方法中的callback实现仅用于测试, 实际使用由游戏开发者自己处理
     * 
     * @param qihooUserId
     * @param accessToken
     */
    protected void doSdkAntiAddictionQuery(String qihooUserId, String accessToken) {
        Intent intent = getAntiAddictionIntent(qihooUserId, accessToken);
        Matrix.execute(mContext, intent, new IDispatcherCallback() {

            @Override
            public void onFinished(String data) {
                Log.d("demo,anti-addiction query result = ", data);
                if (!TextUtils.isEmpty(data)) {
                    try {
                        JSONObject resultJson = new JSONObject(data);
                        int errorCode = resultJson.getInt("error_code");
                        if (errorCode == 0) {
                            JSONObject contentData = resultJson.getJSONObject("content");
                            // 保存登录成功的用户名及密码
                            JSONArray retData = contentData.getJSONArray("ret");
                            Log.d(TAG, "ret data = " + retData);
                            int status = retData.getJSONObject(0).getInt("status");
                            Log.d(TAG, "status = " + status);
                            if (status == 0) {
                                Toast.makeText(mContext,
                                        mContext.getString(R.string.anti_addiction_query_result_0),
                                        Toast.LENGTH_LONG).show();
                            } else if (status == 1) {
                                Toast.makeText(mContext,
                                        mContext.getString(R.string.anti_addiction_query_result_1),
                                        Toast.LENGTH_LONG).show();
                            } else if (status == 2) {
                                Toast.makeText(mContext,
                                        mContext.getString(R.string.anti_addiction_query_result_2),
                                        Toast.LENGTH_LONG).show();
                            }
                        } else {
                            Toast.makeText(mContext,
                                    resultJson.getString("error_msg"), Toast.LENGTH_SHORT).show();
                        }

                    } catch (JSONException e) {
                        Toast.makeText(mContext,
                                mContext.getString(R.string.anti_addiction_query_exception),
                                Toast.LENGTH_LONG).show();
                        e.printStackTrace();

                    }
                }
            }
        });
    }

    /**
     * 生成防沉迷查询接口的Intent参数
     * 
     * @param qihooUserId
     * @param accessToken
     * @return Intent
     */
    private Intent getAntiAddictionIntent(String qihooUserId, String accessToken) {

        Bundle bundle = new Bundle();
        // 必需参数，用户access token，要使用注意过期和刷新问题，最大64字符。
        bundle.putString(ProtocolKeys.ACCESS_TOKEN, accessToken);
        // 必需参数，360账号id，整数。
        bundle.putString(ProtocolKeys.QIHOO_USER_ID, qihooUserId);
        // 必需参数，使用360SDK的防沉迷查询模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_ANTI_ADDICTION_QUERY);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /**
     * 调用短信支付，调用方需要设置相关参数，而sdk会帮助下单、发短信，并将短信发送的结果返回 注意：短信的发送是异步的
     */
    protected void doSdkSmsPay() {
        Intent intent = getSmsPayIntent();
        Matrix.execute(mContext, intent, null);
    }

    /**
     * 短信支付的相关参数，只需在原有的支付参数上面添加三个短信支付的参数即可
     * 
     * @return
     */
    private Intent getSmsPayIntent() {
        QihooPayInfo pay = getQihooPayInfo(true);
        Bundle bundle = new Bundle();

        // *** 以下非界面相关参数 ***

        // 设置QihooPay中的参数。
        // 必需参数，用户access token，要使用注意过期和刷新问题，最大64字符。
        bundle.putString(ProtocolKeys.ACCESS_TOKEN, pay.getAccessToken());

        // 必需参数，360账号id，整数。
        bundle.putString(ProtocolKeys.QIHOO_USER_ID, pay.getQihooUserId());

        // 必需参数，人民币与游戏充值币的默认比例，例如2，代表1元人民币可以兑换2个游戏币，整数。
        bundle.putString(ProtocolKeys.RATE, pay.getExchangeRate());

        // 必需参数，所购买商品名称，应用指定，建议中文，最大10个中文字。
        bundle.putString(ProtocolKeys.PRODUCT_NAME, pay.getProductName());

        // 必需参数，购买商品的商品id，应用指定，最大16字符。
        bundle.putString(ProtocolKeys.PRODUCT_ID, pay.getProductId());

        // 必需参数，应用方提供的支付结果通知uri，最大255字符。360服务器将把支付接口回调给该uri，具体协议请查看文档中，支付结果通知接口–应用服务器提供接口。
        bundle.putString(ProtocolKeys.NOTIFY_URI, pay.getNotifyUri());

        // 必需参数，游戏或应用名称，最大16中文字。
        bundle.putString(ProtocolKeys.APP_NAME, pay.getAppName());

        // 必需参数，应用内的用户名，如游戏角色名。 若应用内绑定360账号和应用账号，则可用360用户名，最大16中文字。（充值不分区服，
        // 充到统一的用户账户，各区服角色均可使用）。
        bundle.putString(ProtocolKeys.APP_USER_NAME, pay.getAppUserName());

        // 必需参数，应用内的用户id。
        // 若应用内绑定360账号和应用账号，充值不分区服，充到统一的用户账户，各区服角色均可使用，则可用360用户ID最大32字符。
        bundle.putString(ProtocolKeys.APP_USER_ID, pay.getAppUserId());

        // 必需参数，所购买商品金额, 以分为单位。此处为短信付费代码代表的金额,例如2元，此处传入200即可。
        // 注意，此处参数一定要填写正确，否则可能出现下单与实际移动扣费不一致的情况
        bundle.putString(ProtocolKeys.AMOUNT, "200");

        // 短信支付则必须设置以下3个参数，它们由游戏方自己从移动那边申请，以下
        bundle.putString(ProtocolKeys.SMS_PAY_CP_ID, "C00111");

        bundle.putString(ProtocolKeys.SMS_PAY_CP_SERVICE_ID, "120123002000");

        bundle.putString(ProtocolKeys.SMS_PAY_CONSUMER_CODE, "120123002001");

        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_SMS_PAY);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成调用360SDK论坛接口的Intent
     * 
     * @param isLandScape
     * @return Intent
     */
    private Intent getBBSIntent(boolean isLandScape) {

        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);
        // 必需参数，使用360SDK的论坛模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_BBS);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    /***
     * 生成调用360SDK客服接口的Intent
     * 
     * @param isLandScape
     * @return Intent
     */
    private Intent getCustomerServiceIntent(boolean isLandScape) {

        Bundle bundle = new Bundle();

        // 界面相关参数，360SDK界面是否以横屏显示。
        bundle.putBoolean(ProtocolKeys.IS_SCREEN_ORIENTATION_LANDSCAPE, isLandScape);
        // 必需参数，使用360SDK的论坛模块。
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_CUSTOMER_SERVICE);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    // -----------------------------------------自检查询接口----------------------------------------

    /**
     * 本方法中的callback实现仅用于测试, 实际使用由游戏开发者自己处理
     */
    protected void doSdkSelfCheck() {
        Intent intent = getSelfCheckIntent();
        Matrix.execute(mContext, intent, new IDispatcherCallback() {

            @Override
            public void onFinished(String data) {
                Log.d("demo, self check result = ", data);
                try {
                    JSONObject result = new JSONObject(data);
                    Toast.makeText(mContext, result.optString("error_msg"),
                            Toast.LENGTH_LONG).show();

                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * 生成360SDK自检的Intent参数
     * 
     * @return Intent
     */
    private Intent getSelfCheckIntent() {
        Bundle bundle = new Bundle();
        // 必需参数，使用360SDK的自检模块
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_SELF_CHECK);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }

    // -----------------------------------------pro包信息查询接口----------------------------------------

    /**
     * 本方法中的callback实现仅用于测试, 实际使用由游戏开发者自己处理
     */
    protected void doSdkProInfoQuery() {
        Intent intent = getProInfoQueryIntent();
        Matrix.execute(mContext, intent, new IDispatcherCallback() {

            @Override
            public void onFinished(String data) {
                Log.d("demo, self check result = ", data);
                try {
                    JSONObject result = new JSONObject(data);
                    int errCode = result.optInt("error_code");
                    if (errCode == 0) {
                        JSONObject content = result.optJSONObject("content");
                        String verName = null;
                        int verCode = 0;
                        if (content != null) {
                            verName = content.optString("ver_name");
                            verCode = content.optInt("ver_code");
                        }
                        Toast.makeText(mContext,
                                "version name: " + verName + ", version code: " + verCode,
                                Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(mContext, result.optString("error_msg"),
                                Toast.LENGTH_LONG).show();
                    }

                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * 使用360SDK的支付接口
     * 
     * @param isLandScape 是否横屏显示支付界面
     * @param isFixed 是否定额支付
     */
    protected void toSdkPay(boolean isLandScape, boolean isFixed) {
   
        QihooPayInfo pay = getQihooPayInfo(isFixed);
        Intent intent = getPayIntent(isLandScape, pay);
        
        Bundle bundle = new Bundle();
        // 可选参数，定制支付类型
        int iLen = mPayTypeList.size();
        if (iLen > 0) {
            String[] payType = (String[]) mPayTypeList.toArray(new String[iLen]);
            bundle.putStringArray(ProtocolKeys.PAY_TYPE, payType);
        }

        // 可选参数，默认支付类型
        bundle.putString(ProtocolKeys.DEFAULT_PAY_TYPE, ProtocolKeys.PayType.ALIPAY);
        intent.putExtras(bundle);
        
        Matrix.invokeActivity(mContext, intent, mPayCallback);
        
        
        
        
    }
  /*   * 生成SDK查询的Intent参数
     * 
     * @return Intent
     */
    private Intent getProInfoQueryIntent() {
        Bundle bundle = new Bundle();
        // 必需参数，使用360SDK的pro包查询模块
        bundle.putInt(ProtocolKeys.FUNCTION_CODE, ProtocolConfigs.FUNC_CODE_OUT_SDK_INFO);

        Intent intent = new Intent(mContext, ContainerActivity.class);
        intent.putExtras(bundle);

        return intent;
    }
    protected void onGotPayCallback(int err, String msg)
    {
    	
    }

}
