
#include "stdafx.h"

#include "LoadingFrame.h"
#include "GameMessages.h"
#include "Language.h"
#include "lib91.h"
#include "SeverConsts.h"
#include "GameMaths.h"
#include "ServerDateManager.h"
#include "InstructionManager.h"
#include "SoundManager.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include "libOS.h"
#include "ActiveCodePage.h"
#include "BlackBoard.h"
#include "TimeCalculator.h"
USING_NS_CC;
USING_NS_CC_EXT;

REGISTER_PAGE("LoadingFrame",LoadingFrame);


#ifdef PROJECTIOSLOCAL
const std::string _platfrom="version_ios_local.cfg";
#endif

#ifdef PROJECT91
const std::string _platfrom="version_ios_91.cfg";
#endif

#ifdef PROJECTPP
const std::string _platfrom="version_ios_pp.cfg";
#endif

#ifdef PROJECTAG
const std::string _platfrom="version_ios_ag.cfg";
#endif

#ifdef PROJECTTB
const std::string _platfrom="version_ios_tb.cfg";
#endif

#ifdef PROJECTITools
const std::string _platfrom="version_ios_itools.cfg";
#endif

#ifdef PROJECT91Debug
const std::string _platfrom="version_debug.cfg";
#endif

#ifdef WIN32
const std::string _platfrom="version_win32.cfg";
#endif	

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
const std::string _platfrom="version_android.cfg";
#endif	


LoadingFrame::LoadingFrame(void)
{
}


LoadingFrame::~LoadingFrame(void)
{
}


void LoadingFrame::Enter( GamePrecedure* )
{

    CCLOG("enter loading frame");
	
    mIsUpdateDown = false;
	mLogined = false;
	m91checked = false;
	MessageManager::Get()->regisiterMessageHandler(MSG_LOADINGFRAME_ENTERGAME,this);
	//
	libOS::getInstance()->registerListener(this);
	/*
		创建与（apk|ipa）loader程序的衔接
	*/
	libPlatformManager::getPlatform()->registerListener(this);
    CCLOG("init severConst...");
    SeverConsts::Get()->init(_platfrom);
    CCLOG("init lib91...");
	/*
		这个Platform()->init起码在Android无作用
	*/
	
    std::string privateLogin = VaribleManager::Get()->getSetting("privateLogin","","false");
    if(privateLogin != "true")
        libPlatformManager::getPlatform()->init();
    else
        libPlatformManager::getPlatform()->init(true);

    //lib91::getInstance()->showInputbox(true);
    //lib91::getInstance()->openURL("http://www.baidu.com");
    //lib91::getInstance()->setWaiting(true);
    
    CCLOG("init lib91 done...");
	
	mScene = CCScene::create();
	mScene->retain();

	mScene->addChild(this);

    CCLOG("load ccbi file");
	
	load();
    CCB_FUNC(this, "mEnterButton", CCNode, setVisible(false));
    CCB_FUNC(this, "mLoginNode", CCNode, setVisible(false));

	showUpdateChecking();
	
	//modify by dylan at 20130623 hidden vesion info
	CCB_FUNC(this,"mVer",cocos2d::CCLabelBMFont,setVisible(false));
	CCB_FUNC(this,"mVer",cocos2d::CCLabelBMFont,setString(""));
    //CCB_FUNC(this,"mVer",cocos2d::CCLabelBMFont,setString(SeverConsts::Get()->getVersion().c_str()));
    CCLOG("start running scene");
	
	CCDirector::sharedDirector()->setDepthTest(true);
	// run
	if(cocos2d::CCDirector::sharedDirector()->getRunningScene())
		cocos2d::CCDirector::sharedDirector()->replaceScene(mScene);
	else
		cocos2d::CCDirector::sharedDirector()->runWithScene(mScene);
    
    CCLOG("loading frame enter done");
	
}

void LoadingFrame::Exit( GamePrecedure* )
{
	libOS::getInstance()->removeListener(this);
	/*
		解除与loader的衔接
	*/
    libPlatformManager::getPlatform()->removeListener(this);
	PacketManager::Get()->removePacketHandler(this);
	MessageManager::Get()->removeMessageHandler(this);
	mScene->release();
	unload();
}

