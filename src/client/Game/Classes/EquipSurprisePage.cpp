
#include "stdafx.h"

#include "EquipSurprisePage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipCardPage.h"
#include "PopupManager.h"
REGISTER_PAGE("EquipSurprisePage",EquipSurprisePage);
USING_NS_CC;

EquipSurprisePage::EquipSurprisePage(void)
{
	mCard = NULL;
}

EquipSurprisePage::~EquipSurprisePage(void)
{
}

void EquipSurprisePage::load( void )
{
	loadCcbiFile("EquipmentInfo4.ccbi");
	mEquipCard = dynamic_cast<CCNode*>(getVariable("mEquipCard"));
	refreshPage();
	buildCard();
}

void EquipSurprisePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	BlackBoard::Get()->ShowEquip = 0;
	bool isClose=true;
	if(itemName=="onFunction1")
	{
	}
	else if(itemName=="onFunction2")
	{
	}
	else if(itemName=="onClose")
	{
	}
	else if(itemName=="onXClose")
	{
	}
	if(isClose)
	{
		MsgMainFramePopPage msg;
		msg.pageName = "EquipSurprisePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	PopupManager::Get()->popPage();
}

void EquipSurprisePage::refreshPage( void )
{
	Equip equip( BlackBoard::Get()->ShowEquip);
	
	CCLabelBMFont* buttonName1=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName1"));
	if(buttonName1)
	{
		buttonName1->setString(Language::Get()->getString("@Recieve").c_str());
	}
	CCLabelBMFont* buttonName2=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName2"));
	if(buttonName2)
	{
		buttonName2->setString(Language::Get()->getString("@Close").c_str());
	}
	CCLabelBMFont* equipValue = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipValue"));
	if(equipValue)
	{
		equipValue->setString(StringConverter::toString(equip.getEquipValue()).c_str());
	}
	CCLabelBMFont* equipIntroduce = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipIntro"));
	if(equipIntroduce)
	{
		equipIntroduce->setString(Language::Get()->getString(equip.describe()).c_str());
	}
	CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@RewardSurpriseTitle").c_str()));
}

void EquipSurprisePage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(BlackBoard::Get()->ShowEquip);
		mCard->refreshContent();
	}
	else
	{
		mCard = new EquipCardPage(BlackBoard::Get()->ShowEquip);
		mCard->init();
		mEquipCard->removeAllChildren();
		mEquipCard->addChild(mCard->getContainerNode());
	}

	
}

void EquipSurprisePage::unload( void )
{
	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}
