/*
 *
 *
 *
 *
 * */



#include <jni.h>
#include <android/log.h>
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"

//
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"

//
#include "AppDelegate.h"

//
#define  LOG_TAG    "GameMain"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//

using namespace cocos2d;

//
/*

		xinzheng 2013-06-20
		应用程序启动、更新检测、更新流程：
		apk和ipa程序入口相当于一个loader程序，应在loader拉起Game.so（AppDelegate被创建）的时候，准备好二者的衔接；
		而且只是State(LoadingFrame)这个状态需要与loader通信；
		以下顺序执行，为了减少麻烦，不并行：
		0、apk入口，显示公司&游戏logo、动画，这个过程不宜过长；检测网络、存储条件；
		0a、隔离平台的版本更新检测和实现；因为有些平台强制在初始化使用其版本更新功能；
		1、初始化平台SDK，显示平台logo，如果平台不需显示logo，可以延长0步的显示界面；如果不接入平台SDK，直接跳到第2步；
		2、版本更新检测与下载安装，a、自有版本更新实现，需要区别是否使用平台提供的更新检测；b、调用平台SDK提供的更新实现；更新检测结果为可选或无时，进入第3步；
		3、初始化OpenGL ES Context、纹理、音频、输入等与系统相关的必须组件；这个过程会黑屏，需要在loader层保持最上层logo界面或者另外的logo、动画界面；
		4、初始化Game.so，创建AppDelegate，指定集成的登录&支付平台，准备好与loader的衔接，进入State(LoadingFrame)，通知可以发起内更新检测；移除loader层多余的界面，真正开始显示Game.so渲染的界面；
		5、内更新检测返回无需更新，发起调用平台账号登录；或者，内更新进行更新，成功完毕后，发起调用平台账号登录；
		6、停留在State(LoadingFrame)，直到平台账号登录成功通知Game.so，解除与loader的衔接，允许进入游戏State(MainFrame）；
		7、目前进入State(MainFrame）后不允许再切换账号返回State(LoadingFrame)，只得重启；

*/
AppDelegate* g_pAppDelegate = NULL;
//
extern "C"
{

	/*
		java端：static {System.load(Game.so)}
	*/
	jint JNI_OnLoad(JavaVM *vm, void *reserved)
	{
		JniHelper::setJavaVM(vm);

		LOGD("1		JNI_OnLoad");

		return JNI_VERSION_1_6;
	}
	/*
		4、初始化Game.so，创建AppDelegate，准备好与loader的衔接，进入State(LoadingFrame)，通知可以发起内更新检测
	*/
	void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
	{
		LOGD("Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit");

		/*
		 * 按照GameContextState里使用GLSurfaceView的方法，有时需要下面if条件，有时不需要，
		 * 统一改为不需要
		 * */
		//if (!CCDirector::sharedDirector()->getOpenGLView())
		CCDirector::sharedDirector()->getOpenGLView();
		{
			CCEGLView* view = CCEGLView::sharedOpenGLView();
			view->setFrameSize(w, h);

			g_pAppDelegate = new AppDelegate();
			CCApplication::sharedApplication()->run();
		}

	}
	/*
		
	*/
	void Java_org_cocos2dx_lib_Cocos2dxActivity_nativeOnDestroy(JNIEnv*  env, jobject thiz)
	{
		//test
		CCApplication::sharedApplication()->applicationWillGoToExit();
		//
		delete g_pAppDelegate;
	}

	/*
	 *
	 * */
	JNIEXPORT jint JNICALL Java_com_youai_dreamonepiece_GameConfig_nativeReadGameAppID(JNIEnv*  env, jobject thiz,
			jint platform_type)
	{



		return 0;
	}
	/*
	 *
	 * */
	JNIEXPORT jstring JNICALL Java_com_youai_dreamonepiece_GameConfig_nativeReadGameAppKey(JNIEnv*  env, jobject thiz,
			jint platform_type)
	{



		return 0;
	}
	/*
	 *
	 * */
	JNIEXPORT jstring JNICALL Java_com_youai_dreamonepiece_GameConfig_nativeReadGameAppSecret(JNIEnv*  env, jobject thiz,
			jint platform_type)
	{

		if (platform_type == 3)
		{
			//enPlatform_360
			return JniHelper::string2jstring("36e2226811a4c93d2ebc7429329808aa");
		}

		return 0;
	}

	//--begin
	/*
		GameApp在各个平台的appid、appkey、各种secret等数字，字符串，
		在java代码很容易被反编译，
		硬编码在c++代码，提高反编译难度，
		如果需要，可进一步加密字符串，
		按json格式传到java，好解析
	*/
	//
	static char gamePlatformNd91[] = "{ \"appid\":\"102581\", \"appkey\":\"149de67f489cbef8681e66bb12056dd9c0b0f06c51a6d449\"}";

	static char gamePlatformUC[] = "{ \"cpid\":\"27385\", \"appid\":\"515092\", \"svrid\":\"1986\", \"appkey\":\"c6570bd69c858f779a649906f7ea4698\"}";

	//360

	static char gamePlatformDangLe[] = "{ \"cpid\":\"323\", \"appid\":\"498\", \"svrid\":\"1\", \"appkey\":\"cOPRIXPv\"}";

	static char gamePlatformXiaoMi[] = "{ \"appid\":\"15438\", \"appkey\":\"2349f52c-498b-fb55-730b-51c7a6916c8d\"}";

	static char gamePlatformWanDouJia[] = "{ \"appid\":\"100000045\", \"appkey\":\"726baa2b5efcf7f314dd15a3ed3e74ca\"}";

	static char gamePlatformDuoKu[] = "{ \"appid\":\"372\", \"appkey\":\"13985c6752ed74c62368a7eefb911145\", \"appsecret\":\"fe9242cb70f5e4fe169f086a63239fc0\"}";

	static char gamePlatformRenRen[] = "{ \"appid\":\"237963\", \"appkey\":\"086a46f2b98447c498a74e740208fb8d\", \"appsecret\":\"4e04c89aa3bb42a2b877fd7f5676cb8e\"}";

	static char gamePlatformYingYongHui[] = "{ \"appid\":\"10030600000001100306\", \"appkey\":\"MjU4Qjc4MzQxQ0U3MjQ5QjVDQTZEQTU2MkM5RkNCRkE5ODA1Q0ExRk1USTJNamt5TnpBM05URXpORFkzT0RNek1EY3JNakE1TkRZeE9EazBNVEkwT0RnME56SXhPRE14TVRrME1UZzRNalUxTlRJd01UZ3dNRFkz\"}";

	static char gamePlatformAnZhi[] = "{ \"appkey\":\"1373855086b74VGQb3C0O1L3x3zpez\", \"appsecret\":\"Mqw5Jx3LkGELDUMD87U576eG\"}";

	//static char gamePlatformJinShan[] = "{ \"appid\":\"200005\", \"appkey\":\"LaL39DyVzLWpXn\", \"appsecret\":\"5\"}";
	static char gamePlatformJinShan[] = "{ \"appid\":\"200011\", \"appkey\":\"3A7jaDPqiXoHWe\", \"appsecret\":\"11\"}";
	//static char gamePlatformJinShan[] = "{ \"appid\":\"200012\", \"appkey\":\"t09Azn53iu4Shl\", \"appsecret\":\"12\"}";
	static char gamePlatformGameSSJJ[] = "{ \"appid\":\"1373981244746530\", \"appkey\":\"e96e4f36bb599d286fa87f2de495cbff\",\"appsecret\":\"4011a79216417a346b4800d0ef22f11d\"}";

	static char gamePlatformKuWo[] = "{ \"appid\":\"6\", \"appkey\":\"mg_6_login\", \"appsecret\":\"mg_6_recharge\"}";

	static char gamePlatformOppo[] = "{ \"appid\":\"1054\", \"appkey\":\"8Ucnr487t2Os4SckK8o844oO0\", \"appsecret\":\"74b33880363f263bbf2ce3473cb29A18\",\"payaddr\":\"http://203.195.147.63/recharge_android/newrecharge.php\"}";

	static char gamePlatformBaiDuGame[] = "{ \"appkey\":\"408f743d01405e13d0283ee895854ef5\", \"appsecret\":\"8a97a7b2408f743d014095854ef53ee9\"}";

	static char gamePlatformLvDou[] = "{ \"appid\":\"10004\", \"appkey\":\"1ad613287e114d6d9571de4e7fb4e64ef95972eabd5c45faa91c7924c620ed65\",\"payaddr\":\"http://203.195.147.63/recharge_android/newrecharge.php\"}";

	static char gamePlatformChuKong[] = "{ \"appid\":\"377144676\", \"appkey\":\"73e9e7352c80143cab960053189798a6\",\"appsecret\":\"http://203.195.147.63/tokenValidate/chukong.php?\"}";

	static char gamePlatformGTV[] = "{ \"appid\":\"2\", \"appkey\":\"39\",\"appsecret\":\"a5bc13014740c509161891da4104f8e2\", \"svrid\":\"0\"}";

	static char gamePlatformXunLei[] = "{ \"cpid\":\"000002\", \"appid\":\"10002\"}";

	static char gamePlatformKuGou[] = "{ \"appid\":\"51\", \"gameid\":\"10170\", \"appkey\":\"0|gTv3}O{%g2z7xU]q%18dR*d%dz%|Gk55cRs6{RN\"}";

	static char gamePlatformHuaWei[] = "{ \"appid\":\"10058657\", \"payid\":\"900086000000103394\", \"private\":\"MIIBVQIBADANBgkqhkiG9w0BAQEFAASCAT8wggE7AgEAAkEAviBXkTfH41uLM3svK0OZ51/BH/LkNX7PJFFtlj8YsvSDX4b/JavaWKROLEbmIQMkAdOkb+iwrxZIeKSQ+Trb3wIDAQABAkBl2aWPYbZvEYdMmNz37EryK9Wla9Tc44XMCuMN7dcpZe7A1cx2CCP7XYhBIcfth4xshFkaoNRED/XiYDVdVldhAiEA5QkJZl//dywnvlF0cQ19DmkcHwMG38iD+nVL+8BqE90CIQDUgp3RJ0qavaPwuoTsX/mDf3PXc2bElew2fMDqaGsg6wIhAMs+eq3zcHMjpt0X0XnlW11nXwISII6DlMK0RkpQlJGtAiB/wH3rTHmXCMZTY22hRBkVqjsTnHZ9W7uDDgBTaq5sPQIhAMFHyHS3pBdW+EicfOOAdA6VOdqsOtiVEXwZPCOumZiS\", \"public\":\"MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAL4gV5E3x+NbizN7LytDmedfwR/y5DV+zyRRbZY/GLL0g1+G/yWr2likTixG5iEDJAHTpG/osK8WSHikkPk6298CAwEAAQ==\", \"payaddr\":\"http://203.195.147.63/recharge_android/newrecharge.php\"}";

	static char gamePlatformSouGou[] = "{ \"appid\":\"90\", \"appkey\":\"3796580dd7d37de315d2cef064002691128c5ab6e584bf2817abca37f1ff6953\", \"appsecret\":\"4ab99c678a49a99ec0b0a39891b0cf1070de806d2271f4a845b22ef27889c96f\", \"payaddr\":\"http://203.195.147.63/tokenValidate/sougou.php\"}";

	/*
	 *
	 * */
	JNIEXPORT jstring JNICALL Java_com_youai_dreamonepiece_GameConfig_nativeReadGamePlatformInfo(JNIEnv*  env, jobject thiz,
			jint platform_type)
	{

		if (platform_type == 1)
		{
			//enPlatform_91
			return JniHelper::string2jstring(gamePlatformNd91);
		}
		if (platform_type == 2)
		{
			//enPlatform_UC
			return JniHelper::string2jstring(gamePlatformUC);
		}
		if (platform_type == 3)
		{
			//enPlatform_360
			return JniHelper::string2jstring("");
		}
		if (platform_type == 4)
		{
			//enPlatform_DangLe
			return JniHelper::string2jstring(gamePlatformDangLe);
		}
		if (platform_type == 5)
		{
			//enPlatform_XiaoMi
			return JniHelper::string2jstring(gamePlatformXiaoMi);
		}
		else if (platform_type == 6)
		{
			//enPlatform_WanDouJia
			return JniHelper::string2jstring(gamePlatformWanDouJia);
		}
		else if (platform_type == 7)
		{
			//enPlatform_BaiduDuoKu
			return JniHelper::string2jstring(gamePlatformDuoKu);
		}
		else if(platform_type == 14)
		{
			return JniHelper::string2jstring(gamePlatformRenRen);
		}
		else if(platform_type == 17)
		{
			return JniHelper::string2jstring(gamePlatformYingYongHui);
		}
		else if(platform_type == 13)
		{
			return JniHelper::string2jstring(gamePlatformAnZhi);
		}
		else if(platform_type == 23)
		{
			return JniHelper::string2jstring(gamePlatformJinShan);
		}
		else if(platform_type == 16)
		{
			return JniHelper::string2jstring(gamePlatformGameSSJJ);
		}
		else if(platform_type == 10)
		{
				return JniHelper::string2jstring(gamePlatformKuWo);
		}
		else if(platform_type == 24)
		{
				return JniHelper::string2jstring(gamePlatformBaiDuGame);
		}
		else if(platform_type == 25)
		{
				return JniHelper::string2jstring(gamePlatformLvDou);
		}
		else if(platform_type == 26)
		{
				return JniHelper::string2jstring(gamePlatformOppo);
		}
		else if(platform_type == 27)
		{
				return JniHelper::string2jstring(gamePlatformChuKong);
		}
		else if(platform_type == 28)
		{
				return JniHelper::string2jstring(gamePlatformXunLei);
		}
		else if(platform_type == 30)
		{
				return JniHelper::string2jstring(gamePlatformKuGou);
		}
		else if(platform_type == 29)
		{
				return JniHelper::string2jstring(gamePlatformGTV);
		}
		else if(platform_type == 31)
		{
				return JniHelper::string2jstring(gamePlatformHuaWei);
		}
		else if(platform_type == 32)
		{
			return JniHelper::string2jstring(gamePlatformSouGou);
		}
		return 0;
	}
	//--end

}//extern "C"

