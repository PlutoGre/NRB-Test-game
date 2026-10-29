package com.youai.dreamonepiece;

import java.io.File;
import java.text.DecimalFormat;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.DownloadManager;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

public class DownloadApk {

	    static final DecimalFormat DOUBLE_DECIMAL_FORMAT = new DecimalFormat("0.##");
	    public static final int    MB_2_BYTE             = 1024 * 1024;
	    public static final int    KB_2_BYTE             = 1024;

		private  DownloadManager        downloadManager;
	    private  DownloadManagerPro     downloadManagerPro;
	    private  long                   downloadId           = 0l;
	    private  Context                context;
	    private  DownLoadHandler              handler;
	    private  DownloadChangeObserver downloadObserver;
	    private  CompleteReceiver       completeReceiver;
	    private   ProgressDialog mpDialog;
	    private static final String     DOWNLOAD_FOLDER_NAME = "Download";
	    static final String     DOWNLOAD_FILE_NAME   = "DreamOnePiece.apk";
		private  File DownloadPathFile ; 
		private String mUrl;
		
		public DownloadApk(Activity mActivity,String pUrl){
			context = mActivity;
			mUrl = pUrl;
			onCreate();
		}
		
	    public void onDestroy(){
	    	context.getContentResolver().unregisterContentObserver(downloadObserver);
	    	context.unregisterReceiver(completeReceiver);
	    }
	    
	    @SuppressWarnings("deprecation")
		public void onCreate(){
	    	downloadObserver = new DownloadChangeObserver();
	        completeReceiver = new CompleteReceiver();
	        context.getContentResolver().registerContentObserver(DownloadManagerPro.CONTENT_URI, true, downloadObserver);
	        context.registerReceiver(completeReceiver, new IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE));
			
	        handler = new DownLoadHandler();
	        downloadManager = (DownloadManager)context.getSystemService(context.DOWNLOAD_SERVICE);
	        downloadManagerPro = new DownloadManagerPro(downloadManager);
	        
	        //Environment.getExternalStoragePublicDirectory
	        String DownloadPath;
	        
	       /* File externalStorageDir = Environment
					.getExternalStorageDirectory();
			if (!externalStorageDir.canRead()
					|| !externalStorageDir.canWrite()
					|| externalStorageDir.getUsableSpace() < 120 * 1024 * 1024){
				String secondPath = StorageUtil
						.getSecondStorageWithFreeSize(120 * 1024 * 1024);
				if (secondPath != null){
					DownloadPath = secondPath;
				}else{
					secondPath = StorageUtil
							.getSecondStorageWithFreeSize(externalStorageDir
									.getUsableSpace());
					if (secondPath != null){
						DownloadPath = secondPath;
					}else{
						DownloadPath = externalStorageDir.getAbsolutePath();
					}
				}
			}else{*/
				 
	            DownloadPath = Environment.getExternalStorageDirectory().getAbsolutePath();
			//}
		 
	        
			 DownloadPathFile = new File(DownloadPath+File.separator+DOWNLOAD_FOLDER_NAME);
             if (!DownloadPathFile.exists() || !DownloadPathFile.isDirectory()) {
            	 DownloadPathFile.mkdirs();
             }
             
             DownloadPathFile = new File(DownloadPath+File.separator+Environment.DIRECTORY_DOWNLOADS);
             if (!DownloadPathFile.exists() || !DownloadPathFile.isDirectory()) {
            	 DownloadPathFile.mkdirs();
             }
             
             File DOWNLOAD_FILE = new File(DownloadPathFile, DOWNLOAD_FILE_NAME);
             if(DOWNLOAD_FILE.isFile()&DOWNLOAD_FILE.exists()& Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB)
             DOWNLOAD_FILE.delete();
			Log.i("DownloadApk path", DOWNLOAD_FILE.getAbsolutePath());
             
