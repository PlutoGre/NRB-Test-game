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


class NewActivePromptPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	NewActivePromptPage(void);
	~NewActivePromptPage(void);
	CREATE_FUNC(NewActivePromptPage);
	virtual PAGE_TYPE getPageType(){ return CustomPage::PT_CONTENT;}
	
	virtual void load( void );
	virtual void Enter( MainFrame* );
	virtual void Execute( MainFrame* );
	virtual void Exit( MainFrame* );
	virtual void onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag );
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setInfo(NewActivePromptItem* item)
	{
		_item = item;
	}

	void setIterator(NewActivePromptTableManager::NewActivePromptList::iterator itr)
	{
		_itr = itr;
	}

	void setEndIterator(NewActivePromptTableManager::NewActivePromptList::iterator endItr)
	{
		_endItr = endItr;
	}

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		return _item->ccbiPath;
	}
private:
	NewActivePromptItem * newActivePromptItem;
	NewActivePromptItem* _item;
	CCNode* mContent;
	NewActivePromptTableManager::NewActivePromptList::iterator _itr ;
	NewActivePromptTableManager::NewActivePromptList::iterator _endItr ;
	void _showNext();
};
