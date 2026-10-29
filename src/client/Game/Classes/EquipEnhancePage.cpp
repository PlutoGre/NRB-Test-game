
#include "stdafx.h"

#include "EquipEnhancePage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipCardPage.h"
#include "MessageBoxPage.h"
#include "CCBContainer.h"
#include "GameMaths.h"



REGISTER_PAGE("EquipEnhancePage",EquipEnhancePage);
USING_NS_CC;

EquipEnhancePage::EquipEnhancePage(void)
{
	mCard = NULL;
}

EquipEnhancePage::~EquipEnhancePage(void)
{
}

void EquipEnhancePage::Enter( MainFrame* frame)
{
	refreshPage();
	buildCard();
}

void EquipEnhancePage::Exit( MainFrame* frame)
{
	delete mCard;
	mCard = NULL;
}

void EquipEnhancePage::Execute( MainFrame* )
{

}

void EquipEnhancePage::load( void )
{
	loadCcbiFile("EquipEnhance.ccbi");
	mEquipCard = dynamic_cast<CCNode*>(getVariable("mEquipCard"));
}

void EquipEnhancePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	if(itemName=="onEnhance")
	{
		Equip equip(BlackBoard::Get()->UpgradeEquip);
		BlackBoard::Get()->EquipBeforeLevel = equip.level();
		BlackBoard::Get()->EquipBeforeQuantity = equip.quantity();
		ADD_OR_SET_V("EquipEnhanceResultPage", false);

		OPUpgradeEquip upgradeEquip;
		upgradeEquip.set_id(equip.id());
		PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_EQUIP_C,&upgradeEquip);
	}
	else if(itemName=="onQuickEnhance"){

		unsigned int vipLimit = StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickEnhanceVIPLimit"));
		if(userBasicInfo.viplevel >= vipLimit){
			Equip equip(BlackBoard::Get()->UpgradeEquip);
			BlackBoard::Get()->EquipBeforeLevel = equip.level();
			BlackBoard::Get()->EquipBeforeQuantity = equip.quantity();
			BlackBoard::Get()->EquipBeforeSilverCoins = userBasicInfo.silvercoins;
			ADD_OR_SET_V("EquipEnhanceResultPage", false);

			OPUpgradeEquip upgradeEquip;
			upgradeEquip.set_id(equip.id());
			upgradeEquip.set_quickupdate(1);
			PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_EQUIP_C,&upgradeEquip);

		}else{
			MSG_BOX(Language::Get()->getString("@QuickEnhanAlert"));
		}


	}
	else if(itemName=="onQuit")
	{
		//¹Ø±Õ
		BlackBoard::Get()->UpgradeEquip = 0;

		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpEquipPageName;
		MessageManager::Get()->sendMessage(&msg);
	}
}

void EquipEnhancePage::refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	if(name)
	{
		name->setString(Language::Get()->getString(userBasicInfo.name).c_str());
	}
	CCLabelBMFont* level = dynamic_cast<CCLabelBMFont* >(getVariable("mPlayerLv"));
	if(level)
	{
		level->setString(StringConverter::toString(userBasicInfo.level).c_str());
	}
	CCLabelBMFont* gold = dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	if(gold)
	{
		gold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	}

	Equip equip( BlackBoard::Get()->UpgradeEquip);
	CCLabelBMFont* equipValue = dynamic_cast<CCLabelBMFont* >(getVariable("mBuff"));
	if(equipValue)
	{
		equipValue->setString(StringConverter::toString(equip.quantity()).c_str());
	}
	CCLabelBMFont* equipIntroduce = dynamic_cast<CCLabelBMFont* >(getVariable("mNewBuff"));
	if(equipIntroduce)
	{
		equipIntroduce->setString(StringConverter::toString(int(equip.quantity() + equip.initGrowthRate())).c_str());
	}
	CCLabelBMFont* equipLV = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipLV"));
	if(equipLV)
	{
		equipLV->setString(StringConverter::toString(equip.level()).c_str());
	}
	CCLabelBMFont* newLevel = dynamic_cast<CCLabelBMFont* >(getVariable("mNewLevel"));
	if(newLevel)
	{
		newLevel->setString(StringConverter::toString(equip.level() + 1).c_str());
	}
	CCLabelBMFont* moneyNeeded = dynamic_cast<CCLabelBMFont* >(getVariable("mMoneyNeeded"));
	if(moneyNeeded)
	{
		moneyNeeded->setString(StringConverter::toString(equip.getConsumeMoneyNeeded()).c_str());
	}
	CCSprite* equipBuffType=dynamic_cast<CCSprite* >(getVariable("mEquipBuffType"));
	if(equipBuffType)
	{
		equipBuffType->initWithFile(equip.getEquipAtkTypePic().c_str());
	}

	
	if(equip.allowUpgrade()){
		CCB_FUNC(this,"mFunction2",CCMenuItemImage,setEnabled(true));
		CCB_FUNC(this,"mQuickEnhance",CCMenuItemImage,setEnabled(true));
	}else{
		CCB_FUNC(this,"mFunction2",CCMenuItemImage,setEnabled(false));
		CCB_FUNC(this,"mQuickEnhance",CCMenuItemImage,setEnabled(false));
	}


	CCB_FUNC(this,"mQuickEnhance",CCMenuItemImage,setVisible(true));
	CCB_FUNC(this,"mQuickEnhanceTex",CCLabelBMFont,setVisible(true));


}

void EquipEnhancePage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(BlackBoard::Get()->UpgradeEquip);
		mCard->refreshContent();
	}
	else
	{
		mCard = new EquipCardPage(BlackBoard::Get()->UpgradeEquip);
		mCard->init();
		mEquipCard->removeAllChildren();
		mEquipCard->addChild(mCard->getContainerNode());
	}


}