void LoadingFrame::Execute( GamePrecedure* gp)
{
    static bool _isFirstIn=true;
	if(_isFirstIn)
	{
		CCLOG("start play music...");

		SoundManager::Get()->playLoadingMusic();
		_isFirstIn=false;
	}
	
	SeverConsts::Get()->update(gp->getFrameTime());

    bool enterShown = false;
    if(getVariable("mLoginNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mLoginNode"));
		enterShown = sever->isVisible();
	}
    
	static SeverConsts::CHECK_STATE checkstate = SeverConsts::CS_NOT_STARTED ;
	
	if(checkstate!=SeverConsts::Get()->checkUpdateInfo())
	{
		checkstate = SeverConsts::Get()->checkUpdateInfo();

		if(	checkstate == SeverConsts::CS_NEED_UPDATE &&
			SeverConsts::Get()->checkUpdateState() == SeverConsts::US_NOT_STARTED)
		{
			//showUpdateUpdating();
			//SeverConsts::Get()->updateResources();
		
			//libOS::getInstance()->registerListener(this);//挪到enter加了

			std::string strMsg;
			float fsizem = SeverConsts::Get()->getNeedUpdateTotalBytes();
			char szTmp[256] = {0};
			if (fsizem < 1024u )
				sprintf(szTmp, "(1Kb)");
			else if (fsizem < 1024u*1024u)
				sprintf(szTmp, "(%dKb)", (int)fsizem/1024);
			else
				sprintf(szTmp, "(%fM)", fsizem/1024.f/1024.f);
			//
            if(libOS::getInstance()->getNetWork() == ReachableViaWWAN)
			{
				strMsg = Language::Get()->getString("@LOADINGFRAME_NeedUpdate3G");
			}
            else
			{
				strMsg = Language::Get()->getString("@LOADINGFRAME_NeedUpdate");
			}
			//strMsg.append(szTmp);

			std::string strUpdateMsg = SeverConsts::Get()->getNeedUpdateMsg();
			if (!strUpdateMsg.empty())
			{
				//strMsg = strUpdateMsg;
				strUpdateMsg.append("\n");
				strUpdateMsg.append(strMsg);
			}
			else
				strUpdateMsg = strMsg;

			libOS::getInstance()->showMessagebox(strUpdateMsg, 100);
		}
		else if(checkstate == SeverConsts::CS_NEED_STORE_UPDATE)
		{
			//jump to update page
			//lib91::getInstance()->openURL(SeverConsts::Get()->getInStoreUpdateAddress());
            //Language::Get()->getString("@LOADINGFRAME_NeedUpdate")

			std::string strMsg = SeverConsts::Get()->getDirectDownloadMsg();
			strMsg.append("\n");
			//
			if(libOS::getInstance()->getNetWork() == ReachableViaWWAN)
			{
				strMsg.append(Language::Get()->getString("@LOADINGFRAME_NeedUpdate3G"));
			}
			else
			{
				strMsg.append(Language::Get()->getString("@LOADINGFRAME_NeedUpdate"));
			}

			libOS::getInstance()->showMessagebox(strMsg,110);
			
			SeverConsts::Get()->clearVersion();
		}
		else if(checkstate == SeverConsts::CS_FAILED)
		{
			showUpdateCheckingFailed();
		}
	}
    if(checkstate == SeverConsts::CS_OK && m91checked && !enterShown)
        showUpdateDone();
    
	static SeverConsts::UPDATE_STATE updatestate = SeverConsts::US_NOT_STARTED;

	if( updatestate!=SeverConsts::Get()->checkUpdateState())
	{
		updatestate = SeverConsts::Get()->checkUpdateState();
		if(updatestate == SeverConsts::US_CHECKING)
		{
			showUpdateFileChecking();
		}
		else if(updatestate == SeverConsts::US_DOWNLOADING)
		{
			showUpdateUpdating();
		}
		else if(updatestate == SeverConsts::US_FAILED)
		{
			showUpdateFailed();
		}
	}
    if(updatestate == SeverConsts::US_OK && m91checked && !enterShown)
	{
		showPersent(1.0f);
        showUpdateDone();
	}
    
	if(updatestate == SeverConsts::US_DOWNLOADING)
	{
		unsigned long total = SeverConsts::Get()->getUpdateTotalCount();
		unsigned long done = SeverConsts::Get()->getUpdatedCount();
		if(total>0)
		{
			float per = (float)done/(float)total;
			std::string _sizeTip="";
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(done/1024));
			replaceList.push_back(StringConverter::toString(total/1024));
			showPersent(per,GameMaths::replaceStringWithStringList(Language::Get()->getString("@UpdateSizeInfo"),&replaceList));
		}
		
	}
    //CCLOG("done execute");
	
}



