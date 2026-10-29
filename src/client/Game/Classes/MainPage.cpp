
#include "stdafx.h"

#include "MainPage.h"
#include "GameMessages.h"
#include "Language.h"
#include "AsyncSprite.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "Disciple.h"
#include "BattleArrayInfo.h"
#include "MessageBoxPage.h"
#include "BlackBoard.h"

#include "cocos-ext.h"
#include "cocos2d.h"
#include "PopupManager.h"
//test code
#include "lib91.h"
//test end

USING_NS_CC;
USING_NS_CC_EXT;

REGISTER_PAGE("MainPage",MainPage)


MainPage::MainPage(void)
:mExpbarInitScale(-1.0f)
{
	mFrameBtnList.clear();
}


MainPage::~MainPage(void)
{
}

void MainPage::Enter( MainFrame* frame)
{
	CCB_FUNC(this,"mGuideBuyEquip",CCMenuItemImage,setVisible(false));
	CCB_FUNC(this,"mGuideBuyExp",CCMenuItemImage,setVisible(false));
	_refreshTitle();
	_refreshBattle();
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	CCB_FUNC(this,"mGuideLuFei",CCNode,setVisible(false));
}

void MainPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllFrameBtn();
}

void MainPage::Execute( MainFrame* )
{
	static float durationTime=0;
	durationTime+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>1.0f)
	{
		durationTime -=1.0f;
		_refreshPageSymbol();
	}
}

void MainPage::_refreshPageSymbol()
{
	bool _showBuyEquipGuide=PopupManager::Get()->checkShowBuyEquip();
	if(_showBuyEquipGuide)
	{
		CCB_FUNC(this,"mGuideBuyEquip",CCMenuItemImage,setVisible(true));
	}
	else
	{
		CCB_FUNC(this,"mGuideBuyEquip",CCMenuItemImage,setVisible(false));
	}
	bool _showBuyExpGuide=PopupManager::Get()->checkShowBuyExp();
	if(_showBuyExpGuide)
	{
		CCB_FUNC(this,"mGuideBuyExp",CCMenuItemImage,setVisible(true));
	}
	else
	{
		CCB_FUNC(this,"mGuideBuyExp",CCMenuItemImage,setVisible(false));
	}
	bool _showAdvGuideContinue=PopupManager::Get()->checkShowContinueAd();
	if(_showAdvGuideContinue)
	{
		CCB_FUNC(this,"mGuideLuFei",CCNode,setVisible(true));
	}
	else
	{
		CCB_FUNC(this,"mGuideLuFei",CCNode,setVisible(false));
	}
}

void MainPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S||info->opcode==OPCODE_GET_USER_TOOL_INFORET_S||info->opcode==OPCODE_GUIDE_BUY_TOOLRET_S||info->opcode==OPCODE_REWARD_CONTINUE_LOGINRET_S)
		{
			_refreshTitle();
			//_updateUserInfo();
		}
		else if(info->opcode==OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S)
		{
			_refreshBattle();
		}
		else if(info->opcode==OPCODE_GET_USER_SYSMSG_INFORET_S)
		{
			_refreshTitle();
		}
	}
}

void MainPage::load( void )
{
	loadCcbiFile("MainScene.ccbi");
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpbar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
	CCB_FUNC(this,"mGuideBuyEquip",CCMenuItemImage,setVisible(false));
	CCB_FUNC(this,"mGuideBuyExp",CCMenuItemImage,setVisible(false));
}

void MainPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName == "onFrameButton")
	{
		if(tag!=999)
		{
			BlackBoard::Get()->TeamShowIndex = tag;
			MsgMainFrameChangePage msg;
			msg.pageName = "TeamPage";
			MessageManager::Get()->sendMessage(&msg);
		}else
		{
			unsigned int nextUnlockLevel = BattleArrayInfo::Get()->getNextUnlockLevel();
			if(nextUnlockLevel>0)
			{
				char buff[128];
				std::string s = Language::Get()->getString("@NeedUnlockLevel");
				sprintf(buff,s.c_str(),nextUnlockLevel);
				MSG_BOX(buff);
			}
			else
			{
				MSG_BOX(Language::Get()->getString("@NeedUnlock").c_str());
			}
		}
		
	}
}

void MainPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onMemberPage")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "DisciplePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	if(itemName == "onEquipPage")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "EquipPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onSkillPage")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "SkillPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onSysMsgButton")
	{
		MsgMainFrameChangePage msga;
		msga.pageName = "SysMsgPage";
		MessageManager::Get()->sendMessage(&msga);
	}
	else if(itemName=="onLinkmanButton")
	{
		if(StringConverter::parseInt(VaribleManager::Get()->getSetting("LinkManOpenStatus"))==0)
		{
			MSG_BOX_LAN("@MAINPAGE_COMINGSOON");
		}else
		{	
 			MsgMainFrameChangePage msg;
 			msg.pageName = "FriendsPage";
 			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onSettingPage")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "SettingsPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onBagButton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "PackagePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onChatButton1")
	{
		if(StringConverter::parseInt(VaribleManager::Get()->getSetting("ChatOpenStatus"))==0)
		{
			MSG_BOX_LAN("@MAINPAGE_COMINGSOON");
		}else
		{	
			MsgMainFrameChangePage msg;
			msg.pageName = "ChatPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onChatButton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "ChatPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onHandBookPage")
	{
		if(StringConverter::parseInt(VaribleManager::Get()->getSetting("HandbookOpenStatus"))==0)
		{
			MSG_BOX_LAN("@MAINPAGE_COMINGSOON");
		}else
		{

			MsgMainFrameChangePage msg;
			msg.pageName = "HandbookPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName == "onHandbookPage")//notice! b is not B
	{
		MsgMainFrameChangePage msg;
 		msg.pageName = "HandbookPage";
 		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onAdventurePage")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "AdventurePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onJianghuPage")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "CareerPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onArenaPage")
	{
		if(ServerDateManager::Get()->getUserBasicInfo().level >=5)
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "ArenaPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			MSG_BOX_LAN("@LevelForArena");
		}
		
	}
	else if(itemName=="onTeamBufferPage")
	{
		const UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
		if(info.level<5)
		{
			MSG_BOX(Language::Get()->getString("@TeamBuffLimitLevel"));
		}
		else
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "TeamBuffPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		
	}
	else if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onGuideBuyEquip")
	{
		if(PopupManager::Get()->checkShowBuyEquip())
		{
			MsgMainFramePushPage msg;
			msg.pageName = "GuideBuyEquipPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onGuideBuyExp")
	{
		if(PopupManager::Get()->checkShowBuyExp())
		{
			MsgMainFramePushPage msg;
			msg.pageName = "GuideBuyExpPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onGuideLuFei")
	{
		if(PopupManager::Get()->checkShowContinueAd())
		{
			BlackBoard::Get()->ToAdventruePageType=10;
			MsgMainFrameChangePage msg;
			msg.pageName = "AdventurePage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			CCB_FUNC(this,"mGuideLuFei",CCNode,setVisible(false));
		}
	}
}

void MainPage::_refreshTitle( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();

	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar)
	{
		if(mExpbarInitScale<0)
			mExpbarInitScale = expbar->getScaleX();
		
		float expscale = 0.0f;
		if(ServerDateManager::Get()->getNextLevelExp()>0)
			expscale = ServerDateManager::Get()->getUserExpPercent();
		if(expscale>1.0f)expscale=1.0f;
		expbar->setScaleX(expscale*mExpbarInitScale);
	}

	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCLabelBMFont* exp = dynamic_cast<CCLabelBMFont* >(getVariable("mExp"));
	if(exp)
	{
		exp->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
	}
	CCLabelBMFont* gold = dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	if(gold)
	{
		gold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	}
	CCLabelBMFont* ti = dynamic_cast<CCLabelBMFont* >(getVariable("mTi"));
	if(ti)
	{
		ti->setString((StringConverter::toString(userBasicInfo.power)+"/"+StringConverter::toString(userBasicInfo.maxpower)).c_str());
	}
	CCLabelBMFont* qi = dynamic_cast<CCLabelBMFont* >(getVariable("mQi"));
	if(qi)
	{
		qi->setString((StringConverter::toString(userBasicInfo.vitality)+"/"+StringConverter::toString(userBasicInfo.maxvitality)).c_str());
	}
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(userBasicInfo.level).c_str()));
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{
		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			vipLevel->setVisible(true);
			vipLevel->initWithFile(vipImage.c_str());
		}else
		{
			vipLevel->setVisible(false);
		}
	}
	int msgUnReadCount=userBasicInfo.unReadCount;
	if(msgUnReadCount!=0)
	{
		CCB_FUNC(this,"mMsgNote",CCNode,setVisible(true));
		CCB_FUNC(this,"mMsgNumber",CCLabelBMFont,setString(StringConverter::toString(msgUnReadCount).c_str()));
	}
	else
	{
		CCB_FUNC(this,"mMsgNote",CCNode,setVisible(false));
	}
}

void MainPage::_refreshBattle(void)
{
	clearAllFrameBtn();
	cocos2d::extension::CCScrollView* members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mMembers"));
	if(members)
	{
		cocos2d::CCNode *node = cocos2d::CCNode::create();
		CCSize size = CCSize(0,0);
		
		unsigned int _index = 1;
		unsigned int battleEnableSize = BattleArrayInfo::Get()->getBattleEnableSize();
		for(;_index<=battleEnableSize;++_index)
		{
			CCBContainer* framebutton = CCBManager::Get()->createAndLoad("framedButton.ccbi");
// 			CCBContainer* framebutton = CCBContainer::create();
// 			framebutton->loadCcbiFile("framedButton.ccbi");
			framebutton->setListener(this,FACE_BUTTON_START+_index);
			CCMenuItemSprite* frame = dynamic_cast<CCMenuItemSprite*>(framebutton->getVariable("mFrameBack"));
			if(frame && frame->getParent())
			{
				frame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
				//frame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
				CCMenu* menu = dynamic_cast<CCMenu*>(frame->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
				CCB_FUNC(framebutton,"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
				if(frame->getSelectedImage()->getContentSize().width>frame->getNormalImage()->getContentSize().width)
				{
					bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
					if(isNeedSetAnchor)
					{
						float scale=(frame->getSelectedImage()->getContentSize().width-frame->getNormalImage()->getContentSize().width)*0.5/frame->getSelectedImage()->getContentSize().width;
						frame->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
					}
				}
			}
			CCSprite* sprite = dynamic_cast<CCSprite*>(framebutton->getVariable("mFrame"));
			if(sprite)
			{
				sprite->setTexture(::getTeamDefaultPic().c_str());
			}
			framebutton->retain();
			mFrameBtnList.insert(std::make_pair<unsigned int,CCBContainer*>(_index,framebutton));
			node->addChild(framebutton);
			framebutton->setPosition(ccp(size.width,5.0f));
			size.width += framebutton->getContentSize().width;
			size.height = framebutton->getContentSize().height;
		}
		UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
		while(itr.hasMoreElements())
		{
			UserBattleInfo* info = itr.getNext();
			if(info->discipleId!=0)
			{
				Disciple disciple(info->discipleId);
				if(mFrameBtnList.find(info->position+1)!=mFrameBtnList.end())
				{
					CCBContainer* framebutton = mFrameBtnList.find(info->position+1)->second;
					CCMenuItemSprite* frame = dynamic_cast<CCMenuItemSprite*>(framebutton->getVariable("mFrameBack"));
					if(frame && frame->getParent())
					{
						frame->setNormalImage(disciple.getFrameNormalSpirte());
						//frame->setSelectedImage(disciple.getFrameSelectedSpirte());
						CCMenu* menu = dynamic_cast<CCMenu*>(frame->getParent());
						if(menu)
						{
							menu->setScrollViewChild(true);
						}
						CCB_FUNC(framebutton,"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
							if(frame->getSelectedImage()->getContentSize().width>frame->getNormalImage()->getContentSize().width)
							{
								bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
								if(isNeedSetAnchor)
								{
									float scale=(frame->getSelectedImage()->getContentSize().width-frame->getNormalImage()->getContentSize().width)*0.5/frame->getSelectedImage()->getContentSize().width;
									frame->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
								}
							}
					}

					CCSprite* sprite = dynamic_cast<CCSprite*>(framebutton->getVariable("mFrame"));
					if(sprite)
					{
						sprite->setTexture(disciple.iconPic().c_str());
					}
				}
				
			}
			
		}
		CCBContainer* framebutton = CCBManager::Get()->createAndLoad("framedButton.ccbi");
// 		CCBContainer* framebutton = CCBContainer::create();
// 		framebutton->loadCcbiFile("framedButton.ccbi");
		framebutton->setListener(this,FACE_BUTTON_START+999);
		CCMenuItemSprite* frame = dynamic_cast<CCMenuItemSprite*>(framebutton->getVariable("mFrameBack"));
		if(frame && frame->getParent())
		{
			frame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			//frame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
			CCMenu* menu = dynamic_cast<CCMenu*>(frame->getParent());
			if(menu)
			{
				menu->setScrollViewChild(true);
			}
			CCB_FUNC(framebutton,"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
			if(frame->getSelectedImage()->getContentSize().width>frame->getNormalImage()->getContentSize().width)
			{
				bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
				if(isNeedSetAnchor)
				{
					float scale=(frame->getSelectedImage()->getContentSize().width-frame->getNormalImage()->getContentSize().width)*0.5/frame->getSelectedImage()->getContentSize().width;
					frame->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
				}
			}
		}
		CCSprite* sprite = dynamic_cast<CCSprite*>(framebutton->getVariable("mFrame"));
		if(sprite)
		{
			sprite->setTexture(::getTeamLockPic().c_str());
		}
		framebutton->retain();
		mFrameBtnList.insert(std::make_pair<unsigned int,CCBContainer*>(_index,framebutton));
		node->addChild(framebutton);
		framebutton->setPosition(ccp(size.width,5.0f));
		size.width += framebutton->getContentSize().width;
		size.height = framebutton->getContentSize().height;
		node->setContentSize(size);
		members->setContainer(node);
		members->setContentSize(size);
	}
}
void MainPage::clearAllFrameBtn()
{
	cocos2d::extension::CCScrollView* members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mMembers"));
	if(members!=NULL)
	{
		members->getContainer()->removeAllChildren();
	}

	FrameBtnIconList::iterator itrIcon = mFrameBtnList.begin();
	while(itrIcon != mFrameBtnList.end())
	{
		if(itrIcon->second)
			itrIcon->second->release();
		++itrIcon;
	}
	mFrameBtnList.clear();
	
	//--begin xinzheng 2013-7-17
	if (CCTextureCache::sharedTextureCache()->getTextrueTotalBytes() > 125*1024*1024)
	{
		extern void globalPurgeCachedData();
		globalPurgeCachedData();
	}
	//--end

}
