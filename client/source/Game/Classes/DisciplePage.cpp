
#include "stdafx.h"

#include "DisciplePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "math.h"
#include "DiscipleRecruitPage.h"
#include "STLUtility.h"

REGISTER_PAGE("DisciplePage",DisciplePage);
USING_NS_CC;

void DiscipleContent::refreshContent( void )
{
	Disciple disciple(mID);
	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(disciple.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(frame)
	{
		frame->setNormalImage(disciple.getFrameNormalSpirte());
		frame->setSelectedImage(disciple.getFrameSelectedSpirte());
	}
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPName"));
	if(name)
	{
		name->setString(Language::Get()->getString(disciple.name()).c_str());
#if defined(Macro_TestUseCCReViScrollViewFacade) && defined(_DEBUG)
		std::string strTmp = StringConverter::toString(this->m_iIdx) + Language::Get()->getString(disciple.name());
		name->setString(strTmp.c_str());
#endif
	}
	
	CCSprite* quality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMemQualityPic"));
	if(quality)
	{
		quality->initWithFile(disciple.getQualityImageFile().c_str());
	}
	CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPLevel"));
	if(levelName)
	{
		levelName->setString(StringConverter::toString(disciple.level()).c_str());
	}
	CCLabelBMFont* valueKey=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPValue"));
	if(valueKey)
	{
		valueKey->setString(StringConverter::toString(disciple.getDiscipleValue()).c_str());
	}
	CCSprite* inBattle=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMPInBattlePic"));
	if(inBattle)
	{
		if(disciple.isInBattle())
			inBattle->setVisible(true);
		else
			inBattle->setVisible(false);
	}

	CCMenuItemImage* oprButton1 = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mOprButton1"));
	if(oprButton1)
	{
		if(!disciple.canUpgradeLevel())
		{
			oprButton1->setEnabled(false);
		}
	}

}

void DiscipleContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onOprButton_1")
	{
		BlackBoard::Get()->TransferExp_Receiver = mID;
		BlackBoard::Get()->jumpDisciplePageName = "DisciplePage";
		MsgMainFrameChangePage msg;
		msg.pageName = "TransferEXPPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onOprButton_2")
	{
		BlackBoard::Get()->TrainDisciple = mID;
		BlackBoard::Get()->jumpDisciplePageName = "DisciplePage";
		MsgMainFrameChangePage msg;
		msg.pageName = "TrainPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowMemberInfo")
	{
		BlackBoard::Get()->NeedChangeDisciple = false;
		BlackBoard::Get()->NeedChooseDisciple = false;
		BlackBoard::Get()->ShowDisciple = mID;
		BlackBoard::Get()->jumpDisciplePageName = "DisciplePage";
		MsgMainFramePushPage msg;
		msg.pageName = "DiscipleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

#ifdef Macro_TestUseCCReViScrollViewFacade

void DiscipleContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void DiscipleContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	DiscipleContentItemData* pItemData = static_cast<DiscipleContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iDiscipleID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}

#endif

void SoulContent::refreshContent( void )
{	
	Soul soul(mID);
	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(soul.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(frame)
	{
		frame->setNormalImage(soul.getFrameNormalSpirte());
		frame->setSelectedImage(soul.getFrameSelectedSpirte());
	}
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPSName"));
	if(name)
	{
		name->setString(Language::Get()->getString(soul.name()).c_str());
#if defined(Macro_TestUseCCReViScrollViewFacade) && defined(_DEBUG)
		std::string strTmp = StringConverter::toString(this->m_iIdx) + Language::Get()->getString(soul.name());
		name->setString(strTmp.c_str());
#endif
	}
	CCSprite* quality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSoulQulityPic"));
	if(quality)
	{
		quality->initWithFile(soul.getQualityImageFile().c_str());
	}
	CCLabelBMFont* soulQuantity = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPSoulQuantity"));
	if(soulQuantity)
	{
		soulQuantity->setString(StringConverter::toString(soul.count()).c_str());
	}
	CCLabelBMFont* soulText = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPMoreXSoul"));
	if(soulText)
	{
		if(soul.getGapNum() > 0)
		{
			soulText->setString(soul.getNumText().c_str());
			soulText->setVisible(true);
		}
		else
		{
			soulText->setVisible(false);
		}
	}

	CCLabelBMFont* buttonName = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mOprName_1"));
	CCMenuItemImage* button = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mOprButton_1"));
	if(button && buttonName)
	{
		if(soul.getGapNum() > 0 || (!soul.canRecruit()&&!soul.canUpgrade()) )
		{
			button->setVisible(false);
			buttonName->setVisible(false);
		}
		else
		{
			buttonName->setVisible(true);
			buttonName->setString(soul.getOprText().c_str());
			button->setVisible(true);
		}
	}

}

void SoulContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onRecOrBreak")
	{
		Soul soul(mID);
		if(soul.canRecruit())
		{
// 			BasePage* obj = CCBManager::Get()->getPage("DiscipleRecruitPage");
// 			DiscipleRecruitPage* page = dynamic_cast<DiscipleRecruitPage*>(obj);
// 			if(page)
// 			{
// 				page->setID(1);
// 			}
// 			MsgMainFramePushPage msg;
// 			msg.pageName = "DiscipleRecruitPage";
// 			MessageManager::Get()->sendMessage(&msg);

			OPRecruitDisciple recruitDisciple;
			recruitDisciple.set_id(soul.itemID());
			PacketManager::Get()->sendPakcet(OPCODE_RECRUIT_DISCIPLE_C,&recruitDisciple);
		}
		else if(soul.canUpgrade())
		{
			BlackBoard::Get()->UpgradeSoul = mID;

			MsgMainFrameChangePage msg;
			msg.pageName = "DiscipleUpgradePage";
			MessageManager::Get()->sendMessage(&msg);
		}
		
	}
	else if(itemName=="onShowMemberInfo")
	{
		BlackBoard::Get()->ShowSoul = mID;

		MsgMainFramePushPage msg;
		msg.pageName = "SoulInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

#ifdef Macro_TestUseCCReViScrollViewFacade

void SoulContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void SoulContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	SoulContentItemData* pItemData = static_cast<SoulContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iSoulID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}

#endif


DisciplePage::DisciplePage(void)
{
	mCurrTabIndex = 0;
	mLongBtnHeight = 0.0f;
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
}

DisciplePage::~DisciplePage(void)
{
}

void DisciplePage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(OPCODE_RECRUIT_DISCIPLERET_S,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_TO_TAB,this);
	mCurrTabIndex = 0;
	selectTab(BlackBoard::Get()->DisciplePageTab);
	BlackBoard::Get()->DisciplePageTab = 1;

	refreshPage();
}

void DisciplePage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
#endif
}

