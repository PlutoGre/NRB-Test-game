
#include "stdafx.h"

#include "TitlePage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Title.h"
#include "DataTableManager.h"
#include "TitleCardPage.h"

REGISTER_PAGE("TitlePage",TitlePage);
USING_NS_CC;

void TitleContent::refreshContent( void )
{

}

void TitleContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

TitlePage::TitlePage(void)
{
	mContentList.clear();
}

TitlePage::~TitlePage(void)
{
}

void TitlePage::load( void )
{
	loadCcbiFile("TitlePage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();

	rebuildAllItem();
}

void TitlePage::buildItem( void )
{
	TitleDateCollect::TitleIDCollect collect = TitleDateCollect::Get()->getTitleCollection();;

	TitleDateCollect::TitleIDCollect::iterator itr = collect.begin();
	while(itr != collect.end())
	{
		TitleContent * item = new TitleContent(*itr);
		item->init();
		addItem(item);
		++itr;
	}
}

void TitlePage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void TitlePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void TitlePage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	membersRootNode->removeAllChildren();
}