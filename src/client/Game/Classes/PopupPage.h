#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "GamePackets.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "GameMaths.h"
#include "CareerData.h"
#include "CareerConfigManager.h"
enum Popup_Page_Type
{
	SnatchSkillBookFightWin,
	SnatchSkillBookFightFail,
	CareerAttackNumLimit,
	NotEnoughGold,
	ChangeDiscipleConfirm,
	CareerAttackNumVipTip,
	Script,
};
class PopupPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	PopupPage(void);
	~PopupPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(PopupPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void showPage(Popup_Page_Type _type);
	virtual void onReceiveMassage(const GameMessage * message);
	Popup_Page_Type mType;

	void showPage(Popup_Page_Type _type, std::string msgKey, std::string titleKey, std::string function1Key, std::string function2Key, int fun1handle);

	static void Pop_Box(Popup_Page_Type _type);
	static void Pop_Box(std::string msgKey, std::string titleKey, std::string function1Key, std::string function2Key, int fun1handle);
private:
	void _refreshPage();
	void initPage();
	CCLabelBMFont* mMsg;
	CCLabelBMFont* mTitle;
	CCLabelBMFont* mFunction1;
	CCLabelBMFont* mFunction2;
	std::string mMsgKey;
	std::string mTitleKey;
	std::string mFunction1Key;
	std::string mFunction2Key;

	int mFunScriptHandle;
};
#define  POP_BOX(mType) dynamic_cast<PopupPage*>(CCBManager::Get()->getPage("PopupPage"))->showPage(mType);