
#include "stdafx.h"

#include "ViewTeamEquipInfoPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "Equip.h"
#include "EquipCardPage.h"
#include "MessageBoxPage.h"
#include "GameMaths.h"

REGISTER_PAGE("ViewTeamEquipInfoPage",ViewTeamEquipInfoPage);
USING_NS_CC;

void ViewTeamEquipStoneInfo::refreshContent(){

	Equip equip(equipId,false,true);

	if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status){
		//未开启	,颜色用灰色，图片用锁头
		CCB_FUNC(mContentContainer,"mStoneBuffPic",cocos2d::CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"mInfoStoneNum",cocos2d::CCLabelBMFont,setVisible(false));
		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setVisible(true));
		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneItemBySign("locked")->iconPic.c_str()));
		CCB_FUNC(mContentContainer,"mStoneColor",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneColorItemBySign("grey")->iconPic.c_str()));
		CCB_FUNC(mContentContainer,"mText",cocos2d::CCLabelBMFont,setString(Language::Get()->getString("@noPunch").c_str()));
		CCB_FUNC(mContentContainer,"mText",cocos2d::CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mInfoStoneLV",cocos2d::CCLabelBMFont,setVisible(false));
		CCB_FUNC(mContentContainer,"mBuff",cocos2d::CCNode,setVisible(false));

	}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("opened")->status){
		//已开启,颜色用正确的，图片不显示
		CCB_FUNC(mContentContainer,"mStoneBuffPic",cocos2d::CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"mInfoStoneNum",cocos2d::CCLabelBMFont,setVisible(false));
		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"mStoneColor",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID))->iconPic.c_str()));
		CCB_FUNC(mContentContainer,"mText",cocos2d::CCLabelBMFont,setString(Language::Get()->getString("@noInlaid").c_str()));
		CCB_FUNC(mContentContainer,"mText",cocos2d::CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mInfoStoneLV",cocos2d::CCLabelBMFont,setVisible(false));
		CCB_FUNC(mContentContainer,"mBuff",cocos2d::CCNode,setVisible(false));

	}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("inlaid")->status){
		//已镶嵌,颜色图片都有
		CCB_FUNC(mContentContainer,"mStoneBuffPic",cocos2d::CCSprite,setVisible(true));
		CCB_FUNC(mContentContainer,"mInfoStoneNum",cocos2d::CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mStoneBuffPic",cocos2d::CCSprite,setTexture(VaribleManager::Get()->getSetting("EquipAttack_ImageFile" + StringConverter::toString(equip.type())).c_str()));

		if(equip.type() == 1){
			CCB_FUNC(mContentContainer,"mInfoStoneNum",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->attackBuff).c_str()));
		}else if(equip.type() == 2){
			CCB_FUNC(mContentContainer,"mInfoStoneNum",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->defendBuff).c_str()));
		}else if(equip.type() == 3){
			CCB_FUNC(mContentContainer,"mInfoStoneNum",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->bloodBuff).c_str()));
		}

		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setVisible(true));
		CCB_FUNC(mContentContainer,"mStonePic",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->iconPic.c_str()));
		CCB_FUNC(mContentContainer,"mStoneColor",cocos2d::CCSprite,setTexture(StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID))->iconPic.c_str()));
		CCB_FUNC(mContentContainer,"mText",cocos2d::CCLabelBMFont,setString(""));
		CCB_FUNC(mContentContainer,"mText",cocos2d::CCLabelBMFont,setVisible(false));
		CCB_FUNC(mContentContainer,"mInfoStoneLV",cocos2d::CCLabelBMFont,setString(StringConverter::toString(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->level).c_str()));
		CCB_FUNC(mContentContainer,"mInfoStoneLV",cocos2d::CCLabelBMFont,setVisible(true));
		CCB_FUNC(mContentContainer,"mBuff",cocos2d::CCNode,setVisible(true));
	}
}

void ViewTeamEquipStoneInfo::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}

