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


class NewActivePromptToolPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	NewActivePromptToolPage(void);
	~NewActivePromptToolPage(void);
	CREATE_FUNC(NewActivePromptToolPage);
	virtual PAGE_TYPE getPageType(){ return CustomPage::PT_CONTENT;}
	
	virtual void load( void );
	virtual void Enter( MainFrame* );
	virtual void Execute( MainFrame* );
	virtual void Exit( MainFrame* );
	virtual void onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag );
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setInfo(NewActivePromptItem* item,unsigned int _id){_item=item;mId=_id;};

private:
	NewActivePromptItem* _item;
	unsigned int mId;
	cocos2d::CCNode* mContent;

};