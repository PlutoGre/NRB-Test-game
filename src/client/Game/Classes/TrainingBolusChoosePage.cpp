
#include "stdafx.h"

#include "TrainingBolusChoosePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "Language.h"
#include "BlackBoard.h"
#include "GameMessages.h"
#include "PacketManager.h"
#include "GamePackets.h"
#include "PopupPage.h"
#include "Popup1stPayTipPage.h"

REGISTER_PAGE("TrainingBolusChoosePage",TrainingBolusChoosePage);
USING_NS_CC;

void TrainingBolusItem::refreshContent( void )
{
	const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByItemId(mID);
	const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(mID);
	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mPropPic"));
	if(prop)
	{
		prop->initWithFile(toolItem->iconPic.c_str());
	}
	CCLabelBMFont* propName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPropName"));
	if(propName)
	{
		propName->setString(toolItem->name.c_str());
	}
	CCLabelBMFont* propDes=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPropDes"));
	if(propDes)
	{
		std::string des;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("TrainingBolusChoosePage_Desc"));
		GameMaths::stringAutoReturn(toolItem->describe,des,width,lines);
		propDes->setString(des.c_str());
	}
	CCLabelBMFont* propNum=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPropCount"));
	if(propNum)
	{
		int propCount = 0;
		if(userToolInfo)
		{
			propCount = userToolInfo->count;
		}
		propNum->setString(StringConverter::toString(propCount).c_str());
	}
	CCLabelBMFont* packButtonTxt=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mLable"));
	if(packButtonTxt)
	{
		if(userToolInfo&&userToolInfo->count>0)
		{
			packButtonTxt->setString(Language::Get()->getString(toolItem->buttonString).c_str());
		}
		else
		{
			packButtonTxt->setString(Language::Get()->getString("@Buy").c_str());
		}
		
	}
	if(!userToolInfo||(userToolInfo&&userToolInfo->count<=0))
	{
		CCLabelBMFont* mPriceTxt=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPropPrice"));
		if(mPriceTxt)
		{
			mPriceTxt->setString(StringConverter::toString(toolItem->price).c_str());
		}
		
	}
	else
	{
		dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPropPrice"))->setVisible(false);
		dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPriceLable"))->setVisible(false);
		dynamic_cast<CCSprite* >(mContentContainer->getVariable("mGoldPic"))->setVisible(false);
	}
}

void TrainingBolusItem::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onBuyItem")
	{
		const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByItemId(mID);
		if(userToolInfo&&userToolInfo->count>0)
		{
			BlackBoard::Get()->TrainingBolusID = mID;

			MsgMainFramePopPage msg;
			msg.pageName = "TrainingBolusChoosePage";
			MessageManager::Get()->sendMessage(&msg);

			MsgMainFrameRefreshPage msg1;
			msg1.pageName = "TranExpPreviewPage";
			MessageManager::Get()->sendMessage(&msg1);
		}
		else
		{
			const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
			const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(mID);
			MsgMainFramePopPage msg;
			msg.pageName = "TrainingBolusChoosePage";
			MessageManager::Get()->sendMessage(&msg);
			if(info.goldcoins<toolItem->price)
			{	
				if(!ServerDateManager::Get()->_getMarketInfo().firstPay)
				{
					POP_BOX(NotEnoughGold);
				}
				else
				{
					POP_1STPAY_TIP_BOX(GoldNotEnough);
				}
			}
			else
			{
				BlackBoard::Get()->MarketBuyItemId = 1503;
				BlackBoard::Get()->TrainingBolusID = mID;
				OPBuyTool pkt;
				pkt.set_itemid(mID);
				pkt.set_count(1);
				PacketManager::Get()->sendPakcet(OPCODE_BUY_TOOLS_C,&pkt);
			}	
		}
		
	}
}

TrainingBolusChoosePage::TrainingBolusChoosePage( void )
{

}

TrainingBolusChoosePage::~TrainingBolusChoosePage( void )
{

}

void TrainingBolusChoosePage::load( void )
{
	loadCcbiFile("TrainingBolusChoose.ccbi");
	CCLabelBMFont* title=dynamic_cast<CCLabelBMFont* >(getVariable("mTitle"));
	if(title)
	{
		title->setString(Language::Get()->getString("@ChooseTransferMedicine").c_str());
	}
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
	rebuildAllItem();
}

void TrainingBolusChoosePage::unload( void )
{
	rebuildAllItem();
	CustomPage::unload();
}

void TrainingBolusChoosePage::buildItem( void )
{
	TrainingBolusItem * item1 = new TrainingBolusItem(1503001);
	item1->init();
	addItem(item1);
	TrainingBolusItem * item2 = new TrainingBolusItem(1503002);
	item2->init();
	addItem(item2);
}

void TrainingBolusChoosePage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void TrainingBolusChoosePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void TrainingBolusChoosePage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList().swap(mContentList);
	membersRootNode->removeAllChildren();
}

void TrainingBolusChoosePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "TrainingBolusChoosePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
