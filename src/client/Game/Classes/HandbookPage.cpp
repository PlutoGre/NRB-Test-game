
#include "stdafx.h"


#include "HandbookPage.h"
#include "Disciple.h"
#include "Equip.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "AsyncSprite.h"
#include "platform/CCPlatformMacros.h"
#include "CCBContainer.h"
#include "EquipHandInfoPage.h"
#include "DiscipleHandInfoPage.h"
#include "SkillHandInfoPage.h"

REGISTER_PAGE("HandbookPage",HandbookPage);
USING_NS_CC;

void HandbookPersonContent::refreshContent(void){

	const DiscipleItem * disItem = DiscipleTableManager::Get()->getDiscipleItemByID(mID);
	Disciple disciple(mID,true);
	CCMenuItemImage* mFace = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(mFace)
	{
		CCMenu*  menu=dynamic_cast<CCMenu* >(mFace->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
		mFace->setNormalImage(disciple.getFrameNormalSpirte());
		mFace->setSelectedImage(disciple.getFrameSelectedSpirte());
	}
	mFace->setZOrder(2);

	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("name"));
	if(name)
	{
		name->setString(disciple.name().c_str());
	}
	
	CCSprite* prop = dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));

	const HandbookPersonInfo * personInfo = ServerDateManager::Get()->getHandbookPersonInfoByID(mID);

	int showFlag = StringConverter::parseInt(VaribleManager::Get()->getSetting("HandbookShowBg"));
	if(showFlag==1||true){

		if(personInfo == NULL)
		{
			mIsShowDetail=false;
			CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(true));
			CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(false));
		}
		else
		{
			mIsShowDetail=true;
			CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(false));
			CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(true));
			prop->removeAllChildren();
			prop->setTexture(disciple.iconPic().c_str());
			mStrTxt = disciple.iconPic();
			prop->setScale(1);
			if(personInfo->userstatus == 0){
				prop->setColor(ccc3(255,255,255));
			}else if(personInfo->userstatus == 1){
				prop->setColor(ccc3(70,70,70));
			}	
		}
	}else{
		CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(true));
		prop->setTexture(disciple.iconPic().c_str());
		mStrTxt = disciple.iconPic();
		prop->setScale(1);
	}

}

void HandbookPersonContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onShowMemberInfo")
	{
		if(mIsShowDetail)
		{
			ShowDiscipleInfoPopup(mID,true);
		}
	}
}

void HandbookPersonContent::initItemView(){
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void HandbookPersonContent::refreshItemView(cocos2d::extension::CCReViSvItemData* pItem){
	HandbookPersonContentItemData* pItemData = static_cast<HandbookPersonContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iDiscipleID;
		this->refreshContent();
	}
}

void HandbookEquipmentContent::refreshContent(void){

	const EquipItem * equipItem = EquipTableManager::Get()->getEquipItemByID(mID);

	CCMenuItemImage* mFace = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(mFace)
	{
		CCMenu*  menu=dynamic_cast<CCMenu* >(mFace->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
		mFace->setNormalImage(::getFrameNormalSpirte(equipItem->quality));
		mFace->setSelectedImage(::getFrameSelectedSpirte(equipItem->quality));
	}

	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("name"));
	if(name)
	{
		name->setString(equipItem->name.c_str());
	}

	CCSprite* prop = dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));

	const HandbookEquipmentInfo * equipmentInfo = ServerDateManager::Get()->getHandbookEquipmentInfoByID(mID);

	int showFlag = StringConverter::parseInt(VaribleManager::Get()->getSetting("HandbookShowBg"));
	if(showFlag==1||true)
	{
		if(equipmentInfo == NULL)
		{
			mIsShowDetail=false;
			CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(true));
			CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(false));
		}
		else
		{
			mIsShowDetail=true;
			CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(false));
			CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(true));
			prop->removeAllChildren();
			prop->setTexture(equipItem->iconPic.c_str());
			mStrTxt = equipItem->iconPic;
			prop->setScale(0.38f);
			if(equipmentInfo->equipmentstatus == 0)
			{
				prop->setColor(ccc3(255,255,255));
			}
			/*else if(equipmentInfo->equipmentstatus == 1)
			{
				prop->setColor(ccc3(70,70,70));
			}	*/
		}
	}else{
		CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(true));
		prop->setTexture(equipItem->iconPic.c_str());
		mStrTxt = equipItem->iconPic;
		prop->setScale(0.38f);
	}

}

void HandbookEquipmentContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onShowMemberInfo")
	{
		if(mIsShowDetail)
		{
			ShowEquipInfoPopup(mID,true);
		}
	}
}

void HandbookEquipmentContent::initItemView(){
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void HandbookEquipmentContent::refreshItemView(cocos2d::extension::CCReViSvItemData* pItem){
	HandbookEquipmentContentItemData* pItemData = static_cast<HandbookEquipmentContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iEquipmentID;
		this->refreshContent();
	}
}

void HandbookSkillContent::refreshContent(void){

	const SkillItem * skillItem = SkillTableManager::Get()->getSkillItemByID(mID);

	
	CCMenuItemImage* mFace = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	if(mFace)
	{
		CCMenu*  menu=dynamic_cast<CCMenu* >(mFace->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
		mFace->setNormalImage(::getFrameNormalSpirte(skillItem->quality));
		mFace->setSelectedImage(::getFrameSelectedSpirte(skillItem->quality));
	}
	
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("name"));
	if(name)
	{
		name->setString(skillItem->name.c_str());
	}

	CCSprite* prop = dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));

	const HandbookSkillInfo * skillInfo = ServerDateManager::Get()->getHandbookSkillInfoByID(mID);

	int showFlag = StringConverter::parseInt(VaribleManager::Get()->getSetting("HandbookShowBg"));
	if(showFlag==1||true){	
		if(skillInfo == NULL)
		{
			mIsShowDetail=false;
			CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(true));
			CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(false));
		}
		else
		{
			mIsShowDetail=true;
			CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(false));
			CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(true));
			prop->removeAllChildren();
			prop->setTexture(skillItem->iconPic.c_str());
			mStrTxt = skillItem->iconPic;
			prop->setScale(0.38f);
			if(skillInfo->skillstatus == 0){
				prop->setColor(ccc3(255,255,255));
			}else if(skillInfo->skillstatus == 1){
				prop->setColor(ccc3(70,70,70));
			}	
		}
	}else{
		CCB_FUNC(mContentContainer,"mCover",CCSprite,setVisible(false));
		CCB_FUNC(mContentContainer,"name",CCLabelBMFont,setVisible(true));
		prop->setTexture(skillItem->iconPic.c_str());
		mStrTxt = skillItem->iconPic;
		prop->setScale(0.38f);
	}

}

void HandbookSkillContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onShowMemberInfo")
	{
		if(mIsShowDetail)
		{
			ShowSkillInfoPopup(mID,true);
		}
	}
}

void HandbookSkillContent::initItemView(){
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void HandbookSkillContent::refreshItemView(cocos2d::extension::CCReViSvItemData* pItem){
	HandbookSkillContentItemData* pItemData = static_cast<HandbookSkillContentItemData*>(pItem);
	if (pItemData)
	{
		this->mID = pItemData->m_iSkillID;
		this->refreshContent();
	}
}


HandbookPage::HandbookPage(void)
{
	mCurrTabIndex = 1;
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
	flag = true;
	initFlag = false;
}

HandbookPage::~HandbookPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}
void HandbookPage::Enter(MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
//	if(flag){
		OPHandbookInfo handbook;
		handbook.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_HANDBOOK_INFO_C,&handbook);
		flag = false;
//	}

	if(initFlag == true){
		if (mCurrTabIndex == 1){
			mCurrTabIndex = 2;
			onMenuItemAction("onTogglePersonButton",NULL);
		}else if (mCurrTabIndex == 2){
			mCurrTabIndex = 3;
			onMenuItemAction("onToggleEquipmentButton",NULL);
		}else if (mCurrTabIndex == 3){
			mCurrTabIndex = 1;
			onMenuItemAction("onToggleWeaponButton",NULL);
		}
	}else{
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_1"))->selected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_2"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_3"))->unselected();
	}

}
void HandbookPage::Execute(MainFrame* frame){

}
void HandbookPage::Exit(MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
}

