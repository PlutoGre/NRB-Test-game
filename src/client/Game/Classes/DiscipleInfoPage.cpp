
#include "stdafx.h"

#include "DiscipleInfoPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "Skill.h"
#include "DiscipleCardPage.h"
#include "MessageBoxPage.h"
REGISTER_PAGE("DiscipleInfoPage",DiscipleInfoPage);
USING_NS_CC;

DiscipleInfoPage::DiscipleInfoPage(void)
{
	mCard = NULL;
}

DiscipleInfoPage::~DiscipleInfoPage(void)
{
	delete mCard;
	mCard = NULL;
}

void DiscipleInfoPage::load( void )
{
	loadCcbiFile("MemberInfo.ccbi");
	mDiscipleCard = dynamic_cast<CCNode*>(getVariable("mMemberCard"));
	mContent = dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	mContent->setContentOffset(mContent->minContainerOffset());
	refreshPage();
	buildCard();
}

void DiscipleInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{
		if(BlackBoard::Get()->NeedChangeDisciple)
		{
			//更换弟子
			BlackBoard::Get()->NeedChangeDisciple = false;

			MsgMainFramePopPage msg;
			msg.pageName = "DiscipleInfoPage";
			MessageManager::Get()->sendMessage(&msg);

			MsgMainFrameChangePage msg1;
			msg1.pageName = "DiscipleChangePage";
			MessageManager::Get()->sendMessage(&msg1);
		}
		else
		{
			//关闭
			BlackBoard::Get()->NeedChangeDisciple = false;
			BlackBoard::Get()->NeedChooseDisciple = false;
			BlackBoard::Get()->ShowDisciple = 0;

			MsgMainFramePopPage msg;
			msg.pageName = "DiscipleInfoPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onFunction2")
	{
		if(BlackBoard::Get()->NeedChooseDisciple)
		{
			//选择
		}
// 		else if(BlackBoard::Get()->NeedRecruitDisciple)
// 		{
// 			//招募
// 		}
// 		else if(BlackBoard::Get()->NeedUpgradeDisciple)
// 		{
// 			//BlackBoard::Get()->UpgradeDiscipleItem = mID;
// 
// 			MsgMainFramePopPage msg;
// 			msg.pageName = "DiscipleInfoPage";
// 			MessageManager::Get()->sendMessage(&msg);
// 
// 			MsgMainFrameChangePage msg1;
// 			msg1.pageName = "DiscipleUpgradePage";
// 			MessageManager::Get()->sendMessage(&msg1);
// 		}
		else
		{
			//传功
			BlackBoard::Get()->TransferExp_Receiver = BlackBoard::Get()->ShowDisciple;
			BlackBoard::Get()->NeedChangeDisciple = 0;
			BlackBoard::Get()->NeedChooseDisciple = false;
			BlackBoard::Get()->ShowDisciple = 0;

			MsgMainFramePopPage msg;
			msg.pageName = "DiscipleInfoPage";
			MessageManager::Get()->sendMessage(&msg);

			MsgMainFrameChangePage msg1;
			msg1.pageName = "TransferEXPPage";
			MessageManager::Get()->sendMessage(&msg1);
		}
	}
	else if(itemName=="onClose")
	{
		BlackBoard::Get()->NeedChangeDisciple = 0;
		BlackBoard::Get()->NeedChooseDisciple = false;
		BlackBoard::Get()->ShowDisciple = 0;

		MsgMainFramePopPage msg;
		msg.pageName = "DiscipleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowSkillInfo")
	{
		BlackBoard::Get()->jumpSkillPageName = "DisciplePage";
		BlackBoard::Get()->NeedChangeSkill= 0;
		BlackBoard::Get()->NeedChooseSkill = 0;
		BlackBoard::Get()->ShowSkill = Disciple(BlackBoard::Get()->ShowDisciple).skill1();
		MsgMainFramePopPage msgInfo;
		msgInfo.pageName = "DiscipleInfoPage";
		MessageManager::Get()->sendMessage(&msgInfo);
		MsgMainFramePushPage msg;
		msg.pageName = "SkillInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void DiscipleInfoPage::refreshPage( void )
{
	Disciple disciple(BlackBoard::Get()->ShowDisciple);

	if(!disciple.skill1() == 0)
	{
		Skill skill(disciple.skill1());
		if(!skill.isHavItem())
		{
			char errMsg[256];
			sprintf(errMsg,"id %d: skillItem not exist!",disciple.skill1());
			MSG_BOX(errMsg);
			return;
		}
		if(!skill.isHavInfo())
		{
			char errMsg[256];
			sprintf(errMsg,"id %d: skillInfo not exist!",disciple.skill1());
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
		if(BlackBoard::Get()->NeedChangeDisciple)
		{
			buttonName1->setString(Language::Get()->getString(::ChangeDisciple_String).c_str());
		}
		else
		{
			buttonName1->setString(Language::Get()->getString(::CloseButton_String).c_str());
		}
	}
	CCLabelBMFont* buttonName2=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName2"));
	if(buttonName2)
	{
		if(BlackBoard::Get()->NeedChooseDisciple)
		{
			buttonName2->setString(Language::Get()->getString(::ChooseDisciple_String).c_str());
		}
		else
		{
			buttonName2->setString(Language::Get()->getString(::TransferExp_String).c_str());
		}
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

void DiscipleInfoPage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(BlackBoard::Get()->ShowDisciple);
		mCard->refreshContent();
	}
	else
	{
		mCard = new DiscipleCardPage(BlackBoard::Get()->ShowDisciple);

		mCard->init();
		mDiscipleCard->removeAllChildren();
		mDiscipleCard->addChild(mCard->getContainerNode());
		mContent->forceRecaculateChildren();
	}
	mCard->getContainerNode()->setPosition(0.0f,0.0f);
}

void DiscipleInfoPage::unload( void )
{
	mDiscipleCard = NULL;
	mContent = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}
