
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
#include "waitingManager.h"
#include <algorithm>
#include <cmath>

#include "cocos-ext.h"
#include "cocos2d.h"
#include "PopupManager.h"
//test code
#include "lib91.h"
//test end

USING_NS_CC;
USING_NS_CC_EXT;

namespace {
const int HomeRootTag=78100,HomeHeaderTag=78101,HomeMembersTag=78102,HomePopupTag=78103;
const char* HomeArt="ported/home/v1/nrb_home_";

bool homeInputBlocked(){
    MainFrame* frame=MainFrame::getInstance();if(!frame)return false;
    CCNode* dialogs=dynamic_cast<CCNode*>(frame->getVariable("mNodeMid"));
    CCNode* blocker=dynamic_cast<CCNode*>(frame->getVariable("mNoTouch"));
    return waitingManager::Get()->getWaitting()||(dialogs&&dialogs->getChildrenCount()>0)||(blocker&&blocker->isVisible());
}
void homePressed(const std::string& action,int tag=0,const char* ccb="MainScene.ccbi"){
    // 保留新手引导监听的原点击消息；不伪造网络回包或完成进度。
    MsgButtonPressed message;message.ccb=ccb;message.func=action;message.tag=tag;MessageManager::Get()->sendMessage(&message);
    CCLOG("MainPage: home action %s tag %d",action.c_str(),tag);
}

// 固定入口与横向阵容使用不同的吞触摸方式，滑动阵容时不误点头像。
class HomeMenu : public CCMenu {
    bool mScrollable;
    int mPriority;
public:
    explicit HomeMenu(bool scrollable,int priority):mScrollable(scrollable),mPriority(priority) { }
    static HomeMenu* create(bool scrollable=false,int priority=0){HomeMenu* menu=new HomeMenu(scrollable,priority);if(!menu->init()){delete menu;return NULL;}menu->setScrollViewChild(scrollable);menu->autorelease();return menu;}
    virtual void registerWithTouchDispatcher(){CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,kCCMenuHandlerPriority-mPriority,!mScrollable);}
    virtual bool ccTouchBegan(CCTouch* touch,CCEvent* event){return !homeInputBlocked()&&CCMenu::ccTouchBegan(touch,event);}
};
class HomeScroll : public CCScrollView {
public:
    static HomeScroll* create(CCSize size){HomeScroll* view=new HomeScroll();if(!view->initWithViewSize(size)){delete view;return NULL;}view->autorelease();return view;}
    virtual void registerWithTouchDispatcher(){CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,kCCMenuHandlerPriority-2,true);}
    virtual bool ccTouchBegan(CCTouch* touch,CCEvent* event){return !homeInputBlocked()&&CCScrollView::ccTouchBegan(touch,event);}
};
CCSprite* homePicture(CCNode* parent,const std::string& path,float x,float y,float width,float height,bool contain=true){
    CCFileUtils* files=CCFileUtils::sharedFileUtils();
    if(!CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(path.c_str())&&!files->isFileExist(files->fullPathForFilename(path.c_str())))return NULL;
    CCSprite* sprite=CCSprite::create(path.c_str());if(!sprite)return NULL;CCSize size=sprite->getContentSize();
    if(size.width<=0||size.height<=0)return NULL;
    if(contain)sprite->setScale(std::min(width/size.width,height/size.height));else{sprite->setScaleX(width/size.width);sprite->setScaleY(height/size.height);}
    sprite->setPosition(ccp(x,y));parent->addChild(sprite);return sprite;
}
CCSprite* homeArt(CCNode* parent,const char* name,float x,float y,float width,float height,bool contain=true){return homePicture(parent,std::string(HomeArt)+name+".png",x,y,width,height,contain);}
void homeText(CCNode* parent,const std::string& value,float x,float y,float width,float font=12,ccColor3B color=ccc3(255,248,218)){
    CCLabelTTF* shadow=CCLabelTTF::create(value.c_str(),"Arial",font);
    float scale=std::min(1.0f,width/std::max(1.0f,shadow->getContentSize().width));shadow->setScale(scale);
    shadow->setAnchorPoint(ccp(0,1));shadow->setPosition(ccp(x+0.7f,y-0.7f));shadow->setColor(ccc3(45,26,12));parent->addChild(shadow,4);
    CCLabelTTF* label=CCLabelTTF::create(value.c_str(),"Arial",font);label->setScale(scale);
    label->setAnchorPoint(ccp(0,1));label->setPosition(ccp(x,y));label->setColor(color);parent->addChild(label,5);
}
CCNode* homeButton(CCNode* parent,CCObject* target,SEL_MenuHandler selector,int tag,float x,float y,float width,float height,bool scrollable=false,bool popup=false){
    CCNode* normal=CCNode::create();normal->setContentSize(CCSizeMake(width,height));
    CCMenuItemSprite* item=CCMenuItemSprite::create(normal,NULL,target,selector);item->setTag(tag);item->setPosition(ccp(x,y));
    HomeMenu* menu=HomeMenu::create(scrollable,popup?1:scrollable?3:0);menu->setPosition(CCPointZero);menu->addChild(item);parent->addChild(menu,10);return normal;
}
CCNode* referenceHome(MainPage* page){return page->getChildByTag(HomeRootTag);}
void openHomeAdventure(unsigned int type){BlackBoard::Get()->ToAdventruePageType=type;MsgMainFrameChangePage message;message.pageName="AdventurePage";MessageManager::Get()->sendMessage(&message);}
}

