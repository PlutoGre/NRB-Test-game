
#include "stdafx.h"

#include "StoneInlaidPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"
#include "MessageBoxPage.h"
#include "math.h"
#include "BuyStoneCountPage.h"
#include "GameMaths.h"
#include "BuyToolAlertPage.h"
				 

REGISTER_PAGE("StoneInlaidPage",StoneInlaidPage);
USING_NS_CC;

void StoneEquipContentItem::refreshContent(){
	Equip equip( BlackBoard::Get()->ShowEquip);


	if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status){
		//未开启	,颜色用灰色，图片用锁头
		CCB_FUNC(mContentContainer,"mAttribute",cocos2d::CCNode,setVisible(false));
		CCB_FUNC(mContentContainer,"mNoAttribute",cocos2d::CCNode,setVisible(true));
		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneItemBySign("locked")->bigIconPic.c_str()););
		CCB_FUNC(mContentContainer,"mStoneColor",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneColorItemBySign("grey")->bigIconPic.c_str()));
		
		CCLabelBMFont* mTxt = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mTxt"));
		if(mTxt){
			std::list<std::string> _list;
			_list.push_back(StringConverter::toString(StoneTableManager::Get()->getStoneHoleByID(mID)->needToolNum).c_str());
			std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@UserPunchHoleNum"),&_list);
			CCB_FUNC(mContentContainer,"mTxt",CCLabelBMFont,setString(replaced.c_str()));
		}
		
		CCB_FUNC(mContentContainer,"mStoneButton",cocos2d::CCLabelBMFont,setString(Language::Get()->getString("@punch").c_str()));

	}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("opened")->status){
		//已开启,颜色用正确的，图片不显示
		CCB_FUNC(mContentContainer,"mAttribute",cocos2d::CCNode,setVisible(false));
		CCB_FUNC(mContentContainer,"mNoAttribute",cocos2d::CCNode,setVisible(true));
		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"mStoneColor",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID))->bigIconPic.c_str()));
		CCB_FUNC(mContentContainer,"mTxt",cocos2d::CCLabelBMFont,setString(Language::Get()->getString("@noInlaid").c_str()));
		CCB_FUNC(mContentContainer,"mStoneButton",cocos2d::CCLabelBMFont,setString(Language::Get()->getString("@inlaid").c_str()));

	}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("inlaid")->status){
		CCB_FUNC(mContentContainer,"mAttribute",cocos2d::CCNode,setVisible(true));
		CCB_FUNC(mContentContainer,"mNoAttribute",cocos2d::CCNode,setVisible(false));
		//已镶嵌,颜色图片都有
		CCB_FUNC(mContentContainer,"mEquipLevel",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->level).c_str()));
		CCB_FUNC(mContentContainer,"mEquipBuffType",cocos2d::CCSprite,setTexture(VaribleManager::Get()->getSetting("EquipAttack_ImageFile" + StringConverter::toString(equip.type())).c_str()));

		if(equip.type() == 1){
			CCB_FUNC(mContentContainer,"mBuffValue",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->attackBuff).c_str()));
		}else if(equip.type() == 2){
			CCB_FUNC(mContentContainer,"mBuffValue",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->defendBuff).c_str()));
		}else if(equip.type() == 3){
			CCB_FUNC(mContentContainer,"mBuffValue",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->bloodBuff).c_str()));
		}

		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->bigIconPic.c_str()));
		CCB_FUNC(mContentContainer,"mStoneColor",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID))->bigIconPic.c_str()));
		CCB_FUNC(mContentContainer,"mStoneButton",cocos2d::CCLabelBMFont,setString(Language::Get()->getString("@removeStone").c_str()));

	}

}

void onPunchStoneHole_Func(){
	OPPunchStoneInfo punchStoneInfo;
	punchStoneInfo.set_version(1);
	punchStoneInfo.set_equipid(BlackBoard::Get()->ShowEquip);
	punchStoneInfo.set_position(BlackBoard::Get()->stonePosition);
	PacketManager::Get()->sendPakcet(OPCODE_PUNCH_STONEINFO_C,&punchStoneInfo);
}

void onRemoveStoneHole_Func(){
	OPRemoveStoneInfo removeStoneInfo;
	removeStoneInfo.set_version(1);
	removeStoneInfo.set_equipid(BlackBoard::Get()->ShowEquip);
	removeStoneInfo.set_position(BlackBoard::Get()->stonePosition);
	PacketManager::Get()->sendPakcet(OPCODE_REMOVE_STONEINFO_C,&removeStoneInfo);
}

