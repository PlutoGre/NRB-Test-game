
#include "stdafx.h"

#include "MainFrame.h"
#include "GameMessages.h"
#include "Language.h"

#include "cocos-ext.h"
#include "cocos2d.h"

#include "MainPage.h"
#include "TeamPage.h"
#include "CustomPage.h"
#include "MessageBoxPage.h"
#include "InstructionManager.h"
#include "GamePrecedure.h"
#include "BlackBoard.h"
#include "MessageBoxPage.h"
#include "libOS.h"
#include "json/json.h"
#include "GamePlatform.h"
#include "AppDelegate.h"
#include "DataTableManager.h"
USING_NS_CC;
USING_NS_CC_EXT;

REGISTER_PAGE("MainFrame",MainFrame);
class MainframeScorllviewController : public cocos2d::extension::CCScrollViewDelegate
{
	virtual void scrollViewDidScroll(CCScrollView* view)
	{
	
	}
	virtual void scrollViewDidDeaccelerateScrolling(CCScrollView* view, CCPoint initSpeed)
	{

	};
	virtual void scrollViewDidDeaccelerateStop(CCScrollView* view, CCPoint initSpeed)
	{
		static std::string buttonsStr =  VaribleManager::getInstance()->getSetting("MainFrameScrollButtons","","6");
		if(buttonsStr == "6")
		{
			CCBContainer* con = CCBManager::Get()->getPage("MainFrame");
			float button = 0.0f;
			CCB_FUNC_RETURN(con,"mBottomL",CCNode,getContentSize().width,button);
			float xoff = view->getContentOffset().x;
			float unit = view->getContainer()->getContentSize().width/6;
			if(xoff<-(unit-button)*0.5f)
			{
				view->setContentOffsetInDuration(view->minContainerOffset(),0.3f);
				CCB_FUNC(con,"mBottomL",cocos2d::CCNode,setVisible(true));
				CCB_FUNC(con,"mBottomR",cocos2d::CCNode,setVisible(false));
			}
			else
			{
				view->setContentOffsetInDuration(view->maxContainerOffset(),0.3f);
				CCB_FUNC(con,"mBottomL",cocos2d::CCNode,setVisible(false));
				CCB_FUNC(con,"mBottomR",cocos2d::CCNode,setVisible(true));
			}
		}
	};
	virtual void scrollViewDidZoom(CCScrollView* view)
	{

	}
};

static MainframeScorllviewController sMainframeScrollviewController;

MainFrame::MainFrame(void)
{
	mIsInitBoardCast=false;
}


MainFrame::~MainFrame(void)
{
	mIsInitBoardCast=false;
}

void MainFrame::initLoadBoardCast()
{
	if(ServerDateManager::Get()->getBoardCastInfoListTotalNum()!=0)
	{
		mBroadcast->addChild(&mBoardCastLabel);
		mBoardCastLabel.init(0,13,this->getContentSize().width*2,20);
		mBoardCastLabel.initStr(CCSizeMake(0,20),kCCTextAlignmentLeft,"Arial",20);
		mIsInitBoardCast=true;
	}
}

void MainFrame::loadBoardCast()
{
	OPBoardCastInfo info;
	info.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_GET_BOARDCAST_INFO_C,&info);
}

