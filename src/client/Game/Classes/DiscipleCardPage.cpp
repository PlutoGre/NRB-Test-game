
#include "stdafx.h"

#include "DiscipleCardPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "Skill.h"
#include "GameMaths.h"
USING_NS_CC;

void DiscipleCardPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	
}

void DiscipleCardPage::refreshContent( void )
{
	Disciple disciple(mID, isItemID,mIsViewTeam);
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
	std::list<std::string> _list;
	_list.push_back(disciple.name());
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@DiscipleNameTitle"),&_list);
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setColor(ccBLACK));
	CCB_FUNC(mContentContainer,"mMemNameTex",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(mContentContainer,"mQualitybg",CCSprite,setTexture(disciple.getQualityBgImageFile().c_str()));
	CCSprite* qualityPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mQualityPic"));
	if(qualityPic)
	{
		if(memName)
		{
//			qualityPic->setPositionY(memName->getPositionY() - memName->getAnchorPoint().y * memName->getContentSize().height - 5);
		}
		qualityPic->setTexture(disciple.getQualityImageFile().c_str());
	}
	CCSprite* shadowPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mShadowPic"));
	if(shadowPic)
	{
		if(isSoul)
		{
			if(qualityPic)
			{
//				shadowPic->setPositionY(qualityPic->getPositionY() - qualityPic->getAnchorPoint().y * qualityPic->getContentSize().height - 5);
			}
			shadowPic->setVisible(true);
		}
		else
		{
			shadowPic->setVisible(false);
		}
	}
	
	CCLabelBMFont* blood=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBlood"));
	if(blood)
	{
		blood->setString(StringConverter::toString(disciple.health()).c_str());
	}
	CCLabelBMFont* attack=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mAttack"));
	if(attack)
	{
		attack->setString(StringConverter::toString(disciple.attack()).c_str());
	}
	CCLabelBMFont* defense=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mDefense"));
	if(defense)
	{
		defense->setString(StringConverter::toString(disciple.defence()).c_str());
	}
	CCLabelBMFont* will=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mWill"));
	if(will)
	{
		will->setString(StringConverter::toString(disciple.rp()).c_str());
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

void DiscipleCardPage::setContentIDWithRemoveTextureCache( unsigned int id )
{
	if (mID == id)
		return;
	//
	if (mID > 0)
	{
		Disciple disciple(mID, isItemID,mIsViewTeam);
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
}

void DiscipleCardPage::setContentID( unsigned int id )
{
	mID = id;
}

DiscipleCardPage::~DiscipleCardPage()
{
	setContentIDWithRemoveTextureCache(-1);//把图片资源release
}