void LoadingFrame::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_LOADINGFRAME_ENTERGAME)
	{
		GamePrecedure::Get()->enterMainMenu();
	}
}


void LoadingFrame::onLogin(libPlatform* lib, bool success, const std::string& log)
{
    if(success)
    {
        updateLoginName();
        //if(lib91::getInstance()->getLogined() && !mLogined && m91checked)
        {
            mLogined = true;
            showEnter();
        }
        
        const std::string address = SeverConsts::Get()->getAnnouncement();
        if(!address.empty())
        {
            if(libPlatformManager::getInstance()->getPlatform()->getClientChannel()!="ITO")
            {//itools sdk hide webkit notice because has bug! 
                BulletinBoardPage* page = BulletinBoardPage::getInstance();
                page->init(120, 70, 300, 500, this);
                page->show(address);
            }
        }
    }
}

void LoadingFrame::onPlatformLogout(libPlatform*)
{
    if(!libPlatformManager::getPlatform()->getLogined())
    {
        //lib91::getInstance()->login();
        CCLabelBMFont* eb = dynamic_cast<CCLabelBMFont*>(getVariable("mLoginName"));
		if(eb)
			eb->setString("Login");
        CCMessageBox(Language::Get()->getString("@LOADINGFRAME_HAVENOT_LOGIN").c_str(),"");
    }
};

void LoadingFrame::onUpdate(libPlatform*,bool ok, std::string msg)
{
    if(!ok)
    {
		CCLog("LoadingFrame::onUpdate failed");
        //SeverConsts::Get()->clearVersion();
        m91checked = false;
        libOS::getInstance()->showMessagebox(msg);
    }
    else
    {
		if (!m91checked)
		{
			CCLog("LoadingFrame::onUpdate success");
			m91checked = true;
			SeverConsts::Get()->start();
		}
    }
}

void LoadingFrame::onMessageboxEnter(int tag)
{
    if(tag == 100)
    {
		//libOS::getInstance()->removeListener(this);//xinzheng remove, cause of remove iterator in stl container loop
        SeverConsts::Get()->updateResources();
    }
	else if (tag == 110)
	{
		//xinzheng 2013-07-15
#if defined(ANDROID)
		libOS::getInstance()->openURL(SeverConsts::Get()->getDirectDownloadUrl());
#else
        if(VaribleManager::Get()->getSetting("useInsideAppUpdate")!="true")
            libOS::getInstance()->openURL(SeverConsts::Get()->getInStoreUpdateAddress());
        else
            libPlatformManager::getInstance()->getPlatform()->updateApp();

#endif
		//
	}
}
void LoadingFrame::onMenuItemAction( const std::string& itemName ,cocos2d::CCObject* sender)
{
	if(itemName == "onSever")
	{
		showSevers(true);
	}
	else if(itemName =="onEnter")
	{
        if(!libPlatformManager::getPlatform()->getLogined())
        {
            libPlatformManager::getPlatform()->login();
            return;
        }

#ifdef WIN32
// 		if(MessageBox(0,L"Connect to game sever?",L"Debug", MB_OKCANCEL) == IDCANCEL)
// 		{
// 			GamePrecedure::Get()->enterMainMenu();
// 			return;
// 		}
#endif
		showEnterAndLoginingIn();
		SeverConsts::SEVERLIST::const_iterator it = SeverConsts::Get()->getSeverList().find(mSelectedSeverID);
		if(it!=SeverConsts::Get()->getSeverList().end())
		{

			PacketManager::Get()->init(it->second->address,it->second->port);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && defined(Macro_AndroidTestJiHuoMa)
			PacketManager::Get()->regisiterPacketHandler(OPCODE_ACTIVE_CODERET_S,this);
#endif
			ServerDateManager::Get()->setSeverID(mSelectedSeverID);
			PacketManager::Get()->regisiterPacketHandler(OPCODE_PLAYER_LOGINRET_S,this);
			OPLogin loginPack;
			loginPack.set_pwd("pswd");
			loginPack.set_version(1);
			
			GamePrecedure::Get()->setServerID(mSelectedSeverID);
			GamePrecedure::Get()->setAlertServerUpdating(false);
			InstructionManager::Get()->setConnectedServerState(mSelectedSeverID,false);

			std::string uin = libPlatformManager::getPlatform()->loginUin();
            std::string platformInfo = libPlatformManager::getPlatform()->getPlatformInfo();
            std::string deviceID = libOS::getInstance()->getDeviceID();
			 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			// Samsung i9100's audio driver has a bug , it can not accept too much effects concurrently.
			if(((platformInfo.find("samsung")!=std::string::npos)||(platformInfo.find("SAMSUNG")!=std::string::npos))&&(platformInfo.find("9100")!=std::string::npos))
			{
				BlackBoard::Get()->isSamSungi9100Audio = true;
				//std::vector<std::string> vec = GameMaths::tokenize(platformInfo,"_");
				//if(vec.size()>=2)
				//{
					//if(StringConverter::parseInt(vec.at(1)) >=14)
					//{
					//	BlackBoard::Get()->isSamSungi9100Audio = false;
					//}
					//else
					//{
					//	BlackBoard::Get()->isSamSungi9100Audio = true;
					//}
				//}
				//std::vector<std::string> vecTemp;
				//vec.swap(vecTemp);
				
			}
#endif
			GamePrecedure::Get()->setUin(uin);

			loginPack.set_clientguidefinished(InstructionManager::Get()->loadLocalInstructionStep()>InstructionManager::NAME_STEP);//need uin

			
			if(!uin.empty())
                loginPack.set_puid(uin);
            else
                loginPack.set_puid("111");
            
			if(!deviceID.empty())
				loginPack.set_deviceid(deviceID);

			if(!platformInfo.empty())
				loginPack.set_platform(platformInfo);

			PacketManager::Get()->sendPakcet(OPCODE_PLAYER_LOGIN_C,&loginPack);
			//caculate the time to kill game when player enter background
			if(VaribleManager::Get()->getSetting("ExitGameTime")!="")
			{
				TimeCalculator::getInstance()->createTimeCalcultor("ExitGameTime",StringConverter::parseInt(VaribleManager::Get()->getSetting("ExitGameTime")));
			}
		}
	}
    else if(itemName =="onLogin")
	{
		CCLOG("onLogin clicked");
        mLogined = false;
        libPlatformManager::getPlatform()->switchUsers();
	}
	CCLOG("LoadingFrame::onMenuItemAction %s: ", itemName.c_str());
}

