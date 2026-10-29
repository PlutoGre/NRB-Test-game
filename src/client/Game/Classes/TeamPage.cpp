
#include "stdafx.h"

#include "TeamPage.h"
#include "Disciple.h"
#include "ServerDateManager.h"
#include "Skill.h"
#include "Equip.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "GameMessages.h"
#include "DataTableManager.h"
#include "MessageBoxPage.h"
#include "BattleArrayInfo.h"
#include "Title.h"
#include "GameMaths.h"
REGISTER_PAGE("TeamPage",TeamPage);
USING_NS_CC;

void PersonCardContent::refreshContent( void )
{
	Disciple disciple(ServerDateManager::Get()->getUserBattleInfo(mID)->discipleId);
	mContentContainer->runAnimation(disciple.backPic());
	CCSprite* memPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMemPic"));
	if(memPic)
	{
		memPic->setTexture(disciple.bodyPic().c_str());
	}
	CCSprite* memName=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMemName"));
	if(memName)
	{
		memName->setVisible(false);
		//memName->setTexture(disciple.namePic().c_str());
	}
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setColor(ccBLACK));
	std::list<std::string> _list;
	_list.push_back(disciple.name());
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@DiscipleNameTitle"),&_list);
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(mContentContainer,"mQualitybg",CCSprite,setTexture(disciple.getQualityBgImageFile().c_str()));
	CCSprite* qualityPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mQualityPic"));
	if(qualityPic)
	{
		if(memName)
		{
//			qualityPic->setPositionY(memName->getPositionY() - memName->getAnchorPoint().y * memName->getContentSize().height - 5);
		}
		qualityPic->setTexture(disciple.getQualityImageFile(true).c_str());
	}
	CCSprite* shadowPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mShadowPic"));
	if(shadowPic)
	{
		shadowPic->setVisible(false);
	}
	CCLabelBMFont* memLV=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMemLV"));
	if(memLV)
	{
		memLV->setString(StringConverter::toString(disciple.level()).c_str());
	}
	CCLabelBMFont* breakNum=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBreakNum"));
	if(breakNum)
	{
		breakNum->setString(("+"+StringConverter::toString(disciple.upgradelevel())).c_str());
	}
	// 	CCLabelBMFont* breakNum=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBreakNum"));
	// 	if(breakNum)
	// 	{
	// 		breakNum->setString(StringConverter::toString(disciple.upgradelevel()).c_str());
	// 	}
	CCLabelBMFont* exp=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mExp"));
	if(exp)
	{
		int expTotal = disciple.getLevelupNeededExp();
		int expNum = 0;
		if(disciple.hasDiscipleInfo())
			expNum = disciple.getLevelExp();
		if(expNum<0)expNum = 0;
		expTotal+=expNum;

		std::string expString = StringConverter::toString(expNum) + "/" + StringConverter::toString(expTotal);
		exp->setString(expString.c_str());

		float expScale = 0;
		if(expTotal>0)
			expScale = ((float)expNum)/((float)expTotal);
		if(expScale>1.0f)expScale = 1.0f;
		CCB_FUNC(mContentContainer,"mMemEXP",CCSprite,setScaleX(expScale));

	}
}

void PersonCardContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
	{
		mOwnner->setTarget(mID + 1);
	}
	else if(itemName =="onShowMemInfo")
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(mID);
		Disciple disciple(info->discipleId);

		BlackBoard::Get()->CurChangeDiscipleID = disciple.id();
		BlackBoard::Get()->NeedChangeDisciple = true;
		BlackBoard::Get()->NeedChooseDisciple = false;
		BlackBoard::Get()->jumpDisciplePageName = "TeamPage";
		BlackBoard::Get()->ShowDisciple = disciple.id();
		BlackBoard::Get()->CurBattlePosition=mID;
		MsgMainFramePushPage msg;
		msg.pageName = "DiscipleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

 void PersonCardContent::setContentIDWithRemoveTextureCache( unsigned int id )
 {
 	UserBattleInfo* pInfo = ServerDateManager::Get()->getUserBattleInfo(id);
 	unsigned int iTmp = pInfo != NULL ? pInfo->discipleId : 0;//这里取出真正
 	//if (mID == id)//id只是索引
 	//	return;
 	//
 	if (m_iDiscipleId == iTmp)
 		return;
 	//
 	//if (mID > 0)//has == 0
 	if (m_iDiscipleId > 0)
 	{
 		Disciple disciple(m_iDiscipleId);
 		//
 		//to do: 如何判定disciple的有效性？
 		//
 		this->mContentContainer->stopAllActions();
 		//
 		CCTextureCache::sharedTextureCache()->removeTextureForKey(disciple.bodyPic().c_str());
 		CCTextureCache::sharedTextureCache()->removeTextureForKey(disciple.namePic().c_str());
 		CCTextureCache::sharedTextureCache()->removeTextureForKey(disciple.getQualityImageFile().c_str());
 		//
 		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(disciple.bodyPic().c_str());
 		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(disciple.namePic().c_str());
 		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(disciple.getQualityImageFile().c_str());
 	}
 	//
 	mID = id;
 	m_iDiscipleId = iTmp;
 }

 void PersonCardContent::setContentID( unsigned int id )
 {
 	setContentIDWithRemoveTextureCache(id);
 }

PersonCardContent::~PersonCardContent()
{
	setContentIDWithRemoveTextureCache(-1);//把图片资源release
}

PersonCardContent::PersonCardContent( unsigned int id, TeamPage* ownner ) :ContentBase(id)
	,mOwnner(ownner)
{
	m_iDiscipleId = ServerDateManager::Get()->getUserBattleInfo(mID)->discipleId;
}

void PersonChooseCardContent::refreshContent( void )
{

}

void PersonChooseCardContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onReplaceMem")
	{
		BlackBoard::Get()->NeedChangeDisciple = false;
		BlackBoard::Get()->CurChangeDiscipleID = 0;
		BlackBoard::Get()->jumpDisciplePageName = "TeamPage";
		BlackBoard::Get()->CurBattlePosition = mID;

		MsgMainFrameChangePage msg;
		msg.pageName = "DiscipleChangePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void TeamOverviewCardContent::refreshContent( void )
{
	for(int i = 0; i<MaxBattleDiscipleNum; ++i)
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(i);
		if(info && info->discipleId != 0)
		{
			Disciple disciple(info->discipleId);

			CCNode* mem=dynamic_cast<CCNode* >(mContentContainer->getVariable("mMem"+StringConverter::toString(i+1)));
			if(mem)
			{
				mem->setVisible(true);
			}
			CCLabelBMFont* memName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMem"+StringConverter::toString(i+1)+"Name"));
			if(memName)
			{
				memName->setString(disciple.name().c_str());
			}
			CCSprite* memQuality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMem"+StringConverter::toString(i+1)+"Quality"));
			if(memQuality)
			{
				memQuality->initWithFile(disciple.getQualityImageFile().c_str());
			}
			CCLabelBMFont* memLV=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMem"+StringConverter::toString(i+1)+"LV"));
			if(memLV)
			{
				memLV->setString(StringConverter::toString(disciple.level()).c_str());
			}
		}
		else
		{
			CCNode* mem=dynamic_cast<CCNode* >(mContentContainer->getVariable("mMem"+StringConverter::toString(i+1)));
			if(mem)
			{
				mem->setVisible(false);
			}
		}
	}
}

void IconContent::setSellect( bool sellect )
{
	if(sellect)
	{
		mFrameBack->selected();
	}
	else
	{
		mFrameBack->unselected();
	}
}

void PersonIconContent::refreshContent( void )
{
	const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(mID);
	if(info && info->discipleId != 0)
	{
		Disciple disciple(info->discipleId);
		CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame"));
		if(frame)
		{
			frame->initWithFile(disciple.iconPic().c_str());
			frame->setAnchorPoint(CCPointZero);
		}
		mFrameBack = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFrameBack"));
		if(mFrameBack)
		{
			mFrameBack->setNormalImage(disciple.getFrameNormalSpirte());
			//mFrameBack->setSelectedImage(disciple.getFrameSelectedSpirte());
		}
	}
	else
	{
		CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame"));
		if(frame)
		{
			frame->initWithFile(::getTeamDefaultPic().c_str());
			frame->setAnchorPoint(CCPointZero);
		}
		mFrameBack = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFrameBack"));
		if(mFrameBack)
		{
			mFrameBack->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			//mFrameBack->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
	}
	CCB_FUNC(mContentContainer,"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
	if(mFrameBack)
	{
		if(mFrameBack->getSelectedImage()->getContentSize().width>mFrameBack->getNormalImage()->getContentSize().width)
		{
			bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
			if(isNeedSetAnchor)
			{
				float scale=(mFrameBack->getSelectedImage()->getContentSize().width-mFrameBack->getNormalImage()->getContentSize().width)*0.5/mFrameBack->getSelectedImage()->getContentSize().width;
				mFrameBack->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
			}
		}
	}
}

void PersonIconContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
	{
#if defined(WIN32) && defined(_DEBUG)
		GamePrecedure::Get()->gameSnapshot();
#endif
		mOwnner->setTarget(mID + 1);
	}
}

void TeamOverviewIconContent::refreshContent( void )
{
	CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame"));
	if(frame)
	{
		frame->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_TeamOverview").c_str());
		frame->setAnchorPoint(CCPointZero);
	}
	mFrameBack = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFrameBack"));
	if(mFrameBack)
	{
		mFrameBack->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
		//mFrameBack->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
	}
	CCB_FUNC(mContentContainer,"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
	if(mFrameBack)
	{
		if(mFrameBack->getSelectedImage()->getContentSize().width>mFrameBack->getNormalImage()->getContentSize().width)
		{
			bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
			if(isNeedSetAnchor)
			{
				float scale=(mFrameBack->getSelectedImage()->getContentSize().width-mFrameBack->getNormalImage()->getContentSize().width)*0.5/mFrameBack->getSelectedImage()->getContentSize().width;
				mFrameBack->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
			}
		}
	}
}

void TeamOverviewIconContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
	{
		mOwnner->setTarget(mID + 1);
	}
}

void TeamUnlockIconContent::refreshContent( void )
{
	CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame"));
	if(frame)
	{
		frame->initWithFile(::getTeamLockPic().c_str());
		frame->setAnchorPoint(CCPointZero);
	}
	mFrameBack = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFrameBack"));
	if(mFrameBack)
	{
		mFrameBack->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
		mFrameBack->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
	}
	CCB_FUNC(mContentContainer,"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
	if(mFrameBack)
	{
		if(mFrameBack->getSelectedImage()->getContentSize().width>mFrameBack->getNormalImage()->getContentSize().width)
		{
			bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
			if(isNeedSetAnchor)
			{
				float scale=(mFrameBack->getSelectedImage()->getContentSize().width-mFrameBack->getNormalImage()->getContentSize().width)*0.5/mFrameBack->getSelectedImage()->getContentSize().width;
				mFrameBack->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
			}
		}
	}
}

void TeamUnlockIconContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
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

TeamPage::TeamPage(void)
{
	mCurrIndex = 0;
}

TeamPage::~TeamPage(void)
{
}

void TeamPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	rebuildAllItem();
	if(BlackBoard::Get()->TeamShowIndex!=0)
	{
		mCurrIndex = BlackBoard::Get()->TeamShowIndex;
		BlackBoard::Get()->TeamShowIndex = 0;
	}
	ContentList::iterator itr = mPersonIconContentList.find(mCurrIndex);
	if(itr== mPersonIconContentList.end())
	{
		setTarget(1);
	}
	else
	{
		setTarget(mCurrIndex);
		refreshPage();
	}

}

void TeamPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void TeamPage::Execute( MainFrame* )
{

}

void TeamPage::load( void )
{
	loadCcbiFile("TeamPage.ccbi");
	mPersonCards =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mPerson"));
	mPersonCardRootNode = mPersonCards->getContainer();
	mPersonCards->setDelegate(this);
	mPersonCards->setScrollDeaccelRate(0);
	mPersonCards->setBounceable(false);

	mPersonIcons =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mMembers"));
	mPersonIconRootNode = mPersonIcons->getContainer();
}

void TeamPage::scrollViewDidDeaccelerateStop( cocos2d::extension::CCScrollView* view ,cocos2d::CCPoint initSpeed)
{
	if(view)
	{
		float mPageWidth = mPersonCardContentList[1]->getContainerNode()->getContentSize().width;
		float currentOffset = view->getContentOffset().x;

		int t;
		if(initSpeed.x<0 && mCurrIndex<mPersonCardContentList.size())
			t = mCurrIndex+1;
		else if(initSpeed.x>0 && mCurrIndex > 1)
			t = mCurrIndex-1;
		else
			t = mCurrIndex;
		setTarget(t, true);
	}
}

void TeamPage::buildItem( void )
{
	int i = 0;
	unsigned int battleEnableSize = BattleArrayInfo::Get()->getBattleEnableSize();
	for(; i<battleEnableSize; ++i)
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(i);
		if(info&&(info->discipleId))
		{
			PersonCardContent * itemCard = new PersonCardContent(i, this);
			itemCard->init();
			addCardItem(itemCard);
		}
		else
		{
			PersonChooseCardContent * itemCard = new PersonChooseCardContent(i, this);
			itemCard->init();
			addCardItem(itemCard);
		}
		PersonIconContent * itemIcon = new PersonIconContent(i, this);
		itemIcon->init();
		addIconItem(itemIcon);
	}

	TeamOverviewCardContent * teamOverviewCard = new TeamOverviewCardContent(i);
	teamOverviewCard->init();
	addCardItem(teamOverviewCard);

	TeamOverviewIconContent * teamOverviewIcon = new TeamOverviewIconContent(i, this);
	teamOverviewIcon->init();
	addIconItem(teamOverviewIcon);

	TeamUnlockIconContent * teamUnlockIcon = new TeamUnlockIconContent(i+1);
	teamUnlockIcon->init();
	addIconItem(teamUnlockIcon);

	mPersonCards->forceRecaculateChildren();
	mPersonIcons->forceRecaculateChildren();
}

