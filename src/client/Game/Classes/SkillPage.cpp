
#include "stdafx.h"

#include "SkillPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"

REGISTER_PAGE("SkillPage",SkillPage);
USING_NS_CC;

void SkillContent::refreshContent( void )
{
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
	CCMenuItemImage* upgrade=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSkillUpgrade"));
	if(upgrade)
	{
		if(skill.allowUpgrade())
			upgrade->setEnabled(true);
		else
			upgrade->setEnabled(false);
	}
	CCSprite* skillAtkTypePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSkillAtkTypePic"));
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
	CCLabelBMFont* attackValue = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mAttackValue"));
	if(attackValue)
	{
		attackValue->setString(skill.quantityStr().c_str());
	}

	CCLabelBMFont* skillCarriedBy=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillCarriedBy"));
	if(skillCarriedBy)
	{
		skillCarriedBy->setString(skill.getSkillCarriedBy().c_str());
	}
	CCSprite* mMPInBattlePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMPInBattlePic"));

	std::string skillCarriedStr = skillCarriedBy->getString();
	if(skillCarriedStr!=""){
		mMPInBattlePic->setVisible(true);
	}else{
		mMPInBattlePic->setVisible(false);
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

}

void SkillContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	//todo
	if(itemName=="onSkillUpgrade")
	{
		BlackBoard::Get()->resetSkillConsume();
		BlackBoard::Get()->UpgradeSkill = mID;
		BlackBoard::Get()->jumpSkillPageName = "SkillPage";
		MsgMainFrameChangePage msg;
		msg.pageName = "SkillUpgradePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowMemberInfo")
	{
		BlackBoard::Get()->NeedChangeSkill= 0;
		BlackBoard::Get()->NeedChooseSkill = 0;
		BlackBoard::Get()->ShowSkill = mID;
		BlackBoard::Get()->jumpSkillPageName = "SkillPage";
		MsgMainFramePushPage msg;
		msg.pageName = "SkillInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

#ifdef Macro_TestUseCCReViScrollViewFacade

void SkillContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void SkillContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	SkillContentItemData* pItemData = static_cast<SkillContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iSkillID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}

#endif

SkillPage::SkillPage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mLongBtnHeight = 0.0f;
	mContentList.clear();
}

SkillPage::~SkillPage(void)
{
}

void SkillPage::Enter( MainFrame* frame)
{
	rebuildAllItem();
	refreshPage();
}

void SkillPage::Exit( MainFrame* frame)
{
	clearAllItem();
	//
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
#endif
}

void SkillPage::Execute( MainFrame* )
{

}

void SkillPage::load( void )
{
	loadCcbiFile("SkillPage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mSkillContent"));
	membersRootNode = members->getContainer();

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif

}

bool SkillSort_Skill( unsigned int id1, unsigned int id2 )
{
	Skill skill1(id1);
	Skill skill2(id2);

	std::string carriedByStr1 = skill1.getSkillCarriedBy();
	std::string carriedByStr2 = skill2.getSkillCarriedBy();
	if(carriedByStr1!=""&&carriedByStr2=="")
	{
		return false;
	}
	else if(carriedByStr2!=""&&carriedByStr1=="")
	{
		return true;
	}

	if(skill1.quality() < skill2.quality())
		return false;
	else if(skill2.quality() < skill1.quality())
		return true;

	if(skill1.level() > skill2.level())
		return false;
	else if(skill2.level() > skill1.level())
		return true;

	if(skill1.getSkillValue() > skill2.getSkillValue())
		return false;
	else if(skill2.getSkillValue() > skill1.getSkillValue())
		return true;

	return false;
}

void SkillPage::buildItem( void )
{

	CCBContainer* longButton = CCBManager::Get()->createAndLoad("LongButton.ccbi");
	longButton->setListener(this,999);
	if(longButton)
	{
		CCLabelBMFont* goGetBtnLabel=dynamic_cast<CCLabelBMFont*>(longButton->getVariable(("mButtonLabel")));
		if(goGetBtnLabel)
		{
			goGetBtnLabel->setString(Language::Get()->getString("@GoCreerGetSkill").c_str());
		}
		membersRootNode->addChild(longButton);
		CCPoint pos = ccp(0,0);
		longButton->setPosition(pos);
		mLongBtnHeight =  longButton->getContentSize().height;
		CCSize size = CCSize(longButton->getContentSize().width, mLongBtnHeight);
		members->setContentSize(size);
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
	}

	SkillDateCollect::SkillIDCollect collect = SkillDateCollect::Get()->getSkillCollection(::skillFilter_default,SkillSort_Skill);
	SkillDateCollect::SkillIDCollect::iterator itr = collect.begin(); 
	
#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		//
		while(itr != collect.end())
		{
			//
			SkillContentItemData* pItemData = new SkillContentItemData();
			pItemData->m_iSkillID = *itr;
			pItemData->m_iIdx = iCount;
			//
			pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount+mLongBtnHeight);
			//
			SkillContent* pItem = NULL;
			if (iCount < iMaxNode)
			{
				//
				pItem = new SkillContent(*itr);
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

	for(; itr != collect.end(); ++itr)
	{
		SkillContent * item = new SkillContent((*itr));
		item->init();
		addItem(item);
	}
#endif

}

void SkillPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1)+mLongBtnHeight);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size()+mLongBtnHeight);
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void SkillPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void SkillPage::clearAllItem( void )
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
}

void SkillPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void SkillPage::removeItem( ContentBase* item )
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

void SkillPage::refreshPage( void )
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
void SkillPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onFunciton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "CareerPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}