void MainFrame::_loadPopPage()
{
	const UserBasicInfo info =ServerDateManager::Get()->getUserBasicInfo();
	if(info.tutorialstep==1 && (InstructionManager::Get()->loadLocalInstructionStep()>=InstructionManager::TOTAL_INSTRUCTION||InstructionManager::Get()->loadLocalInstructionStep()==1))
	{
		if(BlackBoard::Get()->hasVarible("hasContinue") && !BlackBoard::Get()->getVaribleAsBool("hasContinue")||StringConverter::parseInt(VaribleManager::Get()->getSetting("ContinueLoginOpenStatus"))==0)
		{
			if(VaribleManager::Get()->getSetting("AnnouncementPageOpenStatus")=="1")
			{
				MsgMainFramePushPage msg;
				msg.pageName = "AnnouncementPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
		else
		{
			OPContinueLogin msg;
			msg.set_version(1);
			PacketManager::Get()->sendPakcet(OPCODE_GET_CONTINUE_LOGIN_C, &msg);
		}
	}
}

void MainFrame::Enter( GamePrecedure* )
{
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

    libPlatformManager::getPlatform()->registerListener(this);

	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_CHANGEPAGE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_POPPAGE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_PUSHPAGE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_COVERSHOW,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_COVERHIDE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_MSGSHOW,this);
	mScene = CCScene::create();
	mScene->retain();

	load();
    cocos2d::extension::CCScrollView* view = 0;
	cocos2d::CCObject* viewNode = this->getVariable("mMainbuttons");
	if(viewNode)
	{
		view = dynamic_cast<cocos2d::extension::CCScrollView*>(viewNode);
		if(view)
		{
			view->setContentOffsetInDuration(view->maxContainerOffset(),0.5f);
			view->setDelegate(&sMainframeScrollviewController);
			view->setBounceable(false);
		}
	}
	CCB_FUNC(this,"mBottomL",cocos2d::CCNode,setVisible(false));
	CCB_FUNC(this,"mBottomR",cocos2d::CCNode,setVisible(true));

    if(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="PP")
        CCB_FUNC(this,"mForum",CCNode,setVisible(false));
    if(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="91")
        CCB_FUNC(this,"mForum",CCNode,setVisible(true))
    if(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="AppleGarden")
            CCB_FUNC(this,"mForum",CCNode,setVisible(false));
    if(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="TB")
        CCB_FUNC(this,"mForum",CCNode,setVisible(true));
    if(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="ITO")
        CCB_FUNC(this,"mForum",CCNode,setVisible(false));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CCB_FUNC(this,"mForum",CCMenuItemImage,setVisible(BBSConfigManager::Get()->getBBSOpenStatusByID(libPlatformManager::getPlatform()->getPlatformId())));
#endif
	mScene->addChild(this);

	m_pStateMachine = new StateMachine<MainFrame>(this);
	
// 	BasePage* mainpage = CCBManager::Get()->getPage("MainPage");
// 	if(mainpage)
// 		mainpage->load();
// 
// 	BasePage* teampage = CCBManager::Get()->getPage("TeamPage");;
// 	if(teampage)
// 		teampage->load();


	CCDirector::sharedDirector()->setDepthTest(true);
	cocos2d::CCScene * scene =  cocos2d::CCTransitionFade::create(2.0f,mScene);
	// run
	if(cocos2d::CCDirector::sharedDirector()->getRunningScene())
		cocos2d::CCDirector::sharedDirector()->replaceScene(scene);
	else
		cocos2d::CCDirector::sharedDirector()->runWithScene(mScene);
	loadBoardCast();
	_loadPopPage();
	InstructionManager::Get()->start(this);
	mCurShowPage = NULL;
	showPage("MainPage");
	_refreshActivityTip("mAdvNew","ad");
	_refreshActivityTip("mPraise","m");
}

void MainFrame::Exit( GamePrecedure* )
{
	MessageManager::Get()->removeMessageHandler(this);
	libOS::getInstance()->removeListener(this);
	libPlatformManager::getPlatform()->removeListener(this);

	if(m_pStateMachine)
	{
		m_pStateMachine->CurrentState()->Exit(this);
		delete m_pStateMachine;
		m_pStateMachine = 0;
	}
	BasePage* mainpage = CCBManager::Get()->getPage("MainPage");
	if(mainpage)
		mainpage->unload();

	BasePage* teampage = CCBManager::Get()->getPage("TeamPage");;
	if(teampage)
		teampage->unload();

	MessageManager::Get()->removeMessageHandler(this);
	mScene->release();
}

void MainFrame::Execute( GamePrecedure* )
{
	if(m_pStateMachine)
		m_pStateMachine->Update();
	if(mIsInitBoardCast)
	{
		mBoardCastLabel.run();
	}
	static float durationTime=0;
	static float durationTimeTip=0;
	durationTime+=GamePrecedure::Get()->getFrameTime();
	durationTimeTip+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>1.0f)
	{
		durationTime -=1.0f;
		ServerDateManager::Get()->updateBasicSyncTime();
	}
	if(durationTimeTip>2.0f)
	{
		durationTimeTip-=2.0f;
		_refreshActivityTip("mPraise","m");
		_refreshActivityTip("mAdvNew","ad");
	}
}

