
#include "stdafx.h"

#include "SysMsgPage.h"
#include "ServerDateManager.h"
#include "MessageBoxPage.h"
#include "PopupNotEnoughPage.h"
#include "BlackBoard.h"
#include "PopupPage.h"
#include "FightPage.h"
REGISTER_PAGE("SysMsgPage",SysMsgPage);
#include "StringConverter.h"
USING_NS_CC;

void SysMsgContent::refreshContent( void )
{
	bool reward=false;
	const UserSysMsgInfo* entity=ServerDateManager::Get()->getUserSysMsgEntityByID(getContentID());
	Json::Reader jreader;
	jreader.parse(entity->info,mData,false);
	if(!entity)
	{
		return;
	}
	bool isFriend=false;
	char msgKey[256];
	CCB_FUNC(getContainerNode(),"mAddEnemy",CCMenuItemImage,setVisible(false));
	if(entity->sysMsgType==RANKING)
	{
		if(entity->fightRet==WIN)
		{
			sprintf(msgKey,"@SMRankingWin");
		}
		else if(entity->fightRet==FAIL)
		{
			sprintf(msgKey,"@SMRankingFail");
		}
	}
	else if(entity->sysMsgType==GONGFU)
	{
		if(entity->fightRet==WIN)
		{
			sprintf(msgKey,"@SMGongfuWin");
		}
		else if(entity->fightRet==FAIL)
		{
			sprintf(msgKey,"@SMGongfuFail");
		}
	}
	else if(entity->sysMsgType==FRIENDS_REQ)
	{
		isFriend=true;
		sprintf(msgKey,"@SMFriendsMsg");
	}
	else if(entity->sysMsgType==SYSTEM_FIGHTEND)
	{
		reward=true;
		sprintf(msgKey,"@SMFightEndReward");
	}
	else if(entity->sysMsgType==SYSTEM_WORDBOSS)
	{
		reward=true;
		sprintf(msgKey,"@SMWorldBossReward");
	}
	else
	{
		reward=true;
		sprintf(msgKey,"@System");
	}
	bool showOprMsg=false;
	char oprMsgKey[256];
	if(isFriend&&(entity->oprType==AGREE||entity->oprType==REFUSE))
	{
		if(entity->oprType==AGREE)
		{
			sprintf(oprMsgKey,"@AlreadyAgree");
		}
		else if(entity->oprType==REFUSE)
		{
			sprintf(oprMsgKey,"@AlreadyRefuse");
		}
		showOprMsg=true;
	}
	CCBContainer* ccb = getContainerNode();
	if(entity->sysMsgType==GONGFU||entity->sysMsgType==RANKING)
	{
		CCB_FUNC(ccb,"mSysName2",CCLabelTTF,setString(mData["n"].asString().c_str()));
	}
	else
	{
		CCB_FUNC(ccb,"mSysName2",CCLabelTTF,setString(Language::Get()->getString(entity->name).c_str()));
	}
		
	if(reward)
	{
		CCB_FUNC(ccb,"mName",CCLabelBMFont,setVisible(false));
	}

	CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setAnchorPoint(cocos2d::CCPoint(0,0.8f)));
	std::string outstr;
	int lines;
	if(entity->sysMsgType==LEAVE_MSG)
	{
		GameMaths::stringAutoReturn(entity->content,outstr,30,lines);
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
	}
	else if(entity->sysMsgType==FRIENDS_REQ)
	{
		std::list<std::string> replaceList;
		if(!mData["n"].empty())
		{
			replaceList.push_back(mData["n"].asString());
		}
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString(msgKey),&replaceList),outstr,28,lines);
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
	}
	else if(entity->sysMsgType==SYSTEM_FIGHTEND)
	{
		std::list<std::string> replaceList;
		if(!mData["t"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["t"].asInt()));
		}
		if(!mData["b"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["b"].asInt()));
		}
		if(!mData["l"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["l"].asInt()));
		}
		std::string rewardContent="";
		RewardDataMapIterator it=ServerDateManager::Get()->getSysMsgRewardMapIteratorById(entity->id);
		int size=0;
		while(it.hasMoreElements())
		{
			const RewardData* item=it.getNext();
			const ResInfo* info=ResManager::Get()->getResInfoByTypeAndId(item->itemType,item->itemId,item->count);
			rewardContent+=(size!=0?",":"")+info->name+"x"+StringConverter::toString(info->count);
			size++;
		}
		replaceList.push_back(rewardContent);
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString(msgKey),&replaceList),outstr,19,lines);
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
	}
	else if(entity->sysMsgType==SYSTEM_WORDBOSS)
	{
		std::list<std::string> replaceList;
		if(!mData["t"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["t"].asInt()));
		}
		if(!mData["b"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["b"].asInt()));
		}
		if(!mData["l"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["l"].asInt()));
		}
		std::string rewardContent="";
		RewardDataMapIterator it=ServerDateManager::Get()->getSysMsgRewardMapIteratorById(entity->id);
		int size=0;
		while(it.hasMoreElements())
		{
			const RewardData* item=it.getNext();
			const ResInfo* info=ResManager::Get()->getResInfoByTypeAndId(item->itemType,item->itemId,item->count);
			rewardContent+=(size!=0?",":"")+info->name+"x"+StringConverter::toString(info->count);
			size++;
		}
		replaceList.push_back(rewardContent);
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString(msgKey),&replaceList),outstr,19,lines);
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
	}
	else if(entity->sysMsgType==SYSTEM)
	{
		std::list<std::string> replaceList;
		std::string rewardContent=entity->content;
		if(entity->rewardDataMap.size()!=0)
		{
			RewardDataMapIterator it=ServerDateManager::Get()->getSysMsgRewardMapIteratorById(entity->id);
			int size=0;
			while(it.hasMoreElements())
			{
				const RewardData* item=it.getNext();
				const ResInfo* info=ResManager::Get()->getResInfoByTypeAndId(item->itemType,item->itemId,item->count);
				rewardContent+=(size!=0?",":"")+info->name+"x"+StringConverter::toString(info->count);
				size++;
			}
		}
		replaceList.push_back(rewardContent);
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(rewardContent,&replaceList),outstr,19,lines);
		std::string newStr="";
		char key[128];
		sprintf(key,"\\n");
		std::string::size_type index=outstr.find(key);
		if(index!=newStr.npos)
		{
			int splitEnd=index+2;
			newStr+=outstr.substr(0,index);
			newStr+='\n';
			newStr+=outstr.substr(splitEnd,outstr.length());
		}
		else
		{
			newStr=outstr;
		}
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(newStr.c_str()));
	}
	else if(entity->sysMsgType==GONGFU||entity->sysMsgType==RANKING)
	{
		std::list<std::string> replaceList;
		if(!mData["s"].empty())
		{
			int skillId=mData["s"].asInt();
			replaceList.push_back(SkillTableManager::Get()->getSkillItemByID(skillId)->name);
		}
		if(!mData["l"].empty())
		{
			replaceList.push_back(StringConverter::toString(mData["l"].asInt()));
		}
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString(msgKey),&replaceList),outstr,19,lines);
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
	}
	else
	{
		GameMaths::stringAutoReturn(Language::Get()->getString(msgKey),outstr,19,lines);
		CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
	}	

	CCB_FUNC(getContainerNode(),"mEventTime",CCLabelBMFont,setString(GameMaths::formatTimeToDate(entity->eventTime).c_str()));

	if(!isFriend||showOprMsg)
	{
		CCB_FUNC(getContainerNode(),"mRefuse",CCMenuItemImage,setVisible(false));
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mRefuse",CCMenuItemImage,setVisible(true));
	}

	if(!isFriend||showOprMsg)
	{
		CCB_FUNC(getContainerNode(),"mOprName_1",CCLabelBMFont,setVisible(false));
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mOprName_1",CCLabelBMFont,setVisible(true));
		CCB_FUNC(getContainerNode(),"mOprName_1",CCLabelBMFont,setString(Language::Get()->getString("@Refuse").c_str()));
	}

	//TODO:already AddEnemy
	if(entity->sysMsgType==RANKING||entity->sysMsgType==GONGFU)
	{
		if(entity->addEnemyType==1)
		{
			CCB_FUNC(getContainerNode(),"mOprName_1",CCLabelBMFont,setVisible(false));
			CCB_FUNC(getContainerNode(),"mAddEnemy",CCMenuItemImage,setVisible(false));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mOprName_1",CCLabelBMFont,setString(Language::Get()->getString("@addEnemyName").c_str()));
			CCB_FUNC(getContainerNode(),"mOprName_1",CCLabelBMFont,setVisible(true));
			CCB_FUNC(getContainerNode(),"mAddEnemy",CCMenuItemImage,setVisible(true));
		}
		
	}
	
	if(!showOprMsg)
	{
		CCB_FUNC(getContainerNode(),"mFriendsOprMsg",CCLabelBMFont,setVisible(false));
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mFriendsOprMsg",CCLabelBMFont,setString(Language::Get()->getString(oprMsgKey).c_str()));
		CCB_FUNC(getContainerNode(),"mFriendsOprMsg",CCLabelBMFont,setVisible(true));
	}

	if(entity->sysMsgType==RANKING&&entity->oprType==UNOPR)
	{
		CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@Ranking").c_str()));
	}
	else if(entity->sysMsgType==GONGFU&&entity->oprType==UNOPR)
	{
		CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@Counterattack").c_str()));
	}
	else if(entity->sysMsgType==FRIENDS_REQ)
	{
		CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(true));
		if(showOprMsg)
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@LeaveMsg").c_str()));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@Agree").c_str()));
		}
	}
	else if(entity->sysMsgType==LEAVE_MSG)
	{
		CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@LeaveMsg").c_str()));
	}
	else if(entity->sysMsgType==SYSTEM_FIGHTEND||entity->sysMsgType==SYSTEM)
	{
		if(entity->oprType==0)
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(true));
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@LvUpReceive").c_str()));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(false));
		}
	}
	else if(entity->sysMsgType==SYSTEM_WORDBOSS)
	{
		if(entity->oprType==0)
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(true));
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setString(Language::Get()->getString("@LvUpReceive").c_str()));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(false));
		}
	}

	if(showOprMsg||entity->sysMsgType!=FRIENDS_REQ)
	{
		CCB_FUNC(getContainerNode(),"mAgree",CCMenuItemImage,setVisible(false));
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mAgree",CCMenuItemImage,setVisible(true));
	}

	if(!showOprMsg&&entity->sysMsgType!=LEAVE_MSG)
	{
		CCB_FUNC(getContainerNode(),"mLeaveMsg",CCMenuItemImage,setVisible(false));
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mLeaveMsg",CCMenuItemImage,setVisible(true));
	}

	if(entity->sysMsgType==RANKING)
	{
		if(entity->oprType==UNOPR)
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(true));
			CCB_FUNC(getContainerNode(),"mRanking",CCMenuItemImage,setVisible(true));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(false));
			CCB_FUNC(getContainerNode(),"mRanking",CCMenuItemImage,setVisible(false));
		}
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mRanking",CCMenuItemImage,setVisible(false));
	}

	if(entity->sysMsgType==GONGFU)
	{
		if(entity->oprType==UNOPR&&entity->fightRet!=WIN)
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(true));
			CCB_FUNC(getContainerNode(),"mGongfu",CCMenuItemImage,setVisible(true));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(false));
			CCB_FUNC(getContainerNode(),"mGongfu",CCMenuItemImage,setVisible(false));
		}
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mGongfu",CCMenuItemImage,setVisible(false));
	}

	if(entity->sysMsgType==SYSTEM_FIGHTEND||entity->sysMsgType==SYSTEM||entity->sysMsgType==SYSTEM_WORDBOSS)
	{
		if(entity->oprType==0)
		{
			CCB_FUNC(getContainerNode(),"mRecieve",CCMenuItemImage,setVisible(true));
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mRecieve",CCMenuItemImage,setVisible(false));
		}
	}
	else
	{
		CCB_FUNC(getContainerNode(),"mRecieve",CCMenuItemImage,setVisible(false));
	}
	if(entity->sysMsgType==SYSTEM_FIGHTEND||entity->sysMsgType==SYSTEM||entity->sysMsgType==SYSTEM_WORDBOSS)
	{
		if(!mData["m"].empty())
		{
			CCB_FUNC(getContainerNode(),"mRecieve",CCMenuItemImage,setVisible(false));
			CCB_FUNC(getContainerNode(),"mOprName_2",CCLabelBMFont,setVisible(false));

			std::string outstr;
			int lines;
			GameMaths::stringAutoReturn(entity->content,outstr,19,lines);
			CCB_FUNC(getContainerNode(),"mMessage",CCLabelBMFont,setString(outstr.c_str()));
		}
	}
	
}

void SysMsgContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	const UserSysMsgInfo* se=ServerDateManager::Get()->getUserSysMsgEntityByID(getContentID());

	if(!se)
	{
		return;
	}

	unsigned int operatorType;
	bool disposeMsg=true;
	if(itemName=="onRefuseButton")
	{
		operatorType=REFUSE;
	}
	else if(itemName=="onAgreeButton")
	{
		operatorType=AGREE;
	}
	else if(itemName=="onLeaveMsgButton")
	{
		disposeMsg=false;
		unsigned int _id=0;
		std::string _name="";
		if(!mData["p"].empty())
		{
			_id=mData["p"].asUInt();
		}
		if(!mData["n"].empty())
		{
			_name=mData["n"].asString();
		}
		MsgMainFramePushPage msg;
		msg.pageName="LeaveMsgPage";
		MessageManager::Get()->sendMessage(&msg);
		MsgLeaveMsgReceivePage msgReceive;
		msgReceive.uID = _id;
		msgReceive.uName = _name;
		MessageManager::Get()->sendMessage(&msgReceive);
	}
	else if(itemName=="onRankingButton")
	{
		operatorType=COUNTERATTACK;
		MsgMainFrameChangePage msg;
		msg.pageName="ArenaPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onGongfuButton")
	{
		if(ServerDateManager::Get()->getUserBasicInfo().vitality>1)
		{
			int _pId=0;
			SkillBookLootInfo* bootLoot=ServerDateManager::Get()->getAndCreatSkillBookLootInfoById(0);
			if(!mData["p"].empty())
			{
				_pId=mData["p"].asInt();
			}
			if(!mData["sb"].empty()&&!mData["s"].empty())
			{
				BlackBoard::Get()->setSkillBookInfo(mData["sb"].asInt(),mData["s"].asInt());
			}
			if(_pId!=0)
			{
				std::string _fightEnemyNameKey="fightEnemyNameKey";
				if(HAS_V(_fightEnemyNameKey))
				{
					SET_V(_fightEnemyNameKey,se->name);
				}
				else
				{
					ADD_V(_fightEnemyNameKey,se->name);
				}
				BasePage* bp = CCBManager::Get()->getPage("FightPage");
				if(bp)
				{
					FightPage* fp = dynamic_cast<FightPage*>(bp);
					fp->setViewTeamStatus(_pId,3,0);
				}
				bootLoot->id=0;
				bootLoot->playerId=_pId;
				bootLoot->playerName=se->name;
				bootLoot->playerLevel=1;
				bootLoot->isNpc=2;
				BlackBoard::Get()->setSnatchSkillBookPlayerId(0);
			}
			else
			{
				MSG_BOX("COUNTERATTACK Error !");
				return;
			}
		}
		else
		{
			POP_NOT_ENOUGH_BOX(VitalityNotEnough);
			return;
		}
		operatorType=COUNTERATTACK;
	}
	else if(itemName=="onRecieve")
	{
		disposeMsg=false;
		if(se->sysMsgType==SYSTEM_FIGHTEND||se->sysMsgType==SYSTEM||se->sysMsgType==SYSTEM_WORDBOSS)
		{
			if(se->oprType==0)
			{
				disposeMsg=true;
				operatorType=1;
				MsgReqType(true);
			}
		}
		if(!disposeMsg)
		{
			MSG_BOX(Language::Get()->getString("@SMReceiveClickFail"));
		}
	}
	else if(itemName=="onAddEnemy")
	{
		OPDisposeFriendsInfo info;
		info.set_version(1);
		info.set_id(se->relationId);
		info.set_oprtype(5);
		info.set_friendtype(2);
		PacketManager::Get()->sendPakcet(OPCODE_DISPOSE_USER_FRIENDS_INFO_C,&info);
		ServerDateManager::Get()->getAndCreatSysMsgEntity(getContentID())->addEnemyType=1;
		refreshContent();
		return;
	}
	if(disposeMsg)
	{
		OPDisposeSysMsg info;
		info.set_version(1);
		info.set_id(se->relationId);
		info.set_oprtype(operatorType);
		info.set_msgtype(se->sysMsgType);
		PacketManager::Get()->sendPakcet(OPCODE_DISPOSE_USER_SYSMSG_INFO_C,&info);
		ServerDateManager::Get()->getAndCreatSysMsgEntity(getContentID())->oprType=operatorType;
		refreshContent();
	}
}

