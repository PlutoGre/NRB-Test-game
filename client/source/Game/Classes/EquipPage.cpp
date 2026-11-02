
#include "stdafx.h"

#include "EquipPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"
#include "math.h"

REGISTER_PAGE("EquipPage",EquipPage);
USING_NS_CC;

void EquipContentStone::refreshContent( void ){
	Equip equip(equipId);

	if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status){
		//未开启	,颜色用灰色，图片用锁头
		CCSprite* mStonePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mStonePic"));
		if(mStonePic){
			mStonePic->setVisible(true);
			mStonePic->setTexture(StoneTableManager::Get()->getStoneItemBySign("locked")->iconPic.c_str());
		}
		CCSprite* mStoneColor=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mStoneColor"));
		if(mStoneColor){
			mStoneColor->setTexture(StoneTableManager::Get()->getStoneColorItemBySign("grey")->iconPic.c_str());
		}

	}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("opened")->status){
		//已开启,颜色用正确的，图片不显示
		CCSprite* mStonePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mStonePic"));
		if(mStonePic){
			mStonePic->setVisible(false);
		}
		CCSprite* mStoneColor=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mStoneColor"));
		if(mStoneColor){
			int color = equip.stoneColor(mID);
			const StoneItem * stoneItem = StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID));
			mStoneColor->setTexture(StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID))->iconPic.c_str());
		}

	}else if(equip.stoneStatus(mID) == StoneTableManager::Get()->getStoneStatusItemBySign("inlaid")->status){
		//已镶嵌,颜色图片都有
		CCSprite* mStonePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mStonePic"));
		if(mStonePic)
		{
			mStonePic->setVisible(true);
			mStonePic->removeAllChildren();
			mStonePic->setTexture(StoneTableManager::Get()->getStoneItemByID(equip.stoneId(mID))->iconPic.c_str());
		}
		CCSprite* mStoneColor=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mStoneColor"));
		if(mStoneColor){
			mStoneColor->removeAllChildren();
			mStoneColor->setTexture(StoneTableManager::Get()->getStoneColorItemByStatus(equip.stoneColor(mID))->iconPic.c_str());
		}
	}

}

void EquipContentStone::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}
void EquipContent::refreshContent( void )
{
	Equip equip(mID);

	if(PopupManager::Get()->showStoneGuide()){
		CCB_FUNC(mContentContainer,"mGuideEffect",cocos2d::CCNode,setVisible(true));
	}else{
		CCB_FUNC(mContentContainer,"mGuideEffect",cocos2d::CCNode,setVisible(false));
	}

	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(equip.iconPic().c_str());
	}
	CCMenuItemImage* frame=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(frame)
	{
		frame->setNormalImage(equip.getFrameNormalSpirte());
		frame->setSelectedImage(equip.getFrameSelectedSpirte());
	}
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipName"));
	if(name)
	{
		name->setString(Language::Get()->getString(equip.name()).c_str());
#if defined(Macro_TestUseCCReViScrollViewFacade) && defined(_DEBUG)
		std::string strTmp = StringConverter::toString(this->m_iIdx) + Language::Get()->getString(equip.name());
		name->setString(strTmp.c_str());
#endif
	}
	CCSprite* quality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipQuality"));
	if(quality)
	{
		quality->initWithFile(equip.getQualityImageFile().c_str());
	}
	CCSprite* equipBuffValue=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mEquipBuffType"));
	if(equipBuffValue)
	{
		equipBuffValue->initWithFile(equip.getEquipAtkTypePic().c_str());
	}
	CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipLevel"));
	if(levelName)
	{
		levelName->setString(StringConverter::toString(equip.level()).c_str());
	}
	CCLabelBMFont* valueKey=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mEquipValue"));
	if(valueKey)
	{
		valueKey->setString(StringConverter::toString(equip.getEquipValue()).c_str());
	}

	CCSprite* mMPInBattlePic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMPInBattlePic"));

	CCLabelBMFont* equipCarriedBy=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mCarriedBy"));
	if(equipCarriedBy)
	{
		std::string carriedByStr = equip.getEquipCarriedBy();
		if(carriedByStr!="")
		{
			char buff[128];
			std::string s= Language::Get()->getString("@CarriedBy");
			sprintf(buff,s.c_str(),carriedByStr.c_str());
			equipCarriedBy->setString(buff);
			if(mMPInBattlePic)
			{
				mMPInBattlePic->setVisible(true);
			}
		}
		else
		{
			equipCarriedBy->setString("");
			if(mMPInBattlePic)
			{
				mMPInBattlePic->setVisible(false);
			}
		}

	}
	CCLabelBMFont* buffValue=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mBuffValue"));
	if(buffValue)
	{
		buffValue->setString(StringConverter::toString(equip.quantity()).c_str());
	}
	CCMenuItemImage* oprName_2 = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mEnhance"));
	if(oprName_2)
	{
		if(equip.allowLevelUpgrade())
			oprName_2->setEnabled(true);
		else
			oprName_2->setEnabled(false);
	}
	bool stoneEnough = true;
	mStone = dynamic_cast<CCNode* >(mContentContainer->getVariable("mStone"));
	if(mStone){
		
		if(ServerDateManager::Get()->getUserBasicInfo().level >=  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneInlaidLevel"))){
			rebuildAllItem();
			int itr = 1;
			int iNum = equip.stoneNum();
			while(itr <= iNum){
				if(equip.stoneStatus(itr) != StoneTableManager::Get()->getStoneStatusItemBySign("inlaid")->status){
					stoneEnough = false;
					break;
				}
				if(equip.stoneColor(itr) != StoneTableManager::Get()->getStoneItemByID(equip.stoneId(itr))->color){
					stoneEnough = false;
					break;
				}
				++itr;
			}
		}
		
	}
	CCLabelBMFont* mIncreasedBuff = dynamic_cast<CCLabelBMFont*>(mContentContainer->getVariable("mIncreasedBuff"));
	if(mIncreasedBuff){
		char buff[128];
		std::string buffName;
		if(equip.type() == 1){
			buffName = Language::Get()->getString("@FightAttrName_2");
		}else if(equip.type() == 2){
			buffName = Language::Get()->getString("@FightAttrName_3");
		}else if(equip.type() == 3){
			buffName = Language::Get()->getString("@FightAttrName_1");
		}
		std::string buffValueStr = equip.stoneBuffValue();
		if(buffValueStr.begin() != buffValueStr.end()){
			std::replace(buffValueStr.begin(), buffValueStr.end(), '_', '%');
		}
		sprintf(buff,"%s %s %s",buffName.c_str(),"+",buffValueStr.c_str());
		mIncreasedBuff->setString(buff);
		if(stoneEnough){
			mIncreasedBuff->setOpacity(255);
		}else{
			mIncreasedBuff->setOpacity(128);
		}
	}
	
}

