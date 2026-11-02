
#include "stdafx.h"

#include "PackagePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "MessageBoxPage.h"
#include "GameMaths.h"
#include "PropInfoPage.h"
#include "GiftPreviewPage.h"

REGISTER_PAGE("PackagePage",PackagePage);
USING_NS_CC;

PackageItem::PackageItem( unsigned int id )
	:mID(id)
{
	if(ToolTableManager::Get()->getToolItemByID(ServerDateManager::Get()->getUserToolInfoByID(mID)->itemid)->isStone == 1 ){
		mItemContainer =CCBManager::Get()->createAndLoad("PackageStoneContent.ccbi");
	}else{
		mItemContainer =CCBManager::Get()->createAndLoad("PackageContent.ccbi");
	}
	mItemContainer->setListener(this);
	mItemContainer->retain();
	refreshItem();
}

PackageItem::~PackageItem()
{
	mItemContainer->release();
}

void PackageItem::refreshItem( void )
{
	const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(mID);
	if(!userToolInfo)
	{
		return;
	}
	const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(userToolInfo->itemid);
	if(!toolItem)
	{
		return;
	}
	if(toolItem->isStone == 1 ){ 
		const StoneItem * stoneItem = StoneTableManager::Get()->getStoneItemByID(toolItem->itemID);

		CCB_FUNC(mItemContainer,"mStoneName",CCLabelBMFont,setString(toolItem->name.c_str()));
		CCB_FUNC(mItemContainer,"mStoneLevel",CCLabelBMFont,setString(StringConverter::toString(stoneItem->level).c_str()));
		CCLabelBMFont* mAttackNum = dynamic_cast<CCLabelBMFont*>(mItemContainer->getVariable("mAttackNum"));
		if(mAttackNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_2");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->attackBuff).c_str());
			mAttackNum->setString(buff);
		}
		CCLabelBMFont* mDefenseNum = dynamic_cast<CCLabelBMFont*>(mItemContainer->getVariable("mDefenseNum"));
		if(mDefenseNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_3");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->defendBuff).c_str());
			mDefenseNum->setString(buff);
		}
		CCLabelBMFont* mHPNum = dynamic_cast<CCLabelBMFont*>(mItemContainer->getVariable("mHPNum"));
		if(mHPNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_1");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->bloodBuff).c_str());
			mHPNum->setString(buff);
		}
		CCB_FUNC(mItemContainer,"mPropNum",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mItemContainer,"mUpGradeButtonTxt",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mItemContainer,"mHeadPic",CCSprite,initWithFile(stoneItem->bigIconPic.c_str()));
		CCB_FUNC(mItemContainer,"mUpgradeButtonTxt",CCLabelBMFont,setString(toolItem->buttonString.c_str()));
//		CCB_FUNC(mItemContainer,"mPackButton",CCMenuItemImage,initWithFile(stoneItem->iconPic.c_str()));

	}
	CCSprite* prop=dynamic_cast<CCSprite* >(mItemContainer->getVariable("mProp"));
	if(prop)
	{
		prop->setTexture(toolItem->iconPic.c_str());
	}
	CCLabelBMFont* propName=dynamic_cast<CCLabelBMFont* >(mItemContainer->getVariable("mPropName"));
	if(propName)
	{
		propName->setString(Language::Get()->getString(toolItem->name).c_str());
	}
	CCLabelBMFont* propDes=dynamic_cast<CCLabelBMFont* >(mItemContainer->getVariable("mPropDes"));
	if(propDes)
	{
		std::string describe;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("PackagePageContent_Desc"));
		GameMaths::stringAutoReturn(toolItem->describe,describe,width,lines);
		propDes->setString(describe.c_str());
	}
	CCLabelBMFont* propNum=dynamic_cast<CCLabelBMFont* >(mItemContainer->getVariable("mPropNum"));
	if(propNum)
	{
		propNum->setString(StringConverter::toString(userToolInfo->count).c_str());
	}
	CCLabelBMFont* propUseBtnTxt = dynamic_cast<CCLabelBMFont* >(mItemContainer->getVariable("mPackButtonTxt"));
	if(propUseBtnTxt)
	{
		if(toolItem->buttonString!="none")
		{
			propUseBtnTxt->setString(Language::Get()->getString(toolItem->buttonString).c_str());
			CCB_FUNC(mItemContainer,"mPackButton",CCMenuItem,setVisible(true));
			UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
			unsigned int vipLimit = StringConverter::parseInt(VaribleManager::Get()->getSetting("ContinueOpenBoxVIPLimit"));
			if(userBasicInfo.viplevel >= vipLimit){
				CCB_FUNC(mItemContainer,"mPackButton2",CCMenuItem,setVisible(true));
				CCB_FUNC(mItemContainer,"mPackButtonTxt2",CCLabelBMFont,setVisible(true));
				
				int count = 0;
				const unsigned int pairTypeId = ResManager::Get()->getTreasurePairType(toolItem->itemID);
				if(pairTypeId!=0)
				{
					const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(pairTypeId);
					const UserToolInfo * info2 = ServerDateManager::Get()->getUserToolInfoByItemId(toolItem->itemID);
					if(info)
					{
						count = info->count;
					}
					if(count>info2->count){
						count = info2->count;
					}
					if(count>StringConverter::parseInt(VaribleManager::Get()->getSetting("ContinueOpenBoxLimit"))){
						count = StringConverter::parseInt(VaribleManager::Get()->getSetting("ContinueOpenBoxLimit"));
					}
				}

				if(count>0){
					std::list<std::string> strList;
					strList.push_back(StringConverter::toString(count));
					CCB_FUNC(mItemContainer,"mPackButtonTxt2",CCLabelBMFont,setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ContinueOpenBox"),&strList).c_str()));

				}else{
					CCB_FUNC(mItemContainer,"mPackButton2",CCMenuItem,setVisible(false));
					CCB_FUNC(mItemContainer,"mPackButtonTxt2",CCLabelBMFont,setVisible(false));

				}

			}else{
				CCB_FUNC(mItemContainer,"mPackButton2",CCMenuItem,setVisible(false));
				CCB_FUNC(mItemContainer,"mPackButtonTxt2",CCLabelBMFont,setVisible(false));

			}
			propUseBtnTxt->setVisible(true);
		}
		else
		{
			propUseBtnTxt->setVisible(false);
			CCMenuItem* propUseBtn=dynamic_cast<CCMenuItem* >(mItemContainer->getVariable("mPackButton"));
			propUseBtn->setVisible(false);
			CCB_FUNC(mItemContainer,"mPackButton2",CCMenuItem,setVisible(false));
			CCB_FUNC(mItemContainer,"mPackButtonTxt2",CCLabelBMFont,setVisible(false));
		}
		
	}
}

