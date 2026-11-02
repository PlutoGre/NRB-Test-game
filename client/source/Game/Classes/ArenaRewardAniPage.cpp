
#include "stdafx.h"

#include "ArenaRewardAniPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipCardPage.h"
#include "SoundManager.h"

REGISTER_PAGE("ArenaRewardAniPage",ArenaRewardAniPage);
USING_NS_CC;

ArenaRewardAniPage::ArenaRewardAniPage(void)
{

}

ArenaRewardAniPage::~ArenaRewardAniPage(void)
{
}

void ArenaRewardAniPage::load( void )
{
	loadCcbiFile("pharmacy.ccbi");
	setListener(this);
	refreshPage();
	runAnimation("Default Timeline");

	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("ArenaReward"));
}

void ArenaRewardAniPage::refreshPage( void )
{
// 	Equip equip( BlackBoard::Get()->UpgradeEquip );
// 
// 	CCSprite* equipmentCar=dynamic_cast<CCSprite* >(getVariable("mEquipmentCar"));
// 	if(equipmentCar)
// 	{
// 		equipmentCar->initWithFile(equip.iconPic().c_str());
// 	}
// 
// 	unsigned int upLevel = equip.level() - BlackBoard::Get()->EquipBeforeLevel;
// 	if(upLevel >= 1 && upLevel <= 6)
// 	{
// 		runAnimation("lvup"+StringConverter::toString(upLevel));
// 	}
// 	else
// 	{
// 		char msg[256];
// 		sprintf(msg,"Equip Enhance Level %s error!", StringConverter::toString(upLevel).c_str());
// 		cocos2d::CCMessageBox(msg,"ERROR!");
// 	}
}

void ArenaRewardAniPage::onAnimationDone( const std::string& animationName )
{
	MsgMainFramePopPage msg;
	msg.pageName = "ArenaRewardAniPage";
	MessageManager::Get()->sendMessage(&msg);

	MsgMainFramePushPage msg1;
	msg1.pageName = "ArenaRewardPage";
	MessageManager::Get()->sendMessage(&msg1);
}
