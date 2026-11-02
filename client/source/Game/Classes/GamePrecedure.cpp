
#include "stdafx.h"

#include "GamePrecedure.h"

#include "StateMachine.h"
#include "LoadingFrame.h"
#include "MainFrame.h"
#include "cocos2d.h"
#include "CCScheduler.h"
#include "Language.h"
#include "CCBManager.h"
#include "PacketManager.h"
#include "CareerConfigManager.h"
#include "PopupManager.h"
#include "TimeCalculator.h"
#include "SeverConsts.h"
#include "CharacterConsts.h"
#include "SkillConsts.h"

#include "DataTableManager.h"
#include "waitingManager.h"
#include "SoundManager.h"
#include "RestrictedWord.h"
#include "InstructionManager.h"

#include "ThreadSocket.h"
#include "inifile.h"

#include "MainPage.h"
#include "TeamPage.h"
#include "CustomPage.h"
#include "GamePlatform.h"
#include "lib91.h"
#include "libPP.h"
#include "libAG.h"
#include "libTB.h"
#include "libITools.h"
#include "lib91Debug.h"
#ifdef PROJECTIOSLOCAL
#include "LocalPlatform.h"
#endif
#include "buyingCheck.h"
#include "FightPage.h"
#include "LoginPacket.h"
#include "FightConfigManager.h"

#include "script_support/CCScriptSupport.h"
#include "CCLuaEngine.h"
#include "Gamelua.h"
#include "pb.h"
#include "libOS.h"
#include "GameNotification.h"
//
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "..\..\cocos2dx\platform\android\jni\JniHelper.h"
#include <jni.h>
#endif
//
using namespace cocos2d;

#define PRELOAD(name, count) \
	{static int preloadCount = 0; \
	int preloadMax = count; \
	if(preloadCount<preloadMax){ \
	CCBContainer* ccb = CCBManager::Get()->createAndLoad(name); \
	ccb->retain(); \
	ccblist.push_back(ccb); \
	preloadCount++;}}

void GamePrecedure::init()
{
	if(mInitialized)
        return;
#ifdef PROJECTIOSLOCAL
    {
        AUTO_REGISTER_PLATFORM(LocalPlatform);
        libPlatformManager::getInstance()->setPlatform("LocalPlatform");
    }
#endif
#ifdef PROJECTPP
{
	AUTO_REGISTER_PLATFORM(libPP);
    //libPlatformManager::getInstance()->registerPlatform("libPP",new libPP);
	libPlatformManager::getInstance()->setPlatform("libPP");
}
#endif
#ifdef PROJECTAG
{
    AUTO_REGISTER_PLATFORM(libAG);
    //libPlatformManager::getInstance()->registerPlatform("libAG",new libAG);
	libPlatformManager::getInstance()->setPlatform("libAG");
}
#endif
    
#ifdef PROJECT91
{
	AUTO_REGISTER_PLATFORM(lib91);
	libPlatformManager::getInstance()->setPlatform("lib91");
}
#endif
    
#ifdef PROJECTTB
    {
        AUTO_REGISTER_PLATFORM(libTB);
        libPlatformManager::getInstance()->setPlatform("libTB");
    }
#endif
    
#ifdef PROJECTITools
    {
        AUTO_REGISTER_PLATFORM(libITools);
        libPlatformManager::getInstance()->setPlatform("libITools");
    }
#endif

#ifdef PROJECT91Debug
    {
        AUTO_REGISTER_PLATFORM(lib91Debug);
        libPlatformManager::getInstance()->setPlatform("lib91Debug");
    }
#endif
#ifdef WIN32
{
	AUTO_REGISTER_PLATFORM(lib91);
	libPlatformManager::getInstance()->setPlatform("lib91");
}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
{
	AUTO_REGISTER_PLATFORM(lib91);
	libPlatformManager::getInstance()->setPlatform("lib91");
}
#endif
	SeverConsts::Get()->initSearchPath();
	SeverConsts::Get()->setOriginalSearchPath();

	TimeCalculator::Get()->init();
//	cocos2d::ccLanguageType languagetype = cocos2d::CCApplication::sharedApplication()->getCurrentLanguage();
// 	if(languagetype == kLanguageChinese)
// 		Language::Get()->init("Lang/chinese.lang");
// 	else
// 		Language::Get()->init("Lang/english.lang");
	
	Language::Get()->init("Lang/chinese.lang");

	//PacketManager::Get()->init("netConfig.cfg");
	//CareerConfigManager::Get()->init("CareerConfig.cfg");

	//SkillConsts::Get()->init("skills.cfg");
	//CharacterConsts::Get()->init("characters.cfg");
	CCSpriteFrameCache * frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	ConfigFile cfg;
	cfg.load("Imageset.txt");
	ConfigFile::SettingsMapIterator itr = cfg.getSettingsMapIterator();
	while(itr.hasMoreElements())
	{
		std::string filename = itr.getNext();
		if(!filename.empty())
			frameCache->addSpriteFramesNameWithFile(filename.c_str());
	}
	
	m_pScheduler = CCDirector::sharedDirector()->getScheduler();
	m_pScheduler->retain();
	m_pScheduler->scheduleUpdateForTarget(this, 0, false);
    
    mInitialized = true;

	mReadTables = false;

	CCLog("GamePrecedure::init");
}

