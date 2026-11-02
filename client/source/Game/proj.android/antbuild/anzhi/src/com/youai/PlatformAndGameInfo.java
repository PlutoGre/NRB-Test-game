
/*
 * 		author:	xinzheng at 2013-05-21
 * 		info:	平台的SDK会处理注册、登录、支付的各种普通Error，这里只传递必要信息给到GameApp
 * */


package com.youai;

import android.graphics.Bitmap;





public class PlatformAndGameInfo {
	//////////enum//////////
	public static final String 				enPlatformShort_Youai = "youai_";
	public static final String 				enPlatformShort_Default = "default_";
	public static final String 				enPlatformShort_91 = "91_";
	public static final String 				enPlatformShort_UC = "uc_";
	public static final String 				enPlatformShort_360 = "360_";
	public static final String 				enPlatformShort_DangLe = "dl_";
	public static final String 				enPlatformShort_XiaoMi = "xm_";
	public static final String 				enPlatformShort_WanDouJia = "wdj_";
	public static final String 				enPlatformShort_BaiduDuoKu = "bddk_";
	public static final String 				enPlatformShort_BaiduAppCenter = "bdac_";
	public static final String 				enPlatformShort_AndroidMarket91 = "m91_";
	public static final String 				enPlatformShort_KuWo = "kuwo_";
	public static final String 				enPlatformShort_FeiLiu = "fl_";
	public static final String 				enPlatformShort_JiFeng = "jf_";
	public static final String 				enPlatformShort_AnZhi = "az_";
	public static final String 				enPlatformShort_RenRen = "rr_";
	public static final String 				enPlatformShort_Sina = "sina_";
	public static final String 				enPlatformShort_Game4399 = "4399_";
	public static final String 				enPlatformShort_YingYongHui = "yyh_";
	
	public static final String 				enPlatformShort_ThirdLogin = "thl_";
	public static final String              enPlatformShort_JinShan = "js_";
	public static final String              enPlatformShort_BaiDuGame ="bdgm_";
	public static final String              enPlatformShort_LvDou = "ld_";
	public static final String              enPlatformShort_Oppo ="oppo_";
	public static final String              enPlatformShort_ChuKong ="ck_";
	public static final String              enPlatformShort_GTV ="gtv_";
	public static final String              enPlatformShort_XunLei ="xl_";
	public static final String              enPlatformShort_KuGou = "kg_";
	public static final String              enPlatformShort_HuaWei = "hw_";
	public static final String              enPlatformShort_SouGou ="sg_";

	
	//////////enum//////////
	public static final String 				enPlatformName_Youai = "Android_Youai";
	public static final String 				enPlatformName_Default = "Android_Default";
	public static final String 				enPlatformName_91 = "Android_91";
	public static final String 				enPlatformName_UC = "Android_UC";
	public static final String 				enPlatformName_360 = "Android_360";
	public static final String 				enPlatformName_DangLe = "Android_DangLe";
	public static final String 				enPlatformName_XiaoMi = "Android_XiaoMi";
	public static final String 				enPlatformName_WanDouJia = "Android_WanDouJia";
	public static final String 				enPlatformName_BaiduDuoKu = "Android_BaiduDuoKu";
	public static final String 				enPlatformName_BaiduAppCenter = "Android_BaiduAppCenter";
	public static final String 				enPlatformName_AndroidMarket91 = "Android_AndroidMarket91";
	public static final String 				enPlatformName_KuWo = "Android_KuWo";
	public static final String 				enPlatformName_FeiLiu = "Android_FeiLiu";
	public static final String 				enPlatformName_JiFeng = "Android_JiFeng";
	public static final String 				enPlatformName_AnZhi = "Android_AnZhi";
	public static final String 				enPlatformName_RenRen = "Android_RenRen";
	public static final String 				enPlatformName_Sina = "Android_Sina";
	public static final String 				enPlatformName_Game4399 = "Android_Game4399";
	public static final String 				enPlatformName_YingYongHui = "Android_YingYongHui";
	public static final String 				enPlatformName_ThirdLogin = "Android_ThirdLogin";
	public static final String              enPlatformName_JinShan = "Android_JinShan";
	public static final String              enPlatformName_BaiDuGame = "Android_BaiDuGame";
	public static final String              enPlatformName_LvDouGame = "Android_LvDouGame";
	public static final String              enPlatformName_Oppo = "Android_Oppo";
	public static final String              enPlatformName_ChuKong = "Android_ChuKong";
	public static final String              enPlatformName_GTV = "Android_GTV";
	public static final String              enPlatformName_XunLei = "Android_XunLei";
	public static final String              enPlatformName_KuGou = "Android_KuGou";
	public static final String              enPlatformName_HuaWei = "Android_HuaWei";
	public static final String              enPlatformName_SouGou = "Android_SouGou";
	
