
#include "stdafx.h"

#include "AdventurePage.h"
#include "AdventureContinuePage.h"
#include "FightEndPopupPage.h"
REGISTER_PAGE("AdventurePage",AdventurePage);
USING_NS_CC;

void NavContent::refreshContent( void )
{
	CCObject* buttonObj=getContainerNode()->getVariable("mFrameBack");
	if(!buttonObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mFrameBack");
		CCMessageBox(errMsg,"error");
	}
	mButton=dynamic_cast<cocos2d::CCMenuItemImage* >(buttonObj);
	if(mButton && mButton->getParent())
	{
		CCMenu* menu = dynamic_cast<CCMenu*>(mButton->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
		CCB_FUNC(getContainerNode(),"mFrameBack",CCMenuItemImage,setDisabledImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())));
		CCB_FUNC(getContainerNode(),"mFrameBack",CCMenuItemImage,setSelectedImage(cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("AdventureIconFrameSelected_ImageFile").c_str())))
		if(mButton->getSelectedImage()->getContentSize().width>mButton->getNormalImage()->getContentSize().width)
		{
			bool isNeedSetAnchor=StringConverter::parseBool(VaribleManager::Get()->getSetting("ButtonFrameAnchorPoint"),false);
			if(isNeedSetAnchor)
			{
				float scale=(mButton->getSelectedImage()->getContentSize().width-mButton->getNormalImage()->getContentSize().width)*0.5/mButton->getSelectedImage()->getContentSize().width;
				mButton->getSelectedImage()->setAnchorPoint(ccp(scale,scale));
				mButton->getDisabledImage()->setAnchorPoint(ccp(scale,scale));
			}
		}
	}

	cocos2d::CCSprite* icon=dynamic_cast<cocos2d::CCSprite* >(getContainerNode()->getVariable("mFrame"));
	const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(ServerDateManager::Get()->getAdventureItemInfoByID(getContentID())->adventureId);
	unsigned int _quality=4;
	if(adventureItem)
	{
		if(adventureItem->adventureType==SkillBook)
		{
			const SkillItem* skill=SkillTableManager::Get()->getSkillItemByID(ServerDateManager::Get()->getAdventureItemInfoByID(getContentID())->itemId);
			if(skill)
			{
				if(icon)
				{
					icon->setTexture(skill->iconPic.c_str());
					icon->setScale(0.32f);
				}
				cocos2d::CCLabelBMFont* name=dynamic_cast<cocos2d::CCLabelBMFont* >(getContainerNode()->getVariable("mName"));
				if(name)
				{
					name->setString(skill->name.c_str());
				}
				_quality=skill->quality;
			}
		}
		else
		{
			if(icon)
			{
				icon->setTexture(adventureItem->iconPic.c_str());
			}
			cocos2d::CCLabelBMFont* name=dynamic_cast<cocos2d::CCLabelBMFont* >(getContainerNode()->getVariable("mName"));
			if(name)
			{
				if(adventureItem->name=="none")
				{
					name->setString("");
				}
				else
				{
					name->setString(adventureItem->name.c_str());
				}
			}
		}
		CCB_FUNC(getContainerNode(),"mNavNew",CCNode,setVisible(isShowActivityTip()));
		if(_quality>4||_quality<=0)
		{
			_quality=4;
		}
		if(mButton)
		{
			mButton->setNormalImage(getFrameNormalSpirte(_quality));
		}
	}
}

void NavContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

}

void NavContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	MsgAdventurePageChange msg;
	msg.index = getContentID();
	MessageManager::Get()->sendMessage(&msg);
}

void NavContent::loadData()
{

}

void NavContent::updatePage(bool isSelect,float dt)
{
	mButton->setEnabled(!isSelect);
	if(!mButton->isEnabled())
	{
		if(isShowActivityTip())
		{
			unsigned int _serverVersion=ServerDateManager::Get()->getAdventureItemInfoByID(getContentID())->version;
			_saveActivityGuideStatus(_getTipSaveKey(),_serverVersion);
			CheckAdventureGuide();
		}
		CCB_FUNC(getContainerNode(),"mNavNew",CCNode,setVisible(false));
	}
}