void TeamPage::addCardItem( ContentBase* item )
{
	CCBContainer* container = item->getContainerNode();
	if(container)
	{
		mPersonCardContentList.insert(std::make_pair<unsigned int, ContentBase *>(mPersonCardContentList.size() + 1, item));
		mPersonCardRootNode->addChild(container);

		CCPoint pos = ccp(container->getContentSize().width * (mPersonCardContentList.size()-1), 0);
		container->setPosition(pos);
		CCSize size = CCSize(container->getContentSize().width * mPersonCardContentList.size(), container->getContentSize().height);
		mPersonCards->setContentSize(size);
		mPersonCards->forceRecaculateChildren();
	}
}

void TeamPage::addIconItem( ContentBase* item )
{
	CCBContainer* container = item->getContainerNode();
	if(container)
	{
		mPersonIconContentList.insert(std::make_pair<unsigned int, ContentBase *>(mPersonIconContentList.size() + 1, item));
		mPersonIconRootNode->addChild(container);

		CCPoint pos = ccp(container->getContentSize().width * (mPersonIconContentList.size()-1), 0);
		container->setPosition(pos);
		CCSize size = CCSize(container->getContentSize().width * mPersonIconContentList.size(), container->getContentSize().height);
		mPersonIcons->setContentSize(size);
	}
}

void TeamPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void TeamPage::clearAllItem( void )
{
	ContentList::iterator itrCard = mPersonCardContentList.begin();
	while(itrCard != mPersonCardContentList.end())
	{
		delete itrCard->second;
		++itrCard;
	}
	mPersonCardContentList.clear();
	mPersonCardRootNode->removeAllChildren();

	ContentList::iterator itrIcon = mPersonIconContentList.begin();
	while(itrIcon != mPersonIconContentList.end())
	{
		delete itrIcon->second;
		++itrIcon;
	}
	mPersonIconContentList.clear();
	mPersonIconRootNode->removeAllChildren();
}

void TeamPage::refreshPage( void )
{
	ContentList::iterator it = mPersonCardContentList.find(mCurrIndex);
	if(it!=mPersonCardContentList.end() && it->second)
	{
		unsigned int id = it->second->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);
		if(info/*&&info->discipleId*/)
		{
			refreshPersonPage();
		}
		else
		{
			refreshOverviewPage();
		}
	}
}

void TeamPage::setTarget( unsigned int index, bool isDuration/*=false*/)
{
	ContentList::iterator itr = mPersonIconContentList.find(index);
	if(mCurrIndex != index)
	{
		itr = mPersonIconContentList.find(mCurrIndex);
		if(itr!= mPersonIconContentList.end())
		{
			dynamic_cast<IconContent*>(itr->second)->setSellect(false);
		}

		mCurrIndex = index;

		itr = mPersonIconContentList.find(mCurrIndex);
		if(itr!= mPersonIconContentList.end())
		{
			dynamic_cast<IconContent*>(itr->second)->setSellect(true);
		}

		refreshPage();
	}

	if(itr!= mPersonIconContentList.end())
	{
		if(isDuration)
		{
			float mPageWidth = mPersonCardContentList[index]->getContainerNode()->getContentSize().width;
			float offsetx = (1.0f - (float)index) * mPageWidth;
			mPersonCards->setContentOffsetInDuration(ccp(offsetx, mPersonCards->getContentOffset().y),0.2f);

			resetNavOffset();
		}
		else
		{
			float mPageWidth = mPersonCardContentList[index]->getContainerNode()->getContentSize().width;
			float offsetx = (1.0f - (float)index) * mPageWidth;
			mPersonCards->setContentOffset(ccp(offsetx, mPersonCards->getContentOffset().y));
		}
	}
}

void TeamPage::resetNavOffset(void)
{
	float currentOffset = mPersonIcons->getContentOffset().x;
	float iconWidth = mPersonIconContentList[mCurrIndex]->getContainerNode()->getContentSize().width;

	float targetOffset = (float)(mCurrIndex -1) * iconWidth + currentOffset;
	if(targetOffset < 0)
	{
		currentOffset = - (float)(mCurrIndex -1) * iconWidth;
		mPersonIcons->setContentOffsetInDuration(ccp(currentOffset,mPersonIcons->getContentOffset().y),0.3f);
	}
	else if(targetOffset > mPersonIcons->getViewSize().width - iconWidth)
	{
		currentOffset = mPersonIcons->getViewSize().width - mCurrIndex * iconWidth;
		mPersonIcons->setContentOffsetInDuration(ccp(currentOffset,mPersonIcons->getContentOffset().y),0.3f);
	}
}

void TeamPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onEmbattle")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "EmbattlePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onSkill1")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);

		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 1;

			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			if(info->discipleId != 0)
			{
				Disciple disciple(info->discipleId);
				BlackBoard::Get()->NeedChangeSkill= 0;
				BlackBoard::Get()->NeedChooseSkill = 0;
				BlackBoard::Get()->jumpSkillPageName = "TeamPage";
				BlackBoard::Get()->ShowSkill = disciple.skill1();

				MsgMainFramePushPage msg;
				msg.pageName = "SkillInfoPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{

			}
		}
	}
	else if(itemName == "onSkill2")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);

		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 2;

			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			BlackBoard::Get()->CurChangeSkillPosition = 1;
			BlackBoard::Get()->CurBattlePosition = info->position;
			BlackBoard::Get()->jumpSkillPageName = "TeamPage";
			if(info->skill1 != 0)
			{
				BlackBoard::Get()->NeedChangeSkill= 1;
				BlackBoard::Get()->NeedChooseSkill = 0;
				BlackBoard::Get()->ShowSkill = info->skill1;

				MsgMainFramePushPage msg;
				msg.pageName = "SkillInfoPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(info->discipleId != 0)
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "SkillChangePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{

			}
		}		
	}
	else if(itemName == "onSkill3")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);
		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 4;

			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		
		}
		else
		{
			BlackBoard::Get()->CurChangeSkillPosition = 2;
			BlackBoard::Get()->CurBattlePosition = info->position;
			BlackBoard::Get()->jumpSkillPageName = "TeamPage";
			if(info->skill2 != 0)
			{
				BlackBoard::Get()->NeedChangeSkill= 1;
				BlackBoard::Get()->NeedChooseSkill = 0;
				BlackBoard::Get()->ShowSkill = info->skill2;

				MsgMainFramePushPage msg;
				msg.pageName = "SkillInfoPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(info->discipleId != 0)
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "SkillChangePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{

			}
		}
	}
	else if(itemName == "onWeapon")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);
		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 3;

			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			BlackBoard::Get()->CurChangeEquipID = info->equip1;
			BlackBoard::Get()->CurChangeEquipType = 1;
			BlackBoard::Get()->CurBattlePosition = info->position;
			BlackBoard::Get()->jumpEquipPageName = "TeamPage";
			if(info->equip1 != 0)
			{
				BlackBoard::Get()->NeedChangeEquip = true;
				BlackBoard::Get()->NeedChooseEquip = false;
				BlackBoard::Get()->ShowEquip = info->equip1;

				MsgMainFramePushPage msg;
				msg.pageName = "EquipInfoPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(info->discipleId != 0)
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "EquipChangePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{

			}
		}
		
	}
	else if(itemName == "onArmor")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);

		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 5;

			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			BlackBoard::Get()->CurChangeEquipID = info->equip2;
			BlackBoard::Get()->CurChangeEquipType = 2;
			BlackBoard::Get()->CurBattlePosition = info->position;
			BlackBoard::Get()->jumpEquipPageName = "TeamPage";
			if(info->equip2 != 0)
			{
				BlackBoard::Get()->NeedChangeEquip = true;
				BlackBoard::Get()->NeedChooseEquip = false;
				BlackBoard::Get()->ShowEquip = info->equip2;

				MsgMainFramePushPage msg;
				msg.pageName = "EquipInfoPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(info->discipleId != 0)
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "EquipChangePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{

			}
		}
	}
	else if(itemName == "onJewery")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);

		if(!info)
		{

		}
		else
		{
			BlackBoard::Get()->CurChangeEquipID = info->equip3;
			BlackBoard::Get()->CurChangeEquipType = 3;
			BlackBoard::Get()->CurBattlePosition = info->position;
			BlackBoard::Get()->jumpEquipPageName = "TeamPage";
			if(info->equip3 != 0)
			{
				BlackBoard::Get()->NeedChangeEquip = true;
				BlackBoard::Get()->NeedChooseEquip = false;
				BlackBoard::Get()->ShowEquip = info->equip3;

				MsgMainFramePushPage msg;
				msg.pageName = "EquipInfoPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(info->discipleId != 0)
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "EquipChangePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{

			}
		}
	}
}