	//////////enum//////////
	public static final int 				enPlatform_Youai = 111;
	public static final int 				enPlatform_Default = 0;
	public static final int 				enPlatform_91 = 1;
	public static final int 				enPlatform_UC = 2;
	public static final int 				enPlatform_360 = 3;
	public static final int 				enPlatform_DangLe = 4;
	public static final int 				enPlatform_XiaoMi = 5;
	public static final int 				enPlatform_WanDouJia = 6;
	public static final int 				enPlatform_BaiduDuoKu = 7;
	public static final int 				enPlatform_BaiduAppCenter = 8;
	public static final int 				enPlatform_AndroidMarket91 = 9;
	public static final int 				enPlatform_KuWo = 10;
	public static final int 				enPlatform_FeiLiu = 11;
	public static final int 				enPlatform_JiFeng = 12;
	public static final int 				enPlatform_AnZhi = 13;
	public static final int 				enPlatform_RenRen = 14;
	public static final int 				enPlatform_Sina = 15;
	public static final int 				enPlatform_Game4399 = 16;
	public static final int 				enPlatform_YingYongHui = 17;
	
	public static final int 				enPlatform_Tencent_WeChat = 20;
	public static final int 				enPlatform_Tencent_QQGame = 21;
	public static final int 				enPlatform_ThirdLogin = 22;
	public static final int                 enPlatform_JinShan = 23;
	public static final int                 enPlatform_BaiDuGame = 24;
	public static final int                 enPlatform_LvDouGame = 25;
	public static final int                 enPlatform_Oppo = 26;
	public static final int                 enPlatform_ChuKong = 27;
	public static final int                 enPlatform_XunLei = 28;
	public static final int                 enPlatform_GTV = 29;
	public static final int                 enPlatform_KuGou = 30;
	public static final int                 enPlatform_HuaWei = 31;
	public static final int                 enPlatform_SouGou = 32;
	
	//////////enum//////////
	public static final int 				enScreenOrientation_Portrait = 1;
	public static final int 				enScreenOrientation_Landscape = 2;
	//////////enum//////////
	public static final int 				enDebugMode_Release = 1;
	public static final int 				enDebugMode_Debug = 2;
	//////////enum//////////
	public static final int 				DoNotSupportUpdate = 0;
	public static final int 				SupportUpdateCheck = 1;
	public static final int 				SupportUpdateCheckAndDownload = 2;
	//////////////////////////////////////////////////////////////////////////////////////
	
	public static class GameInfo {
		
		public int 							platform_type;
		public String						platform_type_str;
		public int 							use_platform_sdk_type;
		/*
		 * 考虑不将以下三个值硬编码在java代码中,而通过jni调用从Game.so取
		 * */
		public int 							app_id;
		public String                       app_id_str;
		public int                          gameid;
		public String 						app_key;
		public String						app_secret;
		public int	 						cp_id;	//平台给厂商编的号码
		public int							svr_id;	//支付相关
		public String						pay_addr;
		public String                       pay_id_str;
		public String                       private_str;
		public String                       public_str;
		//
		public int 							screen_orientation;
		public int 							debug_mode;
		