void HandbookPage::load(void){
	loadCcbiFile("HandbookPage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("handbookDetail"));
	membersRootNode = members->getContainer();
//	members->setContainer(membersRootNode);
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
//	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
//	m_pScrollViewFacade->init(10,30);
	m_pScrollViewFacade->init(20,30,NULL,5);
}
void HandbookPage::buildItem(void){
	contentNum = 0;
	const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
//	const int iMaxNode = 100;
	int iCount = 0;
	float fOneItemHeight = 0.f;//等高
	float fOneItemWidth = 0.f;
	if(mCurrTabIndex == 1){
		DiscipleTableManager::DiscipleListIterator disConItr = DiscipleTableManager::Get()->getDiscipleIterator();
		while(disConItr.hasMoreElements()){
			if((dynamic_cast<DiscipleItem*>(disConItr.getNext()))->itemID<=1000){
				contentNum++;
			}else{
				break;
			}
		}

		HandbookDateCollect::HandbookIDCollect collect;
		collect = HandbookDateCollect::Get()->getHandbookDiscipleCollection(HandbookFilter_default,HandbookSort_person);
		HandbookDateCollect::HandbookIDCollect::iterator itr = collect.begin();
		while(itr != collect.end()){

			const HandbookPersonInfo * personInfo = ServerDateManager::Get()->getHandbookPersonInfoByID(*itr);

			if(personInfo && personInfo->id > 1000){
				break;
			}
			HandbookPersonContentItemData* pItemData = new HandbookPersonContentItemData();
			pItemData->m_iDiscipleID = *itr;
			pItemData->m_iIdx = iCount;

			HandbookPersonContent* pItem = NULL;
			if(iCount < iMaxNode)
			{
				pItem = new HandbookPersonContent(*itr);
				pItem->initItemView();
				if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
					fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
				if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
					fOneItemWidth = pItem->getContainerNode()->getContentSize().width;

			}
			pItemData->m_ptPosition = ccp(fOneItemWidth*(4-((iCount+(5-contentNum%5))%5)),fOneItemHeight*((iCount+(5-contentNum%5))/5));
			m_pScrollViewFacade->addItem(pItemData,pItem);
			++itr;
			++iCount;

		}
	}else if(mCurrTabIndex == 2){
		EquipTableManager::EquipListIterator equipConItr = EquipTableManager::Get()->getEquipIterator();
		while(equipConItr.hasMoreElements()){
			equipConItr.getNext();
			contentNum++;
		}
		HandbookDateCollect::HandbookIDCollect collect;
		collect = HandbookDateCollect::Get()->getHandbookEquipCollection(HandbookFilter_default,HandbookSort_equip);
		HandbookDateCollect::HandbookIDCollect::iterator itr = collect.begin();
		while(itr != collect.end()){
			const HandbookEquipmentInfo * equipInfo = ServerDateManager::Get()->getHandbookEquipmentInfoByID(*itr);

			HandbookEquipmentContentItemData* pItemData = new HandbookEquipmentContentItemData();
			pItemData->m_iEquipmentID = *itr;
			pItemData->m_iIdx = iCount;

			HandbookEquipmentContent* pItem = NULL;
			if(iCount < iMaxNode)
			{
				pItem = new HandbookEquipmentContent(*itr);
				pItem->initItemView();
				if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
					fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
				if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
					fOneItemWidth = pItem->getContainerNode()->getContentSize().width;

			}
			pItemData->m_ptPosition = ccp(fOneItemWidth*(4-((iCount+(5-contentNum%5))%5)),fOneItemHeight*((iCount+(5-contentNum%5))/5));

			m_pScrollViewFacade->addItem(pItemData,pItem);
			++itr;
			++iCount;
		}
	}else if(mCurrTabIndex == 3){

		SkillTableManager::SkillListIterator skillConItr = SkillTableManager::Get()->getSkillIterator();
		while(skillConItr.hasMoreElements()){
			skillConItr.getNext();
			contentNum++;
		}

		HandbookDateCollect::HandbookIDCollect collect;
		collect = HandbookDateCollect::Get()->getHandbookSkillCollection(HandbookFilter_default,HandbookSort_skill);
		HandbookDateCollect::HandbookIDCollect::iterator itr = collect.begin();
		while(itr != collect.end()){
			const HandbookSkillInfo * skillInfo = ServerDateManager::Get()->getHandbookSkillInfoByID(*itr);

			HandbookSkillContentItemData* pItemData = new HandbookSkillContentItemData();
			pItemData->m_iSkillID = *itr;
			pItemData->m_iIdx = iCount;

			HandbookSkillContent* pItem = NULL;
			if(iCount < iMaxNode)
			{
				pItem = new HandbookSkillContent(*itr);
				pItem->initItemView();
				if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
					fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
				if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
					fOneItemWidth = pItem->getContainerNode()->getContentSize().width;

			}
			pItemData->m_ptPosition = ccp(fOneItemWidth*(4-((iCount+(5-contentNum%5))%5)),fOneItemHeight*((iCount+(5-contentNum%5))/5));

			m_pScrollViewFacade->addItem(pItemData,pItem);
			++itr;
			++iCount;

		}

	}
	/*
	if(members->getContentSize().height>members->getViewSize().height)
	{
		members->setContentOffset(ccp(0,members->minContainerOffset().y));
	}
	else
	{
		members->setContentOffset(ccp(0,members->maxContainerOffset().y));
	}
	members->forceRecaculateChildren();

	*/

	CCSize size = CCSize(fOneItemWidth, fOneItemHeight*((iCount)/5+1));
	members->setContentSize(size);
		/*
			注意ScrollView里除了受控的DynamicItems，还有其他Items
			ScrollView锚点(0.f,0.f)位于左下角
			height由下向上增长
			width由左向右增长
			下面两句顺序不能颠倒，意思是：最后添加的显示到可视区域左上角
		*/
	members->setContentOffset(ccp(0, members->getViewSize().height -  members->getContentSize().height*members->getScaleY() ));
	members->forceRecaculateChildren();
	m_pScrollViewFacade->setDynamicItemsStartPosition(iCount-1);
	{
		//m_pScrollViewFacade->setDynamicItemsStartPosition(0);
	}
		
}
void HandbookPage::addItem(ContentBase* item){
	
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos;

	pos = ccp((item->getContainerNode()->getContentSize().width)*((mContentList.size()-1)%5),item->getContainerNode()->getContentSize().height*((contentNum-1)/5 - (mContentList.size()-1)/5 ));

	item->getContainerNode()->setPosition(pos);
	int intsize = (mContentList.size() + 4 )/5;
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width*5,item->getContainerNode()->getContentSize().height*intsize);

	members->setContentSize(size);
	members->setContentOffset(ccp(0,members->minContainerOffset().y));
		
}
void HandbookPage::rebuildAllItem(void){
	clearAllItem();
	buildItem();
}
void HandbookPage::clearAllItem(void){
	ContentList::iterator personItr = mContentList.begin();
	while(personItr != mContentList.end()){
		delete (*personItr);
		++personItr;
	}
	
	ContentList contentList;
	mContentList.swap(contentList);
	
	membersRootNode->removeAllChildren();
	m_pScrollViewFacade->clearAllItems();
}
void HandbookPage::refreshAllItem(void){
}
void HandbookPage::onMenuItemAction(const std::string& itemName,cocos2d::CCObject* sender){
	if(itemName == "onTogglePersonButton" && mCurrTabIndex != 1){
		mCurrTabIndex = 1;
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_1"))->selected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_2"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_3"))->unselected();

	}else if(itemName == "onToggleEquipmentButton" && mCurrTabIndex != 2){
		mCurrTabIndex = 2;
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_1"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_2"))->selected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_3"))->unselected();

	}else if(itemName == "onToggleWeaponButton" && mCurrTabIndex != 3){
		mCurrTabIndex = 3;
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_1"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_2"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab_3"))->selected();
	}
	rebuildAllItem();
}
void HandbookPage::onMenuItemAction(const std::string& itemName,cocos2d::CCObject* sender,int tag){

}
void HandbookPage::refreshPage(){
}

