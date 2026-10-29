
#include "stdafx.h"

#include "AdventureContinuePage.h"
#include "GameMaths.h"
//
USING_NS_CC;

void ContinueContentItem::refreshContent(void)
{
	mItem=AdventureContinueLoginTableManager::Get()->getAdventureContinueLoginItemByID(mID);
	const UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
	Json::Reader jreader;
	jreader.parse(info.mContinueLoginInfo,mLoginStatus,false);
	ResInfoMap rewardList=ResManager::Get()->getResInfoListByResBagStr(mItem->rewardInfo);
	ResInfoMap::iterator it=rewardList.begin();
	int i=1;
	for(;it!=rewardList.end();++it)
	{
		if(it->second)
		{
			if(i<=3)
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
				if(ResManager::Get()->getResMainType(it->second->type)==DISCIPLE_TYPE||ResManager::Get()->getResMainType(it->second->type)==SOUL_TYPE)
				{
					CCB_FUNC(this->getContainerNode(),_key,CCSprite,setScale(1.0f));
				}
				sprintf(_key,"mFrame%d",i);
				CCB_FUNC(this->getContainerNode(),_key,CCMenuItemImage,setNormalImage(::getFrameNormalSpirte(it->second->quality>0?it->second->quality:4)));
				CCB_FUNC(this->getContainerNode(),_key,CCMenuItemImage,setNormalImage(::getFrameSelectedSpirte(it->second->quality>0?it->second->quality:4)));
				
				++i;
			}
		}
	}
	for(;i<=3;++i)
	{
		char _key[128];
		sprintf(_key,"mNode%d",i);
		CCB_FUNC(this->getContainerNode(),_key,CCNode,setVisible(true));
	}


	std::list<std::string> _list;
	_list.push_back(StringConverter::toString(mItem->dayCount));
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@AdventureContinueLoginDes"),&_list);
	CCB_FUNC(this->getContainerNode(),"mDays",CCLabelBMFont,setString(replaced.c_str()));
	_refreshBtn();
}

void ContinueContentItem::_refreshBtn()
{
	int status=_getStatus();
	if(status==0)
	{
		CCB_FUNC(this->getContainerNode(),"mButtenNode",CCNode,setVisible(true));
		if(_getLoginCount()>=mItem->dayCount)
		{
			CCB_FUNC(this->getContainerNode(),"mGetReward",CCMenuItemImage,setEnabled(true));
		}
		else
		{
			CCB_FUNC(this->getContainerNode(),"mGetReward",CCMenuItemImage,setEnabled(false));
			//CCB_FUNC(this->getContainerNode(), "mGetRewardLable", CCLabelBMFont, setString(Language::Get()->getString("@AlreadyGetReward").c_str()));
			CCB_FUNC(this->getContainerNode(), "mGetRewardLable", CCLabelBMFont, setString(Language::Get()->getString("@LvUpReceive").c_str()));
		}
	}
	else
	{
		CCB_FUNC(this->getContainerNode(),"mButtenNode",CCNode,setVisible(false));
		CCB_FUNC(this->getContainerNode(),"mGetReward",CCMenuItemImage,setEnabled(false));
		CCB_FUNC(this->getContainerNode(), "mGetRewardLable", CCLabelBMFont, setString(Language::Get()->getString("@AlreadyGetReward").c_str()));

	}
}

int ContinueContentItem::_getStatus()
{
	int status=0;
	try//android compile need -fexceptions
	{
		char key[128];
		sprintf(key,"%d",mID);
		status=mLoginStatus[key].asInt();
	}
	catch (...)
	{

	}
	return status;
}

int ContinueContentItem::_getLoginCount()
{
	int count=0;
	try
	{
		char key[128];
		sprintf(key,"%s","c");
		count=mLoginStatus[key].asInt();
	}
	catch (...)
	{

	}
	return count;
}

void ContinueContentItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onGetRewardBtn")
	{
		if(_getStatus()==0&&_getLoginCount()>=mItem->dayCount)
		{
			mLoginStatus[StringConverter::toString(mID)]=Json::Value(1);
			_refreshBtn();
			OPRewardAdventureContinueLogin msg;
			msg.set_version(1);
			msg.set_id(mID);
			PacketManager::Get()->sendPakcet(OPCODE_ADVENTURE_CONTINUELOGIN_REWARD_C, &msg);
		}
	}
}
void ContinueContentItem::deleteContinueContentItemItem()
{
	if(mLoginStatus.size()>=(AdventureContinueLoginTableManager::Get()->getTotalNum()+2))
	{
		MsgAdventureRemoveItem msg;
		msg.index = mParentID;
		MessageManager::Get()->sendMessage(&msg);
	}
}

void AdventureContinuePage::refreshContent()
{
}

void AdventureContinuePage::showSuccessDialog()
{
}

void AdventureContinuePage::loadData()
{
	refreshFlag = true;
	bool getInit = getIsInitData();
	CCB_FUNC(this->getContainerNode(),"mLoginX",CCLabelBMFont,setVisible(false));
	if(!getIsInitData())
	{
		clearAllItem();
		OPGetAdventureContinueLogin msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFO_C, &msg);
	}
}

void AdventureContinuePage::updatePage(bool isShow,float dt)
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

void AdventureContinuePage::exitPage()
{

}

void AdventureContinuePage::changeRewardStatus()
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
	Json::Value loginStatus;
	const UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
	Json::Reader jreader;
	jreader.parse(info.mContinueLoginInfo,loginStatus,false);
	int count=0;
	//try
	{
		char key[128];
		sprintf(key,"%s","c");
		count=loginStatus[key].asInt();
	}
	//catch (...)
	{

	}
	std::list<std::string> _list;
	_list.push_back(StringConverter::toString(count));
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@youhaveloginxdays"),&_list);
	CCB_FUNC(this->getContainerNode(),"mLoginX",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(this->getContainerNode(),"mLoginX",CCLabelBMFont,setVisible(true));
	rebuildAllItem();
	refreshFlag=false;
}

void AdventureContinuePage::rebuildAllItem(void)
{
	clearAllItem();
	buildItem();
}

void AdventureContinuePage::clearAllItem(void)
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

void AdventureContinuePage::buildItem(void)
{
	AdventureContinueLoginTableManager::AdventureContinueLoginIterator itr = AdventureContinueLoginTableManager::Get()->getAdventureContinueLoginIterator();
	std::list<unsigned int > idList;
	while(itr.hasMoreElements())
	{
		idList.push_back(itr.getNext()->itemID);
	}
	std::list<unsigned int >::reverse_iterator it=idList.rbegin();
	for(;it!=idList.rend();++it)
	{
		ContinueContentItem * item = new ContinueContentItem(*it,mID);
		item->init();
		addItem(item);
	}
	idList.clear();
}

void AdventureContinuePage::addItem(ContentBase* item)
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((mContent->getViewSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height*mContentList.size());
	mContent->setContentSize(size);
	mContent->setContentOffset(ccp(0, mContent->getViewSize().height - mContent->getContentSize().height*mContent->getScaleY()));
}


void AdventureContinuePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

	
}

void AdventureContinuePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

AdventureContinuePage::~AdventureContinuePage()
{
	clearAllItem();
}
