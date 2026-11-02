
#include "stdafx.h"

#include "TeamBuffUpgradePage.h"
#include "MessageBoxPage.h"
REGISTER_PAGE("TeamBuffUpgradePage",TeamBuffUpgradePage);
USING_NS_CC;

void TeamBuffUpgradeContent::refreshContent( void )
{
	mMemberPic=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mPic"));
	mMemberFrame=dynamic_cast<CCMenuItemImage* >(this->getObjectByVariableName("mFrame"));
	mName=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mName"));
	mQuality=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mQuality"));
	mNumber=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mNumber"));
	mSoul=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mSoul"));
	mNumberBg=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mNumberbg"));
	if(mMemberFrame&&mMemberFrame->getParent())
	{
		CCMenu* menu=dynamic_cast<CCMenu* >(mMemberFrame->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
	}
	_refreshPage();
}

void TeamBuffUpgradeContent::_refreshPage()
{
	if(mType==TeamBufferDiscipleType)
	{
		_refreshDisciple();
	}
	else if(mType==TeamBufferSoulType)
	{
		_refreshSoul();
	}
};

void TeamBuffUpgradeContent::_refreshDisciple()
{
	Disciple disciple(getContentID());
	int count=BlackBoard::Get()->getUpTBDiscipleById(getContentID());
	if(mName)
	{
		mName->setString(disciple.name().c_str());
	}
	if(mQuality)
	{
		mQuality->setVisible(true);
		mQuality->initWithFile(disciple.getQualityImageFile().c_str());
	}
	if(mSoul)
	{
		mSoul->setVisible(false);
	}
	if(mMemberPic)
	{
		mMemberPic->initWithFile(disciple.iconPic().c_str());
	}
	if(mMemberFrame)
	{
		mMemberFrame->setNormalImage(disciple.getFrameNormalSpirte());
		mMemberFrame->setSelectedImage(disciple.getFrameSelectedSpirte());
		mMemberFrame->setDisabledImage(disciple.getFrameDisabledSpirte());
		if(count>0)
		{
			CCB_FUNC(getContainerNode(),"mDisable",CCLayer,setVisible(true));
			mMemberFrame->setEnabled(false);
			if(mNumberBg)
			{
				mNumberBg->setVisible(false);
			}
			if(mNumber)
			{
				mNumber->setVisible(false);
			}
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mDisable",CCLayer,setVisible(false));
			mMemberFrame->setEnabled(true);
			if(mNumberBg)
			{
				mNumberBg->setVisible(true);
			}
			if(mNumber)
			{
				mNumber->setString(StringConverter::toString(1).c_str());
				mNumber->setVisible(true);
			}
		}
	}
}

void TeamBuffUpgradeContent::_refreshSoul()
{
	Soul soul(getContentID());
	int count=soul.count();
	count-=BlackBoard::Get()->getUpTBSoulById(getContentID());
	if(mName)
	{
		mName->setString((soul.name()+Language::Get()->getString("@MPTabSoul")).c_str());
	}
	if(mQuality)
	{
		mQuality->setVisible(false);
	}
	if(mSoul)
	{
		mSoul->setVisible(true);
	}
	if(mNumber)
	{
		
		if(count > 0)
		{
			mNumber->setString(StringConverter::toString(count).c_str());
			mNumber->setVisible(true);
			if(mNumberBg)
			{
				mNumberBg->setVisible(true);
			}
		}
		else
		{
			if(mNumberBg)
			{
				mNumberBg->setVisible(false);
			}
			mNumber->setVisible(false);
		}
	}
	if(mMemberPic)
	{
		mMemberPic->initWithFile(soul.iconPic().c_str());
	}
	if(mMemberFrame)
	{
		mMemberFrame->setNormalImage(soul.getFrameNormalSpirte());
		mMemberFrame->setSelectedImage(soul.getFrameSelectedSpirte());
		mMemberFrame->setDisabledImage(soul.getFrameDisabledSpirte());
		if(count>0)
		{
			CCB_FUNC(getContainerNode(),"mDisable",CCLayer,setVisible(false));
			mMemberFrame->setEnabled(true);
		}
		else
		{
			CCB_FUNC(getContainerNode(),"mDisable",CCLayer,setVisible(true));
			mMemberFrame->setEnabled(false);
		}
	}
}
void TeamBuffUpgradeContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onUpgrade")
	{
		int itemId=-1;
		if(mType==TeamBufferDiscipleType)
		{
			const UserDiscipleInfo* item = ServerDateManager::Get()->getUserDiscipleInfoByID(getContentID());
			if(item)
			{
				itemId=item->itemid;
				if(DiscipleTableManager::Get()->getDiscipleItemByID(itemId)->quality == 1){
					MSG_BOX(Language::Get()->getString("@ADiscipleKilled"));
				}
			}
		}
		else
		{
			const UserSoulInfo* item = ServerDateManager::Get()->getUserSoulInfoByID(getContentID());
			if(item)
			{
				itemId=item->itemid;
				if(DiscipleTableManager::Get()->getDiscipleItemByID(itemId)->quality == 1){
					MSG_BOX(Language::Get()->getString("@ASoulKilled"));
				}
			}
		}
		if(itemId!=-1)
		{
			const DiscipleItem* disItem=DiscipleTableManager::Get()->getDiscipleItemByID(itemId);
			if(disItem)
			{
				const TeamBuffBoldnessItem* item=TeamBuffBoldnessTableManager::Get()->getTeamBuffBoldnessItemByQuality(disItem->quality);
				if(item)
				{
					TeamBuffAddBoldness(mType==TeamBufferDiscipleType,getContentID(),mType==TeamBufferDiscipleType?item->discipleBoldness:item->soulBoldness);
					_refreshPage();
				}
			}
			
		}
		
	}
}