void LoadingFrame::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	mSelectedSeverID = tag;
	_updateLocalSeverId();//add by dylan at 20130711 HZW-1623
	updateSeverName();

	showSevers(false);
}

void LoadingFrame::_updateLocalSeverId()
{
	cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey("DefaultSeverID",mSelectedSeverID);
	cocos2d::CCUserDefault::sharedUserDefault()->flush();
}

void LoadingFrame::load( void )
{
	loadCcbiFile("LoadingFrame.ccbi");
}

void LoadingFrame::showSevers(bool _show)
{
	if(getVariable("mSeverNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mSeverNode"));
		if(sever) 
		{
			
			sever->setVisible(_show);

			if(_show && getVariable("mSeverList"))
			{
				CCScrollView* severlist = dynamic_cast<CCScrollView*>(getVariable("mSeverList"));

				CCNode* attachNode = CCNode::create();

				CCBContainer * node = CCBContainer::create();
				node->loadCcbiFile("LoadingFrameSeverList.ccbi");
				
				if(node->getVariable("mRecent"))
				{
					CCNode* recentNode = dynamic_cast<CCNode*>(node->getVariable("mRecent"));
					SeverConsts::SEVERLIST::const_iterator it = SeverConsts::Get()->getSeverList().find(mSelectedSeverID);
					if(it==SeverConsts::Get()->getSeverList().end())
						it = SeverConsts::Get()->getSeverList().find(SeverConsts::Get()->getSeverDefaultID());

					if(recentNode && it!=SeverConsts::Get()->getSeverList().end())
					{
						CCBContainer* button = CCBContainer::create();
						button->setListener(this,it->second->id);
						button->loadCcbiFile("LoadingFrameSever.ccbi");
						if(button->getVariable("mSeverName"))
						{
							CCLabelBMFont* str = dynamic_cast<CCLabelBMFont*>(button->getVariable("mSeverName"));
							if(str)	str->setString(it->second->name.c_str());
						}
						CCNode* fullSever = 0;
						CCNode* newSever = 0;
						if(button->getVariable("mSeverFull"))
							fullSever = dynamic_cast<CCNode*>(button->getVariable("mSeverFull"));
						if(button->getVariable("mSeverNew"))
							newSever = dynamic_cast<CCNode*>(button->getVariable("mSeverNew"));

						if(fullSever) fullSever->setVisible(it->second->state == SeverConsts::SS_FULL);
						if(newSever) newSever->setVisible(it->second->state == SeverConsts::SS_NEW);
						
						recentNode->addChild(button);
					}
				}
				int countID = 0;
				float singleButtonHeight = 0;
				if(node->getVariable("mAll1") && node->getVariable("mAll2"))
				{
					CCNode* allNode1 = dynamic_cast<CCNode*>(node->getVariable("mAll1"));
					CCNode* allNode2 = dynamic_cast<CCNode*>(node->getVariable("mAll2"));
					SeverConsts::SEVERLIST::const_reverse_iterator it = SeverConsts::Get()->getSeverList().rbegin();
					
					for(;it!=SeverConsts::Get()->getSeverList().rend();++it)
					{
						CCBContainer* button = CCBContainer::create();
						button->setListener(this,it->second->id);
						button->loadCcbiFile("LoadingFrameSever.ccbi");
						if(button->getVariable("mSeverName"))
						{
							CCLabelBMFont* str = dynamic_cast<CCLabelBMFont*>(button->getVariable("mSeverName"));
							if(str)	str->setString(it->second->name.c_str());
						}
						CCNode* fullSever = 0;
						CCNode* newSever = 0;
						if(button->getVariable("mSeverFull"))
							fullSever = dynamic_cast<CCNode*>(button->getVariable("mSeverFull"));
						if(button->getVariable("mSeverNew"))
							newSever = dynamic_cast<CCNode*>(button->getVariable("mSeverNew"));

						if(fullSever) fullSever->setVisible(it->second->state == SeverConsts::SS_FULL);
						if(newSever) newSever->setVisible(it->second->state == SeverConsts::SS_NEW);

						if(countID %2 ==0)
							allNode1->addChild(button);
						else
							allNode2->addChild(button);
						singleButtonHeight = button->getContentSize().height;
						button->setPosition(ccp(0,-(singleButtonHeight)*(countID-countID%2)*0.5f));
						countID++;
					}
				}
				attachNode->addChild(node);
				node->setPosition(0,countID*0.5f*singleButtonHeight);
				CCSize size;
				size.width = node->getContentSize().width;
				size.height = node->getContentSize().height+ countID*0.5f*singleButtonHeight;
				attachNode->setContentSize(size);
				severlist->setContainer(attachNode);
				severlist->setContentSize(size);
				severlist->setContentOffset(ccp(0,severlist->getViewSize().height-size.height));
			}
		}
	}
}

