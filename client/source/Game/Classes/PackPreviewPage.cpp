
#include "stdafx.h"

#include "PackPreviewPage.h"
#include "PopupManager.h"
REGISTER_PAGE("PackPreviewPage",PackPreviewPage);
USING_NS_CC;

void PackPreviewContent::refreshContent( void )
{
	CCObject* name=getContainerNode()->getVariable("mPropName");
	if(!name)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPropName");
		CCMessageBox(errMsg,"error");
	}
	 mPropName=dynamic_cast<cocos2d::CCLabelBMFont* >(name);


	CCObject* number=getContainerNode()->getVariable("mNumber");
	if(!number)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mNumber");
		CCMessageBox(errMsg,"error");
	}

	mPropNumber=dynamic_cast<cocos2d::CCLabelBMFont* >(number);

	CCObject* buttonObj=getContainerNode()->getVariable("mFrameBack");
	if(!buttonObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mFrameBack");
		CCMessageBox(errMsg,"error");
	}
	mPropFrame=dynamic_cast<cocos2d::CCMenuItemImage* >(buttonObj);
	if(mPropFrame && mPropFrame->getParent())
	{
		CCMenu* menu = dynamic_cast<CCMenu*>(mPropFrame->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
	}

	CCObject* icon=getContainerNode()->getVariable("mPropPic");
	if(!icon)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPropPic");
		CCMessageBox(errMsg,"error");
	}
	mPropIcon=dynamic_cast<cocos2d::CCSprite* >(icon);
	if(this->mIndex==FIGHT_END_REWARD)
	{
		refreshFightEndReward();
	}
	else if(this->mIndex==OPEN_GIFT_BOX_REWARD||this->mIndex==OPEN_TREASURE_REWARD||this->mIndex==KISS_GODNESS_REWARD)
	{
		refreshUseToolReward();
	}

}
void PackPreviewContent::refreshUseToolReward()
{
	const ResInfo* info=ServerDateManager::Get()->getResInfoById(getContentID());
	if(mPropFrame)
	{
		mPropFrame->setNormalImage(::getFrameNormalSpirte(info->quality>0?info->quality:4));
		mPropFrame->setSelectedImage(::getFrameSelectedSpirte(info->quality>0?info->quality:4));
	}

	if(mPropIcon)
	{
		if(info->icon.length()>0)
		{
			//CCTexture2D*tex = CCTextureCache::sharedTextureCache()->addImage(info->icon.c_str());
			mPropIcon->setTexture(info->icon.c_str());
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//mPropIcon->setTextureRect(rect);
		}
	}
	if(mPropName)
	{
		mPropName->setString(info->name.c_str());
	}
	if(mPropNumber)
	{
		mPropNumber->setString(StringConverter::toString(info->count).c_str());
	}
}
void PackPreviewContent::refreshFightEndReward()
{
	const AdventureFightEndInfo::RewardItem* rewardItem=ServerDateManager::Get()->getFightEndRewardItemById(getContentID());
	const ResInfo* ri=ResManager::Get()->getResInfoByTypeAndId(rewardItem->itemType,rewardItem->itemId,rewardItem->count);
	if(mPropFrame)
	{
		mPropFrame->setNormalImage(::getFrameNormalSpirte(ri->quality>0?ri->quality:4));
		mPropFrame->setSelectedImage(::getFrameSelectedSpirte(ri->quality>0?ri->quality:4));
	}
	if(mPropIcon)
	{
		mPropIcon->initWithFile(ri->icon.c_str());
	}
	if(mPropName)
	{
		mPropName->setString(ri->name.c_str());
	}
	if(mPropNumber)
	{
		mPropNumber->setString(StringConverter::toString(rewardItem->count).c_str());
	}
}

void PackPreviewContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{

}

void PackPreviewContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	
}

PackPreviewPage::PackPreviewPage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
}

PackPreviewPage::~PackPreviewPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}


void PackPreviewPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_PACKPREVIEW_SOURCE_MSG,this);
	//rebuildAllItem();
}

void PackPreviewPage::buildItem( void )
{
	if(mDataSource==FIGHT_END_REWARD)
	{
		buildFightEndRewardItem();
	}
	else if(mDataSource==OPEN_TREASURE_REWARD||mDataSource==OPEN_GIFT_BOX_REWARD||mDataSource==KISS_GODNESS_REWARD)
	{
		buildUseToolRewardItem();
	}
}