void DisciplePage::Execute( MainFrame* )
{

}

void DisciplePage::load( void )
{
	loadCcbiFile("MembersPage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif
}

void DisciplePage::buildItem( void )
{
	CCBContainer* longButton = CCBManager::Get()->createAndLoad("LongButton.ccbi");
	longButton->setListener(this,999);
	if(longButton)
	{
		CCLabelBMFont* goGetBtnLabel=dynamic_cast<CCLabelBMFont*>(longButton->getVariable(("mButtonLabel")));
		if(goGetBtnLabel)
		{
			goGetBtnLabel->setString(Language::Get()->getString("@GoMarketGetDisciple").c_str());
		}
		membersRootNode->addChild(longButton);
		CCPoint pos = ccp(0,0);
		longButton->setPosition(pos);
		mLongBtnHeight =  longButton->getContentSize().height;
		CCSize size = CCSize(longButton->getContentSize().width, mLongBtnHeight);
		members->setContentSize(size);
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
	}

#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		//
		if(mCurrTabIndex == 1)
		{
			DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getDiscipleCollection();
			DiscipleDateCollect::DiscipleIDCollect::reverse_iterator itr = collect.rbegin(); 
			while(itr != collect.rend())
			{
				//
				DiscipleContentItemData* pItemData = new DiscipleContentItemData();
				pItemData->m_iDiscipleID = *itr;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount+mLongBtnHeight);
				//
				DiscipleContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new DiscipleContent(*itr);
					pItem->initItemView();

					//pItem->refreshItemView(pItemData);//初始在m_pScrollViewFacade->setDynamicItemsStartPosition设置
					//
					if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
						fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
					if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
						fOneItemWidth = pItem->getContainerNode()->getContentSize().width;
				}
				m_pScrollViewFacade->addItem(pItemData, pItem);
				//
				++itr;
				++iCount;
			}
		}
		else
		{
			DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getSoulCollection(SoulFilter_default,SoulSort_recruitUpgrade);
			DiscipleDateCollect::DiscipleIDCollect::iterator itr = collect.begin(); 
			if(itr == collect.end()){
				int fontWidth =members->getViewSize().width; 
				int fontHeight =members->getViewSize().height; 

				CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
				normalBMFontSingle->setString(Language::Get()->getString("@noSoulToChange").c_str());
				normalBMFontSingle->setColor(ccBLACK);
				normalBMFontSingle->setScale(1);
				CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
				normalBMFontSingle->setPosition(pos);
				membersRootNode->addChild(normalBMFontSingle);

				CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
				normalBMFontSingle2->setString(Language::Get()->getString("@noSoulToChange").c_str());
				normalBMFontSingle2->setColor(ccGRAY);
				normalBMFontSingle2->setScale(1);
				CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
				normalBMFontSingle2->setPosition(pos2);
				membersRootNode->addChild(normalBMFontSingle2);
			}
			while(itr != collect.end())
			{
				//
				SoulContentItemData* pItemData = new SoulContentItemData();
				pItemData->m_iSoulID = *itr;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount+mLongBtnHeight);
				//
				SoulContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new SoulContent(*itr);
					pItem->initItemView();

					//pItem->refreshItemView(pItemData);//初始在m_pScrollViewFacade->setDynamicItemsStartPosition设置
					//
					if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
						fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
					if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
						fOneItemWidth = pItem->getContainerNode()->getContentSize().width;
				}
				m_pScrollViewFacade->addItem(pItemData, pItem);
				//
				++itr;
				++iCount;
			}
		}
		//
		CCSize size = CCSize(fOneItemWidth, fOneItemHeight*iCount+mLongBtnHeight);
		members->setContentSize(size);
		/*
			注意ScrollView里除了受控的DynamicItems，还有其他Items
			ScrollView锚点(0.f,0.f)位于左下角
			height由下向上增长
			width由左向右增长
			下面两句顺序不能颠倒，意思是：最后添加的显示到可视区域左上角
		*/
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		m_pScrollViewFacade->setDynamicItemsStartPosition(iCount-1);
		{
			//m_pScrollViewFacade->setDynamicItemsStartPosition(0);
		}
		//
	}
