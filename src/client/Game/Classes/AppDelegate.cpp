
#include "stdafx.h"

#include "AppDelegate.h"
#include "GamePrecedure.h"
#include "SimpleAudioEngine.h"
#include "ThreadSocket.h"
#include "SoundManager.h"
#include "CCBManager.h"
#include "CCBScriptContainer.h"
#include "TimeCalculator.h"
#include <time.h>

//
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "..\..\cocos2dx\platform\android\jni\JniHelper.h"
#include "../../cocos2dx/platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#endif
//

static time_t timestamp = time(0);//see this file

//
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{

}


bool AppDelegate::applicationDidFinishLaunching()
{
	// initialize director
	CCDirector *pDirector = CCDirector::sharedDirector();
	pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
	pDirector->setProjection(kCCDirectorProjection2D);
	CCSize screenSize = CCEGLView::sharedOpenGLView()->getFrameSize();

	/*
	480*854		320*569.333
	320*480		320*480
	640*1136	320*568
	768*1024	360*480
	800*1280	320*512
	480*800		320*533.333
	720*1280	320*568.889
	738*1152	320*480
	*/
	CCSize designSize = CCSizeMake(320, 480);
	float asp = screenSize.height/screenSize.width;
	if(asp >1.5f)
		designSize.height = 320*asp;
	else
		designSize.width = 480/asp;

	//CCSize designSize = CCSize(screenSize.width/2.0f,screenSize.height/2.0f);//CCSizeMake(320, 568);

	if (screenSize.height > 480)
	{
		//CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
		CCFileUtils::sharedFileUtils()->addSearchResolutionsOrder("iphonehd/");
		//pDirector->setContentScaleFactor(screenSize.height/designSize.height);
	}
 	else
 	{
 		CCFileUtils::sharedFileUtils()->addSearchResolutionsOrder("iphone/");
 	}


	CCEGLView::sharedOpenGLView()->setDesignResolutionSize(designSize.width, designSize.height, kResolutionNoBorder);
	//CCEGLView::sharedOpenGLView()->setViewPortInPoints(screenSize.width,screenSize.height,1,1);

#if defined(_DEBUG) //|| (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	// turn on display FPS
	pDirector->setDisplayStats(true);
#endif
	

	// set FPS. the default value is 1.0/60 if you don't call this
	pDirector->setAnimationInterval(1.0 / 30);

    //GamePrecedure::Get()->enterMainMenu();
	GamePrecedure::Get()->enterLoading();

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    if(GamePrecedure::Get()->isInLoadingSceneAndNeedExit())
	{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || defined(PROJECTIOSLOCAL)

	#else
        exit(0);
	#endif
	}
	//
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (getIsOnTempShortPauseJNI() == false)
	#endif
		ThreadSocket::Get()->disconnect();
	//
	CCDirector::sharedDirector()->stopAnimation();
	SoundManager::Get()->appGotoBackground();
    
    if(!GamePrecedure::Get()->isInLoadingScene())
	{
        GamePrecedure::Get()->enterBackGround();
    }
	if(TimeCalculator::getInstance()->hasKey("ExitGameTime"))
	{
		if(TimeCalculator::getInstance()->getTimeLeft("ExitGameTime")<=0)
		{
			exit(0);
		}
	}
	//--begin xinzheng 2013-7-18
	{
		
		time_t nowstamp = time(0);
		struct tm* timetm = localtime(&timestamp);
		struct tm savetm = {0};
		memcpy(&savetm, timetm, sizeof(struct tm));
		/*
			localtime返回同一个struct tm实例的指针
		*/
		struct tm* nowtm = localtime(&nowstamp);
		if (nowtm->tm_yday > savetm.tm_yday)
		{
			exit(0);
		}
#if _DEBUG
		//test
		if (nowtm->tm_yday == savetm.tm_yday && (nowtm->tm_hour == savetm.tm_hour) && (nowtm->tm_min-savetm.tm_min) > 1)
		{
			exit(0);
		}
#endif
		//
		timestamp = nowstamp;
	}
	//--end
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    if(!GamePrecedure::Get()->isInLoadingScene())
        GamePrecedure::Get()->enterForeGround();
    
	CCDirector::sharedDirector()->startAnimation();
	SoundManager::Get()->appResumeBackground();
}

void AppDelegate::purgeCachedData( void )
{
	CCBScriptContainer::purgeCachedData();
	//
	CCBManager::Get()->purgeCachedData();
	//
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
}

void AppDelegate::applicationWillGoToExit()
{
	this->purgeCachedData();
	//
	CCBManager::Get()->Free();
	//
	MessageManager::Get()->Free();
	//
	PacketManager::Get()->Free();
	//
	
	//CurlDownload::Get()->Free();
	//
	SoundManager::Get()->Free();

	//
	Language::Get()->Free();
	//
	GamePrecedure::Get()->Free();
	//
	ThreadSocket::Get()->Free();
	
	//
	
	//

}

void globalPurgeCachedData()
{
	CCApplication::sharedApplication()->purgeCachedData();

	CCDirector::sharedDirector()->purgeCachedData();

	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
}


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C" {

	JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativeGameDestroy(JNIEnv*  env, jobject thiz) {
		CCApplication::sharedApplication()->applicationWillGoToExit();
	}

	JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativePurgeCachedData(JNIEnv*  env, jobject thiz) {
		
		CCApplication::sharedApplication()->purgeCachedData();

		CCDirector::sharedDirector()->purgeCachedData();

		CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();

	}
}
#endif