REGISTER_PAGE("MainPage",MainPage)


MainPage::MainPage(void)
:mExpbarInitScale(-1.0f),mHomePopup(0)
{
	mFrameBtnList.clear();
}


MainPage::~MainPage(void)
{
}

void MainPage::Enter( MainFrame* frame)
{
	mHomePopup=0;
	CCB_FUNC(this,"mGuideBuyEquip",CCMenuItemImage,setVisible(false));
	CCB_FUNC(this,"mGuideBuyExp",CCMenuItemImage,setVisible(false));
	_refreshTitle();
	_refreshBattle();
	_refreshReferenceShortcuts();
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	CCB_FUNC(this,"mGuideLuFei",CCNode,setVisible(false));
}

void MainPage::Exit( MainFrame* frame)
{
	mHomePopup=0;
	MessageManager::Get()->removeMessageHandler(this);
	clearAllFrameBtn();
}

void MainPage::Execute( MainFrame* )
{
	if(!referenceHome(this))_buildReferenceHome();
	static float durationTime=0;
	durationTime+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>1.0f)
	{
		durationTime -=1.0f;
		_refreshPageSymbol();
		_refreshReferenceTitle();
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
	_buildReferenceHome();
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
	_refreshReferenceTitle();
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
	if(referenceHome(this)){_refreshReferenceBattle();_refreshReferenceTitle();return;}
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
			mFrameBtnList.insert(std::pair<unsigned int,CCBContainer*>(_index,framebutton));
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
		mFrameBtnList.insert(std::pair<unsigned int,CCBContainer*>(_index,framebutton));
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

// 新首页以旧场景的实际边界为布局范围，保留全局广播、底栏和弹窗层。
void MainPage::_buildReferenceHome(){
    if(referenceHome(this))return;
    CCNode* legacy=getChildren()&&getChildren()->count()?static_cast<CCNode*>(getChildren()->objectAtIndex(0)):NULL;
    if(!legacy||!getParent()||legacy->getTag()==HomeRootTag+4)return;
    CCFileUtils* files=CCFileUtils::sharedFileUtils();
    if(!files->isFileExist(files->fullPathForFilename((std::string(HomeArt)+"u_mainSceneBannerBG1.png").c_str()))){
        legacy->setTag(HomeRootTag+4);CCLOG("MainPage: reference home assets missing; keeping original layout");return;
    }
    // 旧 MainScene 根节点没有尺寸；挂到 MainFrame 后按世界坐标避让广播和底栏。
    CCSize window=CCDirector::sharedDirector()->getWinSize();float bottom=0,top=window.height;
    MainFrame* frame=MainFrame::getInstance();
    CCScrollView* navigation=frame?dynamic_cast<CCScrollView*>(frame->getVariable("mMainbuttons")):NULL;
    if(navigation){CCPoint a=navigation->convertToWorldSpace(CCPointZero),b=navigation->convertToWorldSpace(ccp(0,navigation->getViewSize().height));bottom=std::max(a.y,b.y)+3;}
    CCMenuItem* banner=getMenuItemByAction("onPlayerInfo");
    if(banner){CCPoint a=banner->convertToWorldSpace(CCPointZero),b=banner->convertToWorldSpace(ccp(0,banner->getContentSize().height));top=std::min(top,std::max(a.y,b.y)-6.0f);}
    if(top-bottom<180){legacy->setTag(HomeRootTag+4);CCLOG("MainPage: home viewport unavailable; keeping original layout");return;}
    CCPoint a=convertToNodeSpace(ccp(0,bottom)),b=convertToNodeSpace(ccp(window.width,top));
    CCRect bounds=CCRectMake(a.x,a.y,b.x-a.x,b.y-a.y);
    legacy->setVisible(false);
    float unit=bounds.size.width/320.0f,height=bounds.size.height/unit;
    CCNode* root=CCNode::create();root->setTag(HomeRootTag);root->setContentSize(CCSizeMake(320,height));root->setPosition(bounds.origin);root->setScale(unit);addChild(root,1);
    mHomePopup=0;mHomeHeaderSignature.clear();
    const float seaBottom=61,seaTop=height-126,seaHeight=std::max(50.0f,seaTop-seaBottom);
    homeArt(root,"u_mainFrameBG01",160,seaBottom+seaHeight/2,320,seaHeight,false);
    homeArt(root,"u_mainFrameCloud01",100,seaBottom+seaHeight*0.73f,205,66);
    CCLayerColor* water=CCLayerColor::create(ccc4(28,160,216,255),320,seaBottom+seaHeight*0.43f);water->setPosition(CCPointZero);root->addChild(water);
    // 双份海浪纹理连续平移，循环接缝不依赖 Spine 或参考服的场景脚本。
    for(int row=0;row<4;row++){
        float waveY=row==0?29:seaBottom+seaHeight*(0.05f+0.15f*(row-1));
        CCNode* waves=CCNode::create();waves->setPosition(ccp(0,waveY));root->addChild(waves);
        homeArt(waves,"u_mainFrameBGsea",160,0,320,seaHeight*0.22f,false);
        homeArt(waves,"u_mainFrameBGsea",480,0,320,seaHeight*0.22f,false);
        waves->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(9.0f+row*2,ccp(-320,0)),CCPlace::create(ccp(0,waveY)),NULL)));
    }
    const std::string key="HomeShip_"+StringConverter::toString(GamePrecedure::Get()->getServerID())+"_"+GamePrecedure::Get()->getUin();
    int ship=CCUserDefault::sharedUserDefault()->getIntegerForKey(key.c_str(),1);
    float shipHeight=std::min(173.0f,seaHeight*0.75f),shipWidth=shipHeight*0.94f;
    CCNode* boat=homeButton(root,this,menu_selector(MainPage::onReferenceHomeClick),2003,160,seaBottom+seaHeight*0.46f,shipWidth,shipHeight);
    homeArt(boat,ship==2?"Ship_B":"Ship_A",shipWidth/2,shipHeight/2,shipWidth,shipHeight);
    boat->runAction(CCRepeatForever::create(CCSequence::create(CCEaseSineInOut::create(CCMoveBy::create(2.2f,ccp(0,3))),CCEaseSineInOut::create(CCMoveBy::create(2.2f,ccp(0,-3))),NULL)));
    homeArt(root,"u_mainFrameTree01",310,seaTop-52,75,156);
    CCNode* gift=homeButton(root,this,menu_selector(MainPage::onReferenceHomeClick),2100,33,seaTop-34,58,66);
    homeArt(gift,"u_mainSceneBtn7_2",29,27,39,36);homeArt(gift,"u_mainSceneBtn7_1",29,48,45,28);
    homeText(gift,"活动",16,23,40,13);
    // 选人、头像与快捷入口各自有独立节点，只刷新收到变化的内容。
    CCNode* header=CCNode::create();header->setTag(HomeHeaderTag);header->setPosition(ccp(0,height-60));header->setContentSize(CCSizeMake(320,60));root->addChild(header,20);
    CCNode* members=CCNode::create();members->setTag(HomeMembersTag);members->setPosition(ccp(0,height-126));members->setContentSize(CCSizeMake(320,66));root->addChild(members,20);
    CCLayerColor* tint=CCLayerColor::create(ccc4(17,39,67,210),320,66);members->addChild(tint);
    CCNode* quick=CCNode::create();quick->setContentSize(CCSizeMake(320,61));root->addChild(quick,20);
    const char* labels[]={"船员","装备","奥义","船舱","修行","常用"};
    for(int i=0;i<6;i++){
        CCNode* icon=homeButton(quick,this,menu_selector(MainPage::onReferenceHomeClick),2000+i,26.5f+i*53.3f,30.5f,53,61);
        homeArt(icon,"u_mainSceneBtn0_1",26.5f,31,46,46);
        if(i==0){
            homeArt(icon,"u_mainSceneBtn1_3",26,18,16,12);homeArt(icon,"u_mainSceneBtn1_2",43,34,17,15);homeArt(icon,"u_mainSceneBtn1_4",25,39,42,39);
        }else if(i==1)homeArt(icon,"u_mainSceneBtn2_1",26,35,26,45);
        else if(i==2)homeArt(icon,"u_mainSceneBtn3_1",26,34,38,38);
        else if(i==3)homeArt(icon,"Ship_A",26,36,37,41);
        else if(i==4)homeArt(icon,"u_mainSceneBtn5_1",26,35,35,42);
        else{homeArt(icon,"u_mainSceneBtn6_2",26,34,43,43);homeArt(icon,"u_mainSceneBtn6_1",26,34,17,17);}
        homeText(icon,labels[i],12,23,42,13);
    }
    _refreshReferenceTitle();_refreshReferenceBattle();
    CCLOG("MainPage: reference home layout loaded (%.0f x %.0f)",bounds.size.width,bounds.size.height);
}

