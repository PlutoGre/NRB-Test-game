
#include "stdafx.h"

#include "EquipSellPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"

REGISTER_PAGE("EquipSellPage",EquipSellPage);
USING_NS_CC;

void EquipSellContent::refreshContent( void )
{
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSelected"));
	//mSelect->unselected();
	mSelect->setNormalImage(::getContentUnselectedSpirte("Normal"));
	mSelect->setSelectedImage(::getContentUnselectedSpirte("Normal"));
	Equip equip(mID);

	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipPic"));
	if(prop)
	{
		prop->initWithFile(equip.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(frame)
	{
		frame->setNormalImage(equip.getFrameNormalSpirte());
		frame->setSelectedImage(equip.getFrameSelectedSpirte());
	}
	CCSprite* quality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipQuality"));
	if(quality)
	{
		quality->initWithFile(equip.getQualityImageFile().c_str());
	}
	CCSprite* equipBuffValue=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipBuffType"));
	if(equipBuffValue)
	{
		equipBuffValue->initWithFile(equip.getEquipAtkTypePic().c_str());
	}
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipName"));
	if(name)
	{
		name->setString(Language::Get()->getString(equip.name()).c_str());
#if defined(Macro_TestUseCCReViScrollViewFacade) && defined(_DEBUG)
		std::string strTmp = StringConverter::toString(this->m_iIdx) + Language::Get()->getString(equip.name());
		name->setString(strTmp.c_str());
#endif
	}
	CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipLevel"));
	if(levelName)
	{
		levelName->setString(StringConverter::toString(equip.level()).c_str());
	}
	CCLabelBMFont* valueKey=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipValue"));
	if(valueKey)
	{
		valueKey->setString(StringConverter::toString(equip.getEquipValue()).c_str());
	}
	CCLabelBMFont* buffValue=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBuffValue"));
	if(buffValue)
	{
		buffValue->setString(StringConverter::toString(equip.quantity()).c_str());
	}
	CCLabelBMFont* equipCarriedBy=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mCarriedBy"));
	if(equipCarriedBy)
	{
		equipCarriedBy->setVisible(false);
	}
	CCB_FUNC(mContentContainer,"mMPInBattlePic",CCSprite,setVisible(false));
#ifdef Macro_TestUseCCReViScrollViewFacade
	setSellect(m_pItemData->m_bSelected);
#endif
}

void EquipSellContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onSelected")
	{
#ifdef Macro_TestUseCCReViScrollViewFacade
		mOwnner->setSellectedContent(m_pItemData);
		setSellect(m_pItemData->m_bSelected);
#else
		mOwnner->setSellectedContent(this);
#endif
	}
}

void EquipSellContent::setSellect( bool sellect )
{
	if(sellect)
	{
		//mSelect->selected();
		mSelect->setNormalImage(::getContentSelectedSpirte("Normal"));
		mSelect->setSelectedImage(::getContentSelectedSpirte("Normal"));
	}
	else
	{
		//mSelect->unselected();
		mSelect->setNormalImage(::getContentUnselectedSpirte("Normal"));
		mSelect->setSelectedImage(::getContentUnselectedSpirte("Normal"));
	}
}

#ifdef Macro_TestUseCCReViScrollViewFacade

void EquipSellContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void EquipSellContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	EquipSellContentItemData* pItemData = static_cast<EquipSellContentItemData*>(pItem);
	if (pItemData)
	{
		this->m_pItemData = pItemData;
		this->mID = pItemData->m_iEquipID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}

#endif

EquipSellPage::EquipSellPage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
	mSelectedContents.clear();
}

EquipSellPage::~EquipSellPage(void)
{
}

void EquipSellPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	rebuildAllItem();
	refreshPage();
	mSelectedContents.clear();
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_setSelectedItems.clear();
#endif
}

void EquipSellPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
	m_setSelectedItems.clear();
#endif
}

void EquipSellPage::Execute( MainFrame* )
{
	
}

void EquipSellPage::load( void )
{
	loadCcbiFile("EquipmentSell.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mEquipSellContent"));
	membersRootNode = members->getContainer();

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif

}