void MainFrame::_refreshActivityTip(const std::string guideName,std::string key)
{
	bool _isShow=false;
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		Json::Value _status;
		Json::Reader jreader;
		jreader.parse(ServerDateManager::Get()->getUserBasicInfo().actStatus.c_str(),_status,false);
		if(!_status[key].empty())
		{
			if(_status[key].asInt()>0)
			{
				char str[128];
				sprintf(str,"s%d_tip_%s",GamePrecedure::Get()->getServerID(),key.c_str());
				unsigned int _version=cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(str,0);
				_isShow=_status[key].asInt()>_version;
			}
		}
	}
	CCB_FUNC(this,guideName,CCNode,setVisible(_isShow));
}

void MainFrame::_saveActivityGuideStatus(const std::string guideName,std::string key)
{
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		CCObject* _obj=getVariable(guideName);
		if(_obj)
		{
			CCNode* _guide=dynamic_cast<CCNode* >(_obj);
			bool isShow=_guide->isVisible();
			if(isShow)
			{
				Json::Value _status;
				Json::Reader jreader;
				jreader.parse(ServerDateManager::Get()->getUserBasicInfo().actStatus.c_str(),_status,false);
				if(!_status[key].empty())
				{
					if(_status[key].asInt()>0)
					{
						char str[128];
						sprintf(str,"s%d_tip_%s",GamePrecedure::Get()->getServerID(),key.c_str());
						cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(str,_status[key].asInt());
						cocos2d::CCUserDefault::sharedUserDefault()->flush();
						_guide->setVisible(false);
					}
				}
			}
		}
	}	
}

void MainFrame::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_CHANGEPAGE)
	{
		const MsgMainFrameChangePage * msg = dynamic_cast<const MsgMainFrameChangePage*>(message);
		if(msg)
		{
			if(msg->needPopAllPage)
				popAllPage();
			showPage(msg->pageName);
		}
	}
	else if(message->getTypeId() == MSG_MAINFRAME_PUSHPAGE)
	{
		const MsgMainFramePushPage * msg = dynamic_cast<const MsgMainFramePushPage*>(message);
		if(msg)
		{
			pushPage(msg->pageName);
		}
	}
	else if(message->getTypeId() == MSG_MAINFRAME_POPPAGE)
	{
		const MsgMainFramePopPage * msg = dynamic_cast<const MsgMainFramePopPage*>(message);
		if(msg)
		{
			popPage(msg->pageName);
		}
	}
	else if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_GET_BOARDCAST_INFORET_S)
			{
				initLoadBoardCast();
			}
			else if(msg->opcode==OPCODE_GET_CONTINUE_LOGINRET_S)
			{
				pushPage("ContinueLoginPage");
			}
		}
	}
	else if(message->getTypeId() == MSG_MAINFRAME_COVERSHOW)
	{
		const MsgMainFrameCoverShow * msg = dynamic_cast<const MsgMainFrameCoverShow*>(message);
		if(msg)
		{

			showCover(msg->pageName);
		}
	}
	else if(message->getTypeId() == MSG_MAINFRAME_COVERHIDE)
	{
		hideCover();
	}
	else if(message->getTypeId() == MSG_MAINFRAME_MSGSHOW)
	{
		const MsgMainFrameMSGShow * msg = dynamic_cast<const MsgMainFrameMSGShow*>(message);
		if(msg)
		{

			showMsg();
		}
	}
}

