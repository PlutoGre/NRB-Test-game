
#include "stdafx.h"

#include "TeamBuffPage.h"
REGISTER_PAGE("TeamBuffPage",TeamBuffPage);
USING_NS_CC;

void TeamBuffContent::refreshContent( void )
{
	mBuffPic=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mBuffPic"));
	mBuffTypePic=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mBuffAtkTypePic"));
	mBuffName=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mBuffName"));
	mNowNum=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mNowNum"));
	mMaxNum=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mMaxNum"));
	mBuffLV=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mBuffLV"));
	mBuffValue=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mBuffValue"));
	mTeamBuffStats=ServerDateManager::Get()->getTeamBuffInfoById(getContentID());
	if(mTeamBuffStats)
	{
		mTeamBuffItem=TeamBuffTableManager::Get()->getTeamBuffItemByID(mTeamBuffStats->teamId);
	}
	
	if(mTeamBuffStats&&mTeamBuffItem)
	{
		_refreshPage();
	}
}

void TeamBuffContent::_refreshPage()
{
	if(mNowNum)
	{
		mNowNum->setString(StringConverter::toString(mTeamBuffStats->currBoldness).c_str());
	}
	if(mMaxNum)
	{
		mMaxNum->setString(StringConverter::toString(TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(mTeamBuffStats->teamId,mTeamBuffStats->teamLevel)).c_str());
	}
	if(mBuffLV)
	{
		mBuffLV->setString(StringConverter::toString(mTeamBuffStats->teamLevel).c_str());
	}
	if(mBuffValue)
	{
		mBuffValue->setString(StringConverter::toString(TeamBuffTableManager::Get()->getAdditionValueByIdAndLevel(mTeamBuffStats->teamId,mTeamBuffStats->teamLevel)).c_str());
	}
	if(mBuffName)
	{
		mBuffName->setString(mTeamBuffItem->name.c_str());
	}
	if(mBuffPic)
	{
		mBuffPic->setTexture(mTeamBuffItem->iconPic.c_str());
		//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
		//mBuffPic->setTextureRect(rect);
	}
	if(mBuffTypePic)
	{
		mBuffTypePic->setTexture(mTeamBuffItem->namePic.c_str());
		//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
		//mBuffTypePic->setTextureRect(rect);
	}
};

void TeamBuffContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onUpgrade")
	{
		TeamBuffUpgrade(mTeamBuffItem->buffType);
	}
}

cocos2d::CCObject * TeamBuffContent::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getContainerNode()->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

TeamBuffPage::TeamBuffPage(void)
{
	
}


TeamBuffPage::~TeamBuffPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void TeamBuffPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	loadData();
}

void TeamBuffPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void TeamBuffPage::Execute( MainFrame* )
{
	if(BlackBoard::Get()->getNeedRefreshTeamBuffPage())
	{
		refreshAllItem();
		BlackBoard::Get()->setNeedRefreshTeamBuffPage(false);
	}
}

cocos2d::CCObject* TeamBuffPage::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

void TeamBuffPage::loadData()
{
	if(ServerDateManager::Get()->getTeamBuffInfoTotalSum()<=0||_lastLevel!=ServerDateManager::Get()->getUserBasicInfo().level)
	{
		OPTeamBuffInfo msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_TEAM_BUFF_INFO_C, &msg);
	}
	else
	{
		rebuildAllItem();
	}
}

void TeamBuffPage::load( void )
{
	loadCcbiFile("TeamBuff.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
}

void TeamBuffPage::buildItem( void )
{
	TeamBuffInfoMapIterator it=ServerDateManager::Get()->getTeamBuffInfoMapIterator();
	std::list<unsigned int > _list;
	while(it.hasMoreElements())
	{
		unsigned int id = it.getNext()->teamId;
		_list.push_back(id);
	}
	_list.sort(&TeamBuffPage::cmp);
	std::list<unsigned int >::iterator itl=_list.begin();
	for(;itl!=_list.end();++itl)
	{
		TeamBuffContent * item = new TeamBuffContent(*itl);
		item->init();
		addItem(item);
	}

}

bool TeamBuffPage::cmp(unsigned int a,unsigned int b)
{
	return a>b;
}

void TeamBuffPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((members->getViewSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void TeamBuffPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void TeamBuffPage::clearAllItem( void )
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

void TeamBuffPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void TeamBuffPage::removeItem( ContentBase* item )
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

void TeamBuffPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onBack")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "SkillBookLootPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void TeamBuffPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

void TeamBuffPage::onReceiveMassage(const GameMessage * message)
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_GET_TEAM_BUFF_INFORET_S)
			{
				_lastLevel=ServerDateManager::Get()->getUserBasicInfo().level;
				rebuildAllItem();
				refreshPage();
			}
		}
	}
}

void TeamBuffPage::refreshPage( void )
{

}