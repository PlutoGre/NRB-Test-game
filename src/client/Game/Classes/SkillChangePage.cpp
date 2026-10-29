
#include "stdafx.h"

#include "SkillChangePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "Disciple.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("SkillChangePage",SkillChangePage);
USING_NS_CC;

void SkillChangeContent::refreshContent( void )
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

	CCSprite* mMPInBattlePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMPInBattlePic"));
	CCLabelBMFont* skillCarriedBy=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillCarriedBy"));
	if(skillCarriedBy)
	{
		carriedByDiscipleName = skill.getSkillCarriedBy();
		if(carriedByDiscipleName!="")
		{
			char buff[128];
			std::string s= Language::Get()->getString("@CarriedBy");
			sprintf(buff,s.c_str(),carriedByDiscipleName.c_str());
			skillCarriedBy->setString(buff);
			mMPInBattlePic->setVisible(true);
		}
		else
		{
			skillCarriedBy->setString("");
			mMPInBattlePic->setVisible(false);
		}
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
#ifdef Macro_TestUseCCReViScrollViewFacade
	setSellect(m_pItemData->m_bSelected);
#endif
}

void SkillChangeContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onSelected")
	{
		mOwnner->setSellectedContent(this);
		if(carriedByDiscipleName!="" && mSelect->isSelected())
		{
			char buff[128];
			std::string s= Language::Get()->getString("@AlreadyUsedBy");
			sprintf(buff,s.c_str(),carriedByDiscipleName.c_str());
			MSG_BOX(buff);
		}
	}
}

void SkillChangeContent::setSellect( bool sellect )
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

void SkillChangeContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void SkillChangeContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	SkillChangeContentItemData* pItemData = static_cast<SkillChangeContentItemData*>(pItem);
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

SkillChangePage::SkillChangePage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
	mCurSellectedContent = NULL;
	isDesOrder=true;
}

SkillChangePage::~SkillChangePage(void)
{
}

void SkillChangePage::Enter( MainFrame* frame)
{
	isDesOrder=true;
	rebuildAllItem();
	mCurSellectedContent = NULL;
	refreshPage();
}

void SkillChangePage::Exit( MainFrame* frame)
{
	clearAllItem();
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
#endif
}

void SkillChangePage::Execute( MainFrame* )
{

}

void SkillChangePage::load( void )
{
	loadCcbiFile("SkillsChanged.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mChangeContent"));
	membersRootNode = members->getContainer();

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif

}

bool SkillFilter_Change(unsigned int id)
{
	const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(BlackBoard::Get()->CurBattlePosition);
	Skill skill(id);

	//是天赋技能
	if(skill.isBornSkill())
		return false;

	//和天赋技能一样
	Skill bornSkill(Disciple(info->discipleId).skill1());
	if(skill.itemID() == bornSkill.itemID() )
	{
		return false;
	}

	unsigned int CurChangeSkillID = 0;
	if(BlackBoard::Get()->CurChangeSkillPosition == 1)
	{
		CurChangeSkillID = info->skill1;
		//和技能槽2的技能一样
		if( info->skill2 && Skill(info->skill2).itemID() == skill.itemID() )
			return false;
	}
	else if(BlackBoard::Get()->CurChangeSkillPosition == 2)
	{
		CurChangeSkillID = info->skill2;
		//和技能槽1的技能一样
		if( info->skill1 && Skill(info->skill1).itemID() == skill.itemID() )
			return false;
	}

	if(CurChangeSkillID != 0)
	{
		//和当前技能一样，并且级别也一样
		Skill skillNow(CurChangeSkillID);
		if(skillNow.itemID() == skill.itemID() && skill.level() == skillNow.level())
		{
			return false;
		}
	}
	
	return true;
}

