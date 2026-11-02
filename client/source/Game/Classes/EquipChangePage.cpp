
#include "stdafx.h"

#include "EquipChangePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"
#include "Disciple.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("EquipChangePage",EquipChangePage);
USING_NS_CC;

void EquipChangeContent::refreshContent( void )
{
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSelected"));
//	mSelect->unselected();
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
	CCSprite* mMPInBattlePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMPInBattlePic"));

	CCLabelBMFont* equipCarriedBy=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mCarriedBy"));
	if(equipCarriedBy)
	{
		equipCarriedBy->setVisible(true);
		carriedByDiscipleName = equip.getEquipCarriedBy();
		if(carriedByDiscipleName!="")
		{
			char buff[128];
			std::string s= Language::Get()->getString("@CarriedBy");
			sprintf(buff,s.c_str(),carriedByDiscipleName.c_str());
			equipCarriedBy->setString(buff);
			if(mMPInBattlePic)
			{
				mMPInBattlePic->setVisible(true);
			}
		}
		else
		{
			equipCarriedBy->setString("");
			if(mMPInBattlePic)
			{
				mMPInBattlePic->setVisible(false);
			}
		}

	}

#ifdef Macro_TestUseCCReViScrollViewFacade
	setSellect(m_pItemData->m_bSelected);
#endif

}

void EquipChangeContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onSelected")
	{
#ifdef Macro_TestUseCCReViScrollViewFacade
		mOwnner->setSellectedContent(m_pItemData);
		setSellect(m_pItemData->m_bSelected);
#else
		mOwnner->setSellectedContent(this);
#endif
		if(carriedByDiscipleName!="" && mSelect->isSelected())
		{
			char buff[128];
			std::string s= Language::Get()->getString("@AlreadyUsedBy");
			sprintf(buff,s.c_str(),carriedByDiscipleName.c_str());
			MSG_BOX(buff);
		}
	}
}

void EquipChangeContent::setSellect( bool sellect )
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

void EquipChangeContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void EquipChangeContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	EquipChangeContentItemData* pItemData = static_cast<EquipChangeContentItemData*>(pItem);
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

EquipChangePage::EquipChangePage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
	mCurSellectedContent = NULL;
	isDesOrder=true;
}

EquipChangePage::~EquipChangePage(void)
{
}

void EquipChangePage::Enter( MainFrame* frame)
{
	isDesOrder=true;
	rebuildAllItem();
	mCurSellectedContent = NULL;
	refreshPage();
}

void EquipChangePage::Exit( MainFrame* frame)
{
	clearAllItem();
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
	m_curSelectedItemData = NULL;
#endif
}

void EquipChangePage::Execute( MainFrame* )
{

}

void EquipChangePage::load( void )
{
	loadCcbiFile("EquipReplace.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mEquipContent"));
	membersRootNode = members->getContainer();

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
	m_curSelectedItemData = NULL;
#endif

}

bool EquipFilter_Change(unsigned int id)
{
	Equip equip(id);

	//和当前更换装备类型不同
	if(BlackBoard::Get()->CurChangeEquipType != equip.type())
	{
		return false;
	}

	//当前装备
	if(equip.id() == BlackBoard::Get()->CurChangeEquipID)
	{
		return false;
	}

	return true;
}

void EquipChangePage::buildItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		EquipDateCollect::EquipIDCollect collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Change);
		if(collect.rbegin() == collect.rend()){

			int fontWidth =members->getViewSize().width; 
			int fontHeight =members->getViewSize().height; 

			CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
			normalBMFontSingle->setString(Language::Get()->getString("@noEquipToChange").c_str());
			normalBMFontSingle->setColor(ccBLACK);
			normalBMFontSingle->setScale(1);
			CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
			normalBMFontSingle->setPosition(pos);
			membersRootNode->addChild(normalBMFontSingle);
			
			CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
			normalBMFontSingle2->setString(Language::Get()->getString("@noEquipToChange").c_str());
			normalBMFontSingle2->setColor(ccGRAY);
			normalBMFontSingle2->setScale(1);
			CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
			normalBMFontSingle2->setPosition(pos2);
			membersRootNode->addChild(normalBMFontSingle2);
			

		}
		if (isDesOrder)
		{
			EquipDateCollect::EquipIDCollect::reverse_iterator itr = collect.rbegin(); 
			while(itr != collect.rend())
			{
				//
				EquipChangeContentItemData* pItemData = new EquipChangeContentItemData();
				pItemData->m_iEquipID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				EquipChangeContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new EquipChangeContent(*itr, this);
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
			EquipDateCollect::EquipIDCollect::iterator itr = collect.begin(); 
			while(itr != collect.end())
			{
				//
				EquipChangeContentItemData* pItemData = new EquipChangeContentItemData();
				pItemData->m_iEquipID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				EquipChangeContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new EquipChangeContent(*itr, this);
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
	EquipDateCollect::EquipIDCollect collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Change);
	EquipDateCollect::EquipIDCollect::reverse_iterator itr = collect.rbegin(); 
	if(itr == collect.end()){
		int fontWidth =members->getViewSize().width; 
		int fontHeight =members->getViewSize().height; 

		CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
		normalBMFontSingle->setString(Language::Get()->getString("@noEquipToChange").c_str());
		normalBMFontSingle->setColor(ccBLACK);
		normalBMFontSingle->setScale(1);
		CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
		normalBMFontSingle->setPosition(pos);
		membersRootNode->addChild(normalBMFontSingle);

		CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
		normalBMFontSingle2->setString(Language::Get()->getString("@noEquipToChange").c_str());
		normalBMFontSingle2->setColor(ccGRAY);
		normalBMFontSingle2->setScale(1);
		CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
		normalBMFontSingle2->setPosition(pos2);
		membersRootNode->addChild(normalBMFontSingle2);
	}
	for(; itr != collect.rend(); ++itr)
	{
		EquipChangeContent * item = new EquipChangeContent((*itr), this);
		item->init();
		addItem(item);
	}
#endif
}

void EquipChangePage::addItem( ContentBase* item )
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

void EquipChangePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void EquipChangePage::clearAllItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade->clearAllItems();
	m_curSelectedItemData = NULL;
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
	//
	mCurSellectedContent = NULL;
}