void NavContent::exitPage()
{

}

bool NavContent::isShowActivityTip()
{
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		bool _isShow=false;
		//check config is need show tip
		const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(ServerDateManager::Get()->getAdventureItemInfoByID(getContentID())->adventureId);
		if(adventureItem->checkVersion!=1)
		{
			return false;
		}
		//check server is have version
		unsigned int _serverVersion=ServerDateManager::Get()->getAdventureItemInfoByID(getContentID())->version;
		if(_serverVersion==0||_serverVersion>99999)
		{
			return false;
		}
		
		unsigned int _localVersion=cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(_getTipSaveKey().c_str(),0);
		if(_serverVersion>_localVersion)
		{
			return true;
		}
		return false;
	}
	return false;
}

const std::string NavContent::_getTipSaveKey()
{
	//char is serverId_tip_adventure_adventureId,eg.1_tip_adventure_1
	char str[128];
	sprintf(str,"s%d_tip_adventure_%d",GamePrecedure::Get()->getServerID(),ServerDateManager::Get()->getAdventureItemInfoByID(getContentID())->adventureId);
	return str;
}

void NavContent::_saveActivityGuideStatus(std::string _key,unsigned int _version)
{
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		//char is sserverId_tip_adventure_adventureId,eg.1_tip_adventure_1
		cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(_key.c_str(),_version);
		cocos2d::CCUserDefault::sharedUserDefault()->flush();
	}	
}

AdventurePage::AdventurePage(void)
{
	mIsInitData=false;
	mCurrIndex=0;
	mNavContentNode = NULL;
	mPageContentNode = NULL;
}


AdventurePage::~AdventurePage(void)
{
	mIsInitData=false;
	mCurrIndex=0;
	mNavContentNode = 0;
	mPageContentNode = 0;
	clearAllItem();
}


void AdventurePage::Enter( MainFrame* frame)
{
	//mCurrIndex = 0;
	initPage();
	if(mPageContent)
	{
		mPageContent->setDelegate(this);
		mPageContent->setBounceable(false);
		mPageContent->setScrollDeaccelRate(0);
	}
}

void AdventurePage::initPage()
{
	MessageManager::Get()->regisiterMessageHandler(MSG_ADVENTURE_PAGECHANGE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_ADVENTURE_REMOVEITEM,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	if(!mIsInitData&&ServerDateManager::Get()->getAdventureItemInfoMapTotalNum()==0)
	{
		OPAdventureInfo msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_ADVENTURE_INFO_C, &msg);
	}
	else
	{
		rebuildAllItem();
	}
}

