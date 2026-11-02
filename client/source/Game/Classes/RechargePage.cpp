
#include "stdafx.h"

#include "RechargePage.h"
#include "GameMessages.h"
#include "ServerDateManager.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "lib91.h"
#include "OP.pb.h"
#include "ShopList.pb.h"
#include "PacketManager.h"
#include "json/json.h"
#include "GameMaths.h"
#include "GamePlatform.h"
#include "MessageBoxPage.h"
#include "libPlatform.h"

REGISTER_PAGE("RechargePage",RechargePage);

USING_NS_CC;
USING_NS_CC_EXT;

RechargePage::RechargePage(void)
{
}


RechargePage::~RechargePage(void)
{
}

void RechargePage::initContent()
{
	CCObject * obj = getVariable("mShopList");
	if(obj)
	{
		CCScrollView* view = dynamic_cast<CCScrollView*>(obj);
		if(view)
		{
			CCNode* node = CCNode::create();
			CCSize size(0,0);
			ShopItemList::iterator it = ServerDateManager::Get()->mShopList.begin();
			int i = 0;
			for(;it!=ServerDateManager::Get()->mShopList.end(); ++it,++i)
			{
				CCBContainer* framebutton = CCBContainer::create();
				framebutton->loadCcbiFile("RechargeItem.ccbi");
				//framebutton->setListener(this,FACE_BUTTON_START+i);
				framebutton->setPosition(0,framebutton->getContentSize().height*(i));
				size.width = framebutton->getContentSize().width;
				size.height+=framebutton->getContentSize().height;
				framebutton->setListener(this,i);
				CCObject* rmb = framebutton->getVariable("mItemRMB");
				CCObject* gold = framebutton->getVariable("mItemGold");
				CCObject* gift = framebutton->getVariable("mItemGift");
				if(rmb && gold && gift)
				{
					CCLabelBMFont * lrmb = dynamic_cast<CCLabelBMFont*>(rmb);
					CCLabelBMFont * lgold = dynamic_cast<CCLabelBMFont*>(gold);
					CCLabelBMFont * lgift = dynamic_cast<CCLabelBMFont*>(gift);
					if(lrmb && lgold && lgift)
					{
						char str[64];
						sprintf(str,"%d",(int)(ServerDateManager::Get()->mShopList[i]->productPrice*libPlatformManager::getPlatform()->getPlatformChangeRate()));
						lrmb->setString(str);
						sprintf(str,"%d",ServerDateManager::Get()->mShopList[i]->productCount);
						lgold->setString(str);
						sprintf(str,"%d",(int)(ServerDateManager::Get()->mShopList[i]->productOrignalPrice));
						lgift->setString(str);
					}
				}
                
               
                
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
				CCObject* platCoin = framebutton->getVariable("mPlatformCoin");
				if(platCoin)
				{
					CCLabelBMFont * lplatCoin = dynamic_cast<CCLabelBMFont*>(platCoin);
					char cPlat[64];
					sprintf(cPlat,"@%s", PlatformNameManager::getInstance()->getPlatNameByID(libPlatformManager::getPlatform()->getPlatformId()).c_str());
					if(Language::Get()->hasString(cPlat))
					{
						lplatCoin->setString(Language::Get()->getString(cPlat).c_str());
					}else
					{
						lplatCoin->setString(Language::Get()->getString("@Android_Default").c_str());
					}
				}
#else
			 CCB_FUNC(framebutton,"mPlatformCoin", CCLabelBMFont, setString(libPlatformManager::getInstance()->getPlatform()->getPlatformMoneyName().c_str()));
#endif
				node->addChild(framebutton);
			}

			CCBContainer* firstPay = 0;
			if(ServerDateManager::Get()->_getMarketInfo().firstPay)
				firstPay = readFirstPayInfo();
			if(firstPay)
			{
				firstPay->setPosition(0,size.height);
				size.height+=firstPay->getContentSize().height;
				node->addChild(firstPay);
			}

			node->setContentSize(size);
			view->setContainer(node);
			view->setContentSize(size);
			view->setContentOffset(ccp(0,-size.height+view->getViewSize().height));
		}
	}

	int viplevel = ServerDateManager::Get()->getUserBasicInfo().viplevel;

	const VIPPrivilegeItem* vipPrivilegeItem = VIPPrivilegeTableManager::Get()->getVIPPrivilegeItemByID(viplevel+1);

	CCLabelBMFont* vipTitle=dynamic_cast<CCLabelBMFont* >(getVariable("mRechargMsg"));
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

void RechargePage::Enter( MainFrame* )
{
	mShowRechargeMessage = false;
	OPShopList shop;
	shop.set_version(1);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	shop.set_platname(PlatformNameManager::getInstance()->getPlatNameByID(libPlatformManager::getPlatform()->getPlatformId()));
#endif
	PacketManager::Get()->sendPakcet(OPCODE_GET_SHOP_ITEM_LIST_C,&shop);

	libPlatformManager::getPlatform()->registerListener(this);
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_RECHARGE_SUCCESS,this);
	initContent();
}