void GamePrecedure::enterLoading()
{
	if(!mLoadingScene)
    {
        init();
        mLoadingScene =  LoadingFrame::create();
        mLoadingScene->retain();
    }
    
	if(!m_pStateMachine)
		m_pStateMachine = new StateMachine<GamePrecedure>(this);
	
    m_pStateMachine->ChangeState(mLoadingScene);
    
    if(mMainFrame)
	{
		mMainFrame->release();
		mMainFrame = 0;
	}
}


void GamePrecedure::enterMainMenu()
{
	gotoMainScene = true;
    
}

void GamePrecedure::enterInstruction(int id)
{
}

void GamePrecedure::enterGame()
{
}

void GamePrecedure::exitGame()
{
	if(m_pStateMachine)
	{
		m_pStateMachine->CurrentState()->Exit(this);
		delete m_pStateMachine;
		m_pStateMachine = 0;
	}
	if(mMainFrame)
	{
		mMainFrame->release();
		mMainFrame = 0;
	}
	PacketManager::Get()->disconnect();
	if(m_pScheduler)
	{
		m_pScheduler->unscheduleUpdateForTarget(this);
		m_pScheduler->release();
		m_pScheduler = 0;
	}
}

void GamePrecedure::update( float dt )
{
	mFrameTime = dt;
	mTotalTime += dt;
	static float durationTime=0;
	durationTime+=dt;
	if(durationTime>1.0f)
	{
		durationTime -=1.0f;
		++mServerTime;
	}
	m_pStateMachine->Update();
	PacketManager::Get()->update(dt);
	MessageManager::Get()->update();
	waitingManager::Get()->update(dt);
	TimeCalculator::Get()->update();
	LoginPacket::Get()->update(dt);
	static std::list<CCBContainer*> ccblist;

	if(gotoMainScene && mRequestPackages.empty())
	{
		PacketManager::Get()->removePacketHandler(this);
		LoginPacket::Get()->setEnabled(true);

		while(!ccblist.empty())
		{
			ccblist.front()->release();
			ccblist.pop_front();
		}

		gotoMainScene = false;

		mInLoadingScene = false;

		SoundManager::Get()->init();
		CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
		SoundManager::Get()->playGeneralMusic();

		if(!mMainFrame)
			mMainFrame = dynamic_cast<MainFrame*>(CCBManager::Get()->getPage("MainFrame"));//MainFrame::create();
		if(mMainFrame)mMainFrame->retain();

		if(!m_pStateMachine)
			m_pStateMachine = new StateMachine<GamePrecedure>(this);
		m_pStateMachine->ChangeState(mMainFrame);

		if(mLoadingScene)
		{
			mLoadingScene->release();
			mLoadingScene = 0;
		}
	}
	else if(isInLoadingScene() && SeverConsts::Get()->checkUpdateInfo() == SeverConsts::CS_OK)
	{
        long mem = libOS::getInstance()->avalibleMemory();
        if(mem>30)
		{
            PRELOAD("CareerChapterButton.ccbi",12);//20
            PRELOAD("CareerCustomsPass.ccbi",12);//30
            PRELOAD("EquipmentContent.ccbi",12);//100->20//xinzheng 2013-06-03用省内存的ScrollViewFacade
            PRELOAD("SkillContent.ccbi",12);//100->20//xinzheng 2013-06-03用省内存的ScrollViewFacade
            PRELOAD("MemberCard.ccbi",8);//8
            PRELOAD("TeamNoMem.ccbi",0);
            PRELOAD("TeamOverviewCard.ccbi",1);
            PRELOAD("framedButton.ccbi",8);//8
            PRELOAD("Battle.ccbi",8);//8
			PRELOAD("HandbookContent.ccbi",40);
			PRELOAD("AddPowerPage.ccbi",1);
			PRELOAD("ContinueLoginRewardContent.ccbi",1);
			PRELOAD("FightEndRewardFrame.ccbi",1);
			PRELOAD("BoundlessHolePage.ccbi",1);
			PRELOAD("WorldBossPage.ccbi",1);
			PRELOAD("LevelUpRewardItem.ccbi",1);
			PRELOAD("SkillBook.ccbi",1);
        }
    }
	else if(!isInLoadingScene())
	{
		cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine();
		if(pEngine)
		{
			pEngine->executeGlobalFunctionByName("GamePrecedure_update",this,"GamePrecedure");
		}
		if(mHeartbeatTime > 30)
		{
			mHeartbeatTime = 0;
			OPHeartbeat info;
			info.set_version(1);
			PacketManager::Get()->sendPakcet(OPCODE_HEARTBEAT_C,&info, false);
		}
		else
			mHeartbeatTime += dt;
	}
}

