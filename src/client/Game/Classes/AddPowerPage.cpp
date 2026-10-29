
#include "stdafx.h"

#include "AddPowerPage.h"
#include "GameMaths.h"
USING_NS_CC;

void AddPowerPage::refreshContent( void )
{
	CCObject* amLabelObj=getContainerNode()->getVariable("mAMStatusTxt");
	if(!amLabelObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mAMStatusTxt");
		CCMessageBox(errMsg,"error");
	}
	mAMLabelTxt=dynamic_cast<cocos2d::CCLabelBMFont* >(amLabelObj);
	if(mAMLabelTxt)
	{
		mAMLabelTxt->setVisible(false);
	}
	CCObject* pmLabelObj=getContainerNode()->getVariable("mPMStatusTxt");
	if(!pmLabelObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPMStatusTxt");
		CCMessageBox(errMsg,"error");
	}
	mPMLabelTxt=dynamic_cast<cocos2d::CCLabelBMFont* >(pmLabelObj);
	if(mPMLabelTxt)
	{
		mPMLabelTxt->setVisible(false);
	}
	
	CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(false));
	CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
	CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(false));
	CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
	/*
	CCObject* powBgObj=getContainerNode()->getVariable("mNotime");
	if(!powBgObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mNotime");
		CCMessageBox(errMsg,"error");
	}
	mCanNotEat=dynamic_cast<cocos2d::CCNode* >(powBgObj);
	if(mCanNotEat)
	{
		mCanNotEat->setVisible(false);
	}
	CCObject* addPowerPanelObj=getContainerNode()->getVariable("mRighttime");
	if(!addPowerPanelObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mRighttime");
		CCMessageBox(errMsg,"error");
	}
	mCanEat=dynamic_cast<cocos2d::CCNode* >(addPowerPanelObj);
	if(mCanEat)
	{
		mCanEat->setVisible(false);
	}
	*/
}

void AddPowerPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{


}

void AddPowerPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(mAddPowerType!=CAN_NOT_ADD)
	{
//		mCanEat->setVisible(false);
//		mCanNotEat->setVisible(true);
		sendAddPowerPacket();
	}
}

void AddPowerPage::sendAddPowerPacket()
{
	OPAddPower msg;
	msg.set_typeid_(AdventureAddPowerType);
	PacketManager::Get()->sendPakcet(OPCODE_ADD_POWER_C, &msg);
	UserAdventureDataInfo& userAddPowerInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
	if(mAddPowerType==AM_ADD)
	{
		userAddPowerInfo.addPowerInfo.amStatus=2;
	}
	else if(mAddPowerType==PM_ADD)
	{
		userAddPowerInfo.addPowerInfo.pmStatus=2;
	}
}

void AddPowerPage::showSuccessDialog()
{
	char buff[128];
	std::string str = Language::Get()->getString("@AdventureAddPowerSuccess");
	sprintf(buff,str.c_str(),10);
	MSG_BOX(buff);
}

void AddPowerPage::loadData()
{
	mIsShow=true;
	if(!getIsInitData()&&BlackBoard::Get()->ToAdventruePageType<=0)
	{
		OPGetAdventurePower msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_ADVENTURE_POWERINFO_C, &msg);
	}
}

void AddPowerPage::updatePage(bool isShow,float)
{
	mIsShow=isShow;
	if(mIsShow&&getIsInitData())
	{
		static float durationTime=0;
		durationTime+=GamePrecedure::Get()->getFrameTime();
		if(durationTime>1.0f)
		{
			durationTime -=1.0f;
			if(mAMLabelTxt)
			{
				mAMLabelTxt->setVisible(true);
			}
			if(mPMLabelTxt)
			{
				mPMLabelTxt->setVisible(true);
			}
			changePowerStatus();
		}
	}
}

void AddPowerPage::exitPage()
{

}

