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
class SkillBookSurprisePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	SkillBookSurprisePage(void);
	~SkillBookSurprisePage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(SkillBookSurprisePage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void setInfo(int _skillId, int _partId, int _count = 1){mSkillId = _skillId;mPartId = _partId;mCount = _count;}
private:
	int mPartId;
	int mSkillId;
	int mCount;
	const SkillBookTable::SkillBookItem* mSkillBookItem;
	const SkillItem* mSkillItem;
	SkillBookInfo::SkillBookItem* mSkillBookData;
	CCLabelBMFont* mTitle;
	CCLabelBMFont* mSummary;
	CCLabelBMFont* mName;
	CCLabelBMFont* mNumber1;
	CCLabelBMFont* mDescription;
	CCLabelBMFont* mPopMsg;
	CCLabelBMFont* mNote1;
	CCLabelBMFont* mFunctionLabel1;
	CCLabelBMFont* mFunctionLabel2;
	CCMenuItemImage* mFrame;
	CCSprite* mPic;
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
};