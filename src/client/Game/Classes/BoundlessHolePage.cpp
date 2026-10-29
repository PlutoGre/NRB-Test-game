
#include "stdafx.h"

#include "BoundlessHolePage.h"
#include "GameMaths.h"
USING_NS_CC;

void BoundlessHolePage::refreshContent( void )
{
	CCObject* statusObj=getContainerNode()->getVariable("mBoundlessStatus");
	if(!statusObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mBoundlessStatus");
		CCMessageBox(errMsg,"error");
	}

	mBoundlessStatus=dynamic_cast<cocos2d::CCLabelBMFont* >(statusObj);
	mBoundlessStatus->setString("");

	CCObject* statusObj2=getContainerNode()->getVariable("mBoundlessStatus2");
	if(!statusObj2)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mBoundlessStatus");
		CCMessageBox(errMsg,"error");
	}
	mBoundlessStatus2=dynamic_cast<cocos2d::CCLabelBMFont* >(statusObj2);
	mBoundlessStatus2->setString("");

	CCObject* btnObj=getContainerNode()->getVariable("mBoundlessHole");
	if(!btnObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mBoundlessHole");
		CCMessageBox(errMsg,"error");
	}

			
	mKissTxt1 = dynamic_cast<cocos2d::CCLabelBMFont* >(statusObj);
	mBoundlessHole=dynamic_cast<cocos2d::CCMenuItemImage* >(btnObj);
	if(mBoundlessHole)
	{
		CCB_FUNC(this->getContainerNode(),"mKissTxt1",CCLabelBMFont,setString(Language::Get()->getString("@KissTxt").c_str()));
		CCB_FUNC(this->getContainerNode(),"mKissTxt2",CCLabelBMFont,setString(Language::Get()->getString("@KissTxt").c_str()));
		mBoundlessHole->setEnabled(false);
	}	
	mAlreadyUpdate=false;
}

void BoundlessHolePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

}

void BoundlessHolePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(!getIsInitData())
	{
		return;
	}
	else
	{
		const UserAdventureDataInfo& userAdventureDataInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
		if(userAdventureDataInfo.boundlessHoldeInfo.todayStatus==0)
		{
			OPAddBoundlessHole msg;
			msg.set_version(1);
			PacketManager::Get()->sendPakcet(OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFO_C, &msg);
			mBoundlessHole->setEnabled(false);
			CCB_FUNC(this->getContainerNode(),"mKissTxt1",CCLabelBMFont,setString(Language::Get()->getString("@AlreadyKissTxt").c_str()));
			CCB_FUNC(this->getContainerNode(),"mKissTxt2",CCLabelBMFont,setString(Language::Get()->getString("@AlreadyKissTxt").c_str()));
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@AlreadyKissed"));
		}
	}
}

void BoundlessHolePage::showSuccessDialog()
{
	_updateStatus(false);
	showHeartOnce(3);
}

void BoundlessHolePage::_updateStatus(bool isPlayAni)
{
	const UserAdventureDataInfo& userAdventureDataInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
	if(mBoundlessHole)
	{
		if(userAdventureDataInfo.boundlessHoldeInfo.todayStatus>0)
		{
			CCB_FUNC(this->getContainerNode(),"mKissTxt1",CCLabelBMFont,setString(Language::Get()->getString("@AlreadyKissTxt").c_str()));
			CCB_FUNC(this->getContainerNode(),"mKissTxt2",CCLabelBMFont,setString(Language::Get()->getString("@AlreadyKissTxt").c_str()));
			mBoundlessHole->setEnabled(false);
			if(isPlayAni)
			{
				showHeartOnce(1);
			}
		}
		else
		{
			CCB_FUNC(this->getContainerNode(),"mKissTxt1",CCLabelBMFont,setString(Language::Get()->getString("@KissTxt").c_str()));
			CCB_FUNC(this->getContainerNode(),"mKissTxt2",CCLabelBMFont,setString(Language::Get()->getString("@KissTxt").c_str()));
			mBoundlessHole->setEnabled(true);
			if(isPlayAni)
			{
				showHeartOnce(2);
			}
		}
	}
	if(mBoundlessStatus)
	{
		char v[256];
		sprintf(v,"%d/%d",userAdventureDataInfo.boundlessHoldeInfo.currCount,userAdventureDataInfo.boundlessHoldeInfo.accumulateCount);
		std::list<std::string> replaceList;
		replaceList.push_back(v);
		mBoundlessStatus->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@AlreadyAccumulateDes"),&replaceList).c_str());
	}
	if(mBoundlessStatus2)
	{
		mBoundlessStatus2->setString(Language::Get()->getString("@AccumulateSurpris").c_str());
	}
}

void BoundlessHolePage::loadData()
{
	mIsShow=true;
	if(!getIsInitData())
	{
		OPGetBoundlessHole msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFO_C, &msg);
	}
	heartFlag = 0;
}

void BoundlessHolePage::updatePage(bool isShow,float dt)
{

	mIsShow=isShow;
	if(getIsInitData()&&isShow&&!mAlreadyUpdate)
	{
		mAlreadyUpdate=true;
		_updateStatus(true);
	}
}

void BoundlessHolePage::showHeartOnce(int status)
{
	if(status == 1)
	{
		mContentContainer->runAnimation("kissedTimeline");
	}
	else if(status == 2)
	{
		mContentContainer->runAnimation("kissTimeline");
	}
	else if(status==3)
	{
		mContentContainer->runAnimation("kissdonghua");
	}
}
void BoundlessHolePage::exitPage()
{

}