void AdventurePage::buildItem( void )
{
	mContentSize = CCSize(0,0);
	mNavSize= CCSize(0,0);
	std::vector<unsigned int > skillList;
	AdventureItemInfoMapIterator itr = ServerDateManager::Get()->getAdventureItemInfoMapIterator();
	int i=0;
	while(itr.hasMoreElements())
	{
		const AdventureItemInfo* adventureItemInfo=itr.getNext(); 
		unsigned int id = adventureItemInfo->id;
		unsigned int adventureId = adventureItemInfo->adventureId;
		const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(adventureId);
		bool isAddItem=true;
		if(adventureItem!=NULL)
		{
			if(adventureItem->adventureType!=SkillBook)
			{
				if(GamePrecedure::Get()->getServerTime()<adventureItemInfo->beginTime||GamePrecedure::Get()->getServerTime()>adventureItemInfo->endTime)
				{
					continue;
				}
			}
			if(adventureItem->adventureType==Power)
			{
				AdventureBase* item=new AddPowerPage(id);
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==WordBoss)
			{
				if(ServerDateManager::Get()->getUserBasicInfo().level >= StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossOpenLevel")))
				{
					AdventureBase* item=new WorldBossPage(id);
					item->init();
					addContentItem(i,id,item);
				}
				else
				{
					continue;
				}
			}
			else if(adventureItem->adventureType==Wish)
			{
				AdventureBase* item=new WishPage(id);
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==Teach)
			{
				AdventureBase* item=new TeacherTeachPage(id);
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==FightEnd)
			{
				if(ServerDateManager::Get()->getBattleArraySize()>=5)
				{
					AdventureBase* item=new FightEndPage(id);
					item->init();
					addContentItem(i,id,item);
				}
				else
				{
					continue;
				}
			}
			else if(adventureItem->adventureType==BoundlessHole)
			{
				AdventureBase* item=new BoundlessHolePage(id);
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==SkillBook)
			{
				skillList.push_back(id);
				continue;
			}
			else if(adventureItem->adventureType==LevelUpReward)
			{
				AdventureBase* item=new LevelUpRewardPage(id,mContentList.size());
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==ContinueLogin)
			{
				AdventureBase* item=new AdventureContinuePage(id,mContentList.size());
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==Invite)
			{
				AdventureBase* item=new AdventureInvitePage(id,mContentList.size());
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==ChargeRebate)
			{
				AdventureBase* item=new ChargeRebatePage(id);
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType==FortuneCome)
			{
				AdventureBase* item=new FortuneComePage(id);
				item->init();
				addContentItem(i,id,item);
			}
			else if(adventureItem->adventureType>=LuaScriptStart)
			{
				AdventureBase* item=new AdventureScriptBase(id, adventureItem->ccbi);
				item->init();
				addContentItem(i,id,item);
			}
			else
			{
				isAddItem=false;
			}
		}
		else
		{
			isAddItem=false;
		}
		if(isAddItem)
		{
			mPageIndexList.insert(PageIndexList::value_type(i,id));
			AdventureBase* itemNav=new NavContent(id);
			itemNav->init();
			addNavItem(i,id,itemNav);
			++i;
		}
	}

	std::vector<unsigned int >::iterator sIt=skillList.begin();
	for(;sIt!=skillList.end();++sIt)
	{
		unsigned int id=*sIt;
		SkillBookPage* item=new SkillBookPage(id);
		item->init();
		addContentItem(i,id,item);
		mPageIndexList.insert(PageIndexList::value_type(i,id));
		AdventureBase* itemNav=new NavContent(id);
		itemNav->init();
		addNavItem(i,id,itemNav);
		++i;
	}

	mNavContent->forceRecaculateChildren();
	mPageContent->forceRecaculateChildren();
	if(mPageIndexList.size()>0)
	{
// 		getNavContentByIndex(mCurrIndex)->updatePage(true);
// 		getAdventureBaseByIndex(mCurrIndex)->loadData();
// 		getAdventureBaseByIndex(mCurrIndex)->updatePage(true);
		changePage(mCurrIndex, false);
	}
}

void AdventurePage::addNavItem(unsigned int index,unsigned int id, AdventureBase* item )
{
	mNavContentList.insert(ContentList::value_type(id,item));
	mNavContentNode->addChild(item->getContainerNode());
	mNavWidth=item->getContainerNode()->getContentSize().width;
	item->getContainerNode()->setPosition(mNavSize.width,0);
	mNavSize.width +=mNavWidth;
	mNavSize.height= item->getContainerNode()->getContentSize().height;
	mNavContent->setContentSize(mNavSize);
}

void AdventurePage::addContentItem(unsigned int index,unsigned int id, AdventureBase* item )
{
	mContentList.insert(ContentList::value_type(id,item));
	mPageContentNode->addChild(item->getContainerNode());
	item->getContainerNode()->setPosition(mContentSize.width,0);
	mPageWidth=item->getContainerNode()->getContentSize().width;
	mContentSize.width += mPageWidth;
	mContentSize.height=item->getContainerNode()->getContentSize().height;
	mPageContent->setContentSize(mContentSize);
	if(mPageInitStats.find(id)!=mPageInitStats.end())
	{
		item->setIsInitData(mPageInitStats.find(id)->second);
	}
}

void AdventurePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
	_checkAdventureInfo();
	if(BlackBoard::Get()->ToAdventruePageType>0)
	{
		AdventureItemInfoMapIterator itr = ServerDateManager::Get()->getAdventureItemInfoMapIterator();
		while(itr.hasMoreElements())
		{
			const AdventureItemInfo* adventureItemInfo=itr.getNext();
			const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(adventureItemInfo->adventureId);
			if(adventureItem->adventureType==BlackBoard::Get()->ToAdventruePageType)
			{
				MsgAdventurePageChange msg;
				msg.index = adventureItemInfo->id;
				MessageManager::Get()->sendMessage(&msg);
			}
		}
		BlackBoard::Get()->ToAdventruePageType = 0;
	}
}

