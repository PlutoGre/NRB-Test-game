
#include "stdafx.h"

#include "GuideBuyExpPage.h"
#include "StringConverter.h"
#include "MessageBoxPage.h"
#include "PopupManager.h"
#include "PopupPage.h"
#include "Popup1stPayTipPage.h"

REGISTER_PAGE("GuideBuyExpPage",GuideBuyExpPage);
USING_NS_CC;

GuideBuyExpPage::GuideBuyExpPage(void)
{
}


GuideBuyExpPage::~GuideBuyExpPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void GuideBuyExpPage::Enter(MainFrame*)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	mCurrLevel=ServerDateManager::Get()->getUserBasicInfo().level;
	CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@guideBuyExpTitle").c_str()));
	CCB_FUNC(this,"mPic",CCSprite,initWithFile(VaribleManager::Get()->getSetting("GuideBuyExpIcon").c_str()));
	CCB_FUNC(this,"mPrice",CCLabelBMFont,setString(VaribleManager::Get()->getSetting("GuideBuyExpPrice").c_str()));
	CCB_FUNC(this,"mSummary",CCLabelBMFont,setString(""));
	std::string outstr;
	int lines;
	GameMaths::stringAutoReturn(Language::Get()->getString("@guideBuyExpDescription"),outstr,StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyExpDescriptionNumber")),lines);

	CCB_FUNC(this,"mDescription",CCLabelBMFont,setString(outstr.c_str()));

	CCB_FUNC(this,"mName",CCLabelBMFont,setString(Language::Get()->getString("@guideBuyExpName").c_str()));

	std::list<std::string> _listNote;
	_listNote.push_back(VaribleManager::Get()->getSetting("GuideBuyExpMaxLevel"));
	std::string guideBuyExpNote1 = GameMaths::replaceStringWithStringList(Language::Get()->getString("@guideBuyExpNote1"),&_listNote);

	CCB_FUNC(this,"mNote1",CCLabelBMFont,setString(guideBuyExpNote1.c_str()));


	std::list<std::string> _listNumber;
	_listNumber.push_back(VaribleManager::Get()->getSetting("GuideBuyExpPrice"));
	std::string guideBuyExpNumber = GameMaths::replaceStringWithStringList(Language::Get()->getString("@guideBuyExpNumber"),&_listNumber);
	CCB_FUNC(this,"mNumber",CCLabelBMFont,setString((guideBuyExpNumber).c_str()));


}

void GuideBuyExpPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void GuideBuyExpPage::Execute( MainFrame* )
{

}

void GuideBuyExpPage::load( void )
{
	loadCcbiFile("GuidEquipSale.ccbi");
}

void GuideBuyExpPage::showPage()
{
	MsgMainFramePushPage msg;
	msg.pageName="GuideBuyExpPage";
	MessageManager::Get()->sendMessage(&msg);
	
}
void GuideBuyExpPage::inputMethodCallBack(const std::string& content)
{
}

void GuideBuyExpPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction2")
	{
		if(PopupManager::Get()->checkShowBuyExp())
		{
			int price=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipPrice"));
			if(ServerDateManager::Get()->getUserBasicInfo().goldcoins>=price)
			{
				OPGuideBuyTool msg;
				msg.set_version(1);
				msg.set_typeid_(2);
				PacketManager::Get()->sendPakcet(OPCODE_GUIDE_BUY_TOOL_C, &msg);
				PopupManager::Get()->setShowStatsByType("GBuyExp",2);
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
	else
	{
		MsgMainFramePopPage popPage;
		popPage.pageName="GuideBuyExpPage";
		MessageManager::Get()->sendMessage(&popPage);
	}
}

void GuideBuyExpPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void GuideBuyExpPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_GUIDE_BUY_TOOLRET_S)
		{
			MsgMainFramePopPage popPage;
			popPage.pageName="GuideBuyExpPage";
			MessageManager::Get()->sendMessage(&popPage);

			ServerDateManager::Get()->getNextLevelExp();
			if(mCurrLevel!=ServerDateManager::Get()->getUserBasicInfo().level)
			{
				PopupManager::Get()->popPage();
			}
		}
	}
}