void PackageItem::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	//todo
	if(itemName=="onUseItem"||itemName=="onUseItem2")
	{
		const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(mID);
		if(!userToolInfo)
		{
			return;
		}
		const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(userToolInfo->itemid);
		if(!toolItem)
		{
			return;
		}

		if(toolItem->isStone == 1 ){

			if(StoneTableManager::Get()->getStoneItemByID(toolItem->itemID)->nextLevelID == 0){
				
				MSG_BOX(Language::Get()->getString("@StoneTopLevel"));

			}else{

				BlackBoard::Get()->currentStone = toolItem->itemID;
				BlackBoard::Get()->currentStoneMId = userToolInfo->id;

				MsgMainFramePushPage msg;
				msg.pageName="StoneUpgradePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			return;
		}
		
		if(toolItem->operationType==POPUP_OPRATION)
		{
			MsgMainFramePushPage msg;
			msg.pageName=toolItem->jumpPage;
			MessageManager::Get()->sendMessage(&msg);
		}
		else if(toolItem->operationType==ADD_OPRATION||toolItem->operationType==OPEN_OPRATION)
		{
			const unsigned int pairTypeId = ResManager::Get()->getTreasurePairType(toolItem->itemID);
			if(pairTypeId!=0)
			{
				const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(pairTypeId);
				if(info==NULL||info->count<=0)
				{
					//TODO Alert Numer of (ToolTableManager::Get()->getToolItemByID(toolItem->togetherUseItemID)->name) is not enough! 
					std::list<std::string> strList;
					strList.push_back(ToolTableManager::Get()->getToolItemByID(pairTypeId)->name);
					MSG_BOX(GameMaths::replaceStringWithStringList(Language::Get()->getString("@NeedTreasureCanOpen"),&strList).c_str());
					return;
				}
			}
			if(toolItem->itemID == TIAOZHANSHU)
			{
				if(ServerDateManager::Get()->getUserArenaInfo().arenaNum+3>ServerDateManager::Get()->getUserBasicInfo().viplevel+4)
				{
					MSG_BOX(Language::Get()->getString("@ArenaNumWillOverRanging"));
					return;
				}
			}
			if(toolItem->itemID==HAIXIANFAN)
			{
				const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
				if(info.todayAddedPowerNum>=ResManager::Get()->getVipDailyLimit(info.viplevel))
				{
					std::string s = Language::Get()->getString("@VipAddPowerLimit");
					char buff[128] = {0};
					sprintf(buff , s.c_str() , info.viplevel,ResManager::Get()->getVipDailyLimit(info.viplevel));
					MSG_BOX(buff);
					return;
				}
			}
			if(toolItem->itemID==JISUZHEN)
			{
				const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
				if(info.todayAddedVitalityNum>=ResManager::Get()->getVipDailyLimit(info.viplevel))
				{
					std::string s = Language::Get()->getString("@VipAddVitalityLimit");
					char buff[128] = {0};
					sprintf(buff , s.c_str() , info.viplevel,ResManager::Get()->getVipDailyLimit(info.viplevel));
					MSG_BOX(buff);
					return;
				}
			}
			if ( ResManager::Get()->getResMainType(toolItem->itemID) == WINE_TYPE )
			{
				UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
				bool nobodyCanAddExp = true;
				while( itr.hasMoreElements() ) 
				{
					UserBattleInfo* userBattleInfo = dynamic_cast<UserBattleInfo*>(itr.getNext());
					if ( userBattleInfo->discipleId == 0 ) 
						continue;
					UserDiscipleInfo* userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByID(userBattleInfo->discipleId);
					if ( userDiscipleInfo->level < ServerDateManager::Get()->getUserBasicInfo().level * 3 )
					{
						nobodyCanAddExp = false;
						break;
					}
				}
				if ( nobodyCanAddExp )
				{
					MSG_BOX(Language::Get()->getString("@noDiscipleCanAddExp"));
					return;
				}
			}

			if(itemName=="onUseItem2"){
				if(toolItem->operationType==ADD_OPRATION||toolItem->operationType==OPEN_OPRATION)
				{
					const unsigned int pairTypeId = ResManager::Get()->getTreasurePairType(toolItem->itemID);
					if(pairTypeId!=0)
					{
						const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(pairTypeId);
						const UserToolInfo * info2 = ServerDateManager::Get()->getUserToolInfoByItemId(toolItem->itemID);

						int count = info->count;
						if(count>info2->count){
							count = info2->count;
						}
						if(count>StringConverter::parseInt(VaribleManager::Get()->getSetting("ContinueOpenBoxLimit"))){
							count = StringConverter::parseInt(VaribleManager::Get()->getSetting("ContinueOpenBoxLimit"));
						}
						if(count>0){
							ResManager::Get()->useToolByIDAndCount(mID,toolItem->itemID,count);
						}
					}
				}
			}else{
				ResManager::Get()->useToolByIDAndCount(mID,toolItem->itemID,1);
			}
		}
		else if(toolItem->operationType==REDIRECT_OPTION)
		{
			MsgMainFrameChangePage msg;
			msg.pageName=toolItem->jumpPage;
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onShowPropInfo1" || itemName=="onShowPropInfo")
	{
		const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(mID);
		if(!userToolInfo)
		{
			return;
		}
		const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(userToolInfo->itemid);
		if(!toolItem)
		{
			return;
		}
		if(toolItem->includeOther.empty())
		{
			PropInfoPageShow(mID,PROP_USE,false);
		}else
		{
			CCBContainer* ccb = CCBManager::Get()->getPage("GiftPreviewPage");
			if(ccb)
			{
				GiftPreviewPage* page = dynamic_cast<GiftPreviewPage*>(ccb);
				if(page)
				{
					page->setTitle("@GiftPackPreviewText");
					GiftPreviewPage::PACKCONTENT_LIST _list;
					ResInfoMap::const_iterator it = toolItem->includeOther.begin();
					for(;it!=toolItem->includeOther.end();++it)
					{
						GiftPreviewPage::PackContent content(it->second->name,it->second->icon,it->second->count,it->second->quality);
						_list.push_back(content);
					}
					page->setContent(_list);
					MsgMainFramePushPage msg;
					msg.pageName = "GiftPreviewPage";
					MessageManager::Get()->sendMessage(&msg);
				}
			}
		}
		
	}
	
}
PackagePage::PackagePage(void)
	:mExpbarInitScale(-1.0f)
{
}

PackagePage::~PackagePage(void)
{
}

void PackagePage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	rebuildAllItem();
	refreshTitle();
}

void PackagePage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
}