void AdventurePage::clearAllItem( void )
{
	mPageIndexList.clear();
	ContentList::iterator itr = mNavContentList.begin();
	while(itr != mNavContentList.end())
	{
		delete itr->second;
		++itr;
	}
	mNavContentList.clear();
	if (mNavContentNode)
		mNavContentNode->removeAllChildren();
	ContentList::iterator citr = mContentList.begin();
	while(citr != mContentList.end())
	{
		delete citr->second;
		++citr;
	}
	mContentList.clear();
	if (mPageContentNode)
		mPageContentNode->removeAllChildren();
}

void AdventurePage::reSizeAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		itr->second->refreshContent();
		++itr;
	}
}

void AdventurePage::removeNavItem( AdventureBase* item )
{
	ContentList::iterator itr = mNavContentList.begin();
	while(itr != mNavContentList.end())
	{
		if(itr->second==item)
		{
			mNavContentNode->removeChild(item->getContainerNode());
			delete item;
			mNavContentList.erase(itr);
			break;
		}
		++itr;
	}
}

void AdventurePage::removeContentItem( AdventureBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if(itr->second==item)
		{
			mPageContentNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
			break;
		}
		++itr;
	}
}

void AdventurePage::removeItem(unsigned int index)
{
	bool isChange=false;
	if(index<mCurrIndex)
	{
		if(mPageIndexList.size()>1)
		{
			isChange=true;
			mCurrIndex--;
		}
	}
	else if(index>mCurrIndex)
	{

	}
	else
	{
		if(mCurrIndex==mPageIndexList.size())
		{
			if(mPageIndexList.size()>1)
			{
				isChange=true;
				mCurrIndex=mPageIndexList.size()-1;
			}
			else
			{

			}
		}
		else
		{
			if(mCurrIndex==0)
			{
				if(mPageIndexList.size()>1)
				{
					isChange=true;
					mCurrIndex++;
				}
				else
				{

				}
			}
			else
			{
				if(mPageIndexList.size()>1)
				{
					isChange=true;
					mCurrIndex--;
				}
				else
				{

				}
			}
		}
		if(isChange)
		{
			isChange=false;
			changePage(mCurrIndex,true);
		}
	}
	
	removeNavItem(getNavContentByIndex(index));
	removeContentItem(getAdventureBaseByIndex(index));
	ServerDateManager::Get()->removeAdventureItemInfoById(getIdByIndex(index));
	mPageIndexList.clear();
	int i=0;
	ContentList::iterator itr = mContentList.begin();
	std::vector<unsigned int > skillList;
	while(itr!=mContentList.end())
	{
		const AdventureItemInfo* item=ServerDateManager::Get()->getAdventureItemInfoByID(itr->first);
		if(item)
		{
			const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(item->adventureId);
			if(adventureItem->adventureType!=SkillBook)
			{
				mPageIndexList.insert(PageIndexList::value_type(i,itr->first));
				++i;
			}
			else
			{
				skillList.push_back(itr->first);
			}
		}
		++itr;
	}

	std::vector<unsigned int >::iterator sIt=skillList.begin();
	for(;sIt!=skillList.end();++sIt)
	{
		mPageIndexList.insert(PageIndexList::value_type(i,*sIt));
		++i;
	}
	if(!_checkAdventureInfo())
	{
		refreshAllItem();
		if(isChange)
		{
			changePage(mCurrIndex,true);
		}
	}
}