bool EquipFilter_Sell(unsigned int id)
{
	Equip equip(id);

	if(equip.isEquipCarriedBy())
		return false;

	bool trueType = true;
	unsigned int mCurrTabIndex = GET_V_UINT("SellEquip");
	if(mCurrTabIndex == 2)
	{
		return EquipFilter_Type1(id);
	}
	else if(mCurrTabIndex == 3)
	{
		return EquipFilter_Type2(id);
	}
	else if(mCurrTabIndex == 4)
	{
		return EquipFilter_Type3(id);
	}

	return true;
}

void EquipSellPage::buildItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		EquipDateCollect::EquipIDCollect collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Sell);
		EquipDateCollect::EquipIDCollect::iterator itr = collect.begin(); 
		while(itr != collect.end())
		{
			//
			EquipSellContentItemData* pItemData = new EquipSellContentItemData();
			pItemData->m_iEquipID = *itr;
			pItemData->m_bSelected = false;
			pItemData->m_iIdx = iCount;
			//
			pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
			//
			EquipSellContent* pItem = NULL;
			if (iCount < iMaxNode)
			{
				//
				pItem = new EquipSellContent(*itr, this);
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
		//
		CCSize size = CCSize(fOneItemWidth, fOneItemHeight*iCount);
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
	EquipDateCollect::EquipIDCollect collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Sell);
	EquipDateCollect::EquipIDCollect::iterator itr = collect.begin(); 
	for(; itr != collect.end(); ++itr)
	{
		EquipSellContent * item = new EquipSellContent((*itr), this);
		item->init();
		addItem(item);
	}
#endif
}

void EquipSellPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	//membersRootNode->setContentSize(size);
	members->setContentSize(size);
	members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void EquipSellPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void EquipSellPage::clearAllItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade->clearAllItems();
	m_setSelectedItems.clear();
#endif
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	membersRootNode->removeAllChildren();
	mSelectedContents.clear();
}

void EquipSellPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void EquipSellPage::removeItem( ContentBase* item )
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

void EquipSellPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onOK")
	{
#ifdef Macro_TestUseCCReViScrollViewFacade
		if(m_setSelectedItems.size() == 0)
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "EquipPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			std::set<EquipSellContentItemData*>::iterator itr = m_setSelectedItems.begin();
			OPSellEquip sellEquip;
			unsigned int index = 0;
			while(itr != m_setSelectedItems.end() )
			{
				sellEquip.add_id((*itr)->m_iEquipID);
				++itr;
				++index;
			}
			PacketManager::Get()->sendPakcet(OPCODE_SELL_EQUIP_C,&sellEquip);
		}
#else
		if(mSelectedContents.size() == 0)
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "EquipPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			std::set<EquipSellContent*>::iterator itr = mSelectedContents.begin();
			OPSellEquip sellEquip;
			unsigned int index = 0;
			while(itr != mSelectedContents.end() )
			{
				sellEquip.add_id((*itr)->getContentID());
				++itr;
				++index;
			}
			PacketManager::Get()->sendPakcet(OPCODE_SELL_EQUIP_C,&sellEquip);
		}
#endif
	}
}

void EquipSellPage::refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	if(name)
	{
		name->setString(Language::Get()->getString(userBasicInfo.name).c_str());
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

#ifdef Macro_TestUseCCReViScrollViewFacade
void EquipSellPage::setSellectedContent( EquipSellContentItemData* member )
{
	if(m_setSelectedItems.count(member) != 0)
	{
		member->m_bSelected = false;
		m_setSelectedItems.erase(member);
	}
	else
	{
		m_setSelectedItems.insert(member);
		member->m_bSelected = true;
	}
}
#endif

void EquipSellPage::setSellectedContent( EquipSellContent* member )
{
	if(mSelectedContents.count(member) != 0)
	{
		member->setSellect(false);
		mSelectedContents.erase(member);
	}
	else
	{
		mSelectedContents.insert(member);
		member->setSellect(true);
	}
}

void EquipSellPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "EquipSellPage")
		{
#ifdef Macro_TestUseCCReViScrollViewFacade
			Exit(NULL);
			load();
#endif
			Enter(NULL);
		}
	}
}