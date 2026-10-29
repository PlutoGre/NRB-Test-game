
#include "stdafx.h"

#include "DiscipleInfoPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "Skill.h"
#include "DiscipleHandInfoPage.h"
#include "MessageBoxPage.h"
#include "DiscipleCardPage.h"
REGISTER_PAGE("DiscipleHandInfoPage",DiscipleHandInfoPage);
USING_NS_CC;

DiscipleHandInfoPage::DiscipleHandInfoPage(void)
{
	mCard = NULL;
}

DiscipleHandInfoPage::~DiscipleHandInfoPage(void)
{
	delete mCard;
	mCard = NULL;
}

void DiscipleHandInfoPage::load( void )
{
	loadCcbiFile("MemberInfo.ccbi");
	mDiscipleCard = dynamic_cast<CCNode*>(getVariable("mMemberCard"));
	mContent = dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	mContent->setContentOffset(mContent->minContainerOffset());
	refreshPage();
	buildCard();
}

void DiscipleHandInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "DiscipleHandInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onFunction2")
	{
		MSG_BOX(Language::Get()->getString("@MAINPAGE_COMINGSOON"));
	}
	else if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "DiscipleHandInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowSkillInfo")
	{
	}
}

void DiscipleHandInfoPage::refreshPage( void )
{
	Disciple disciple(mId,mIsItem);

	if(!disciple.skill1() == 0)
	{
		Skill skill(disciple.skill1(),mIsItem);
		if(!skill.isHavItem())
		{
			char errMsg[256];
			sprintf(errMsg,"id %d: skillItem not exist!",disciple.skill1());
			MSG_BOX(errMsg);
			return;
		}
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
	}

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

	CCLabelBMFont* memberValue=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberValue"));
	if(memberValue)
	{
		memberValue->setString(StringConverter::toString(disciple.getDiscipleValue()).c_str());
	}
	CCLabelBMFont* memberInfo=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberInfo"));
	if(memberInfo)
	{
		memberInfo->setString(Language::Get()->getString(disciple.describe()).c_str());
	}
	CCLabelBMFont* blood=dynamic_cast<CCLabelBMFont* >(getVariable("mBlood"));
	if(blood)
	{
		blood->setString(StringConverter::toString(disciple.health()).c_str());
	}
	CCLabelBMFont* attack=dynamic_cast<CCLabelBMFont* >(getVariable("mAttack"));
	if(attack)
	{
		attack->setString(StringConverter::toString(disciple.attack()).c_str());
	}
	CCLabelBMFont* defense=dynamic_cast<CCLabelBMFont* >(getVariable("mDefense"));
	if(defense)
	{
		defense->setString(StringConverter::toString(disciple.defence()).c_str());
	}
	CCLabelBMFont* will=dynamic_cast<CCLabelBMFont* >(getVariable("mWill"));
	if(will)
	{
		will->setString(StringConverter::toString(disciple.rp()).c_str());
	}
	CCLabelBMFont* buffInfo=dynamic_cast<CCLabelBMFont* >(getVariable("mBuffInfo"));
	if(buffInfo)
	{
		buffInfo->setString(disciple.getYuanfenDescribe().c_str());
	}
	CCMenuItemImage* button2 = dynamic_cast<CCMenuItemImage* >(getVariable("mButton2"));
	if(button2)
	{
		if(!disciple.canUpgradeLevel())
		{
			button2->setEnabled(false);
		}
		else
		{
			button2->setEnabled(true);
		}
	}
	
}

void DiscipleHandInfoPage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(mId);
		mCard->setItemID(mIsItem);
		mCard->refreshContent();
	}
	else
	{
		mCard = new DiscipleCardPage(mId,mIsItem);
		mCard->init();
		mDiscipleCard->removeAllChildren();
		mDiscipleCard->addChild(mCard->getContainerNode());
		mContent->forceRecaculateChildren();
	}
	mCard->getContainerNode()->setPosition(0.0f,0.0f);
}

void DiscipleHandInfoPage::showPage(unsigned int id,bool isItem)
{
	MsgMainFramePushPage msg;
	msg.pageName="DiscipleHandInfoPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("DiscipleHandInfoPage");
	DiscipleHandInfoPage* page = dynamic_cast<DiscipleHandInfoPage*>(obj);
	if(page)
	{
		page->mId=id;
		page->mIsItem=isItem;
	}
}

void DiscipleHandInfoPage::unload( void )
{
	mDiscipleCard = NULL;
	mContent = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}

void DiscipleHandInfoPage::showDisciplePage(unsigned int id,bool isItem)
{
	dynamic_cast<DiscipleHandInfoPage*>(CCBManager::Get()->getPage("DiscipleHandInfoPage"))->showPage(id,isItem);
}