void SkillChangePage::buildItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		SkillDateCollect::SkillIDCollect collect = SkillDateCollect::Get()->getSkillCollection(SkillFilter_Change);
		if(collect.rbegin() == collect.rend()){
			int fontWidth =members->getViewSize().width; 
			int fontHeight =members->getViewSize().height; 

			CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
			normalBMFontSingle->setString(Language::Get()->getString("@noSkillToChange").c_str());
			normalBMFontSingle->setColor(ccBLACK);
			normalBMFontSingle->setScale(1);
			CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
			normalBMFontSingle->setPosition(pos);
			membersRootNode->addChild(normalBMFontSingle);

			CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
			normalBMFontSingle2->setString(Language::Get()->getString("@noSkillToChange").c_str());
			normalBMFontSingle2->setColor(ccGRAY);
			normalBMFontSingle2->setScale(1);
			CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
			normalBMFontSingle2->setPosition(pos2);
			membersRootNode->addChild(normalBMFontSingle2);

		}
		if (!isDesOrder)
		{
			SkillDateCollect::SkillIDCollect::reverse_iterator itr = collect.rbegin(); 
			while(itr != collect.rend())
			{
				//
				SkillChangeContentItemData* pItemData = new SkillChangeContentItemData();
				pItemData->m_iSkillID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				SkillChangeContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new SkillChangeContent(*itr, this);
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
				SkillChangeContentItemData* pItemData = new SkillChangeContentItemData();
				pItemData->m_iSkillID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				SkillChangeContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new SkillChangeContent(*itr, this);
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
	SkillDateCollect::SkillIDCollect collect = SkillDateCollect::Get()->getSkillCollection(SkillFilter_Change);
	SkillDateCollect::SkillIDCollect::iterator itr = collect.begin(); 
	if(itr == collect.end()){
		int fontWidth =members->getViewSize().width; 
		int fontHeight =members->getViewSize().height; 

		CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
		normalBMFontSingle->setString(Language::Get()->getString("@noSkillToChange").c_str());
		normalBMFontSingle->setColor(ccBLACK);
		normalBMFontSingle->setScale(1);
		CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
		normalBMFontSingle->setPosition(pos);
		membersRootNode->addChild(normalBMFontSingle);

		CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
		normalBMFontSingle2->setString(Language::Get()->getString("@noSkillToChange").c_str());
		normalBMFontSingle2->setColor(ccGRAY);
		normalBMFontSingle2->setScale(1);
		CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
		normalBMFontSingle2->setPosition(pos2);
		membersRootNode->addChild(normalBMFontSingle2);
	}
	for(; itr != collect.end(); ++itr)
	{
		SkillChangeContent * item = new SkillChangeContent((*itr), this);
		item->init();
		addItem(item);
	}
#endif
}

void SkillChangePage::addItem( ContentBase* item )
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

void SkillChangePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void SkillChangePage::clearAllItem( void )
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
	mCurSellectedContent = NULL;
}

void SkillChangePage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void SkillChangePage::removeItem( ContentBase* item )
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

void SkillChangePage::reOrderAllItem()
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

void SkillChangePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onConfirm")
	{
		if(mCurSellectedContent == NULL)
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "TeamPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			OPChangeSkill changeSkill;
			changeSkill.set_newskillid( mCurSellectedContent->getContentID() );
			changeSkill.set_battleposition(BlackBoard::Get()->CurBattlePosition);
			changeSkill.set_skillposition( BlackBoard::Get()->CurChangeSkillPosition );
			PacketManager::Get()->sendPakcet(OPCODE_CHANGE_SKILL_C,&changeSkill);
		}
	}
	else if(itemName=="onOrderByDesc")
	{
		isDesOrder = !isDesOrder;
		reOrderAllItem();
		refreshPage();
	}
}

void SkillChangePage::refreshPage( void )
{
	CCLabelBMFont* funcLable=dynamic_cast<CCLabelBMFont* >(getVariable("mFuncLable"));
	if(funcLable)
	{
		if(isDesOrder)
			funcLable->setString(Language::Get()->getString("@OrderByAsce").c_str());
		else
			funcLable->setString(Language::Get()->getString("@OrderByDesc").c_str());
	}
}

void SkillChangePage::setSellectedContent( SkillChangeContent* member )
{
	if(mCurSellectedContent)
	{
		mCurSellectedContent->m_pItemData->m_bSelected = false;
		mCurSellectedContent->setSellect(false);
	}

	if(mCurSellectedContent != member)
	{
		mCurSellectedContent = member;
		mCurSellectedContent->m_pItemData->m_bSelected = true;
		mCurSellectedContent->setSellect(true);
	}
	else
	{
		mCurSellectedContent = NULL;
	}
}