#else
	if(mCurrTabIndex == 1)
	{
// 		UserDiscipleInfoListIterator itr = ServerDateManager::Get()->getDiscipleIterator();
// 		while(itr.hasMoreElements())
// 		{
// 			unsigned int id = itr.getNext()->id;
// 			DiscipleContent * item = new DiscipleContent(id);
// 			item->init();
// 			addItem(item);
// 		}
		DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getDiscipleCollection();
		DiscipleDateCollect::DiscipleIDCollect::iterator itr = collect.begin(); 
		for(int i = collect.size() - 1; itr != collect.end(); ++itr, --i)
		{
			DiscipleContent * item = new DiscipleContent((*itr));
			item->init();
			//addItem(item);
			mContentList.push_back(item);
			membersRootNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*i+mLongBtnHeight);
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size()+mLongBtnHeight);
			members->setContentSize(size);
			members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}
	}
	else
	{
		DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getSoulCollection(SoulFilter_default,SoulSort_recruitUpgrade);
		DiscipleDateCollect::DiscipleIDCollect::iterator itr = collect.begin(); 
		for(int i = collect.size() - 1; itr != collect.end(); ++itr, --i)
		{
			SoulContent * item = new SoulContent((*itr));
			item->init();
			addItem(item);
		}
	}
#endif
}

void DisciplePage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1)+mLongBtnHeight);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size()+mLongBtnHeight);
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void DisciplePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void DisciplePage::clearAllItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade->clearAllItems();
#endif
// 	ContentList::iterator itr = mContentList.begin();
// 	while(itr != mContentList.end())
// 	{
// 		delete (*itr);
// 		++itr;
// 	}
	//ContentList list;
	//mContentList.swap(list);
	//ContentList().swap(mContentList);
	//Clear_Vec(mContentList);
	SAFE_RELEASE_VECTOR(mContentList);
	membersRootNode->removeAllChildren();
}

void DisciplePage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void DisciplePage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
		}
		++itr;
	}
}

void DisciplePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onToggleMemberButton")
	{
		selectTab(1);	
	}
	else if(itemName=="onToggleSoulButton")
	{
		selectTab(2);
	}
}
void DisciplePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender,int tag)
{
	if(itemName=="onFunciton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "MarketPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}


void DisciplePage::refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCLabelBMFont* level = dynamic_cast<CCLabelBMFont* >(getVariable("mPlayerLv"));
	if(level)
	{
		level->setString(StringConverter::toString(userBasicInfo.level).c_str());
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
}

void DisciplePage::selectTab( unsigned int index )
{
	if(mCurrTabIndex != index)
	{
		mCurrTabIndex = index;
		rebuildAllItem();
	}

	if(mCurrTabIndex == 2)
	{
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_1"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_2"))->selected();
	}
	else
	{
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_1"))->selected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_2"))->unselected();
	}
}

void DisciplePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_RECRUIT_DISCIPLERET_S)
		{
			rebuildAllItem();
		}
	}
	else if(message->getTypeId() == MSG_TO_TAB)
	{
		const MsgToTab * msg = dynamic_cast<const MsgToTab*>(message);
		if(msg)
		{
			selectTab(msg->tabIndex);
		}
	}
}
