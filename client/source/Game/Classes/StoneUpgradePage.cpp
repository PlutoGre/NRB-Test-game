
#include "stdafx.h"

#include "StoneUpgradePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"
#include "math.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("StoneUpgradePage",StoneUpgradePage);
USING_NS_CC;

StoneUpgradePage::StoneUpgradePage(void)
{
	mID = -1;
	mBuyOneOnly = false;
	mCanBuyMaxCount = 999;
	membersRootNode = NULL;
	members = NULL;
	mLongBtnHeight = 0.0f;
	mContentList.clear();
	mTabMap.clear();
}

StoneUpgradePage::~StoneUpgradePage(void)
{
}

void StoneUpgradePage::Enter( MainFrame* frame)
{
	refreshPage();

}

void StoneUpgradePage::Exit( MainFrame* frame)
{
	clearAllItem();

}

void StoneUpgradePage::Execute( MainFrame* )
{

}

void StoneUpgradePage::load( void )
{
	loadCcbiFile("StoneSystemLevelUpPage.ccbi");
//	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mEquipContent"));
//	membersRootNode = members->getContainer();

}

void StoneUpgradePage::buildItem( void )
{
}

void StoneUpgradePage::addItem( ContentBase* item )
{
}

void StoneUpgradePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void StoneUpgradePage::clearAllItem( void )
{
}

void StoneUpgradePage::refreshAllItem( void )
{
}

void StoneUpgradePage::removeItem( ContentBase* item )
{
}

void StoneUpgradePage::buildEquipItem(){

}

void StoneUpgradePage::choosePage(int tag){
}

void StoneUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose"){
		MsgMainFramePopPage msg;
		msg.pageName="StoneUpgradePage";
		MessageManager::Get()->sendMessage(&msg);
	}else if(itemName == "onSure"){

		if(userToolInfo->count < StoneTableManager::Get()->getStoneItemByID(toolItem->itemID)->upgradeNum){
			MSG_BOX(Language::Get()->getString("@StoneNumNotEnough").c_str());
			return;
		}
		int currStoneId = BlackBoard::Get()->currentStone;
		const StoneItem* currStoneItem = StoneTableManager::Get()->getStoneItemByID(currStoneId);
		if(ServerDateManager::Get()->getUserBasicInfo().silvercoins < currStoneItem->upgradeSilver){
			MSG_BOX(Language::Get()->getString("@SilverNumNotEnough"));
			return;
		}

		OPUpgradeStoneInfo info;
		info.set_version(1);
		info.set_stoneid(BlackBoard::Get()->currentStone);
		info.set_stonenum(mCount);
		PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_STONEINFO_C,&info);
		MsgMainFramePopPage msg;
		msg.pageName="StoneUpgradePage";
		MessageManager::Get()->sendMessage(&msg);

	}else if(itemName == "onReduceNum"){
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

}
void StoneUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag)
{


}