ViewTeamEquipInfoPage::ViewTeamEquipInfoPage(void)
{
	mCard = NULL;
}

ViewTeamEquipInfoPage::~ViewTeamEquipInfoPage(void)
{
}

void ViewTeamEquipInfoPage::load( void ){

	if(ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerLevel >=  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneInlaidLevel"))){
		loadCcbiFile("EquipmentInfoStone.ccbi");
	}else{
		loadCcbiFile("EquipmentInfo.ccbi");
	}
	mEquipCard = dynamic_cast<CCNode*>(getVariable("mEquipCard"));
	refreshPage();
	buildCard();
}
void ViewTeamEquipInfoPage::rebuildAllItem(){
	clearAllItem();
	buildItem();
}

void ViewTeamEquipInfoPage::clearAllItem(){
	
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end()){
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	if(mInlayStone != NULL){
		mInlayStone->removeAllChildren();
	}

}

void ViewTeamEquipInfoPage::buildItem(){
	Equip equip(mId,false,true);

	int itr = 1;
	int num = equip.stoneNum();
	while(itr <= equip.stoneNum()){
		ViewTeamEquipStoneInfo * item = new ViewTeamEquipStoneInfo(itr,mId);
		item->init();
		addItem(item);
		++itr;
	}
}

void ViewTeamEquipInfoPage::addItem(ContentBase* item){
	mContentList.push_back(item);
	mInlayStone->addChild(item->getContainerNode());
	int width = (item->getContainerNode()->getContentSize().width*((3 - mContentList.size())%2));
	int height = (item->getContainerNode()->getContentSize().height*((4 - mContentList.size())));
	CCPoint pos = ccp(0,height);
	item->getContainerNode()->setPosition(pos);

}

void ViewTeamEquipInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

	if(itemName == "onFunction1")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ViewTeamEquipInfoPage";
		MessageManager::Get()->sendMessage(&msg);

	}else if(itemName == "onShowEnhanceFail"){
		MSG_BOX(Language::Get()->getString("@MAINPAGE_COMINGSOON"));
	}
	else if(itemName == "onFunction2")
	{
		MSG_BOX(Language::Get()->getString("@MAINPAGE_COMINGSOON"));
	}
	else if(itemName == "onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ViewTeamEquipInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}

void ViewTeamEquipInfoPage::refreshPage( void )
{
	Equip equip(mId,false,true);
	
	CCB_FUNC(this,"mLevel",CCLabelBMFont,setString(StringConverter::toString(equip.level()).c_str()));
	CCB_FUNC(this,"mBuffValue",CCLabelBMFont,setString(StringConverter::toString(equip.quantity()).c_str()));
	CCB_FUNC(this,"mBuffPic",cocos2d::CCSprite,setTexture(VaribleManager::Get()->getSetting("EquipAttack_ImageFile" + StringConverter::toString(equip.type())).c_str()));



	CCLabelBMFont* equipValue = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipValue"));
	if(equipValue)
	{
		equipValue->setString(StringConverter::toString(equip.getEquipValue()).c_str());
	}
	CCLabelBMFont* equipIntroduce = dynamic_cast<CCLabelBMFont* >(getVariable("mEquipIntro"));
	if(equipIntroduce)
	{
		if( ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerLevel >=  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneInlaidLevel"))){
			equipIntroduce->setString(Language::Get()->getString(equip.describe("Equip_Describe_Stone")).c_str());
		}else{
			equipIntroduce->setString(Language::Get()->getString(equip.describe()).c_str());
		}

	}
	CCB_FUNC(this,"mButtenName3",CCLabelBMFont,setVisible(false));
	CCB_FUNC(this,"mInlay",CCMenuItemImage,setVisible(false));

	CCB_FUNC(this,"mButtonName1",CCLabelBMFont,setString(Language::Get()->getString("@Close").c_str()));
	CCB_FUNC(this,"mButtonName2",CCLabelBMFont,setString(Language::Get()->getString("@Share").c_str()));

	mInlayStone = dynamic_cast<CCNode* >(getVariable("mInlayStone"));
	if(mInlayStone)
	{
//		mInlayStone->setString(Language::Get()->getString(equip.describe()).c_str());
	}
	bool stoneEnough = true;
	CCB_FUNC(this,"mStoneMarkon1",CCLabelBMFont,setVisible(true));
	if(equip.stoneNum()>=1){

		if(equip.stoneStatus(1) == 3 && equip.stoneColor(1) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(1))->color){
			CCB_FUNC(this,"mStoneMarkon2",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(this,"mStoneMarkon2",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(this,"mStoneMarkon2",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mStoneMarkon2",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(1))).c_str()));
	}else{
		CCB_FUNC(this,"mStoneMarkon2",CCLabelBMFont,setVisible(false));
		CCB_FUNC(this,"mStoneMarkon1",CCLabelBMFont,setVisible(false));
		CCB_FUNC(this,"mStoneMarkon6",CCLabelBMFont,setVisible(false));
	}
	if(equip.stoneNum()>=2){
		if(equip.stoneStatus(2) == 3 && equip.stoneColor(2) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(2))->color){
			CCB_FUNC(this,"mStoneMarkon3",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(this,"mStoneMarkon3",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(this,"mStoneMarkon3",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mStoneMarkon3",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(2))).c_str()));
	}else{
		CCB_FUNC(this,"mStoneMarkon3",CCLabelBMFont,setVisible(false));
	}
	if(equip.stoneNum()>=3){
		if(equip.stoneStatus(3) == 3 && equip.stoneColor(3) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(3))->color){
			CCB_FUNC(this,"mStoneMarkon4",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(this,"mStoneMarkon4",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(this,"mStoneMarkon4",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mStoneMarkon4",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(3))).c_str()));
	}else{
		CCB_FUNC(this,"mStoneMarkon4",CCLabelBMFont,setVisible(false));
	}
	if(equip.stoneNum()>=4){
		if(equip.stoneStatus(4) == 3 && equip.stoneColor(4) == StoneTableManager::Get()->getStoneItemByID(equip.stoneId(4))->color){
			CCB_FUNC(this,"mStoneMarkon5",CCLabelBMFont,setOpacity(255));
		}else{
			CCB_FUNC(this,"mStoneMarkon5",CCLabelBMFont,setOpacity(128));
			stoneEnough = false;
		}
		CCB_FUNC(this,"mStoneMarkon5",CCLabelBMFont,setVisible(true));
		CCB_FUNC(this,"mStoneMarkon5",CCLabelBMFont,setString(Language::Get()->getString("@Stone"+StringConverter::toString(equip.stoneColor(4))).c_str()));
	}else{
		CCB_FUNC(this,"mStoneMarkon5",CCLabelBMFont,setVisible(false));
	}

	CCLabelBMFont* mStoneMarkon6 =	 dynamic_cast<CCLabelBMFont* >(getVariable("mStoneMarkon6"));

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

	if(ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerLevel >=  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneInlaidLevel"))){
		rebuildAllItem();
	}
}

void ViewTeamEquipInfoPage::showPage(unsigned int id)
{
	MsgMainFramePushPage msg;
	msg.pageName="ViewTeamEquipInfoPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("ViewTeamEquipInfoPage");
	ViewTeamEquipInfoPage* page = dynamic_cast<ViewTeamEquipInfoPage*>(obj);
	if(page)
	{
		page->mId=id;
	}
}


void ViewTeamEquipInfoPage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(mId);
		mCard->setIsViewItem(true);
		mCard->refreshContent();
	}
	else
	{
		mCard = new EquipCardPage(mId,false,true);
		mCard->init();
		mEquipCard->removeAllChildren();
		mEquipCard->addChild(mCard->getContainerNode());
	}

	
}

void ViewTeamEquipInfoPage::unload( void )
{
	mEquipCard = NULL;

	delete mCard;
	mCard = NULL;

	CustomPage::unload();
}