void MainPage::_refreshReferenceTitle(){
    CCNode* root=referenceHome(this);if(!root)return;CCNode* header=root->getChildByTag(HomeHeaderTag);if(!header)return;
    const UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
    unsigned int lead=0;unsigned int position=~0u;UserBattleInfoListIterator itr=ServerDateManager::Get()->getUserBattleIterator();
    while(itr.hasMoreElements()){UserBattleInfo* battle=itr.getNext();if(battle->discipleId&&battle->position<position){lead=battle->discipleId;position=battle->position;}}
    std::string signature=info.name+"|"+StringConverter::toString(info.level)+"|"+StringConverter::toString(info.exp)+"|"+StringConverter::toString(info.power)+"|"+StringConverter::toString(info.maxpower)+"|"+StringConverter::toString(info.vitality)+"|"+StringConverter::toString(info.maxvitality)+"|"+StringConverter::toString(info.goldcoins)+"|"+StringConverter::toString(info.silvercoins)+"|"+StringConverter::toString(info.unReadCount)+"|"+StringConverter::toString(info.viplevel)+"|"+StringConverter::toString(lead);
    if(signature==mHomeHeaderSignature&&header->getChildrenCount()>0)return;mHomeHeaderSignature=signature;header->removeAllChildrenWithCleanup(true);
    // 头像在地图边框下方，货币、等级和传书数量来自当前登录账号。
    if(lead){Disciple disciple(lead);homePicture(header,disciple.iconPic(),29,29,36,36);}
    else homeArt(header,"u_mainSceneBtn1_4",29,29,35,35);
    homeArt(header,"u_mainSceneBannerBG1",160,30,320,60,false);
    homeText(header,info.name,68,58,77,10,ccc3(255,230,143));
    homeText(header,StringConverter::toString(info.power)+"/"+StringConverter::toString(info.maxpower),158,48,43,9);
    homeText(header,StringConverter::toString(info.vitality)+"/"+StringConverter::toString(info.maxvitality),218,48,42,9);
    homeText(header,StringConverter::toString(info.silvercoins),107,32,73,10);
    homeText(header,StringConverter::toString(info.goldcoins),202,32,57,10);
    homeText(header,StringConverter::toString(info.level),115,22,22,12,ccc3(255,232,155));
    float progress=std::max(0.0f,std::min(1.0f,ServerDateManager::Get()->getUserExpPercent()));
    if(progress>0){CCSprite* exp=homeArt(header,"u_mainSceneExp",139+progress*58,11,progress*116,4,false);if(exp)exp->setOpacity(240);}
    homeText(header,ServerDateManager::Get()->getUserExpPercentStr(),165,15,90,7);
    homeText(header,"VIP "+StringConverter::toString(info.viplevel),9,13,48,8,ccc3(255,219,80));
    homeButton(header,this,menu_selector(MainPage::onReferenceHomeClick),2102,29,30,51,56);
    CCNode* mail=homeButton(header,this,menu_selector(MainPage::onReferenceHomeClick),2101,288,31,54,57);
    homeArt(mail,"u_mainSceneIcohelper",27,32,43,47);homeText(mail,"传书",16,20,36,10);
    if(info.unReadCount){
        CCLayerColor* badge=CCLayerColor::create(ccc4(180,35,24,255),22,14);badge->setPosition(ccp(29,43));mail->addChild(badge,3);
        homeText(mail,info.unReadCount>99?"99+":StringConverter::toString(info.unReadCount),31,56,24,9);
    }
}

