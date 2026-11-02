

#include <jni.h>
#include "cocos2d.h"

using namespace cocos2d;

// Cocos2d-x 截图
// ===================== 自定义开始 =========================

// 请以下CLASS的值改为改成游戏Acitvity完整类名
//（即继承Cocos2dxActivity的Activity，将点改为下划线）
#define CLASS	com_youai_dreamonepiece_platform_sy4399_SSJJActivity

// ===================== 自定义结束 =========================


// ===================== 功能 =========================

#define FUNC1(clz, fun) Java_##clz##_##fun
#define FUNC2(clz, fun) FUNC1(clz, fun)
#define FUNC(fun) FUNC2(CLASS, fun)

bool screenShot(char const* saveName, int startX, int startY, int width, int height) {
	CCDirector *director = CCDirector::sharedDirector();
	CCSize size = director->getWinSize();
	if(startX < 0) {startX = 0;}
	if(startY < 0) {startY = 0;}
	if(startX > size.width) {startX = size.width;}
	if(startY > size.height) {startY = size.height;}
	if(width <= 0) {width = size.width; startX = 0;}
	if(height <= 0) {height = size.height; startY = 0;}
	if(width > size.width) {width = size.width;}
	if(height > size.height) {height = size.height;}
	//CCLog("screenShot(), startX = %d, startY = %d, width = %d, height = %d", startX, startY, width, height);
	CCRenderTexture* texture = CCRenderTexture::create(width, height);
	texture->setPosition(ccp(startX + width/2, startY + height/2));
	texture->begin();
	director->getRunningScene()->visit();
	texture->end();
	// 在Android上，截图图片保存为 /data/data/[packageName]/files/[saveName]
	bool ret = texture->saveToFile(saveName, kCCImageFormatJPEG);
	return ret;
}

#ifdef __cplusplus
extern "C" {
#endif

jboolean FUNC(nativeScreenShot)(JNIEnv* env, jobject thiz, jstring saveName, jint startX, jint startY, jint width, jint height)
{
	char const* name = env->GetStringUTFChars(saveName, 0);
	bool ret = screenShot(name, startX, startY, width, height);
	env->ReleaseStringUTFChars(saveName, name);
	//CCLog("end nativeScreenShot(), ret = %d", ret);
	return ret;
}

#ifdef __cplusplus
}
#endif
