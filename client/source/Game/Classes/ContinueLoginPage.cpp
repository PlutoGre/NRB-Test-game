
#include "stdafx.h"

#include "ContinueLoginPage.h"
#include "MessageBoxPage.h"
#include "DataTableManager.h"
REGISTER_PAGE("ContinueLoginPage",ContinueLoginPage);
USING_NS_CC;

CardBase::CardBase( unsigned int id )
	:mID(id)
{

}

CardBase::~CardBase()
{
	mContentContainer->release();
}

void CardBase::init(void)
{
	mContentContainer = CCBContainer::create();
	mContentContainer->loadCcbiFile(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	refreshContent();
}

void CardContent::refreshContent( void )
{
	CCObject* hideCardObj=getContainerNode()->getVariable("mCardHide");
	if(!hideCardObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mCardHide");
		CCMessageBox(errMsg,"error");
	}
	mCardHide=dynamic_cast<cocos2d::CCSprite* >(hideCardObj);

	CCObject* showCardObj=getContainerNode()->getVariable("mCardShow");
	if(!showCardObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mCardShow");
		CCMessageBox(errMsg,"error");
	}
	mCardShow=dynamic_cast<cocos2d::CCSprite* >(showCardObj);

	if(mCardShow)
	{
		mCardShow->setVisible(false);
	}

	CCObject* iconObj=getContainerNode()->getVariable("mIcon");
	if(!iconObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mIcon");
		CCMessageBox(errMsg,"error");
	}
	mCardIcon=dynamic_cast<cocos2d::CCSprite* >(iconObj);

	if(mCardIcon)
	{
		mCardIcon->setVisible(true);
	}

	CCObject* quantityObj=getContainerNode()->getVariable("mQuantity");
	if(!quantityObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mQuantity");
		CCMessageBox(errMsg,"error");
	}
	else
	{
		mQuantity=dynamic_cast<cocos2d::CCLabelBMFont* >(quantityObj);
		//mQuantity->setVisible(false);
	}

	CCObject* mask=getContainerNode()->getVariable("mCardMask");
	if(!mask)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mCardMask");
		CCMessageBox(errMsg,"error");
	}
	mMask=dynamic_cast<cocos2d::CCMenuItemImage* >(mask);

	CCObject* frame=getContainerNode()->getVariable("mFrame");
	if(!frame)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mFrame");
		CCMessageBox(errMsg,"error");
	}
	mFrame=dynamic_cast<cocos2d::CCMenuItemImage* >(frame);
	mCardNode=dynamic_cast<cocos2d::CCNode* >(getContainerNode()->getVariable("mCard"));
	if(mCardNode)
	{
		mCardNode->setVisible(false);
	}
	if(mMask)
	{
		//mMask->setVisible(false);
	}
	if(mFrame)
	{
		mFrame->setVisible(false);
	}
	isFlip=false;
}

