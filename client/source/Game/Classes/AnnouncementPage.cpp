
#include "stdafx.h"

#include "AnnouncementPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "libOS.h"
REGISTER_PAGE("AnnouncementPage", AnnouncementPage);
USING_NS_CC;
USING_NS_CC_EXT;

void AnnouncementContent::refreshContent( void )
{
	mItem=AnnouncementTableManager::Get()->getAnnouncementItemById(getContentID());
	adventureId = mItem->adventureId;
	mTitle = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mAnnSubTitle"));
	if (mTitle)
	{
		mTitle->setString(mItem->title.c_str());
	}
	CCB_FUNC(getContainerNode(),"mAnnSubNum",CCLabelBMFont,setString(mItem->idName.c_str()));
	mTimeMsg = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mTimeMsg"));
	if (mTimeMsg)
	{
		if(mItem->timeMsg=="none")
		{
			mTimeMsg->setString("");
		}
		else
		{
			mTimeMsg->setString(mItem->timeMsg.c_str());
		}
	}

	mMsg = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mAnnSubMsg"));
	if(mMsg)
	{
		std::string des;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("AnnouncementPageDescribe"));
		GameMaths::stringAutoReturn(mItem->msg, des, width, lines);
		mMsg->setString(des.c_str());
	}
	mBtnNode=dynamic_cast<CCNode*>(mContentContainer->getVariable("mButtonNode"));
	if (mItem->isHaveButton==1)
	{
		if(mBtnNode)
		{
			mBtnNode->setVisible(false);
		}
		cocos2d::CCLabelBMFont* labelFont = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mAnnSubBtnText"));
		labelFont->setString("");
	}
	else
	{
		mBtnNode->setVisible(true);
		cocos2d::CCLabelBMFont* labelFont = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mAnnSubBtnText"));
		if (labelFont)
		{
			labelFont->setString(mItem->buttonText.c_str());
		}
	}
	mTop=dynamic_cast<CCNode* >(getObjectByVariableName("mTopNode"));
	mMiddle=dynamic_cast<CCNode* >(getObjectByVariableName("mMiddleNode"));
	mEnd=dynamic_cast<CCNode* >(getObjectByVariableName("mEndNode"));
	if((mMsg->getContentSize().height+mTimeMsg->getContentSize().height+50.0f)>mMiddle->getContentSize().height*mMiddle->getScaleY())
	{
		_resizePage();
	}
	mTimeMsg->setPositionY(mEnd->getPositionY()+mEnd->getContentSize().height);
	CCB_FUNC(getContainerNode(),"mAnnSubNum",CCLabelBMFont,setPositionY(mTop->getContentSize().height+mTop->getPositionY()-33.0f));
}

void AnnouncementContent::_resizePage()
{
	float h=(mMsg->getContentSize().height+mTimeMsg->getContentSize().height+50.0f);
	mMiddle->setScaleY(h/mMiddle->getContentSize().height);
	mMsg->setPositionY(mMiddle->getPositionY()+mMiddle->getContentSize().height*mMiddle->getScaleY()*mMsg->getAnchorPoint().y-mTimeMsg->getContentSize().height);
	mTimeMsg->setPositionY(mMiddle->getPositionY()+mMiddle->getContentSize().height*mMiddle->getScaleY()-10);
	mTop->setPositionY(mMiddle->getPositionY()+mMiddle->getContentSize().height*mMiddle->getScaleY()-15);
	mTitle->setPositionY(mTop->getPositionY()+mTop->getContentSize().height*0.8f);
	this->getContainerNode()->setContentSize(CCSize(this->getContainerNode()->getContentSize().width,mTop->getContentSize().height+mMiddle->getContentSize().height*mMiddle->getScaleY()+mEnd->getContentSize().height-15));
}

void AnnouncementContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if (itemName == "onGoPage")
	{
		if(mItem->btnType==1)
		{
			int _openType=StringConverter::parseInt(VaribleManager::Get()->getSetting("OpenAnnouncementStatus"));
			if(_openType==1)
			{
				libOS::getInstance()->openURL(mItem->buttonGoPage);
			}
			else
			{
				libPlatformManager::getPlatform()->openBBS();
			}
		}
		else
		{
			
			if(mItem->changeOrPush==1)
			{
				BlackBoard::Get()->ToAdventruePageType = 0;
				MsgMainFramePushPage msg;
				msg.pageName = mItem->buttonGoPage;
				MessageManager::Get()->sendMessage(&msg);
			}
			else if(mItem->changeOrPush==0)
			{
				BlackBoard::Get()->ToAdventruePageType = adventureId;
				MsgMainFrameChangePage msg;
				msg.pageName = mItem->buttonGoPage;
				MessageManager::Get()->sendMessage(&msg);
			}
			MsgMainFramePopPage popMsg;
			popMsg.pageName = "AnnouncementPage";
			MessageManager::Get()->sendMessage(&popMsg);
		}
	}
}

cocos2d::CCObject * AnnouncementContent::getObjectByVariableName(const std::string& variableName)
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

AnnouncementContent::AnnouncementContent( unsigned int id) : ContentBase(id)
{
}


AnnouncementPage::AnnouncementPage(void)
{
	members = NULL;
	memberRootNode = NULL;
	mContentList.clear();
}


AnnouncementPage::~AnnouncementPage(void)
{
}

void AnnouncementPage::Enter( MainFrame* )
{
	rebuildAllItem();
}

void AnnouncementPage::Execute( MainFrame* )
{
}

void AnnouncementPage::Exit( MainFrame* )
{
	clearAllItem();
}

void AnnouncementPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
}

void AnnouncementPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if (itemName == "onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "AnnouncementPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void AnnouncementPage::load( void )
{
	loadCcbiFile("AnnouncementPage.ccbi");
	members = dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mAnnMsgContent"));
	memberRootNode = members->getContainer();
}

void AnnouncementPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void AnnouncementPage::clearAllItem( void )
{
	ContentList::iterator it = mContentList.begin();
	
	while (it != mContentList.end())
	{
		delete (*it);
		++it;
	}

	ContentList list;
	mContentList.swap(list);
	memberRootNode->removeAllChildren();
}

void AnnouncementPage::buildItem( void )
{
	mHeight=0.0f;
	AnnouncementTableManager::AnnouncementItemListIterator it = AnnouncementTableManager::Get()->getAnnouncementItemListIterator();
	bool isHavAnnouncement=false;
	while(it.hasMoreElements())
	{
		AnnouncementItem* item=it.getNext();
		
		long int startTimeStamp = item->startTime;
		long int endTimeStamp = item->endTime;

		time_t now(0);
		time(&now);

		if(now>= startTimeStamp && now<= endTimeStamp)
		{
			isHavAnnouncement=true;
			AnnouncementContent* content = new AnnouncementContent(item->itemId);
			content->init();
			addItem(content);
		}
	}
	if(!isHavAnnouncement)
	{
		MsgMainFramePopPage msg;
		msg.pageName = "AnnouncementPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void AnnouncementPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	memberRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,mHeight);
	mHeight+=item->getContainerNode()->getContentSize().height;
	item->getContainerNode()->setPosition(pos);
	CCLOG("addItem %f",item->getContainerNode()->getContentSize().height);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, mHeight);
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void AnnouncementPage::removeItem( ContentBase* item )
{
	ContentList::iterator it = mContentList.begin();

	while(it != mContentList.end())
	{
		if ((*it) == item)
		{
			memberRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(it);
		}
		++it;
	}
}