SysMsgPage::SysMsgPage(void)
{
	mCurrTabIndex = 4;
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
	isReqData=false;
	mExpbarInitScale = -1.0f;
}


SysMsgPage::~SysMsgPage(void)
{
	isReqData=false;
}


void SysMsgPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	mSysMsgTabList.clear();

	for(unsigned int i=2;i<=SYSMSGTABLENGTH;++i)
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
		CCMenuItemSprite* tabSprite =  dynamic_cast<cocos2d::CCMenuItemSprite* >(obj);
		if(tabSprite)
		{
			mSysMsgTabList.insert(TAB_BUTTON_LIST::value_type(i,tabSprite));
		}
	}
	loadSysMsgConfig();
	_refreshPage();
}

void SysMsgPage::loadSysMsgConfig()
{
	mCurrTabIndex=TABSYSTEM;
	if(ServerDateManager::Get()->getUserBasicInfo().unReadCount!=0&&isReqData)
	{
		OPRecordReadMsgTime info;
		info.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_RECORDREADMSGTIME_C,&info);
	}
	ServerDateManager::Get()->getUserBasicInfo().unReadCount=0;
	ServerDateManager::Get()->getUserBasicInfo().readTime=ServerDateManager::Get()->getUserBasicInfo().servertime;
	if(ServerDateManager::Get()->getUserSysMsgEntityListTotalNum()==0&&!isReqData)
	{
		isReqData=true;
		OPUserSysMsgInfo info;
		info.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_USER_SYSMSG_INFO_C,&info);
	}
	else
	{
		switchTab();
	}
}

