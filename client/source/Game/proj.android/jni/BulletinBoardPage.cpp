
#include <string>
#include "BulletinBoardPage.h"
#include <iostream>
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"

BulletinBoardPage* BulletinBoardPage::mInstance = 0;

void BulletinBoardPage::init(float left,float top, float width, float height, BulletinBoardPageListener *listener)
{

}

void BulletinBoardPage::show(const std::string& url)
{
	const char *pUrl = url.c_str();
	showAnnouncement(pUrl);

}

void BulletinBoardPage::close()
{

}

void BulletinBoardPage::webKitSetLoadingTimeOut(int seconds)
{

}

