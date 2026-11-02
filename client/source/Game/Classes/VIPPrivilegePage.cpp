
#include "stdafx.h"

#include "VIPPrivilegePage.h"
#include "DataTableManager.h"
#include "GameMaths.h"



REGISTER_PAGE("VIPPrivilegePage",VIPPrivilegePage);
USING_NS_CC;

void VIPPrivilegeContentItem::refreshContent(void){

	const VIPPrivilegeItem * vipPrivilegeItem = VIPPrivilegeTableManager::Get()->getVIPPrivilegeItemByID(mID);

	
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("content1"));
	if(name)
	{
		std::string outstr;
		int lines;
		GameMaths::stringAutoReturn(vipPrivilegeItem->describe[this->describeIndex],outstr,StringConverter::parseInt(VaribleManager::Get()->getSetting("VIPPrivilegeDescribe")),lines);
		
		name->setString(outstr.c_str());
	}
	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(vipPrivilegeItem->iconPic.c_str());
	}
	
}

void VIPPrivilegeContentItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}

void VIPPrivilegeContent::refreshContent(void){

	CCSprite* myVIPPic=dynamic_cast<CCSprite* >(mContentContainer->getVariable("myVIPPic"));
	itemStartPos =this->getContentSize().height-10.0f;
	mLineIndex = 0;
	CCLabelBMFont* mVipLevel=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mVipLevel"));
	if(mVipLevel)
	{
		mVipLevel->setString(StringConverter::toString(mID).c_str());
		itemStartPos = mVipLevel->getPositionY()-mVipLevel->getContentSize().height-10;
	}

	itemMembers = mContentContainer->getVariable("vipPrivilegeItemDetail");
	itemMembersRootNode = dynamic_cast<cocos2d::CCNode*>(itemMembers);

	rebuildAllItem(mID);
}

void VIPPrivilegeContent::rebuildAllItem(int mID){
	clearAllItem();
	buildItem(mID);
}

void VIPPrivilegeContent::clearAllItem(void){

}
void VIPPrivilegeContent::buildItem(int mID){
	const VIPPrivilegeItem* vipPrivilegeItem = VIPPrivilegeTableManager::Get()->getVIPPrivilegeItemByID(mID);

	int num = vipPrivilegeItem->describeNum;
	for(unsigned int i=0;i < (vipPrivilegeItem->describeNum);i++){
		
		VIPPrivilegeContentItem* vipPrivilegeContentItem  = new VIPPrivilegeContentItem(mID);
		vipPrivilegeContentItem->describeIndex = i;
		vipPrivilegeContentItem->init();
		addItem(vipPrivilegeContentItem);
	}
}
void VIPPrivilegeContent::addItem(ContentBase* item){
	mContentItemList.push_back(item);
	itemMembersRootNode->addChild(item->getContainerNode());
	const VIPPrivilegeItem * vipPrivilegeItem = VIPPrivilegeTableManager::Get()->getVIPPrivilegeItemByID(mID);
	std::string inStr = vipPrivilegeItem->describe[dynamic_cast<VIPPrivilegeContentItem* >(item)->describeIndex];
	std::string outstr;
	int lines = 0;
	int descLen = StringConverter::parseInt(VaribleManager::Get()->getSetting("VIPPrivilegeDescribe"));

	GameMaths::stringAutoReturn(inStr,outstr,descLen,lines);
	
	float height = item->getContainerNode()->getContentSize().height+5;
	CCPoint pos = ccp(0,itemStartPos-height*(mLineIndex+1));
	mLineIndex += (lines +1);
	item->getContainerNode()->setPosition(pos);
	
}
void VIPPrivilegeContent::onMenuItemAction(const std::string& itemName,cocos2d::CCObject* sender , int tag){

}

VIPPrivilegePage::VIPPrivilegePage(void){
}

VIPPrivilegePage::~VIPPrivilegePage(void){
}

void VIPPrivilegePage::Enter(MainFrame* ){
	rebuildAllItem();
	refreshPage();
}

void VIPPrivilegePage::Execute(MainFrame* ){
}

void VIPPrivilegePage::Exit(MainFrame*){
}

void VIPPrivilegePage::load(void){
	loadCcbiFile("VipPage.ccbi");
	members = dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("vipPrivilegeDetail"));
	membersRootNode = members->getContainer();


}

void VIPPrivilegePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender){
	if(itemName=="onClose")
	{
		//¹Ø±Õ
		MsgMainFramePopPage msg;
		msg.pageName = "VIPPrivilegePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void VIPPrivilegePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag){
}


void VIPPrivilegePage::rebuildAllItem(void){
	refreshTitle();
	clearAllItem();
	buildItem();
}

void VIPPrivilegePage::refreshTitle(void){
	
	int viplevel = ServerDateManager::Get()->getUserBasicInfo().viplevel;
		
	const VIPPrivilegeItem* vipPrivilegeItem = VIPPrivilegeTableManager::Get()->getVIPPrivilegeItemByID(viplevel+1);

	CCLabelBMFont* vipTitle=dynamic_cast<CCLabelBMFont* >(getVariable("vipTitle"));
	std::string outStr; 
	if(viplevel == 0){
		outStr = Language::Get()->getString("@vipDescribeWithoutMoney");
	}else if(viplevel == VIPPrivilegeTableManager::Get()->getTotalNum()){
		outStr = "";
	}else{
		std::string temStr = Language::Get()->getString("@vipDescribeWithMoney");
		std::list<std::string> _list;

		_list.push_back(StringConverter::toString(viplevel).c_str());
		_list.push_back(StringConverter::toString(vipPrivilegeItem->rechangeNum - ServerDateManager::Get()->getUserBasicInfo().rechargenum).c_str());
		_list.push_back(StringConverter::toString(viplevel+1).c_str());

		outStr = GameMaths::replaceStringWithStringList(temStr,&_list);

	}
	if(vipTitle)
	{
		vipTitle->setString(outStr.c_str());
	}


}
void VIPPrivilegePage::clearAllItem(void){
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end()){
		delete (*itr);
		++itr;
	}
	ContentList contentList;
	mContentList.swap(contentList);

	membersRootNode->removeAllChildren();


}

void VIPPrivilegePage::buildItem(void){
	VIPPrivilegeTableManager::VIPPrivilegeListIterator vipItr = VIPPrivilegeTableManager::Get()->getVIPPrivilegeIterator();
//	VIPPrivilegeTableManager::VIPPrivilegeList list = 
	while(vipItr.hasMoreElements()){
		VIPPrivilegeItem * item = vipItr.getNext();
		int len = item->describe->length();
		std::string des1 = item->describe[0];
		std::string des2 = item->describe[1];
		VIPPrivilegeContent * vipPrivilegeItem = new VIPPrivilegeContent(item->itemID);
		vipPrivilegeItem->initContentBase();
		addItem(vipPrivilegeItem);
	}
	members->forceRecaculateChildren();
}

void VIPPrivilegePage::addItem(ContentBase* item){
	
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(VIPPrivilegeTableManager::Get()->getTotalNum() - mContentList.size()));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width,item->getContainerNode()->getContentSize().height*(mContentList.size()));
	members->setContentSize(size);
	members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void VIPPrivilegePage::refreshPage(){

}













