package com.youai.dreamonepiece.platform.huawei;

import java.util.Map;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import com.huawei.android.hwpay.service.IHuaweiPayService;
import com.huawei.android.hwpay.service.IRemoteServiceCallback;

public class MobileSecurePayHelper {
	static final String TAG = "MobileSecurePayHelper";
	Context mContext = null;
	private Integer lock = 1;
	private IHuaweiPayService huaweiPayService;
	private boolean isPaying = false;

	public MobileSecurePayHelper(Context context) {
		this.mContext = context;
	}

	private ServiceConnection conn = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			// 获得通信通道
			synchronized (lock) {
				huaweiPayService = IHuaweiPayService.Stub.asInterface(service);
				lock.notify();
			}

		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			huaweiPayService = null;
		}
	};

	public boolean startPay(final Map<String, Object> payInfo, final Handler handler, final int what) {

		// 检测安全支付服务是否安装
		HuaweiPayInstall mspHelper = new HuaweiPayInstall(mContext);
		boolean isMobile_spExist = mspHelper.detectMobile_sp();

		if (!isMobile_spExist) 
		{
			return false;
		}
				
//		Log.e(TAG, "enter startPay() payInfo = " + payInfo.toString()
//				+ ", is in paying " + this.isPaying);

		// Return false if it’s paying.
		if (this.isPaying)
			return false;
		this.isPaying = true;

		if (huaweiPayService == null) {
			mContext.bindService(new Intent(IHuaweiPayService.class.getName()),
					conn, Context.BIND_AUTO_CREATE);

		}
		// 实例一个线程来进行支付
		new Thread(new Runnable() {
			public void run() {
				try {
					// 等待安全支付服务绑定操作结束
					// 注意：这里很重要，否则mAlixPay.Pay()方法会失败
					synchronized (lock) {
						if (huaweiPayService == null)
							lock.wait();
					}

					// 调用安全支付服务的pay方法
					String strRet = huaweiPayService.pay(payInfo, mCallback);

//					Log.e(TAG, "After Pay: " + strRet);

					// 将isPaying置为false，表示支付结束
					// 解绑安全支付服务
					isPaying = false;

					mContext.unbindService(conn);

					Message msg = handler.obtainMessage(what);
					msg.what = what;
					msg.obj = strRet;
					handler.sendMessage(msg);
					
				} catch (Exception e) {
					e.printStackTrace();

				}
			}
		}).start();

		return isPaying;
	}

	/**
	 * 实现安全支付的回调
	 */
	private IRemoteServiceCallback mCallback = new IRemoteServiceCallback.Stub() {
		/**
		 *通过IPC机制启动安全支付服务
		 */
		public void startActivity(String packageName, String className,
				int iCallingPid, Bundle bundle) throws RemoteException {
			Intent intent = new Intent(Intent.ACTION_MAIN, null);
			Log.e(TAG, "bundle == null" + (bundle == null));

			if (bundle == null)
				bundle = new Bundle();
		
			try {
				intent.putExtras(bundle);
			} catch (Exception e) {
				e.printStackTrace();
			}

			intent.setClassName(packageName, className);
			mContext.startActivity(intent);
		}
	};
}