void EquipChangePage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void EquipChangePage::reOrderAllItem()
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	this->rebuildAllItem();
	return;
#endif
	membersRootNode->removeAllChildren();
	if(isDesOrder)
	{
		ContentList::iterator itr = mContentList.begin(); 
		for(int i = 0; itr != mContentList.end(); ++itr,++i)
		{
			ContentBase * item = *itr;
			membersRootNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * i);
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height * (i+1));
			members->setContentSize(size);
			members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}
	}
	else
	{
		ContentList::reverse_iterator itr = mContentList.rbegin(); 
		for(int i = 0; itr != mContentList.rend(); ++itr,++i)
		{
			ContentBase * item = *itr;
			membersRootNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * i);
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height * (i+1));
			members->setContentSize(size);
			members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}
	}
}

void EquipChangePage::removeItem( ContentBase* item )
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

void EquipChangePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onConfirm")
	{
#ifdef Macro_TestUseCCReViScrollViewFacade
		if(m_curSelectedItemData == NULL)
#else
		if(mCurSellectedContent == NULL)
#endif
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "TeamPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			OPChangeEquip changeEquip;
		#ifdef Macro_TestUseCCReViScrollViewFacade
			changeEquip.set_id(m_curSelectedItemData->m_iEquipID);
		#else
			changeEquip.set_id( mCurSellectedContent->getContentID() );
		#endif
			changeEquip.set_battleposition( BlackBoard::Get()->CurBattlePosition );
			PacketManager::Get()->sendPakcet(OPCODE_CHANGE_EQUIP_C,&changeEquip);
		}
	}
	else if(itemName=="onOrderByDesc")
	{
		isDesOrder = !isDesOrder;
		reOrderAllItem();
		refreshPage();
	}
}

void EquipChangePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	cocos2d::extension::CCReViSvItemNodeFacade* node = m_pScrollViewFacade->findItemNodeByIndex(tag);
	if(node!=NULL)
	{
		EquipChangeContent* content = dynamic_cast<EquipChangeContent* >(node);
		if(content)
		{
			setSellectedContent(content);
		}
	}
#else
	if(!mContentList.empty() && mContentList.size()>tag && tag>=0)
	{
		ContentBase * cont = mContentList[tag];
		if(cont)
		{
			EquipChangeContent* content = dynamic_cast<EquipChangeContent*>(cont);
			if(content)
			{
				setSellectedContent(content);
			}
		}
	}
#endif
}

void EquipChangePage::refreshPage( void )
{
	CCLabelBMFont* funcLable=dynamic_cast<CCLabelBMFont* >(getVariable("mFuncLabel"));
	if(funcLable)
	{
		if(isDesOrder)
			funcLable->setString(Language::Get()->getString("@OrderByAsce").c_str());
		else
			funcLable->setString(Language::Get()->getString("@OrderByDesc").c_str());
	}

}

void EquipChangePage::setSellectedContent( EquipChangeContent* member )
{
	if(mCurSellectedContent)
		mCurSellectedContent->setSellect(false);

	if(mCurSellectedContent != member)
	{
		mCurSellectedContent = member;
		mCurSellectedContent->setSellect(true);
	}
	else
	{
		mCurSellectedContent = NULL;
	}
}

#ifdef Macro_TestUseCCReViScrollViewFacade
void EquipChangePage::setSellectedContent( EquipChangeContentItemData* member )
{
	if (m_curSelectedItemData)
	{
		m_curSelectedItemData->m_bSelected = false;
		//因为是单选，所以选中另一个时需要把之前选的放掉
		EquipChangeContent* pItemView = (EquipChangeContent*)m_pScrollViewFacade->findItemNodeByItemData(m_curSelectedItemData);
		if (pItemView)
			pItemView->setSellect(false);
	}

	if(m_curSelectedItemData != member)
	{
		m_curSelectedItemData = member;
		member->m_bSelected = true;
	}
	else
	{
		m_curSelectedItemData = NULL;
	}
}
#endif
