
#include "stdafx.h"

#include "FriendsPage.h"
#include "ServerDateManager.h"
#include "BlackBoard.h"
#include "MessageBoxPage.h"
REGISTER_PAGE("FriendsPage",FriendsPage);
USING_NS_CC;

void FriendsContent::refreshContent( void )
{
	mLastIsSmallBtn=false;
	const UserFriendsInfo* entity=ServerDateManager::Get()->getUserFriendsInfoByID(getContentID(),mNodeType);
	if(!entity)
	{
		return;
	}
	
	CCB_FUNC(getContainerNode(),"mFriendsName",CCLabelBMFont,setString(Language::Get()->getString(entity->name).c_str()));
	CCB_FUNC(getContainerNode(),"mLevel",CCLabelBMFont,setString(StringConverter::toString(entity->level).c_str()));

	if(mNodeType==FRIDEDS_NODE&&(mNodeType==Friend_TABENEMY))
	{
		CCB_FUNC(getContainerNode(),"mType",CCLabelBMFont,setVisible(true));
		if(entity->type==Friend_GONGFU)
		{
			CCB_FUNC(getContainerNode(),"mType",CCLabelBMFont,setString(Language::Get()->getString("@GongfuEnemy").c_str()));
		}
		else if(entity->type==Friend_RANKING)
		{
			CCB_FUNC(getContainerNode(),"mType",CCLabelBMFont,setString(Language::Get()->getString("@GongfuEnemy").c_str()));
		}
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mType",CCLabelBMFont,setVisible(false));
	}

	CCMenuItemSprite* button=dynamic_cast<CCMenuItemSprite* >(getContainerNode()->getVariable("mButton"));
	CCLabelBMFont* buttonLabel=dynamic_cast<CCLabelBMFont* >(getContainerNode()->getVariable("mButtonLabel"));
	if(button)
	{
		CCMenu* menu = dynamic_cast<CCMenu*>(button->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
	}

	CCB_FUNC(getContainerNode(),"mButton",CCMenuItemImage,setVisible(true));

	if(entity->type==Friend_FRIENDS)
	{
		CCB_FUNC(getContainerNode(),"mButtonLabel",CCLabelBMFont,setString(Language::Get()->getString("@LeaveMsg").c_str()));
	}
	else if(entity->type==Friend_RANKING)
	{
		CCB_FUNC(getContainerNode(),"mButtonLabel",CCLabelBMFont,setString(Language::Get()->getString("@View").c_str()));
	}
	else if(entity->type==Friend_GONGFU)
	{
		CCB_FUNC(getContainerNode(),"mButtonLabel",CCLabelBMFont,setString(Language::Get()->getString("@View").c_str()));
	}
	else if(entity->type==Friend_INVITES)
	{
		if(mCurrTabIndex==Friend_TABFRIENDS)
		{
			CCB_FUNC(getContainerNode(),"mButtonLabel",CCLabelBMFont,setString(Language::Get()->getString("@Invite").c_str()));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mButtonLabel",CCLabelBMFont,setString(Language::Get()->getString("@InviteEnemy").c_str()));
		}
	}

	CCB_FUNC(getContainerNode(),"mButtonLabel",CCLabelBMFont,setVisible(true));
	CCMenuItemSprite* contentButton=dynamic_cast<CCMenuItemSprite* >(getContainerNode()->getVariable("mContentButton"));
	if(contentButton)
	{
		CCMenu* menu = dynamic_cast<CCMenu*>(contentButton->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
	}

}

void FriendsContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	const UserFriendsInfo* se=ServerDateManager::Get()->getUserFriendsInfoByID(getContentID(),mNodeType);
	if(!se)
	{
		return;
	}
	if(itemName=="onClickButton")
	{
		mLastIsSmallBtn=true;
		if(mNodeType==FRIDEDS_NODE)
		{
			if(se->type==Friend_FRIENDS)
			{
				/*MsgMainFramePushPage msg;
				msg.pageName="LeaveMsgPage";
				MessageManager::Get()->sendMessage(&msg);
				MsgLeaveMsgReceivePage msgReceive;
				msgReceive.uID = se->id;
				msgReceive.uName = se->name;
				MessageManager::Get()->sendMessage(&msgReceive);*/
				mLastIsSmallBtn = false;
			}
			else if(se->type==Friend_RANKING)
			{
				if(mNodeType==FRIDEDS_NODE)
				{
					MsgMainFramePushPage msg;
					msg.pageName="FriendsDetailPopupPage";
					MessageManager::Get()->sendMessage(&msg);
					MsgFriendsDetailReceivePage msgReceive;
					msgReceive.index=tag;
					msgReceive.uID = se->id;
					msgReceive.type = se->type;
					msgReceive.uName = se->name;
					msgReceive.isAttention =false;
					MessageManager::Get()->sendMessage(&msgReceive);
				}
			}
			else if(se->type==Friend_GONGFU)
			{
				if(mNodeType==FRIDEDS_NODE)
				{
					MsgMainFramePushPage msg;
					msg.pageName="FriendsDetailPopupPage";
					MessageManager::Get()->sendMessage(&msg);
					MsgFriendsDetailReceivePage msgReceive;
					msgReceive.index=tag;
					msgReceive.uID = se->id;
					msgReceive.type = se->type;
					msgReceive.uName = se->name;
					msgReceive.isAttention =false;
					MessageManager::Get()->sendMessage(&msgReceive);
				}
			}
		}
		else if(mNodeType==MAKEFRIENDS_NODE)
		{
			if(mCurrTabIndex==Friend_FRIENDS)
			{
				unsigned int friendNum=VipDataTableManager::Get()->getVipDataByID(ServerDateManager::Get()->getUserBasicInfo().viplevel)->friendNumLimit;
				if(ServerDateManager::Get()->cloneUserFriendsInfoByType(FRIDEDS_NODE,mCurrTabIndex)->size()>=friendNum)
				{
					MSG_BOX(Language::Get()->getString("@FriendsAddCountMax"));
					return;
				}
			}
			else
			{
				unsigned int enemy=VipDataTableManager::Get()->getVipDataByID(ServerDateManager::Get()->getUserBasicInfo().viplevel)->enemyNumLimit;
				if(ServerDateManager::Get()->cloneUserFriendsInfoByType(FRIDEDS_NODE,mCurrTabIndex)->size()>=enemy)
				{
					MSG_BOX(Language::Get()->getString("@FriendsAddCountMax"));
					return;
				}
			}
			OPDisposeFriendsInfo info;
			info.set_version(1);
			info.set_id(se->id);
			info.set_oprtype(mCurrTabIndex==Friend_FRIENDS?1:2);
			info.set_friendtype(mCurrTabIndex);
			PacketManager::Get()->sendPakcet(OPCODE_DISPOSE_USER_FRIENDS_INFO_C,&info);
			CCLabelBMFont* label=dynamic_cast<CCLabelBMFont* >(getContainerNode()->getVariable("mButtonLabel"));
			CCMenuItemSprite* button=dynamic_cast<CCMenuItemSprite* >(getContainerNode()->getVariable("mButton"));
			if(label)
			{
				label->setVisible(false);
			}
			if(button)
			{
				button->setVisible(false);
			}
		}
	}
	else if(itemName=="onContentButton")
	{
		if(mLastIsSmallBtn)
		{
			mLastIsSmallBtn=false;
			return;
		}
		if(mNodeType==FRIDEDS_NODE)
		{
			MsgMainFramePushPage msg;
			msg.pageName="FriendsDetailPopupPage";
			MessageManager::Get()->sendMessage(&msg);
			MsgFriendsDetailReceivePage msgReceive;
			msgReceive.index=tag;
			msgReceive.uID = se->id;
			msgReceive.type = se->type;
			msgReceive.uName = se->name;
			msgReceive.isAttention =false;
			MessageManager::Get()->sendMessage(&msgReceive);
		}
	}
}

FriendsPage::FriendsPage(void)
	:mSelCondition(0)
	,mExpbarInitScale(-1.0f)
{
	mIsReqData=false;
	mCurrTabIndex = 1;
}


FriendsPage::~FriendsPage(void)
{
	mIsReqData=false;
	libOS::getInstance()->removeListener(this);
}


void FriendsPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_FRIENDSPAGE_CHANGEPAGE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FRIEND_DETAIL_DISPOSE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	libOS::getInstance()->registerListener(this);
	//
	clearAllItem();
	_clearTabList();
	for(unsigned int i=1;i<=TABLENGTH;++i)
	{
		char tab[128];
		sprintf(tab,"mTab_%d",i);
		CCObject* obj=getVariable(tab);
		if(!obj)
		{
			char errMsg[256];
			sprintf(errMsg,"Failed to find variable:mTab_%d",i);
			CCMessageBox(errMsg,"error");
		}
		CCMenuItemImage* tabSprite =  dynamic_cast<cocos2d::CCMenuItemImage* >(obj);
		if(tabSprite)
		{
			tabSprite->retain();
			mFriendsTabList.insert(TAB_BUTTON_LIST::value_type(i,tabSprite));
		}
	}
	CCObject* fNodeObj=getVariable("mFriendsNode");
	if(!fNodeObj)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mFriendsNode");
		CCMessageBox(errMsg,"error");
	}
	cocos2d::CCNode* friendsNode=dynamic_cast<cocos2d::CCNode*>(fNodeObj);
	if(friendsNode)
	{
		friendsNode->retain();
		mNodeList.push_back(std::make_pair(FRIDEDS_NODE,friendsNode));
	}
	
	CCObject* makeFNodeObj=getVariable("mMakeFriendsNode");
	if(!makeFNodeObj)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mMakeFriendsNode");
		CCMessageBox(errMsg,"error");
		
	}
	cocos2d::CCNode* makeFriendsNode=dynamic_cast<cocos2d::CCNode*>(makeFNodeObj);
	if(makeFriendsNode)
	{
		makeFriendsNode->retain();
		mNodeList.push_back(std::make_pair(MAKEFRIENDS_NODE,makeFriendsNode));
	}
	mSelCondition=dynamic_cast<cocos2d::CCLabelBMFont*>(getVariable("mSelCondition"));

	_refreshPage();
	mCurrNodeIndex=FRIDEDS_NODE;
	loadFriendsConfig();
}

