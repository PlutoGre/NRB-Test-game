
#include "stdafx.h"

#include "SkillUpgradeResultPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"

REGISTER_PAGE("SkillUpgradeResultPage",SkillUpgradeResultPage);
USING_NS_CC;

SkillUpgradeResultPage::SkillUpgradeResultPage(void)
{

}

SkillUpgradeResultPage::~SkillUpgradeResultPage(void)
{
}

void SkillUpgradeResultPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void SkillUpgradeResultPage::Exit( MainFrame* frame)
{

}

void SkillUpgradeResultPage::Execute( MainFrame* )
{

}

void SkillUpgradeResultPage::load( void )
{
	loadCcbiFile("SkillUpgradeResult.ccbi");	
}

void SkillUpgradeResultPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onUpgrade")
	{
		Skill skill(BlackBoard::Get()->UpgradeSkill);
		BlackBoard::Get()->resetSkillConsume();
		BlackBoard::Get()->SkillUpgradeBeforeLV = 0;
		BlackBoard::Get()->SkillUpgradeBeforeQuantity = "";

		MsgMainFrameChangePage msg;
		msg.pageName = "SkillUpgradePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onQuit")
	{		
		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpSkillPageName;
		MessageManager::Get()->sendMessage(&msg);

		BlackBoard::Get()->UpgradeSkill = 0;
		BlackBoard::Get()->resetSkillConsume();
		BlackBoard::Get()->SkillUpgradeBeforeLV = 0;
		BlackBoard::Get()->SkillUpgradeBeforeQuantity = "";
	}
}

void SkillUpgradeResultPage::refreshPage( void )
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

	unsigned int SkillUpgradeReceiver = BlackBoard::Get()->UpgradeSkill;
	Skill skill(SkillUpgradeReceiver);

	CCSprite* prop=dynamic_cast<CCSprite* >(getVariable("mSkillPic"));
	if(prop)
	{
		prop->initWithFile(skill.iconPic().c_str());
	}
	CCSprite* frame=dynamic_cast<CCSprite* >(getVariable("mQualityFrame"));
	if(frame)
	{
		frame->setTexture(skill.getFrameNormalSpirte()->getTexture());
		frame->setColor(skill.getFrameColor());
	}
// 	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(getVariable("mQualityFrame"));
// 	if(frame)
// 	{
// 		frame->setNormalImage(skill.getFrameNormalSpirte());
// 		frame->setSelectedImage(skill.getFrameSelectedSpirte());
// 	}
	CCLabelBMFont* skillName = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillName"));
	if(skillName)
	{
		skillName->setString(skill.name().c_str());
	}
	CCLabelBMFont* skillLevel=dynamic_cast<CCLabelBMFont* >(getVariable("mSkillLevel"));
	if(skillLevel)
	{
		skillLevel->setString(StringConverter::toString(skill.level()).c_str());
	}
	CCSprite* skillQuality=dynamic_cast<CCSprite* >(getVariable("mSkillQuality"));
	if(skillQuality)
	{
		skillQuality->initWithFile(skill.getQualityImageFile().c_str());
	}
	CCLabelBMFont* attackValue = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillBuffValue"));
	if(attackValue)
	{
		attackValue->setString(skill.quantityStr().c_str());
	}
	CCSprite* skillBuffType=dynamic_cast<CCSprite* >(getVariable("mSkillBuffType"));
	if(skillBuffType)
	{
		skillBuffType->initWithFile(skill.getSkillAtkTypePic().c_str());
	}

	CCLabelBMFont* successLabel = dynamic_cast<CCLabelBMFont* >(getVariable("mSuccessLabel"));
	CCLabelBMFont* failLabel = dynamic_cast<CCLabelBMFont* >(getVariable("mFailLabel"));
	if(skill.level() == BlackBoard::Get()->SkillUpgradeBeforeLV)
	{
		if( successLabel && failLabel)
		{
			successLabel->setVisible(false);
			failLabel->setVisible(true);
		}	
	}
	else
	{
		if( successLabel && failLabel)
		{
			successLabel->setVisible(true);
			failLabel->setVisible(false);
		}
	}
	CCLabelBMFont* rateLabel = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillLV"));
	if(rateLabel)
	{
		rateLabel->setString(StringConverter::toString(BlackBoard::Get()->SkillUpgradeBeforeLV).c_str());
	}
	CCLabelBMFont* skillNewLV = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillNewLV"));
	if(skillNewLV)
	{
		skillNewLV->setString(StringConverter::toString(skill.level()).c_str());
	}
	CCLabelBMFont* newBuffValue = dynamic_cast<CCLabelBMFont* >(getVariable("mNewBuffValue"));
	if(newBuffValue)
	{
		newBuffValue->setString(skill.quantityStr().c_str());
	}
	CCLabelBMFont* buffValue = dynamic_cast<CCLabelBMFont* >(getVariable("mBuffValue"));
	if(buffValue)
	{
		buffValue->setString(BlackBoard::Get()->SkillUpgradeBeforeQuantity.c_str());
	}
	CCSprite* skillBuffType1=dynamic_cast<CCSprite* >(getVariable("mSkillBuffType1"));
	if(skillBuffType1)
	{
		skillBuffType1->initWithFile(skill.getSkillAtkTypePic().c_str());
	}
	CCSprite* skillBuffType2=dynamic_cast<CCSprite* >(getVariable("mSkillBuffType2"));
	if(skillBuffType2)
	{
		skillBuffType2->initWithFile(skill.getSkillAtkTypePic().c_str());
	}
}
