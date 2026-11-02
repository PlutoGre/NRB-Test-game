
#include "stdafx.h"

#include "BuyPropsPage.h"
#include "GameMessages.h"
#include "ServerDateManager.h"
#include "DataTableManager.h"
#include "GameMaths.h"
#include "StringConverter.h"
#include "lib91.h"
#include "BuyPropsCountPage.h"
#include "BuyPropInfoPage.h"
#include "GiftPreviewPage.h"
#include "PropInfoPage.h"
#include "MessageBoxPage.h"
#include "json/json.h"
#include "GamePlatform.h"
REGISTER_PAGE("BuyPropsPage",BuyPropsPage);

USING_NS_CC;

BuyPropsPage::BuyPropsPage(void)
	:mShowPage(PAGE_TOOL)
	,mExpbarInitScale(-1.0f)
{
}


BuyPropsPage::~BuyPropsPage(void)
{
}

void BuyPropsPage::Enter( MainFrame* )
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	_refreshTitle();
	if(ServerDateManager::Get()->_getMarketInfo().firstPay)
	{
		CCB_FUNC(this,"m1stPayPic",CCSprite,setVisible(true));
	}
	else
	{
		CCB_FUNC(this,"m1stPayPic",CCSprite,setVisible(false));
	}
	if(mShowPage == PAGE_TOOL)
		createTools();
	else if(mShowPage == PAGE_GIFT)
		createGifts();
	_refreshActivityTip();
}

void BuyPropsPage::Execute( MainFrame* )
{
	const UserBasicInfo& user =  ServerDateManager::Get()->getUserBasicInfo();
	if(mLastFrameRMB != user.goldcoins)
	{
		mLastFrameRMB = user.goldcoins;
		CCB_FUNC(this,"mGold",CCLabelBMFont,setString(StringConverter::toString(user.goldcoins).c_str()));
	}
	static float durationTime=0;
	durationTime+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>1.0f)
	{
		durationTime -=1.0f;
		_refreshActivityTip();
	}
}