void LoadingFrame::showUpdateChecking()
{
	if(getVariable("mPostUpdateNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPostUpdateNode"));
		if(sever) sever->setVisible(false);
	}
	if(getVariable("mPersentageNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPersentageNode"));
		if(sever) sever->setVisible(false);
	}
	if(getVariable("mWaitingNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mWaitingNode"));
		if(sever) sever->setVisible(true);
	}
	if(getVariable("mUpdateDescription"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mUpdateDescription"));
		if(sever)
		{
			sever->setString(Language::Get()->getString("@UpdateDescriptionChecking").c_str(),true);
		}
	}
}
void LoadingFrame::showUpdateCheckingFailed()
{
	if(getVariable("mUpdateDescription"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mUpdateDescription"));
		if(sever)
		{
			sever->setString(Language::Get()->getString("@UpdateDescriptionCheckingFailed").c_str(),true);
		}
	}
	cocos2d::CCMessageBox(Language::Get()->getString("@UpdateDescriptionCheckingFailed").c_str(),"");
    

}
void LoadingFrame::showUpdateFileChecking()
{

	if(getVariable("mUpdateDescription"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mUpdateDescription"));
		if(sever)
		{
			sever->setString(Language::Get()->getString("@UpdateDescriptionUpdatingCheck").c_str(),true);
		}
	}
}

void LoadingFrame::showUpdateUpdating()
{
	showPersent(0);
	if(getVariable("mPersentageNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPersentageNode"));
		if(sever) sever->setVisible(true);
	}
	
	if(getVariable("mUpdateDescription"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mUpdateDescription"));
		if(sever)
		{
			sever->setString(Language::Get()->getString("@UpdateDescriptionUpdating").c_str(),true);
		}
	}
}
void LoadingFrame::showUpdateFailed()
{
	std::list<std::string> replaceList;
	replaceList.push_back(SeverConsts::Get()->getFailedName());
	std::string _errStr=GameMaths::replaceStringWithStringList(Language::Get()->getString("@UpdateDescriptionupdateFailed"),&replaceList);
	if(getVariable("mUpdateDescription"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mUpdateDescription"));
		if(sever)
		{
			sever->setString(_errStr.c_str(),true);
		}
	}
	cocos2d::CCMessageBox(_errStr.c_str(),"");
}
void LoadingFrame::showUpdateDone()
{
    mIsUpdateDown = true;
    //lib91::getInstance()->setWaiting(false);
	showSevers(false);

	mSelectedSeverID = getDefaultSeverID();
	updateSeverName();

	if(getVariable("mPostUpdateNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPostUpdateNode"));
		if(sever) sever->setVisible(true);
	}
	if(getVariable("mPersentageNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPersentageNode"));
		if(sever) sever->setVisible(false);
	}
	if(getVariable("mWaitingNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mWaitingNode"));
		if(sever) sever->setVisible(false);
	}
	
	if(getVariable("mLoginNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mLoginNode"));
		if(sever) sever->setVisible(true);
	}
    if(getVariable("mEnterButton"))
	{
		CCNode* eb = dynamic_cast<CCNode*>(getVariable("mEnterButton"));
		if(eb)
			eb->setVisible(true);
	}
	//First set up game and show servers Dialog
	if(cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("DefaultSeverID",-1)<0)
	{
		showSevers(true);
	}
	
	if(!mLogined)
		libPlatformManager::getPlatform()->login();
}
void LoadingFrame::showEnter()
{
	//平台登录结果回来了，可选区 进入游戏
	CCLOG("LoadingFrame::showEnter");
	if(getVariable("mLoginNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mLoginNode"));
		if(sever) sever->setVisible(true);
	}
	if(getVariable("mEnterButton"))
	{
		CCNode* eb = dynamic_cast<CCNode*>(getVariable("mEnterButton"));
		if(eb)
			eb->setVisible(true);
	}
	
	updateLoginName();
}