void StoneEquipContentItem::inlaidFunction(){
	CCLOG("inlaidFunction");
}
void StoneEquipContentItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

	Equip equip( BlackBoard::Get()->ShowEquip);
	if(itemName == "onButten"){

		BlackBoard::Get()->stonePosition = mID;
		if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status){
			//打孔
			for(int i = 1 ;i < mID ;i ++){
				if(equip.stoneStatus(i) == StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status){

					MSG_BOX("@PunchHoleInOrder");
					return;
				}
			}

			CCBContainer* ccb = CCBManager::Get()->getPage("BuyStoneCountPage");
			if(ccb)
			{
				BuyStoneCountPage* page = dynamic_cast<BuyStoneCountPage*>(ccb);
				if(page)
				{
					page->setProductID(StoneTableManager::Get()->getStoneHoleByID(1)->status);
					page->setBuyOneOnly(false);
					page->needNum = StoneTableManager::Get()->getStoneHoleByID(mID)->needToolNum;

					const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByItemId(StoneTableManager::Get()->getStoneHoleByID(1)->status);
					const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(StoneTableManager::Get()->getStoneHoleByID(1)->status);

					if(userToolInfo){
						page->hasNum = userToolInfo->count;
					}else{
						page->hasNum = 0;
					}

					if(page->hasNum<page->needNum){
						MsgMainFramePushPage msg;
						msg.pageName = "BuyStoneCountPage";
						MessageManager::Get()->sendMessage(&msg);
					}else{
						
						CCBContainer* alertCcb = CCBManager::Get()->getPage("BuyToolAlertPage");

						BuyToolAlertPage* alertPage = dynamic_cast<BuyToolAlertPage*>(alertCcb);
						alertPage->setTitle(Language::Get()->getString("@RemoveStoneTitle"));
						if(toolItem!=NULL)
						{
							alertPage->setName(toolItem->name.c_str());
						}					
						std::list<std::string> replaceList;
						replaceList.push_back(StringConverter::toString(page->needNum));
						std::string outstr;
						int lines;
						int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("BuyStoneHoleTexts"));
						GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@PunchStoneText"),&replaceList),outstr,width,lines);
						alertPage->setDescription(outstr.c_str());

						alertPage->setNumber1(StringConverter::toString(page->hasNum).c_str());
						alertPage->setSure(Language::Get()->getString("@Confirm"));
						if(toolItem!=NULL)
						{
							alertPage->setPic(toolItem->iconPic.c_str());
						}
						alertPage->setSureFun(onPunchStoneHole_Func);

						MsgMainFramePushPage msg;
						msg.pageName = "BuyToolAlertPage";
						MessageManager::Get()->sendMessage(&msg);


					}
				}
			}



		}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("opened")->status){
			//镶嵌
			BlackBoard::Get()->stonePosition = mID;
			mOwnner->choosePage(2);
	
		}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("inlaid")->status){
			//拆除

			

			CCBContainer* ccb = CCBManager::Get()->getPage("BuyStoneCountPage");
			if(ccb)
			{
				BuyStoneCountPage* page = dynamic_cast<BuyStoneCountPage*>(ccb);
				if(page)
				{
					page->setProductID(StoneTableManager::Get()->getStoneHoleByID(2)->status);
					page->setBuyOneOnly(false);
					page->needNum = StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->needToolNum;

					const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByItemId(StoneTableManager::Get()->getStoneHoleByID(2)->status);
					const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(StoneTableManager::Get()->getStoneHoleByID(2)->status);

					if(userToolInfo){
						page->hasNum = userToolInfo->count;
					}else{
						page->hasNum = 0;
					}

					if(page->hasNum<page->needNum){
						MsgMainFramePushPage msg;
						msg.pageName = "BuyStoneCountPage";
						MessageManager::Get()->sendMessage(&msg);
					}else{
						BlackBoard::Get()->stonePosition = mID;
						CCBContainer* alertCcb = CCBManager::Get()->getPage("BuyToolAlertPage");

						BuyToolAlertPage* alertPage = dynamic_cast<BuyToolAlertPage*>(alertCcb);
						alertPage->setTitle(Language::Get()->getString("@RemoveStoneTitle"));
						alertPage->setName(toolItem->name.c_str());

						std::list<std::string> replaceList;
						replaceList.push_back(StringConverter::toString(page->needNum));
						std::string outstr;
						int lines;
						int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("BuyStoneHoleTexts"));
						GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@RemoveStoneText"),&replaceList),outstr,width,lines);
						alertPage->setDescription(outstr.c_str());

						alertPage->setNumber1(StringConverter::toString(page->hasNum).c_str());
						alertPage->setSure(Language::Get()->getString("@Confirm"));
						alertPage->setPic(toolItem->iconPic.c_str());
						alertPage->setSureFun(onRemoveStoneHole_Func);

						MsgMainFramePushPage msg;
						msg.pageName = "BuyToolAlertPage";
						MessageManager::Get()->sendMessage(&msg);



					}
				}
			}

		}
	}
}