cocos2d::CCObject * TeamBuffUpgradeContent::getObjectByVariableName(const std::string& variableName)
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

TeamBuffUpgradePage::TeamBuffUpgradePage(void)
{
	
}


TeamBuffUpgradePage::~TeamBuffUpgradePage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void TeamBuffUpgradePage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	_refreshPage();
	rebuildAllItem();
}

void TeamBuffUpgradePage::Exit( MainFrame* frame)
{
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
	BlackBoard::Get()->clearUpTBCounsumeInfo();
}

void TeamBuffUpgradePage::Execute( MainFrame* )
{

}

void TeamBuffUpgradePage::showPage(unsigned int id)
{
	MsgMainFrameChangePage msg;
	msg.pageName="TeamBuffUpgradePage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("TeamBuffUpgradePage");
	TeamBuffUpgradePage* page = dynamic_cast<TeamBuffUpgradePage*>(obj);
	if(page)
	{
		page->mId=id;
	}
}

void TeamBuffUpgradePage::AddBoldness(bool type,unsigned int id,unsigned int count)
{
	const TeamBuffInfo* teamBuffStats=ServerDateManager::Get()->getTeamBuffInfoById(mId);
	const TeamBuffItem* teamBuffItem;
	if(teamBuffStats)
	{
		teamBuffItem=TeamBuffTableManager::Get()->getTeamBuffItemByID(teamBuffStats->teamId);
	}
	int currLevel=0;
	if(teamBuffItem)
	{
		int currBoldness=teamBuffStats->currBoldness+BlackBoard::Get()->getCurrCashBoldness()+count;
		currLevel=teamBuffStats->teamLevel;
		int maxNum=TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(teamBuffStats->teamId,currLevel);
		while(currBoldness>=maxNum)
		{
			currLevel++;
			currBoldness-=maxNum;
			maxNum=TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(teamBuffStats->teamId,currLevel);
		}
	}
	const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();

	if(currLevel>info.level)
	{
		std::list<std::string> _list;
		_list.push_back(StringConverter::toString(info.level));
		std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@TeamBuffUpgradeMaxLimit"),&_list);
		MSG_BOX(replaced);
	}
	else
	{
		if(type)
		{
			BlackBoard::Get()->addUpTBDisciple(id,1,count);
		}
		else
		{
			BlackBoard::Get()->addUpTBSoul(id,1,count);
		}
		_refreshPage();
	}
}

cocos2d::CCObject* TeamBuffUpgradePage::getObjectByVariableName(const std::string& variableName)
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

void TeamBuffUpgradePage::loadData()
{
	/*OPSkillBookLoot msg;
	msg.set_version(1);
	msg.set_skillbookid(BlackBoard::Get()->getSkillBookId());
	mSkillItem=SkillTableManager::Get()->getSkillItemByID(BlackBoard::Get()->getSkillBookSkillId());
	if(mSkillItem&&mSkillName)
	{
	mSkillName->setString(mSkillItem->name.c_str());
	}
	PacketManager::Get()->sendPakcet(OPCODE_GET_SKILL_BOOK_LOOT_INFO_C, &msg);*/
}