//FightEndReward
void PackPreviewPage::buildFightEndRewardItem( void )
{
	AdventureFightEndInfo::RewardItemMapIterator it=ServerDateManager::Get()->getFightEndRewardItemMapIterator();
	while(it.hasMoreElements())
	{
		PackPreviewContent * item = new PackPreviewContent(mDataSource,it.getNext()->id);
		item->init();
		addItem(item);
	}
}
//OPEN_TREASURE_REWARD and OPEN_GIFT_BOX_REWARD
void PackPreviewPage::buildUseToolRewardItem( void )
{
	ResIDCollect::reverse_iterator itr = ServerDateManager::Get()->mResIDList.rbegin();
	while(itr != ServerDateManager::Get()->mResIDList.rend())
	{
		PackPreviewContent * item = new PackPreviewContent(mDataSource,(*itr));
		item->init();
		addItem(item);
		++itr;
	}
	ServerDateManager::Get()->clealAllResInfo();
}


void PackPreviewPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((members->getViewSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void PackPreviewPage::rebuildAllItem( void )
{
	if(mDataSource==FIGHT_END_REWARD)
	{
		mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());
		mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());
	}
	else if(mDataSource==OPEN_TREASURE_REWARD)
	{
		mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());

		if(BlackBoard::Get()->continueOpenBoxNum>1){
			std::list<std::string> strList;
			strList.push_back(StringConverter::toString(BlackBoard::Get()->continueOpenBoxNum));
			CCB_FUNC(this,"mPackPageMsg",CCLabelBMFont,setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ContinueOpenBox"),&strList).c_str()));
		}else{
			mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());

		}
	}
	else if(mDataSource==OPEN_GIFT_BOX_REWARD)
	{
		mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());
		mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());
	}
	else if(mDataSource==KISS_GODNESS_REWARD)
	{
		mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());
		mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());
	}
	clearAllItem();
	buildItem();
}

void PackPreviewPage::clearAllItem( void )
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

void PackPreviewPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void PackPreviewPage::removeItem( ContentBase* item )
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


void PackPreviewPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void PackPreviewPage::Execute( MainFrame* )
{

}

void PackPreviewPage::load( void )
{
	loadCcbiFile("PackPreivew.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
	mTitle=dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mTitle"));
	mPackPageMsg=dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mPackPageMsg"));
}

void PackPreviewPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void PackPreviewPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PackPreviewPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onConfirm")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PackPreviewPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	PopupManager::Get()->popPage();
}

void PackPreviewPage::showPage(unsigned int dataSource)
{
	MsgMainFramePushPage msg;
	msg.pageName="PackPreviewPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("PropInfoPage");
	PackPreviewPage* page = dynamic_cast<PackPreviewPage*>(obj);
	if(page)
	{
		page->mDataSource=dataSource;
	}
}

void PackPreviewPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_PACKPREVIEW_SOURCE_MSG)
	{
		const MsgPackPreviewSourceMsg* msg = dynamic_cast<const MsgPackPreviewSourceMsg*>(message);
		if(msg)
		{
			mDataSource=msg->index;
			if(mDataSource==FIGHT_END_REWARD)
			{
				mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());
				mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());
			}
			else if(mDataSource==OPEN_TREASURE_REWARD)
			{
				mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());

				if(BlackBoard::Get()->continueOpenBoxNum>1){
					std::list<std::string> strList;
					strList.push_back(StringConverter::toString(BlackBoard::Get()->continueOpenBoxNum));
					CCB_FUNC(this,"mPackPageMsg",CCLabelBMFont,setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ContinueOpenBox"),&strList).c_str()));
				}else{
					mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());

				}

			}
			else if(mDataSource==OPEN_GIFT_BOX_REWARD)
			{
				mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());
				mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());
			}
			else if(mDataSource==KISS_GODNESS_REWARD)
			{
				mTitle->setString(Language::Get()->getString("@PackPreviewTitle").c_str());
				mPackPageMsg->setString(Language::Get()->getString("@PackPreviewPageMsg").c_str());
			}

			rebuildAllItem();
		}
	}

}