bool AdventurePage::_checkAdventureInfo()
{
	if(mPageIndexList.size()==0&&mIsInitData)
	{
		MSG_BOX(Language::Get()->getString("@CurrAdventureNoPage"));
		MsgMainFrameChangePage msg;
		msg.pageName = "MainPage";
		MessageManager::Get()->sendMessage(&msg);
		return true;
	}
	return false;
}

void AdventurePage::refreshAllNavItem()
{
	mNavSize.width=0;
	PageIndexList::iterator it=mPageIndexList.begin();
	for(int i=0;it!=mPageIndexList.end();++it,++i)
	{
		AdventureBase* adventureBase=getNavContentByIndex(it->first);
		if(adventureBase)
		{
			mNavWidth=adventureBase->getContainerNode()->getContentSize().width;
			adventureBase->getContainerNode()->setPosition(mNavSize.width,0);
			mNavSize.width +=mNavWidth;
			mNavContent->setContentSize(mNavSize);
		}
	}
}

void AdventurePage::refreshAllContentItem()
{
	mContentSize.width=0;
	PageIndexList::iterator it=mPageIndexList.begin();
	for(int i=0;it!=mPageIndexList.end();++it,++i)
	{
		AdventureBase* adventureBase=getAdventureBaseByIndex(it->first);
		if(adventureBase)
		{
			mPageWidth=adventureBase->getContainerNode()->getContentSize().width;
			adventureBase->getContainerNode()->setPosition(mContentSize.width,0);
			mContentSize.width +=mPageWidth;
			mPageContent->setContentSize(mContentSize);
		}
	}
}

void AdventurePage::refreshAllItem()
{
	refreshAllNavItem();
	refreshAllContentItem();
}

