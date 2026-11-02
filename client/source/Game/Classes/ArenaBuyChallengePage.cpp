
#include "stdafx.h"

#include "ArenaBuyChallengePage.h"
#include "CCBManager.h"
#include "MessageBoxPage.h"
#include "ServerDateManager.h"
#include "DataTableManager.h"
#include "StringConverter.h"
#include "GameMessages.h"
#include "Language.h"
#include "PopupPage.h"
#include "Popup1stPayTipPage.h"

REGISTER_PAGE("ArenaBuyChallengePage",ArenaBuyChallengePage);
USING_NS_CC;

ArenaBuyChallengePage::ArenaBuyChallengePage( void )
{

}

ArenaBuyChallengePage::~ArenaBuyChallengePage( void )
{

}

void ArenaBuyChallengePage::load( void )
{
	loadCcbiFile("ArenaBuyChallengeLetter.ccbi");
	refreshPage();
}

void ArenaBuyChallengePage::refreshPage( void )
{
	const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(2503001);
	const ToolItem * item = ToolTableManager::Get()->getToolItemByID(2503001);
	if(!item)
		return;

	if(info && info->count > 0)
	{
		CCLabelBMFont* title=dynamic_cast<CCLabelBMFont* >(getVariable("mTitle"));
		if(title)
		{
			title->setString(Language::Get()->getString("@BuyChallengeLetter").c_str());
		}
		CCLabelBMFont* buttonLabel=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonLable"));
		if(buttonLabel)
		{
			buttonLabel->setString(Language::Get()->getString("@UseChallenge").c_str());
		}

		CCLabelBMFont* number1=dynamic_cast<CCLabelBMFont* >(getVariable("mNumber1"));
		if(number1)
		{
			number1->setString(StringConverter::toString(info->count).c_str());
		}
		CCNode* priceNode=dynamic_cast<CCNode* >(getVariable("mPriceNode"));
		if(priceNode)
		{
			priceNode->setVisible(false);
		}
	}
	else
	{
		CCLabelBMFont* title=dynamic_cast<CCLabelBMFont* >(getVariable("mTitle"));
		if(title)
		{
			title->setString(Language::Get()->getString("@UseChallengeLetter").c_str());
		}

		CCLabelBMFont* buttonLabel=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonLable"));
		if(buttonLabel)
		{
			buttonLabel->setString(Language::Get()->getString("@BuyChallenge").c_str());
		}

		CCLabelBMFont* number1=dynamic_cast<CCLabelBMFont* >(getVariable("mNumber1"));
		if(number1)
		{
			number1->setString("0");
		}
		CCNode* priceNode=dynamic_cast<CCNode* >(getVariable("mPriceNode"));
		if(priceNode)
		{
			priceNode->setVisible(true);
		}
		CCLabelBMFont* priceLabel=dynamic_cast<CCLabelBMFont* >(getVariable("mPrice"));
		if(priceLabel)
		{
			priceLabel->setVisible(true);
			priceLabel->setString(StringConverter::toString(item->price).c_str());
		}
	}
}

void ArenaBuyChallengePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose" || itemName=="onXClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ArenaBuyChallengePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if (itemName == "onFunciton")
	{
		const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(TIAOZHANSHU);
		const ToolItem * item = ToolTableManager::Get()->getToolItemByID(TIAOZHANSHU);
		if(!item)
			return;
		
		MsgMainFramePopPage msg;
		msg.pageName = "ArenaBuyChallengePage";
		MessageManager::Get()->sendMessage(&msg);
		if(info && info->count > 0)
		{
			ResManager::Get()->useToolByIDAndCount(info->id,TIAOZHANSHU,1);
		}
		else
		{
			const UserBasicInfo basicInfo=ServerDateManager::Get()->getUserBasicInfo();
			if(basicInfo.goldcoins<item->price)
			{	
				if(!ServerDateManager::Get()->_getMarketInfo().firstPay)
				{
					POP_BOX(NotEnoughGold);
				}
				else
				{
					POP_1STPAY_TIP_BOX(GoldNotEnough);
				}
			}
			else
			{
				OPAddArenaNum msgAddArena;
				msgAddArena.set_typeid_(1);
				PacketManager::Get()->sendPakcet(OPCODE_ADD_ARENA_NUM_C, &msgAddArena);
			}
		}
		
	}
}