void TeamBuffUpgradePage::load( void )
{
	loadCcbiFile("TeamBuffUpgrade.ccbi");
	mMembers =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContentMem"));
	membersRootNode = mMembers->getContainer();
	mSouls =  dynamic_cast<cocos2d::extension::CCScrollView*>(getObjectByVariableName("mContentSoul"));
	soulsRootNode = mSouls->getContainer();
	mBuffPic=dynamic_cast<cocos2d::CCSprite*>(getObjectByVariableName("mBuffPic"));
	mBuffTypePic1=dynamic_cast<cocos2d::CCSprite*>(getObjectByVariableName("mBuffTypePic1"));
	mBuffTypePic2=dynamic_cast<cocos2d::CCSprite*>(getObjectByVariableName("mBuffTypePic2"));
	mBuffName=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mBuffName"));
	mBuffLV=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mBuffLV"));
	mBuffLV1=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mBuffLV1"));
	mBuffCurrValue=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mBuffValue"));
	mBuffNextValue=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mBuffValueNew"));
	mSoulNum=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mSoulNum"));
	mMaxNum=dynamic_cast<cocos2d::CCLabelBMFont*>(getObjectByVariableName("mMaxNum"));
	mSquareBg=dynamic_cast<cocos2d::CCSprite*>(getObjectByVariableName("mSquareBg"));
	mSquarePic=dynamic_cast<cocos2d::CCSprite*>(getObjectByVariableName("mSquarePic"));
	//mSquareTop=dynamic_cast<cocos2d::CCSprite*>(getObjectByVariableName("mSquareTop"));
}

void TeamBuffUpgradePage::buildMemberItem( void )
{
	//UserDiscipleInfoListIterator itr = ServerDateManager::Get()->getDiscipleIterator();
	DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getDiscipleCollection(DiscipleFilter_default,DiscipleSort_qualityASC);
	DiscipleDateCollect::DiscipleIDCollect::iterator itr = collect.begin(); 
	int i = collect.size() - 1-ServerDateManager::Get()->getUserBattleSize();
	i=(i%2==0)?i+1:i;
	for(;itr != collect.end(); ++itr)
	{
		int id = (*itr);
		Disciple disciple(id);
		if(!disciple.isInBattle())
		{
			TeamBuffUpgradeContent * item = new TeamBuffUpgradeContent(id,TeamBufferDiscipleType);
			item->init();
			addMemberItem(item,i);
			--i;
		}
	}
	mMembers->forceRecaculateChildren();
}

void TeamBuffUpgradePage::addMemberItem( ContentBase* item ,unsigned int index)
{
	mMemberContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(item->getContainerNode()->getContentSize().width*(((mMemberContentList.size()-1)%2))+(mMembers->getViewSize().width-item->getContainerNode()->getContentSize().width*2)*0.25*(((mMemberContentList.size()-1)%2)==1?3:1)-20.0f,item->getContainerNode()->getContentSize().height*(floorf(index/2)));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(mMembers->getViewSize().width,item->getContainerNode()->getContentSize().height*(floorf((mMemberContentList.size()-1)/2)+1.0f));
	mMembers->setContentSize(size);
	mMembers->setContentOffset(ccp(0, mMembers->getViewSize().height - mMembers->getContentSize().height*mMembers->getScaleY()));
}

void TeamBuffUpgradePage::buildSoulItem( void )
{
	DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getSoulCollection(SoulFilter_default,SoulSort_qualityASC);
	DiscipleDateCollect::DiscipleIDCollect::iterator itr = collect.begin();
	int i = collect.size() - 1;
	i=(i%2==0)?i+1:i;
	for(;itr != collect.end(); ++itr, --i)
	{
		int id = (*itr);
		TeamBuffUpgradeContent * item = new TeamBuffUpgradeContent(id,TeamBufferSoulType);
		item->init();
		addSoulItem(item,i);
	}
	mSouls->forceRecaculateChildren();
}

void TeamBuffUpgradePage::addSoulItem( ContentBase* item,unsigned int index)
{
	mSoulContentList.push_back(item);
	soulsRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(item->getContainerNode()->getContentSize().width*(((mSoulContentList.size()-1)%2))+(mSouls->getViewSize().width-item->getContainerNode()->getContentSize().width*2)*0.25*(((mSoulContentList.size()-1)%2)==1?3:1)-20.0f,item->getContainerNode()->getContentSize().height*(floorf((index)/2)));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(mSouls->getViewSize().width,item->getContainerNode()->getContentSize().height*(floorf((mSoulContentList.size()-1)/2)+1.0f));
	mSouls->setContentSize(size);
	mSouls->setContentOffset(ccp(0, mSouls->getViewSize().height - mSouls->getContentSize().height*mSouls->getScaleY()));
}

void TeamBuffUpgradePage::rebuildAllItem( void )
{
	clearAllItem();
	buildMemberItem();
	buildSoulItem();
}