void FriendsPage::switchNode()
{
	std::list<std::pair<FRIENDS_NODE_TYPE,cocos2d::CCNode*> >::iterator it=mNodeList.begin();
	for(;it!=mNodeList.end();++it)
	{
		std::pair<FRIENDS_NODE_TYPE,cocos2d::CCNode*> node=*it;
		if(node.first==mCurrNodeIndex)
		{
			node.second->setVisible(true);
		}
		else
		{
			node.second->setVisible(false);
		}
	}
	switchTab();
}

void FriendsPage::buildItem( void )
{
	if(mCurrNodeIndex==FRIDEDS_NODE)
	{
		if(mCurrTabIndex==Friend_TABMESSAGE)
		{
			UserSysMsgEntityList* itmap=ServerDateManager::Get()->cloneSysMsgEntityList(TABFRIENDADD);
			UserSysMsgEntityList::reverse_iterator it=itmap->rbegin();
			for(;it!=itmap->rend();it++)
			{
				unsigned int id =(*it)->second->id;
				SysMsgContent * item = new SysMsgContent(id);
				item->init();
				addItem(item);
			}
			CCB_FUNC(this,"mTypeNum",CCLabelBMFont,setVisible(false));
			CCB_FUNC(this,"mFriendNum",CCLabelBMFont,setVisible(false));
		}
		else
		{
			UserFriendsInfoIterList* itmap=ServerDateManager::Get()->cloneUserFriendsInfoByType(mCurrNodeIndex,mCurrTabIndex);

			UserFriendsInfoIterList::reverse_iterator it=itmap->rbegin();
			int i=0;
			for(;it!=itmap->rend();it++)
			{
				i++;
				unsigned int id =(*it)->second->id;
				FriendsContent * item = new FriendsContent(id,mCurrNodeIndex,mCurrTabIndex);
				item->init();
				addItem(item);
			}
			CCB_FUNC(this,"mTypeNum",CCLabelBMFont,setVisible(true));
			CCB_FUNC(this,"mFriendNum",CCLabelBMFont,setVisible(true));
			_refreshMemberCount();
		}
	}
	else
	{
		UserFriendsInfoIterList* itmap=ServerDateManager::Get()->cloneUserFriendsInfoByType(mCurrNodeIndex,mCurrTabIndex);

		UserFriendsInfoIterList::reverse_iterator it=itmap->rbegin();
		int i=0;
		for(;it!=itmap->rend();it++)
		{
			unsigned int id =(*it)->second->id;
			FriendsContent * item = new FriendsContent(id,mCurrNodeIndex,mCurrTabIndex);
			item->init();
			addItem(item);
		}
	}
	

	if(mCurrNodeIndex==FRIDEDS_NODE)
	{
		if(membersFriends->getContentSize().height<membersFriends->getViewSize().height)
		{
			membersFriends->setContentOffset(ccp(0,membersFriends->maxContainerOffset().y));
		}
		else
		{
			membersFriends->setContentOffset(ccp(0,membersFriends->minContainerOffset().y));
		}
		membersFriends->forceRecaculateChildren();
	}
	else if(mCurrNodeIndex==MAKEFRIENDS_NODE)
	{
		if(membersMakeFriends->getContentSize().height<membersMakeFriends->getViewSize().height)
		{
			membersMakeFriends->setContentOffset(ccp(0,membersMakeFriends->maxContainerOffset().y));
		}
		else
		{
			membersMakeFriends->setContentOffset(ccp(0,membersMakeFriends->minContainerOffset().y));
		}
		membersMakeFriends->forceRecaculateChildren();
	}
}