void PackagePage::Execute( MainFrame* )
{

}

void PackagePage::load( void )
{
	loadCcbiFile("PackagePage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();

	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
}

bool PackagePage::_sSort(unsigned int id1,unsigned int id2)
{
	return ServerDateManager::Get()->getUserToolInfoByID(id1)->itemid>ServerDateManager::Get()->getUserToolInfoByID(id2)->itemid;
}

void PackagePage::buildItem( void )
{
	UserToolInfoListIterator itr = ServerDateManager::Get()->getToolIterator();
	std::list<unsigned int> myList;
	while(itr.hasMoreElements())
	{
		UserToolInfo* info = itr.getNext();
		if(info->count>0)
		{
			const ToolItem* item = ToolTableManager::Get()->getToolItemByID(info->itemid);
			if(item!=NULL)
			{
				if(item->isStone == 1){
				}
				if(item->showInBag == 0){
					continue;
				}

				unsigned int id = info->id;
				myList.push_back(id);
			}
			
		}
	}
	myList.sort(&_sSort);
	std::list<unsigned int >::reverse_iterator it=myList.rbegin();
	for(;it!=myList.rend();++it)
	{
		PackageItem * item = new PackageItem(*it);
		addItem(item);
	}
	myList.clear();
}

void PackagePage::addItem( PackageItem* item )
{
	mPackegeItemList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mPackegeItemList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mPackegeItemList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void PackagePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void PackagePage::clearAllItem( void )
{
	PackegeItemList::iterator itr = mPackegeItemList.begin();
	while(itr != mPackegeItemList.end())
	{
		delete (*itr);
		++itr;
	}
// 	PackegeItemList list;
// 	mPackegeItemList.swap(list);
	PackegeItemList().swap(mPackegeItemList);
	membersRootNode->removeAllChildren();
}

void PackagePage::refreshAllItem( void )
{
	PackegeItemList::iterator itr = mPackegeItemList.begin();
	while(itr != mPackegeItemList.end())
	{
		(*itr)->refreshItem();
		++itr;
	}
}

void PackagePage::removeItem( PackageItem* item )
{
	PackegeItemList::iterator itr = mPackegeItemList.begin();
	while(itr != mPackegeItemList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mPackegeItemList.erase(itr);
		}
		++itr;
	}
}

void PackagePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{
	if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void PackagePage::refreshTitle( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCLabelBMFont* exp = dynamic_cast<CCLabelBMFont* >(getVariable("mExp"));
	if(exp)
	{
		exp->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
	}
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar)
	{
		if(mExpbarInitScale<0)
			mExpbarInitScale = expbar->getScaleX();

		float expscale = 0;
		if(ServerDateManager::Get()->getNextLevelExp()>0)
			expscale = ServerDateManager::Get()->getUserExpPercent();
		if(expscale>1.0f)expscale=1.0f;
		expbar->setScaleX(expscale*mExpbarInitScale);
	}
	CCLabelBMFont* gold = dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	if(gold)
	{
		gold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	}
	CCLabelBMFont* ti = dynamic_cast<CCLabelBMFont* >(getVariable("mTi"));
	if(ti)
	{
		ti->setString((StringConverter::toString(userBasicInfo.power)+"/"+StringConverter::toString(userBasicInfo.maxpower)).c_str());
	}
	CCLabelBMFont* qi = dynamic_cast<CCLabelBMFont* >(getVariable("mQi"));
	if(qi)
	{
		qi->setString((StringConverter::toString(userBasicInfo.vitality)+"/"+StringConverter::toString(userBasicInfo.maxvitality)).c_str());
	}
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(userBasicInfo.level).c_str()));
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{

		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			vipLevel->setVisible(true);
			vipLevel->initWithFile(vipImage.c_str());
		}else
		{
			vipLevel->setVisible(false);
		}
	}
}
void PackagePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "PackagePage")
		{
			posOffset = members->getContentOffset();
			rebuildAllItem();
			refreshTitle();
			float yPos = members->getViewSize().height - members->getContentSize().height*members->getScaleY();
			if(posOffset.y<yPos)
			{
				posOffset.y = yPos;
			}
			members->setContentOffset(posOffset);
		}
	}
	else if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S)
		{
			refreshTitle();
		}
		else if(info->opcode==OPCODE_CHANE_NAMERET_S)
		{
			refreshTitle();
		}
	}
}
