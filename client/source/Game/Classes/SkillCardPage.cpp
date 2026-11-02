
#include "stdafx.h"

#include "SkillCardPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Skill.h"

USING_NS_CC;

void SkillCardPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	
}

void SkillCardPage::refreshContent( void )
{
	Skill skill(mID,mIsItemID,mViewTeam);
	CCSprite* skillPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSkillPic"));
	if(skillPic)
	{
		skillPic->setTexture(skill.bodyPic().c_str());
	}
	CCSprite* buffPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mBuffPic"));
	if(buffPic)
	{
		buffPic->setTexture(skill.getSkillAtkTypePic().c_str());
	}
	CCSprite* skillName=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mSkillName"));
	if(skillName)
	{
		skillName->setVisible(false);
		//skillName->setTexture(skill.namePic().c_str());
	}
	CCB_FUNC(mContentContainer,"mSkillNameTex",CCLabelBMFont,setString(skill.name().c_str()));
	CCSprite* qualityPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mQualityPic"));
	if(qualityPic)
	{
		if(skillName)
		{
			qualityPic->setPositionY(skillName->getPositionY() - skillName->getAnchorPoint().y * skillName->getTexture()->getContentSize().height - 5);
		}
		qualityPic->setTexture(skill.getQualityImageFile().c_str());
	}
	CCLabelBMFont* buffValue=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBuffValue"));
	if(buffValue)
	{
		buffValue->setString(skill.quantityStr().c_str());
	}
	CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mSkillLV"));
	if(levelName)
	{
		levelName->setString(StringConverter::toString(skill.level()).c_str());
	}
	CCLabelBMFont* sttackType=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mAttackType"));
	if(sttackType)
	{
		sttackType->setString(skill.getSkillAtkType().c_str());
	}
}
