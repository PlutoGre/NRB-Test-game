
#include "stdafx.h"

#include "EquipInfoPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipHandInfoPage.h"
#include "MessageBoxPage.h"
#include "GameMaths.h"

REGISTER_PAGE("EquipHandInfoPage",EquipHandInfoPage);
USING_NS_CC;


EquipHandInfoPage::EquipHandInfoPage(void)
{
	mCard = NULL;
}

EquipHandInfoPage::~EquipHandInfoPage(void)
{
}

void EquipHandInfoPage::load( void )
{
	loadCcbiFile("EquipmentInfo.ccbi");
	mEquipCard = dynamic_cast<CCNode*>(getVariable("mEquipCard"));
	refreshPage();
	buildCard();
}

void EquipHandInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "EquipHandInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onFunction2")
	{
		MSG_BOX(Language::Get()->getString("@MAINPAGE_COMINGSOON"));
	}
	else if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "EquipHandInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowEnhanceFail")
	{

	}
}

void EquipHandInfoPage::refreshPage( void )
{
	Equip equip(mId,mIsItem);
	
	CCLabelBMFont* buttonName1=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName1"));
	if(buttonName1)
	{
		buttonName1->setString(Language::Get()->getString(::CloseButton_String).c_str());
	}
	CCLabelBMFont* buttonName2=dynamic_cast<CCLabelBMFont* >(getVariable("mButtonName2"));
	if(buttonName2)
	{
		CCMenuItemImage* function2=dynamic_cast<CCMenuItemImage* >(getVariable("mFunction2"));
		function2->setEnabled(true);
		buttonName2->setString(Language::Get()->getString("@Share").c_str());
	}
	CCLabelBMFont* equipValue = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipValue"));
	if(equipValue)
	{
		equipValue->setString(StringConverter::toString(equip.getEquipValue()).c_str());
	}
	CCLabelBMFont* equipIntroduce = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipIntro"));
	if(equipIntroduce)
	{
		equipIntroduce->setString(Language::Get()->getString(equip.describe()).c_str());
	}

}

void EquipHandInfoPage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(mId);
		mCard->setCardItem(mIsItem);
		mCard->refreshContent();
	}
	else
	{
		mCard = new EquipCardPage(mId,mIsItem);
		mCard->init();
		mEquipCard->removeAllChildren();
		mEquipCard->addChild(mCard->getContainerNode());
	}
}

void EquipHandInfoPage::showPage(unsigned int id,bool isItem)
{
	MsgMainFramePushPage msg;
	msg.pageName="EquipHandInfoPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("EquipHandInfoPage");
	EquipHandInfoPage* page = dynamic_cast<EquipHandInfoPage*>(obj);
	if(page)
	{
		page->mId=id;
		page->mIsItem=isItem;
	}
}

void EquipHandInfoPage::unload( void )
{
	mEquipCard = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}


void EquipHandInfoPage::showEquipPage(unsigned int id,bool isItem)
{
	dynamic_cast<EquipHandInfoPage*>(CCBManager::Get()->getPage("EquipHandInfoPage"))->showPage(id,isItem);
}