void CardContent::flipSprite(const unsigned int id,const unsigned count,bool isSelect)
{
	isFlip=true;
	const ContinueLoginInfo::ContinueReward* item=ServerDateManager::Get()->getContinueRewardByIndex(id);
	const ResInfo* ri=ResManager::Get()->getResInfoByTypeAndId(item->itemType,item->itemId,item->quantity);
	mMask->setVisible(false);
	mFrame->setVisible(false);
	char key[128];
	sprintf(key,"ImageFileQuality%d",ri->quality);
	//cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile").c_str());
	//sprite->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting(key)));
	//mFrame->setNormalImage(sprite);
	//mFrame->setSelectedImage(sprite);
	mIsSelect=isSelect;
	if(mIsSelect)
	{
		mMask->selected();
	}
	mMask->setEnabled(false);
	if(mQuantity)
	{
		mQuantity->setVisible(true);
		mQuantity->setString(StringConverter::toString(count).c_str());
	}
	mCardIcon->initWithFile(ri->icon.c_str());
	if(ResManager::Get()->getResMainType(ri->type)==SOUL_TYPE)
	{
		mCardIcon->setScale(1.0f);
	}
	mCardHide->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 0 , 90 , 0, 0),
	CCCallFuncN::create(this->mCardHide , callfuncN_selector(CardContent::hideReset) ), CCOrbitCamera::create(1 , 1 , 0 , 90, 90 , 0, 0), NULL) );
	mCardShow->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 0 , 90 , 0, 0),
		CCCallFuncN::create(this->mCardShow , callfuncN_selector(CardContent::showReset) ) , CCOrbitCamera::create(1 , 1 , 0 , 90, 90 , 0, 0), NULL) );
	mCardNode->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 180 , 180 , 0, 0),
		CCCallFuncN::create(this->mCardNode , callfuncN_selector(CardContent::showReset) ),  CCOrbitCamera::create(1 , 1 , 0 , 180 , 180 , 0, 0),NULL) );
	if(mIsSelect)
	{
		mMask->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 0 , 135 , 0, 0),
			CCCallFuncN::create(this->mMask , callfuncN_selector(CardContent::hideReset)), CCOrbitCamera::create(1 , 1 , 0 , 90, 90 , 0, 0), NULL) );
		if(mFrame)
		{
			mFrame->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 0 , 135 , 0, 0),
				CCCallFuncN::create(this->mFrame , callfuncN_selector(CardContent::showReset)) , CCOrbitCamera::create(1 , 1 , 0 , 90, 90 , 0, 0), NULL) );
		}
	}
	else
	{
		mMask->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 0 , 135 , 0, 0),
			CCCallFuncN::create(this->mMask , callfuncN_selector(CardContent::showReset)), CCOrbitCamera::create(1 , 1 , 0 , 90, 90 , 0, 0), NULL) );
		if(mFrame)
		{
			mFrame->runAction(CCSequence::create( CCOrbitCamera::create(1 , 1 , 0 , 0 , 135 , 0, 0),
				CCCallFuncN::create(this->mFrame , callfuncN_selector(CardContent::hideReset)) , CCOrbitCamera::create(1 , 1 , 0 , 90, 90 , 0, 0), NULL) );
		}
	}
	
}

void CardContent::hideReset(CCNode* sender)
{
	if(sender)
	{
		sender->setVisible(false);
	}
}

void CardContent::showReset(CCNode* sender)
{
	if(sender)
	{
		sender->setVisible(true);
	}
}

void CardContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onShowRewardButton")
	{
		if(mIsSelect)
		{
			mMask->selected();
		}
		else
		{
			mMask->unselected();
		}
		if(!isFlip)
		{	
			MsgContinueLoginFlipCard msgReceive;
			msgReceive.index = getContentID();
			MessageManager::Get()->sendMessage(&msgReceive);
		}
	}
}

ContinueLoginPage::ContinueLoginPage(void)
{
	mCardLength=9;
}


ContinueLoginPage::~ContinueLoginPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
	CCDirector::sharedDirector()->setDepthTest(true);
}

void ContinueLoginPage::Enter()
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_CONTINUE_LOGIN_FLIPCARD,this);
	CCDirector::sharedDirector()->setDepthTest(false);
	initPanel();
}

void ContinueLoginPage::initPanel(void)
{
	CCB_FUNC(this,"mXClose",CCMenuItemImage,setVisible(false));
	mRewardCount=0;
	_clearAllItem();
	continueLoginInfo=ServerDateManager::Get()->getContinueLoginInfo();
	CCObject* obj=getVariable("mContent");
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mContent");
		CCMessageBox(errMsg,"error");
	}
	mCardNode =  dynamic_cast<cocos2d::CCNode* >(obj);
	CCObject* objDetail=getVariable("mContinueLoginDetail");
	if(!objDetail)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mContinueLoginDetail");
		CCMessageBox(errMsg,"error");
	}
	cocos2d::CCNode* detailNode=dynamic_cast<cocos2d::CCNode*>(objDetail);
	detailNode->retain();
	mNodeList.insert(NodeList::value_type(1,detailNode));
	CCObject* objClosePanel=getVariable("mClosePanel");
	if(!objClosePanel)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mClosePanel");
		CCMessageBox(errMsg,"error");
	}
	cocos2d::CCNode* closeNode=dynamic_cast<cocos2d::CCNode*>(objClosePanel);
	closeNode->setVisible(false);
	closeNode->retain();
	mNodeList.insert(NodeList::value_type(2,closeNode));

	for(int i=1;i<=DAYCOUNT;++i)
	{
		char cName[128];
		sprintf(cName,"mContinue%d",i);
		CCObject* continueObj=getVariable(cName);
		if(!continueObj)
		{
			char errMsg[256];
			sprintf(errMsg,"Failed to find variable:%s",cName);
			CCMessageBox(errMsg,"error");
		}
		else
		{
			cocos2d::CCSprite* continueSprite=dynamic_cast<cocos2d::CCSprite* >(continueObj);
			continueSprite->retain();
			mContinueDaySpriteList.insert(SpriteList::value_type(i,continueSprite));
		}
		
		char dName[128];
		sprintf(dName,"mDay%d",i);
		CCObject* dayObj=getVariable(dName);
		if(!dayObj)
		{
			char errMsg[256];
			sprintf(errMsg,"Failed to find variable:%s",dName);
			CCMessageBox(errMsg,"error");
		}
		else
		{
			cocos2d::CCSprite* daySprite=dynamic_cast<cocos2d::CCSprite* >(dayObj);
			daySprite->retain();
			mDaySpriteList.insert(SpriteList::value_type(i,daySprite));
		}
		
	}
	refreshContinueSymbol();
	buildItem();
}

