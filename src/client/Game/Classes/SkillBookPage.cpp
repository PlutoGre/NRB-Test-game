
#include "stdafx.h"

#include "SkillBookPage.h"
#include "SkillHandInfoPage.h"
USING_NS_CC;

void SkillBookPage::refreshContent( void )
{
	mAdventureItemInfo=ServerDateManager::Get()->getAdventureItemInfoByID(getContentID());
	mSkillTable=SkillBookTableManager::Get()->getSkillTableBySkillId(mAdventureItemInfo->itemId);
	mSkill=SkillTableManager::Get()->getSkillItemByID(mAdventureItemInfo->itemId);
	mSkillPic=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mBookPic"));
	mSkillName=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mBookName"));
	mSkillCount=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mSkillNum"));
	mShowList.clear();
	SkillBookTable::SkillBookList::iterator it=mSkillTable->mSkillBookItemList.begin();
	for(int m=0,n=0;it!=mSkillTable->mSkillBookItemList.end();++it,++n)
	{
		if(mSkillTable->getSkillBookItemSize()==3)
		{
			m=n*2+1;
		}
		else if(mSkillTable->getSkillBookItemSize()==4)
		{
			m=n+2;
			if(m>=4)
			{
				m+=1;
			}
		}
		else if(mSkillTable->getSkillBookItemSize()==6)
		{
			m=n+1;
		}
		mShowList.insert(SkillBookShowList::value_type(m,it->second->itemID));
	}

	for(int i=1;i<=SkillBookMaxNum;++i)
	{
		char icon[128];
		sprintf(icon,"mPiece%d",i);
		CCB_FUNC(this->getContainerNode(),icon,CCSprite,setVisible(false));
		char num[128];
		sprintf(num,"mBookNum%d",i);
		CCB_FUNC(this->getContainerNode(),num,CCLabelBMFont,setVisible(false));
	}
	mSkillCount->setString(StringConverter::toString(0).c_str());
	mSkillName->setString(mSkill->name.c_str());
	mSkillPic->setTexture(mSkill->iconPic.c_str());
}

void SkillBookPage::refreshPage()
{
	mSkillName->setString(mSkill->name.c_str());
	mSkillPic->setTexture(mSkill->iconPic.c_str());
	int skillCount=0;
	SkillInfoListIterator itr = ServerDateManager::Get()->getSkillIterator();
	while(itr.hasMoreElements())
	{
		unsigned int id = itr.getNext()->itemid;
		if(id==mSkill->itemID)
		{
			skillCount++;
		}
	}
	CCB_FUNC(this->getContainerNode(),"mSBFrame",CCMenuItemImage,setNormalImage(getFrameNormalSpirte(mSkill->quality)));
	mSkillCount->setString(StringConverter::toString(skillCount).c_str());
	for(int i=1;i<=SkillBookMaxNum;++i)
	{
		char frame[128];
		sprintf(frame,"mFrame%d",i);
		char icon[128];
		sprintf(icon,"mPiece%d",i);
		char num[128];
		sprintf(num,"mBookNum%d",i);
		if(mShowList.find(i)!=mShowList.end())
		{
			CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setVisible(true));
			CCB_FUNC(this->getContainerNode(),num,CCLabelBMFont,setVisible(true));
			CCB_FUNC(this->getContainerNode(),icon,CCSprite,setVisible(true));
			const SkillBookTable::SkillBookItem* bookItem=SkillBookTableManager::Get()->getSkillBookItemByID(mSkill->itemID,mShowList.find(i)->second);
			CCB_FUNC(this->getContainerNode(),icon,CCSprite,setTexture(bookItem->iconPic.c_str()));
			CCB_FUNC(this->getContainerNode(),num,CCLabelBMFont,setString(StringConverter::toString(getSkillPartCountByPartId(mShowList.find(i)->second)).c_str()));
		}
		else
		{
			CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setVisible(false));
			CCB_FUNC(this->getContainerNode(),num,CCLabelBMFont,setVisible(false));
			CCB_FUNC(this->getContainerNode(),icon,CCSprite,setVisible(false));
		}
	}

}

void SkillBookPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

}

void SkillBookPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(!getIsInitData())
	{
		return;
	}
	else
	{
		unsigned int index=0;
		bool isBookFrame=false;
		if(itemName=="onPieceBook")
		{
			if(checkSkillPartEnough())
			{
				OPPieceSkillBook msg;
				msg.set_version(1);
				msg.set_skillid(mAdventureItemInfo->itemId);
				PacketManager::Get()->sendPakcet(OPCODE_PIECE_SKILL_BOOK_C, &msg);
			}
			else
			{
				MSG_BOX(Language::Get()->getString("@SkillBookNotEnough"));
			}
		}
		else if(itemName=="onFrame1")
		{
			index=1;
			isBookFrame=true;
		}
		else if(itemName=="onFrame2")
		{
			index=2;
			isBookFrame=true;
		}
		else if(itemName=="onFrame3")
		{
			index=3;
			isBookFrame=true;
		}
		else if(itemName=="onFrame4")
		{
			index=4;
			isBookFrame=true;
		}
		else if(itemName=="onFrame5")
		{
			index=5;
			isBookFrame=true;
		}
		else if(itemName=="onFrame6")
		{
			index=6;
			isBookFrame=true;
		}
		else if(itemName=="onSkillCenter")
		{
			ShowSkillInfoPopup(mSkill->itemID,true);
			isBookFrame=false;
		}
		if(isBookFrame)
		{
			if(mShowList.find(index)!=mShowList.end())
			{
				unsigned int id=mShowList.find(index)->second;
				if(getSkillPartCountByPartId(id)>0)
				{
					MSG_BOX(Language::Get()->getString("@SkillBookAlreadyHave"));
				}
				else
				{
					BlackBoard::Get()->setSkillBookInfo(id,mSkill->itemID);
					MsgMainFramePushPage msg;
					msg.pageName = "SkillBookLootPage";
					MessageManager::Get()->sendMessage(&msg);
				}
			}
		}
	}
}

unsigned int SkillBookPage::getSkillPartCountByPartId(unsigned int id)
{
	const SkillBookInfo::SkillBookItem* bookItem=ServerDateManager::Get()->getSkillBookItemBySkillIdAndPartId(mSkill->itemID,id);
	return bookItem->partCount;
}

bool SkillBookPage::checkSkillPartEnough()
{
	bool isCanPiece=true;
	SkillBookShowList::iterator it=mShowList.begin();
	for(;it!=mShowList.end();++it)
	{
		unsigned int id=it->second;
		if(getSkillPartCountByPartId(id)<=0)
		{
			isCanPiece=false;
			break;
		}
	}
	return isCanPiece;
}

void SkillBookPage::showSuccessDialog()
{
	CCLOG("BoundlessHolePage showSuccessDialog");
}

void SkillBookPage::loadData()
{
	mIsShow=true;
	if(!getIsInitData())
	{
		OPGetSkillBookInfo msg;
		msg.set_version(1);
		msg.set_skillid(mAdventureItemInfo->itemId);
		PacketManager::Get()->sendPakcet(OPCODE_GET_SKILL_BOOK_INFO_C, &msg);
	}
}

void SkillBookPage::updatePage(bool isShow,float dt)
{
	mIsShow=isShow;
	if(getIsInitData())
	{
		static float durationTime=0;
		if(durationTime>1.0f||durationTime==0.0f)
		{
			durationTime -=1.0f;
			refreshPage();
		}
		durationTime+=GamePrecedure::Get()->getFrameTime();
	}
}

void SkillBookPage::exitPage()
{

}