void AdventurePage::Exit( MainFrame* frame)
{
	AdventureBase* currNav=getAdventureBaseByIndex(mCurrIndex);
	if(currNav)
		currNav->exitPage();
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void AdventurePage::Execute( MainFrame* )
{
	AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
	if(base)
	{
		if(base->getIsInitData())
		{
			base->updatePage(true,GamePrecedure::Get()->getFrameTime());
		}
	}
}

void AdventurePage::load( void )
{
	loadCcbiFile("AdventurePage.ccbi");
	mNavContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mNavContent"));
	mNavContentNode=mNavContent->getContainer();
	mPageContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	mPageContentNode=mPageContent->getContainer();
}

void AdventurePage::scrollViewDidDeaccelerateStop( cocos2d::extension::CCScrollView* view ,cocos2d::CCPoint initSpeed)
{
	if(view)
	{
		float currentOffset = view->getContentOffset().x;
		float target = 0;
		while(fabs(currentOffset) - target > mPageWidth)
		{
			target+=mPageWidth;
		}
		target = (currentOffset<0)?-target:target;
		if(initSpeed.x<0 && (currentOffset - target)<0)
		{
			target -= mPageWidth;
		}
		else if(initSpeed.x>0 && (currentOffset - target)>0)
		{
			target += mPageWidth;
		}
	
		if(currentOffset>target)
		{

			if((currentOffset-target)>mPageWidth*0.7)
			{
				target+=mPageWidth;
			}
		}
		else
		{
			if((target-currentOffset)>mPageWidth*0.7)
			{
				target-=mPageWidth;
			}
		}
		if(target>view->maxContainerOffset().x)
		{	
			target = view->maxContainerOffset().x;
		}
		if(target<view->minContainerOffset().x)
		{
			target = view->minContainerOffset().x;
		}
		
		int index=-target/mPageWidth;
		if(fabs(currentOffset - target)>1)
		{
			if(index==mCurrIndex)
			{
				mPageContent->setContentOffsetInDuration(ccp(target,view->getContentOffset().y),0.1f);
			}
			else
			{
				changePage(index,true);
			}
		}
	}
}

NavContent* AdventurePage::getNavContentByIndex(const unsigned int index)
{
	const int id=getIdByIndex(index);
	if(id==-1)
	{
		return NULL;
	}
	if(mNavContentList.find(id)!=mNavContentList.end())
	{
		return dynamic_cast<NavContent* >(mNavContentList.find(id)->second);
	}
	return NULL;
}

AdventureBase* AdventurePage::getAdventureBaseByIndex(const unsigned int index)
{
	const int id=getIdByIndex(index);
	if(id==-1)
	{
		return NULL;
	}
	if(mContentList.find(id)!=mContentList.end())
	{
		return (mContentList.find(id)->second);
	}
	return NULL;
}

const int AdventurePage::getIdByIndex(const unsigned int index)
{
	if(mPageIndexList.find(index)!=mPageIndexList.end())
	{
		return mPageIndexList.find(index)->second;
	}
	return -1;
}

unsigned int AdventurePage::_getIndexById(unsigned int id)
{
	PageIndexList::iterator it=mPageIndexList.begin();
	for(;it!=mPageIndexList.end();++it)
	{
		if(it->second==id)
		{
			return it->first;
		}
	}
	return 0;
}

void AdventurePage::resetNavOffset(const unsigned int index)
{
	float currNavOffsetX=mNavContent->getContentOffset().x;
	float maxOffsetX=0-index*mNavWidth;
	float minOffsetX=0-(index+1)*mNavWidth;
	if((currNavOffsetX-minOffsetX)>mNavContent->getViewSize().width)
	{
		mNavContent->setContentOffsetInDuration(ccp(maxOffsetX,mNavContent->getContentOffset().y),0.5);
	}
	else
	{
		if(currNavOffsetX-maxOffsetX<0)
		{
			float target=currNavOffsetX+mNavContent->getViewSize().width;
			if(target>0.0)
			{
				target=0.0;
			}
			mNavContent->setContentOffsetInDuration(ccp(target,mNavContent->getContentOffset().y),0.5);
		}
	}
}

void AdventurePage::changePage(const unsigned int index,bool isDuration/*=true*/)
{
	float offsetX=0-index*mPageWidth;
	if(isDuration)
		mPageContent->setContentOffsetInDuration(ccp(offsetX,mPageContent->getContentOffset().y),0.3f);
	else
		mPageContent->setContentOffset(ccp(offsetX, mPageContent->getContentOffset().y));
	AdventureBase* contentPage=getAdventureBaseByIndex(index);
	if(contentPage)
	{
		contentPage->loadData();
		contentPage->updatePage(true);
	}
	NavContent* preNav=getNavContentByIndex(mCurrIndex);
	if(preNav&&index!=mCurrIndex)
	{
		preNav->updatePage(false);
	}
	
	//resetNavOffset(index);
	float currNavOffsetX=mNavContent->getContentOffset().x;
	float maxOffsetX=0-index*mNavWidth;
	float minOffsetX=0-(index+1)*mNavWidth;
	if((currNavOffsetX-minOffsetX)>mNavContent->getViewSize().width)
	{
		if(isDuration)
			mNavContent->setContentOffsetInDuration(ccp(maxOffsetX,mNavContent->getContentOffset().y),0.5);
		else
			mNavContent->setContentOffset(ccp(maxOffsetX, mNavContent->getContentOffset().y));
	}
	else
	{
		if(currNavOffsetX-maxOffsetX<0)
		{
			float target=currNavOffsetX+mNavContent->getViewSize().width;
			if(target>0.0)
			{
				target=0.0;
			}
			if(isDuration)
				mNavContent->setContentOffsetInDuration(ccp(target,mNavContent->getContentOffset().y),0.5);
			else
				mNavContent->setContentOffset(ccp(target, mNavContent->getContentOffset().y));
		}
	}

	mCurrIndex=index;
	NavContent* currNav=getNavContentByIndex(mCurrIndex);
	if(currNav)
	{
		currNav->updatePage(true);
	}
}

void AdventurePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_ADVENTURE_PAGECHANGE)
	{
		const MsgAdventurePageChange * msg = dynamic_cast<const MsgAdventurePageChange*>(message);
		if(msg)
		{
			unsigned int _index=_getIndexById(msg->index);
			if(mCurrIndex!=_index)
			{
				changePage(_index);
			}
		}
	}
	else if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate* >(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_GET_ADVENTURE_INFORET_S)
			{
				mIsInitData=true;
				rebuildAllItem();
			}
			//eg 
			else if(msg->opcode==OPCODE_ADD_POWERRET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					AddPowerPage* addPowerPage=dynamic_cast<AddPowerPage* >(base);
					if(addPowerPage)addPowerPage->showSuccessDialog();
				}
			}
			else if(msg->opcode==OPCODE_GET_ADVENTURE_POWERINFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					BoundlessHolePage* boundlessHolePage=dynamic_cast<BoundlessHolePage* >(base);
					boundlessHolePage->showSuccessDialog();
				}
			}
			else if(msg->opcode==OPCODE_GET_ADVENTURE_TEACHERINFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_ADD_ADVENTURE_TEACHEXPRET_S)
			{
				MsgAdventureTeachExpDataInit msg;
				msg.index = mCurrIndex;
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(msg->opcode==OPCODE_GET_FIGHTEND_INFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_FIGHTEND_BATTLERET_S||msg->opcode==OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S||msg->opcode==OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S||msg->opcode==OPCODE_GET_FIGHTEND_REWARD_INFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					base->updatePage(false);
				}
				if(msg->opcode==OPCODE_GET_FIGHTEND_REWARD_INFORET_S)
				{
					MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
					packMsg->index=FIGHT_END_REWARD;
					MessageManager::Get()->sendMessage(packMsg);
				}
				else if(msg->opcode==OPCODE_FIGHTEND_BATTLERET_S)
				{
					
				}
			}
			else if(msg->opcode==OPCODE_GET_SKILL_BOOK_INFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_PIECE_SKILL_BOOKRET_S)
			{
				MsgMainFramePushPage msg;
				msg.pageName = "SkillSurprisePage";
				MessageManager::Get()->sendMessage(&msg);
				if(!ServerDateManager::Get()->minusSkillBookCountById(getIdByIndex(mCurrIndex)))
				{
					this->removeItem(mCurrIndex);
				}
			}
			else if(msg->opcode==OPCODE_ADVENTURE_LEVEL_UP_GET_REWARDRET_S)
			{
				MSG_BOX(Language::Get()->getString("@AdventureGetRewardSuccess"));
			}
			else if(msg->opcode==OPCODE_ADVENTURE_LEVEL_UP_REWARDRET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_GET_WORLD_BOSS_INFORET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_ADVENTURE_LEVEL_UP_GET_REWARDRET_S)
			{
				MSG_BOX(Language::Get()->getString("@AdventureGetRewardSuccess"));
			}
			else if(msg->opcode==OPCODE_ADVENTURE_CONTINUELOGIN_REWARDRET_S)
			{
				Json::Value loginStatus;
				const UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
				Json::Reader jreader;
				jreader.parse(info.mContinueLoginInfo,loginStatus,false);
				if(loginStatus.size()>=(AdventureContinueLoginTableManager::Get()->getTotalNum()+2))
				{
					MsgAdventureRemoveItem msg;
					msg.index = getIdByIndex(mCurrIndex);
					MessageManager::Get()->sendMessage(&msg);
				}
			}
			else if(msg->opcode==OPCODE_USER_INVITESTATUSRET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_GET_INVITE_REWARDRET_S)
			{
				Json::Value rewardStatus;
				const UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
				Json::Reader jreader;
				jreader.parse(info.mInviteRewardStatus,rewardStatus,false);
				if(rewardStatus.size()>=(AdventureInviteTableManager::Get()->getTotalNum()))
				{
					MsgAdventureRemoveItem msg;
					msg.index = getIdByIndex(mCurrIndex);
					MessageManager::Get()->sendMessage(&msg);
				}
			}
			else if(msg->opcode==OPCODE_CHARGE_REBATERET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
			else if(msg->opcode==OPCODE_FORTUNE_COMERET_S)
			{
				AdventureBase* base=getAdventureBaseByIndex(mCurrIndex);
				if(base)
				{
					_setPageStats(base->getContentID(),true);
					base->setIsInitData(true);
				}
			}
		}
	}
	else if(message->getTypeId()==MSG_ADVENTURE_REMOVEITEM)
	{
		const MsgAdventureRemoveItem* msg=dynamic_cast<const MsgAdventureRemoveItem* >(message);
		unsigned int _index=_getIndexById(msg->index);
		this->removeItem(_index);
	}
	else if(message->getTypeId()==MSG_FIGHT_END)
	{
		const MsgFightEnd* msg=dynamic_cast<const MsgFightEnd*>(message);
		if(msg)
		{
			std::string varibleKey="adventureFightEndRet";
			if(BlackBoard::Get()->hasVarible(varibleKey))
			{
				if(!BlackBoard::Get()->getVaribleAsBool(varibleKey))
				{
					ShowFightEndPopupPage();
				}
			}
			varibleKey="adventureFightEndToolId";
			if(BlackBoard::Get()->hasVarible(varibleKey))
			{
				unsigned int toolId=BlackBoard::Get()->getVaribleAsUint(varibleKey);
				if(toolId!=0)
				{
					PropInfoPageShow(toolId,PROP_USE,false);
					toolId=0;
					BlackBoard::Get()->setVarible(varibleKey,toolId);
				}
			}
		}
	}
}

