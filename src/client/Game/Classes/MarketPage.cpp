
#include "stdafx.h"

#include "MarketPage.h"
#include "GameMessages.h"
#include "ServerDateManager.h"
#include "GameMaths.h"
#include "StringConverter.h"
#include "lib91.h"
#include "waitingManager.h"
#include "BuyPropsPage.h"
#include "PopupPage.h"
#include "TimeCalculator.h"
#include "MessageBoxPage.h"
#include "Popup1stPayTipPage.h"
#include "json/json.h"
#include "GamePlatform.h"
#include "ActivityPopPage.h"
REGISTER_PAGE("MarketPage",MarketPage);

USING_NS_CC;
MarketPage::MarketPage(void)
	:mExpbarInitScale(-1.0f)
{
}


MarketPage::~MarketPage(void)
{
}

void MarketPage::Enter( MainFrame* )
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	mEclipsedTime = 0;
	_updateTimes();
	_refreshTitle();
	if(ServerDateManager::Get()->_getMarketInfo().firstPay)
	{
		CCB_FUNC(this,"m1stPayPic",CCSprite,setVisible(true));
	}
	else
	{
		CCB_FUNC(this,"m1stPayPic",CCSprite,setVisible(false));
	}
	CCB_FUNC(this,"mTab_1",CCMenuItemSprite,selected());
	mHasActivity = false;
	_refreshActivityTip("mem","m");
	_refreshActivityTip("tool","t");
}

void MarketPage::Execute( MainFrame* )
{
	_updateTimes();

	const UserBasicInfo& user =  ServerDateManager::Get()->getUserBasicInfo();
	if(lastFrameRMB != user.goldcoins)
	{
		lastFrameRMB = user.goldcoins;
		CCB_FUNC(this,"mGold",CCLabelBMFont,setString(StringConverter::toString(user.goldcoins).c_str()));
	}
	static float durationTime=0;
	durationTime+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>1.0f)
	{
		durationTime -=1.0f;
		_refreshActivityTip("mem","m");
		_refreshActivityTip("tool","t");
	}
}

