
#include "stdafx.h"

#include "ViewTeamPage.h"
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
#include "TitleInfoPage.h"
#include "ViewTeamEquipInfoPage.h"
REGISTER_PAGE("ViewTeamPage",ViewTeamPage);
USING_NS_CC;

void ViewPersonCardContent::refreshContent( void )
{
	Disciple disciple(ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(mID)->discipleId,false,true);
	CCSprite* memPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMemPic"));
	if(memPic)
	{
		memPic->setTexture(disciple.bodyPic().c_str());
		mStrTex1 = disciple.bodyPic();
	}
	CCSprite* memName=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMemName"));
	if(memName)
	{
		memName->setVisible(false);
	}
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setColor(ccBLACK));
	std::list<std::string> _list;
	_list.push_back(disciple.name());
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@DiscipleNameTitle"),&_list);
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(mContentContainer,"mQualitybg",CCSprite,setTexture(disciple.getQualityBgImageFile().c_str()));
	mStrTex2 = disciple.getQualityBgImageFile();
	CCSprite* qualityPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mQualityPic"));
	if(qualityPic)
	{
		mStrTex3 = disciple.getQualityImageFile(true);
		qualityPic->setTexture(/*disciple.getQualityImageFile(true)*/mStrTex3.c_str());
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

void ViewPersonCardContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
	{
		mOwnner->setTarget(mID + 1);
	}
	else if(itemName=="onShowMemInfo")
	{
		ShowDiscipleViewInfoPopup(ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(mID)->discipleId);
	}
}

ViewPersonCardContent::~ViewPersonCardContent()
{
	if (!mStrTex1.empty())
	{
		CCTextureCache::sharedTextureCache()->removeTextureForKey(mStrTex1.c_str());
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(mStrTex1.c_str());
	}
	//
	if (!mStrTex2.empty())
	{
		CCTextureCache::sharedTextureCache()->removeTextureForKey(mStrTex2.c_str());
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(mStrTex2.c_str());
	}
	//
	if (!mStrTex3.empty())
	{
		CCTextureCache::sharedTextureCache()->removeTextureForKey(mStrTex3.c_str());
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(mStrTex3.c_str());
	}
}

ViewPersonCardContent::ViewPersonCardContent( unsigned int id, ViewTeamPage* ownner ) :ContentBase(id)
	,mOwnner(ownner)
{

}