		public GameInfo() {
			
		}
	}
	//
	public static String getPlatformTypeStr(int type)
	{
		String ret = "Android_unkown";
		
		if (type == enPlatform_91)
			ret = enPlatformName_91;
		else if (type == enPlatform_UC)
			ret = enPlatformName_UC;
		else if (type == enPlatform_360)
			ret = enPlatformName_360;
		else if (type == enPlatform_DangLe)
			ret = enPlatformName_DangLe;
		else if (type == enPlatform_XiaoMi)
			ret = enPlatformName_XiaoMi;
		else if (type == enPlatform_WanDouJia)
			ret = enPlatformName_WanDouJia;
		else if (type == enPlatform_BaiduDuoKu)
			ret = enPlatformName_BaiduDuoKu;
		else if (type == enPlatform_BaiduAppCenter)
			ret = enPlatformName_BaiduAppCenter;
		else if (type == enPlatform_AndroidMarket91)
			ret = enPlatformName_AndroidMarket91;
		else if (type == enPlatform_KuWo)
			ret = enPlatformName_KuWo;
		else if (type == enPlatform_FeiLiu)
			ret = enPlatformName_FeiLiu;
		else if (type == enPlatform_JiFeng)
			ret = enPlatformName_JiFeng;
		else if (type == enPlatform_AnZhi)
			ret = enPlatformName_AnZhi;
		else if (type == enPlatform_RenRen)
			ret = enPlatformName_RenRen;
		else if (type == enPlatform_Sina)
			ret = enPlatformName_Sina;
		else if (type == enPlatform_Game4399)
			ret = enPlatformName_Game4399;
		else if (type == enPlatform_YingYongHui)
			ret = enPlatformName_YingYongHui;
		else if (type == enPlatform_Oppo)
			ret = enPlatformName_Oppo;
		else if(type == enPlatform_ThirdLogin)
			ret = enPlatformName_ThirdLogin;
		else if(type == enPlatform_BaiDuGame)
			ret = enPlatformName_BaiDuGame;
		else if(type == enPlatform_LvDouGame)
			ret = enPlatformName_LvDouGame;
		else if(type == enPlatform_ChuKong)
			ret = enPlatformName_ChuKong;
		else if(type == enPlatform_GTV)
			ret = enPlatformName_GTV;
		else if(type == enPlatform_XunLei)
			ret = enPlatformName_XunLei;
		else if(type == enPlatform_KuGou)
			ret = enPlatformName_KuGou;
		else if(type == enPlatform_HuaWei)
			ret = enPlatformName_HuaWei;
		else if(type == enPlatform_SouGou)
			ret = enPlatformName_SouGou;
		else if(type == enPlatform_Youai)
			ret = enPlatformName_Youai;
		return ret;
	}
	//
	public static int readGameInfoPlatformType(final String str, int dfault) {
		
		if (str == null)
			return dfault;
		
		if (str.equals("1"))
			return enPlatform_91;
		else if (str.equals("2"))
			return enPlatform_UC;
		else if (str.equals("20"))
			return enPlatform_Tencent_WeChat;
		else if (str.equals("21"))
			return enPlatform_Tencent_QQGame;
		else if(str.equals("14"))
			return enPlatform_RenRen;
		else if(str.equals("22"))
			return enPlatform_ThirdLogin;
		else if(("enPlatform_WanDouJia").equals(str))
			return enPlatform_WanDouJia;
		
		return dfault;
	}
	//
	public static int readGameInfoUsePlatformSdkType(final String str, int platform_type, int dfault) {
		
		int value = dfault;
		try {
			value = Integer.parseInt(str);
		}catch(NumberFormatException e) {
			
		}
		
		if (platform_type == enPlatform_91) {
			if (value < 0 || value > 1)
				return dfault;
			return value;
		}
		
		return dfault;
	}
	//////////////////////////////////////////////////////////////////////////////////////
	//////////enum//////////
	public static final int 				enLoginResult_Success = 0;
	public static final int 				enLoginResult_Failed = 1;
	
	//////////////////////////////////////////////////////////////////////////////////////
	
	public static class LoginInfo {
		
		public int 							login_result;
		public long 						account_uid;				//腾讯平台QQ号会超过int
		public String						account_uid_str;
		public String 						account_user_name;
		public String 						account_nick_name;
		public String						login_session;
		
		public LoginInfo() {
			login_result = enLoginResult_Failed;
		}
		
	}
	
	//////////enum//////////
	public static final int 				enUpdateInfo_No = 0;
	public static final int 				enUpdateInfo_Suggest = 1;
	public static final int 				enUpdateInfo_Force = 2;
	
	//////////////////////////////////////////////////////////////////////////////////////
	
	public static class VersionInfo {
		
		public int 							update_info;
		public int 							max_version_code;
		public int 							local_version_code;
		public String 						download_url;
		
		public VersionInfo() {
			
		}
		
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	
	public static class PayInfo {
		
		public int 							result;			//异步购买时，仅仅标示请求发出的结果
		public String 						order_serial;
		public String 						product_id;
		public String 						product_name;
		public float 						price;
		public float 						orignal_price;
		public int 							count;
		public String 						description;
		
		public PayInfo() {
			
		}
		
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	
	public static class ShareInfo {
		
		public String 						content;
		public String 						img_path;
		public Bitmap 						bitmap;
		
		public ShareInfo() {
			
		}
		
	}
	
}