void TeamBuffUpgradePage::clearAllItem( void )
{
	ContentList::iterator itm = mMemberContentList.begin();
	while(itm != mMemberContentList.end())
	{
		delete (*itm);
		++itm;
	}
	ContentList memList;
	mMemberContentList.swap(memList);
	membersRootNode->removeAllChildren();

	ContentList::iterator its = mSoulContentList.begin();
	while(its != mSoulContentList.end())
	{
		delete (*its);
		++its;
	}
	ContentList soulList;
	mSoulContentList.swap(soulList);
	soulsRootNode->removeAllChildren();
}

void TeamBuffUpgradePage::refreshAllItem( void )
{
	ContentList::iterator itm = mMemberContentList.begin();
	while(itm != mMemberContentList.end())
	{
		(*itm)->refreshContent();
		++itm;
	}
	ContentList::iterator its = mSoulContentList.begin();
	while(its != mSoulContentList.end())
	{
		(*its)->refreshContent();
		++its;
	}
}

void TeamBuffUpgradePage::removeMemberItem( ContentBase* item )
{
	ContentList::iterator itr = mMemberContentList.begin();
	while(itr != mMemberContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mMemberContentList.erase(itr);
		}
		++itr;
	}
}

void TeamBuffUpgradePage::removeSoulItem( ContentBase* item )
{
	ContentList::iterator itr = mSoulContentList.begin();
	while(itr != mSoulContentList.end())
	{
		if((*itr) == item)
		{
			soulsRootNode->removeChild(item->getContainerNode());
			delete item;
			mSoulContentList.erase(itr);
		}
		++itr;
	}
}

void TeamBuffUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onOK")
	{
		const TeamBuffInfo* teamBuffStats=ServerDateManager::Get()->getTeamBuffInfoById(mId);
		bool isSelect=false;
		BlackBoard::UpTBConsumeMemberMapIterator::const_iterator itd=BlackBoard::Get()->getUpTBConsumeDiscipleMapIterator().begin();
		OPUpgradeTeamBuffInfo msg;
		while(itd!=BlackBoard::Get()->getUpTBConsumeDiscipleMapIterator().end())
		{
			isSelect=true;
			OPUpgradeTeamBuffInfo_CousumeItem* memberList=msg.add_memberlist();
			memberList->set_memberid(itd->first);
			++itd;
		}
		BlackBoard::UpTBConsumeMemberMapIterator::const_iterator its=BlackBoard::Get()->getUpTBConsumeSoulMapIterator().begin();
		while(its!=BlackBoard::Get()->getUpTBConsumeSoulMapIterator().end())
		{
			isSelect=true;
			OPUpgradeTeamBuffInfo_CousumeItem* memberList=msg.add_soullist();
			memberList->set_memberid(ServerDateManager::Get()->getUserSoulInfoByID(its->first)->itemid);
			memberList->set_count(its->second);
			++its;
		}
		if(isSelect)
		{
			msg.set_version(1);
			msg.set_teamid(teamBuffStats->teamId);
			PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_TEAM_BUFF_INFO_C, &msg);
		}
		else
		{
			MsgMainFrameChangePage msg;
			msg.pageName = "TeamBuffPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onGiveUp")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "TeamBuffPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void TeamBuffUpgradePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

void TeamBuffUpgradePage::onReceiveMassage(const GameMessage * message)
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_UPGRADE_TEAM_BUFF_INFORET_S)
			{
				BlackBoard::UpTBConsumeMemberMapIterator::const_iterator itd=BlackBoard::Get()->getUpTBConsumeDiscipleMapIterator().begin();
				while(itd!=BlackBoard::Get()->getUpTBConsumeDiscipleMapIterator().end())
				{
					ServerDateManager::Get()->removeDiscipleInfoById(itd->first);
					++itd;
				}
				BlackBoard::UpTBConsumeMemberMapIterator::const_iterator its=BlackBoard::Get()->getUpTBConsumeSoulMapIterator().begin();
				while(its!=BlackBoard::Get()->getUpTBConsumeSoulMapIterator().end())
				{
					ServerDateManager::Get()->increaseSoulInfoById(its->first,-its->second);
					++its;
				}
				TeamBuffInfo* teamBuffStats=ServerDateManager::Get()->getTeamBuffInfoById(mId);
				bool isLevel=false;
				const TeamBuffItem* teamBuffItem;
				if(teamBuffStats)
				{
					teamBuffStats->currBoldness+=BlackBoard::Get()->getCurrCashBoldness();
					int maxNum=TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(teamBuffStats->teamId,teamBuffStats->teamLevel);
					while(teamBuffStats->currBoldness>=maxNum)
					{
						isLevel=true;
						++teamBuffStats->teamLevel;
						teamBuffStats->currBoldness-=maxNum;
						maxNum=TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(teamBuffStats->teamId,teamBuffStats->teamLevel);
					}
					teamBuffItem=TeamBuffTableManager::Get()->getTeamBuffItemByID(teamBuffStats->teamId);
					std::list<std::string> _list;
					_list.push_back(teamBuffItem->name);
					if(isLevel)
					{
						_list.push_back(StringConverter::toString(teamBuffStats->teamLevel));
					}
					std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString((isLevel?"@TeamBuffUpgradeLevelRet":"@TeamBuffUpgradeRet")),&_list);
					MSG_BOX(replaced);
				}
	
				BlackBoard::Get()->setNeedRefreshTeamBuffPage(true);
				BlackBoard::Get()->clearUpTBCounsumeInfo();
				MsgMainFrameChangePage msg;
				msg.pageName = "TeamBuffPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
}