void ViewTeamOverviewCardContent::refreshContent( void )
{
	for(int i = 0; i<MaxBattleDiscipleNum; ++i)
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(i);
		if(info && info->discipleId != 0)
		{
			Disciple disciple(info->discipleId,false,true);

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

void ViewIconContent::setSellect( bool sellect )
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

void ViewPersonIconContent::refreshContent( void )
{
	const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(mID);
	if(info && info->discipleId != 0)
	{
		Disciple disciple(info->discipleId,false,true);
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
			mFrameBack->setSelectedImage(disciple.getFrameSelectedSpirte());
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
			mFrameBack->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
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

void ViewPersonIconContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
	{
		mOwnner->setTarget(mID + 1);
	}
}

void ViewTeamOverviewIconContent::refreshContent( void )
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

void ViewTeamOverviewIconContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFrameButton")
	{
		mOwnner->setTarget(mID + 1);
	}
}

ViewTeamPage::ViewTeamPage(void)
{
	mCurrIndex = 0;
}

ViewTeamPage::~ViewTeamPage(void)
{
}

void ViewTeamPage::Enter( MainFrame* frame)
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

void ViewTeamPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void ViewTeamPage::Execute( MainFrame* )
{

}

void ViewTeamPage::load( void )
{
	loadCcbiFile("teampage_b.ccbi");
	mPersonCards =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mPerson"));
	mPersonCardRootNode = mPersonCards->getContainer();
	mPersonCards->setDelegate(this);
	mPersonCards->setScrollDeaccelRate(0);
	mPersonCards->setBounceable(false);

	mPersonIcons =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mMembers"));
	mPersonIconRootNode = mPersonIcons->getContainer();
}

void ViewTeamPage::scrollViewDidDeaccelerateStop( cocos2d::extension::CCScrollView* view ,cocos2d::CCPoint initSpeed)
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

void ViewTeamPage::buildItem( void )
{
	int i = 0;
	unsigned int battleEnableSize = ServerDateManager::Get()->getViewTeamBattleInfoItem().getBattleArraySize();
	for(; i<battleEnableSize; ++i)
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(i);
		if(info&&(info->discipleId))
		{
			ViewPersonCardContent * itemCard = new ViewPersonCardContent(i, this);
			itemCard->init();
			addCardItem(itemCard);
		}
		ViewPersonIconContent * itemIcon = new ViewPersonIconContent(i, this);
		itemIcon->init();
		addIconItem(itemIcon);
	}

	ViewTeamOverviewCardContent * teamOverviewCard = new ViewTeamOverviewCardContent(i);
	teamOverviewCard->init();
	addCardItem(teamOverviewCard);

	ViewTeamOverviewIconContent * teamOverviewIcon = new ViewTeamOverviewIconContent(i, this);
	teamOverviewIcon->init();
	addIconItem(teamOverviewIcon);

	mPersonCards->forceRecaculateChildren();
	mPersonIcons->forceRecaculateChildren();
}

void ViewTeamPage::addCardItem( ContentBase* item )
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

void ViewTeamPage::addIconItem( ContentBase* item )
{
	CCBContainer* container = item->getContainerNode();
	if(container)
	{
		mPersonIconContentList.insert(std::make_pair<unsigned int, ContentBase *>(mPersonIconContentList.size() + 1, item));
		mPersonIconRootNode->addChild(container);

		CCPoint pos = ccp(container->getContentSize().width * (mPersonIconContentList.size()-1), 5.0f);
		container->setPosition(pos);
		CCSize size = CCSize(container->getContentSize().width * mPersonIconContentList.size(), container->getContentSize().height);
		mPersonIcons->setContentSize(size);
	}
}

void ViewTeamPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void ViewTeamPage::clearAllItem( void )
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

void ViewTeamPage::refreshPage( void )
{
	ContentList::iterator it = mPersonCardContentList.find(mCurrIndex);
	if(it!=mPersonCardContentList.end() && it->second)
	{
		unsigned int id = it->second->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);
		if(info)
		{
			refreshPersonPage();
		}
		else
		{
			refreshOverviewPage();
		}
	}
}