            DownloadManager.Request request = new DownloadManager.Request(Uri.parse(mUrl));
           request.setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, DOWNLOAD_FILE_NAME);
          //  request.setDestinationUri(Uri.fromFile(DOWNLOAD_FILE));
            
            request.setTitle("梦想海贼王");
            request.setDescription("梦想海贼王 ");
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
            	request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED);
            
            request.setVisibleInDownloadsUi(false);
           
            // request.allowScanningByMediaScanner();
            // request.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_WIFI);
            // request.setShowRunningNotification(false);
            // request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_HIDDEN);
            request.setMimeType("application/vnd.android.package-archive");
            downloadId = downloadManager.enqueue(request);
           
	        mpDialog = new ProgressDialog(context);
	        mpDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
	        mpDialog.setTitle("提示");  
            mpDialog.setMessage("正在下载最新软件，需要较长时间，是否切换后台下载?");  
            mpDialog.setIndeterminate(false);  
            mpDialog.setCancelable(false);  
            mpDialog.setButton("确定", new DialogInterface.OnClickListener(){  

                @Override  
                public void onClick(DialogInterface dialog, int which) {  
                  // GameActivity.this.onKeyDown( KeyEvent.KEYCODE_HOME,KeyEvent.);
                    Intent i= new Intent(Intent.ACTION_MAIN);
                    i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK); 
                    i.addCategory(Intent.CATEGORY_HOME);
                    context.startActivity(i);
                }  
                  
            });  
            mpDialog.show();
            
            updateView();
	    }

		public void updateView() {
	        int[] bytesAndStatus = downloadManagerPro.getBytesAndStatus(downloadId);
	        handler.sendMessage(handler.obtainMessage(0, bytesAndStatus[0], bytesAndStatus[1], bytesAndStatus[2]));
	    }
		
		
	public static CharSequence getAppSize(long size) {
        if (size <= 0) {
            return "0M";
        }

        if (size >= MB_2_BYTE) {
            return new StringBuilder(16).append(DOUBLE_DECIMAL_FORMAT.format((double)size / MB_2_BYTE)).append("M");
        } else if (size >= KB_2_BYTE) {
            return new StringBuilder(16).append(DOUBLE_DECIMAL_FORMAT.format((double)size / KB_2_BYTE)).append("K");
        } else {
            return size + "B";
        }
    }
	
	 public static boolean isDownloading(int downloadManagerStatus) {
	        return downloadManagerStatus == DownloadManager.STATUS_RUNNING
	               || downloadManagerStatus == DownloadManager.STATUS_PAUSED
	               || downloadManagerStatus == DownloadManager.STATUS_PENDING;
	    }
	    
	
	 /***/
	    public static boolean install(Context context, File filePath) {
	        Intent i = new Intent(Intent.ACTION_VIEW);
	        if (filePath != null && filePath.length() > 0 && filePath.exists() && filePath.isFile()) {
	            i.setDataAndType(Uri.fromFile(filePath), "application/vnd.android.package-archive");
	            i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
	            context.startActivity(i);
	            return true;
	        }
	        return false;
	    }

	    
	    
	    class DownloadChangeObserver extends ContentObserver {

	        public DownloadChangeObserver(){
	            super(handler);
	        }

	        @Override
	        public void onChange(boolean selfChange) {
	        	updateView();
	        }

	    }


	    class CompleteReceiver extends BroadcastReceiver {

	        @Override
	        public void onReceive(Context context, Intent intent) {
	            /**
	             * get the id of download which have download success, if the id is my id and it's status is successful,
	             * then install it
	             **/
	            long completeDownloadId = intent.getLongExtra(DownloadManager.EXTRA_DOWNLOAD_ID, 0);
	            if (completeDownloadId == downloadId&completeDownloadId!=0) {
	            	mpDialog.dismiss();
	                // if download successful, install apk
	                if (downloadManagerPro.getStatusById(downloadId) == DownloadManager.STATUS_SUCCESSFUL) {
	                	
	                	String path  = downloadManagerPro.getFileName(downloadId);
	                    Log.i("path", path);
	                	install(context, new File(path));
	                }
	            }
	        }
	    };



	    @SuppressLint("HandlerLeak")
		class DownLoadHandler extends Handler {

	        @Override
	        public void handleMessage(Message msg) {
	            super.handleMessage(msg);
	            
	            switch (msg.what) {
	                case 0:
	                    int status = (Integer)msg.obj;
	                  
	                     if (isDownloading(status)) {
	                    	 mpDialog.show();
	                    	Log.i("isDownloading", "isDownloading"+status);
	                        if (msg.arg2 < 0) {
	                        } else {
	                        	int max;
	                        	int progress;
	                        	String maxSize = "";
	                        	String progressSize ="";
	                        	 if (msg.arg2 <= 0) {
	                        		 max = 0;
	                        		 progress = 0;
	                             }
	                             if (msg.arg2 >= MB_2_BYTE) {
	                            	 max = msg.arg2/MB_2_BYTE;
	                        		 progress = msg.arg1/MB_2_BYTE;
	                        		 maxSize = max+"MB";
	                            	 progressSize = progress+"MB";
	                        		 
	                             } else if (msg.arg2 >= KB_2_BYTE) {
	                            	 max = msg.arg2/KB_2_BYTE;
	                        		 progress = msg.arg1/KB_2_BYTE;
	                        		 maxSize = max+"KB";
	                            	 progressSize = progress+"KB";
	                             } else {
	                            	 max = msg.arg2;
	                        		 progress = msg.arg1;
	                        		 maxSize = max+"B";
	                            	 progressSize = progress+"B";
	                             }
	                        	mpDialog.setMax(max);
	                        	mpDialog.setProgress(progress);
	                        	mpDialog.setMessage("正在下载最新版本安装包，需要较长时间，是否切换后台下载?"+"\r\n"+"下载："+progressSize+"/"+maxSize);
	                        }
	                    } else {
	           			 
	                        if (status == DownloadManager.STATUS_FAILED) {
	                        	mpDialog.dismiss();
	                        	downloadManager.remove(downloadId);
	                        	Toast.makeText(context, "存储空间不足  下载失败", Toast.LENGTH_SHORT).show();
	                        	 
	                        } else if (status == DownloadManager.STATUS_SUCCESSFUL) {
	                        	
	                        	Toast.makeText(context, "下载成功", Toast.LENGTH_SHORT).show();
	                        	
	                        }
	                    }
	                    break;
	            }
	        }
	    }
	    
	    
}