void FriendsPage::_refreshMemberCount()
{
	UserFriendsInfoIterList* itmap=ServerDateManager::Get()->cloneUserFriendsInfoByType(mCurrNodeIndex,mCurrTabIndex);
	char num[20];

	if(mCurrTabIndex==Friend_TABFRIENDS)
	{
		sprintf(num,"%d/%d",itmap->size(),VipDataTableManager::Get()->getVipDataByID(ServerDateManager::Get()->getUserBasicInfo().viplevel)->friendNumLimit);
		CCB_FUNC(this,"mTypeNum",CCLabelBMFont,setString(Language::Get()->getString("@FriendNum").c_str()));
		CCB_FUNC(this,"mFriendNum",CCLabelBMFont,setString(num));
	}
	else if(mCurrTabIndex==Friend_TABENEMY)
	{
		sprintf(num,"%d/%d",itmap->size(),VipDataTableManager::Get()->getVipDataByID(ServerDateManager::Get()->getUserBasicInfo().viplevel)->enemyNumLimit);
		CCB_FUNC(this,"mTypeNum",CCLabelBMFont,setString(Language::Get()->getString("@EnemyNum").c_str()));
		CCB_FUNC(this,"mFriendNum",CCLabelBMFont,setString(num));
	}
}

void FriendsPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	if(mCurrNodeIndex==FRIDEDS_NODE)
	{
		if(mCurrTabIndex==Friend_TABMESSAGE)
		{
			mMessageRootNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
			mMessageContent->setContentSize(size);
			mMessageContent->setContentOffset(ccp(0, mMessageContent->getViewSize().height - mMessageContent->getContentSize().height*mMessageContent->getScaleY()));
		}
		else
		{
			membersFriendsRootNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
			membersFriends->setContentSize(size);
			membersFriends->setContentOffset(ccp(0, membersFriends->getViewSize().height - membersFriends->getContentSize().height*membersFriends->getScaleY()));
		}
		
	}
	else if(mCurrNodeIndex==MAKEFRIENDS_NODE)
	{
		membersMakeFriendsRootNode->addChild(item->getContainerNode());
		CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
		item->getContainerNode()->setPosition(pos);
		CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
		membersMakeFriends->setContentSize(size);
		membersMakeFriends->setContentOffset(ccp(0, membersMakeFriends->getViewSize().height - membersMakeFriends->getContentSize().height*membersMakeFriends->getScaleY()));
	}
}

void FriendsPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void FriendsPage::_clearTabList(void)
{
	TAB_BUTTON_LIST::iterator itFT=mFriendsTabList.begin();
	for(;itFT!=mFriendsTabList.end();++itFT)
	{
		if(itFT->second)
		{
			itFT->second->release();
		}
	}
	mFriendsTabList.clear();
	NodeList::iterator it=mNodeList.begin();
	for(;it!=mNodeList.end();++it)
	{
		if(it->second)
		{
			it->second->release();
		}
	}
	mNodeList.clear();
}

void FriendsPage::clearAllItem( void )
{
	//
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	membersFriendsRootNode->removeAllChildren();
	membersMakeFriendsRootNode->removeAllChildren();
	mMessageRootNode->removeAllChildren();
}

void FriendsPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void FriendsPage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			if(mCurrNodeIndex==FRIDEDS_NODE)
			{
				if(mCurrTabIndex==Friend_TABMESSAGE)
				{
					mMessageRootNode->removeChild(item->getContainerNode());
				}
				else
				{
					membersFriendsRootNode->removeChild(item->getContainerNode());
				}
			}
			else if(mCurrNodeIndex==MAKEFRIENDS_NODE)
			{
				membersMakeFriendsRootNode->removeChild(item->getContainerNode());
			}
			delete item;
			mContentList.erase(itr);
			break;
		}
		++itr;
	}
	refreshAllContentItem();
}