void AddPowerPage::changePowerStatus()
{
	const UserAdventureDataInfo userAddPowerInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
	time_t tick=(time_t)(GamePrecedure::Get()->getServerTime());
// 	struct tm* tmLocal=localtime(&tick);
// 	int localHour=tmLocal->tm_hour;
	struct tm* tm=gmtime(&tick);
	int serverHour=tm->tm_hour;
// 	int _timeAM=AMSTART_TIME+localHour-serverHour;
// 	if(_timeAM<0)
// 	{
// 		_timeAM+=24;
// 	}
// 	if(_timeAM>24)
// 	{
// 		_timeAM-=24;
// 	}
// 	int _timePM=PMSTART_TIME+localHour-serverHour;
// 	if(_timePM<0)
// 	{
// 		_timePM+=24;
// 	}
// 	if(_timePM>24)
// 	{
// 		_timePM-=24;
// 	}
// 	CCLOG("gmTime:%d,localTime:%d,timeAM:%d,timePM:%d",serverHour,localHour,_timeAM,_timePM);
	mAddPowerType=CAN_NOT_ADD;
	if(serverHour<AMSTART_TIME)
	{
		this->mAMLabelTxt->setString(Language::Get()->getString("@TimeIsNot").c_str());
		mPMLabelTxt->setString(Language::Get()->getString("@TimeIsNot").c_str());
		CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(true));
		CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
		CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(true));
		CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
//		mCanEat->setVisible(false);
//		mCanNotEat->setVisible(true);
		return;
	}
	if(serverHour>=AMSTART_TIME&&serverHour<AMEND_TIME)
	{
		mPMLabelTxt->setString(Language::Get()->getString("@TimeIsNot").c_str());
		if(userAddPowerInfo.addPowerInfo.amStatus<ALREADY_REWARD)
		{
			mAMLabelTxt->setString(Language::Get()->getString("@TimeUp").c_str());
			CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(false));
			CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
//			mCanNotEat->setVisible(false);
//			mCanEat->setVisible(true);
			mAddPowerType=AM_ADD;
		}
		else
		{
			mAMLabelTxt->setString(Language::Get()->getString("@AlreadyAddPower").c_str());
			CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
			CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
//			mCanNotEat->setVisible(true);
//			mCanEat->setVisible(false);
		}
		return;
	}
	if(serverHour>=AMEND_TIME&&serverHour<PMSTART_TIME)
	{
		if(userAddPowerInfo.addPowerInfo.amStatus<ALREADY_REWARD)
		{
			mAMLabelTxt->setString(Language::Get()->getString("@TimeOut").c_str());
		}
		else
		{
			mAMLabelTxt->setString(Language::Get()->getString("@AlreadyAddPower").c_str());
		}
		mPMLabelTxt->setString(Language::Get()->getString("@TimeIsNot").c_str());
		CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(true));
		CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
		CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(true));
		CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
//		mCanNotEat->setVisible(true);
//		mCanEat->setVisible(false);
		return;
	}
	if(serverHour>=PMSTART_TIME&&serverHour<PMEND_TIME)
	{
		if(userAddPowerInfo.addPowerInfo.amStatus<ALREADY_REWARD)
		{
			mAMLabelTxt->setString(Language::Get()->getString("@TimeOut").c_str());
		}
		else
		{
			mAMLabelTxt->setString(Language::Get()->getString("@AlreadyAddPower").c_str());
		}
		if(userAddPowerInfo.addPowerInfo.pmStatus<ALREADY_REWARD)
		{
			mPMLabelTxt->setString(Language::Get()->getString("@TimeUp").c_str());
			CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
			CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(false));
			CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(true));
//			mCanNotEat->setVisible(false);
//			mCanEat->setVisible(true);
			mAddPowerType=PM_ADD;
		}
		else
		{
			mPMLabelTxt->setString(Language::Get()->getString("@AlreadyAddPower").c_str());
			CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
			CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(true));
			CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
//			mCanNotEat->setVisible(true);
//			mCanEat->setVisible(false);
		}
		return;
	}

	if(serverHour>=PMEND_TIME)
	{
		if(userAddPowerInfo.addPowerInfo.amStatus<ALREADY_REWARD)
		{
			mAMLabelTxt->setString(Language::Get()->getString("@TimeOut").c_str());
		}
		else
		{
			mAMLabelTxt->setString(Language::Get()->getString("@AlreadyAddPower").c_str());
		}
		if(userAddPowerInfo.addPowerInfo.pmStatus<ALREADY_REWARD)
		{
			mPMLabelTxt->setString(Language::Get()->getString("@TimeOut").c_str());
		}
		else
		{
			mPMLabelTxt->setString(Language::Get()->getString("@AlreadyAddPower").c_str());
		}
		CCB_FUNC(getContainerNode(),"mAM1",CCNode,setVisible(true));
		CCB_FUNC(getContainerNode(),"mAM2",CCNode,setVisible(false));
		CCB_FUNC(getContainerNode(),"mPM1",CCNode,setVisible(true));
		CCB_FUNC(getContainerNode(),"mPM2",CCNode,setVisible(false));
//		mCanNotEat->setVisible(true);
//		mCanEat->setVisible(false);
		return;
	}
}