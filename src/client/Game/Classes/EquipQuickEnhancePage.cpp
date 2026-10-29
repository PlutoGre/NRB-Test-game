
#include "stdafx.h"

#include "EquipQuickEnhancePage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipQuickEnhancePage.h"
#include "MessageBoxPage.h"
#include "GameMaths.h"

REGISTER_PAGE("EquipQuickEnhancePage",EquipQuickEnhancePage);
USING_NS_CC;

EquipQuickEnhancePage::EquipQuickEnhancePage(void)
{
	mCard = NULL;
}

EquipQuickEnhancePage::~EquipQuickEnhancePage(void)
{

}

void EquipQuickEnhancePage::load( void )
{
	loadCcbiFile("EquipEnhanceQuickResult.ccbi");
	mEquipCard = dynamic_cast<CCNode*>(getVariable("mEquipCard"));
	refreshPage();
	buildCard();
}

void EquipQuickEnhancePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		//¹Ø±Õ
		BlackBoard::Get()->NeedChangeEquip = false;
		BlackBoard::Get()->NeedChooseEquip = false;
		BlackBoard::Get()->ShowEquip = 0;
		MsgMainFramePopPage msg;
		msg.pageName = "EquipQuickEnhancePage";
		MessageManager::Get()->sendMessage(&msg);
	}else if(itemName == "onConform"){
		BlackBoard::Get()->NeedChangeEquip = false;
		BlackBoard::Get()->NeedChooseEquip = false;
		BlackBoard::Get()->ShowEquip = 0;
		MsgMainFramePopPage msg;
		msg.pageName = "EquipQuickEnhancePage";
		MessageManager::Get()->sendMessage(&msg);

	}
}

void EquipQuickEnhancePage::refreshPage( void )
{
	Equip equip( BlackBoard::Get()->UpgradeEquip );
	unsigned int upLevel = equip.level() - BlackBoard::Get()->EquipBeforeLevel;
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();

	std::list<std::string> replaceList;
	replaceList.push_back(StringConverter::toString(BlackBoard::Get()->quickEnhanceTimes));
	replaceList.push_back(StringConverter::toString(upLevel));
	replaceList.push_back(StringConverter::toString( BlackBoard::Get()->EquipBeforeSilverCoins - userBasicInfo.silvercoins));
	std::string outstr;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickEnhanceDescribe"));
	GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@QuickEnhanceTxt"),&replaceList),outstr,width,lines);
	CCB_FUNC(this,"mQuickResult",CCLabelBMFont,setString(outstr.c_str()));
	
}

void EquipQuickEnhancePage::buildCard( void )
{
	
}

void EquipQuickEnhancePage::unload( void )
{

}
