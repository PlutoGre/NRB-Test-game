
#include "stdafx.h"

#include "SkillInfoPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "SkillCardPage.h"

REGISTER_PAGE("SkillInfoPage",SkillInfoPage);
USING_NS_CC;

SkillInfoPage::SkillInfoPage(void)
{
	mCard = NULL;
}

SkillInfoPage::~SkillInfoPage(void)
{
}

void SkillInfoPage::load( void )
{
	loadCcbiFile("SkillInfo.ccbi");
	mSkillCard = dynamic_cast<CCNode*>(getVariable("mSkillCard"));
	refreshPage();
	buildCard();
}

void SkillInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{
		if(BlackBoard::Get()->NeedChangeSkill)
		{
			//更换技能
			BlackBoard::Get()->NeedChangeSkill= 0;

			MsgMainFramePopPage msg;
			msg.pageName = "SkillInfoPage";
			MessageManager::Get()->sendMessage(&msg);

			MsgMainFrameChangePage msg1;
			msg1.pageName = "SkillChangePage";
			MessageManager::Get()->sendMessage(&msg1);
			
		}
		else
		{
			//关闭
			BlackBoard::Get()->NeedChangeSkill= 0;
			BlackBoard::Get()->NeedChooseSkill = 0;
			BlackBoard::Get()->JustViewSkill = false;
			BlackBoard::Get()->ShowSkill = 0;

			MsgMainFramePopPage msg;
			msg.pageName = "SkillInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onFunction2")
	{
		if(BlackBoard::Get()->NeedChooseSkill)
		{
			//选择
		}
		else
		{
			//参悟
			BlackBoard::Get()->UpgradeSkill = BlackBoard::Get()->ShowSkill;
// 			BlackBoard::Get()->NeedChangeSkill= 0;
// 			BlackBoard::Get()->NeedChooseSkill = 0;
			BlackBoard::Get()->ShowSkill = 0;
			
			MsgMainFramePopPage msg;
			msg.pageName = "SkillInfoPage";
			MessageManager::Get()->sendMessage(&msg);

			MsgMainFrameChangePage msg1;
			msg1.pageName = "SkillUpgradePage";
			msg1.needPopAllPage = true;
			MessageManager::Get()->sendMessage(&msg1);
		}
	}
	else if(itemName=="onClose")
	{
		//关闭
		BlackBoard::Get()->NeedChangeSkill= 0;
		BlackBoard::Get()->NeedChooseSkill = 0;
		BlackBoard::Get()->JustViewSkill = false;
		BlackBoard::Get()->ShowSkill = 0;
		MsgMainFramePopPage msg;
		msg.pageName = "SkillInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

void SkillInfoPage::refreshPage( void )
{
	unsigned int ShowSkill = BlackBoard::Get()->ShowSkill;
	Skill skill(ShowSkill);
	
	CCLabelBMFont* buttonName1=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName1"));
	if(buttonName1)
	{
		if(BlackBoard::Get()->NeedChangeSkill)
		{
			buttonName1->setString(Language::Get()->getString(::ChangeSkill_String).c_str());
		}
		else
		{
			buttonName1->setString(Language::Get()->getString(::CloseButton_String).c_str());
		}
	}
	CCLabelBMFont* buttonName2=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName2"));
	if(buttonName2)
	{
		if(BlackBoard::Get()->NeedChooseSkill)
		{
			buttonName2->setString(Language::Get()->getString(::ChooseSkill_String).c_str());
		}
		else
		{
			buttonName2->setString(Language::Get()->getString(::UpGradeSkill_String).c_str());
		}
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
		if(BlackBoard::Get()->JustViewSkill)
		{
			func2->setEnabled(false);
		}
		else
		{
			func2->setEnabled(true);
		}
	}
	
	

	
}

void SkillInfoPage::buildCard( void )
{
	int skillId = BlackBoard::Get()->ShowSkill;
	if(mCard)
	{
		mCard->setContentID(skillId);
		mCard->refreshContent();
	}
	else
	{
		mCard = new SkillCardPage(skillId);
		mCard->init();
		mSkillCard->removeAllChildren();
		mSkillCard->addChild(mCard->getContainerNode());
	}


}

void SkillInfoPage::unload( void )
{
	mSkillCard = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}