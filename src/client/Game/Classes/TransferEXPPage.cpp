
#include "stdafx.h"

#include "TransferEXPPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("TransferEXPPage",TransferEXPPage);
USING_NS_CC;

TransferEXPPage::TransferEXPPage(void)
{

}

TransferEXPPage::~TransferEXPPage(void)
{
}

void TransferEXPPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void TransferEXPPage::Exit( MainFrame* frame)
{

}

void TransferEXPPage::Execute( MainFrame* )
{

}

void TransferEXPPage::load( void )
{
	loadCcbiFile("TransferEXP.ccbi");	
}

void TransferEXPPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onSellectTransferMember")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "TranExpChooseMemberPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onQuit")
	{
		BlackBoard::Get()->TransferExp_Receiver = 0;
		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpDisciplePageName;
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onTransferEXP")
	{
		MSG_BOX("@MSG_ChooseDisciple");
	}
}

void TransferEXPPage::refreshPage( void )
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

	
	Disciple disciple(BlackBoard::Get()->TransferExp_Receiver);

	CCSprite* prop=dynamic_cast<CCSprite* >(getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(disciple.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(getVariable("mFace"));
	if(frame)
	{
		frame->setNormalImage(disciple.getFrameNormalSpirte());
		frame->setSelectedImage(disciple.getFrameSelectedSpirte());
	}
	CCLabelBMFont* memberName=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName"));
	if(memberName)
	{
		memberName->setString(Language::Get()->getString(disciple.name()).c_str());
	}
	CCSprite* memberQuality=dynamic_cast<CCSprite* >(getVariable("mMemberQuality"));
	if(memberQuality)
	{
		memberQuality->initWithFile(disciple.getQualityImageFile().c_str());
	}
	CCLabelBMFont* memberLV=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV"));
	if(memberLV)
	{
		memberLV->setString(StringConverter::toString(disciple.level()).c_str());
	}
	CCMenuItemImage* mTransferEXP = dynamic_cast<CCMenuItemImage* >(getVariable("mTransferEXP"));
	if(mTransferEXP)
	{
		if(!disciple.canUpgradeLevel())
		{
			mTransferEXP->setEnabled(false);
		}else{

		}
	}
}