void TeamPage::refreshPersonPage()
{
	unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
	const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);

	if(info->discipleId)
	{
		Disciple disciple(info->discipleId);

		CCLabelBMFont* blood=dynamic_cast<CCLabelBMFont* >(getVariable("mBlood"));
		if(blood)
		{
			blood->setString(StringConverter::toString(disciple.health()).c_str());
		}
		CCLabelBMFont* attack=dynamic_cast<CCLabelBMFont* >(getVariable("mAttack"));
		if(attack)
		{
			attack->setString(StringConverter::toString(disciple.attack()).c_str());
		}
		CCLabelBMFont* defense=dynamic_cast<CCLabelBMFont* >(getVariable("mDefense"));
		if(defense)
		{
			defense->setString(StringConverter::toString(disciple.defence()).c_str());
		}
		CCLabelBMFont* will=dynamic_cast<CCLabelBMFont* >(getVariable("mWill"));
		if(will)
		{
			will->setString(StringConverter::toString(disciple.rp()).c_str());
		}

		Skill skill1(disciple.skill1());
		CCSprite* skillPic1=dynamic_cast<CCSprite* >(getVariable("mSkillPic1"));
		if(!skill1.isHavItem())
		{
			char errMsg[256];
			sprintf(errMsg,"id %d: skillItem not exist!",disciple.skill1());
			MSG_BOX(errMsg);
			return;
		}
		if(!skill1.isHavInfo())
		{
			char errMsg[256];
			sprintf(errMsg,"id %d: skillInfo not exist!",disciple.skill1());
			MSG_BOX(errMsg);
			return;
		}
		if(skillPic1)
		{
			
			skillPic1->initWithFile(skill1.iconPic().c_str());
		}
		CCMenuItemImage* skillFrame1=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill1"));
		if(skillFrame1)
		{
			skillFrame1->setNormalImage(skill1.getFrameNormalSpirte("_Foot"));
			skillFrame1->setSelectedImage(skill1.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* skill1LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill1LV"));
		if(skill1LV)
		{
			skill1LV->setString(StringConverter::toString(skill1.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlnum1"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(skill1.getFrameColor());
		}
		CCLabelBMFont* skill1Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill1Name"));
		if(skill1Name)
		{
			skill1Name->setString(Language::Get()->getString(skill1.name()).c_str());
		}
	}
	else
	{
		CCSprite* skillPic1=dynamic_cast<CCSprite* >(getVariable("mSkillPic1"));
		if(skillPic1)
		{
			skillPic1->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Skill").c_str());
		}
		CCMenuItemImage* skillFrame1=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill1"));
		if(skillFrame1)
		{
			skillFrame1->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			skillFrame1->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* skill1LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill1LV"));
		if(skill1LV)
		{
			skill1LV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlnum1"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* skill1Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill1Name"));
		if(skill1Name)
		{
			skill1Name->setString(Language::Get()->getString("@Aoyi").c_str());	   
		}
	}

	if(info->skill1 != 0)
	{
		Skill skill2(info->skill1);
		CCSprite* skillPic2=dynamic_cast<CCSprite* >(getVariable("mSkillPic2"));
		if(skillPic2)
		{
			skillPic2->initWithFile(skill2.iconPic().c_str());
		}
		CCMenuItemImage* skillFrame2=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill2"));
		if(skillFrame2)
		{
			skillFrame2->setNormalImage(skill2.getFrameNormalSpirte("_Foot"));
			skillFrame2->setSelectedImage(skill2.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* skill2LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill2LV"));
		if(skill2LV)
		{
			skill2LV->setString(StringConverter::toString(skill2.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm2"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(skill2.getFrameColor());
		}
		CCLabelBMFont* skill2Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill2Name"));
		if(skill2Name)
		{
			skill2Name->setString(Language::Get()->getString(skill2.name()).c_str());
		}
	}
	else
	{
		CCSprite* skillPic2=dynamic_cast<CCSprite* >(getVariable("mSkillPic2"));
		if(skillPic2)
		{
			skillPic2->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Skill").c_str());
		}
		CCMenuItemImage* skillFrame2=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill2"));
		if(skillFrame2)
		{
			skillFrame2->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			skillFrame2->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* skill2LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill2LV"));
		if(skill2LV)
		{
			skill2LV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm2"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* skill2Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill2Name"));
		if(skill2Name)
		{
			skill2Name->setString(Language::Get()->getString("@Aoyi").c_str());	   
		}
	}

	if(info->skill2 != 0)
	{
		Skill skill3(info->skill2);
		CCSprite* skillPic3=dynamic_cast<CCSprite* >(getVariable("mSkillPic3"));
		if(skillPic3)
		{
			skillPic3->initWithFile(skill3.iconPic().c_str());
		}
		CCMenuItemImage* skillFrame3=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill3"));
		if(skillFrame3)
		{
			skillFrame3->setNormalImage(skill3.getFrameNormalSpirte("_Foot"));
			skillFrame3->setSelectedImage(skill3.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* skill3LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill3LV"));
		if(skill3LV)
		{
			skill3LV->setString(StringConverter::toString(skill3.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm3"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(skill3.getFrameColor());
		}
		CCLabelBMFont* skill3Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill3Name"));
		if(skill3Name)
		{
			skill3Name->setString(Language::Get()->getString(skill3.name()).c_str());
		}
	}
	else
	{
		CCSprite* skillPic3=dynamic_cast<CCSprite* >(getVariable("mSkillPic3"));
		if(skillPic3)
		{
			skillPic3->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Skill").c_str());
		}
		CCMenuItemImage* skillFrame3=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill3"));
		if(skillFrame3)
		{
			skillFrame3->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			skillFrame3->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* skill3LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill3LV"));
		if(skill3LV)
		{
			skill3LV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm3"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* skill3Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill3Name"));
		if(skill3Name)
		{
			skill3Name->setString(Language::Get()->getString("@Aoyi").c_str());	   
		}
	}

	if(info->equip1)
	{
		Equip equip1(info->equip1);
		CCSprite* weaponPic=dynamic_cast<CCSprite* >(getVariable("mWeaponPic"));
		if(weaponPic)
		{
			weaponPic->initWithFile(equip1.iconPic().c_str());
		}
		CCMenuItemImage* weaponFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mWeapon"));
		if(weaponFrame)
		{
			weaponFrame->setNormalImage(equip1.getFrameNormalSpirte("_Foot"));
			weaponFrame->setSelectedImage(equip1.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* weaponLV=dynamic_cast<CCLabelBMFont* >(getVariable("mWeaponLV"));
		if(weaponLV)
		{
			weaponLV->setString(StringConverter::toString(equip1.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm4"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(equip1.getFrameColor());
		}
		CCLabelBMFont* weaponName=dynamic_cast<CCLabelBMFont* >(getVariable("mWeaponName"));
		if(weaponName)
		{
			weaponName->setString(Language::Get()->getString(equip1.name()).c_str());
		}
	}
	else
	{
		CCSprite* weaponPic=dynamic_cast<CCSprite* >(getVariable("mWeaponPic"));
		if(weaponPic)
		{
			weaponPic->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Weapon").c_str());
		}
		CCMenuItemImage* weaponFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mWeapon"));
		if(weaponFrame)
		{
			weaponFrame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			weaponFrame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* weaponLV=dynamic_cast<CCLabelBMFont* >(getVariable("mWeaponLV"));
		if(weaponLV)
		{
			weaponLV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm4"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* weaponName=dynamic_cast<CCLabelBMFont* >(getVariable("mWeaponName"));
		if(weaponName)
		{
			weaponName->setString(Language::Get()->getString("@Weapon").c_str());
		}
	}

	if(info->equip2)
	{
		Equip equip2(info->equip2);
		CCSprite* armorPic=dynamic_cast<CCSprite* >(getVariable("mArmorPic"));
		if(armorPic)
		{
			armorPic->initWithFile(equip2.iconPic().c_str());
		}
		CCMenuItemImage* armorFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mArmor"));
		if(armorFrame)
		{
			armorFrame->setNormalImage(equip2.getFrameNormalSpirte("_Foot"));
			armorFrame->setSelectedImage(equip2.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* armorLV=dynamic_cast<CCLabelBMFont* >(getVariable("mArmorLV"));
		if(armorLV)
		{
			armorLV->setString(StringConverter::toString(equip2.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm5"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(equip2.getFrameColor());
		}
		CCLabelBMFont* armorName=dynamic_cast<CCLabelBMFont* >(getVariable("mArmorName"));
		if(armorName)
		{
			armorName->setString(Language::Get()->getString(equip2.name()).c_str());
		}
	}
	else
	{
		CCSprite* armorPic=dynamic_cast<CCSprite* >(getVariable("mArmorPic"));
		if(armorPic)
		{
			armorPic->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Armor").c_str());
		}
		CCMenuItemImage* armorFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mArmor"));
		if(armorFrame)
		{
			armorFrame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			armorFrame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* armorLV=dynamic_cast<CCLabelBMFont* >(getVariable("mArmorLV"));
		if(armorLV)
		{
			armorLV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm5"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* armorName=dynamic_cast<CCLabelBMFont* >(getVariable("mArmorName"));
		if(armorName)
		{
			armorName->setString(Language::Get()->getString("@Armor").c_str());
		}
	}

	if(info->equip3)
	{
		Equip equip3(info->equip3);
		CCSprite* jeweryPic=dynamic_cast<CCSprite* >(getVariable("mJeweryPic"));
		if(jeweryPic)
		{
			jeweryPic->initWithFile(equip3.iconPic().c_str());
		}
		CCMenuItemImage* jeweryFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mJewery"));
		if(jeweryFrame)
		{
			jeweryFrame->setNormalImage(equip3.getFrameNormalSpirte("_Foot"));
			jeweryFrame->setSelectedImage(equip3.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* jeweryLV=dynamic_cast<CCLabelBMFont* >(getVariable("mJeweryLV"));
		if(jeweryLV)
		{
			jeweryLV->setString(StringConverter::toString(equip3.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm6"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(equip3.getFrameColor());
		}
		CCLabelBMFont* jeweryName=dynamic_cast<CCLabelBMFont* >(getVariable("mJeweryName"));
		if(jeweryName)
		{
			jeweryName->setString(Language::Get()->getString(equip3.name()).c_str());
		}
	}
	else
	{
		CCSprite* jeweryPic=dynamic_cast<CCSprite* >(getVariable("mJeweryPic"));
		if(jeweryPic)
		{
			jeweryPic->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Jewery").c_str());
		}
		CCMenuItemImage* jeweryFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mJewery"));
		if(jeweryFrame)
		{
			jeweryFrame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			jeweryFrame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* jeweryLV=dynamic_cast<CCLabelBMFont* >(getVariable("mJeweryLV"));
		if(jeweryLV)
		{
			jeweryLV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm6"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* jeweryName=dynamic_cast<CCLabelBMFont* >(getVariable("mJeweryName"));
		if(jeweryName)
		{
			jeweryName->setString(Language::Get()->getString("@Jewelry").c_str());
		}
	}

	CCLabelBMFont* personValue=dynamic_cast<CCLabelBMFont* >(getVariable("mPersonValue"));
	if(personValue)
	{
		personValue->setString(StringConverter::toString(BattleArrayInfo::Get()->getBattleInfoValue(info)).c_str());
	}

	
	if(info->discipleId)
	{
		CCNode* mGongfang=dynamic_cast<CCNode* >(getVariable("mGongfang"));
		if(mGongfang)
		{
			mGongfang->setVisible(true);
		}
		CCNode* teamValueFrame=dynamic_cast<CCNode* >(getVariable("mTeamValueFrame"));
		if(teamValueFrame)
		{
			teamValueFrame->setVisible(true);
		}
		CCNode* memberViewFoot=dynamic_cast<CCNode* >(getVariable("mMemberViewFoot"));
		if(memberViewFoot)
		{
			memberViewFoot->setVisible(true);
		}
	}
	else
	{
		CCNode* mGongfang=dynamic_cast<CCNode* >(getVariable("mGongfang"));
		if(mGongfang)
		{
			mGongfang->setVisible(false);
		}
		CCNode* teamValueFrame=dynamic_cast<CCNode* >(getVariable("mTeamValueFrame"));
		if(teamValueFrame)
		{
			teamValueFrame->setVisible(false);
		}
		CCNode* memberViewFoot=dynamic_cast<CCNode* >(getVariable("mMemberViewFoot"));
		if(memberViewFoot)
		{
			memberViewFoot->setVisible(false);
		}

	}
	

	CCNode* allTitle=dynamic_cast<CCNode* >(getVariable("mAllTitle"));
	if(allTitle)
	{
		allTitle->setVisible(false);
	}

	

	Disciple disciple(info->discipleId);
	int yuanfenId = 0;
	int i = 0;
	std::map<unsigned int, unsigned int>::const_iterator itr = info->fateInfos.begin();
	CCLabelBMFont* yuanLable1=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanLable1"));
	if(yuanLable1)
	{
		yuanfenId = disciple.yuanfens(i);
		if(yuanfenId != -1 && yuanfenId !=0)
		{
			yuanLable1->setVisible(true);
			itr = info->fateInfos.find(yuanfenId);
			if(itr != info->fateInfos.end()){
				const YuanfenItem * yuanfenItem = YuanfenTableManager::Get()->getYuanfenItemByID(itr->first);
				if(_checkFates(yuanfenItem,itr->first))
				{
					yuanLable1->setString(yuanfenItem->name.c_str());
					if(itr->second == 0)
						yuanLable1->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated")));
					else
						yuanLable1->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Activate")));
				}
			}else{
				yuanLable1->setVisible(false);
			}
		}
		else
		{
			yuanLable1->setVisible(false);
		}
		i++;
		
	}
	CCLabelBMFont* yuanLable2=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanLable2"));
	if(yuanLable2)
	{
		yuanfenId = disciple.yuanfens(i);
		if(yuanfenId != -1 && yuanfenId !=0)
		{
			yuanLable2->setVisible(true);
			itr = info->fateInfos.find(yuanfenId);
			if(itr != info->fateInfos.end()){
				const YuanfenItem * yuanfenItem = YuanfenTableManager::Get()->getYuanfenItemByID(itr->first);
				if(_checkFates(yuanfenItem,itr->first))
				{
					yuanLable2->setString(yuanfenItem->name.c_str());
					if(itr->second == 0)
						yuanLable2->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated")));
					else
						yuanLable2->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Activate")));
				}
			}else{
				yuanLable2->setVisible(false);
			}
		}
		else
		{
			yuanLable2->setVisible(false);
		}
		i++;

	}
	CCLabelBMFont* yuanLable3=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanLable3"));
	if(yuanLable3)
	{
		yuanfenId = disciple.yuanfens(i);
		if(yuanfenId != -1 && yuanfenId !=0)
		{
			yuanLable3->setVisible(true);
			itr = info->fateInfos.find(yuanfenId);
			if(itr != info->fateInfos.end()){
				const YuanfenItem * yuanfenItem = YuanfenTableManager::Get()->getYuanfenItemByID(itr->first);
				if(_checkFates(yuanfenItem,itr->first))
				{
					yuanLable3->setString(yuanfenItem->name.c_str());
					if(itr->second == 0)
						yuanLable3->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated")));
					else
						yuanLable3->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Activate")));
				}
			}else{
				yuanLable3->setVisible(false);
			}
		}
		else
		{
			yuanLable3->setVisible(false);
		}
		i++;

	}
	CCLabelBMFont* yuanLable4=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanLable4"));
	if(yuanLable4)
	{
		yuanfenId = disciple.yuanfens(i);
		if(yuanfenId != -1 && yuanfenId !=0)
		{
			yuanLable4->setVisible(true);
			itr = info->fateInfos.find(yuanfenId);
			if(itr != info->fateInfos.end()){
				const YuanfenItem * yuanfenItem = YuanfenTableManager::Get()->getYuanfenItemByID(itr->first);
				if(_checkFates(yuanfenItem,itr->first))
				{
					yuanLable4->setString(yuanfenItem->name.c_str());
					if(itr->second == 0)
						yuanLable4->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated")));
					else
						yuanLable4->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Activate")));
				}
			}else{
				yuanLable4->setVisible(false);
			}
		}
		else
		{
			yuanLable4->setVisible(false);
		}

		i++;
	}
	CCLabelBMFont* yuanLable5=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanLable5"));
	if(yuanLable5)
	{
		yuanfenId  = disciple.yuanfens(i);
		if(yuanfenId != -1 && yuanfenId !=0)
		{
			yuanLable5->setVisible(true);
			itr = info->fateInfos.find(yuanfenId);
			if(itr != info->fateInfos.end()){
				const YuanfenItem * yuanfenItem = YuanfenTableManager::Get()->getYuanfenItemByID(itr->first);
				if(_checkFates(yuanfenItem,itr->first))
				{
					yuanLable5->setString(yuanfenItem->name.c_str());
					if(itr->second == 0)
						yuanLable5->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated")));
					else
						yuanLable5->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Activate")));
				}
			}else{
				yuanLable5->setVisible(false);
			}
		}
		else
		{
			yuanLable5->setVisible(false);
		}
		i++;

	}
	CCLabelBMFont* yuanLable6=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanLable6"));
	if(yuanLable6)
	{
		yuanfenId = disciple.yuanfens(i);
		if(yuanfenId != -1 && yuanfenId !=0)
		{
			yuanLable6->setVisible(true);
			itr = info->fateInfos.find(yuanfenId);
			if(itr != info->fateInfos.end()){
				const YuanfenItem * yuanfenItem = YuanfenTableManager::Get()->getYuanfenItemByID(itr->first);
				if(_checkFates(yuanfenItem,itr->first))
				{
					yuanLable6->setString(yuanfenItem->name.c_str());
					if(itr->second == 0)
						yuanLable6->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated")));
					else
						yuanLable6->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Activate")));
				}
			}else{
				yuanLable6->setVisible(false);
			}
		}
		else
		{
			yuanLable6->setVisible(false);
		}
		i++;

	}

	CCNode* overViewFoot=dynamic_cast<CCNode* >(getVariable("mOverViewFoot"));
	if(overViewFoot)
	{
		overViewFoot->setVisible(false);
	}
}

void TeamPage::refreshOverviewPage( void )
{
	CCNode* mGongfang=dynamic_cast<CCNode* >(getVariable("mGongfang"));
	if(mGongfang)
	{
		mGongfang->setVisible(false);
	}
	CCNode* teamValueFrame=dynamic_cast<CCNode* >(getVariable("mTeamValueFrame"));
	if(teamValueFrame)
	{
		teamValueFrame->setVisible(false);
	}

	CCNode* allTitle=dynamic_cast<CCNode* >(getVariable("mAllTitle"));
	if(allTitle)
	{
		allTitle->setVisible(false);
	}

	CCNode* memberViewFoot=dynamic_cast<CCNode* >(getVariable("mMemberViewFoot"));
	if(memberViewFoot)
	{
		memberViewFoot->setVisible(false);
	}

	CCNode* overViewFoot=dynamic_cast<CCNode* >(getVariable("mOverViewFoot"));
	if(overViewFoot)
	{
		overViewFoot->setVisible(true);
	}

	CCLabelBMFont* teamTotalValue=dynamic_cast<CCLabelBMFont* >(getVariable("mTeamTotalValue"));
	if(teamTotalValue)
	{
		teamTotalValue->setString(StringConverter::toString(BattleArrayInfo::Get()->getBattleInfoTotalValue()).c_str());
	}

	CCLabelBMFont* qishi=dynamic_cast<CCLabelBMFont* >(getVariable("mQishi"));
	if(qishi)
	{
		qishi->setString(StringConverter::toString(ServerDateManager::Get()->getUserBasicInfo().titleTotalNum).c_str());
	}

	{
		Title title(1);

		CCSprite* skillPic1=dynamic_cast<CCSprite* >(getVariable("mSkillPic1"));
		if(skillPic1)
		{
			skillPic1->initWithFile(title.iconPic().c_str());
		}
		CCMenuItemImage* skillFrame1=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill1"));
		if(skillFrame1)
		{
			skillFrame1->setNormalImage(title.getFrameNormalSpirte("_Foot"));
			skillFrame1->setSelectedImage(title.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* skill1LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill1LV"));
		if(skill1LV)
		{
			skill1LV->setString(StringConverter::toString(title.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlnum1"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(title.getFrameColor());
		}
		CCLabelBMFont* skill1Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill1Name"));
		if(skill1Name)
		{
			skill1Name->setString(Language::Get()->getString(title.name()).c_str());
		}
	}

	{
		Title title(2);

		CCSprite* skillPic2=dynamic_cast<CCSprite* >(getVariable("mSkillPic2"));
		if(skillPic2)
		{
			skillPic2->initWithFile(title.iconPic().c_str());
		}
		CCMenuItemImage* skillFrame2=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill2"));
		if(skillFrame2)
		{
			skillFrame2->setNormalImage(title.getFrameNormalSpirte("_Foot"));
			skillFrame2->setSelectedImage(title.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* skill2LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill2LV"));
		if(skill2LV)
		{
			skill2LV->setString(StringConverter::toString(title.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm2"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(title.getFrameColor());
		}
		CCLabelBMFont* skill2Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill2Name"));
		if(skill2Name)
		{
			skill2Name->setString(Language::Get()->getString(title.name()).c_str());
		}
	}

	{
		Title title(4);

		CCSprite* skillPic3=dynamic_cast<CCSprite* >(getVariable("mSkillPic3"));
		if(skillPic3)
		{
			skillPic3->initWithFile(title.iconPic().c_str());
		}
		CCMenuItemImage* skillFrame3=dynamic_cast<CCMenuItemImage* >(getVariable("mSkill3"));
		if(skillFrame3)
		{
			skillFrame3->setNormalImage(title.getFrameNormalSpirte("_Foot"));
			skillFrame3->setSelectedImage(title.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* skill3LV=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill3LV"));
		if(skill3LV)
		{
			skill3LV->setString(StringConverter::toString(title.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm3"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(title.getFrameColor());
		}
		CCLabelBMFont* skill3Name=dynamic_cast<CCLabelBMFont* >(getVariable("mSkill3Name"));
		if(skill3Name)
		{
			skill3Name->setString(Language::Get()->getString(title.name()).c_str());
		}
	}

	{
		Title title(3);

		CCSprite* weaponPic=dynamic_cast<CCSprite* >(getVariable("mWeaponPic"));
		if(weaponPic)
		{
			weaponPic->initWithFile(title.iconPic().c_str());
		}
		CCMenuItemImage* weaponFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mWeapon"));
		if(weaponFrame)
		{
			weaponFrame->setNormalImage(title.getFrameNormalSpirte("_Foot"));
			weaponFrame->setSelectedImage(title.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* weaponLV=dynamic_cast<CCLabelBMFont* >(getVariable("mWeaponLV"));
		if(weaponLV)
		{
			weaponLV->setString(StringConverter::toString(title.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm4"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(title.getFrameColor());
		}
		CCLabelBMFont* weaponName=dynamic_cast<CCLabelBMFont* >(getVariable("mWeaponName"));
		if(weaponName)
		{
			weaponName->setString(Language::Get()->getString(title.name()).c_str());
		}
	}

	{
		Title title(5);

		CCSprite* armorPic=dynamic_cast<CCSprite* >(getVariable("mArmorPic"));
		if(armorPic)
		{
			armorPic->initWithFile(title.iconPic().c_str());
		}
		CCMenuItemImage* armorFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mArmor"));
		if(armorFrame)
		{
			armorFrame->setNormalImage(title.getFrameNormalSpirte("_Foot"));
			armorFrame->setSelectedImage(title.getFrameSelectedSpirte("_Foot"));
		}
		CCLabelBMFont* armorLV=dynamic_cast<CCLabelBMFont* >(getVariable("mArmorLV"));
		if(armorLV)
		{
			armorLV->setString(StringConverter::toString(title.level()).c_str());
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm5"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(true);
			lvlnumbg1->setColor(title.getFrameColor());
		}
		CCLabelBMFont* armorName=dynamic_cast<CCLabelBMFont* >(getVariable("mArmorName"));
		if(armorName)
		{
			armorName->setString(Language::Get()->getString(title.name()).c_str());
		}
	}

	{
		CCSprite* jeweryPic=dynamic_cast<CCSprite* >(getVariable("mJeweryPic"));
		if(jeweryPic)
		{
			jeweryPic->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_Jewery").c_str());
		}
		CCMenuItemImage* jeweryFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mJewery"));
		if(jeweryFrame)
		{
			jeweryFrame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
			jeweryFrame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
		}
		CCLabelBMFont* jeweryLV=dynamic_cast<CCLabelBMFont* >(getVariable("mJeweryLV"));
		if(jeweryLV)
		{
			jeweryLV->setString("");
		}
		CCSprite* lvlnumbg1 = dynamic_cast<CCSprite*>(getVariable("lvlunm6"));
		if (lvlnumbg1)
		{
			lvlnumbg1->setVisible(false);
		}
		CCLabelBMFont* jeweryName=dynamic_cast<CCLabelBMFont* >(getVariable("mJeweryName"));
		if(jeweryName)
		{
			jeweryName->setString("");
		}
	}
}

void TeamPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "TeamPage")
		{
			ContentList::iterator itr = mPersonCardContentList.begin();
			while (itr!=mPersonCardContentList.end())
			{
				itr->second->refreshContent();
				++itr;
			}

			itr = mPersonIconContentList.begin();
			while (itr!=mPersonIconContentList.end())
			{
				itr->second->refreshContent();
				++itr;
			}
			refreshPage();
		}
	}
}

bool TeamPage::_checkFates(const YuanfenItem* item,unsigned int id)
{
	if(item)
	{
		return true;
	}
	else
	{
		char errMsg[256];
		sprintf(errMsg,"id %d: fates not exist!",id);
		MSG_BOX(errMsg);
		return false;
	}
}
