
#include "stdafx.h"

#include "BuyStonePopupPage.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "PropInfoPage.h"
#include "BuyPropsCountPage.h"
#include "GiftPreviewPage.h"
#include "StoneInlaidPage.h"
#include "BlackBoard.h"

REGISTER_PAGE("BuyStonePopupPage",BuyStonePopupPage);
USING_NS_CC;

void BuyStonePopupContent::refreshContent(){
	const ToolItem * item = ToolTableManager::Get()->getToolItemByID(mID);

	if(!item) return;
	if(item->pageID != 1) return;
	if(item->buyCountLimit == 0) return;
	if(item->isStone == 0) return;

	CCB_FUNC(mContentContainer,"mPropName",CCLabelBMFont,setString(item->name.c_str()));
	std::string des;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("BuyProp_Describe"));
	GameMaths::stringAutoReturn(item->describe,des,width,lines);
	CCB_FUNC(mContentContainer,"mPropDes",CCLabelBMFont,setString(des.c_str()));
	CCB_FUNC(mContentContainer,"mPropPrice",CCLabelBMFont,setString(StringConverter::toString(item->price).c_str()));
	CCB_FUNC(mContentContainer,"mPropPic",CCSprite,initWithFile(item->iconPic.c_str()));
	const UserToolInfo* tool = ServerDateManager::Get()->getUserToolInfoByItemId(item->itemID);
	if(tool)
	{
		CCB_FUNC(mContentContainer,"mPropCount",CCLabelBMFont,setString(StringConverter::toString(tool->count).c_str()));
	}
	else
	{
		CCB_FUNC(mContentContainer,"mPropCount",CCLabelBMFont,setString("0"));
	}

}

void BuyStonePopupContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

	if(itemName == "onBuyItem")
	{
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsCountPage");
		if(ccb)
		{
			BuyPropsCountPage* page = dynamic_cast<BuyPropsCountPage*>(ccb);
			if(page)
			{
				page->setProductID(mID);
				page->setBuyOneOnly(false);

				MsgMainFramePushPage msg;
				msg.pageName = "BuyPropsCountPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}

	}
	if(itemName == "onShowPropInfo")
	{
		const ToolItem* tool = ToolTableManager::Get()->getToolItemByID(tag);
		if(tool)
		{
			if(tool->includeOther.empty())
			{
				PropInfoPageShow(tag,PROP_BUY,false);
			}
			else
			{
				CCBContainer* ccb = CCBManager::Get()->getPage("GiftPreviewPage");
				if(ccb)
				{
					GiftPreviewPage* page = dynamic_cast<GiftPreviewPage*>(ccb);
					if(page)
					{
						page->setTitle("@GiftPackPreviewText");
						GiftPreviewPage::PACKCONTENT_LIST _list;
						//std::list<ToolItem::INCLUDED>::const_iterator it = tool->includeOther.begin();
						ResInfoMap::const_iterator it = tool->includeOther.begin();
						for(;it!=tool->includeOther.end();++it)
						{
							//const ToolItem* tool = ToolTableManager::Get()->getToolItemByID(it->);
							//if(tool)
							{
								//GiftPreviewPage::PackContent content(it->statment,it->icon,it->count);
								GiftPreviewPage::PackContent content(it->second->name,it->second->icon,it->second->count,it->second->quality);
								_list.push_back(content);
							}
						}

						page->setContent(_list);


						MsgMainFramePushPage msg;
						msg.pageName = "GiftPreviewPage";
						MessageManager::Get()->sendMessage(&msg);
					}
				}
			}
		}

	}
	if(itemName == "onBuyButton")
	{
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsCountPage");
		if(ccb)
		{
			BuyPropsCountPage* page = dynamic_cast<BuyPropsCountPage*>(ccb);
			if(page)
			{
				page->setProductID(tag);
				page->setBuyOneOnly(true);

				MsgMainFramePushPage msg;
				msg.pageName = "BuyPropsCountPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}

	}

}

BuyStonePopupPage::BuyStonePopupPage(void)
{
}


BuyStonePopupPage::~BuyStonePopupPage(void)
{
}

void BuyStonePopupPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	rebuildAllItem();
}

void BuyStonePopupPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);

}

void BuyStonePopupPage::Execute( MainFrame* frame)
{

}

void BuyStonePopupPage::load( void )
{
	loadCcbiFile("StoneSystemPopUp.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();

}


void BuyStonePopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

	if(itemName=="onBuyPropButton")
	{

	}else if(itemName=="onShowPropDetailButton")

	{
	}else if(itemName == "onClose"){
		MsgMainFramePopPage msg;
		msg.pageName = "BuyStonePopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

void BuyStonePopupPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

void BuyStonePopupPage::buildItem(){
	ToolTableManager::ToolListIterator itr = ToolTableManager::Get()->getToolIterator();
	while(itr.hasMoreElements()){
		ToolItem* toolItem = itr.getNext();

		if(!toolItem) continue;;
		if(toolItem->pageID != 1) continue;
		if(toolItem->buyCountLimit == 0) continue;
		if(toolItem->isStone == 0) continue;

		BuyStonePopupContent * item = new BuyStonePopupContent(toolItem->itemID);
		item->init();
		addItem(item);
	}
}

void BuyStonePopupPage::addItem(ContentBase* item){
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*item->getContainerNode()->getScaleY()*(mContentList.size() - 1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*item->getContainerNode()->getScaleY()*(mContentList.size()));
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));

}

void BuyStonePopupPage::rebuildAllItem(){
	clearAllItem();
	buildItem();
}

void BuyStonePopupPage::clearAllItem(){
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	mContentList.clear();
	membersRootNode->removeAllChildren();
}

void BuyStonePopupPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_BUY_TOOLSRET_S)
		{
			MsgMainFramePopPage msg2;
			msg2.pageName = "PropUpPage";
			MessageManager::Get()->sendMessage(&msg2);

			MsgMainFramePopPage msg;
			msg.pageName = "BuyStonePopupPage";
			MessageManager::Get()->sendMessage(&msg);

			CCBContainer* ccb = CCBManager::Get()->getPage("StoneInlaidPage");
			StoneInlaidPage* alertPage = dynamic_cast<StoneInlaidPage*>(ccb);
			alertPage->chooseNum = BlackBoard::Get()->currStonePage;

			MsgMainFrameRefreshPage msg1;
			msg1.pageName = "StoneInlaidPage";
			MessageManager::Get()->sendMessage(&msg1);
		}
	}
}