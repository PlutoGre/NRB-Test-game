
#include "stdafx.h"

#include "LevelUpRewardPage.h"

USING_NS_CC;

void LevelUpRewardItem::refreshContent(void)
{
	const UserBasicInfo userBasicInfo=ServerDateManager::Get()->getUserBasicInfo();
	AdventureLevelUpRewardInfo* info = ServerDateManager::Get()->getAdventureLevelUpRewardInfoByID(mID);
	std::string _levelStr=StringConverter::toString(info->needLevel)+Language::Get()->getString("@Ji");
	CCB_FUNC(getContainerNode(),"mLevel",CCLabelBMFont,setString(_levelStr.c_str()));
	CCLabelBMFont* mGoldNumWithVip = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mGoldNumWithVip"));
	if(mGoldNumWithVip)
	{
		mGoldNumWithVip->setString(Language::Get()->getString(StringConverter::toString(info->goldcoins)).c_str());
	}
	CCLabelBMFont* mGoldNumWithoutVip = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mGoldNumWihtoutVip"));
	if(mGoldNumWithoutVip)
	{
		mGoldNumWithoutVip->setString(Language::Get()->getString(StringConverter::toString(info->goldcoins)).c_str());
	}

	CCNode* mNodeWithVip=dynamic_cast<CCNode* >(mContentContainer->getVariable("mNodeWithVip"));
	CCNode* mNodeWithoutVip=dynamic_cast<CCNode* >(mContentContainer->getVariable("mNodeWithoutVip"));

	CCSprite* mVipPic1=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mVipPic1"));
	if(mVipPic1)
	{
		std::string vipImage = ::getVipLevelImageFile(info->upVipLevel);
		if(vipImage!="")
		{
			CCB_FUNC(mContentContainer,"mNodeWithVip",CCNode,setVisible(true));
			CCB_FUNC(mContentContainer,"mNodeWithoutVip",CCNode,setVisible(false));
			mVipPic1->initWithFile(vipImage.c_str());
		}
		else
		{
			CCB_FUNC(mContentContainer,"mNodeWithVip",CCNode,setVisible(false));
			CCB_FUNC(mContentContainer,"mNodeWithoutVip",CCNode,setVisible(true));
			mVipPic1->setVisible(false);
		}
	}


	
	CCSprite* mVipPic2=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mVipPic2"));
	if(mVipPic2)
	{
		mVipPic2->setVisible(false);
	}

	CCMenuItemImage* getReward = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("getReward"));

	CCLabelBMFont* mGetRewardLable = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mGetRewardLable"));

	if(getReward)
	{
		if(userBasicInfo.level>=info->needLevel)
		{
			getReward->setEnabled(true);
		}
		else
		{
			getReward->setEnabled(false);
		}
		if(info->rewardStatus == 2)
		{
			mGetRewardLable->setString(Language::Get()->getString("@AlreadyGetReward").c_str());
			getReward->setEnabled(false);
		}
		else
		{
			if (userBasicInfo.level<info->needLevel)
			{
				getReward->setEnabled(false);

			}else
			{
				getReward->setEnabled(true);
			}
			mGetRewardLable->setString(Language::Get()->getString("@LvUpReceive").c_str());
		}
	}
}

void LevelUpRewardItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	AdventureLevelUpRewardInfo* info = ServerDateManager::Get()->getAdventureLevelUpRewardInfoByID(mID);
	const UserBasicInfo userBasicInfo=ServerDateManager::Get()->getUserBasicInfo();
	if(itemName=="getRewardBtn")
	{
		if(info->rewardStatus <= 1&&userBasicInfo.level>=info->needLevel)
		{
			OPAdventureLevelUpGetReward msg;
			msg.set_version(1);
			msg.set_rewardid(info->id);
			PacketManager::Get()->sendPakcet(OPCODE_ADVENTURE_LEVEL_UP_GET_REWARD_C, &msg);

			info->rewardStatus = 2;

			CCMenuItemImage* getReward = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("getReward"));
			CCLabelBMFont* mGetRewardLable = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mGetRewardLable"));
			if(getReward)
			{
				mGetRewardLable->setString(Language::Get()->getString("@AlreadyGetReward").c_str());
				getReward->setEnabled(false);
			}
			deleteLevelUpRewardItem();
		}
		else
		{
			if(userBasicInfo.level<info->needLevel)
			{
				MSG_BOX(Language::Get()->getString("@AdventureRewardLevelErr"));
			}
			else
			{
				if(info->rewardStatus==2)
				{
					MSG_BOX(Language::Get()->getString("@AdventureAlreadyRewardErr"));
				}
			}
		}
	}
}
void LevelUpRewardItem::deleteLevelUpRewardItem()
{
	AdventureLevelUpRewardInfo* deleteInfo;
	bool deleteFlag = true;
	for(int i=1;i<=ServerDateManager::Get()->getAdventureLevelUpRewardInfoSize();i++)
	{
		deleteInfo = ServerDateManager::Get()->getAdventureLevelUpRewardInfoByID(i);
		if(deleteInfo==NULL)
		{
			deleteFlag = false;

		}
		else if(deleteInfo->rewardStatus != 2)
		{
			deleteFlag = false;
			break;
		}
	}
	if(deleteFlag)
	{
		MsgAdventureRemoveItem msg;
		msg.index = mParentID;
		MessageManager::Get()->sendMessage(&msg);
		
	}
}

void LevelUpRewardPage::refreshContent()
{

}

void LevelUpRewardPage::showSuccessDialog()
{
	CCLOG("LevelUpRewardPage showSuccessDialog");
}

void LevelUpRewardPage::loadData()
{
	refreshFlag = true;
	bool getInit = getIsInitData();
	if(!getIsInitData())
	{
		OPAdventureLevelUpReward msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_ADVENTURE_LEVEL_UP_REWARD_C, &msg);
	}
}

void LevelUpRewardPage::updatePage(bool isShow,float dt)
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

void LevelUpRewardPage::exitPage(){

}

void LevelUpRewardPage::changeRewardStatus()
{
	levelUpContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(mContentContainer->getVariable("levelUpContent"));
	//--begin xinzheng 2013-6-5
	{
		CCTouchHandler* pHandler = CCDirector::sharedDirector()->getTouchDispatcher()
										->findHandler(levelUpContent);
		if (pHandler)
		{
			CCTargetedTouchHandler* pTh = dynamic_cast<CCTargetedTouchHandler*>(pHandler);
			if (pTh)
			{
				pTh->setSwallowsTouches(false);//不消化，继续传递
				//自己是竖直touch，父ScrollView横向touch
			}
		}
		levelUpContent->setBounceable(false);
	}
	//--end
	membersRootNode = levelUpContent->getContainer();
	rebuildAllItem();
}

void LevelUpRewardPage::rebuildAllItem(void)
{
	clearAllItem();
	buildItem();
}

void LevelUpRewardPage::clearAllItem(void)
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

void LevelUpRewardPage::buildItem(void)
{

	AdventureLevelUpRewardInfoIterator itr = ServerDateManager::Get()->getAdventureLevelUpRewardInfoIterator();
	ServerDateManager::Get()->getAdventureLevelUpRewardInfoSize(); 

	while(itr.hasMoreElements())
	{

		refreshFlag = false;

		AdventureLevelUpRewardInfo* adventureLevelUpRewardInfo = itr.getNext();
		adventureLevelUpRewardInfo->id;

		LevelUpRewardItem * item = new LevelUpRewardItem(adventureLevelUpRewardInfo->id,mID);
		item->init();
		addItem(item);
	}
	
	if(ServerDateManager::Get()->getAdventureLevelUpRewardInfoListTotalNum()<=4){
		levelUpContent->setTouchEnabled(false);
	}else{
		levelUpContent->setTouchEnabled(true);
	}
	/*
	if(levelUpContent->getContentSize().height <= levelUpContent->getViewSize().height){
		levelUpContent->setTouchEnabled(false);
	}
	*/

}

void LevelUpRewardPage::addItem(ContentBase* item)
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(ServerDateManager::Get()->getAdventureLevelUpRewardInfoSize() - mContentList.size() ));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height*( mContentList.size()));
	levelUpContent->setContentSize(size);
	levelUpContent->setContentOffset(ccp(0,levelUpContent->minContainerOffset().y));

}


void LevelUpRewardPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

	
}

void LevelUpRewardPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

LevelUpRewardPage::~LevelUpRewardPage()
{
	clearAllItem();
}