void EquipContent::rebuildAllItem(){
	clearAllItem();
	buildItem();

}
void EquipContent::clearAllItem(){
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end()){
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	if (mStone)
		mStone->removeAllChildren();

}
void EquipContent::addItem(ContentBase* item){

	mContentList.push_back(item);
	mStone->addChild(item->getContainerNode());
	CCPoint pos = ccp(item->getContainerNode()->getContentSize().width * (mContentList.size() - 1),0);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width *mContentList.size(), item->getContainerNode()->getContentSize().height);
//	members->setContentSize(size);
//	members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));

}
void EquipContent::buildItem(){

	Equip equip(mID);

	int itr = 1;
	int iNum = equip.stoneNum();
	while(itr <= iNum){
		EquipContentStone * item = new EquipContentStone(itr,mID);
		item->init();
		addItem(item);
		++itr;
	}

}

void EquipContent::refreshAllItem(){
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end()){
		(*itr)->refreshContent();
		++itr;
	}

}

void EquipContent::removeItem(ContentBase* item){
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end()){
		if((*itr) == item){
			mStone->removeChild(item->getContainerNode());
			mContentList.erase(itr);
		}
		++itr;
	}

}

void EquipContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onEnhance")
	{
		BlackBoard::Get()->UpgradeEquip = mID;
		BlackBoard::Get()->jumpEquipPageName = "EquipPage";
		MsgMainFrameChangePage msg;
		msg.pageName = "EquipEnhancePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowEquipInfo")
	{
		BlackBoard::Get()->NeedChangeEquip = false;
		BlackBoard::Get()->NeedChooseEquip = false;
		BlackBoard::Get()->ShowEquip = mID;
		BlackBoard::Get()->jumpEquipPageName = "EquipPage";
		MsgMainFramePushPage msg;
		msg.pageName = "EquipInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onRemold"){
		BlackBoard::Get()->jumpEquipPageName = "";
		PopupManager::Get()->setStoneGuide();
		BlackBoard::Get()->ShowEquip = mID;
		MsgMainFrameChangePage msg;
		msg.pageName = "StoneInlaidPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

#ifdef Macro_TestUseCCReViScrollViewFacade

void EquipContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void EquipContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	EquipContentItemData* pItemData = static_cast<EquipContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iEquipID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}
#endif

EquipContent::~EquipContent()
{
	clearAllItem();
	mStone = NULL;
}


EquipPage::EquipPage(void)
{
	membersRootNode = NULL;
	members = NULL;
	mLongBtnHeight = 0.0f;
	mContentList.clear();
	mTabMap.clear();
}

EquipPage::~EquipPage(void)
{
}

void EquipPage::Enter( MainFrame* frame)
{
	mCurrTabIndex=0;
	if(HAS_V("SellEquip"))
	{
		unsigned int index = GET_V_UINT("SellEquip");
		if(index == 0)
			index = 1;

		sellectTab(index);
	}
	else
		sellectTab(1);

	refreshPage();
}

void EquipPage::Exit( MainFrame* frame)
{
	clearAllItem();
	//
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
#endif
}