void RechargePage::Execute( MainFrame* )
{
}

void RechargePage::Exit( MainFrame* )
{
	libPlatformManager::getPlatform()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
}

void RechargePage::load( void )
{
	loadCcbiFile("Recharge.ccbi");
}

void RechargePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if("onOprButton_1" == itemName)
	{

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//		//xinzheng 2013-06-24
//		{//·â±Õ²âÊÔÔÝ²»¿ª³äÖµ
//			std::string outStr = Language::Get()->getString("@TEST_PAY_NOT_OPEN");
//			libOS::getInstance()->showMessagebox(outStr);
//			return;
//		}
//#else
		BUYINFO buyinfo;
		if(tag<ServerDateManager::Get()->mShopList.size() && tag>=0)
		{
			ShopItem* item = ServerDateManager::Get()->mShopList[tag];
			buyinfo.productCount = item->productCount;
			buyinfo.productName = item->productName;
			buyinfo.productId = item->productID;
			buyinfo.productPrice = item->productPrice;
			buyinfo.productOrignalPrice = item->productOrignalPrice;
			char severIDStr[64];
			sprintf(severIDStr,"%d",ServerDateManager::Get()->getSeverID());
			buyinfo.description = severIDStr;
			libPlatformManager::getPlatform()->buyGoods(buyinfo);
		}
//#endif
	}
}

void RechargePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "RechargePage";
		MessageManager::Get()->sendMessage(&msg);
	}else if(itemName == "onVipPrivilege"){
		MsgMainFramePushPage msg;
		msg.pageName="VIPPrivilegePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void RechargePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode == OPCODE_GET_SHOP_ITEM_LISTRET_S)
			initContent();
	}
	if(message->getTypeId() == MSG_RECHARGE_SUCCESS)
	{
		mShowRechargeMessage = true;
	}

}

CCBContainer* RechargePage::readFirstPayInfo()
{
	Json::Reader jreader;
	Json::Value data;
	unsigned long filesize;
	char* pBuffer = (char*)getFileData(
		cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename("pay1st.cfg").c_str(),
		"rt",&filesize);

	if(!pBuffer)
	{
		char msg[256];
		sprintf(msg,"Failed open file: pay1st.cfg !!");
		cocos2d::CCMessageBox(msg,"ERROR!");
	}
	else
	{
		jreader.parse(pBuffer,data,false);
		CC_SAFE_DELETE_ARRAY(pBuffer);

		if(data["version"].asInt()==1)
		{
			CCBContainer* firstPay = CCBContainer::create();
			firstPay->loadCcbiFile("RewardFor1stPay.ccbi");
			CCB_FUNC(firstPay,"mItem1Pic",CCSprite,setTexture(data["icon1"].asString().c_str()));
			CCB_FUNC(firstPay,"mItem2Pic",CCSprite,setTexture(data["icon2"].asString().c_str()));
			CCB_FUNC(firstPay,"mItem3Pic",CCSprite,setTexture(data["icon3"].asString().c_str()));
			CCB_FUNC(firstPay,"mItem4Pic",CCSprite,setTexture(data["icon4"].asString().c_str()));
			std::string str = data["description1"].asString();
			CCB_FUNC(firstPay,"mReward1",CCLabelBMFont,setString(str.c_str()));
			str = data["description2"].asString();
			CCB_FUNC(firstPay,"mReward2",CCLabelBMFont,setString(str.c_str()));
			str = data["description3"].asString();
			CCB_FUNC(firstPay,"mReward3",CCLabelBMFont,setString(str.c_str()));
			str = data["description4"].asString();
			CCB_FUNC(firstPay,"mReward4",CCLabelBMFont,setString(str.c_str()));
			if(data["quality1"].asInt()>0)
			{
				CCB_FUNC(firstPay,"mItem1Frame",CCSprite,setColor(::getFrameColor(data["quality1"].asInt())));
			}
			if(data["quality2"].asInt()>0)
			{
				CCB_FUNC(firstPay,"mItem2Frame",CCSprite,setColor(::getFrameColor(data["quality2"].asInt())));
			}
			if(data["quality3"].asInt()>0)
			{
				CCB_FUNC(firstPay,"mItem3Frame",CCSprite,setColor(::getFrameColor(data["quality3"].asInt())));
			}
			if(data["quality4"].asInt()>0)
			{
				CCB_FUNC(firstPay,"mItem4Frame",CCSprite,setColor(::getFrameColor(data["quality4"].asInt())));
			}

			CC_SAFE_DELETE_ARRAY(pBuffer);
			return firstPay;
		}
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
	return 0;
}

void RechargePage::onBuyinfoSent(lib91*, bool success, const BUYINFO&, const std::string& log)
{
	if(success)
	{
		if (log == "callpay")
		{
			MSG_BOX(Language::Get()->getString("@PayCall"));
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@PaySend"));
		}
	}
}