void BuyPropsPage::_refreshActivityTip()
{
	Json::Value _status;
	Json::Reader jreader;
	jreader.parse(ServerDateManager::Get()->getUserBasicInfo().actStatus.c_str(),_status,false);
	std::string _key="m";
	bool _isShow=false;
	if(!_status[_key].empty())
	{
		if(_status[_key].asInt()>0)
		{
			_isShow=true;
			if(StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBtnTipOpenStatus"))==1)
			{
				char str[128];
				sprintf(str,"s%d_tip_%s",GamePrecedure::Get()->getServerID(),_key.c_str());
				unsigned int _version=cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(str,0);
				_isShow=_status[_key].asInt()>_version;
			}
			else
			{
				_isShow=false;
			}
		}
	}
	CCB_FUNC(this,"mMemPraise",CCNode,setVisible(_isShow));
	_key="t";
	if(!_status[_key].empty())
	{
		if(_status[_key].asInt()>0)
		{
			_isShow=true;
			if(StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBtnTipOpenStatus"))==1)
			{
				char str[128];
				sprintf(str,"s%d_tip_%s",GamePrecedure::Get()->getServerID(),_key.c_str());
				unsigned int _version=cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(str,0);
				_isShow=_status[_key].asInt()>_version;
			}
			else
			{
				_isShow=false;
			}
		}
	}
	CCB_FUNC(this,"mPropsPraise",CCNode,setVisible(_isShow));
}

void BuyPropsPage::Exit( MainFrame* )
{
	MessageManager::Get()->removeMessageHandler(this);
}

void BuyPropsPage::load( void )
{
	loadCcbiFile("Props.ccbi");
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
}

void BuyPropsPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onRecMem")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "MarketPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if (itemName == "onProps")
	{
		if(mShowPage == PAGE_GIFT)
		{
			createTools();
			mShowPage = PAGE_TOOL;
		}
	}
	else if(itemName == "onGiftPack")
	{
		if(mShowPage == PAGE_TOOL)
		{
			createGifts();
			mShowPage = PAGE_GIFT;
		}
	}
	else if(itemName == "onCallRecharge")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "RechargePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void BuyPropsPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName == "onBuyItem")
	{
		if(mShowPage==PAGE_GIFT)
		{
			if(ServerDateManager::Get()->_getMarketInfo().gotVipGifts.find(tag)!=ServerDateManager::Get()->_getMarketInfo().gotVipGifts.end())
			{
				std::string out = Language::Get()->getString("@BUYLimitTime");
				MSG_BOX(out);
				return;
			}
		}
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsCountPage");
		if(ccb)
		{
			BuyPropsCountPage* page = dynamic_cast<BuyPropsCountPage*>(ccb);
			if(page)
			{
				page->setProductID(tag);
				page->setBuyOneOnly(false);

				MsgMainFramePushPage msg;
				msg.pageName = "BuyPropsCountPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
		
	}
	if(itemName == "onShowPropInfo")
	{
		const ToolItem* tool = ToolTableManager::Get()->getToolItemByID(tag);
		if(tool)
		{
			if(tool->includeOther.empty())
			{
				PropInfoPageShow(tag,PROP_BUY,false);
			}
			else
			{
				CCBContainer* ccb = CCBManager::Get()->getPage("GiftPreviewPage");
				if(ccb)
				{
					GiftPreviewPage* page = dynamic_cast<GiftPreviewPage*>(ccb);
					if(page)
					{
						page->setTitle("@GiftPackPreviewText");
						GiftPreviewPage::PACKCONTENT_LIST _list;
						//std::list<ToolItem::INCLUDED>::const_iterator it = tool->includeOther.begin();
						ResInfoMap::const_iterator it = tool->includeOther.begin();
						for(;it!=tool->includeOther.end();++it)
						{
							//const ToolItem* tool = ToolTableManager::Get()->getToolItemByID(it->);
							//if(tool)
							{
								//GiftPreviewPage::PackContent content(it->statment,it->icon,it->count);
								GiftPreviewPage::PackContent content(it->second->name,it->second->icon,it->second->count,it->second->quality);
								_list.push_back(content);
							}
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
	if(itemName == "onBuyButton")
	{
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsCountPage");
		if(ccb)
		{
			BuyPropsCountPage* page = dynamic_cast<BuyPropsCountPage*>(ccb);
			if(page)
			{
				page->setProductID(tag);
				page->setBuyOneOnly(true);

				MsgMainFramePushPage msg;
				msg.pageName = "BuyPropsCountPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}

	}
}

void BuyPropsPage::createTools()
{
	CCB_FUNC(this,"mProps",CCMenuItemSprite,selected());
	CCB_FUNC(this,"mGiftPack",CCMenuItemSprite,unselected());
	CCObject* obj = getVariable("mPropsContent");
	if(obj)
	{
		cocos2d::extension::CCScrollView* view = dynamic_cast<cocos2d::extension::CCScrollView*>(obj);
		if(view)
		{
			CCNode* node = CCNode::create();
			ToolTableManager::ToolListIterator it = ToolTableManager::Get()->getToolIterator();
			CCSize size(0,0);
			while(it.hasMoreElements())
			{
				ToolItem* item = it.getNext();

				if(!item)continue;
				if(item->pageID != 1)continue;
				if(item->buyCountLimit == 0) continue;

				CCBContainer* ccb = CCBContainer::create();
				ccb->loadCcbiFile("PropsContent.ccbi");
				ccb->setListener(this,item->itemID);
				CCB_FUNC(ccb,"mPropName",CCLabelBMFont,setString(item->name.c_str()));
				std::string des;
				int lines;
				int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("BuyProp_Describe"));
				GameMaths::stringAutoReturn(item->describe,des,width,lines);
				CCB_FUNC(ccb,"mPropDes",CCLabelBMFont,setString(des.c_str()));
				CCB_FUNC(ccb,"mPropPrice",CCLabelBMFont,setString(StringConverter::toString(item->price).c_str()));
				CCB_FUNC(ccb,"mPropPic",CCSprite,initWithFile(item->iconPic.c_str()));
				const UserToolInfo* tool = ServerDateManager::Get()->getUserToolInfoByItemId(item->itemID);
				if(tool)
				{
					CCB_FUNC(ccb,"mPropCount",CCLabelBMFont,setString(StringConverter::toString(tool->count).c_str()));
				}
				else
				{
					CCB_FUNC(ccb,"mPropCount",CCLabelBMFont,setString("0"));
				}

				ccb->setPosition(0,size.height);
				size.width = ccb->getContentSize().width;
				size.height+=ccb->getContentSize().height;
				node->addChild(ccb);
			}

			view->setContainer(node);
			view->setContentSize(size);
			view->setContentOffset(ccp(0,view->minContainerOffset().y));
		}
	}
}

void BuyPropsPage::createGifts()
{
	CCB_FUNC(this,"mProps",CCMenuItemSprite,unselected());
	CCB_FUNC(this,"mGiftPack",CCMenuItemSprite,selected());
	CCObject* obj = getVariable("mPropsContent");
	if(obj)
	{
		cocos2d::extension::CCScrollView* view = dynamic_cast<cocos2d::extension::CCScrollView*>(obj);
		if(view)
		{
			CCNode* node = CCNode::create();
			ToolTableManager::ToolListIterator it = ToolTableManager::Get()->getToolIterator();
			CCSize size(0,0);
			std::list<unsigned int> myList;
			bool _isNewPlayerPack=false;
			unsigned int _newPlayerPackId=3502001;
			while(it.hasMoreElements())
			{
				ToolItem* item = it.getNext();
				if(item->itemID!=_newPlayerPackId)
				{
					myList.push_back(item->itemID);
				}
				else
				{
					_isNewPlayerPack=true;
				}
			}
			myList.sort(&_sSort);
			if(_isNewPlayerPack)
			{
				myList.push_front(_newPlayerPackId);
			}
			std::list<unsigned int >::reverse_iterator itId=myList.rbegin();
			for(;itId!=myList.rend();++itId)
			{
				const ToolItem* item =ToolTableManager::Get()->getToolItemByID(*itId);
				if(!item)continue;
				if(item->pageID != 2)continue;
				if(item->buyCountLimit == 0) continue;
				if(ServerDateManager::Get()->_getMarketInfo().gotVipGifts.find(item->itemID)
					!=ServerDateManager::Get()->_getMarketInfo().gotVipGifts.end())
					continue;

				CCBContainer* ccb = CCBContainer::create();
				ccb->loadCcbiFile("GiftPackContent.ccbi");
				ccb->setListener(this,item->itemID);
				CCB_FUNC(ccb,"mPropName",CCLabelBMFont,setString(item->name.c_str()));
				std::string des;
				int lines;
				int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("BuyProp_Describe"));
				GameMaths::stringAutoReturn(item->describe,des,width,lines);
				CCB_FUNC(ccb,"mPropDes",CCLabelBMFont,setString(des.c_str()));
				CCB_FUNC(ccb,"mPropPrice",CCLabelBMFont,setString(StringConverter::toString(item->originPrice).c_str()));
				CCB_FUNC(ccb,"mSalePrice",CCLabelBMFont,setString(StringConverter::toString(item->price).c_str()));
				CCB_FUNC(ccb,"mPropPic",CCSprite,initWithFile(item->iconPic.c_str()));
				if(item->itemID/1000 == 3501 )
				{
					int vipID = item->itemID%1000;
					const UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
					if(info.viplevel<vipID)
					{
						CCB_FUNC(ccb,"mBuyButton",CCNode,setVisible(false));
					}
				}

				ccb->setPosition(0,size.height);
				size.width = ccb->getContentSize().width;
				size.height+=ccb->getContentSize().height;
				node->addChild(ccb);
			}
			myList.clear();
			view->setContainer(node);
			view->setContentSize(size);
			view->setContentOffset(ccp(0,view->minContainerOffset().y));
		}
	}
}

bool BuyPropsPage::_sSort(unsigned int id1,unsigned int id2)
{
	return id1<id2;
}

void BuyPropsPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S)
		{
			_refreshTitle();
		}
		else if(info->opcode==OPCODE_BUY_TOOLSRET_S)
		{
			CCObject* obj = getVariable("mPropsContent");
			if(obj)
			{
				cocos2d::extension::CCScrollView* view = dynamic_cast<cocos2d::extension::CCScrollView*>(obj);
				if(view)
				{
					CCPoint offset = view->getContentOffset();
					if(mShowPage == PAGE_TOOL)
					{
						createTools();
					}
					else if(mShowPage == PAGE_GIFT)
					{
						createGifts();
					}
					view->setContentOffset(offset);
				}
			}
		}
	}
}

void BuyPropsPage::_refreshTitle()
{
	const UserBasicInfo& user =  ServerDateManager::Get()->getUserBasicInfo();
	CCB_FUNC(this,"mName",CCLabelTTF,setString(user.name.c_str()));
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
	mLastFrameRMB = user.goldcoins;
	CCB_FUNC(this,"mGold",CCLabelBMFont,setString(StringConverter::toString(user.goldcoins).c_str()));
	CCB_FUNC(this,"mCoin",CCLabelBMFont,setString(StringConverter::toString(user.silvercoins).c_str()));
	CCB_FUNC(this,"mTi",CCLabelBMFont,setString((StringConverter::toString(user.power)+"/"+StringConverter::toString(user.maxpower)).c_str()));
	CCB_FUNC(this,"mQi",CCLabelBMFont,setString((StringConverter::toString(user.vitality)+"/"+StringConverter::toString(user.maxvitality)).c_str()));
	std::string expstr = ServerDateManager::Get()->getUserExpPercentStr();
	CCB_FUNC(this,"mExp",CCLabelBMFont,setString(expstr.c_str()));
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(user.level).c_str()));
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{
		const std::string vipImage = ::getVipLevelImageFile(user.viplevel);
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