void MarketPage::_refreshActivityTip(const std::string guideName,std::string _key)
{
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		Json::Value _status;
		Json::Reader jreader;
		jreader.parse(ServerDateManager::Get()->getUserBasicInfo().actStatus.c_str(),_status,false);
		bool _isShow=false;
		if(!_status[_key].empty())
		{
			if(_status[_key].asInt()>0)
			{
				_isShow=true;
			}
		}
		
		if(guideName=="mem")
		{
			CCB_FUNC(this,"mPraiseA",CCNode,setVisible(_isShow));
			CCB_FUNC(this,"mPraiseB",CCNode,setVisible(_isShow));
			mHasActivity = _isShow;
			if(_isShow&&StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBtnTipOpenStatus"))==1)
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
			CCB_FUNC(this,"mMemPraise",CCNode,setVisible(_isShow));
		}
		else
		{
			if(_isShow&&StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBtnTipOpenStatus"))==1)
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
			CCB_FUNC(this,"mPropsPraise",CCNode,setVisible(_isShow));
		}
	}
	else
	{
		CCB_FUNC(this,"mPraiseA",CCNode,setVisible(false));
		CCB_FUNC(this,"mPraiseB",CCNode,setVisible(false));
		CCB_FUNC(this,"mMemPraise",CCNode,setVisible(false));
		CCB_FUNC(this,"mPropsPraise",CCNode,setVisible(false));
	}
}

void MarketPage::_saveActivityGuideStatus(const std::string guideName,std::string key)
{
	int _openStatus=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideTipOpenStatus"));
	if(_openStatus==1)
	{
		CCObject* _obj=getVariable(guideName);
		if(_obj)
		{
			CCNode* _guide=dynamic_cast<CCNode* >(_obj);
			bool isShow=_guide->isVisible();
			if(isShow)
			{
				Json::Value _status;
				Json::Reader jreader;
				jreader.parse(ServerDateManager::Get()->getUserBasicInfo().actStatus.c_str(),_status,false);
				if(!_status[key].empty())
				{
					if(_status[key].asInt()>0)
					{
						char str[128];
						sprintf(str,"s%d_tip_%s",GamePrecedure::Get()->getServerID(),key.c_str());
						cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(str,_status[key].asInt());
						cocos2d::CCUserDefault::sharedUserDefault()->flush();
					}
				}
				_guide->setVisible(false);
			}
		}
	}	
}

void MarketPage::Exit( MainFrame* )
{
	CCB_FUNC(this,"mTab_1",CCMenuItemSprite,unselected());
	MessageManager::Get()->removeMessageHandler(this);
	mHasActivity = false;
}

void MarketPage::load( void )
{
	loadCcbiFile("Fair.ccbi");
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
}

void MarketPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	std::string sendKey="MarketPageSendPacket";
	if(itemName == "onRecharge")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "RechargePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "mTab_1")
	{
		
	}
	else if(itemName == "onToggleItem")
	{
		_saveActivityGuideStatus("mPropsPraise","t");
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsPage");
		if(ccb)
		{
			BuyPropsPage* page = dynamic_cast<BuyPropsPage*>(ccb);
			if(page)
			{
				page->setPage(BuyPropsPage::PAGE_TOOL);

				MsgMainFrameChangePage msg;
				msg.pageName = "BuyPropsPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
	else if(itemName == "onToggleGift")
	{
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsPage");
		if(ccb)
		{
			BuyPropsPage* page = dynamic_cast<BuyPropsPage*>(ccb);
			if(page)
			{
				page->setPage(BuyPropsPage::PAGE_GIFT);
				MsgMainFrameChangePage msg;
				msg.pageName = "BuyPropsPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
	else if(itemName == "onBuy1")
	{
		if(!_checkEnoughGold(1))
		{
			MsgMainFramePopPage msg;
			msg.pageName = "BuyPropsCountPage";
			MessageManager::Get()->sendMessage(&msg);
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

		OPMarketRecruitDisciple pkt;
		pkt.set_type((int)mCanBuy[0]);
		pkt.set_kind(1);
		PacketManager::Get()->sendPakcet(OPCODE_MARKET_RECRUIT_DISCIPLE_C,&pkt);
		//waitingManager::Get()->startWaiting(OPCODE_MARKET_RECRUIT_DISCIPLERET_S);
		
	}
	else if(itemName == "onBuy2")
	{
		if(!_checkEnoughGold(2))
		{
			MsgMainFramePopPage msg;
			msg.pageName = "BuyPropsCountPage";
			MessageManager::Get()->sendMessage(&msg);
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

		OPMarketRecruitDisciple pkt;
		pkt.set_type((int)mCanBuy[1]);
		pkt.set_kind(2);
		PacketManager::Get()->sendPakcet(OPCODE_MARKET_RECRUIT_DISCIPLE_C,&pkt);
		//waitingManager::Get()->startWaiting(OPCODE_MARKET_RECRUIT_DISCIPLERET_S);
	}
	else if(itemName == "onBuy3")
	{
		if(!_checkEnoughGold(3))
		{
			MsgMainFramePopPage msg;
			msg.pageName = "BuyPropsCountPage";
			MessageManager::Get()->sendMessage(&msg);
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

		OPMarketRecruitDisciple pkt;
		pkt.set_type((int)mCanBuy[2]);
		pkt.set_kind(3);
		PacketManager::Get()->sendPakcet(OPCODE_MARKET_RECRUIT_DISCIPLE_C,&pkt);
		//waitingManager::Get()->startWaiting(OPCODE_MARKET_RECRUIT_DISCIPLERET_S);
	}
	else if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onPop2")
	{
		if(mHasActivity)
		{
			ActivityPop(SendSoulTwo);
		}
		
	}
	else if(itemName=="onPop3")
	{
		if(mHasActivity)
		{
			ActivityPop(SendSoulThree);
		}
	}
}

void MarketPage::_updateTimes()
{
	const MarketInfo& user =  ServerDateManager::Get()->_getMarketInfo();

	CCNode * mFirstB = dynamic_cast<CCNode*>(this->getVariable("mFirstB"));
	if(mFirstB){
		if(user.firstbaili == true){
				mFirstB->setVisible(true);
		}else if(user.firstbaili == false){
				mFirstB->setVisible(false);
		}
	}

	CCNode * mFirstA = dynamic_cast<CCNode*>(this->getVariable("mFirstA"));
	if(mFirstA){
		if(user.firstwanli == true){
			mFirstA->setVisible(true);
		}else if(user.firstwanli == false){
			mFirstA->setVisible(false);
		}
	}

// 	struct cc_timeval now;
// 	int eclipsedtime = GamePrecedure::Get()->getTotalTime();
// 	if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
// 		eclipsedtime =now.tv_sec - user.msgGotTime;
// 
// 	if(eclipsedtime!=mEclipsedTime)
// 	{
// 		mEclipsedTime = eclipsedtime;
// 		int time1 = user.recurittime1 - mEclipsedTime;
// 		int time2 = user.recurittime2 - mEclipsedTime;
// 		int time3 = user.recurittime3 - mEclipsedTime;
// 		translateTime(time1,1,user.recuritTenCount,5,user.recuritCost1);
// 		translateTime(time2,2,1,1,user.recuritCost2);
// 		translateTime(time3,3,1,1,user.recuritCost3);
// 	}
	long timeleft;
	if(TimeCalculator::Get()->getTimeLeft("marketinfo_recurittime1",timeleft))
		translateTime(timeleft,1,user.recuritTenCount,5,user.recuritCost1);
	if(TimeCalculator::Get()->getTimeLeft("marketinfo_recurittime2",timeleft))
		translateTime(timeleft,2,1,1,user.recuritCost2);
	if(TimeCalculator::Get()->getTimeLeft("marketinfo_recurittime3",timeleft))
		translateTime(timeleft,3,1,1,user.recuritCost3);
}

void MarketPage::translateTime( int seconds , int id, int Count, int maxCount, int price)
{
	char btnStr[20];
	sprintf(btnStr,"mBuy%d",id);
	if(seconds<=0)
	{
		mCanBuy[id-1] = ORDINARL;
		mPrice[id-1]=0;
		std::string freeCount = Language::Get()->getString("@RecruitPageFreeCount");
		char times[128];
		if(maxCount==1)
		{
			sprintf(times,"1");
		}
		else
		{
			sprintf(times,"%d/%d",Count,maxCount);
		}
		std::list<std::string> _list;
		_list.push_back(times);
		std::string replaced = GameMaths::replaceStringWithStringList(freeCount,&_list);
		bool isGold=false;
		if(maxCount==5&&Count<=0)
		{
			replaced=Language::Get()->getString("@RecruitPageNotFreeCount");
			mCanBuy[id-1] = USING_RMB;
			isGold=true;
			CCB_FUNC(this,btnStr,CCLabelBMFont,setString(Language::Get()->getString("@Buy").c_str()));
		}else
		{
			CCB_FUNC(this,btnStr,CCLabelBMFont,setString(Language::Get()->getString("@RecMem").c_str()));

		}
		char names[64];
		sprintf(names,"mTxt%d",id);
		CCB_FUNC(this,names,CCLabelBMFont,setString(replaced.c_str()));
		sprintf(names,"mGold%d",id);
		CCB_FUNC(this,names,CCLabelBMFont,setString(StringConverter::toString(price).c_str()));
		CCB_FUNC(this,names,CCLabelBMFont,setVisible(isGold));
		sprintf(names,"mGoldIcon%d",id);
		CCB_FUNC(this,names,CCNode,setVisible(isGold));
	}
	else
	{
		mCanBuy[id-1] = USING_RMB;
		mPrice[id-1]=price;
		char str[64];
		int hh = seconds/3600;
		int mm = (seconds-3600*hh)/60;
		int ss = seconds%60;
		sprintf(str,"%02d:%02d:%02d",hh,mm,ss);


		std::string freeTime = Language::Get()->getString("@RecruitPageFreeTime");
		std::list<std::string> _list;
		_list.push_back(str);
		std::string replaced = GameMaths::replaceStringWithStringList(freeTime,&_list);
		char names[64];
		sprintf(names,"mTxt%d",id);
		CCB_FUNC(this,names,CCLabelBMFont,setString(replaced.c_str()));
		sprintf(names,"mGold%d",id);
		CCB_FUNC(this,names,CCLabelBMFont,setVisible(true));
		char priceStr[64];
		sprintf(priceStr,"%d",price);
		CCB_FUNC(this,names,CCLabelBMFont,setString(priceStr));
		sprintf(names,"mGoldIcon%d",id);
		CCB_FUNC(this,names,CCNode,setVisible(true));

		CCB_FUNC(this,btnStr,CCLabelBMFont,setString(Language::Get()->getString("@Buy").c_str()));
	}
	
}


void MarketPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S)
		{
			_refreshTitle();
		}
		else if(info->opcode==OPCODE_MARKET_RECRUIT_DISCIPLERET_S)
		{
			_refreshTitle();
		}
	}
}

void MarketPage::_refreshTitle()
{
	const UserBasicInfo& user =  ServerDateManager::Get()->getUserBasicInfo();
	CCB_FUNC(this,"mName",CCLabelTTF,setString(user.name.c_str()));
	lastFrameRMB = user.goldcoins;
	CCB_FUNC(this,"mGold",CCLabelBMFont,setString(StringConverter::toString(user.goldcoins).c_str()));
	CCB_FUNC(this,"mCoin",CCLabelBMFont,setString(StringConverter::toString(user.silvercoins).c_str()));
	CCB_FUNC(this,"mTi",CCLabelBMFont,setString((StringConverter::toString(user.power)+"/"+StringConverter::toString(user.maxpower)).c_str()));
	CCB_FUNC(this,"mQi",CCLabelBMFont,setString((StringConverter::toString(user.vitality)+"/"+StringConverter::toString(user.maxvitality)).c_str()));
	std::string expstr = ServerDateManager::Get()->getUserExpPercentStr();
	CCB_FUNC(this,"mExp",CCLabelBMFont,setString(expstr.c_str()));
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

bool MarketPage::_checkEnoughGold(unsigned int id)
{
	const UserBasicInfo& info =  ServerDateManager::Get()->getUserBasicInfo();
	if(mCanBuy[id-1]==USING_RMB)
	{
		if (info.goldcoins<mPrice[id-1])
		{
			return false;
		}
	}
	return true;
}