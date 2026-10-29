
#include "stdafx.h"

#include "TitleCardPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Title.h"

USING_NS_CC;

void TitleCardPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	
}

void TitleCardPage::refreshContent( void )
{
	Title title(mID,mIsViewTeam);
	CCSprite* titlePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mTitlePic"));
	if(titlePic)
	{
		titlePic->initWithFile(title.iconPic().c_str());
	}
	CCSprite* buffPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mBuffPic"));
	if(buffPic)
	{
		buffPic->initWithFile(title.getTitleBuffPic().c_str());
	}
	CCSprite* titleName=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mTitleName"));
	if(titleName)
	{
		titleName->setVisible(false);
		//titleName->initWithFile(title.namePic().c_str());
	}
	CCB_FUNC(mContentContainer,"mTitleNameTex",CCLabelBMFont,setString(title.name().c_str()));
	CCLabelBMFont* buffValue=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBuffValue"));
	if(buffValue)
	{
		std::string buffQuantity = title.buffType()==1?StringConverter::toString(title.quantity()):StringConverter::toString(title.quantity())+"%";
		buffValue->setString(buffQuantity.c_str());
	}
	CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mTitleLV"));
	if(levelName)
	{
		levelName->setString(StringConverter::toString(title.level()).c_str());
	}
	CCLabelBMFont* status=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mStatus"));
	if(status)
	{
		if(title.hasGet())
			status->setString(Language::Get()->getString("@AlreadyGet").c_str());
		else
			status->setString(Language::Get()->getString("@NoGet").c_str());
	}

}
