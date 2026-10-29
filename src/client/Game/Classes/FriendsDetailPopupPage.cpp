
#include "stdafx.h"
#include "BlackBoard.h"
#include "FriendsDetailPopupPage.h"
REGISTER_PAGE("FriendsDetailPopupPage",FriendsDetailPopupPage);
USING_NS_CC;

FriendsDetailPopupPage::FriendsDetailPopupPage(void)
{
}


FriendsDetailPopupPage::~FriendsDetailPopupPage(void)
{
}

void FriendsDetailPopupPage::Enter()
{
	_clearNode();
	MessageManager::Get()->regisiterMessageHandler(MSG_FRIEND_DETAIL_RECICEVEMSG,this);

	CCObject* obj=getVariable("mTitle");
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mTitle");
		CCMessageBox(errMsg,"error");
	}
	mTitle =  dynamic_cast<cocos2d::CCLabelBMFont* >(obj);
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString(mUserName).c_str());
	}

	CCObject* nodeLeaveMsg=getVariable("mLeaveMsg");
	if(!nodeLeaveMsg)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mLeaveMsg");
		CCMessageBox(errMsg,"error");
	}
	cocos2d::CCNode* leaveMsg=dynamic_cast<cocos2d::CCNode*>(nodeLeaveMsg);
	if(leaveMsg)
	{
		leaveMsg->retain();
		mNodeList.push_back(std::make_pair(LEAVE_MSG,leaveMsg));
	}

	CCObject* nodeEnemy=getVariable("mEnemy");
	if(!nodeEnemy)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mEnemy");
		CCMessageBox(errMsg,"error");
	}
	cocos2d::CCNode* enemy=dynamic_cast<cocos2d::CCNode*>(nodeEnemy);
	if(enemy)
	{
		enemy->retain();
		mNodeList.push_back(std::make_pair(ENEMY,enemy));
	}
}

void FriendsDetailPopupPage::switchNode()
{
	NodeList::iterator it=mNodeList.begin();
	for(;it!=mNodeList.end();++it)
	{
		std::pair<NODE_TYPE,cocos2d::CCNode*> node=*it;
		if(node.first==mCurrNodeType)
		{
			node.second->setVisible(true);
		}
		else
		{
			node.second->setVisible(false);
		}
	}
}

void FriendsDetailPopupPage::_clearNode()
{
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

void FriendsDetailPopupPage::Exit( MainFrame* frame)
{
	_clearNode();
}

void FriendsDetailPopupPage::Execute( MainFrame* )
{

}

void FriendsDetailPopupPage::load( void )
{
	loadCcbiFile("FriendsDetailPage.ccbi");
	Enter();
}

void FriendsDetailPopupPage::initDetailContent(void)
{
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString(mUserName).c_str());
	}
	switchNode();
}

void FriendsDetailPopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	bool _isClose=false;
	if(itemName=="onClose")
	{
		_isClose=true;
	}
	else if(itemName=="onCloseButton")
	{
		_isClose=true;
	}
	else if(itemName=="onLeaveMsgButton")
	{
		MsgMainFramePushPage msg;
		msg.pageName="LeaveMsgPage";
		MessageManager::Get()->sendMessage(&msg);
		MsgLeaveMsgReceivePage msgReceive;
		msgReceive.uID = mUserId;
		msgReceive.uName = mUserName;
		MessageManager::Get()->sendMessage(&msgReceive);
		_isClose=true;
	}
	else if(itemName=="onShowTeamButton"||itemName=="onShowEnemyTeam")
	{
		std::string _viewTeamPageName="ViewTeamPageKey";
		std::string _value="ViewTeamPage";
		if(HAS_V(_viewTeamPageName))
		{
			SET_V(_viewTeamPageName,_value);
		}
		else
		{
			ADD_V(_viewTeamPageName,_value);
		}
		OPShowUserBattleArray* msg = new OPShowUserBattleArray;
		msg->set_version(1);
		msg->set_isnpc(1);
		msg->set_relationid(mUserId);
		msg->set_relationtype(1);
		PacketManager::Get()->sendPakcet(OPCODE_SHOW_USER_BATTLE_C,msg);
		_isClose=true;
	}
	else if(itemName=="onDelFriendButton"||itemName=="onDelEnemyButton")
	{
		MsgFriendsDetailDisposePage msg;
		msg.index = mUserId;
		msg.oprType=mCurrNodeType==ENEMY?4:3;
		MessageManager::Get()->sendMessage(&msg);
		_isClose=true;
	}
	if(_isClose)
	{
		MsgMainFramePopPage msg;
		msg.pageName="FriendsDetailPopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void FriendsDetailPopupPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void FriendsDetailPopupPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_FRIEND_DETAIL_RECICEVEMSG)
	{
		const MsgFriendsDetailReceivePage * msg = dynamic_cast<const MsgFriendsDetailReceivePage*>(message);
		if(msg)
		{
			mIndex=msg->index;
			mUserId=msg->uID;
			mUserName=msg->uName;
			mType=msg->type;
			if(mType==LEAVE_MSG)
			{
				mCurrNodeType=LEAVE_MSG;
			}
			else if(mType==Friend_RANKING||mType==Friend_GONGFU)
			{
				mCurrNodeType=ENEMY;
			}
			else if(mType==Friend_Message)
			{
				mCurrNodeType=Friend_Message;
			}
			initDetailContent();
		}
	}
}