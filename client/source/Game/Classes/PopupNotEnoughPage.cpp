
#include "stdafx.h"

#include "PopupNotEnoughPage.h"
#include "GameMessages.h"
#include "DataTableManager.h"
#include "GameMaths.h"
#include "PacketManager.h"
#include "StringConverter.h"
#include "PopupPage.h"
#include "Popup1stPayTipPage.h"
#include "TimeCalculator.h"

REGISTER_PAGE("PopupNotEnoughPage",PopupNotEnoughPage);
USING_NS_CC;
PopupNotEnoughPage::PopupNotEnoughPage(void)
{
	mDiamonToClearTime = 0;
}


PopupNotEnoughPage::~PopupNotEnoughPage(void)
{
}
void PopupNotEnoughPage::load( void )
{
	loadCcbiFile("PopUpNotEnoughT.ccbi");
}
void PopupNotEnoughPage::showPage(Popup_Not_Enough_type _type)
{
	BasePage* obj = CCBManager::Get()->getPage("PopupNotEnoughPage");
	PopupNotEnoughPage* page = dynamic_cast<PopupNotEnoughPage*>(obj);
	if(page)
	{
		page->mType=_type;
	}
	MsgMainFramePushPage msg;
	msg.pageName="PopupNotEnoughPage";
	MessageManager::Get()->sendMessage(&msg);
}
void PopupNotEnoughPage::initPage()
{
	char buff[256] = {0};
	if(mType==PowerNotEnough)
	{
		const ToolItem* item = ToolTableManager::Get()->getToolItemByID(2501001);
		mTitleKey="@PowerNotEnoughTitle";
		mMsgKey="@PowerNotEnoughMsg";
		mPropNameStr = item->name;
		mPropIconStr = item->iconPic;
		mPriceStr = StringConverter::toString(item->price);
		mFunction1Key="@GoArena";
		mFunction2Key="@RecoveryPower";
		const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
		std::string s = Language::Get()->getString("@PowerAddLimitMsg");
		unsigned int nextLimitVipLevel = ResManager::Get()->getNextDailyLimitVipLevel(info.viplevel);
		if(nextLimitVipLevel!=0)
		{
			s += Language::Get()->getString("@PowerNextVipLimitMsg");
			sprintf(buff , s.c_str() ,info.todayAddedPowerNum,ResManager::Get()->getVipDailyLimit(info.viplevel),nextLimitVipLevel,ResManager::Get()->getVipDailyLimit(nextLimitVipLevel));
		}else
		{
			sprintf(buff , s.c_str() ,info.todayAddedPowerNum,ResManager::Get()->getVipDailyLimit(info.viplevel));
		}
	}
	else if(mType==VitalityNotEnough)
	{
		const ToolItem* item = ToolTableManager::Get()->getToolItemByID(2502001);
		mTitleKey="@VatalityNotEnoughTitle";
		mMsgKey="@VatalityNotEnoughMsg";
		mPropNameStr = item->name;
		mPropIconStr = item->iconPic;
		mPriceStr = StringConverter::toString(item->price);
		mFunction1Key="@GoCareer";
		mFunction2Key="@RecoveryVitality";
		const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
		std::string s = Language::Get()->getString("@VatalityAddLimitMsg");
		unsigned int nextLimitVipLevel = ResManager::Get()->getNextDailyLimitVipLevel(info.viplevel);
		if(nextLimitVipLevel!=0)
		{
			s += Language::Get()->getString("@VatalityNextVipLimitMsg");
			sprintf(buff , s.c_str() ,info.todayAddedVitalityNum,ResManager::Get()->getVipDailyLimit(info.viplevel),nextLimitVipLevel,ResManager::Get()->getVipDailyLimit(nextLimitVipLevel));
		}
		else
		{
			sprintf(buff , s.c_str() ,info.todayAddedVitalityNum,ResManager::Get()->getVipDailyLimit(info.viplevel));
		}
	}else if (mType == SpendDiamondClearCoolTime)//add by xinghui 2013/7/1
	{
		mTitleKey="@SpendDiamondClearCoolTime";
		mMsgKey="@ClearCoolTimeMsg";
		mFunction1Key="@GoCareer";
		mPropNameStr = Language::Get()->getString("@ZuanShi");
		mFunction2Key="@DiamonCosume";
		std::string s = Language::Get()->getString("@SpendDiamond");
		long aa = TimeCalculator::Get()->getTimeLeft("coldtime");
		int midd = TimeCalculator::Get()->getTimeLeft("coldtime")/60%StringConverter::parseInt(VaribleManager::Get()->getSetting("Divisor"))>0? TimeCalculator::Get()->getTimeLeft("coldtime")/60/StringConverter::parseInt(VaribleManager::Get()->getSetting("Divisor"))+1 : TimeCalculator::Get()->getTimeLeft("coldtime")/60/StringConverter::parseInt(VaribleManager::Get()->getSetting("Divisor"));
		mDiamonToClearTime = midd*StringConverter::parseInt(VaribleManager::Get()->getSetting("BaseDiamon"));
		sprintf(buff, s.c_str(), mDiamonToClearTime);
	}
	std::string msgStr;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("PopupNotEnoughPage_Msg"));
	GameMaths::stringAutoReturn(buff,mPropDescribeStr,width,lines);
}
void PopupNotEnoughPage::Enter( MainFrame* )
{
	initPage();
	CCB_FUNC(this,"mTiltle",CCLabelBMFont,setString(Language::Get()->getString(mTitleKey).c_str()));
	std::string msgStr;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("PopupNotEnoughPage_PropDescribe"));
	GameMaths::stringAutoReturn(Language::Get()->getString(mMsgKey),msgStr,width,lines);
	const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
	CCB_FUNC(this,"mNoteMsg1",CCLabelBMFont,setString(msgStr.c_str()));
	CCB_FUNC(this,"mPropName",CCLabelBMFont,setString(mPropNameStr.c_str()));
	CCB_FUNC(this,"mNoteMsg2",CCLabelBMFont,setString(mPropDescribeStr.c_str()));
	//CCB_FUNC(this,"mSellPrice",CCLabelBMFont,setString(mPriceStr.c_str()));
	CCB_FUNC(this,"mFunc1Lable",CCLabelBMFont,setString(Language::Get()->getString(mFunction1Key).c_str()));
	CCB_FUNC(this,"mFunc2Lable",CCLabelBMFont,setString(Language::Get()->getString(mFunction2Key).c_str()));
	std::string aa = Language::Get()->getString(mFunction2Key);
	CCSprite* propIcon=dynamic_cast<CCSprite* >(getVariable("mHeadPic1"));
	if(propIcon)
	{
		propIcon->initWithFile(mPropIconStr.c_str());
	}

	if(mType==PowerNotEnough)
	{
		struct tm *localTime;
		time_t t;
		t = time(NULL);
		localTime = localtime(&t);

		int totalPrice = StringConverter::parseInt(mPriceStr)*(info.todayBuyPowerNum+1);
		CCB_FUNC(this,"mSellPrice", CCLabelBMFont,setString(StringConverter::toString(totalPrice).c_str()));

		CCB_FUNC(this,"mHeadPic2",CCSprite,setTexture(VaribleManager::Get()->getSetting("PowerPopNotEnoughImageFile").c_str()));
		CCMenuItemImage* menu = dynamic_cast<CCMenuItemImage* >(getVariable("mFunction2"));
		if(info.todayAddedPowerNum>=ResManager::Get()->getVipDailyLimit(info.viplevel))
		{
			if(menu)
			{
				menu->setEnabled(false);
			}
		}
		else
		{
			if(menu)
			{
				menu->setEnabled(true);
			}
		}
	}
	else if(mType==VitalityNotEnough)
	{
		int totalPrice = StringConverter::parseInt(mPriceStr)*(info.todayBuyVitalityNum+1);
		CCB_FUNC(this,"mSellPrice", CCLabelBMFont,setString(StringConverter::toString(totalPrice).c_str()));

		CCB_FUNC(this,"mHeadPic2",CCSprite,setTexture(VaribleManager::Get()->getSetting("VitalityPopNotEnoughImageFile").c_str()));
		CCMenuItemImage* menu = dynamic_cast<CCMenuItemImage* >(getVariable("mFunction2"));
		if(info.todayAddedVitalityNum>=ResManager::Get()->getVipDailyLimit(info.viplevel))
		{
			if(menu)
			{
				menu->setEnabled(false);
			}
		}
		else
		{

			if(menu)
			{
				menu->setEnabled(true);
			}
		}
	}else if (mType == SpendDiamondClearCoolTime)
	{
		CCB_FUNC(this,"mSellPrice", CCLabelBMFont,setString(StringConverter::toString(mDiamonToClearTime).c_str()));
		CCB_FUNC(this,"mHeadPic1",CCSprite,setTexture(VaribleManager::Get()->getSetting("Diamon").c_str()));
	}
	
}