void EquipPage::Execute( MainFrame* )
{

}

void EquipPage::load( void )
{
	loadCcbiFile("Equipment.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mEquipContent"));
	membersRootNode = members->getContainer();

	mTabMap.clear();
	mTabMap[1] = dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTabAll"));
	mTabMap[2] = dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTabWeapon"));
	mTabMap[3] = dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTabArmor"));
	mTabMap[4] = dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTabJewery"));

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif

}

void EquipPage::buildItem( void )
{
	CCBContainer* longButton = CCBManager::Get()->createAndLoad("LongButton.ccbi");
	longButton->setListener(this,999);
	if(longButton)
	{
		CCLabelBMFont* goGetBtnLabel=dynamic_cast<CCLabelBMFont*>(longButton->getVariable(("mButtonLabel")));
		if(goGetBtnLabel)
		{
			goGetBtnLabel->setString(Language::Get()->getString("@GoCreerGetEquip").c_str());
		}
		membersRootNode->addChild(longButton);
		CCPoint pos = ccp(0,0);
		longButton->setPosition(pos);
		mLongBtnHeight =  longButton->getContentSize().height;
		CCSize size = CCSize(longButton->getContentSize().width, mLongBtnHeight);
		members->setContentSize(size);
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
	}

	EquipDateCollect::EquipIDCollect collect;
	if(mCurrTabIndex == 1)
	{
		collect = EquipDateCollect::Get()->getEquipCollection();
	}
	else if(mCurrTabIndex == 2)
	{
		collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Type1);
	}
	else if(mCurrTabIndex == 3)
	{
		collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Type2);
	}
	else if(mCurrTabIndex == 4)
	{
		collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Type3);
	}

#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		EquipDateCollect::EquipIDCollect::reverse_iterator itr = collect.rbegin();
		while(itr != collect.rend())
		{
			//
			EquipContentItemData* pItemData = new EquipContentItemData();
			pItemData->m_iEquipID = *itr;
			pItemData->m_iIdx = iCount;
			//
			pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount+mLongBtnHeight);
			//
			EquipContent* pItem = NULL;
			if (iCount < iMaxNode)
			{
				//
				pItem = new EquipContent(*itr);
				pItem->initItemView();
				//pItem->refreshItemView(pItemData);//初始在m_pScrollViewFacade->setDynamicItemsStartPosition设置
				//
				if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
					fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
				if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
					fOneItemWidth = pItem->getContainerNode()->getContentSize().width;
			}
			m_pScrollViewFacade->addItem(pItemData, pItem);
			
			//
			++itr;
			++iCount;
		}
		//
		CCSize size = CCSize(fOneItemWidth, fOneItemHeight*iCount+mLongBtnHeight);
		members->setContentSize(size);
		/*
			注意ScrollView里除了受控的DynamicItems，还有其他Items
			ScrollView锚点(0.f,0.f)位于左下角
			height由下向上增长
			width由左向右增长
			下面两句顺序不能颠倒，意思是：最后添加的显示到可视区域左上角
		*/
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		m_pScrollViewFacade->setDynamicItemsStartPosition(iCount-1);
		{
			//m_pScrollViewFacade->setDynamicItemsStartPosition(0);
		}
		//
	}
#else
	EquipDateCollect::EquipIDCollect::reverse_iterator itr = collect.rbegin();
	while(itr != collect.rend())
	{
		EquipContent * item = new EquipContent(*itr);
		item->init();
		addItem(item);
		++itr;
	}
#endif
}

void EquipPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1)+mLongBtnHeight);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size()+mLongBtnHeight);
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void EquipPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void EquipPage::clearAllItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade->clearAllItems();
#endif
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	membersRootNode->removeAllChildren();
}

void EquipPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void EquipPage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
		}
		++itr;
	}
}

void EquipPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	std::string name;
	if(itemName == "onAll")
	{
		sellectTab(1);
	}
	else if(itemName == "onWeapon")
	{
		sellectTab(2);
	}
	else if(itemName == "onArmor")
	{
		sellectTab(3);
	}
	else if(itemName == "onJewery")
	{
		sellectTab(4);
	}
	else if(itemName == "onSell")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "EquipSellPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
void EquipPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onFunciton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "CareerPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void EquipPage::refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCLabelBMFont* level = dynamic_cast<CCLabelBMFont* >(getVariable("mPlayerLv"));
	if(level)
	{
		level->setString(StringConverter::toString(userBasicInfo.level).c_str());
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
}

void EquipPage::sellectTab( unsigned int tabIndex )
{
	TabMap::iterator itr = mTabMap.begin();
	while(itr != mTabMap.end())
	{
		if(itr->first == tabIndex)
			itr->second->selected();
		else
			itr->second->unselected();

		++itr;
	}

	if(tabIndex != mCurrTabIndex)
	{
		mCurrTabIndex = tabIndex;
		ADD_OR_SET_V("SellEquip", mCurrTabIndex);
		rebuildAllItem();
	}
}