void FriendsPage::refreshAllContentItem()
{
	ContentList::iterator it=mContentList.begin();
	CCSize size;
	for(int i=0;it!=mContentList.end();++it,++i)
	{
		(*it)->getContainerNode()->setPosition(0,i*(*it)->getContainerNode()->getContentSize().height);
		size.height += (*it)->getContainerNode()->getContentSize().height;
	}
	if(mCurrNodeIndex==FRIDEDS_NODE)
	{
		if(mCurrTabIndex==Friend_TABMESSAGE)
		{
			size.width=mMessageContent->getContentSize().width;
			mMessageContent->setContentSize(size);
			mMessageContent->setContentOffset(mMessageContent->getContentOffset());
		}
		else
		{
			size.width=membersFriends->getContentSize().width;
			membersFriends->setContentSize(size);
			membersFriends->setContentOffset(membersFriends->getContentOffset());
		}

	}
	else if(mCurrNodeIndex==MAKEFRIENDS_NODE)
	{
		size.width=membersMakeFriends->getContentSize().width;
		membersMakeFriends->setContentSize(size);
		membersMakeFriends->setContentOffset(membersMakeFriends->getContentOffset());
	}	
}

void FriendsPage::loadFriendsConfig()
{
	mCurrListSize=ServerDateManager::Get()->getUserFriendsInfoTotalNum(mCurrNodeIndex);
	//mCurrTabIndex=Friend_TABFRIENDS;
	if(mCurrListSize<=0&&!mIsReqData)
	{
		mIsReqData=true;
		OPUserFriendsInfo info;
		info.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_USER_FRIENDS_INFO_C,&info);
	}
	else
	{
		switchNode();
	}
}

