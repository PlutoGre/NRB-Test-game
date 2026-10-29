
#include "stdafx.h"

#include "FortuneComeReward.h"
#include "PopupManager.h"
REGISTER_PAGE("FortuneComeReward",FortuneComeReward);
USING_NS_CC;

FortuneComeReward::FortuneComeReward(void)
{
}

FortuneComeReward::~FortuneComeReward(void)
{
}


void FortuneComeReward::Enter( MainFrame* frame)
{

	mPopMsg = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getVariable("mPopMsg"));
	mMessage = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getVariable("mMessage"));

	FortuneComeItem& fortuneComeItem = ServerDateManager::Get()->getFortuneComeItem();

	std::list<std::string> popMsgList;
	popMsgList.push_back(StringConverter::toString(fortuneComeItem.costGold).c_str());
	popMsgList.push_back(StringConverter::toString(fortuneComeItem.rewardGold).c_str());

	std::string outstr;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("FortuneComePopMsg"));
	GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FortuneComePopMsg"),&popMsgList),outstr,width,lines);
	mPopMsg->setString(outstr.c_str());


	mMessage->setString(StringConverter::toString(fortuneComeItem.rewardGold).c_str());

}


void FortuneComeReward::Exit( MainFrame* frame)
{
}

void FortuneComeReward::Execute( MainFrame* )
{

}

void FortuneComeReward::load( void )
{
	loadCcbiFile("GodofWealthcode.ccbi");
}

void FortuneComeReward::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void FortuneComeReward::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "FortuneComeReward";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onSendMsgButton")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "FortuneComeReward";
		MessageManager::Get()->sendMessage(&msg);
	}
//	PopupManager::Get()->popPage();
}