void SysMsgPage::switchTab()
{
	TAB_BUTTON_LIST::iterator it=mSysMsgTabList.begin();
	for(;it!=mSysMsgTabList.end();++it)
	{
		CCMenuItemSprite* tabSprite=it->second;
		if(mCurrTabIndex!=it->first)
		{
			tabSprite->unselected();
		}
		else
		{
			tabSprite->selected();
		}
	}

	rebuildAllItem();
	if(mCurrTabIndex>SYSMSGTABLENGTH)
	{
		mCurrTabIndex=TABSYSTEM;
	}
}

void SysMsgPage::buildItem( void )
{
	UserSysMsgEntityList* itmap=ServerDateManager::Get()->cloneSysMsgEntityList(mCurrTabIndex);
	UserSysMsgEntityList::reverse_iterator it=itmap->rbegin();
	for(;it!=itmap->rend();it++)
	{
		unsigned int id =(*it)->second->id;
		SysMsgContent * item = new SysMsgContent(id);
		item->init();
		addItem(item);
	}
}

void SysMsgPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((membersRootNode->getContentSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void SysMsgPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void SysMsgPage::clearAllItem( void )
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

void SysMsgPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void SysMsgPage::removeItem( ContentBase* item )
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

void SysMsgPage::refreshAllContentItem()
{
	ContentList::iterator it=mContentList.begin();
	for(int i=0;it!=mContentList.end();++it,++i)
	{
		(*it)->getContainerNode()->setPosition(0,i*(*it)->getContainerNode()->getContentSize().height);
		members->setContentSize(CCSize(members->getContentSize().width,i*(*it)->getContainerNode()->getContentSize().height));
	}
}

void SysMsgPage::Exit( MainFrame* frame)
{
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
}

void SysMsgPage::Execute( MainFrame* )
{

}

void SysMsgPage::load( void )
{
	loadCcbiFile("SysMsgPage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
	membersRootNode->removeAllChildren();
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();

}

void SysMsgPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
}

void SysMsgPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	bool toggleTab=true;
	if(itemName=="onToggleALLButton")
	{
		mCurrTabIndex=1;	
	}
	else if(itemName=="onToggleFightButton")
	{
		mCurrTabIndex=2;	
	}
	else if(itemName=="onToggleFriendsButton")
	{
		mCurrTabIndex=3;	
	}
	else if(itemName=="onToggleSysButton")
	{
		mCurrTabIndex=4;	
	}
	else if(itemName=="onPlayerInfo")
	{
		toggleTab=false;
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	if(toggleTab)
	{
		switchTab();
	}
}

void SysMsgPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_DISPOSE_USER_SYSMSG_INFORET_S)
		{
			if(mIsReward)
			{
				MSG_BOX(Language::Get()->getString("@SMRewardOk"));
			}
			_refreshPage();
		}
		else if(msg->opcode==OPCODE_GET_USER_SYSMSG_INFORET_S)
		{
			loadSysMsgConfig();
		}
		else if(msg->opcode==OPCODE_GET_USER_BASIC_INFORET_S)
		{
			_refreshPage();
		}
		else if(msg->opcode==OPCODE_SNATCH_SKILL_BOOK_INFORET_S)
		{
			_refreshPage();
		}
	}
	else if(message->getTypeId()==MSG_FIGHT_END)
	{
		std::string key="snatchSkillBookRetKey";
		unsigned int ret=GET_V_UINT(key);
		if(ret==1)
		{
			MsgMainFramePushPage pushMsg;
			pushMsg.pageName = "ObtainSkillBookPage";
			MessageManager::Get()->sendMessage(&pushMsg);
		}
		else
		{
			if(ret==0)
			{
				POP_BOX(SnatchSkillBookFightWin);
			}
			else
			{
				POP_BOX(SnatchSkillBookFightFail);
			}
		}
	}
	mIsReward=false;
}

void SysMsgPage::_refreshPage( void )
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
		}else
		{
			vipLevel->setVisible(false);
		}
	}
}