void FriendsPage::_refreshAssistBtn()
{
	if(mCurrTabIndex==Friend_TABFRIENDS)
	{
		mMessageContent->setVisible(false);
		membersFriends->setVisible(true);
		CCB_FUNC(this,"mInviteFriends",CCNode,setVisible(true));
		CCB_FUNC(this,"mInviteEnemy",CCNode,setVisible(false));
		CCB_FUNC(this,"mFriendNumNode",CCNode,setVisible(true));
	}
	else if(mCurrTabIndex==Friend_TABENEMY)
	{
		mMessageContent->setVisible(false);
		membersFriends->setVisible(true);
		CCB_FUNC(this,"mInviteFriends",CCNode,setVisible(false));
		CCB_FUNC(this,"mInviteEnemy",CCNode,setVisible(true));
		CCB_FUNC(this,"mFriendNumNode",CCNode,setVisible(true));
	}
	else
	{
		mMessageContent->setVisible(true);
		membersFriends->setVisible(false);
		CCB_FUNC(this,"mInviteFriends",CCNode,setVisible(false));
		CCB_FUNC(this,"mInviteEnemy",CCNode,setVisible(false));
		CCB_FUNC(this,"mFriendNumNode",CCNode,setVisible(false));
	}
}

void FriendsPage::switchTab()
{
	_setSelectBtn();
	rebuildAllItem();
	if(mCurrTabIndex>TABLENGTH)
	{
		mCurrTabIndex=Friend_FRIENDS;
	}
	_refreshAssistBtn();
}

void FriendsPage::_setSelectBtn()
{
	if(mCurrNodeIndex==FRIDEDS_NODE)
	{
		TAB_BUTTON_LIST::iterator it=mFriendsTabList.begin();
		for(;it!=mFriendsTabList.end();++it)
		{
			CCMenuItemImage* tabSprite=it->second;
			if(mCurrTabIndex!=it->first)
			{
				tabSprite->unselected();
			}
			else
			{
				tabSprite->selected();
			}
		}
	}
}

void FriendsPage::Exit( MainFrame* frame)
{
	clearAllItem();
	_clearTabList();
	MessageManager::Get()->removeMessageHandler(this);
	libOS::getInstance()->removeListener(this);
}

void FriendsPage::Execute( MainFrame* )
{

}

void FriendsPage::load( void )
{
	loadCcbiFile("FriendsPage.ccbi");
	membersFriends =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mFriendsContent"));
	membersFriendsRootNode = membersFriends->getContainer();
	membersMakeFriends =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mMakeFriendsContent"));
	membersMakeFriendsRootNode = membersMakeFriends->getContainer();
	mMessageContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mMessegeContent"));
	mMessageRootNode = mMessageContent->getContainer();
	membersFriendsRootNode->removeAllChildren();
	membersMakeFriendsRootNode->removeAllChildren();
	mMessageRootNode->removeAllChildren();
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
}


void FriendsPage::delContainerByIndex(const int index)
{
	ContentList::iterator it=mContentList.begin();
	for(;it!=mContentList.end();++it)
	{
		if((*it)->getContentID()==index)
		{
			removeItem(*it);
			ServerDateManager::Get()->delUserFriendsInfoById(index,mCurrNodeIndex);
			break;
		}
	}
	_refreshMemberCount();
}

void FriendsPage::_clearFirstEnemy()
{
	clearAllItem();
	mSelCondition->setString("");
}

void FriendsPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onToggleFriendsButton")
	{
		if(mCurrNodeIndex!=FRIDEDS_NODE)
		{
			mCurrNodeIndex=FRIDEDS_NODE;
			switchNode();
		}
	}
	else if(itemName=="onToggleMakeFriendsButton")
	{
		mSelCondition->setString("");
		if(mCurrNodeIndex!=MAKEFRIENDS_NODE)
		{
			mCurrNodeIndex=MAKEFRIENDS_NODE;
			if(ServerDateManager::Get()->getUserFriendsInfoTotalNum(mCurrNodeIndex)==0)
			{
				const UserBasicInfo userBasicInfo=ServerDateManager::Get()->getUserBasicInfo();
				OPUserFriendsInfo info;
				info.set_version(1);
				info.set_condition(StringConverter::toString(userBasicInfo.level));
				PacketManager::Get()->sendPakcet(OPCODE_GET_USER_FRIENDS_INFO_C,&info);
			}
			else
			{
				switchNode();
			}
		}
	}
	else if(itemName=="onToggleMakeEnemyButton")
	{
		if(mCurrNodeIndex!=MAKEFRIENDS_NODE)
		{
			mCurrNodeIndex=MAKEFRIENDS_NODE;
			switchNode();
			_clearFirstEnemy();
		}
	}
	else if(itemName=="onSelSendButton")
	{
		if(mInputCondition.size()!=0)
		{
			clearAllItem();
			OPUserFriendsInfo info;
			info.set_version(1);
			info.set_condition(mInputCondition);
			PacketManager::Get()->sendPakcet(OPCODE_GET_USER_FRIENDS_INFO_C,&info);
		}
	}
	else if(itemName=="onFriendsListButton")
	{
		if(mCurrTabIndex!=Friend_TABFRIENDS)	
		{
			mCurrTabIndex=Friend_TABFRIENDS;
			switchTab();
		}
		else
		{
			_setSelectBtn();
		}
	}
	else if(itemName=="onEnemyListButton")
	{
		if(mCurrTabIndex!=Friend_TABENEMY)	
		{
			mCurrTabIndex=Friend_TABENEMY;
			switchTab();
		}
		else
		{
			_setSelectBtn();
		}
	}
	else if(itemName=="onMessegeButton")
	{
		if(mCurrTabIndex!=Friend_TABMESSAGE)	
		{
			mCurrTabIndex=Friend_TABMESSAGE;
			switchTab();
		}
		else
		{
			_setSelectBtn();
		}
	}
	else if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowInputMethod")
	{
		libOS::getInstance()->registerListener(this);
		libOS::getInstance()->showInputbox(true);
	}
}
void FriendsPage::onInputboxEnter(const std::string& content)
{
	mInputCondition=content;
	refreshCondition();
}

void FriendsPage::refreshCondition()
{
	if(mSelCondition)
	{
		mSelCondition->setString(mInputCondition.c_str());
	}
}

void FriendsPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_FRIENDSPAGE_CHANGEPAGE)
	{
		const MsgFriendsPageChangePage* msg = dynamic_cast<const MsgFriendsPageChangePage*>(message);
		if(msg)
		{
			if(msg->oprType==-1)
			{
				delContainerByIndex(msg->index);
			}
			else if(msg->oprType==1)
			{
				delContainerByIndex(msg->index);
			}
			else if(msg->oprType==3)
			{
				delContainerByIndex(msg->index);
			}
			else if(msg->oprType==2)
			{
				/*
				const UserFriendsInfo* entity=ServerDateManager::Get()->getUserFriendsInfoByID(msg->index,mCurrNodeIndex);
				if(entity)
				{
					UserFriendsInfo* createEntity=ServerDateManager::Get()->getAndCreatFriendInfo(mCurrListSize);
					createEntity->id=mCurrListSize;
					createEntity->name=entity->name;
					createEntity->level=entity->level;
					createEntity->type=Friend_Message;
					++mCurrListSize;
				}
				*/
			}
		}
	}
	else if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_DISPOSE_USER_FRIENDS_INFORET_S)
			{
				if(mCurrNodeIndex!=MAKEFRIENDS_NODE)
				{
					delContainerByIndex(mCurrReqIndex);
				}
				else
				{
					std::string disposeRetFriendId="disposeRetFriendIdKey";
					if(HAS_V(disposeRetFriendId))
					{
						int _id=GET_V_UINT(disposeRetFriendId);
						if(_id>0)
						{
							if(mCurrTabIndex==Friend_TABENEMY&&mCurrNodeIndex==MAKEFRIENDS_NODE)
							{
								UserFriendsInfo * userFriendsInfo = ServerDateManager::Get()->getAndCreatFriendInfo(_id);

								const UserFriendsInfo* _inviteItem=ServerDateManager::Get()->getUserFriendsInfoByID(_id,MAKEFRIENDS_NODE);
								if(_inviteItem)
								{
									userFriendsInfo->id			= _inviteItem->id;
									userFriendsInfo->name		= _inviteItem->name;
									userFriendsInfo->level		= _inviteItem->level;
									userFriendsInfo->type		= 2;
								}
							}
						}
					}
				}
			}
			else if(msg->opcode==OPCODE_GET_USER_FRIENDS_INFORET_S)
			{
				//loadFriendsConfig();
				switchNode();
			}
		}
	}
	else if(message->getTypeId() == MSG_FRIEND_DETAIL_DISPOSE)
	{
		const MsgFriendsDetailDisposePage * msg = dynamic_cast<const MsgFriendsDetailDisposePage*>(message);
		if(msg)
		{
			mCurrReqIndex=msg->index;
			mCurrReqOprType=msg->oprType;
			const UserFriendsInfo* createEntity=ServerDateManager::Get()->getUserFriendsInfoByID(mCurrReqIndex,mCurrNodeIndex);
			OPDisposeFriendsInfo info;
			info.set_version(1);
			info.set_id(createEntity->id);
			info.set_oprtype(mCurrReqOprType);
			PacketManager::Get()->sendPakcet(OPCODE_DISPOSE_USER_FRIENDS_INFO_C,&info);
		}
	}
	else if(message->getTypeId()==OPCODE_GET_USER_BASIC_INFORET_S)
	{
		_refreshPage();
	}
}
void FriendsPage::_refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCLabelBMFont* exp = dynamic_cast<CCLabelBMFont* >(getVariable("mExp"));
	if(exp)
	{
		exp->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
	}
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar)
	{
		if(mExpbarInitScale<0)
			mExpbarInitScale = expbar->getScaleX();

		float expscale = 0;
		if(ServerDateManager::Get()->getNextLevelExp()>0)
			expscale = ServerDateManager::Get()->getUserExpPercent();
		if(expscale>1.0f)expscale=1.0f;
		expbar->setScaleX(expscale*mExpbarInitScale);
	}
	CCLabelBMFont* gold = dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	if(gold)
	{
		gold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	}
	CCLabelBMFont* ti = dynamic_cast<CCLabelBMFont* >(getVariable("mTi"));
	if(ti)
	{
		ti->setString((StringConverter::toString(userBasicInfo.power)+"/"+StringConverter::toString(userBasicInfo.maxpower)).c_str());
	}
	CCLabelBMFont* qi = dynamic_cast<CCLabelBMFont* >(getVariable("mQi"));
	if(qi)
	{
		qi->setString((StringConverter::toString(userBasicInfo.vitality)+"/"+StringConverter::toString(userBasicInfo.maxvitality)).c_str());
	}
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(userBasicInfo.level).c_str()));
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{
		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			vipLevel->setVisible(true);
			vipLevel->initWithFile(vipImage.c_str());
		}
		else
		{
			vipLevel->setVisible(false);
		}
	}
}

void FriendsPage::unload( void )
{
	CCBContainer::unload();
	membersFriendsRootNode = NULL;
	membersMakeFriendsRootNode = NULL;
}