// void MainFrame::_setContent( cocos2d::CCNode* child, bool zero )
// {
// 	cocos2d::CCNode * mNodeMid = dynamic_cast<CCNode*>(getVariable("mNodeMid"));
// 	cocos2d::CCNode * mNodeBack = dynamic_cast<CCNode*>(getVariable("mNodeBack"));
// 	cocos2d::CCNode * mNodeFront = dynamic_cast<CCNode*>(getVariable("mNodeFront"));
// 	
// 
// 	if(mContentNode && mContentNode->getChildByTag(TAG_CONTENT))
// 	{
// 		if(child == mContentNode->getChildByTag(TAG_CONTENT))
// 			return;
// 		else
// 			mContentNode->removeAllChildren();
// 	}
// 
// 	if(mContentNodeZero && mContentNodeZero->getChildByTag(TAG_CONTENT))
// 	{
// 		if(child == mContentNodeZero->getChildByTag(TAG_CONTENT))
// 			return;
// 		else
// 			mContentNodeZero->removeAllChildren();
// 	}
// 
// 	if(mContentNodeCover && mContentNodeCover->getChildByTag(TAG_CONTENT))
// 	{
// 		if(child == mContentNodeCover->getChildByTag(TAG_CONTENT))
// 			return;
// 		else
// 			mContentNodeCover->removeAllChildren();
// 	}
// 
// 	if(!child)
// 		return;
// 
// 	CustomPage* page = dynamic_cast<CustomPage*>(child);
// 	if(page)
// 	{
// 		if(page->getPageType() == CustomPage::PT_CONTENT)
// 		{
// 			mContentNodeZero->addChild(child,0,TAG_CONTENT);
// 		}
// 		else if(page->getPageType() == CustomPage::PT_COVER)
// 		{
// 			mContentNodeCover->addChild(child,0,TAG_CONTENT);
// 		}
// 	}
// 	else
// 	{
// 		if(zero && mContentNodeZero)
// 			mContentNodeZero->addChild(child,0,TAG_CONTENT);
// 		else if(!zero && mContentNode)
// 			mContentNode->addChild(child,0,TAG_CONTENT);
// 	}
// }

void MainFrame::showPage( const std::string& name )
{
	BasePage* page = CCBManager::Get()->getPage(name);
	if(!page)
		return;
	
	State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(page);
	if(sta)
	{
		page->load();

		m_pStateMachine->ChangeState(sta);
		//_setContent(page);
		mNodeBack->removeAllChildrenWithCleanup(true);//xinzheng 2013-06-04
		mNodeBack->addChild(page);
		if(mCurShowPage&&mCurShowPage!=page)
			mCurShowPage->unload();
		mCurShowPage = page;

	}
}

void MainFrame::pushPage( const std::string& name )
{
	PageList::iterator it=mPageList.find(name);
	if(it!=mPageList.end())
	{
		return;
	}
	BasePage* page = CCBManager::Get()->getPage(name);
	if(!page)
		return;

	page->load();

	mNodeMid->addChild(page);
	mPageList.insert(PageList::value_type(name,page));

	cocos2d::CCLayer* layer = CCLayer::create();
	//layer->setContentSize(CCDirector::sharedDirector()->getOpenGLView()->getVisibleSize());
	CCSize maxsize(738.0f,1152.0f);
	layer->setContentSize(maxsize);
	layer->setPosition(0,0);
	layer->setAnchorPoint(ccp(0,0));
	layer->setTouchEnabled(true);
	layer->setTouchMode(cocos2d::kCCTouchesOneByOne);
	page->addChild(layer, -1);

	State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(page);
	if(sta)
		m_pStateMachine->AddGlobalState(sta);//sta->Enter(this);
}

void MainFrame::popPage(const std::string& name)
{
	PageList::iterator itr = mPageList.find(name);
	if(itr != mPageList.end())
	{
		State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(itr->second);
		if(sta)
			m_pStateMachine->removeGlobalState(sta);//sta->Exit(this);

		if(itr->second)
			itr->second->unload();

		mNodeMid->removeChild(itr->second,false);
		mPageList.erase(itr);
	}
}

void MainFrame::popPage(BasePage* page)
{
	PageList::iterator itr = mPageList.begin();
	while(itr != mPageList.end())
	{
		if(itr->second == page)
		{
			State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(itr->second);
			if(sta)
				m_pStateMachine->removeGlobalState(sta);//sta->Exit(this);
			
			if(itr->second)
				itr->second->unload();

			mNodeMid->removeChild(itr->second,false);
			mPageList.erase(itr);
			return;
		}

		++itr;
	}
}