void MainPage::_refreshReferenceBattle(){
    CCNode* root=referenceHome(this);if(!root)return;CCNode* members=root->getChildByTag(HomeMembersTag);if(!members)return;
    if(CCNode* previous=members->getChildByTag(1))previous->removeFromParentAndCleanup(true);
    HomeScroll* view=HomeScroll::create(CCSizeMake(320,66));view->setTag(1);view->setDirection(kCCScrollViewDirectionHorizontal);view->setBounceable(false);
    CCNode* content=CCNode::create();unsigned int count=std::max(1u,BattleArrayInfo::Get()->getBattleEnableSize());
    bool locked=BattleArrayInfo::Get()->getNextUnlockLevel()>0;content->setContentSize(CCSizeMake(std::max(320.0f,(count+(locked?1:0))*61.0f+8),66));
    for(unsigned int i=0;i<count+(locked?1:0);i++){
        CCNode* normal=homeButton(content,this,menu_selector(MainPage::onReferenceHomeClick),i<count?1000+i:1999,33+i*61,33,54,57,true);
        UserBattleInfo* battle=i<count?ServerDateManager::Get()->getUserBattleInfo(i):NULL;
        if(battle&&battle->discipleId){
            Disciple disciple(battle->discipleId);CCSprite* frame=disciple.getFrameNormalSpirte();
            if(frame){CCSize size=frame->getContentSize();frame->setScale(std::min(53/size.width,53/size.height));frame->setPosition(ccp(27,29));normal->addChild(frame);}
            homePicture(normal,disciple.iconPic(),27,29,44,44);
        }else{
            homePicture(normal,i<count ? ::getTeamDefaultPic() : ::getTeamLockPic(),27,29,50,50);
        }
    }
    view->setContainer(content);view->setContentOffset(CCPointZero);members->addChild(view,2);
}