void StoneInlaidContentItem::refreshContent(){
   	Equip equip( BlackBoard::Get()->ShowEquip);
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSelected"));
	mSelect->setNormalImage(::getContentUnselectedSpirte("Normal"));
	mSelect->setSelectedImage(::getContentUnselectedSpirte("Normal"));

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
	if(toolItem->isStone == 1){
		const StoneItem * stoneItem = StoneTableManager::Get()->getStoneItemByID(toolItem->itemID);

		CCB_FUNC(mContentContainer,"mStoneName",CCLabelBMFont,setString(toolItem->name.c_str()));
		CCB_FUNC(mContentContainer,"mStoneLevel",CCLabelBMFont,setString(StringConverter::toString(stoneItem->level).c_str()));
		CCLabelBMFont* mAttackNum = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mAttackNum"));
		if(mAttackNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_2");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->attackBuff).c_str());
			mAttackNum->setString(buff);
		}
		CCLabelBMFont* mDefenseNum = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mDefenseNum"));
		if(mDefenseNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_3");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->defendBuff).c_str());
			mDefenseNum->setString(buff);
		}
		CCLabelBMFont* mHPNum = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mHPNum"));
		if(mHPNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_1");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->bloodBuff).c_str());
			mHPNum->setString(buff);
		}
		CCB_FUNC(mContentContainer,"mNum",CCLabelBMFont,setString(StringConverter::toString(userToolInfo->count).c_str()));
		CCB_FUNC(mContentContainer,"mStonePic",CCSprite,setTexture(stoneItem->bigIconPic.c_str()));
		CCB_FUNC(mContentContainer,"mStoneColor",CCSprite,setVisible(false));
//		CCB_FUNC(mItemContainer,"mPackButton",CCMenuItemImage,initWithFile(stoneItem->iconPic.c_str()));
	}
}

void StoneInlaidContentItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){
	if(itemName == "onSelected"){
		mOwnner->setSellectedContent(this);
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
		mOwnner->itemId = toolItem->itemID;
	}

}

void StoneInlaidContentItem::setSellect(bool sellect){
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSelected"));
	if(sellect)
	{
		//mSelect->selected();
		mSelect->setNormalImage(::getContentSelectedSpirte("Normal"));
		mSelect->setSelectedImage(::getContentSelectedSpirte("Normal"));
	}
	else
	{
		//mSelect->unselected();
		mSelect->setNormalImage(::getContentUnselectedSpirte("Normal"));
		mSelect->setSelectedImage(::getContentUnselectedSpirte("Normal"));
	}

}