void LoadingFrame::showPersent( float persentage,std::string sizeTip )
{
	if(getVariable("mPersentage"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPersentage"));
		if(persentage>1.0f)
			persentage = 1.0f;
		if(persentage<0.06f)
			persentage = 0.06f;

		if(sever) 
			sever->setScaleX(persentage);
	}
	if(getVariable("mPersentageTxt"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mPersentageTxt"));
		char perTxt[64];
		sprintf(perTxt,"%d%%   %s",(int)(persentage*100),sizeTip.c_str());
		if(sever) 
			sever->setString(perTxt);
	}
}

void LoadingFrame::showEnterAndLoginingIn()
{
	if(getVariable("mPostUpdateNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPostUpdateNode"));
		if(sever) sever->setVisible(false);
	}
	if(getVariable("mPersentageNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mPersentageNode"));
		if(sever) sever->setVisible(false);
	}
	if(getVariable("mWaitingNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mWaitingNode"));
		if(sever) sever->setVisible(true);
	}
	if(getVariable("mLoginNode"))
	{
		CCNode* sever = dynamic_cast<CCNode*>(getVariable("mLoginNode"));
		if(sever) sever->setVisible(false);
	}
	if(getVariable("mUpdateDescription"))
	{
		CCLabelBMFont* sever = dynamic_cast<CCLabelBMFont*>(getVariable("mUpdateDescription"));
		if(sever)
		{
			sever->setString(Language::Get()->getString("@UpdateDescriptionupLoginingIn").c_str(),true);
		}
	}
}

int LoadingFrame::getDefaultSeverID()
{
	return cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("DefaultSeverID",SeverConsts::Get()->getSeverDefaultID());
}

void LoadingFrame::updateSeverName()
{
	if(getVariable("mSeverName1") && mSelectedSeverID!=-1)
	{
		CCLabelBMFont* eb = dynamic_cast<CCLabelBMFont*>(getVariable("mSeverName1"));
		SeverConsts::SEVERLIST::const_iterator it = SeverConsts::Get()->getSeverList().find(mSelectedSeverID);
		if(eb && it!=SeverConsts::Get()->getSeverList().end())
			eb->setString(it->second->name.c_str());
	}
}

