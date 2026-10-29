#pragma once

#include "cocos-ext.h"
#include "cocos2d.h"
#include "CustomPage.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "StringConverter.h"
#include "GameMessages.h"
#include "DataTableManager.h"
class PlayerUpgradePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	PlayerUpgradePage(void);
	~PlayerUpgradePage(void);
	CREATE_FUNC(PlayerUpgradePage);
	virtual PAGE_TYPE getPageType(){ return CustomPage::PT_CONTENT;}
	virtual void load( void );
	virtual void Enter( MainFrame* );
	virtual void Execute( MainFrame* );
	virtual void Exit( MainFrame* );
	virtual void onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag );
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
};
