
#include "stdafx.h"

#include "AdventureInvitePage.h"
#include "GameMaths.h"
USING_NS_CC;

void InviteContentItem::refreshContent(void)
{
	mItem=AdventureInviteTableManager::Get()->getAdventureInviteItemByID(mID);
	const UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
	Json::Reader jreader;
	jreader.parse(info.mInviteStatus,mInviteStatus,false);
	jreader.parse(info.mInviteRewardStatus,mRewardStatus,false);
	ResInfoMap rewardList=ResManager::Get()->getResInfoListByResBagStr(mItem->rewardInfo);
	ResInfoMap::iterator it=rewardList.begin();
	int i=1;
	for(;it!=rewardList.end();++it)
	{
		if(it->second)
		{
			if(i<=4)
			{
				char _key[128];
				sprintf(_key,"mNode%d",i);
				CCB_FUNC(this->getContainerNode(),_key,CCNode,setVisible(true));
				sprintf(_key,"mName%d",i);
				CCB_FUNC(this->getContainerNode(),_key,CCLabelBMFont,setString(it->second->name.c_str()));
				sprintf(_key,"mNum%d",i);
				CCB_FUNC(this->getContainerNode(),_key,CCLabelBMFont,setString(("x"+StringConverter::toString(it->second->count)).c_str()));
				sprintf(_key,"mPic%d",i);
				CCB_FUNC(this->getContainerNode(),_key,CCSprite,setTexture(it->second->icon.c_str()));
				sprintf(_key,"mFrame%d",i);
				CCB_FUNC(this->getContainerNode(),_key,CCMenuItemImage,setNormalImage(::getFrameNormalSpirte(it->second->quality>0?it->second->quality:4)));
				CCB_FUNC(this->getContainerNode(),_key,CCMenuItemImage,setNormalImage(::getFrameSelectedSpirte(it->second->quality>0?it->second->quality:4)));
				++i;
			}
		}
	}
	for(;i<=4;++i)
	{
		char _key[128];
		sprintf(_key,"mNode%d",i);
		CCB_FUNC(this->getContainerNode(),_key,CCNode,setVisible(false));
	}


	std::list<std::string> _list;
	_list.push_back(StringConverter::toString(mItem->requireCount));
	std::string replaced = GameMaths::replaceStringWithStringList(mItem->describe,&_list);
	CCB_FUNC(this->getContainerNode(),"mDescribe",CCLabelBMFont,setString(replaced.c_str()));

	char _finishNum[128];
	sprintf(_finishNum,"%s/%s",StringConverter::toString(_getInviteCount()).c_str(),StringConverter::toString(mItem->requireCount).c_str());
	std::string fini;
	fini = _finishNum;
	CCB_FUNC(this->getContainerNode(),"mFinishNum",CCLabelBMFont,setString(fini.c_str()));
	_refreshBtn();
}

void InviteContentItem::_refreshBtn()
{
	int status=_getRewardStatus();
	if(status==0)
	{
		CCB_FUNC(this->getContainerNode(),"mButtenNode",CCNode,setVisible(true));
		if(_getInviteCount()>=mItem->requireCount)
		{
			CCB_FUNC(this->getContainerNode(),"mGetReward",CCMenuItemImage,setEnabled(true));
		}
		else
		{
			CCB_FUNC(this->getContainerNode(),"mGetReward",CCMenuItemImage,setEnabled(false));
		}
	}
	else
	{
		CCB_FUNC(this->getContainerNode(),"mButtenNode",CCNode,setVisible(false));
		CCB_FUNC(this->getContainerNode(),"mGetReward",CCMenuItemImage,setEnabled(false));
	}
}

int InviteContentItem::_getRewardStatus()
{
	int status=0;
	try
	{
		char key[128];
		sprintf(key,"%d",mID);
		status=mRewardStatus[key].asInt();
	}
	catch (...)
	{

	}
	return status;
}

int InviteContentItem::_getInviteCount()
{
	int count=0;
	try
	{
		char key[128];
		sprintf(key,"%d",mID);
		count=mInviteStatus[key].asInt();
	}
	catch (...)
	{

	}
	return count;
}

void InviteContentItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onGetRewardBtn")
	{
		if(_getRewardStatus()==0&&_getInviteCount()>=mItem->requireCount)
		{
			mRewardStatus[StringConverter::toString(mID)]=Json::Value(1);
			_refreshBtn();
			OPGetInviteReward msg;
			msg.set_type(mID);
			PacketManager::Get()->sendPakcet(OPCODE_GET_INVITE_REWARD_C, &msg);
		}
	}
}
void InviteContentItem::deleteContinueContentItemItem()
{
	if(mRewardStatus.size()>=(AdventureInviteTableManager::Get()->getTotalNum()))
	{
		MsgAdventureRemoveItem msg;
		msg.index = mParentID;
		MessageManager::Get()->sendMessage(&msg);
	}
}

void AdventureInvitePage::refreshContent()
{

}

void AdventureInvitePage::showSuccessDialog()
{
}

void AdventureInvitePage::loadData()
{
	refreshFlag = true;
	bool getInit = getIsInitData();
	CCB_FUNC(this->getContainerNode(),"mLoginX",CCLabelBMFont,setVisible(false));
	if(!getIsInitData())
	{
		OPUserInviteStatus msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_USER_INVITESTATUS_C, &msg);
	}
}

void AdventureInvitePage::updatePage(bool isShow,float dt)
{
	mIsShow = isShow;
	bool getInit = getIsInitData();
	if(mIsShow&&getIsInitData())
	{
		if(refreshFlag)
		{
			changeRewardStatus();
		}
	}
}

void AdventureInvitePage::exitPage()
{

}

void AdventureInvitePage::changeRewardStatus()
{
	mContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(mContentContainer->getVariable("mContent"));
	//--begin xinzheng 2013-6-5
	{
		CCTouchHandler* pHandler = CCDirector::sharedDirector()->getTouchDispatcher()
											->findHandler(mContent);
		if (pHandler)
		{
			CCTargetedTouchHandler* pTh = dynamic_cast<CCTargetedTouchHandler*>(pHandler);
			if (pTh)
			{
				pTh->setSwallowsTouches(false);//不消化，继续传递
				//自己是竖直touch，父ScrollView横向touch
			}
		}
	}
	//--end
	membersRootNode = mContent->getContainer();
	const UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
	std::list<std::string> _list;
	_list.push_back(info.mInviteKey);
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@InviteDescribe"),&_list);
	CCB_FUNC(this->getContainerNode(),"mLoginX",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(this->getContainerNode(),"mLoginX",CCLabelBMFont,setVisible(true));
	rebuildAllItem();
	refreshFlag=false;
}

void AdventureInvitePage::rebuildAllItem(void)
{
	clearAllItem();
	buildItem();
}

void AdventureInvitePage::clearAllItem(void)
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	if(membersRootNode)
		membersRootNode->removeAllChildren();

}

void AdventureInvitePage::buildItem(void)
{
	AdventureInviteTableManager::AdventureInviteIterator itr = AdventureInviteTableManager::Get()->getAdventureContinueLoginIterator();
	std::list<unsigned int > idList;
	while(itr.hasMoreElements())
	{
		idList.push_back(itr.getNext()->itemID);
	}
	std::list<unsigned int >::reverse_iterator it=idList.rbegin();
	for(;it!=idList.rend();++it)
	{
		InviteContentItem * item = new InviteContentItem(*it,mID);
		item->init();
		addItem(item);
	}
	idList.clear();
}

void AdventureInvitePage::addItem(ContentBase* item)
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((mContent->getViewSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height*mContentList.size());
	mContent->setContentSize(size);
	mContent->setContentOffset(ccp(0, mContent->getViewSize().height - mContent->getContentSize().height*mContent->getScaleY()));
}


void AdventureInvitePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

	
}

void AdventureInvitePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

AdventureInvitePage::~AdventureInvitePage()
{
	clearAllItem();
}