void HandbookPage::onReceiveMassage( const GameMessage * message ){
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate* >(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_HANDBOOK_INFORET_S)
			{
				initFlag = true;
				rebuildAllItem();
				refreshPage();
			}
		}
	}
}

bool HandbookFilter_default(unsigned int id){
	return true;
}

bool HandbookSort_default(unsigned int id1,unsigned int id2){
	if(id1>id2){
		return true;
	}else{
		return false;
	}
}

bool HandbookSort_person(unsigned int id1,unsigned int id2){

	Disciple disciple1(id1,true);
	Disciple disciple2(id2,true);

	if(disciple1.quality()<disciple2.quality()){
		return false;
	}else if(disciple1.quality()>disciple2.quality()){
		return true;
	}

	if(disciple1.type() > disciple2.type())
		return false;
	else if(disciple1.type() < disciple2.type())
		return true;

	if(disciple1.itemID()<disciple2.itemID()){
		return false;
	}else if(disciple1.itemID()>disciple2.itemID()){
		return true;
	}

	return true;

}

bool HandbookSort_equip(unsigned int id1,unsigned int id2){
	Equip equip1(id1,true);
	Equip equip2(id2,true);

	if(equip1.quality() < equip2.quality())
		return false;
	else if(equip1.quality() > equip2.quality())
		return true;

	if(equip1.type() > equip2.type())
		return false;
	else if(equip1.type() < equip2.type())
		return true;

	if(equip1.itemID() < equip2.itemID())
		return false;
	else if(equip1.itemID() > equip2.itemID())
		return true;

	return true;
}

