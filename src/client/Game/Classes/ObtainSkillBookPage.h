#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "CharacterConsts.h"
#include "GameMessages.h"
#include "GameMaths.h"
#include "GamePackets.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "StringConverter.h"
class ObtainSkillBookPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	ObtainSkillBookPage(void);
	~ObtainSkillBookPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(ObtainSkillBookPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);

private:
	const SkillBookTable::SkillBookItem* mSkillBookItem;
	const SkillItem* mSkillItem;
	SkillBookInfo::SkillBookItem* mSkillBookData;
	CCLabelBMFont* mTitle;
	CCLabelBMFont* mSummary;
	CCLabelBMFont* mName;
	CCLabelBMFont* mNumber1;
	CCLabelBMFont* mNumber;
	CCLabelBMFont* mDescription;
	CCLabelBMFont* mPopMsg;
	CCLabelBMFont* mNote1;
	CCLabelBMFont* mFunctionLabel1;
	CCLabelBMFont* mFunctionLabel2;
	CCMenuItemImage* mFrame;
	CCSprite* mPic;
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
};