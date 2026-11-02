
#include "stdafx.h"

#include "CareerStoryPage.h"
#include "CareerConfigManager.h"
#include "GameMessages.h"
#include "StringConverter.h"
#include "GameMaths.h"
#include "BlackBoard.h"
REGISTER_PAGE("CareerStoryPage",CareerStoryPage);
USING_NS_CC;
USING_NS_CC_EXT;
CareerStoryPage::CareerStoryPage(void)
{
}


CareerStoryPage::~CareerStoryPage(void)
{
}

void CareerStoryPage::Enter( MainFrame* )
{
	mCurrentStep = CareerConfigManager::Get()->getChaptersDataByChaptersId(mChapterID)->dialogs.begin();
	if(mCurrentStep != CareerConfigManager::Get()->getChaptersDataByChaptersId(mChapterID)->dialogs.end())
	{
		updateContent();
		mCurrentStep++;
	}
	else
	{
		MsgMainFrameCoverHide msg;
		msg.pageName = "CareerStoryPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void CareerStoryPage::Execute( MainFrame* )
{

}

void CareerStoryPage::Exit( MainFrame* )
{
	if(BlackBoard::Get()->UpdateTitleType>0)
	{
		if(BlackBoard::Get()->PreTitleId==0)
		{
			MsgMainFramePushPage msg;
			msg.pageName = "TitleGetNewPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			MsgMainFramePushPage msg;
			msg.pageName = "TitleLVUpPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
}

void CareerStoryPage::load( void )
{
	loadCcbiFile("CareerConversation.ccbi");
}

void CareerStoryPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onTouch")
	{
		if(mCurrentStep != CareerConfigManager::Get()->getChaptersDataByChaptersId(mChapterID)->dialogs.end())
		{
			updateContent();
			mCurrentStep++;
		}
		else
		{
			MsgMainFrameCoverHide msg;
			msg.pageName = "CareerStoryPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
}

void CareerStoryPage::updateContent()
{
	if(mCurrentStep != CareerConfigManager::Get()->getChaptersDataByChaptersId(mChapterID)->dialogs.end())
	{
		CCB_FUNC(this,"mPerson1",CCSprite,setVisible(false));
		CCB_FUNC(this,"mPerson2",CCSprite,setVisible(false));

		if(mCurrentStep->icon1!="")
		{
			CCB_FUNC(this,"mPerson1",CCSprite,setVisible(true));
			CCB_FUNC(this,"mPerson1",CCSprite,setTexture(mCurrentStep->icon1.c_str()));
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//CCB_FUNC(this,"mPerson1",CCSprite,setTextureRect(rect));
		}
		if(mCurrentStep->icon2!="")
		{
			CCB_FUNC(this,"mPerson2",CCSprite,setVisible(true));
			CCB_FUNC(this,"mPerson2",CCSprite,setTexture(mCurrentStep->icon2.c_str()));
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//CCB_FUNC(this,"mPerson2",CCSprite,setTextureRect(rect));
		}
		std::string dialogs;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("CareerStoryDialogs"));
		GameMaths::stringAutoReturn(mCurrentStep->dialogs,dialogs,width,lines);
		CCB_FUNC(this,"mMsg",CCLabelBMFont,setString(dialogs.c_str()));
	}
}
