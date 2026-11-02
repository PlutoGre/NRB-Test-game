
#include "stdafx.h"

#include "NewActivePromptPage.h"
#include "BlackBoard.h"
#include "PopupManager.h"
#include "AdventurePage.h"
REGISTER_PAGE("NewActivePromptPage", NewActivePromptPage);
USING_NS_CC;

NewActivePromptPage::NewActivePromptPage(void)
{
	_item = new NewActivePromptItem;
}

NewActivePromptPage::~NewActivePromptPage(void)
{
}

void NewActivePromptPage::load( void )
{
	loadCcbiFile("NewPlayPage.ccbi");
	mContent=dynamic_cast<cocos2d::CCNode* >(getVariable("mContent"));
}

void NewActivePromptPage::Enter( MainFrame* )
{
	if(mContent)
	{
		mContent->removeAllChildren();
		CCBContainer* ccb = CCBContainer::create();
		ccb->loadCcbiFile(_item->ccbiPath);
		if(ccb)
		{
			ccb->setListener(this,0);
			ccb->setPosition((mContent->getContentSize().width-ccb->getContentSize().width)*0.5,(mContent->getContentSize().height-ccb->getContentSize().height)*0.5);
			mContent->addChild(ccb);
		}
	}
}

void NewActivePromptPage::Execute( MainFrame* )
{

}

void NewActivePromptPage::Exit( MainFrame* )
{
}

void NewActivePromptPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	MsgMainFrameChangePage msg1;
	msg1.pageName = _item->pageName;
	MessageManager::Get()->sendMessage(&msg1);
	if(_item->dropType=="fightEnd")
	{
		BlackBoard::Get()->ToAdventruePageType = FightEnd;
	}
	MsgMainFramePopPage msg2;
	msg2.pageName = "NewActivePromptPage";
	MessageManager::Get()->sendMessage(&msg2);
	_showNext();
}

void NewActivePromptPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onGoAhead")
	{
		MsgMainFrameChangePage msg1;
		msg1.pageName = _item->pageName;
		MessageManager::Get()->sendMessage(&msg1);
		if(_item->dropType=="fightEnd")
		{
			BlackBoard::Get()->ToAdventruePageType = FightEnd;
		}
	}
	MsgMainFramePopPage msg;
	msg.pageName = "NewActivePromptPage";
	MessageManager::Get()->sendMessage(&msg);
	_showNext();	
}

void NewActivePromptPage::_showNext()
{
	bool _showNext=false;
	if(_itr != _endItr && ++_itr != _endItr)
	{

		NewActivePromptItem* item = dynamic_cast<NewActivePromptItem*>(_itr->second);
		BasePage* bp = CCBManager::Get()->getPage("NewActivePromptPage");
		if(bp)
		{
			NewActivePromptPage* page = dynamic_cast<NewActivePromptPage*>(bp);
			if(page)
			{
				_showNext=true;
				page->setIterator(_itr);
				page->setEndIterator(_endItr);
				page->setInfo(item);
				MsgMainFramePushPage msg;
				msg.pageName = "NewActivePromptPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
	//if(_showNext)
	//{
		PopupManager::Get()->popPage();
	//}
}