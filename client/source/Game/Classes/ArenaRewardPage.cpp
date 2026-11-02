
#include "stdafx.h"

#include "ArenaRewardPage.h"
#include "DataTableManager.h"
#include "StringConverter.h"
#include "GameMessages.h"
#include "BlackBoard.h"
#include "CCBManager.h"
#include "Language.h"
#include "cocos2d.h"
#include "Arena.h"
#include "GamePackets.h"
#include "MessageBoxPage.h"
#include "GameMaths.h"

REGISTER_PAGE("ArenaRewardPage",ArenaRewardPage);
USING_NS_CC;

void ArenaRewardPage::load( void )
{
	loadCcbiFile("ArenaRewardPopUp.ccbi");
	refreshPage();
}

void ArenaRewardPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onFunciton1")
	{
		DEL_V("ArenaRewardID");
		BlackBoard::Get()->DisciplePageTab = 1;
		MsgMainFrameChangePage msg;
		msg.pageName = "DisciplePage";
		MessageManager::Get()->sendMessage(&msg);

		MsgMainFramePopPage msg1;
		msg1.pageName = "ArenaRewardPage";
		MessageManager::Get()->sendMessage(&msg1);
	}
	else if(itemName == "onFunction2")
	{
		unsigned int id = GET_V_UINT("ArenaRewardID");
		ArenaReward arenaReward(id);
		if(arenaReward.type() == 1)
		{
			if(arenaReward.isEnable())
			{
				OPGetArenaReward getArenaReward;
				getArenaReward.set_rewardid(id);
				PacketManager::Get()->sendPakcet(OPCODE_GET_ARENA_REWARD_C,&getArenaReward);

				MsgMainFramePopPage msg1;
				msg1.pageName = "ArenaRewardPage";
				MessageManager::Get()->sendMessage(&msg1);
			}
			else
			{
				MSG_BOX(Language::Get()->getString("@RewardNotEnough"));
			}
		}
		else
		{
			DEL_V("ArenaRewardID");

			MsgMainFramePopPage msg;
			msg.pageName = "ArenaRewardPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		
	}
	else if(itemName == "onClose")
	{
		DEL_V("ArenaRewardID");

		MsgMainFramePopPage msg;
		msg.pageName = "ArenaRewardPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void ArenaRewardPage::refreshPage()
{
	unsigned int id = GET_V_UINT("ArenaRewardID");
	ArenaReward arenaReward(id);

	std::string mSummary = Language::Get()->getString("@PopUpContulation");
	std::string mName;
	std::string mNumber = "";
	std::string mDescription;
	std::string mNote1 = "";
	std::string mFunctionLable1 = Language::Get()->getString("@TrainDisciple");
	std::string mFunctionLable2 = Language::Get()->getString("@Continue_GetArenaReward");
	if(arenaReward.type() == 1)
		mFunctionLable2 = Language::Get()->getString("@Continue_GetArenaReward");
	else
		mFunctionLable2 = Language::Get()->getString("@Close");


	const UserToolInfo* info = ServerDateManager::Get()->getUserToolInfoByItemId(1502001);
	if(info)
		mNumber = StringConverter::toString(info->count);
	const ToolItem* item = ToolTableManager::Get()->getToolItemByID(1502001);
	if(item)
	{
		mName = item->name;
		std::string outstr;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("SurpriseDescription"));
		GameMaths::stringAutoReturn(item->describe,mDescription,width,lines);
		mSummary.append(mName);
	}

	CCSprite* pic=dynamic_cast<CCSprite* >(getVariable("mPic"));
	if(pic)
	{
		pic->initWithFile(item->iconPic.c_str());
	}
	CCSprite* frame=dynamic_cast<CCSprite* >(getVariable("mFrame"));
	if(frame)
	{
		frame->initWithFile(::getQualityFrameNormalImageFile().c_str());
	}
	CCB_FUNC(this,"mNumber",CCLabelBMFont,setString(mNumber.c_str()));
	CCB_FUNC(this,"mName",CCLabelBMFont,setString(mName.c_str()));
	CCB_FUNC(this,"mNote1",CCLabelBMFont,setString(mNote1.c_str()));
	CCB_FUNC(this,"mDescription",CCLabelBMFont,setString(mDescription.c_str()));
	CCB_FUNC(this,"mNumber1",CCLabelBMFont,setString(mNumber.c_str()));
	CCB_FUNC(this,"mSummary",CCLabelBMFont,setString(mSummary.c_str()));
	
	CCB_FUNC(this,"mFunctionLable1",CCLabelBMFont,setString(mFunctionLable1.c_str()));
	CCB_FUNC(this,"mFunctionLable2",CCLabelBMFont,setString(mFunctionLable2.c_str()));
	

}