void StoneUpgradePage::refreshPage( void )
{
	userToolInfo = ServerDateManager::Get()->getUserToolInfoByID(BlackBoard::Get()->currentStoneMId);
	if(!userToolInfo)
	{
		return;
	}
	toolItem = ToolTableManager::Get()->getToolItemByID(userToolInfo->itemid);
	mID = userToolInfo->itemid;
	if(!toolItem)
	{
		return;
	}

	int currStoneId = BlackBoard::Get()->currentStone;
	const StoneItem* currStoneItem = StoneTableManager::Get()->getStoneItemByID(currStoneId);
	const StoneItem* nextStoneItem = StoneTableManager::Get()->getStoneItemByID(currStoneItem->nextLevelID);


	CCB_FUNC(this,"mBeforeLV",CCLabelBMFont,setString(StringConverter::toString(currStoneItem->level).c_str()));
	CCB_FUNC(this,"mAfterLV",CCLabelBMFont,setString(StringConverter::toString(nextStoneItem->level).c_str()));
	
	CCB_FUNC(this,"mMoney",CCLabelBMFont,setString(StringConverter::toString((userToolInfo->count / currStoneItem->upgradeNum)*currStoneItem->upgradeSilver).c_str()));

	CCB_FUNC(this,"mBeforeAttackNum",CCLabelBMFont,setString(StringConverter::toString(currStoneItem->attackBuff).c_str()));
	CCB_FUNC(this,"mAfterAttackNum",CCLabelBMFont,setString(StringConverter::toString(nextStoneItem->attackBuff).c_str()));
	CCB_FUNC(this,"mBeforeArmorNum",CCLabelBMFont,setString(StringConverter::toString(currStoneItem->defendBuff).c_str()));
	CCB_FUNC(this,"mAfterArmorNum",CCLabelBMFont,setString(StringConverter::toString(nextStoneItem->defendBuff).c_str()));
	CCB_FUNC(this,"mBeforeJewelryNum",CCLabelBMFont,setString(StringConverter::toString(currStoneItem->bloodBuff).c_str()));
	CCB_FUNC(this,"mAfterJewelryNum",CCLabelBMFont,setString(StringConverter::toString(nextStoneItem->bloodBuff).c_str()));

	CCB_FUNC(this,"mStoneLevel",CCLabelBMFont,setString(StringConverter::toString(currStoneItem->level).c_str()))
	CCB_FUNC(this,"mStoneAmount",CCLabelBMFont,setString(StringConverter::toString(userToolInfo->count).c_str()))

	CCB_FUNC(this,"mStonePic",CCSprite,initWithFile(toolItem->iconPic.c_str()))
	CCB_FUNC(this,"mStoneColor",CCSprite,setVisible(false))

	CCB_FUNC(this,"mStoneName",CCLabelBMFont,setString(toolItem->name.c_str()))
	CCLabelBMFont* mStoneUpgradeTexts = dynamic_cast<CCLabelBMFont*>(getVariable("mTexts"));
	if(mStoneUpgradeTexts){
		std::string outstr;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneUpgradeTexts"));
		GameMaths::stringAutoReturn(Language::Get()->getString("@StoneUpgradeTexts"),outstr,width,lines);
		mStoneUpgradeTexts->setString(outstr.c_str());
	}
	
	mCount = 1;

	CCB_FUNC(this,"mBeforeNum",CCLabelBMFont,setString(StringConverter::toString(mCount * currStoneItem->upgradeNum).c_str()))
	CCB_FUNC(this,"mAfterNum",CCLabelBMFont,setString(StringConverter::toString(mCount).c_str()))

	caculateEnoughCount();
	caculatePrice();

}

void StoneUpgradePage::caculatePrice()
{
	if(mID>0)
	{
		const ToolItem* item = ToolTableManager::Get()->getToolItemByID(mID);
		if(item)
		{
			int currStoneId = BlackBoard::Get()->currentStone;
			const StoneItem* currStoneItem = StoneTableManager::Get()->getStoneItemByID(currStoneId);
			mTotalPrice = mCount*currStoneItem->upgradeSilver;
			if(!checkGoldEnough())
			{
				mCount = mCanBuyMaxCount;
				CCB_FUNC(this,"mPropNote",CCLabelBMFont,setString(Language::Get()->getString("@BuyStoneCount").c_str()));
			}
			else
			{				  
				CCB_FUNC(this,"mPropNote",CCLabelBMFont,setString(Language::Get()->getString("@BuyStoneCount").c_str()));
			}

			if(userToolInfo->count <= currStoneItem->upgradeNum){
				mCount = mCanBuyMaxCount;
			}

			CCB_FUNC(this,"mNum",CCLabelBMFont,setString(StringConverter::toString(mCount).c_str()));
			CCB_FUNC(this,"mTotalNum",CCLabelBMFont,setString(StringConverter::toString(mTotalPrice).c_str()));

			CCB_FUNC(this,"mBeforeNum",CCLabelBMFont,setString(StringConverter::toString(currStoneItem->upgradeNum * mCount).c_str()));
			CCB_FUNC(this,"mAfterNum",CCLabelBMFont,setString(StringConverter::toString(mCount).c_str()));

			CCB_FUNC(this,"mMoney",CCLabelBMFont,setString(StringConverter::toString(mCount * currStoneItem->upgradeSilver).c_str()));

		}
	}
}

bool StoneUpgradePage::checkGoldEnough()
{
	const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
	if(info.silvercoins<mTotalPrice)
	{
		return false;
	}
	return true;
}
void StoneUpgradePage::caculateEnoughCount()
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
	int currStoneId = BlackBoard::Get()->currentStone;
	const StoneItem* currStoneItem = StoneTableManager::Get()->getStoneItemByID(currStoneId);

	mEnoughCount = int(info.silvercoins/currStoneItem->upgradeSilver);

	mCanBuyMaxCount = userToolInfo->count / currStoneItem->upgradeNum;

	if(mEnoughCount>mCanBuyMaxCount)
	{
		mEnoughCount = mCanBuyMaxCount;
	}else{
		mCanBuyMaxCount = mEnoughCount;
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
void StoneUpgradePage::sellectTab( unsigned int tabIndex )
{
}
