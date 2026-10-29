
#include "stdafx.h"

#include "NewActivePromptToolPage.h"
#include "AdventurePage.h"
#include "PopupManager.h"
REGISTER_PAGE("NewActivePromptToolPage", NewActivePromptToolPage);

USING_NS_CC;

NewActivePromptToolPage::NewActivePromptToolPage(void)
{
	_item = new NewActivePromptItem;
}

NewActivePromptToolPage::~NewActivePromptToolPage(void)
{
}

void NewActivePromptToolPage::load( void )
{
	loadCcbiFile("NewPlayPage.ccbi");
	mContent=dynamic_cast<cocos2d::CCNode* >(getVariable("mContent"));
}

void NewActivePromptToolPage::Enter( MainFrame* )
{
	mContent->removeAllChildren();
	CCBContainer* ccb = CCBContainer::create();
	ccb->loadCcbiFile(_item->ccbiPath);
	ccb->setListener(this,0);
	ccb->setPosition((mContent->getContentSize().width-ccb->getContentSize().width)*0.5,(mContent->getContentSize().height-ccb->getContentSize().height)*0.5);
	mContent->addChild(ccb);
}

void NewActivePromptToolPage::Execute( MainFrame* )
{

}

void NewActivePromptToolPage::Exit( MainFrame* )
{
}

void NewActivePromptToolPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	MsgMainFramePopPage msg;
	msg.pageName = "NewActivePromptToolPage";
	MessageManager::Get()->sendMessage(&msg);
	MsgMainFrameChangePage msg1;
	msg1.pageName = _item->pageName;
	MessageManager::Get()->sendMessage(&msg1);
	if(_item->dropType=="soul")
	{
		MsgToTab msg;
		msg.tabIndex = 2;
		MessageManager::Get()->sendMessage(&msg);
	}
	PopupManager::Get()->popPage();
}

void NewActivePromptToolPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onGoAhead")
	{
		MsgMainFrameChangePage msg1;
		msg1.pageName = _item->pageName;
		MessageManager::Get()->sendMessage(&msg1);
		if(_item->dropType=="soul")
		{
			MsgToTab msg;
			msg.tabIndex = 2;
			MessageManager::Get()->sendMessage(&msg);
		}
		else if(_item->dropType=="skillBook")
		{
			const SkillItem*  skillItem=SkillTableManager::Get()->getSkillItemByID(mId);
			AdventureItemInfoMapIterator itr = ServerDateManager::Get()->getAdventureItemInfoMapIterator();
			int _indexId=0;
			while(itr.hasMoreElements())
			{
				const AdventureItemInfo* adventureItemInfo=itr.getNext(); 
				unsigned int adventureId = adventureItemInfo->adventureId;
				const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(adventureId);
				bool isAddItem=true;
				if(adventureItem!=NULL)
				{
					if(adventureItem->adventureType==SkillBook)
					{
						if(adventureItemInfo->itemId==skillItem->itemID)
						{
							_indexId=adventureItemInfo->id;
							break;
						}
					}
				}
			}
			MsgAdventurePageChange msg;
			msg.index = _indexId;
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	MsgMainFramePopPage msg;
	msg.pageName = "NewActivePromptToolPage";
	MessageManager::Get()->sendMessage(&msg);
	PopupManager::Get()->popPage();
}