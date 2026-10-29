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
#include "ContentBase.h"
#include "StringConverter.h"
#define THIRTY_MINUTES 30*60
class PlayerInfPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	PlayerInfPage(void);
	~PlayerInfPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(PlayerInfPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
private:
	void initPage();
	void refreshPage();
	void refreshTimeCount();
	cocos2d::CCLabelTTF *mName;
	cocos2d::CCSprite *mVIPLV;
	cocos2d::CCLabelBMFont *mLV;
	cocos2d::CCLabelBMFont *mWeiwang;
	cocos2d::CCLabelBMFont *mDizishang;
	cocos2d::CCLabelBMFont *mDiziyou;
	cocos2d::CCLabelBMFont *mGold;
	cocos2d::CCLabelBMFont *mCoin;
	cocos2d::CCLabelBMFont *mTi;
	cocos2d::CCLabelBMFont *mTihuifu;
	cocos2d::CCLabelBMFont *mTiquanhuifu;
	cocos2d::CCLabelBMFont *mQi;
	cocos2d::CCLabelBMFont *mQihuifu;
	cocos2d::CCLabelBMFont *mQiquanhuifu;
};