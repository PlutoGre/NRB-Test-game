
#include "stdafx.h"

#include "GuideBuyEquipPage.h"
#include "StringConverter.h"
#include "MessageBoxPage.h"
#include "PopupManager.h"
#include "PopupPage.h"
#include "Equip.h"
#include "Popup1stPayTipPage.h"

REGISTER_PAGE("GuideBuyEquipPage",GuideBuyEquipPage);
USING_NS_CC;

GuideBuyEquipPage::GuideBuyEquipPage(void)
{
}


GuideBuyEquipPage::~GuideBuyEquipPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void GuideBuyEquipPage::Enter(MainFrame*)
{
	Equip equip(StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipId")),true);
	if(equip.itemID())
	{
		CCB_FUNC(this,"mPic",CCSprite,initWithFile(equip.iconPic().c_str()));
		CCB_FUNC(this,"mPrice",CCLabelBMFont,setString(VaribleManager::Get()->getSetting("GuideBuyEquipPrice").c_str()));
		CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@guideBuyEquipTitle").c_str()));
		CCB_FUNC(this,"mSummary",CCLabelBMFont,setString(""));
		CCB_FUNC(this,"mFrame",CCSprite,setColor(equip.getFrameColor()));
		std::string outstr;
		int lines;
		GameMaths::stringAutoReturn(Language::Get()->getString("@guideBuyEquipDescription"),outstr,StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipDescriptionNumber")),lines);

		CCB_FUNC(this,"mDescription",CCLabelBMFont,setString(outstr.c_str()));
		CCB_FUNC(this,"mName",CCLabelBMFont,setString(equip.name().c_str()));

		std::list<std::string> _listNote;
		_listNote.push_back(VaribleManager::Get()->getSetting("GuideBuyEquipMaxLevel"));
		std::string guideBuyEquipNote1 = GameMaths::replaceStringWithStringList(Language::Get()->getString("@guideBuyEquipNote1"),&_listNote);

		CCB_FUNC(this,"mNote1",CCLabelBMFont,setString(guideBuyEquipNote1.c_str()));

		std::list<std::string> _listNumber;
		_listNumber.push_back(VaribleManager::Get()->getSetting("GuideBuyEquipPrice"));
		std::string guideBuyEquipNumber = GameMaths::replaceStringWithStringList(Language::Get()->getString("@guideBuyEquipNumber"),&_listNumber);
		CCB_FUNC(this,"mNumber",CCLabelBMFont,setString((guideBuyEquipNumber).c_str()));
	}
}

void GuideBuyEquipPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void GuideBuyEquipPage::Execute( MainFrame* )
{

}

void GuideBuyEquipPage::load( void )
{
	loadCcbiFile("GuidEquipSale.ccbi");
}

void GuideBuyEquipPage::showPage()
{
	MsgMainFramePushPage msg;
	msg.pageName="GuideBuyEquipPage";
	MessageManager::Get()->sendMessage(&msg);
	
}
void GuideBuyEquipPage::inputMethodCallBack(const std::string& content)
{
}

void GuideBuyEquipPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction2")
	{
		if(PopupManager::Get()->checkShowBuyEquip())
		{
			int price=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipPrice"));
			if(ServerDateManager::Get()->getUserBasicInfo().goldcoins>=price)
			{
				OPGuideBuyTool msg;
				msg.set_version(1);
				msg.set_typeid_(1);
				PacketManager::Get()->sendPakcet(OPCODE_GUIDE_BUY_TOOL_C, &msg);
				PopupManager::Get()->setShowStatsByType("GBuyEquip",2);
			}
			else
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
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@AlreatBuy"));
		}
	}
	MsgMainFramePopPage popPage;
	popPage.pageName="GuideBuyEquipPage";
	MessageManager::Get()->sendMessage(&popPage);
}

void GuideBuyEquipPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void GuideBuyEquipPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_CDKEY_REWARDRET_S)
		{
			
		}
	}
}
