#pragma once


#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "GamePackets.h"
#include "GameMessages.h"
#include "StringConverter.h"
class SettingsPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
	, public CCBContainer::CCBContainerListener
{
public:
	SettingsPage(void);
	~SettingsPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(SettingsPage);
	CCBContainer* scrollViewContent;
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	void SettingItemButtonAndLabel(std::string type,bool scollViewChild=false, bool isOpen=false);
	void SettingItemButton(std::string type);
	virtual void onReceiveMassage(const GameMessage * message);
private:
	enum BUTTON_TAG
	{
		FACE_BUTTON_START = 0,
		FACE_BUTTON_END = 10,
	};
	void _refreshPage();
};

