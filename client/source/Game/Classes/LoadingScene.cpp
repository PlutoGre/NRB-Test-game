
#include "stdafx.h"

#include "LoadingScene.h"
#include "LoadingUnit.h"
#include "AES.h"
#include "CurlDownload.h"
#include "protobufTest.pb.h"
#include "PacketManager.h"
#include "lib91.h"
#include "Login.pb.h"
#include "GamePlatform.h"

USING_NS_CC;

LoadingScene::LoadingScene(void)
	:mScene(0)
	,mLoadingData(0)
	,mLoadingGroup(0)
{
}


LoadingScene::~LoadingScene(void)
{
}

void LoadingScene::Enter( GamePrecedure* )
{
	mScene = CCScene::create();
	mScene->retain();

	CCLayer *layer = CCLayer::create();
	mScene->addChild(layer,0,TAGS_LAYER);

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	// add "HelloWorld" splash screen"
	CCSprite* pSprite = CCSprite::create("HelloWorld.png");

	// position the sprite on the center of the screen
	pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

	// add the sprite as a child to this layer
	layer->addChild(pSprite, 0);

	CCProgressTimer *left = CCProgressTimer::create(CCSprite::create("progressbar.png"));
	left->setType(kCCProgressTimerTypeBar);
	//    Setup for a bar starting from the left since the midpoint is 0 for the x
	left->setMidpoint(ccp(0,0));
	//    Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
	left->setBarChangeRate(ccp(1, 0));
	layer->addChild(left,1,TAGS_PROGRESS);
	left->setPosition(origin.x+visibleSize.width*0.5, 80*CCDirector::sharedDirector()->getContentScaleFactor());

	_addLoadingList();

	// run
	if(cocos2d::CCDirector::sharedDirector()->getRunningScene())
		cocos2d::CCDirector::sharedDirector()->replaceScene(mScene);
	else
		cocos2d::CCDirector::sharedDirector()->runWithScene(mScene);

}

void LoadingScene::Execute( GamePrecedure* pre)
{
	CurlDownload::Get()->update(0);
	
	CCProgressTimer *left = static_cast<CCProgressTimer *>(mScene->getChildByTag(TAGS_LAYER)->getChildByTag(TAGS_PROGRESS));
	
	if(mLoadingGroup&&mLoadingGroup->checkLoadState() == LoadingUnit::LS_DOING)
	{
		left->setPercentage((float)mLoadingGroup->getFinishedCount()*100.0f/(float)mLoadingGroup->getTotalCount());
		return;
	}

	if(CurlDownload::Get()->getDownloadQueueSize()==0 && libPlatformManager::getPlatform()->getLogined())
	{
		mLoadingGroup->release();
		left->setPercentage(100.0f);
		pre->enterMainMenu();
	}
}

void LoadingScene::Exit( GamePrecedure* )
{
	mScene->release();
}

void LoadingScene::_addLoadingList()
{
	libPlatformManager::getPlatform()->init(/*	 102617,"3e77d7dac206996edb4b15883a9f3f04dfa557b73e61fefd",libPlatformManager::DIR_PORTRAIT*/);
	libPlatformManager::getPlatform()->login();
	while(0)
	{
		//			net test
		OPLogin login;
		login.set_puid("name");
		login.set_pwd("password");
		PacketManager::Get()->sendPakcet(PacketManager::Get()->nameToOpcode("OPLogin"),&login);


		//			curl test
		CurlDownload::Get()->addListener(this);
		std::string pathfile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath();
		std::string file1 = pathfile;file1.append("a1.jpg");
		CurlDownload::Get()->downloadFile("http://img.hb.aicdn.com/9ac6cdae4817138ea40e52d45e34c4d9147a3e2b432ee-hYxMjy_fw554",file1.c_str(),6270);
		std::string file2 = pathfile;file2.append("a2.txt");
		CurlDownload::Get()->downloadFile("http://blog.csdn.net/JGood",file2.c_str());
		//------------------------------

		//        encrypt test         //

		AES aes;
		unsigned long size = 0;
		std::string encfilepath = CCFileUtils::sharedFileUtils()->getWritablePath();
		encfilepath.append("/mainUI_enc.dep");
		std::string writefilepath = CCFileUtils::sharedFileUtils()->getWritablePath();
		writefilepath.append("/mainUI_dec.dep");

		{
			unsigned char* dataIn = getFileData("mainUI_ccbi.dep","rb",&size);
			unsigned char* dataOut = new unsigned char[size];
			aes.Encrypt(dataIn,size,dataOut,(const unsigned char*)"0123456789abcdef");

			FILE* fp;
			fp = fopen(encfilepath.c_str(),"wb");
			fwrite(dataOut,1, size,fp);
			fclose(fp);

			delete[] dataIn;
			delete[] dataOut;
		}

		{
			unsigned char* data2 = getFileData(encfilepath.c_str(),"rb",&size);
			unsigned char* data3 = new unsigned char[size];
			aes.Decrypt(data2,size,data3,(const unsigned char*)"0123456789abcdef");


			FILE* fp;
			fp = fopen(writefilepath.c_str(),"wb");
			fwrite(data3,1,size,fp);
			fclose(fp);
			delete[] data2;
			delete[] data3;
		}

		//-----------------------------

		//		protobuf test		//
		std::string str;
		tutorial::Person person;
		person.set_name("Hello");
		person.set_id(1);
		person.SerializeToString(&str);

		tutorial::Person person2;
		person2.ParseFromString(str);
		str = person2.DebugString();

		if(person2.id()!= 1) return;
		//-----------------------------
	}

	mLoadingGroup = new LoadingGroup("mainUI_ccbi.dep");
	mLoadingGroup->loadAuto();
}

void LoadingScene::_addImage( const char* file)
{
	mLoadingData->addImageSync(file);
}

void LoadingScene::_addSound( const char* file)
{
	mLoadingData->addSound(file);
}

void LoadingScene::downloaded( const std::string& url,const std::string& filename )
{
	if(filename.find("a1.jpg")!=string::npos)
	{
		// add "HelloWorld" splash screen"
		
		//cocos2d::CCTextureCache::sharedTextureCache()->addImage(pathfile.c_str());
		CCSprite* pSprite = CCSprite::create(filename.c_str());

		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
		// position the sprite on the center of the screen
		pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));



		mScene->getChildByTag(TAGS_LAYER)->addChild(pSprite,2);
	}
}

void LoadingScene::onLogin( libPlatform* )
{

}

