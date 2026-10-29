
#include "stdafx.h"

#include "PlayerUpgradePage.h"
#include "PopupManager.h"
#include "SoundManager.h"
REGISTER_PAGE("PlayerUpgradePage", PlayerUpgradePage);

USING_NS_CC;
PlayerUpgradePage::PlayerUpgradePage(void)
{
}


PlayerUpgradePage::~PlayerUpgradePage(void)
{
}

void PlayerUpgradePage::load( void )
{
	loadCcbiFile("PlayerUpgradePopupPage.ccbi");
}

void PlayerUpgradePage::Enter( MainFrame* )
{
	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("LevelUpMusic"));
	const UserBasicInfo userBasicInfo =  ServerDateManager::Get()->getUserBasicInfo();
	ServerDateManager::Get()->getNextLevelExp();
	unsigned int level = userBasicInfo.level;
	if(level<=0)
	{
		level=1;
	}

	const PlayerGradeItem* item = PlayerGradeTableManager::Get()->getGradeItemByID(level);
	if(item)
	{
		CCLabelBMFont* lb_Grade = dynamic_cast<CCLabelBMFont*>(getVariable("mUpgradeGrade"));
		if(lb_Grade)
		{
			lb_Grade->setString(StringConverter::toString(item->ID).c_str());
		}

		CCLabelBMFont* lb_Disciple = dynamic_cast<CCLabelBMFont*>(getVariable("mUpgradeDisciple"));
		if(lb_Disciple)
		{
			lb_Disciple->setString(StringConverter::toString(item->canDiscipleNum).c_str());
		}

		CCLabelBMFont* lb_UpgradePower = dynamic_cast<CCLabelBMFont*>(getVariable("mUpgradePower"));
		if(lb_UpgradePower)
		{
			lb_UpgradePower->setString(StringConverter::toString(item->maxPower).c_str());
		}

		CCLabelBMFont* lb_Vitality = dynamic_cast<CCLabelBMFont*>(getVariable("mUpgradeVitality"));
		if(lb_UpgradePower)
		{
			lb_Vitality->setString(StringConverter::toString(item->maxVitality).c_str());
		}

		CCLabelBMFont* lb_Gold = dynamic_cast<CCLabelBMFont*>(getVariable("mUpgradeGold"));
		if(lb_Gold)
		{
			lb_Gold->setString(StringConverter::toString(item->goldReward).c_str());
		}

		CCLabelBMFont* lb_Silver = dynamic_cast<CCLabelBMFont*>(getVariable("mUpgradeSilver"));
		if(lb_Silver)
		{
			lb_Silver->setString(StringConverter::toString(item->silverReward).c_str());
		}
	}
	
}

void PlayerUpgradePage::Execute( MainFrame* )
{
	
}

void PlayerUpgradePage::Exit( MainFrame* )
{
}

void PlayerUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
}

void PlayerUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	MsgMainFramePopPage msg;
	msg.pageName = "PlayerUpgradePage";
	MessageManager::Get()->sendMessage(&msg);
	PopupManager::Get()->showNewActiveGrade();
}