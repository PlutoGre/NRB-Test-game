
#include "stdafx.h"

#include "TitleLVUpPage.h"
#include "BlackBoard.h"
#include "MessageManager.h"
#include "Title.h"
#include "GameMessages.h"
#include "StringConverter.h"
REGISTER_PAGE("TitleLVUpPage",TitleLVUpPage);
USING_NS_CC;
TitleLVUpPage::TitleLVUpPage(void)
{
}


TitleLVUpPage::~TitleLVUpPage(void)
{
}

void TitleLVUpPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void TitleLVUpPage::Exit( MainFrame* frame)
{
	BlackBoard::Get()->PreTitleId = 0;
	BlackBoard::Get()->UpdateTitleType = 0;
}

void TitleLVUpPage::Execute( MainFrame* )
{
}

void TitleLVUpPage::load( void )
{
	loadCcbiFile("TitleLVUp.ccbi");
}

void TitleLVUpPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage mPopPage;
		mPopPage.pageName = "TitleLVUpPage";
		MessageManager::Get()->sendMessage(&mPopPage);
	}
	else if(itemName=="onViewTitle")
	{
		MsgMainFramePopPage mPopPage;
		mPopPage.pageName = "TitleLVUpPage";
		MessageManager::Get()->sendMessage(&mPopPage);

		BlackBoard::Get()->ShowTitle = BlackBoard::Get()->UpdateTitleType;

		MsgMainFramePushPage msg;
		msg.pageName = "TitleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
void TitleLVUpPage::refreshPage( void )
{
	Title title(BlackBoard::Get()->UpdateTitleType);
	const TitleItem* titleItem = TitleTableManager::Get()->getTitleItemByID(BlackBoard::Get()->PreTitleId);
	CCB_FUNC(this,"mTitleLV",CCLabelBMFont,setString(StringConverter::toString(titleItem->itemID-1000*((int)(titleItem->itemID/1000))).c_str()));
	CCB_FUNC(this,"mTitleLV1",CCLabelBMFont,setString(StringConverter::toString(title.level()).c_str()));
	CCB_FUNC(this,"mName",CCLabelBMFont,setString(titleItem->name.c_str()));
	CCB_FUNC(this,"mName1",CCLabelBMFont,setString(title.name().c_str()));
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(titleItem->itemID-1000*((int)(titleItem->itemID/1000))).c_str()));
	CCB_FUNC(this,"mLV1",CCLabelBMFont,setString(StringConverter::toString(title.level()).c_str()));
	std::string buffQuantity = "";
	if(titleItem->buffType == 1)
	{
		buffQuantity ="+"+StringConverter::toString(titleItem->quantity);
	}
	else
	{
		buffQuantity ="+"+StringConverter::toString(titleItem->quantity) + "%";
	}
	CCB_FUNC(this,"mValue",CCLabelBMFont,setString(buffQuantity.c_str()));
	CCB_FUNC(this,"mValue1",CCLabelBMFont,setString((title.getQuantityString()).c_str()));
	CCB_FUNC(this,"mTitlePic",CCSprite,initWithFile(titleItem->iconPic.c_str()));
	CCB_FUNC(this,"mTitlePic1",CCSprite,initWithFile(title.iconPic().c_str()));
}