void PopupNotEnoughPage::Execute( MainFrame* )
{

}

void PopupNotEnoughPage::Exit( MainFrame* )
{

}
void PopupNotEnoughPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	bool isClose=false;
	if(itemName=="onClose")
	{
		isClose=true;
	}
	else if(itemName=="onFunction1")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PopupNotEnoughPage";
		MessageManager::Get()->sendMessage(&msg);
		if(mType==PowerNotEnough)
		{
			MsgMainFrameChangePage msgChangePage;
			msgChangePage.pageName = "ArenaPage";
			MessageManager::Get()->sendMessage(&msgChangePage);
		}
		else if(mType==VitalityNotEnough)
		{
			MsgMainFrameChangePage msgChangePage;
			msgChangePage.pageName = "CareerPage";
			MessageManager::Get()->sendMessage(&msgChangePage);
		}
	}
	else if(itemName=="onFunction2")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PopupNotEnoughPage";
		MessageManager::Get()->sendMessage(&msg);
		if(mType==PowerNotEnough)
		{
			const UserBasicInfo basicInfo=ServerDateManager::Get()->getUserBasicInfo();
			const ToolItem * item = ToolTableManager::Get()->getToolItemByID(HAIXIANFAN);
			if(basicInfo.goldcoins<(item->price*(basicInfo.todayBuyPowerNum+1)))//add by xinghui
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
				OPAddPower msgAddPower;
				msgAddPower.set_typeid_(3);
				PacketManager::Get()->sendPakcet(OPCODE_ADD_POWER_C, &msgAddPower);
			}
			
		}
		else if(mType==VitalityNotEnough)
		{
			const UserBasicInfo basicInfo=ServerDateManager::Get()->getUserBasicInfo();
			const ToolItem * item = ToolTableManager::Get()->getToolItemByID(JISUZHEN);
			if(basicInfo.goldcoins<(item->price*(basicInfo.todayBuyVitalityNum+1)))
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
				OPAddVitality msgAddVitality;
				msgAddVitality.set_typeid_(3);
				PacketManager::Get()->sendPakcet(OPCODE_ADD_VITALITY_C, &msgAddVitality);
			}
		}else if (mType == SpendDiamondClearCoolTime)
		{
			const UserBasicInfo basicInfo=ServerDateManager::Get()->getUserBasicInfo();
			if (basicInfo.goldcoins >= mDiamonToClearTime)
			{
				OPQuickCareerBattle msgSpendDiamon;
				msgSpendDiamon.set_version(1);
				msgSpendDiamon.set_type(OPQuickCareerBattle_Type_CARRER);
				msgSpendDiamon.set_opponentid(1);
				msgSpendDiamon.set_costgold(mDiamonToClearTime);
				msgSpendDiamon.set_battletimes(1);
				PacketManager::Get()->sendPakcet(OPCODE_QUICK_CAREER_BATTLE_C, &msgSpendDiamon);
			}else
			{
				POP_BOX(NotEnoughGold);
			}
			
		}
	}
	if(isClose)
	{
		MsgMainFramePopPage msg;
		msg.pageName="PopupNotEnoughPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void PopupNotEnoughPage::Pop_NotEnough_Box( Popup_Not_Enough_type _type )
{
	dynamic_cast<PopupNotEnoughPage*>(CCBManager::Get()->getPage("PopupNotEnoughPage"))->showPage(_type);
}