void MainFrame::popAllPage()
{
	PageList::iterator itr = mPageList.begin();
	while(itr != mPageList.end())
	{
		State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(itr->second);
		if(sta)
			m_pStateMachine->removeGlobalState(sta);//sta->Exit(this);
		
		if(itr->second)
			itr->second->unload();

		mNodeMid->removeChild(itr->second,false);
		++itr;
	}
	mPageList.clear();
}

bool MainFrame::isPageInList(BasePage* page)
{
	PageList::iterator itr = mPageList.begin();
	while(itr != mPageList.end())
	{
		if(itr->second == page)
			return true;

		++itr;
	}
	return false;
}

bool MainFrame::isPageInList(const std::string& name)
{
	PageList::iterator itr = mPageList.find(name);
	if(itr != mPageList.end())
		return true;

	return false;
}

void MainFrame::onMenuItemAction( const std::string& itemName ,cocos2d::CCObject* sender)
{
	if(itemName == "onButton1")
	{
		popAllPage();
		showPage("MainPage");
	}
	else if(itemName == "onButton2")
	{
		popAllPage();
		showPage("TeamPage");
	}
	else if(itemName =="onButton3")
	{
		popAllPage();
		showPage("CareerPage");
	}
	else if(itemName=="onButton4")
	{
		popAllPage();
		showPage("AdventurePage");
		//pushPage("PlayerInfPage");
	}
	else if(itemName=="onButton5")
	{
		if(ServerDateManager::Get()->getUserBasicInfo().level >=5)
		{
			popAllPage();
			showPage("ArenaPage");
		}
		else
		{
			MSG_BOX_LAN("@LevelForArena");
		}
	}
	else if(itemName == "onButton6")
	{
		_saveActivityGuideStatus("mPraise","m");
		popAllPage();
		//showPage("SettingsPage");
		//pushPage("RechargePage");
		//pushPage("FightPage");
		//MsgMainFrameFightStart msg;
		//MessageManager::Get()->sendMessage(&msg);
		showPage("MarketPage");

	}
	else if(itemName == "on18183")
	{
		std::string url ="http://bbs.18183.com/forum-mxhzw-1.html";
		libPlatformManager::getPlatform()->openBBS();

// 		CCApplication::sharedApplication()->purgeCachedData();
// 		CCDirector::sharedDirector()->purgeCachedData();
// 		CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
	}
	
	cocos2d::CCObject* viewNode = this->getVariable("mMainbuttons");
	cocos2d::extension::CCScrollView* view = viewNode?dynamic_cast<cocos2d::extension::CCScrollView*>(viewNode):0;
	static std::string buttonenable =  VaribleManager::getInstance()->getSetting("MainFrameScrollButEnable","","true");
	if(view && itemName == "onBottomR" && buttonenable == "true")
	{
		view->setContentOffsetInDuration(view->minContainerOffset(),0.3f);
		CCB_FUNC(this,"mBottomL",cocos2d::CCNode,setVisible(true));
		CCB_FUNC(this,"mBottomR",cocos2d::CCNode,setVisible(false));
	}
	else if(view && itemName == "onBottomL"  && buttonenable == "true")
	{
		view->setContentOffsetInDuration(view->maxContainerOffset(),0.3f);
		CCB_FUNC(this,"mBottomL",cocos2d::CCNode,setVisible(false));
		CCB_FUNC(this,"mBottomR",cocos2d::CCNode,setVisible(true));
	}
}

