
#include "stdafx.h"

#include "SoulInfoPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "Skill.h"
#include "DiscipleCardPage.h"

REGISTER_PAGE("SoulInfoPage",SoulInfoPage);
USING_NS_CC;

SoulInfoPage::SoulInfoPage(void)
{
	mCard = NULL;
}

SoulInfoPage::~SoulInfoPage(void)
{
}

void SoulInfoPage::load( void )
{
	loadCcbiFile("SoulInfo.ccbi");
	mDiscipleCard = dynamic_cast<CCNode*>(getVariable("mMemberCard"));
	mContent = dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	mContent->setContentOffset(mContent->minContainerOffset());
	refreshPage();
	buildCard();
}

void SoulInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction2" || itemName=="onXClose"||itemName=="onClose")
	{
		BlackBoard::Get()->ShowSoul = 0;
		MsgMainFramePopPage msg;
		msg.pageName = "SoulInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowSkillInfo")
	{
		//BlackBoard::Get()->ShowSkillItem = Soul(BlackBoard::Get()->ShowSoul).bornSkillItemID();
		BlackBoard::Get()->ShowSkill = Soul(BlackBoard::Get()->ShowSoul).bornSkillItemID();
		BlackBoard::Get()->NeedChangeSkill= 0;
		BlackBoard::Get()->NeedChooseSkill = 0;
		BlackBoard::Get()->JustViewSkill = true;
		MsgMainFramePushPage msg;
		msg.pageName = "SkillInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void SoulInfoPage::refreshPage( void )
{
	Soul soul(BlackBoard::Get()->ShowSoul);

	Skill skill(soul.bornSkillItemID(),true);
	CCSprite* skillPic=dynamic_cast<CCSprite* >(getVariable("mSkillPic"));
	if(skillPic)
	{
		skillPic->initWithFile(skill.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(getVariable("mSkillFace"));
	if(frame)
	{
		frame->setNormalImage(skill.getFrameNormalSpirte());
		frame->setSelectedImage(skill.getFrameSelectedSpirte());
	}
	CCLabelBMFont* skillName=dynamic_cast<CCLabelBMFont* >(getVariable("mSkillName"));
	if(skillName)
	{
		skillName->setString(Language::Get()->getString(skill.name()).c_str());
	}

	CCLabelBMFont* memberValue=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberValue"));
	if(memberValue)
	{
		memberValue->setString(StringConverter::toString(soul.getSoulValue()).c_str());
	}
	CCLabelBMFont* memberInfo=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberInfo"));
	if(memberInfo)
	{
		memberInfo->setString(Language::Get()->getString(soul.describe()).c_str());
	}
	CCLabelBMFont* buffInfo=dynamic_cast<CCLabelBMFont* >(getVariable("mBuffInfo"));
	if(buffInfo)
	{
		buffInfo->setString(soul.getYuanfenDescribe().c_str());
	}
	CCLabelBMFont* blood=dynamic_cast<CCLabelBMFont* >(getVariable("mBlood"));
	if(blood)
	{
		blood->setString(StringConverter::toString(soul.initHealth()).c_str());
	}
	CCLabelBMFont* attack=dynamic_cast<CCLabelBMFont* >(getVariable("mAttack"));
	if(attack)
	{
		attack->setString(StringConverter::toString(soul.initAttack()).c_str());
	}
	CCLabelBMFont* defense=dynamic_cast<CCLabelBMFont* >(getVariable("mDefense"));
	if(defense)
	{
		defense->setString(StringConverter::toString(soul.initDefence()).c_str());
	}
	CCLabelBMFont* will=dynamic_cast<CCLabelBMFont* >(getVariable("mWill"));
	if(will)
	{
		will->setString(StringConverter::toString(soul.initPower()).c_str());
	}
}

void SoulInfoPage::buildCard( void )
{
	Soul soul(BlackBoard::Get()->ShowSoul);
	if(mCard)
	{
		mCard->setContentID(soul.itemID());
		mCard->setItemID(true);
		mCard->setIsSoul(true);
		mCard->refreshContent();
	}
	else
	{
		mCard = new DiscipleCardPage(soul.itemID());
		mCard->setItemID(true);
		mCard->setIsSoul(true);
		mCard->init();
		mDiscipleCard->removeAllChildren();
		mDiscipleCard->addChild(mCard->getContainerNode());
		mContent->forceRecaculateChildren();
	}
}

void SoulInfoPage::unload( void )
{
	mDiscipleCard = NULL;
	mContent = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}