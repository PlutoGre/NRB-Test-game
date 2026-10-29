
#include "stdafx.h"

#include "EquipCardPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"

USING_NS_CC;

void EquipCardPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	
}

void EquipCardPage::refreshContent( void )
{
	Equip equip(mID,mIsItem,mIsViewTeam);
	CCSprite* equipPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipPic"));
	if(equipPic)
	{
		equipPic->setTexture(equip.bodyPic().c_str());
	}
	CCSprite* buffPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mBuffPic"));
	if(buffPic)
	{
		buffPic->setTexture(equip.getEquipAtkTypePic().c_str());
	}
	CCSprite* namePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipName"));
	if(namePic)
	{
		namePic->setVisible(false);
		//namePic->setTexture(equip.namePic().c_str());
	}
	CCB_FUNC(mContentContainer,"mEquipNameTex",CCLabelBMFont,setString(equip.name().c_str()));
	CCSprite* qualityPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mQualityPic"));
	if(qualityPic)
	{
		if(namePic)
		{
			qualityPic->setPositionY(namePic->getPositionY() - namePic->getAnchorPoint().y * namePic->getTexture()->getContentSize().height - 5);
		}
		qualityPic->setTexture(equip.getQualityImageFile().c_str());
	}
	CCLabelBMFont* buffValue=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBuffValue"));
	if(buffValue)
	{
		buffValue->setString(StringConverter::toString(equip.quantity()).c_str());
	}
	CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipLV"));
	if(levelName)
	{
		levelName->setString(StringConverter::toString(equip.level()).c_str());
	}
}
