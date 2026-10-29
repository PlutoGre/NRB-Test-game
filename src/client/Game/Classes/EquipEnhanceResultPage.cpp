
#include "stdafx.h"

#include "EquipEnhanceResultPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipCardPage.h"
#include "CCBContainer.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("EquipEnhanceResultPage",EquipEnhanceResultPage);
USING_NS_CC;

EquipEnhanceResultPage::EquipEnhanceResultPage(void)
{
	mCard = NULL;
}

EquipEnhanceResultPage::~EquipEnhanceResultPage(void)
{
}

void EquipEnhanceResultPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	refreshPage();
	buildCard();
}

void EquipEnhanceResultPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	delete mCard;
	mCard = NULL;
}

void EquipEnhanceResultPage::Execute( MainFrame* )
{

}

void EquipEnhanceResultPage::load( void )
{
	loadCcbiFile("EquipEnhanceResult.ccbi");
	mEquipCard = dynamic_cast<CCNode*>(getVariable("mEquipCard"));
}

void EquipEnhanceResultPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	if(itemName=="onEnhanceAgain")
	{
		Equip equip(BlackBoard::Get()->UpgradeEquip);
		BlackBoard::Get()->EquipBeforeLevel = equip.level();
		BlackBoard::Get()->EquipBeforeQuantity = equip.quantity();
		SET_V("EquipEnhanceResultPage", true);

		OPUpgradeEquip upgradeEquip;
		upgradeEquip.set_id(equip.id());
		PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_EQUIP_C,&upgradeEquip);
	}
	else if(itemName=="onQuickEnhanceAgain"){

		unsigned int vipLimit = StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickEnhanceVIPLimit"));
		if(userBasicInfo.viplevel >= vipLimit){
			Equip equip(BlackBoard::Get()->UpgradeEquip);
			BlackBoard::Get()->EquipBeforeLevel = equip.level();
			BlackBoard::Get()->EquipBeforeQuantity = equip.quantity();
			BlackBoard::Get()->EquipBeforeSilverCoins = userBasicInfo.silvercoins;
			SET_V("EquipEnhanceResultPage", true);

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
		BlackBoard::Get()->EquipBeforeLevel = 0;
		BlackBoard::Get()->EquipBeforeQuantity = 0;

		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpEquipPageName;
		MessageManager::Get()->sendMessage(&msg);
	}
}

void EquipEnhanceResultPage::refreshPage( void )
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
	CCLabelBMFont* oldBuff = dynamic_cast<CCLabelBMFont* >(getVariable("mOldBuff"));
	if(oldBuff)
	{
		oldBuff->setString(StringConverter::toString(BlackBoard::Get()->EquipBeforeQuantity).c_str());
	}
	CCLabelBMFont* newBuff = dynamic_cast<CCLabelBMFont* >(getVariable("mNewBuff"));
	if(newBuff)
	{
		newBuff->setString(StringConverter::toString(equip.quantity()).c_str());
	}
	CCLabelBMFont* oldLV = dynamic_cast<CCLabelBMFont* >(getVariable("mOldLV"));
	if(oldLV)
	{
		oldLV->setString(StringConverter::toString(BlackBoard::Get()->EquipBeforeLevel).c_str());
	}
	CCLabelBMFont* newLV = dynamic_cast<CCLabelBMFont* >(getVariable("mNewLV"));
	if(newLV)
	{
		newLV->setString(StringConverter::toString(equip.level()).c_str());
	}
	CCSprite* equipBuffType=dynamic_cast<CCSprite* >(getVariable("mEquipBuffType"));
	if(equipBuffType)
	{
		equipBuffType->initWithFile(equip.getEquipAtkTypePic().c_str());
	}
	
	CCLabelBMFont* nowBuff1Again = dynamic_cast<CCLabelBMFont* >(getVariable("mNowBuff1Again"));
	if(nowBuff1Again)
	{
		nowBuff1Again->setString(StringConverter::toString(equip.quantity()).c_str());
	}
	CCLabelBMFont* newBuffAgain = dynamic_cast<CCLabelBMFont* >(getVariable("mNewBuffAgain"));
	if(newBuffAgain)
	{
		newBuffAgain->setString(StringConverter::toString(int(equip.quantity() + equip.initGrowthRate())).c_str());
	}
	CCLabelBMFont* nowLV1Again = dynamic_cast<CCLabelBMFont* >(getVariable("mNowLV1Again"));
	if(nowLV1Again)
	{
		nowLV1Again->setString(StringConverter::toString(equip.level()).c_str());
	}
	CCLabelBMFont* newLVAgain = dynamic_cast<CCLabelBMFont* >(getVariable("mNewLVAgain"));
	if(newLVAgain)
	{
		newLVAgain->setString(StringConverter::toString(equip.level() + 1).c_str());
	}
	CCLabelBMFont* moneyNeeded = dynamic_cast<CCLabelBMFont* >(getVariable("mMoneyNeeded"));
	if(moneyNeeded)
	{
		moneyNeeded->setString(StringConverter::toString(equip.getConsumeMoneyNeeded()).c_str());
	}
	CCSprite* equipBuffType1=dynamic_cast<CCSprite* >(getVariable("mEquipBuffType1"));
	if(equipBuffType1)
	{
		equipBuffType1->initWithFile(equip.getEquipAtkTypePic().c_str());
	}

	CCMenuItemImage* enhanceAgain=dynamic_cast<CCMenuItemImage* >(getVariable("mEnhanceAgain"));
	if(enhanceAgain)
	{
		if(equip.allowUpgrade())
			enhanceAgain->setEnabled(true);
		else
			enhanceAgain->setEnabled(false);
	}

	CCMenuItemImage* quickEnhanceAgain=dynamic_cast<CCMenuItemImage* >(getVariable("mQuickEnhanceAgain"));
	if(quickEnhanceAgain)
	{
		if(equip.allowUpgrade())
			quickEnhanceAgain->setEnabled(true);
		else
			quickEnhanceAgain->setEnabled(false);
	}


	CCB_FUNC(this,"mQuickEnhanceAgain",CCMenuItemImage,setVisible(true));
	CCB_FUNC(this,"mQuickEnhanAgain",CCLabelBMFont,setVisible(true));

}

void EquipEnhanceResultPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "EquipEnhanceResultPage")
		{
			refreshPage();
			buildCard();
		}
	}
}


void EquipEnhanceResultPage::buildCard( void )
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