void TeamBuffUpgradePage::_refreshPage( void )
{
	const TeamBuffInfo* teamBuffStats=ServerDateManager::Get()->getTeamBuffInfoById(mId);
	const TeamBuffItem* teamBuffItem;
	if(teamBuffStats)
	{
		teamBuffItem=TeamBuffTableManager::Get()->getTeamBuffItemByID(teamBuffStats->teamId);
	}
	if(teamBuffItem)
	{
		int currBoldness=teamBuffStats->currBoldness+BlackBoard::Get()->getCurrCashBoldness();
		int currLevel=teamBuffStats->teamLevel;
		int maxNum=TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(teamBuffStats->teamId,currLevel);
		while(currBoldness>=maxNum)
		{
			currLevel++;
			currBoldness-=maxNum;
			maxNum=TeamBuffTableManager::Get()->getBoldnessByIdAndLevel(teamBuffStats->teamId,currLevel);
		}
		//float scaleX = (mSquareBg->getContentSize().width*mSquareBg->getScaleX()/maxNum/mSquarePic->getContentSize().width)*currBoldness;
		//CCActionInterval*  actionTo = CCScaleTo::create(0.3f, scaleX,1.0f);
		float scaleY = (float)currBoldness/(float)maxNum;
		CCActionInterval*  actionTo = CCScaleTo::create(0.3f,1.0f,scaleY);
		mSquarePic->runAction(actionTo);
		//mSquarePic->setScaleX();
		//mSquareTop->setPositionY(mSquarePic->getPositionY()+mSquareTop->getContentSize().width*mSquareTop->getScaleX()*mSquareTop->getAnchorPoint().x+mSquarePic->getContentSize().width*mSquarePic->getScaleX());
		if(mSoulNum)
		{
			mSoulNum->setString(StringConverter::toString(currBoldness).c_str());
		}
		if(mMaxNum)
		{
			mMaxNum->setString(StringConverter::toString(maxNum).c_str());
		}
		if(mBuffLV)
		{
			mBuffLV->setString(StringConverter::toString(currLevel).c_str());
		}
		if(mBuffLV1)
		{
			mBuffLV1->setString(StringConverter::toString(currLevel).c_str());
		}
		if(mBuffCurrValue)
		{
			mBuffCurrValue->setString(StringConverter::toString(TeamBuffTableManager::Get()->getAdditionValueByIdAndLevel(teamBuffStats->teamId,currLevel)).c_str());
		}
		if(mBuffNextValue)
		{
			mBuffNextValue->setString(StringConverter::toString(TeamBuffTableManager::Get()->getAdditionValueByIdAndLevel(teamBuffStats->teamId,currLevel+1)).c_str());
		}
		if(mBuffName)
		{
			mBuffName->setString(teamBuffItem->name.c_str());
		}
		if(mBuffPic)
		{
			mBuffPic->setTexture(teamBuffItem->iconPic.c_str());
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//mBuffPic->setTextureRect(rect);
		}
		if(mBuffTypePic1)
		{
			//CCTexture2D*tex = CCTextureCache::sharedTextureCache()->addImage(teamBuffItem->namePic.c_str());
			mBuffTypePic1->setTexture(teamBuffItem->namePic.c_str());
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//mBuffTypePic1->setTextureRect(rect);
		}
		if(mBuffTypePic2)
		{
			//CCTexture2D*tex = CCTextureCache::sharedTextureCache()->addImage(teamBuffItem->namePic.c_str());
			mBuffTypePic2->setTexture(teamBuffItem->namePic.c_str());
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//mBuffTypePic2->setTextureRect(rect);
		}
	}
}