void MainPage::_refreshReferenceShortcuts(){
    CCNode* root=referenceHome(this);if(!root)return;if(CCNode* old=root->getChildByTag(HomePopupTag))old->removeFromParentAndCleanup(true);
    if(mHomePopup==0)return;
    CCNode* popup=CCNode::create();popup->setTag(HomePopupTag);popup->setPosition(ccp(8,65));root->addChild(popup,40);
    CCScale9Sprite* frame=CCScale9Sprite::create((std::string(HomeArt)+"u_mainSceneBtnBG01.png").c_str());
    if(frame){frame->setContentSize(CCSizeMake(304,112));frame->setPosition(ccp(152,56));popup->addChild(frame);}
    homeText(popup,mHomePopup==1?"常用功能 · 再点常用收起":"修行与养成 · 再点修行收起",14,104,280,12);
    const char* common[]={"背包","传书","图鉴","好友","聊天","设置"};
    const char* growth[]={"修行","潜能训练","船员养成","装备养成","奥义扩展","运宝"};
    for(int i=0;i<6;i++){
        CCNode* normal=homeButton(popup,this,menu_selector(MainPage::onReferenceHomeClick),(mHomePopup==1?2200:2300)+i,53+(i%3)*99,65-(i/3)*35,91,30,false,true);
        CCLayerColor* background=CCLayerColor::create(ccc4(27,82,99,235),91,30);normal->addChild(background);homeText(normal,mHomePopup==1?common[i]:growth[i],9,23,82,12);
    }
}