bool HandbookSort_skill(unsigned int id1,unsigned int id2){
	Skill skill1(id1,true);
	Skill skill2(id2,true);

	if(skill1.quality() < skill2.quality())
		return false;
	else if(skill1.quality() > skill2.quality())
		return true;
	
	if(skill1.type() > skill2.type())
		return false;
	else if(skill1.type() < skill2.type())
		return true;

	if(skill1.itemID() < skill2.itemID())
		return false;
	else if(skill1.itemID() > skill2.itemID())
		return true;
		

	return true;
}

HandbookDateCollect::HandbookIDCollect HandbookDateCollect::getHandbookDiscipleCollection(HandbookFilter sFilter /* = HandbookFilter_default  */, HandbookSort sSort /* = HandbookSort_default */)
{
	HandbookIDCollect myList;
	DiscipleTableManager::DiscipleListIterator discipleItr = DiscipleTableManager::Get()->getDiscipleIterator();
	while(discipleItr.hasMoreElements())
	{
		unsigned int id = discipleItr.getNext()->itemID;
		if (id>1000)
		{
			break;
		}
		if(sFilter(id)){
			myList.push_back(id);
		}
	}
	myList.sort(sSort);
	return myList;

}

HandbookDateCollect::HandbookIDCollect HandbookDateCollect::getHandbookEquipCollection(HandbookFilter sFilter /* = HandbookFilter_default  */, HandbookSort sSort /* = HandbookSort_default */)
{
	HandbookIDCollect myList;
	EquipTableManager::EquipListIterator equipItr = EquipTableManager::Get()->getEquipIterator();
	while(equipItr.hasMoreElements())
	{
		unsigned int id = equipItr.getNext()->itemID;
		if(sFilter(id)){
			myList.push_back(id);
		}
	}
	myList.sort(sSort);
	return myList;

}

HandbookDateCollect::HandbookIDCollect HandbookDateCollect::getHandbookSkillCollection(HandbookFilter sFilter /* = HandbookFilter_default  */, HandbookSort sSort /* = HandbookSort_default */)
{
	HandbookIDCollect myList;
	SkillTableManager::SkillListIterator skillItr = SkillTableManager::Get()->getSkillIterator();
	while(skillItr.hasMoreElements())
	{
		unsigned int id = skillItr.getNext()->itemID;
		if(sFilter(id)){
			myList.push_back(id);
		}
	}
	myList.sort(sSort);
	return myList;

}