float GamePrecedure::getFrameTime()
{
	return mFrameTime;
}

float GamePrecedure::getTotalTime()
{
	return mTotalTime;
}

static time_t s_gotoBackgroundTime = 0;

void GamePrecedure::enterForeGround()
{
    std::string privateLogin = VaribleManager::Get()->getSetting("privatePause","","false");
    if(privateLogin!="true")
        libPlatformManager::getPlatform()->gamePause();
    
    time_t t = time(0);

//     if(s_gotoBackgroundTime>0 && t-s_gotoBackgroundTime>60*60)
//         lib91::getInstance()->login();
    
    LoginPacket::Get()->forceSentPacket();

	cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine();
	if(pEngine)
	{
		pEngine->executeGlobalFunctionByName("GamePrecedure_enterForeground",this,"GamePrecedure");
	}

}

void GamePrecedure::preEnterMainMenu()
{
	readTables();

	if(!cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine())
	{
		cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine();
		cocos2d::CCScriptEngineManager::sharedManager()->setScriptEngine(pEngine);
		tolua_Gamelua_open(pEngine->getLuaStack()->getLuaState());
		luaopen_pb(pEngine->getLuaStack()->getLuaState());

		pEngine->executeString("require \"main\"");

		pEngine->executeGlobalFunctionByName("GamePrecedure_preEnterMainMenu",this,"GamePrecedure");

	}

	requestPackages();

// 	//temp code
// 	InstructionManager::Get()->setStep(1);
// 	OPInstructionRet ret;
// 	ret.mutable_battle1()->CopyFrom(*FightPage::receiveMessage());
// 	ret.mutable_battle2()->CopyFrom(*FightPage::receiveMessage());
// 	ret.mutable_disciplerandom()->set_id(2);
// 	ret.mutable_disciplerandom()->set_itemid(1);
// 	ret.mutable_discipleselected()->set_id(1);
// 	ret.mutable_discipleselected()->set_itemid(1);
// 	InstructionManager::Get()->setMessage(&ret);
// 	//


}