void MainPage::onReferenceHomeClick(CCObject* sender){
    int tag=static_cast<CCNode*>(sender)->getTag();
    if(homeInputBlocked())return;
    // 页面跳转或收起菜单会移除当前按钮；保留其菜单到本帧结束，供触摸回调安全收尾。
    CCNode* menu=static_cast<CCNode*>(sender)->getParent();if(menu){menu->retain();menu->autorelease();}
    if(mHomePopup&&tag<2200&&tag!=2004&&tag!=2005){mHomePopup=0;_refreshReferenceShortcuts();return;}
    if(tag==2005||tag==2004){int next=tag==2005?1:2;mHomePopup=mHomePopup==next?0:next;_refreshReferenceShortcuts();return;}
    if(tag>=1000&&tag<2000){int position=tag==1999?999:tag-1000+1;homePressed("onFrameButton",position,"framedButton.ccbi");onMenuItemAction("onFrameButton",sender,position);return;}
    mHomePopup=0;_refreshReferenceShortcuts();
    if(tag==2003){openHomeAdventure(97);return;}
    if(tag>=2300&&tag<=2305){
        if(tag==2300){homePressed("onTeamBufferPage");onMenuItemAction("onTeamBufferPage",sender);}
        else{const unsigned int modes[]={0,96,95,93,94,92};openHomeAdventure(modes[tag-2300]);}
        return;
    }
    std::string action;
    if(tag==2000)action="onMemberPage";else if(tag==2001)action="onEquipPage";else if(tag==2002)action="onSkillPage";
    else if(tag==2100)action="onAdventurePage";else if(tag==2101)action="onSysMsgButton";else if(tag==2102)action="onPlayerInfo";
    else if(tag>=2200&&tag<=2205){const char* common[]={"onBagButton","onSysMsgButton","onHandBookPage","onLinkmanButton","onChatButton1","onSettingPage"};action=common[tag-2200];}
    if(!action.empty()){homePressed(action);onMenuItemAction(action,sender);}
}
