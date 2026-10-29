
#include "stdafx.h"

#include "TeacherTeachPage.h"
USING_NS_CC;

void TeacherTeachPage::refreshContent( void )
{
	mTeacherPicSprite = dynamic_cast<cocos2d::CCSprite* >(this->getObjectByVariableName("mTeacherPicSprite"));
	mTeacherSayLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mTeacherSayLabel"));
	mGetExpNode = dynamic_cast<cocos2d::CCNode* >(this->getObjectByVariableName("mGetExpNode"));
	mGetExpValueLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mGetExpValueLabel"));
	mSendWineValueLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mSendWineValueLabel"));
	mCountDownNode = dynamic_cast<cocos2d::CCNode* >(this->getObjectByVariableName("mCountDownNode"));
	mCountDownLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mCountDownLabel"));
	mWillGetExpValueLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mWillGetExpValueLabel"));
	mCountDownNode->setVisible(false);
	mGetExpNode->setVisible(false);
}

void TeacherTeachPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onGetCommonExpButton")
	{
		OPAddTeachExp msg;
		msg.set_version(1);
		msg.set_type(GET_COMMON_EXP);
		msg.set_adventureid(mID);
		PacketManager::Get()->sendPakcet(OPCODE_ADD_ADVENTURE_TEACHEXP_C, &msg);

		MsgMainFramePushPage msgPushPage;
		msgPushPage.pageName="TeacherTeachResultPage";
		MessageManager::Get()->sendMessage(&msgPushPage);
	}
	else if(itemName=="onGetDoubleExpButton")
	{
		const AdventureTeacherInfo* info = ServerDateManager::Get()->getAdventureTeacherInfoByID(mID);
		if(info!=NULL)
		{
			const AdventureTeacherItem* teacherItem	= AdventureTeacherTableManager::Get()->getAdventureTeacherByID(info->teacherId);
			if(teacherItem!=NULL)
			{
				const UserToolInfo* tool = ServerDateManager::Get()->getUserToolInfoByItemId(teacherItem->propID);
				if((tool==NULL)||(tool->count<teacherItem->needPropNum))
				{
					MsgMainFramePushPage pushPageMsg;
					pushPageMsg.pageName = "BuyPropPopupPage";
					MessageManager::Get()->sendMessage(&pushPageMsg);

					MsgBuyPropPopupPage buyPropMsg;
					buyPropMsg.toolId = teacherItem->propID;
					MessageManager::Get()->sendMessage(&buyPropMsg);
				}else
				{
					OPAddTeachExp msg;
					msg.set_version(1);
					msg.set_type(GET_DOUBLE_EXP);
					msg.set_adventureid(mID);
					PacketManager::Get()->sendPakcet(OPCODE_ADD_ADVENTURE_TEACHEXP_C, &msg);

					MsgMainFramePushPage msgPushPage;
					msgPushPage.pageName="TeacherTeachResultPage";
					MessageManager::Get()->sendMessage(&msgPushPage);
				}
			}
		}

		
		
	}
}

void TeacherTeachPage::loadData()
{
	if(!getIsInitData())
	{
		OPGetTeacherInfo *msg = new  OPGetTeacherInfo();
		msg->set_version(1);
		msg->set_adventureitemid(mID);
		PacketManager::Get()->sendPakcet(OPCODE_GET_ADVENTURE_TEACHERINFO_C,msg);
	}
}

void TeacherTeachPage::updatePage(bool isSelect,float dt)
{
	if(isSelect&&getIsInitData())
	{
		setTeacherStatus();
	}
}
void TeacherTeachPage::setTeacherStatus()
{
	const AdventureTeacherInfo* info = ServerDateManager::Get()->getAdventureTeacherInfoByID(mID);
	if(info!=NULL)
	{
		const AdventureTeacherItem* teacherItem	= AdventureTeacherTableManager::Get()->getAdventureTeacherByID(info->teacherId);
		if(teacherItem!=NULL)
		{
			mTeacherPicSprite->initWithFile((teacherItem->iconPic).c_str());
			float timeOffset = GamePrecedure::Get()->getTotalTime()-info->totalTimeStamp;
			if(info->serverTime+(int)timeOffset>=info->endTime)
			{
				mTeacherSayLabel->setString(Language::Get()->getString("@CanGetTeachExp").c_str());
				mCountDownNode->setVisible(false);
				mGetExpNode->setVisible(true);
				mGetExpValueLabel->setString(StringConverter::toString(info->exp).c_str());
				mSendWineValueLabel->setString(StringConverter::toString(teacherItem->needPropNum).c_str());
			}else
			{
				mTeacherSayLabel->setString(Language::Get()->getString("@WaitToGetTeachExp").c_str());
				mCountDownNode->setVisible(true);
				mGetExpNode->setVisible(false);
				mWillGetExpValueLabel->setString(StringConverter::toString(info->exp).c_str());
				mCountDownLabel->setString(GameMaths::formatSecondsToTime(info->endTime-(info->serverTime+(int)timeOffset)).c_str());
			}

		}
	}
}

void TeacherTeachPage::exitPage()
{

}