void GamePrecedure::readTables()
{
	mReadTables = true;
	CCFileUtils::sharedFileUtils()->purgeCachedEntries();
	//
	Language::Get()->clear();
	Language::Get()->init("Lang/chinese.lang");

    VaribleManager::Get()->reload();
    
	DiscipleTableManager::Get()->init("Disciples.txt");
	unsigned int num = DiscipleTableManager::Get()->getTotalNum();

	SkillTableManager::Get()->init("Skills.txt");
	num = SkillTableManager::Get()->getTotalNum();

	EquipTableManager::Get()->init("Equips.txt");
	num = EquipTableManager::Get()->getTotalNum();

	YuanfenTableManager::Get()->init("Yuanfens.txt");
	num = YuanfenTableManager::Get()->getTotalNum();

	UserPropertyTableManager::Get()->init("UserProperty.txt");
	num = UserPropertyTableManager::Get()->getTotalNum();

	ToolTableManager::Get()->init("Tools.txt");
	num = ToolTableManager::Get()->getTotalNum();

	DisciplesLevelParamManager::Get()->init("DisciplesLevelParam.txt");
	num = DisciplesLevelParamManager::Get()->getTotalNum();

	EquipLevelParamManager::Get()->init("EquipLevelParam.txt");
	num = EquipLevelParamManager::Get()->getTotalNum();

	HelpTableManager::Get()->init("Help.txt");
	num = HelpTableManager::Get()->getTotalNum();

	VIPPrivilegeTableManager::Get()->init("VIPPrivilege.txt");
	num = VIPPrivilegeTableManager::Get()->getTotalNum();

	AboutTableManager::Get()->init("About.txt");
	num = AboutTableManager::Get()->getTotalNum();

	ContinueLoginTableManager::Get()->init("ContinueLogin.txt");
	num = ContinueLoginTableManager::Get()->getTotalNum();

	AdventureTableManager::Get()->init("AdventureConfig.txt");
	num = AdventureTableManager::Get()->getTotalNum();

	AdventureTeacherTableManager::Get()->init("TeachConfig.txt");
	num = AdventureTeacherTableManager::Get()->getTotalNum();

	PlayerGradeTableManager::Get()->init("PlayerGradeConfig.txt");
	num = PlayerGradeTableManager::Get()->getTotalNum();

	SkillBookTableManager::Get()->init("SkillBooks.txt");
	num = SkillBookTableManager::Get()->getTotalNum();

	TitleTableManager::Get()->init("Title.txt");
	num = TitleTableManager::Get()->getTotalNum();

	TeamBuffBoldnessTableManager::Get()->init("TeamBuffBoldness.txt");
	num = TeamBuffBoldnessTableManager::Get()->getTotalNum();

	TeamBuffTableManager::Get()->init("TeamBuff.txt");
	num = TeamBuffTableManager::Get()->getTotalNum();

	AnnouncementTableManager::Get()->init("AnnouncementConfig.txt");
	num = AnnouncementTableManager::Get()->getTotalNum();

	ToolTableManager::Get()->initIncludeOther();



	NewActivePromptTableManager::Get()->init("NewActivePrompt.txt");
	num = NewActivePromptTableManager::Get()->getTotalNum();

	ArenaRewardTableManager::Get()->init("ArenaReward.txt");
	num = ArenaRewardTableManager::Get()->getTotalNum();

	FightEndRewardTableManager::Get()->init("FightEndRewardConfig.txt");
	num = FightEndRewardTableManager::Get()->getTotalNum();

	AdventureContinueLoginTableManager::Get()->init("AdventureContinueLoginConifg.txt");
	num = AdventureContinueLoginTableManager::Get()->getTotalNum();

	AdventureInviteTableManager::Get()->init("AdventureInviteConfig.txt");
	num = AdventureInviteTableManager::Get()->getTotalNum();

	FortuneComeRewardTableManager::Get()->init("FortuneComeReward.txt");
	num = AdventureInviteTableManager::Get()->getTotalNum();

	StoneTableManager::Get()->init("StoneConfig.txt");
	num = StoneTableManager::Get()->getTotalNum();

	CareerConfigManager::Get()->init("chapters.txt","stages.txt","stagesNPC.txt");

	RestrictedWord::Get()->init("restrict.dlt");

	FightConfigManager::Get()->init("FightVarible.cfg");

	VipDataTableManager::Get()->init("VIPData.txt");

	ActivityPopTableManager::Get()->init("ActivityPopConfig.txt");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	PlatformNameManager::Get()->init("platfromName.txt");
	BBSConfigManager::Get()->init("BBSConfig.txt");
#endif

	PopupManager::Get()->init();

	TableReaderManager::Get()->reloadAllReader();
}

