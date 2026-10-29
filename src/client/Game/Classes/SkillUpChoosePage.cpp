
#include "stdafx.h"

#include "SkillUpChoosePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Skill.h"

REGISTER_PAGE("SkillUpChoosePage",SkillUpChoosePage);
USING_NS_CC;

void SkillConsumeContent::refreshContent( void )
{
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSelect"));
	//mSelect->unselected();
	mSelect->setNormalImage(::getContentUnselectedSpirte("Normal"));
	mSelect->setSelectedImage(::getContentUnselectedSpirte("Normal"));
	Skill skill(mID);

	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSkillPic"));
	if(prop)
	{
		prop->initWithFile(skill.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mQualityFrame"));
	if(frame)
	{
		frame->setNormalImage(skill.getFrameNormalSpirte());
		frame->setSelectedImage(skill.getFrameSelectedSpirte());
	}
	CCSprite* skillAtkTypePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSkillBuffPic"));
	if(skillAtkTypePic)
	{
		skillAtkTypePic->initWithFile(skill.getSkillAtkTypePic().c_str());
	}
	CCSprite* skillQuality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSkillQuality"));
	if(skillQuality)
	{
		skillQuality->initWithFile(skill.getQualityImageFile().c_str());
	}
	CCLabelBMFont* skillName = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillName"));
	if(skillName)
	{
		skillName->setString(skill.name().c_str());
#if defined(Macro_TestUseCCReViScrollViewFacade) && defined(_DEBUG)
		std::string strTmp = StringConverter::toString(this->m_iIdx) + Language::Get()->getString(skill.name());
		skillName->setString(strTmp.c_str());
#endif
	}
	CCLabelBMFont* skillLevel=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillLevel"));
	if(skillLevel)
	{
		skillLevel->setString(StringConverter::toString(skill.level()).c_str());
	}
	CCLabelBMFont* attackValue = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBuffValue"));
	if(attackValue)
	{
		attackValue->setString(skill.quantityStr().c_str());
	}
	CCLabelBMFont* skillCarriedBy=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillCarriedBy"));
	if(skillCarriedBy)
	{
		skillCarriedBy->setString(skill.getSkillCarriedBy().c_str());
	}
	CCLabelBMFont* skillValue=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillValue"));
	if(skillValue)
	{
		skillValue->setString(StringConverter::toString(skill.getSkillValue()).c_str());
	}
	CCLabelBMFont* skillAtkType = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillAtkType"));
	if(skillAtkType)
	{
		skillAtkType->setString(skill.getSkillAtkType().c_str());
	}

	CCSprite* mMPInBattlePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMPInBattlePic"));
	if(mMPInBattlePic)
	{
		mMPInBattlePic->setVisible(false);
	}

#ifdef Macro_TestUseCCReViScrollViewFacade
	setSellect(m_pItemData->m_bSelected);
#endif
}

void SkillConsumeContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onSelected")
	{
		mOwnner->setSellectedContent(this);
	}
}

void SkillConsumeContent::setSellect( bool sellect )
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

void SkillConsumeContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void SkillConsumeContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	SkillConsumeContentItemData* pItemData = static_cast<SkillConsumeContentItemData*>(pItem);
	if (pItemData)
	{
		this->m_pItemData = pItemData;
		this->mID = pItemData->m_iSkillID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}

#endif

SkillUpChoosePage::SkillUpChoosePage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
	mSelectedContents.clear();
	isDesOrder=false;
}

SkillUpChoosePage::~SkillUpChoosePage(void)
{
}

void SkillUpChoosePage::Enter( MainFrame* frame)
{
	isDesOrder=false;
	rebuildAllItem();

	mSelectedContents.clear();
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		SkillConsumeContent* content = dynamic_cast<SkillConsumeContent* >(*itr);
		if(BlackBoard::Get()->isConsumedSkill(content->getContentID()))
		{
			mSelectedContents.insert(content);
			content->setSellect(true);
		}
		++itr;
	}

	refreshPage();
}

void SkillUpChoosePage::Exit( MainFrame* frame)
{
	clearAllItem();
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
#endif
}

void SkillUpChoosePage::Execute( MainFrame* )
{
	
}