void ContinueLoginPage::refreshContinueSymbol()
{
	SpriteList::iterator cdItr=mContinueDaySpriteList.begin();
	for(int i=1;cdItr!=mContinueDaySpriteList.end();++cdItr,++i)
	{
		if(i<=continueLoginInfo.count)
		{
			cdItr->second->setVisible(true);
		}
		else
		{
			cdItr->second->setVisible(false);
		}
	}

	SpriteList::iterator dItr=mDaySpriteList.begin();
	for(int i=1;dItr!=mDaySpriteList.end();++dItr,++i)
	{
		if(i<=continueLoginInfo.count)
		{
			dItr->second->setVisible(false);
		}
		else
		{
			dItr->second->setVisible(true);
		}
	}
}

void ContinueLoginPage::buildItem( void )
{
	for(int i=1;i<10;++i)
	{
		CardContent* item=new CardContent(i);
		item->init();
		addItem(i,item);
	}
}

void ContinueLoginPage::addItem(unsigned int index, CardContent* item )
{
	mContinueCard.insert(ContentList::value_type(index,item));
	mCardNode->addChild(item->getContainerNode());
	int wIndex=ceil(mContinueCard.size()-3*ceil((float)(mContinueCard.size()/3)));
	int hIndex=ceil((float)mContinueCard.size()/3)-1;
	float relativeW=(mCardNode->getContentSize().width-item->getContainerNode()->getContentSize().width*3)/4;
	float relativeH=(mCardNode->getContentSize().height-item->getContainerNode()->getContentSize().height*3)/4;
	CCPoint pos = ccp((item->getContainerNode()->getContentSize().width)*wIndex+(relativeW*(wIndex+1)),(item->getContainerNode()->getContentSize().height+relativeH)*(hIndex));
	item->getContainerNode()->setPosition(pos);
}

void ContinueLoginPage::flipItemByIndex(const unsigned int index)
{
	if(mRewardCount<continueLoginInfo.count)
	{
		unsigned int id=ServerDateManager::Get()->getContinueRewardByIndex(mRewardCount)->id;
		if(id!=-1)
		{
			mRewardMap.insert(IntMap::value_type(id,index));
			mClickList.insert(IntMap::value_type(index,id));
			CardContent* cardContent=getItemByIndex(index);
			unsigned int count=ServerDateManager::Get()->getContinueRewardByIndex(id)->quantity;
			cardContent->flipSprite(id,count,true);
			mRewardCount++;
		}
	}
	if(mRewardCount>=continueLoginInfo.count)
	{
		generateContinueReward();
		ContentList::reverse_iterator it=mContinueCard.rbegin();
		for(;it!=mContinueCard.rend();it++)
		{
			if(mClickList.find(it->first)!=mClickList.end())
			{
				continue;
			}
			else
			{
				it->second->flipSprite(mRewardCount,ServerDateManager::Get()->getContinueRewardByIndex(mRewardCount)->quantity);
				mRewardCount++;
			}
		}
		mNodeList.find(1)->second->setVisible(false);
		mNodeList.find(2)->second->setVisible(true);
		OPRewardContinueLogin info;
		info.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_REWARD_CONTINUE_LOGIN_C,&info);
	}
}

CardContent* ContinueLoginPage::getItemByIndex(const unsigned int index)
{
	return mContinueCard.find(index)->second;
}

void ContinueLoginPage::Exit( MainFrame* frame)
{
	_clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
	CCDirector::sharedDirector()->setDepthTest(false);
}

