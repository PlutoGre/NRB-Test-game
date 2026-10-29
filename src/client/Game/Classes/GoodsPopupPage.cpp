
#include "stdafx.h"

#include "StoneUpgradePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"
#include "math.h"
#include "GoodsPopupPage.h"

REGISTER_PAGE("GoodsPopupPage",GoodsPopupPage);
USING_NS_CC;

GoodsPopupPage::GoodsPopupPage(void)
{
	mSure=NULL;
}

GoodsPopupPage::~GoodsPopupPage(void)
{
}

void GoodsPopupPage::Enter( MainFrame* frame)
{
	const ResInfo* _info=ResManager::Get()->getResInfoByTypeAndId(mType,mItemId,mCount,mPartId);

	CCB_FUNC(this,"mHeadPic",CCSprite,setTexture(_info->icon.c_str()));
	CCB_FUNC(this,"mItemName",CCLabelBMFont,setString(_info->name.c_str()));
	CCB_FUNC(this,"mFrame",CCMenuItemImage,setNormalImage(::getFrameNormalSpirte(_info->quality>0?_info->quality:4)));
	CCB_FUNC(this,"mFrame",CCMenuItemImage,setSelectedImage(::getFrameNormalSpirte(_info->quality>0?_info->quality:4)));
	
	if(mCount>1)
	{
		unsigned int mainType=ResManager::Get()->getResMainType(mType);
		if(mainType==USER_PROPERTY_TYPE||mainType==TOOLS_TYPE)
		{
			CCB_FUNC(this,"mDeepSeaLV",CCNode,setVisible(false));
			CCB_FUNC(this,"mDeepSeaNum",CCNode,setVisible(true));
			CCB_FUNC(this,"mItemNum",CCLabelBMFont,setString(StringConverter::toString(_info->count).c_str()));
		}
		else
		{
			CCB_FUNC(this,"mDeepSeaLV",CCNode,setVisible(true));
			CCB_FUNC(this,"mDeepSeaNum",CCNode,setVisible(false));
			CCB_FUNC(this,"mItemLV",CCLabelBMFont,setString(StringConverter::toString(_info->count).c_str()));
			CCB_FUNC(this,"lvlnum1",CCSprite,setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(_info->quality>0?_info->quality:4)))));
		}
		
	}
	else
	{
		CCB_FUNC(this,"mDeepSeaNum",CCNode,setVisible(false));
		CCB_FUNC(this,"mDeepSeaLV",CCNode,setVisible(false));
	}
}

void GoodsPopupPage::Exit( MainFrame* frame)
{

}

void GoodsPopupPage::Execute( MainFrame* )
{

}

void GoodsPopupPage::load( void )
{
	loadCcbiFile("DeepSeaTreasurePage.ccbi");
}

void GoodsPopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName="GoodsPopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onButton")
	{
		if(mSure)
		{
			mSure();
		}
		MsgMainFramePopPage msg;
		msg.pageName="GoodsPopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}

}
void GoodsPopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag)
{


}

void GoodsPopupPage::showPage(unsigned int type,unsigned int Id,unsigned int count,unsigned int partId)
{
	MsgMainFramePushPage msg;
	msg.pageName="GoodsPopupPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("GoodsPopupPage");
	GoodsPopupPage* page = dynamic_cast<GoodsPopupPage*>(obj);
	if(page)
	{
		page->mType=type;
		page->mItemId=Id;
		page->mCount=count;
		page->mPartId=partId;
	}
}

void GoodsPopupPage::showGoodsPopup(unsigned int type,unsigned int Id,unsigned int count,unsigned int partId)
{
	dynamic_cast<GoodsPopupPage*>(CCBManager::Get()->getPage("GoodsPopupPage"))->showPage(type,Id,count,partId);
}
