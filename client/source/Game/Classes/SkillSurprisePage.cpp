
#include "stdafx.h"

#include "SkillSurprisePage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "SkillCardPage.h"
#include "PopupManager.h"
REGISTER_PAGE("SkillSurprisePage",SkillSurprisePage);
USING_NS_CC;

SkillSurprisePage::SkillSurprisePage(void)
{
	mCard = NULL;
}

SkillSurprisePage::~SkillSurprisePage(void)
{
}

void SkillSurprisePage::load( void )
{
	loadCcbiFile("SkillInfo4.ccbi");
	mSkillCard = dynamic_cast<CCNode*>(getVariable("mSkillCard"));
	refreshPage();
	buildCard();
}

void SkillSurprisePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	//BlackBoard::Get()->ShowSkill = 0;
	SET_V("SkillSurprise", (unsigned int)0);
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
		msg.pageName = "SkillSurprisePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	PopupManager::Get()->popPage();
}

void SkillSurprisePage::refreshPage( void )
{
	unsigned int ShowSkill = GET_V_UINT("SkillSurprise");
	Skill skill(ShowSkill);
	
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
	CCLabelBMFont* skillValue = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillValue"));
	if(skillValue)
	{
		skillValue->setString(StringConverter::toString(skill.getSkillValue()).c_str());
	}
	CCLabelBMFont* skillIntroduce = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillIntroduce"));
	if(skillIntroduce)
	{
		skillIntroduce->setString(Language::Get()->getString(skill.describe()).c_str());
	}
	CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@RewardSurpriseTitle").c_str()));
}

void SkillSurprisePage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(GET_V_UINT("SkillSurprise"));
		mCard->refreshContent();
	}
	else
	{
		mCard = new SkillCardPage(GET_V_UINT("SkillSurprise"));
		mCard->init();
		mSkillCard->removeAllChildren();
		mSkillCard->addChild(mCard->getContainerNode());
	}
}

void SkillSurprisePage::unload( void )
{
	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}
