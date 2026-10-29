
#include "stdafx.h"

#include "TitleGetNewPage.h"
#include "BlackBoard.h"
#include "MessageManager.h"
#include "Title.h"
#include "GameMessages.h"
#include "StringConverter.h"
REGISTER_PAGE("TitleGetNewPage",TitleGetNewPage);
USING_NS_CC;

TitleGetNewPage::TitleGetNewPage(void)
{
}


TitleGetNewPage::~TitleGetNewPage(void)
{
}

void TitleGetNewPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void TitleGetNewPage::Exit( MainFrame* frame)
{
	BlackBoard::Get()->UpdateTitleType = 0;
}

void TitleGetNewPage::Execute( MainFrame* )
{
}

void TitleGetNewPage::load( void )
{
	loadCcbiFile("TitleGetNew.ccbi");
}

void TitleGetNewPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage mPopPage;
		mPopPage.pageName = "TitleGetNewPage";
		MessageManager::Get()->sendMessage(&mPopPage);
	}
	else if(itemName=="onShowTitleInfo")
	{
		MsgMainFramePopPage mPopPage;
		mPopPage.pageName = "TitleGetNewPage";
		MessageManager::Get()->sendMessage(&mPopPage);

		BlackBoard::Get()->ShowTitle = BlackBoard::Get()->UpdateTitleType;

		MsgMainFramePushPage msg;
		msg.pageName = "TitleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
void TitleGetNewPage::refreshPage( void )
{
	Title title(BlackBoard::Get()->UpdateTitleType);
	CCB_FUNC(this,"mTitleInfo",CCLabelBMFont,setString(title.name().c_str()));
	CCB_FUNC(this,"mTitleBuff",CCLabelBMFont,setString((title.getQuantityString()).c_str()));
	std::string descStr =  title.describe();
	if(descStr=="none")
	{
		CCB_FUNC(this,"mNote1",CCLabelBMFont,setVisible(false));
		CCB_FUNC(this,"mNote2",CCLabelBMFont,setVisible(false));
	}
	else
	{
		CCB_FUNC(this,"mNote1",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mNote1",CCLabelBMFont,setString(descStr.c_str()));
		CCB_FUNC(this,"mNote2",CCLabelBMFont,setVisible(true));
	}
	
	CCB_FUNC(this,"mTitleLV",CCLabelBMFont,setString(StringConverter::toString(title.level()).c_str()));
	CCB_FUNC(this,"mTitlePic",CCSprite,initWithFile(title.iconPic().c_str()));
}