void MainFrame::load( void )
{
	loadCcbiFile("MainFrame.ccbi");
	mNodeMid = dynamic_cast<CCNode*>(getVariable("mNodeMid"));
	mNodeBack = dynamic_cast<CCNode*>(getVariable("mNodeBack"));
	mNodeFront = dynamic_cast<CCNode*>(getVariable("mNodeFront"));
	mNodeMsg = dynamic_cast<CCNode*>(getVariable("mNodeMsg"));
	mBroadcast=dynamic_cast<CCNode*>(getVariable("mBroadcast"));
	
	mNoTouch=dynamic_cast<CCLayer*>(getVariable("mNoTouch"));
	mNoTouch->setContentSize(CCDirector::sharedDirector()->getOpenGLView()->getVisibleSize());
	mNoTouch->setPosition(0,0);
	mNoTouch->setAnchorPoint(ccp(0,0));
	mNoTouch->setTouchEnabled(false);
	mNoTouch->setVisible(false);
	mNoTouch->setTouchMode(cocos2d::kCCTouchesOneByOne);
}

void MainFrame::showCover(const std::string& pagename)
{
	BasePage* page = CCBManager::Get()->getPage(pagename.c_str());
	if(!page)
		return;

	State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(page);
	if(sta)
	{
		hideCover();

		page->load();
		page->removeFromParent();
		m_pStateMachine->AddGlobalState(sta);
		//_setContent(page);		//mNodeFront->removeAllChildrenWithCleanup(false);
		cocos2d::CCLayer* layer = CCLayer::create();
		layer->setContentSize(CCDirector::sharedDirector()->getOpenGLView()->getVisibleSize());
		layer->setPosition(0,0);
		layer->setAnchorPoint(ccp(0,0));
		layer->setTouchEnabled(true);
		layer->setTouchMode(cocos2d::kCCTouchesOneByOne);
		page->addChild(layer, -1);
		mNodeFront->addChild(page);
		//mCurShowPage = page;
		mNodeMid->setVisible(false);
		mNodeBack->setVisible(false);
		mBroadcast->setVisible(false);
	}
}

void MainFrame::hideCover()
{
	if(mNodeFront && mNodeFront->getChildren() && mNodeFront->getChildren()->count()>0)
	{
		CCObject* obj = mNodeFront->getChildren()->objectAtIndex(0);
		State<MainFrame>* sta = obj?dynamic_cast<State<MainFrame>*>(obj):0;
		if(sta)
		{
			m_pStateMachine->removeGlobalState(sta);
		}

		BasePage* bp = obj?dynamic_cast<BasePage*>(obj):0;
		if(bp)
			bp->unload();

		mNodeFront->removeAllChildrenWithCleanup(true);//xinzheng 2013-06-04
	}

	mNodeMid->setVisible(true);
	mNodeBack->setVisible(true);
	mBroadcast->setVisible(true);
}

void MainFrame::showMsg()
{
	BasePage* page = MessageBoxPage::Get();
	if(!page)
		return;

	State<MainFrame>* sta = dynamic_cast<State<MainFrame>*>(page);
	if(sta)
	{
		m_pStateMachine->AddGlobalState(sta);
		//mNodeMsg->removeAllChildren();
		if(page->getParent() == NULL)
			mNodeMsg->addChild(page);
	}
}

void MainFrame::onPlatformLogout(libPlatform*)
{
    bool ok = true;
    if(!libPlatformManager::getPlatform()->getLogined())
        ok = false;
    if(libPlatformManager::getPlatform()->loginUin()!=GamePrecedure::Get()->getUin())
        ok = false;
    
    if(!ok)
    {
        std::string outStr = Language::Get()->getString("@RELOGIN_ERROR");
		libOS::getInstance()->registerListener(this);
        libOS::getInstance()->showMessagebox(outStr,101);
    }
}
void MainFrame::onMessageboxEnter(int tag)
{
    if(tag == 101)
        exit(0);
	else
		libOS::getInstance()->removeListener(this);
        //lib91::getInstance()->switchUsers();
}

MainFrame* MainFrame::getInstance()
{
	return dynamic_cast<MainFrame*>(CCBManager::Get()->getPage("MainFrame"));
}

CCScene* MainFrame::getRootSceneNode()
{
	return mScene;
}

void MainFrame::showNoTouch()
{
	mNoTouch->setVisible(true);
	mNoTouch->setTouchEnabled(true);
}

void MainFrame::hideNoTouch()
{
	mNoTouch->setVisible(false);
	mNoTouch->setTouchEnabled(false);
}
