
#include "stdafx.h"

#include "BuyStoneCountPage.h"
#include "DataTableManager.h"
#include "StringConverter.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Language.h"
#include "GameMessages.h"
#include "BlackBoard.h"
#include "PopupPage.h"
#include "Popup1stPayTipPage.h"
REGISTER_PAGE("BuyStoneCountPage",BuyStoneCountPage);
USING_NS_CC;
BuyStoneCountPage::BuyStoneCountPage(void)
	:mID(-1)
	,mBuyOneOnly(false),mCanBuyMaxCount(999)
{
}


BuyStoneCountPage::~BuyStoneCountPage(void)
{
}


void BuyStoneCountPage::Enter( MainFrame* )
{
	mCount = needNum - hasNum;
	
	caculatePrice();
	caculateEnoughCount();
}

void BuyStoneCountPage::Execute( MainFrame* )
{

}

void BuyStoneCountPage::Exit( MainFrame* )
{

}

void BuyStoneCountPage::load( void )
{
	loadCcbiFile("PropsBuyPopUp.ccbi");
}

void BuyStoneCountPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose" || itemName == "onShutdown")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "BuyStoneCountPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onReduceNum")
	{
		if (mCount <= 1)
			return;
		mCount--;
		if(mCount<1)mCount = 1;
		if(mCount==1)
		{
			CCB_FUNC(this,"mReduceButton10",CCMenuItemImage,setVisible(false));
		}
		CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(true));
		caculatePrice();
	}
	else if(itemName == "onAddNum")
	{
		if (mCount >= mCanBuyMaxCount)
			return;
		mCount++;
		if(mCount>=mEnoughCount)
		{			
			CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(false));
		}
		if(mCount>1)
		{
			CCB_FUNC(this,"mReduceButton10",CCMenuItemImage,setVisible(true));
		}
		if(mCount>999)
		{
			mCount = 999;
			CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(false));
		}
		caculatePrice();
	}
	else if(itemName == "onReduceNum10")
	{
		mCount-=10;
		if(mCount<=0)
		{
			mCount = 1;
		}
		if(mCount==1)
		{
			CCB_FUNC(this,"mReduceButton10",CCMenuItemImage,setVisible(false));
		}
		CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(true));
		caculatePrice();
	}
	else if(itemName == "onAddNum10")
	{
		mCount+=10;
		if(mCount > mCanBuyMaxCount)
			mCount = mCanBuyMaxCount;
		if(mCount>=mEnoughCount)
		{			
			mCount = mEnoughCount;
			CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(false));
		}
		if(mCount>1)
		{
			CCB_FUNC(this,"mReduceButton10",CCMenuItemImage,setVisible(true));
		}
		if(mCount>999)
		{
			mCount = 999;
			CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(false));
		}
		caculatePrice();
	}
	else if(itemName == "onConfirm")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "BuyStoneCountPage";
		MessageManager::Get()->sendMessage(&msg);
		if(!checkGoldEnough())
		{
			if(!ServerDateManager::Get()->_getMarketInfo().firstPay)
			{
				POP_BOX(NotEnoughGold);
			}
			else
			{
				POP_1STPAY_TIP_BOX(GoldNotEnough);
			}
			return;
		}
		BlackBoard::Get()->MarketBuyItemId = mID;
		OPBuyTool pkt;
		pkt.set_itemid(mID);
		pkt.set_count(mCount);
		PacketManager::Get()->sendPakcet(OPCODE_BUY_TOOLS_C,&pkt);

	}
}

void BuyStoneCountPage::caculatePrice()
{
	if(mID>0)
	{
		const ToolItem* item = ToolTableManager::Get()->getToolItemByID(mID);
		if(item)
		{
			mTotalPrice = mCount*item->price;
			if(!checkGoldEnough())
			{
				CCB_FUNC(this,"mPropNote",CCLabelBMFont,setString(Language::Get()->getString("@BuyPropCountNotEnough").c_str()));
			}
			else
			{
				std::list<std::string> _list;
				_list.push_back(StringConverter::toString(hasNum).c_str());
				_list.push_back(item->name.c_str());
				_list.push_back(StringConverter::toString(needNum - hasNum).c_str());
				std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@removeHoldTxt"),&_list);
				CCB_FUNC(this,"mPropNote",CCLabelBMFont,setString(replaced.c_str()));
			}
			
			CCB_FUNC(this,"mNum",CCLabelBMFont,setString(StringConverter::toString(mCount).c_str()));
			CCB_FUNC(this,"mTotalNum",CCLabelBMFont,setString(StringConverter::toString(mTotalPrice).c_str()));
			
		}
	}
}

bool BuyStoneCountPage::checkGoldEnough()
{
	const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
	if(info.goldcoins<mTotalPrice)
	{
		return false;
	}
	return true;
}
void BuyStoneCountPage::caculateEnoughCount()
{
	const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
	const ToolItem* item = ToolTableManager::Get()->getToolItemByID(mID);
	//
	mCanBuyMaxCount = item->buyCountLimit;
	if (mCanBuyMaxCount < 0)//item->buyCountLimit==-1
		mCanBuyMaxCount = 999;
	if (mCanBuyMaxCount > 999)
		mCanBuyMaxCount = 999;
	//
	mEnoughCount = int(info.goldcoins/item->price);
	if(mEnoughCount>mCanBuyMaxCount)
	{
		mEnoughCount = mCanBuyMaxCount;
	}
	
	CCB_FUNC(this,"mReduceButton10",CCMenuItemImage,setVisible(false));
	if(mEnoughCount>1)
	{
		CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(true));
	}
	else
	{
		CCB_FUNC(this,"mAddButton10",CCMenuItemImage,setVisible(false));
	}
	
}