void SkillUpChoosePage::load( void )
{
	loadCcbiFile("SkillUpChoose.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mChooseContent"));
	membersRootNode = members->getContainer();

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif

}

bool SkillFilter_Upgrade(unsigned int id)
{
	if(id == BlackBoard::Get()->UpgradeSkill)
		return false;

	Skill skill(id);
	if(skill.isBornSkill() || skill.isSkillCarriedBy())
		return false;

	return true;
}

void SkillUpChoosePage::buildItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		SkillDateCollect::SkillIDCollect collect = SkillDateCollect::Get()->getSkillCollection(SkillFilter_Upgrade);
		if (!isDesOrder)
		{
			SkillDateCollect::SkillIDCollect::reverse_iterator itr = collect.rbegin(); 
			while(itr != collect.rend())
			{
				//
				SkillConsumeContentItemData* pItemData = new SkillConsumeContentItemData();
				pItemData->m_iSkillID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				SkillConsumeContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new SkillConsumeContent(*itr, this);
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
			SkillDateCollect::SkillIDCollect::iterator itr = collect.begin(); 
			while(itr != collect.end())
			{
				//
				SkillConsumeContentItemData* pItemData = new SkillConsumeContentItemData();
				pItemData->m_iSkillID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				SkillConsumeContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new SkillConsumeContent(*itr, this);
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
	SkillDateCollect::SkillIDCollect collect = SkillDateCollect::Get()->getSkillCollection(SkillFilter_Upgrade);
	SkillDateCollect::SkillIDCollect::reverse_iterator itr = collect.rbegin(); 
	for(; itr != collect.rend(); ++itr)
	{
		SkillConsumeContent * item = new SkillConsumeContent((*itr), this);
		item->init();
		addItem(item);
	}
#endif
}

void SkillUpChoosePage::addItem( ContentBase* item )
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

void SkillUpChoosePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void SkillUpChoosePage::clearAllItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade->clearAllItems();
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

void SkillUpChoosePage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void SkillUpChoosePage::removeItem( ContentBase* item )
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

void SkillUpChoosePage::reOrderAllItem()
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	this->rebuildAllItem();
	return;
#endif
	membersRootNode->removeAllChildren();
	if(!isDesOrder)
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

void SkillUpChoosePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onConfirm")
	{
		BlackBoard::Get()->resetSkillConsume();
		std::set<SkillConsumeContent*>::iterator itr = mSelectedContents.begin();
		while(itr != mSelectedContents.end())
		{
			BlackBoard::Get()->addSkillConsume((*itr)->getContentID());
			++itr;
		}
		MsgMainFrameChangePage msg;
		msg.pageName = "SkillUpgradePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onOrderByDesc")
	{
		isDesOrder = !isDesOrder;
		reOrderAllItem();
		refreshPage();
	}
}

void SkillUpChoosePage::refreshPage( void )
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

	CCLabelBMFont* cardnum = dynamic_cast<CCLabelBMFont* >(getVariable("mCardnum"));
	if(cardnum)
	{
		cardnum->setString(StringConverter::toString(mSelectedContents.size()).c_str());
	}

	CCLabelBMFont* funcLable=dynamic_cast<CCLabelBMFont* >(getVariable("mFuncLble"));
	if(funcLable)
	{
		if(isDesOrder)
			funcLable->setString(Language::Get()->getString("@OrderByAsce").c_str());
		else
			funcLable->setString(Language::Get()->getString("@OrderByDesc").c_str());
	}
}

void SkillUpChoosePage::setSellectedContent( SkillConsumeContent* member )
{
	if(mSelectedContents.count(member) != 0)
	{
		member->setSellect(false);
		member->m_pItemData->m_bSelected = false;
		mSelectedContents.erase(member);
	}
	else
	{
		if(mSelectedContents.size() >= 5)
		{

		}
		else
		{
			mSelectedContents.insert(member);
			member->setSellect(true);
			member->m_pItemData->m_bSelected = true;
		}
	}

	CCLabelBMFont* cardnum = dynamic_cast<CCLabelBMFont* >(getVariable("mCardnum"));
	if(cardnum)
	{
		cardnum->setString(StringConverter::toString(mSelectedContents.size()).c_str());
	}
}