void GamePrecedure::requestPackages()
{
	LoginPacket::Get()->setEnabled(false);

	OPUserBasicInfo *info = new OPUserBasicInfo;
	info->set_version(1);
	_requestPackage(OPCODE_GET_USER_BASIC_INFO_C,OPCODE_GET_USER_BASIC_INFORET_S,info);
	
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_BASIC_INFORET_S);

	OPUserDiscipleInfo* discipleInfo = new OPUserDiscipleInfo;
	discipleInfo->set_version(1);
	_requestPackage(OPCODE_GET_USER_DISCIPLE_INFO_C, OPCODE_GET_USER_DISCIPLE_INFORET_S, discipleInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_DISCIPLE_INFORET_S);

	OPUserToolInfo* userToolInfo = new OPUserToolInfo;
	userToolInfo->set_version(1);
	_requestPackage(OPCODE_GET_USER_TOOL_INFO_C, OPCODE_GET_USER_TOOL_INFORET_S, userToolInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_TOOL_INFORET_S);

	OPUserSoulInfo* userSoulInfo = new OPUserSoulInfo;
	userSoulInfo->set_version(1);
	_requestPackage(OPCODE_GET_USER_SOUL_INFO_C, OPCODE_GET_USER_SOUL_INFORET_S ,userSoulInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_SOUL_INFORET_S);

	OPUserSkillInfo* skillInfo = new OPUserSkillInfo;
	skillInfo->set_version(1);
	_requestPackage(OPCODE_GET_USER_SKILL_INFO_C, OPCODE_GET_USER_SKILL_INFORET_S ,skillInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_SKILL_INFORET_S);

	OPUserCareerInfo* career = new OPUserCareerInfo;
	career->set_version(1);
	_requestPackage(OPCODE_GET_USER_CAREER_INFO_C,OPCODE_GET_USER_CAREER_INFORET_S,career);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_CAREER_INFORET_S);

	OPShopList* shop = new OPShopList;
	shop->set_version(1);
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	shop->set_platname(PlatformNameManager::getInstance()->getPlatNameByID(libPlatformManager::getPlatform()->getPlatformId()));
#else
	//shop->set_platname(libPlatformManager::getInstance()->getPlatform()->getClientChannel());
#endif
	_requestPackage(OPCODE_GET_SHOP_ITEM_LIST_C,OPCODE_GET_SHOP_ITEM_LISTRET_S,shop);
	//waitingManager::Get()->startWaiting(OPCODE_GET_SHOP_ITEM_LISTRET_S);

	OPUserEquipleInfo* equip = new OPUserEquipleInfo;
	equip->set_version(1);
	_requestPackage(OPCODE_GET_USER_EQUIP_INFO_C,OPCODE_GET_USER_EQUIP_INFORET_S, equip);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_EQUIP_INFORET_S);

	OPUserBattleArray* userBattleArray = new OPUserBattleArray;
	userBattleArray->set_version(1);
	_requestPackage(OPCODE_GET_USER_BATTLE_ARRAY_INFO_C,OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S, userBattleArray);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S);

	OPUserMarketInfo* marketInfo = new OPUserMarketInfo;
	marketInfo->set_version(1);
	_requestPackage(OPCODE_GET_USER_MARKET_INFO_C,OPCODE_GET_USER_MARKET_INFORET_S,marketInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_MARKET_INFORET_S);

	OPTitleInfo* titleInfo = new OPTitleInfo;
	titleInfo->set_version(1);
	_requestPackage(OPCODE_GET_TITLE_INFO_C,OPCODE_GET_TITLE_INFORET_S,titleInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_TITLE_INFORET_S);

// 	OPUserArenaInfo userArenaInfo;
// 	userArenaInfo.set_version(1);
// 	PacketManager::Get()->sendPakcet(OPCODE_USER_ARENA_INFO_C,&userArenaInfo);
	//waitingManager::Get()->startWaiting(OPCODE_USER_ARENA_INFORET_S);

// 	OPGetArenaOpponentList getArenaOpponentList;
// 	getArenaOpponentList.set_version(1);
// 	PacketManager::Get()->sendPakcet(OPCODE_GET_ARENA_OPPONENT_LIST_C,&getArenaOpponentList);


	buyingCheck::Get()->init();
	buyingCheck::Get()->check();
}

