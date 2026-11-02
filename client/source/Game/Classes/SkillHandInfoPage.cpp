
#include "stdafx.h"

#include "SkillHandInfoPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "SkillCardPage.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("SkillHandInfoPage",SkillHandInfoPage);
USING_NS_CC;

SkillHandInfoPage::SkillHandInfoPage(void)
{
	mCard = NULL;
}

SkillHandInfoPage::~SkillHandInfoPage(void)
{
}

void SkillHandInfoPage::load( void )
{
	loadCcbiFile("SkillInfo.ccbi");
	mSkillCard = dynamic_cast<CCNode*>(getVariable("mSkillCard"));
	refreshPage();
	buildCard();
}

void SkillHandInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "SkillHandInfoPage";
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
		msg.pageName = "SkillHandInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

void SkillHandInfoPage::refreshPage( void )
{
	unsigned int ShowSkill = mId;
	Skill skill(ShowSkill,mIsItem);
	
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

void SkillHandInfoPage::buildCard( void )
{
	int skillId = mId;
	if(mCard)
	{
		mCard->setItemID(mIsItem);
		mCard->setContentID(skillId);
		mCard->refreshContent();
	}
	else
	{
		mCard = new SkillCardPage(skillId);
		mCard->setItemID(mIsItem);
		mCard->init();
		mSkillCard->removeAllChildren();
		mSkillCard->addChild(mCard->getContainerNode());
	}


}

void SkillHandInfoPage::unload( void )
{
	mSkillCard = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}

void SkillHandInfoPage::showPage(unsigned int id,bool isItem)
{
	MsgMainFramePushPage msg;
	msg.pageName="SkillHandInfoPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("SkillHandInfoPage");
	SkillHandInfoPage* page = dynamic_cast<SkillHandInfoPage*>(obj);
	if(page)
	{
		page->mId=id;
		page->mIsItem=isItem;
	}
}

void SkillHandInfoPage::showSkillPage(unsigned int id,bool isItem)
{
	dynamic_cast<SkillHandInfoPage*>(CCBManager::Get()->getPage("SkillHandInfoPage"))->showPage(id,isItem);
}