void LoadingFrame::updateLoginName()
{
    std::string nickgame = libPlatformManager::getPlatform()->nickName();
	CCLOG("LoadingFrame::updateLoginName: ", nickgame.c_str());
	if(getVariable("mLoginName") && !nickgame.empty())
	{
		CCLabelBMFont* eb = dynamic_cast<CCLabelBMFont*>(getVariable("mLoginName"));
		if(eb)
			eb->setString(nickgame.c_str());
	}
}

void LoadingFrame::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	if(opcode == _getOpcode())
	{
		const OPLoginRet * logret = dynamic_cast<const OPLoginRet*>(packet);
		if(logret && logret->status() == 1)
		{
			SeverConsts::SEVERLIST::const_iterator it = SeverConsts::Get()->getSeverList().find(mSelectedSeverID);
			InstructionManager::Get()->setConnectedServerState(it->second->id,logret->serverguidefinished());

			GamePrecedure::Get()->preEnterMainMenu();
			_updateLocalSeverId();//add by dylan at 20130711 HZW-1623
			MsgLoadingFrameEnterGame enterGameMsg;
			MessageManager::Get()->sendMessage(&enterGameMsg);
			PacketManager::Get()->removePacketHandler(this);
			GamePrecedure::Get()->setAlertServerUpdating(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			libPlatformManager::getPlatform()->notifyEnterGame();
#endif
		}
		else if(logret && logret->status() == 3)
		{
			std::string out = Language::Get()->getString("@LoadingFrameSeversFull");
			CCMessageBox(out.c_str(),"");
		}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && defined(Macro_AndroidTestJiHuoMa)
		else if(logret && logret->status() == 4)
		{
			ActiveCodePage* acPage = ActiveCodePage::create();
			acPage->setScale(0.5);
			acPage->setPosition((mScene->getContentSize().width*(mScene->getScaleX())/2)-(acPage->getContentSize().width*(acPage->getScaleX())/2),0);
			mScene->addChild(acPage);
			acPage->setTag(999);
			cocos2d::CCLayer* layer = CCLayer::create();
			//layer->setContentSize(CCDirector::sharedDirector()->getOpenGLView()->getVisibleSize());
			CCSize maxsize(738.0f,1152.0f);
			layer->setContentSize(maxsize);
			layer->setPosition(0,0);
			layer->setAnchorPoint(ccp(0,0));
			layer->setTouchEnabled(true);
			layer->setTouchMode(cocos2d::kCCTouchesOneByOne);
			acPage->addChild(layer, -1);
		}
#endif
		else
		{
			//std::string out = Language::Get()->getString("@LoadingFrameSeversConnectionPasswordWrong");
			//CCMessageBox(out.c_str(),"error");
		}
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && defined(Macro_AndroidTestJiHuoMa)
	else if(opcode == OPCODE_ACTIVE_CODERET_S)
	{
		const OPActiveCodeRet * ret = dynamic_cast<const OPActiveCodeRet*>(packet);
		if(ret->status()==1)
		{
			mScene->removeChildByTag(999,true);
			std::string out = Language::Get()->getString("@ActiveCodeSuccess");
			CCMessageBox(out.c_str(),"success");
		}
		else if(ret->status()==2)
		{
			std::string out = Language::Get()->getString("@ActiveCodeBeenUsed");
			CCMessageBox(out.c_str(),"error");
		}
		else
		{
			std::string out = Language::Get()->getString("@WrongActiveCode");
			CCMessageBox(out.c_str(),"error");
		}
	}
#endif
}

// void LoadingFrame::onSendPacketFailed( const int opcode )
// {
// 	std::string out = Language::Get()->getString("@LoadingFrameSeversConnectionFailed");
// 	CCMessageBox(out.c_str(),"error");
// }

void LoadingFrame::onConnectFailed( std::string ip, int port )
{
	std::string out = Language::Get()->getString("@LoadingFrameSeversConnectionFailed");
	CCMessageBox(out.c_str(),"error");
}

CCScene* LoadingFrame::getRootSceneNode()
{
	return mScene;
}

void LoadingFrame::onLoadingTimeOut()
{
    BulletinBoardPage::getInstance()->close();
}

void LoadingFrame::onBtnAction()
{
    BulletinBoardPage::getInstance()->close();
}
