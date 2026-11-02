
#include "stdafx.h"

#include "SkillViewTeamInfoPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "SkillCardPage.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("SkillViewTeamInfoPage",SkillViewTeamInfoPage);
USING_NS_CC;

SkillViewTeamInfoPage::SkillViewTeamInfoPage(void)
{
	mCard = NULL;
}

SkillViewTeamInfoPage::~SkillViewTeamInfoPage(void)
{
}

void SkillViewTeamInfoPage::load( void )
{
	loadCcbiFile("SkillInfo.ccbi");
	mSkillCard = dynamic_cast<CCNode*>(getVariable("mSkillCard"));
	refreshPage();
	buildCard();
}

void SkillViewTeamInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "SkillViewTeamInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onFunction2")
	{
		MSG_BOX(Language::Get()->getString("@MAINPAGE_COMINGSOON"));
	}
	else if(itemName=="onClose")
	{
		//¹Ø±Õ
		MsgMainFramePopPage msg;
		msg.pageName = "SkillViewTeamInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

void SkillViewTeamInfoPage::refreshPage( void )
{
	unsigned int ShowSkill = mId;
	Skill skill(ShowSkill,false,true);
	
	CCLabelBMFont* buttonName1=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName1"));
	if(buttonName1)
	{
		buttonName1->setString(Language::Get()->getString(::CloseButton_String).c_str());
	}
	CCLabelBMFont* buttonName2=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName2"));
	if(buttonName2)
	{
		buttonName2->setString(Language::Get()->getString("@Share").c_str());
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
	CCMenuItemImage* func2 = dynamic_cast<CCMenuItemImage* >(getVariable("mFunc2"));
	if(func2)
	{
		func2->setEnabled(true);
	}
}

void SkillViewTeamInfoPage::buildCard( void )
{
	int skillId = mId;
	if(mCard)
	{
		mCard->setViewTeam(true);
		mCard->setContentID(skillId);
		mCard->refreshContent();
	}
	else
	{
		mCard = new SkillCardPage(skillId);
		mCard->setViewTeam(true);
		mCard->init();
		mSkillCard->removeAllChildren();
		mSkillCard->addChild(mCard->getContainerNode());
	}


}

void SkillViewTeamInfoPage::unload( void )
{
	mSkillCard = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}

void SkillViewTeamInfoPage::showPage(unsigned int id)
{
	MsgMainFramePushPage msg;
	msg.pageName="SkillViewTeamInfoPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("SkillViewTeamInfoPage");
	SkillViewTeamInfoPage* page = dynamic_cast<SkillViewTeamInfoPage*>(obj);
	if(page)
	{
		page->mId=id;
	}
}

void SkillViewTeamInfoPage::showSkillPage(unsigned int id)
{
	dynamic_cast<SkillViewTeamInfoPage*>(CCBManager::Get()->getPage("SkillHandInfoPage"))->showPage(id);
}