void StoneUpgradeContentItem::refreshContent(){
  	Equip equip( BlackBoard::Get()->ShowEquip);
	UserToolInfoListIterator itr = ServerDateManager::Get()->getToolIterator();

	const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(mID);
	if(!userToolInfo)
	{
		return;
	}
	toolItem = ToolTableManager::Get()->getToolItemByID(userToolInfo->itemid);
	if(!toolItem)
	{
		return;
	}
	if(toolItem->isStone == 1){
		const StoneItem * stoneItem = StoneTableManager::Get()->getStoneItemByID(toolItem->itemID);

		CCB_FUNC(mContentContainer,"mStoneName",CCLabelBMFont,setString(toolItem->name.c_str()));
		CCB_FUNC(mContentContainer,"mStoneLevel",CCLabelBMFont,setString(StringConverter::toString(stoneItem->level).c_str()));
		CCLabelBMFont* mAttackNum = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mAttackNum"));
		if(mAttackNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_2");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->attackBuff).c_str());
			mAttackNum->setString(buff);
		}
		CCLabelBMFont* mDefenseNum = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mDefenseNum"));
		if(mDefenseNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_3");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->defendBuff).c_str());
			mDefenseNum->setString(buff);
		}
		CCLabelBMFont* mHPNum = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mHPNum"));
		if(mHPNum){
			char buff[128];
			std::string buffName;
			buffName = Language::Get()->getString("@FightAttrName_1");
			sprintf(buff,"%s %s %s",buffName.c_str(),"+",StringConverter::toString(stoneItem->bloodBuff).c_str());
			mHPNum->setString(buff);
		}
		CCB_FUNC(mContentContainer,"mNum",CCLabelBMFont,setString(StringConverter::toString(userToolInfo->count).c_str()));
		CCB_FUNC(mContentContainer,"mUpGradeButtonTxt",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mStonePic",CCSprite,setTexture(stoneItem->bigIconPic.c_str()));
		CCB_FUNC(mContentContainer,"mStoneColor",CCSprite,setVisible(false));
		//		CCB_FUNC(mItemContainer,"mPackButton",CCMenuItemImage,initWithFile(stoneItem->iconPic.c_str()));
		int next = StoneTableManager::Get()->getStoneItemByID(toolItem->itemID)->nextLevelID ;

		if(StoneTableManager::Get()->getStoneItemByID(toolItem->itemID)->nextLevelID == 0){
			CCB_FUNC(mContentContainer,"mUpGradeButtenTxt",CCLabelBMFont,setVisible(false));
			CCB_FUNC(mContentContainer,"mUpGradeButten",CCMenuItemImage,setVisible(false));
		}else{
			CCB_FUNC(mContentContainer,"mUpGradeButtenTxt",CCLabelBMFont,setVisible(true));
			CCB_FUNC(mContentContainer,"mUpGradeButten",CCMenuItemImage,setVisible(true));
		}

	}
}

void StoneUpgradeContentItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

	if(StoneTableManager::Get()->getStoneItemByID(toolItem->itemID)->nextLevelID == 0){
		MSG_BOX(Language::Get()->getString("@StoneTopLevel"));
	}else{
		const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(mID);
		BlackBoard::Get()->currentStoneMId = userToolInfo->id;
		BlackBoard::Get()->currentStone = toolItem->itemID;
		MsgMainFramePushPage msg;
		msg.pageName="StoneUpgradePage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

void StoneEquipContent::refreshContent(){
	Equip equip( BlackBoard::Get()->ShowEquip);

	CCB_FUNC(mContentContainer,"mHeadPic",cocos2d::CCSprite,setTexture(equip.iconPic().c_str()));

	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(frame)
	{
		frame->setNormalImage(equip.getFrameNormalSpirte());
		frame->setSelectedImage(equip.getFrameSelectedSpirte());
	}

	CCB_FUNC(mContentContainer,"mEquipLevel",cocos2d::CCLabelBMFont,setString(StringConverter::toString(equip.level()).c_str()));
	CCB_FUNC(mContentContainer,"mBuffValue",cocos2d::CCLabelBMFont,setString(StringConverter::toString(equip.quantity()).c_str()));

	CCB_FUNC(mContentContainer,"mEquipBuffType",cocos2d::CCSprite,setTexture(equip.getEquipAtkTypePic().c_str()));
	CCB_FUNC(mContentContainer,"mEquipName",cocos2d::CCLabelBMFont,setString(equip.name().c_str()));

	SSICItem = dynamic_cast<cocos2d::CCNode* >(mContentContainer->getVariable("SSICItem"));
	bool stoneEnough = true;
	CCB_FUNC(mContentContainer,"mStoneMarkon1",CCLabelBMFont,setVisible(true));
	if(equip.stoneNum()>=1){

		if(equip.stoneStatus(1) == 3 && equip.stoneColor(1) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(1))->color){
			CCB_FUNC(mContentContainer,"mStoneMarkon2",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(mContentContainer,"mStoneMarkon2",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(mContentContainer,"mStoneMarkon2",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mStoneMarkon2",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(1))).c_str()));
	}else{
		CCB_FUNC(mContentContainer,"mStoneMarkon2",CCLabelBMFont,setVisible(false));
	}
	if(equip.stoneNum()>=2){
		if(equip.stoneStatus(2) == 3 && equip.stoneColor(2) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(2))->color){
			CCB_FUNC(mContentContainer,"mStoneMarkon3",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(mContentContainer,"mStoneMarkon3",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(mContentContainer,"mStoneMarkon3",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mStoneMarkon3",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(2))).c_str()));
	}else{
		CCB_FUNC(mContentContainer,"mStoneMarkon3",CCLabelBMFont,setVisible(false));
	}
	if(equip.stoneNum()>=3){
		if(equip.stoneStatus(3) == 3 && equip.stoneColor(3) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(3))->color){
			CCB_FUNC(mContentContainer,"mStoneMarkon4",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(mContentContainer,"mStoneMarkon4",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(mContentContainer,"mStoneMarkon4",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mStoneMarkon4",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(3))).c_str()));
	}else{
		CCB_FUNC(mContentContainer,"mStoneMarkon4",CCLabelBMFont,setVisible(false));
	}
	if(equip.stoneNum()>=4){
		if(equip.stoneStatus(4) == 3 && equip.stoneColor(4) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(4))->color){
			CCB_FUNC(mContentContainer,"mStoneMarkon5",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(mContentContainer,"mStoneMarkon5",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(mContentContainer,"mStoneMarkon5",CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mStoneMarkon5",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(4))).c_str()));
	}else{
		CCB_FUNC(mContentContainer,"mStoneMarkon5",CCLabelBMFont,setVisible(false));
	}

	CCLabelBMFont* mStoneMarkon6 =	 dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mStoneMarkon6"));

	if(mStoneMarkon6){
		char buff[128];
		std::string buffName;
		if(equip.type() == 1){																	 
			buffName = Language::Get()->getString("@FightAttrName_2");
		}else if(equip.type() == 2){
			buffName = Language::Get()->getString("@FightAttrName_3");
		}else if(equip.type() == 3){
			buffName = Language::Get()->getString("@FightAttrName_1");
		}

		std::list<std::string> _list;

		std::string buffValueStr = equip.stoneBuffValue();
		if(buffValueStr.begin() != buffValueStr.end()){
			std::replace(buffValueStr.begin(), buffValueStr.end(), '_', '%');
		}

		_list.push_back(buffValueStr.c_str());
		_list.push_back(buffName.c_str());
		mStoneMarkon6->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@StoneMarkon6"),&_list).c_str());
		if(stoneEnough){
			mStoneMarkon6->setOpacity(255);
		}else{
			mStoneMarkon6->setOpacity(128);
		}
	}
	rebuildAllItem();

}

void StoneEquipContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){
	if(itemName == "onReturn"){
		if(BlackBoard::Get()->jumpEquipPageName == ""){
			MsgMainFrameChangePage msg;
			msg.pageName = "EquipPage";
			MessageManager::Get()->sendMessage(&msg);
		}else{
			BlackBoard::Get()->UpgradeEquip = 0;

			MsgMainFrameChangePage msg;
			msg.pageName = BlackBoard::Get()->jumpEquipPageName;
			MessageManager::Get()->sendMessage(&msg);

			BlackBoard::Get()->NeedChangeEquip = true;
			BlackBoard::Get()->NeedChooseEquip = false;

			MsgMainFramePushPage msg1;
			msg1.pageName = "EquipInfoPage";
			MessageManager::Get()->sendMessage(&msg1);

		}
	}

}

void StoneEquipContent::buildItem(void){

	Equip equip( BlackBoard::Get()->ShowEquip);

	int itr = 1;
	int num = equip.stoneNum();
	while(itr <= equip.stoneNum()){
		StoneEquipContentItem * item = new StoneEquipContentItem(itr,mOwnner);
		item->init();
		addItem(item);
		++itr;
	}

}

void StoneEquipContent::addItem(ContentBase* item){
	
	mContentList.push_back(item);
	SSICItem->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(4 - mContentList.size()));
	item->getContainerNode()->setPosition(pos);

}

void StoneEquipContent::rebuildAllItem(void){
	clearAllItem();
	buildItem();
}

void StoneEquipContent::clearAllItem(void){
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	mContentList.clear();
	SSICItem->removeAllChildren();
}

void StoneEquipContent::refreshAllItem(void){

}

void StoneEquipContent::removeItem(ContentBase* item){

}

StoneInlaidPage::StoneInlaidPage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mLongBtnHeight = 0.0f;
	mContentList.clear();
	mTabMap.clear();
	mCurSellectedContent = NULL;
	isDesOrder = true;
}

StoneInlaidPage::~StoneInlaidPage(void)
{
}

void StoneInlaidPage::Enter( MainFrame* frame)
{
	isDesOrder = true;
	mCurSellectedContent = NULL;
	refreshPage();
	choosePage(1);
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
}

void StoneInlaidPage::Exit( MainFrame* frame)
{
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);

}

void StoneInlaidPage::Execute( MainFrame* )
{

}

void StoneInlaidPage::load( void )
{
	loadCcbiFile("StoneSystem.ccbi");
	
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mStoneUpContent"));
	membersRootNode = members->getContainer();

}

bool StoneInlaidPage::_sSort(unsigned int id1,unsigned int id2)
{
	return ServerDateManager::Get()->getUserToolInfoByID(id1)->itemid>ServerDateManager::Get()->getUserToolInfoByID(id2)->itemid;
}

void StoneInlaidPage::buildItem( void )
{
	if(this->tag == 1){
		StoneEquipContent * item = new StoneEquipContent(1,this);
		item->init();
		addItem(item);
	}else if(this->tag == 2){
		CCBContainer* longButton = CCBManager::Get()->createAndLoad("LongButton.ccbi");
		longButton->setListener(this,2);
		if(longButton)
		{
			CCLabelBMFont* goGetBtnLabel=dynamic_cast<CCLabelBMFont*>(longButton->getVariable(("mButtonLabel")));
			if(goGetBtnLabel)
			{
				goGetBtnLabel->setString(Language::Get()->getString("@GoMarketGetStone").c_str());
			}
			membersRootNode->addChild(longButton);
			CCPoint pos = ccp(0,0);
			longButton->setPosition(pos);
			mLongBtnHeight =  longButton->getContentSize().height;
			CCSize size = CCSize(longButton->getContentSize().width, mLongBtnHeight);
			members->setContentSize(size);
			members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}
		UserToolInfoListIterator itr = ServerDateManager::Get()->getToolIterator();
		std::list<unsigned int> myList;
		while(itr.hasMoreElements()){
			UserToolInfo* info = itr.getNext();
			const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(info->itemid);
			/*
			int isStone = ToolTableManager::Get()->getToolItemByID(info->itemid)->isStone;
			if(ToolTableManager::Get()->getToolItemByID(info->itemid)->isStone==1){ }
			*/
			
			if(toolItem!=NULL&&toolItem->isStone == 1){
				if(info->count>0){
					unsigned int id = info->id;
					myList.push_back(id);
				}
			}
		}
		myList.sort(&_sSort);
		std::list<unsigned int >::reverse_iterator it=myList.rbegin();
		for(;it!=myList.rend();++it)
		{
			StoneInlaidContentItem * item = new StoneInlaidContentItem(*it,this);
			item->init();
			addItem(item);
		}
		myList.clear();
	}else if(this->tag == 3){
		CCBContainer* longButton = CCBManager::Get()->createAndLoad("LongButton.ccbi");
		longButton->setListener(this,3);
		if(longButton)
		{
			CCLabelBMFont* goGetBtnLabel=dynamic_cast<CCLabelBMFont*>(longButton->getVariable(("mButtonLabel")));
			if(goGetBtnLabel)
			{
				goGetBtnLabel->setString(Language::Get()->getString("@GoMarketGetStone").c_str());
			}
			membersRootNode->addChild(longButton);
			CCPoint pos = ccp(0,0);
			longButton->setPosition(pos);
			mLongBtnHeight =  longButton->getContentSize().height;
			CCSize size = CCSize(longButton->getContentSize().width, mLongBtnHeight);
			members->setContentSize(size);
			members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}

		UserToolInfoListIterator itr = ServerDateManager::Get()->getToolIterator();
		std::list<unsigned int> myList;
		while(itr.hasMoreElements()){
			UserToolInfo* info = itr.getNext();
			const ToolItem* item = ToolTableManager::Get()->getToolItemByID(info->itemid);
			if(item!=NULL&&item->isStone == 1){
				if(info->count>0){
					unsigned int id = info->id;
					myList.push_back(id);
				}
			}
		}
		myList.sort(&_sSort);
		std::list<unsigned int >::reverse_iterator it=myList.rbegin();
		for(;it!=myList.rend();++it)
		{
			StoneUpgradeContentItem * item = new StoneUpgradeContentItem(*it,this);
			item->init();
			addItem(item);
		}
		myList.clear();
	}
}

void StoneInlaidPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	if(this->tag == 1){
		mPageInlay->addChild(item->getContainerNode());
		CCPoint pos = ccp(0,0);
		item->getContainerNode()->setPosition(pos);

		CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height);
		mInlayContent->setContentSize(size);
		mInlayContent->setContentOffset(ccp(0,0)); 
		mInlayContent->setTouchEnabled(false);
	}else if(this->tag == 2){
		mStoneUpContentNode->addChild(item->getContainerNode());

		CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * (mContentList.size() - 1)+ mLongBtnHeight);
		item->getContainerNode()->setPosition(pos);

		CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height * mContentList.size()+ mLongBtnHeight );
		mStoneUpContent->setContentSize(size);
		mStoneUpContent->setContentOffset(ccp(0,mStoneUpContent->getViewSize().height - mStoneUpContent->getContentSize().height*mStoneUpContent->getScaleY() ));
		mStoneUpContent->setTouchEnabled(true);

	}else if(this->tag == 3){
		mStoneUpContentNode->addChild(item->getContainerNode());
		CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * (mContentList.size() - 1)+ mLongBtnHeight);
		item->getContainerNode()->setPosition(pos);

		CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height * mContentList.size()+ mLongBtnHeight );
		mStoneUpContent->setContentSize(size);
		mStoneUpContent->setContentOffset(ccp(0,mStoneUpContent->getViewSize().height - mStoneUpContent->getContentSize().height*mStoneUpContent->getScaleY() ));
		mStoneUpContent->setTouchEnabled(true);

	}
}

void StoneInlaidPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void StoneInlaidPage::clearAllItem( void )
{
	mCurSellectedContent = NULL;
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	mContentList.clear();
	mPageInlay->removeAllChildren();
	mStoneUpContentNode->removeAllChildren();
}

void StoneInlaidPage::refreshAllItem( void )
{
}

void StoneInlaidPage::removeItem( ContentBase* item )
{
}

void StoneInlaidPage::buildEquipItem(){
	/*
	StoneEquipContent * content = new StoneEquipContent(1);
	content->init();
	mInlayContent->addChild(content->getContainerNode());
	CCPoint pos = ccp(100,100);
	content->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(content->getContainerNode()->getContentSize().width,content->getContainerNode()->getContentSize().height);
	members->setContentSize(size);
	members->setContentOffset(ccp(0,0));

	*/

}

void StoneInlaidPage::choosePage(int tag){
	this->tag = tag;
	if(this->tag == 1){
		rebuildAllItem();
		mTabInlay->selected();
		mTabUpgrade->unselected();	
		CCB_FUNC(this,"mPageInlay",CCNode,setVisible(true));
		CCB_FUNC(this,"mPageSU",CCNode,setVisible(false));
		CCB_FUNC(this,"mSure",CCLabelBMFont,setVisible(false));
		CCB_FUNC(this,"mConfirmButton",CCMenuItemImage,setVisible(false));
	}else if(this->tag == 2){
		rebuildAllItem();
		mTabInlay->selected();
		mTabUpgrade->unselected();	
		CCB_FUNC(this,"mPageInlay",CCNode,setVisible(false));
		CCB_FUNC(this,"mPageSU",CCNode,setVisible(true));
		CCB_FUNC(this,"mSure",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mSure",CCLabelBMFont,setString(Language::Get()->getString("@Confirm").c_str()));
		CCB_FUNC(this,"mConfirmButton",CCMenuItemImage,setVisible(true));
	}else if(this->tag == 3){
		rebuildAllItem();
		mTabInlay->unselected();
		mTabUpgrade->selected();
		CCB_FUNC(this,"mPageInlay",CCNode,setVisible(false));
		CCB_FUNC(this,"mPageSU",CCNode,setVisible(true));
		CCB_FUNC(this,"mSure",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mSure",CCLabelBMFont,setString(Language::Get()->getString("@Back").c_str()));
		CCB_FUNC(this,"mConfirmButton",CCMenuItemImage,setVisible(true));
	}
}

void StoneInlaidPage::setSellectedContent( StoneInlaidContentItem* member ){
	if(mCurSellectedContent)
		mCurSellectedContent->setSellect(false);

	if(mCurSellectedContent != member)
	{
		mCurSellectedContent = member;
		mCurSellectedContent->setSellect(true);
	}
	else
	{
		mCurSellectedContent = NULL;
	}
}

void StoneInlaidPage::reOrderAllItem()
{
	mStoneUpContentNode->removeAllChildren();
	if(isDesOrder)
	{
		ContentList::iterator itr = mContentList.begin(); 
		for(int i = 0; itr != mContentList.end(); ++itr,++i)
		{
			ContentBase * item = *itr;
			mStoneUpContentNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * i);
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height * (i+1));
			members->setContentSize(size);
			members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}
	}
	else
	{
		ContentList::reverse_iterator itr = mContentList.rbegin(); 
		for(int i = 0; itr != mContentList.rend(); ++itr,++i)
		{
			ContentBase * item = *itr;
			mStoneUpContentNode->addChild(item->getContainerNode());
			CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * i);
			item->getContainerNode()->setPosition(pos);
			CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height * (i+1));
			members->setContentSize(size);
			members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		}
	}
}

void StoneInlaidPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender,int tag)
{
	if(itemName=="onFunciton")
	{
		BlackBoard::Get()->currStonePage = tag;
		MsgMainFramePushPage msg;
		msg.pageName = "BuyStonePopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void StoneInlaidPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onInlay"){
			
//		BlackBoard::Get()->stonePosition = -1;
		choosePage(1);

	}else if(itemName == "onUpgrade"){

		BlackBoard::Get()->stonePosition = -1;
		choosePage(3);

	}else if(itemName == "onArray"){
		isDesOrder = !isDesOrder;
		reOrderAllItem();
		refreshPage();
	}else if(itemName == "onSure"){
		if(this->tag == 2){

			if(BlackBoard::Get()->stonePosition == -1){
				if(BlackBoard::Get()->jumpEquipPageName == ""){
					MsgMainFrameChangePage msg;
					msg.pageName = "EquipPage";
					MessageManager::Get()->sendMessage(&msg);
				}else{
					BlackBoard::Get()->UpgradeEquip = 0;

					MsgMainFrameChangePage msg;
					msg.pageName = BlackBoard::Get()->jumpEquipPageName;
					MessageManager::Get()->sendMessage(&msg);

					BlackBoard::Get()->NeedChangeEquip = true;
					BlackBoard::Get()->NeedChooseEquip = false;

					MsgMainFramePushPage msg1;
					msg1.pageName = "EquipInfoPage";
					MessageManager::Get()->sendMessage(&msg1);

				}
			}else{
				if(mCurSellectedContent == NULL){
				}else{
					OPInlaidStoneInfo inlaidStoneInfo;
					inlaidStoneInfo.set_version(1);
					inlaidStoneInfo.set_stoneid(itemId);
					inlaidStoneInfo.set_equipid(BlackBoard::Get()->ShowEquip);
					inlaidStoneInfo.set_position(BlackBoard::Get()->stonePosition);
					PacketManager::Get()->sendPakcet(OPCODE_INLAID_STONEINFO_C,&inlaidStoneInfo);
				}
				choosePage(1);
			}
		}else if(this->tag == 3){
			if(BlackBoard::Get()->jumpEquipPageName == ""){
				MsgMainFrameChangePage msg;
				msg.pageName = "EquipPage";
				MessageManager::Get()->sendMessage(&msg);
			}else{
				BlackBoard::Get()->UpgradeEquip = 0;

				MsgMainFrameChangePage msg;
				msg.pageName = BlackBoard::Get()->jumpEquipPageName;
				MessageManager::Get()->sendMessage(&msg);

				BlackBoard::Get()->NeedChangeEquip = true;
				BlackBoard::Get()->NeedChooseEquip = false;

				MsgMainFramePushPage msg1;
				msg1.pageName = "EquipInfoPage";
				MessageManager::Get()->sendMessage(&msg1);

			}
		}
	}
}
												

void StoneInlaidPage::refreshPage( void )
{
	mInlayContent = dynamic_cast<cocos2d::extension::CCScrollView* >(getVariable("mInlayContent"));
	mPageInlay = mInlayContent->getContainer();

	mTabInlay = dynamic_cast<cocos2d::CCMenuItemImage* >(getVariable("mTabInlay"));
	mTabUpgrade = dynamic_cast<cocos2d::CCMenuItemImage* >(getVariable("mTabUpgrade"));

	mPageSU = dynamic_cast<cocos2d::CCNode* >(getVariable("mPageSU"));
	mStoneUpContent = dynamic_cast<cocos2d::extension::CCScrollView* >(getVariable("mStoneUpContent"));
	mStoneUpContentNode = mStoneUpContent->getContainer();

	CCLabelBMFont* funcLable=dynamic_cast<CCLabelBMFont* >(getVariable("mArray"));
	if(funcLable)
	{
		if(isDesOrder)
			funcLable->setString(Language::Get()->getString("@OrderByAsce").c_str());
		else
			funcLable->setString(Language::Get()->getString("@OrderByDesc").c_str());
	}
}

void StoneInlaidPage::sellectTab( unsigned int tabIndex )
{
}

void StoneInlaidPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "StoneInlaidPage")
		{
			refreshPage();
			choosePage(chooseNum);
		}
	}
}