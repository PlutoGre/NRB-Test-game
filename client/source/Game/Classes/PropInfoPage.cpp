
#include "stdafx.h"

#include "PropInfoPage.h"
#include "PackagePage.h"
#include "BlackBoard.h"
#include "MessageBoxPage.h"
#include "PopupManager.h"
#include "BuyPropsCountPage.h"
REGISTER_PAGE("PropInfoPage",PropInfoPage);
USING_NS_CC;

void PropInfoContent::refreshContent( void )
{
	CCObject* name=getContainerNode()->getVariable("mPropName");
	if(!name)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPropName");
		CCMessageBox(errMsg,"error");
	}
	mPropName=dynamic_cast<cocos2d::CCLabelBMFont* >(name);


	CCObject* price=getContainerNode()->getVariable("mSellPrice");
	if(!price)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mSellPrice");
		CCMessageBox(errMsg,"error");
	}

	mPropSellPrice=dynamic_cast<cocos2d::CCLabelBMFont* >(price);

	CCObject* info=getContainerNode()->getVariable("mPropInfo");
	if(!info)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPropInfo");
		CCMessageBox(errMsg,"error");
	}

	mPropInfo=dynamic_cast<cocos2d::CCLabelBMFont* >(info);

	CCObject* function=getContainerNode()->getVariable("mFunction1");
	if(!function)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mFunction1");
		CCMessageBox(errMsg,"error");
	}

	mFunction1=dynamic_cast<cocos2d::CCLabelBMFont* >(function);

	CCObject* icon=getContainerNode()->getVariable("mPropPic");
	if(!icon)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPropPic");
		CCMessageBox(errMsg,"error");
	}
	mPropIcon=dynamic_cast<cocos2d::CCSprite* >(icon);
}

void PropInfoContent::setPropOprPanel(unsigned int panelType)
{
	mType=panelType;
	if(mType==PROP_BUY)
	{
		dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mBuyNode"))->setVisible(true);
		dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mUseNode"))->setVisible(false);
	}
	else if(mType==PROP_USE)
	{
		dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mBuyNode"))->setVisible(false);
		dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mUseNode"))->setVisible(true);
	}
	else if(mType==PROP_SHOW)
	{
		dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mBuyNode"))->setVisible(false);
		dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mUseNode"))->setVisible(false);
	}

	const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(getContentID());
	unsigned int itemId=0;
	if(userToolInfo)
	{
		itemId=userToolInfo->itemid;
	}
	
	if(itemId==0||mType==PROP_BUY || mType==PROP_SHOW)
	{
		itemId=getContentID();
	}
	if(itemId==0)
	{
		return;
	}
	const ToolItem* toolItem=ToolTableManager::Get()->getToolItemByID(itemId);
	if(toolItem)
	{
		std::string oStr;
		int lines;
		if(mPropName)
		{
			std::list<std::string> _list;
			_list.push_back(toolItem->name);
			std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@PackageNameTitle"),&_list);
			mPropName->setString(replaced.c_str());
		}
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("PropInfoPage_Desc"));
		GameMaths::stringAutoReturn(toolItem->describe,oStr,width,lines);
		if(mPropInfo)
		{
			mPropInfo->setString(oStr.c_str());
		}
		if(mPropSellPrice)
		{
			mPropSellPrice->setString(StringConverter::toString(toolItem->price).c_str());
		}
		if(mPropIcon)
		{
			//CCTexture2D*tex = CCTextureCache::sharedTextureCache()->addImage(toolItem->bodyPic.c_str());
			mPropIcon->setTexture(toolItem->bodyPic.c_str());
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//mPropIcon->setTextureRect(rect);
		}
		if(mFunction1)
		{
			if(mType==PROP_USE)
			{
				if(toolItem->buttonString!="none")
				{
					mFunction1->setString(toolItem->buttonString.c_str());
				}
				else
				{
					dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mUseNode"))->setVisible(false);
				}	
			}
		}
	}
}


void PropInfoContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{

}

void PropInfoContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PropInfoPage";
		MessageManager::Get()->sendMessage(&msg);
		PropInfoShowNext();
	}
	else if(itemName=="onUse")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PropInfoPage";
		MessageManager::Get()->sendMessage(&msg);
		const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(getContentID());
		if(!userToolInfo)
		{
			PropInfoShowNext();
			return;
		}
		const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(userToolInfo->itemid);
		if(!toolItem)
		{
			PropInfoShowNext();
			return;
		}

		if(toolItem->operationType==POPUP_OPRATION)
		{
			MsgMainFramePushPage msg;
			msg.pageName=toolItem->jumpPage;
			MessageManager::Get()->sendMessage(&msg);

		}
		else if(toolItem->operationType==ADD_OPRATION||toolItem->operationType==OPEN_OPRATION)
		{
			const unsigned int pairTypeId = ResManager::Get()->getTreasurePairType(toolItem->itemID);
			if(pairTypeId!=0)
			{
				const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(pairTypeId);
				if(info==NULL||info->count<=0)
				{
					//TODO Alert Numer of (ToolTableManager::Get()->getToolItemByID(toolItem->togetherUseItemID)->name) is not enough! 
					std::list<std::string> strList;
					strList.push_back(ToolTableManager::Get()->getToolItemByID(pairTypeId)->name);
					MSG_BOX(GameMaths::replaceStringWithStringList(Language::Get()->getString("@NeedTreasureCanOpen"),&strList).c_str());
					PropInfoShowNext();
					return;
				}
			}
			if(toolItem->itemID == TIAOZHANSHU)
			{
				if(ServerDateManager::Get()->getUserArenaInfo().arenaNum+3>ServerDateManager::Get()->getUserBasicInfo().viplevel+4)
				{
					MSG_BOX(Language::Get()->getString("@ArenaNumWillOverRanging"));
					PropInfoShowNext();
					return;
				}
			}
			ResManager::Get()->useToolByIDAndCount(getContentID(),toolItem->itemID,1);
			MsgMainFramePopPage msgPop;
			msgPop.pageName = "PropInfoPage";
			MessageManager::Get()->sendMessage(&msgPop);
		}
		else if(toolItem->operationType==REDIRECT_OPTION)
		{
			MsgMainFrameChangePage msg;
			msg.pageName=toolItem->jumpPage;
			MessageManager::Get()->sendMessage(&msg);
		}else if(toolItem->isStone == 1){
			/*if(ServerDateManager::Get()->getUserBasicInfo().level <  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneInlaidLevel"))){
				return;
			}*/
			if(StoneTableManager::Get()->getStoneItemByID(toolItem->itemID)->nextLevelID == 0){

				MSG_BOX(Language::Get()->getString("@StoneTopLevel"));

			}else{

				BlackBoard::Get()->currentStone = toolItem->itemID;
				BlackBoard::Get()->currentStoneMId = userToolInfo->id;

				MsgMainFramePushPage msg;
				msg.pageName="StoneUpgradePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			return;

		}
	}
	else if(itemName == "onBuyProp")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PropInfoPage";
		MessageManager::Get()->sendMessage(&msg);

		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsCountPage");
		if(ccb)
		{
			BuyPropsCountPage* page = dynamic_cast<BuyPropsCountPage*>(ccb);
			if(page)
			{
				page->setProductID(getContentID());
				page->setBuyOneOnly(false);

				MsgMainFramePushPage msg2;
				msg2.pageName = "BuyPropsCountPage";
				MessageManager::Get()->sendMessage(&msg2);
			}
		}
	}
}

PropInfoPage::PropInfoPage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
}

PropInfoPage::~PropInfoPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}


void PropInfoPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_BUY_PROP_POPUPMSG,this);
	if(!mIsReward)
	{
		mTitle->setString(Language::Get()->getString("@PropInfo").c_str());
	}
	else
	{
		mTitle->setString(Language::Get()->getString("@CareerDropToolTitle").c_str());
	}
	rebuildAllItem();
}

void PropInfoPage::buildItem( void )
{
	PropInfoContent * item = new PropInfoContent(mToolId);
	item->init();
	item->setPropOprPanel(mPanelType);
	addItem(item);
}

void PropInfoPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((members->getViewSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void PropInfoPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void PropInfoPage::clearAllItem( void )
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

void PropInfoPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void PropInfoPage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
		}
		++itr;
	}
}


void PropInfoPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void PropInfoPage::Execute( MainFrame* )
{

}

void PropInfoPage::load( void )
{
	loadCcbiFile("PropInfoPage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
	mTitle=dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mTitle"));
}

void PropInfoPage::showPage(unsigned int id,unsigned int type,bool isReward)
{
	MsgMainFramePushPage msg;
	msg.pageName="PropInfoPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("PropInfoPage");
	PropInfoPage* page = dynamic_cast<PropInfoPage*>(obj);
	if(page)
	{
		page->mToolId=id;
		page->mPanelType=type;
		page->mIsReward=isReward;
	}
}

void PropInfoPage::showNextPage()
{
	if(mIsReward)
	{
		mIsReward=false;
		PopupManager::Get()->popPage();
	}
}

void PropInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void PropInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PropInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onConfirm")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PropInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void PropInfoPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_BUY_PROP_POPUPMSG)
	{
		const MsgBuyPropPopupPage* msg = dynamic_cast<const MsgBuyPropPopupPage*>(message);
		if(msg)
		{
			mToolId=msg->toolId;
			mPanelType=msg->panelIndex;
			rebuildAllItem();
		}
	}
}

void PropInfoPage::showPropInfoPage( unsigned int id,unsigned int type,bool isReward )
{
	dynamic_cast<PropInfoPage*>(CCBManager::Get()->getPage("PropInfoPage"))->showPage(id,type,isReward);
}