void ContinueLoginPage::_clearAllItem()
{
	mClickList.clear();
	mRewardMap.clear();
	ContentList::iterator it=mContinueCard.begin();
	for(;it!=mContinueCard.end();++it)
	{
		if(it->second)
		{
			delete it->second;
		}
	}
	mContinueCard.clear();
	NodeList::iterator itNL=mNodeList.begin();
	for(;itNL!=mNodeList.end();++itNL)
	{
		if(itNL->second)
			itNL->second->release();
	}
	mNodeList.clear();
	SpriteList::iterator itCS=mContinueDaySpriteList.begin();
	for(;itCS!=mContinueDaySpriteList.end();++itCS)
	{
		if(itCS->second)
			itCS->second->release();
	}
	mContinueDaySpriteList.clear();
	SpriteList::iterator itDS=mDaySpriteList.begin();
	for(;itDS!=mDaySpriteList.end();++itDS)
	{
		if(itDS->second)
			itDS->second->release();
	}
	mDaySpriteList.clear();
}

void ContinueLoginPage::Execute( MainFrame* )
{

}

void ContinueLoginPage::load( void )
{
	loadCcbiFile("ContinueLoginPage.ccbi");
	Enter();
}

void ContinueLoginPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onRewardContinueLogin" || itemName == "onClose")
	{
		MsgMainFramePopPage msgPop;
		msgPop.pageName = "ContinueLoginPage";
		MessageManager::Get()->sendMessage(&msgPop);
		if(VaribleManager::Get()->getSetting("AnnouncementPageOpenStatus")=="1")
		{
			MsgMainFramePushPage msg;
			msg.pageName = "AnnouncementPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
}

void ContinueLoginPage::generateContinueReward()
{
	int maxId=ServerDateManager::Get()->getContinueLoginInfoMaxIndex()+1;
	typedef std::vector<unsigned int > idPoolVec;
	typedef std::map<unsigned int,idPoolVec > PoolMap;
	PoolMap poolMap;
	for(int i=maxId;i<=mCardLength;++i)
	{
		idPoolVec pool=idPoolVec();
		poolMap.insert(PoolMap::value_type(i,pool));
	};
	ContinueLoginTableManager::ContinueListIterator it=ContinueLoginTableManager::Get()->getContinueLoginInfoIterator();
	while(it.hasMoreElements())
	{
		const ContinueLoginItem* item=it.getNext();
		if(item->poolIndex>=maxId)
		{
			poolMap.find(item->poolIndex)->second.push_back(item->id);
		}
	}
	for(;maxId<=mCardLength;++maxId)
	{
		idPoolVec pool=poolMap.find(maxId)->second;
		int maxNum=pool.size();
		idPoolVec::iterator it=pool.begin()+rand()%maxNum;
		const ContinueLoginItem* item=ContinueLoginTableManager::Get()->getContinueLoginItemByID(*it);
		ContinueLoginInfo::ContinueReward* contiReward=ServerDateManager::Get()->_getContinueReward(maxId-1);
		contiReward->itemType=item->itemType;
		if(item->itemId==USER_PROPERTY_LEVEL_SILVER_COINS)
		{
			const PlayerGradeItem* gradeItem = PlayerGradeTableManager::Get()->getGradeItemByID(ServerDateManager::Get()->getUserBasicInfo().level);
			if(gradeItem)
			{
				contiReward->quantity=item->quantity*gradeItem->levelSilverCoins;
			}
		}
		else
		{
			contiReward->quantity=item->quantity;
		}
		
		int type=ResManager::Get()->getResMainType(item->itemType);
		switch(type)
		{
			case DISCIPLE_TYPE:contiReward->itemId=DiscipleTableManager::Get()->getRandomDiscipleId();break;
			case SOUL_TYPE:contiReward->itemId=DiscipleTableManager::Get()->getRandomDiscipleId();break;
			case SKILL_TYPE:contiReward->itemId=SkillTableManager::Get()->getRandomSkillId();break;
			case EQUIP_TYPE:contiReward->itemId=EquipTableManager::Get()->getRandomEquipId();break;
			default:contiReward->itemId=item->itemId;break;
		}
	}
}

void ContinueLoginPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void ContinueLoginPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_CONTINUE_LOGIN_FLIPCARD)
	{
		const MsgContinueLoginFlipCard * msg = dynamic_cast<const MsgContinueLoginFlipCard*>(message);
		if(msg)
		{
			flipItemByIndex(msg->index);
		}
	}
	else if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate* >(message);
		if(msg->opcode==OPCODE_REWARD_CONTINUE_LOGINRET_S)
		{

		}
		else if(msg->opcode==OPCODE_GET_CONTINUE_LOGINRET_S)
		{
			//generateContinueReward();
		}
	}
}