void AdventurePage::_setPageStats(unsigned int _pageId,bool _stats)
{
	if(mPageInitStats.find(_pageId)!=mPageInitStats.end())
	{
		mPageInitStats[_pageId]=_stats;
	}
	else
	{
		mPageInitStats.insert(PageInitStats::value_type(_pageId,_stats));
	}
}

void AdventurePage::checkAdActivityGuideStatus()
{
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		std::string key="ad";
		Json::Value _status;
		Json::Reader jreader;
		jreader.parse(ServerDateManager::Get()->getUserBasicInfo().actStatus.c_str(),_status,false);
		if(!_status[key].empty())
		{
			unsigned int _serverAd=_status[key].asInt();
			if(_serverAd>0)
			{
				char str[128];
				sprintf(str,"s%d_tip_%s",GamePrecedure::Get()->getServerID(),key.c_str());
				unsigned int _localAd=cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(str,0);
				//check mainframe ad is need modify
				if(_serverAd>_localAd)
				{
					//check unit ad is need modify
					bool isModify=true;
					ContentList::iterator itr = mNavContentList.begin();
					while(itr != mNavContentList.end())
					{
						NavContent* _base=dynamic_cast<NavContent* >(itr->second);
						if(_base)
						{
							if(_base->isShowActivityTip())
							{
								isModify=false;
								break;
							}
						}
						++itr;
					}
					if(isModify)
					{
						cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(str,_serverAd);
						cocos2d::CCUserDefault::sharedUserDefault()->flush();
					}
				}
					
			}
		}
	}	
}