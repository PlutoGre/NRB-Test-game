package com.youai.dreamonepiece;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.net.http.SslError;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.SslErrorHandler;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import com.youai.LogUtils;
import com.youai.NetworkUtil;
import com.youai.dreamonepiece.platform.nd91.R;

/**
 * @brief 可关闭、可超时退出的公告窗口。
 * 公告不可用时释放整个窗口，避免透明窗口拦截游戏触摸。
 */
public class GameAnnounceDialog extends Dialog {
    private static final String TAG = "GameAnnounceDialog";
    /** 公告首次加载最长等待时间，单位毫秒。 */
    private static final int LOAD_TIMEOUT_MS = 15000;
    private final Activity mActivity;
    private final String mRequestUrl;
    private final Handler mHandler = new Handler();
    private ProgressDialog mSpinner;
    private WebView mWebView;
    private RelativeLayout mWebRl;
    private boolean mClosed;
    private final Runnable mTimeout = new Runnable() {
        @Override
        public void run() {
            if (!mClosed) {
                LogUtils.w(TAG, "loadTimeout: 公告超时，释放窗口");
                dismiss();
            }
        }
    };

    /** 空地址表示关闭公告，不回退到旧运营地址。 */
    public GameAnnounceDialog(Activity activity, String url) {
        super(activity, android.R.style.Theme_Translucent_NoTitleBar);
        mActivity = activity;
        mRequestUrl = url == null ? "" : url.trim();
        show();
    }

    /** 关闭入口始终保留，加载错误不隐藏视图。 */
    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        setContentView(R.layout.announce_dialog);
        setCancelable(true);
        setCanceledOnTouchOutside(true);
        mWebRl = (RelativeLayout) findViewById(R.id.announce_webview);
        ImageButton button = (ImageButton) findViewById(R.id.announce_btnok);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) { dismiss(); }
        });
        mSpinner = new ProgressDialog(getContext());
        mSpinner.setMessage("加载中...");
        mSpinner.setCancelable(true);
        mSpinner.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) { dismiss(); }
        });
        mWebView = new WebView(getContext());
        mWebView.getSettings().setDomStorageEnabled(true);
        mWebView.getSettings().setJavaScriptEnabled(true);
        mWebView.setWebViewClient(new AnnouncementClient());
        mWebRl.addView(mWebView, new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
    }

    /** 仅显示有效公告；网络与窗口异常不阻断游戏。 */
    @Override
    public void show() {
        LogUtils.d(TAG, "show: 请求显示公告");
        if (mClosed || mActivity.isFinishing() || mRequestUrl.length() == 0 || isShowing()) {
            return;
        }
        if (!(mRequestUrl.startsWith("http://") || mRequestUrl.startsWith("https://"))) {
            LogUtils.w(TAG, "show: 公告地址协议无效");
            return;
        }
        if (!NetworkUtil.detect(mActivity)) {
            LogUtils.w(TAG, "show: 网络不可用，跳过公告");
            return;
        }
        try {
            super.show();
            mSpinner.show();
            mHandler.postDelayed(mTimeout, LOAD_TIMEOUT_MS);
            mWebView.loadUrl(mRequestUrl);
            LogUtils.i(TAG, "show: 公告已显示");
        } catch (RuntimeException error) {
            LogUtils.e(TAG, "show: 初始化公告失败", error);
            dismiss();
        }
    }

    /** 返回键释放公告窗口。 */
    @Override
    public void onBackPressed() { dismiss(); }

    /** 取消和关闭使用相同的释放路径。 */
    @Override
    public void cancel() { dismiss(); }

    /** 幂等释放计时器、加载框和网页，最后关闭整个窗口。 */
    @Override
    public void dismiss() {
        if (mClosed) { return; }
        mClosed = true;
        mHandler.removeCallbacks(mTimeout);
        hideSpinner();
        try {
            if (mWebView != null) {
                mWebView.setWebViewClient(new WebViewClient());
                mWebView.stopLoading();
                mWebRl.removeView(mWebView);
                mWebView.destroy();
                mWebView = null;
            }
        } catch (RuntimeException error) {
            LogUtils.e(TAG, "dismiss: 释放网页失败", error);
        } finally {
            super.dismiss();
            LogUtils.i(TAG, "dismiss: 公告窗口已释放");
        }
    }

    private void hideSpinner() {
        try {
            if (mSpinner != null && mSpinner.isShowing()) { mSpinner.dismiss(); }
        } catch (RuntimeException error) {
            LogUtils.e(TAG, "hideSpinner: 释放加载框失败", error);
        }
    }

    /** @brief 公告加载回调；错误时关闭窗口，不留下透明触摸层。 */
    private class AnnouncementClient extends WebViewClient {
        /** 网络错误时释放公告。 */
        @Override
        public void onReceivedError(WebView view, int code, String description, String url) {
            LogUtils.w(TAG, "onReceivedError: code=" + code);
            dismiss();
        }

        /** 取消证书错误的请求并释放公告。 */
        @Override
        public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
            handler.cancel();
            LogUtils.w(TAG, "onReceivedSslError: 公告证书错误");
            dismiss();
        }

        /** 页面完成后停止等待，保留可见的关闭入口。 */
        @Override
        public void onPageFinished(WebView view, String url) {
            if (!mClosed) {
                mHandler.removeCallbacks(mTimeout);
                hideSpinner();
                LogUtils.i(TAG, "onPageFinished: 公告加载结束");
            }
        }
    }
}