void GamePrecedure::reRequestPackages()
{

	OPUserBasicInfo *info = new OPUserBasicInfo;
	info->set_version(1);
	_requestPackage(OPCODE_GET_USER_BASIC_INFO_C,OPCODE_GET_USER_BASIC_INFORET_S,info);

	OPShopList* shop = new OPShopList;
	shop->set_version(1);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	shop->set_platname(PlatformNameManager::getInstance()->getPlatNameByID(libPlatformManager::getPlatform()->getPlatformId()));
#endif
	_requestPackage(OPCODE_GET_SHOP_ITEM_LIST_C,OPCODE_GET_SHOP_ITEM_LISTRET_S,shop);
	//waitingManager::Get()->startWaiting(OPCODE_GET_SHOP_ITEM_LISTRET_S);

	OPUserMarketInfo* marketInfo = new OPUserMarketInfo;
	marketInfo->set_version(1);
	_requestPackage(OPCODE_GET_USER_MARKET_INFO_C,OPCODE_GET_USER_MARKET_INFORET_S,marketInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_USER_MARKET_INFORET_S);

	OPTitleInfo* titleInfo = new OPTitleInfo;
	titleInfo->set_version(1);
	_requestPackage(OPCODE_GET_TITLE_INFO_C,OPCODE_GET_TITLE_INFORET_S,titleInfo);
	//waitingManager::Get()->startWaiting(OPCODE_GET_TITLE_INFORET_S);

}
const std::string GamePrecedure::gameSnapshot()
{
	//CCDirector::sharedDirector()->mainLoop();
	//
	CCSize size = CCEGLView::sharedOpenGLView()->getFrameSize();//CCDirector::sharedDirector()->getWinSize();////;//getWinSizeInPixels
	//size.width = 320;
	//size.height = 480;
	CCRenderTexture* in_texture = CCRenderTexture::create((int)size.width, (int)size.height, kCCTexture2DPixelFormat_RGBA8888);
	//in_texture->getSprite()->setAnchorPoint( ccp(0.5f,0.5f) );
	//in_texture->setPosition( ccp(size.width/2, size.height/2) );
	//in_texture->setAnchorPoint( ccp(0.5f,0.5f) );

	CCLOG("GamePrecedure::gameSnapshot width,height: %d,%d", (int)size.width, (int)size.height);

	//CCDirector::sharedDirector()->stopAnimation();
	//CCDirector::sharedDirector()->mainLoop();
	//in_texture->begin();
	//in_texture->beginWithClear(0, 0, 0, 0, 0, 0);
	in_texture->keepBegin();
	if (this->mInLoadingScene)
	{
		if (mLoadingScene && mLoadingScene->getRootSceneNode())
		{
			
			mLoadingScene->getRootSceneNode()->visit();
			//in_texture->addChild(mLoadingScene);
		}
	}
	else
	{
		if (mMainFrame && mMainFrame->getRootSceneNode())
		{
			
			mMainFrame->getRootSceneNode()->visit();
			//in_texture->addChild(mMainFrame);
			
		}
	}
	//in_texture->visit();
	//in_texture->end();
	in_texture->keepEnd();
	//CCDirector::sharedDirector()->startAnimation();

	std::string file_path = "/Cache/share.png";
	bool b_result = in_texture->saveToFile(file_path.c_str(), kCCImageFormatPNG);
	
	if (b_result)
	{
		CCLOG("GamePrecedure::gameSnapshot success to %s", (CCFileUtils::sharedFileUtils()->getWritablePath() + file_path).c_str());
	}
	else
	{
		CCLOG("GamePrecedure::gameSnapshot failed, %s", (CCFileUtils::sharedFileUtils()->getWritablePath() + file_path).c_str());
	}
	
	return CCFileUtils::sharedFileUtils()->getWritablePath() + file_path;
}

