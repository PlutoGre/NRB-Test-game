
#include "stdafx.h"

#include "EquipEnhanceAniPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipCardPage.h"
#include "BlackBoard.h"
#include "SoundManager.h"
#include "PopupManager.h"
REGISTER_PAGE("EquipEnhanceAniPage",EquipEnhanceAniPage);
USING_NS_CC;

EquipEnhanceAniPage::EquipEnhanceAniPage(void)
{

}

EquipEnhanceAniPage::~EquipEnhanceAniPage(void)
{
}

void EquipEnhanceAniPage::load( void )
{
	loadCcbiFile("EnhanceAni.ccbi");
	setListener(this);
	refreshPage();

	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("EquipEnhance"));
}

void EquipEnhanceAniPage::refreshPage( void )
{
	Equip equip( BlackBoard::Get()->UpgradeEquip );

	CCSprite* equipmentCar=dynamic_cast<CCSprite* >(getVariable("mEquipmentCar"));
	if(equipmentCar)
	{
		equipmentCar->initWithFile(equip.iconPic().c_str());
	}

	unsigned int upLevel = equip.level() - BlackBoard::Get()->EquipBeforeLevel;
	if(upLevel >= 1 && upLevel <= 6)
	{
		runAnimation("lvup"+StringConverter::toString(upLevel));
	}
	else
	{
		char msg[256];
		sprintf(msg,"Equip Enhance Level %s error!", StringConverter::toString(upLevel).c_str());
		cocos2d::CCMessageBox(msg,"ERROR!");
	}
}

void EquipEnhanceAniPage::onAnimationDone( const std::string& animationName )
{
	MsgMainFramePopPage msg;
	msg.pageName = "EquipEnhanceAniPage";
	MessageManager::Get()->sendMessage(&msg);

	if(GET_V_B("EquipEnhanceResultPage"))
	{
		MsgMainFrameRefreshPage msg1;
		msg1.pageName = "EquipEnhanceResultPage";
		MessageManager::Get()->sendMessage(&msg1);
	}
	else
	{
		MsgMainFrameChangePage msg1;
		msg1.pageName = "EquipEnhanceResultPage";
		MessageManager::Get()->sendMessage(&msg1);
	}
	PopupManager::Get()->showCoinsEnoughGuide();
}