void ViewTeamPage::setTarget( unsigned int index, bool isDuration/*=false*/)
{
	ContentList::iterator itr = mPersonIconContentList.find(index);
	if(mCurrIndex != index)
	{
		itr = mPersonIconContentList.find(mCurrIndex);
		if(itr!= mPersonIconContentList.end())
		{
			dynamic_cast<ViewIconContent*>(itr->second)->setSellect(false);
		}

		mCurrIndex = index;

		itr = mPersonIconContentList.find(mCurrIndex);
		if(itr!= mPersonIconContentList.end())
		{
			dynamic_cast<ViewIconContent*>(itr->second)->setSellect(true);
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

void ViewTeamPage::resetNavOffset(void)
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

void ViewTeamPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onSkill1")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);

		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 1;
			BasePage* bp  = CCBManager::Get()->getPage("TitleInfoPage");
			TitleInfoPage* page = bp?dynamic_cast<TitleInfoPage*>(bp):0;
			if(page)
				page->setIsTeamView(true);
			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			if(info->discipleId != 0)
			{
				Disciple disciple(info->discipleId,false,true);
				ShowSkillViewInfoPopup(disciple.skill1());
			}
			else
			{

			}
			
		}
	}
	else if(itemName == "onSkill2")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);

		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 2;
			BasePage* bp  = CCBManager::Get()->getPage("TitleInfoPage");
			TitleInfoPage* page = bp?dynamic_cast<TitleInfoPage*>(bp):0;
			if(page)
				page->setIsTeamView(true);
			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}	
		else
		{
			if(info->skill1 != 0)
			{
				ShowSkillViewInfoPopup(info->skill1);
			}
		}
	}
	else if(itemName == "onSkill3")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);
		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 4;

			BasePage* bp  = CCBManager::Get()->getPage("TitleInfoPage");
			TitleInfoPage* page = bp?dynamic_cast<TitleInfoPage*>(bp):0;
			if(page)
				page->setIsTeamView(true);
			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			if(info->skill2 != 0)
			{
				ShowSkillViewInfoPopup(info->skill2);
			}
		}
	}
	else if(itemName == "onWeapon")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);
		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 3;
			BasePage* bp  = CCBManager::Get()->getPage("TitleInfoPage");
			TitleInfoPage* page = bp?dynamic_cast<TitleInfoPage*>(bp):0;
			if(page)
				page->setIsTeamView(true);
			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}else{
			if(info->equip1 != 0)
			{
				ShowEquipViewInfoPopup(info->equip1);
			}
			else
			{

			}
		}
	}
	else if(itemName == "onArmor")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);

		if(!info)
		{
			BlackBoard::Get()->ShowTitle = 5;
			BasePage* bp  = CCBManager::Get()->getPage("TitleInfoPage");
			TitleInfoPage* page = bp?dynamic_cast<TitleInfoPage*>(bp):0;
			if(page)
				page->setIsTeamView(true);
			MsgMainFramePushPage msg;
			msg.pageName = "TitleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}else{
			if(info->equip2 != 0)
			{
				ShowEquipViewInfoPopup(info->equip2);
			}
			else
			{

			}
		}
	}
	else if(itemName == "onJewery")
	{
		unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);

		if(!info)
		{

		}else{
			if(info->equip3 != 0)
			{
				ShowEquipViewInfoPopup(info->equip3);
			}
			else
			{

			}
		}
	}
	else if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ViewTeamPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void ViewTeamPage::refreshPersonPage()
{
	unsigned int id = mPersonCardContentList[mCurrIndex]->getContentID();
	const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);

	if(info->discipleId)
	{
		Disciple disciple(info->discipleId,false,true);

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
		if(disciple.skill1()!=0)
		{
			Skill skill1(disciple.skill1(),false,true);
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
				skill1Name->setString("");
			}
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
			skill1Name->setString("");
		}
	}

	if(info->skill1 != 0)
	{
		Skill skill2(info->skill1,false,true);
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
			skill2Name->setString("");
		}
	}

	if(info->skill2 != 0)
	{
		Skill skill3(info->skill2,false,true);
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
			skill3Name->setString("");
		}
	}

	if(info->equip1!=0)
	{
		Equip equip1(info->equip1,false,true);
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
			weaponName->setString("");
		}
	}

	if(info->equip2!=0)
	{
		Equip equip2(info->equip2,false,true);
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
			armorName->setString("");
		}
	}

	if(info->equip3!=0)
	{
		Equip equip3(info->equip3,false,true);
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
			jeweryName->setString("");
		}
	}

	CCLabelBMFont* personValue=dynamic_cast<CCLabelBMFont* >(getVariable("mPersonValue"));
	if(personValue)
	{
		personValue->setString(StringConverter::toString(BattleArrayInfo::Get()->getBattleInfoValue(info,true)).c_str());
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

	

	Disciple disciple(info->discipleId,false,true);
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

void ViewTeamPage::refreshOverviewPage( void )
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
		teamTotalValue->setString(StringConverter::toString(BattleArrayInfo::Get()->getBattleInfoTotalValue(true)).c_str());
	}

	CCLabelBMFont* qishi=dynamic_cast<CCLabelBMFont* >(getVariable("mQishi"));
	if(qishi)
	{
		qishi->setString(StringConverter::toString(ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerTitleNum).c_str());
	}

	{
		Title title(1,true);

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
		Title title(2,true);

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
		Title title(4,true);
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
		Title title(3,true);

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
		Title title(5,true);

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

void ViewTeamPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "ViewTeamPage")
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

bool ViewTeamPage::_checkFates(const YuanfenItem* item,unsigned int id)
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