void GamePrecedure::unInit()
{
	if (mReadTables)
	{
		DiscipleTableManager::Get()->Free();
		//
		SkillTableManager::Get()->Free();
		//
		EquipTableManager::Get()->Free();
		//
		YuanfenTableManager::Get()->Free();
		//
		UserPropertyTableManager::Get()->Free();
		//
		ToolTableManager::Get()->Free();
		//
		DisciplesLevelParamManager::Get()->Free();
		//
		EquipLevelParamManager::Get()->Free();
		//
		HelpTableManager::Get()->Free();
		//
		VIPPrivilegeTableManager::Get()->Free();
		//
		AboutTableManager::Get()->Free();
		//
		ContinueLoginTableManager::Get()->Free();
		//
		AdventureTableManager::Get()->Free();
		//
		AdventureTeacherTableManager::Get()->Free();
		//
		PlayerGradeTableManager::Get()->Free();
		//
		SkillBookTableManager::Get()->Free();
		//
		TitleTableManager::Get()->Free();
		//
		TeamBuffBoldnessTableManager::Get()->Free();
		//
		TeamBuffTableManager::Get()->Free();
		//
		AnnouncementTableManager::Get()->Free();
		//
		ToolTableManager::Get()->Free();
		//
		NewActivePromptTableManager::Get()->Free();
		//
		ArenaRewardTableManager::Get()->Free();
		//
		FightEndRewardTableManager::Get()->Free();
		//
		AdventureContinueLoginTableManager::Get()->Free();
		//
		AdventureInviteTableManager::Get()->Free();
		//
		CareerConfigManager::Get()->Free();
		//
		RestrictedWord::Get()->Free();
		//
		FightConfigManager::Get()->Free();
		//
		VipDataTableManager::Get()->Free();
		//
		ActivityPopTableManager::Get()->Free();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		PlatformNameManager::Get()->Free();
		BBSConfigManager::Get()->Free();
#endif
		//
		PopupManager::Get()->Free();
		//
		TableReaderManager::Get()->Free();
		//
		buyingCheck::Get()->Free();
	}
	//
	mReadTables = false;
	//
	TimeCalculator::Get()->Free();
	//
	SeverConsts::Get()->Free();
	//
	waitingManager::Get()->Free();
	//
	LoginPacket::Get()->Free();
	//
	if (!mInLoadingScene)
	{
		InstructionManager::Get()->Free();
	}
	//
}

GamePrecedure::~GamePrecedure()
{
	this->unInit();
}

void GamePrecedure::_requestPackage( int opcode, int opcoderet, ::google::protobuf::Message* message )
{
	REQUEST_PACKAGE pkt;
	pkt.opcode = opcode;
	pkt.opcoderet = opcoderet;
	pkt.message = message;

	PacketManager::Get()->regisiterPacketHandler(opcoderet,this);
	mRequestPackages.insert(std::make_pair(opcoderet,pkt));

	PacketManager::Get()->sendPakcet(opcode,message);
}

void GamePrecedure::_gotPackage( int opcoderet )
{
	std::map<int,REQUEST_PACKAGE>::iterator it = mRequestPackages.find(opcoderet);
	if(it!=mRequestPackages.end())
	{
		delete it->second.message;
		mRequestPackages.erase(it);
	}
}

void GamePrecedure::_failedPakcage( int opcode, bool isSendFaild )
{
	if(!isSendFaild)//opcode is opcode ret
	{
		std::map<int,REQUEST_PACKAGE>::iterator it = mRequestPackages.find(opcode);
		if(it!=mRequestPackages.end())
		{
			PacketManager::Get()->sendPakcet(it->second.opcode, it->second.message);
		}
	}
	else
	{
		std::map<int,REQUEST_PACKAGE>::iterator it = mRequestPackages.begin();
		for(;it!=mRequestPackages.end();++it)
		{
			if(it->second.opcode == opcode)
				PacketManager::Get()->sendPakcet(it->second.opcode, it->second.message);
		}
	}
}

void GamePrecedure::enterBackGround()
{
	if(!mRequestPackages.empty())
		exit(0);

	GameNotification::Get()->addNotification();
    
    s_gotoBackgroundTime = time(0);

	cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine();
	if(pEngine)
	{
		pEngine->executeGlobalFunctionByName("GamePrecedure_enterBackGround",this,"GamePrecedure");
	}
    
}

bool GamePrecedure::isInLoadingSceneAndNeedExit()
{
    if(mLoadingScene)
    {
        return !mLoadingScene->isUpdateDown();
    }
    return false;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
	JNIEXPORT jstring JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativeGameSnapshot() {
		JNIEnv * env = 0;

		if (JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK || ! env) {
			return 0;
		}
		std::string file_path = GamePrecedure::Get()->gameSnapshot();
		return env->NewStringUTF(file_path.c_str());
	}

	JNIEXPORT jboolean JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativeHasEnterMainFrame(JNIEnv * env, jobject obj) {

		return (!GamePrecedure::Get()->isInLoadingScene());

	}

	JNIEXPORT jint JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativeGetServerId(JNIEnv * env, jobject obj) {

		return GamePrecedure::Get()->getServerID();

	}
}
#endif

