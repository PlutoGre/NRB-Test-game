
#include "stdafx.h"

#include "PopupManager.h"
#include "BlackBoard.h"
#include "PropInfoPage.h"
#include "SkillBookSurprisePage.h"
#include "BattleArrayInfo.h"
#include "FightGuidePage.h"
#include "Equip.h"
#include "GuideBuyEquipPage.h"
#include "GuideBuyExpPage.h"
#include "DataTableManager.h"
#include "Popup1stPayTipPage.h"
#include "AdventurePage.h"
PopupManager::PopupManager(void)
{
	mCurrLevel=0;
	mIsUpgrade=false;
	mShowDrop=false;
	mShowNewActiveToolPage=false;
	mIsResolving=false;
	mReqGuideAdventure=false;
	mStoneGuideFlag = true;
}

PopupManager::~PopupManager(void)
{
	mIsUpgrade=false;
	mShowDrop=false;
	mShowNewActiveToolPage=false;
	mIsResolving=false;
	mReqGuideAdventure=false;
	MessageManager::Get()->removeMessageHandler(this);
}

void PopupManager::init()
{
	mNewActiveToolPage= dynamic_cast<NewActivePromptToolPage*>(CCBManager::Get()->getPage("NewActivePromptToolPage"));
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
}

void PopupManager::popPage()
{
	bool isShowPage=true;
	if(mShowDrop)
	{
		if(mShowNewActiveToolPage)
		{
			_showNewActiveTool();
		}
		else
		{
			if(mIsUpgrade)
			{
				_showUpgrade();
			}
			else
			{
				isShowPage=false;
			}
		}
		mShowDrop = false;
	}
	else
	{
		if(mIsUpgrade)
		{
			_showUpgrade();
		}
		else
		{
			isShowPage=false;
		}
	}
	std::string fightCareerKey="fromCareerFight";
	bool isFightCareer=false;
	if(HAS_V(fightCareerKey))
	{
		isFightCareer=GET_V_B(fightCareerKey);
	}
	if(isFightCareer)
	{
		if(!isShowPage)
		{
			if(!_showFightGuide())
			{
				isShowPage=false;
			}
		}
		SET_V(fightCareerKey,false);
	}
	if(!isShowPage)
	{
		if(!_showBuyEquipGuide())
		{
			_showBuyExpGuide();
		}
	}
}
void PopupManager::_showGetReward()
{
	bool isShowPopup=false;
	if(mType==GetPropManager::TOOL)
	{
		isShowPopup=true;
		PropInfoPageShow(mId,PROP_USE,true);
	}
	else
	{
		BasePage* bp = CCBManager::Get()->getPage("PopUpSurprisePage");
		if(bp)
		{
			PopUpSurprisePage* page = dynamic_cast<PopUpSurprisePage*>(bp);
			if(page)
			{
				if(mType==GetPropManager::EQUIPMENT)
				{
					BlackBoard::Get()->ShowEquip = mId;
					MsgMainFramePushPage msg;
					msg.pageName = "EquipSurprisePage";
					MessageManager::Get()->sendMessage(&msg);
					isShowPopup=true;
				}
				else if(mType==GetPropManager::SKILL)
				{
					ADD_OR_SET_V("SkillSurprise", mId);
					MsgMainFramePushPage msg;
					msg.pageName = "SkillSurprisePage";
					MessageManager::Get()->sendMessage(&msg);
					isShowPopup=true;
				}
				else if(mType==GetPropManager::SOUL)
				{
					page->setInfo(mType,mId,mCount);
					if(mDropStatus["soul"].asInt()==1)
					{
						MsgMainFramePushPage msg;
						msg.pageName = "PopUpSurprisePage";
						MessageManager::Get()->sendMessage(&msg);
					}
					mShowNewActiveToolPage=_pushNewActiveTool("soul");
					isShowPopup=true;
				}
				else if(mType==GetPropManager::SKILLBOOK)
				{
					BasePage* skillBookBp = CCBManager::Get()->getPage("SkillBookSurprisePage");
					SkillBookSurprisePage* skillBookPage = dynamic_cast<SkillBookSurprisePage*>(skillBookBp);
					if(skillBookPage)
					{
						skillBookPage->setInfo(mSkillId,mId,mCount);
					}
					MsgMainFramePushPage msg;
					msg.pageName = "SkillBookSurprisePage";
					MessageManager::Get()->sendMessage(&msg);
					mShowNewActiveToolPage=_pushNewActiveTool("skillBook",mSkillId);
					isShowPopup=true;
				}
				else if(mType==GetPropManager::SKILLPIECE)
				{

				}
				else
				{
					page->setInfo(mType,mId,mCount);
					MsgMainFramePushPage msg;
					msg.pageName = "PopUpSurprisePage";
					MessageManager::Get()->sendMessage(&msg);
					mShowNewActiveToolPage=_pushNewActiveTool("soul");
					isShowPopup=true;
				}
			}
		}
	}
	if(!isShowPopup)
	{
		mShowDrop=false;
		popPage();
	}
	else
	{
		mShowDrop=true;
	}
}
bool PopupManager::showStoneGuide(void){
	if(mStoneGuideFlag){
		if(ServerDateManager::Get()->getUserBasicInfo().level >=  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneGuideStartLevel")) && ServerDateManager::Get()->getUserBasicInfo().level <  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneGuideEndLevel"))){
			return false;
		}
	
		if(cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("showStoneGuide",0) == 0){
			return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

void PopupManager::setStoneGuide(){
	if(mStoneGuideFlag){
		cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey("showStoneGuide",1);
		cocos2d::CCUserDefault::sharedUserDefault()->flush();
		mStoneGuideFlag = false;
	}
}


bool PopupManager::_showFightGuide(void)
{
	if(BlackBoard::Get()->showFightGuide == false){
		BlackBoard::Get()->showFightGuide = true;
		return false;
	}
	std::string fightRetKey="careerFightFail";
	if(HAS_V(fightRetKey))
	{
		bool isWin=GET_V_B(fightRetKey);
		if(!isWin)
		{
			_checkIsResolving();
			std::string key="GToBattle";
			//show Battle;
			if(mDropStatus[key].asInt()!=1)
			{
				int inBattleCount=ServerDateManager::Get()->getUserBattleSize();
				int battleCount=BattleArrayInfo::Get()->getBattleEnableSize();
				if(battleCount!=inBattleCount)
				{
					BlackBoard::Get()->TeamShowIndex=inBattleCount+1;
					_sendGuideRecordPack(key);
					POP_FIGHT_GUIDE(key);
					mDropStatus[key]=Json::Value(1);
					return true;
				}
				key="GToEquip";
			}
			else
			{
				key="GToEquip";
			}
			//show equip 
			if(mDropStatus[key].asInt()!=1)
			{
				UserBattleInfoListIterator::iterator it=ServerDateManager::Get()->getUserBattleIterator().begin();
				bool isShowEquiqGuide=false;
				int _index=-1;
				int _equipType=0;
				for(;it!=ServerDateManager::Get()->getUserBattleIterator().end();++it)
				{
					if(it->second->equip1==0)
					{
						_equipType=2;
						isShowEquiqGuide=true;
					}
					else if(it->second->equip2==0)
					{
						_equipType=3;
						isShowEquiqGuide=true;
					}
					else if(it->second->equip3==3)
					{
						_equipType=4;
						isShowEquiqGuide=true;
					}
					if(isShowEquiqGuide)
					{
						_index=it->second->position;
						break;
					}
				}
				if(_index!=-1)
				{
					isShowEquiqGuide=false;
					EquipDateCollect::EquipIDCollect collect;
					if(_equipType == 2)
					{
						collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Type1);
					}
					else if(_equipType == 3)
					{
						collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Type2);
					}
					else if(_equipType == 4)
					{
						collect = EquipDateCollect::Get()->getEquipCollection(EquipFilter_Type3);
					}

					EquipDateCollect::EquipIDCollect::reverse_iterator itr = collect.rbegin();
					while(itr != collect.rend())
					{
						Equip equip(*itr);
						if(equip.getEquipCarriedBy().length()==0)
						{
							isShowEquiqGuide=true;
							BlackBoard::Get()->TeamShowIndex=_index;
							break;
						}
						++itr;
					}
					
				}

				if(isShowEquiqGuide)
				{
					_sendGuideRecordPack(key);
					POP_FIGHT_GUIDE(key);
					mDropStatus[key]=Json::Value(1);
					return true;
				}
				key="GToBuy";
			}
			else
			{
				key="GToBuy";
			}
			//show buy member
			if(mDropStatus[key].asInt()!=1)
			{
				_sendGuideRecordPack(key);
				POP_FIGHT_GUIDE(key);
				mDropStatus[key]=Json::Value(1);
				return true;
			}
			return false;
		}
	}
	return false;
}

void PopupManager::_checkIsResolving()
{
	if(!mIsResolving)
	{
		Json::Reader jreader;
		jreader.parse(ServerDateManager::Get()->getUserBasicInfo().dropStatus.c_str(),mDropStatus,false);
		std::vector<std::string > lvList=GameMaths::tokenize(VaribleManager::Get()->getSetting("GuideCoinsEnoughLV"),",");
		std::vector<std::string >::iterator it=lvList.begin();
		for(;it!=lvList.end();++it)
		{
			mGuideLVList.push_back(StringConverter::parseInt(*it));
		}
		lvList.clear();
		mIsResolving=true;
	}
}

int PopupManager::_getCurrGuideLevel()
{
	if(mCurrLevel!=ServerDateManager::Get()->getUserBasicInfo().level)
	{
		mCurrLevel=ServerDateManager::Get()->getUserBasicInfo().level;
		LvList::iterator it=mGuideLVList.begin();
		for(;it!=mGuideLVList.end();++it)
		{
			if(mCurrLevel<=*it)
			{
				mGuideLevel=*it;
				break;
			}
		}
	}
	return mGuideLevel;
}

void PopupManager::_showUpgrade(void)
{
	MsgMainFramePushPage msg;
	msg.pageName = "PlayerUpgradePage";
	MessageManager::Get()->sendMessage(&msg);
	mIsUpgrade=false;
}

//弹出升级指引
void PopupManager::showNewActiveGrade(void)
{
	NewActivePromptTableManager::NewActivePromptList * list =  NewActivePromptTableManager::Get()->getNewAcrivePromptItemByLevel(ServerDateManager::Get()->getUserBasicInfo().level);
	NewActivePromptTableManager::NewActivePromptList::iterator itr = list->begin();
	bool isShow = false;
	if(itr != list->end())
	{
		NewActivePromptItem* item = dynamic_cast<NewActivePromptItem*>(itr->second);

		BasePage* bp = CCBManager::Get()->getPage("NewActivePromptPage");
		if(bp)
		{
			NewActivePromptPage* page = dynamic_cast<NewActivePromptPage*>(bp);
			if(page)
			{
				page->setIterator(itr);
				page->setEndIterator(list->end());
				page->setInfo(item);
				MsgMainFramePushPage msg;
				msg.pageName = "NewActivePromptPage";
				MessageManager::Get()->sendMessage(&msg);
				isShow = true;
			}
		}
	}
	if(!isShow)
	{
		this->popPage();
	}
}

bool PopupManager::checkShowBuyExp(void)
{
	_checkIsResolving();
	int _level=ServerDateManager::Get()->getUserBasicInfo().level;
	int _minLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyExpMinLevel"));
	int _maxLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyExpMaxLevel"));
	if(_level>=_minLevel&&_level<=_maxLevel)
	{
		std::string key="GBuyExp";
		if(!mDropStatus[key].empty())
		{
			if(mDropStatus[key].asInt()==1)
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}
bool PopupManager::checkShowBuyEquip(void)
{
	_checkIsResolving();
	//show buy equip 
	int _level=ServerDateManager::Get()->getUserBasicInfo().level;
	int _minLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipMinLevel"));
	int _maxLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipMaxLevel"));
	if(_level>=_minLevel&&_level<=_maxLevel)
	{
		std::string key="GBuyEquip";
		if(!mDropStatus[key].empty())
		{
			if(mDropStatus[key].asInt()==1)
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool PopupManager::checkShowContinueAd(void)
{
	_checkIsResolving();
	//check show buy equip 
	int _level=ServerDateManager::Get()->getUserBasicInfo().level;
	int _maxLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("AdventureContinueTipLevel"));
	if(_level<=_maxLevel)
	{
		if(!mReqGuideAdventure&&ServerDateManager::Get()->getAdventureItemInfoMapTotalNum()==0)
		{
			OPAdventureInfo msg;
			msg.set_version(1);
			PacketManager::Get()->sendPakcet(OPCODE_GET_ADVENTURE_INFO_C, &msg);
			return false;
		}
		else
		{
			AdventureItemInfoMapIterator itr = ServerDateManager::Get()->getAdventureItemInfoMapIterator();
			while(itr.hasMoreElements())
			{
				const AdventureItemInfo* adventureItemInfo=itr.getNext();
				const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(adventureItemInfo->adventureId);
				if(adventureItem->adventureType==ContinueLogin)
				{
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

void PopupManager::_showBuyExpGuide(void)
{
	_checkIsResolving();
	int _level=ServerDateManager::Get()->getUserBasicInfo().level;
	int _minLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyExpMinLevel"));
	int _maxLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyExpMaxLevel"));
	if(_level>=_minLevel&&_level<=_maxLevel)
	{
		std::string key="GBuyExp";
		if(mDropStatus[key].asInt()!=1&&mDropStatus[key].asInt()!=2)
		{
			mDropStatus[key]=Json::Value(1);
			_sendGuideRecordPack(key);
			POP_GUIDE_BuyExp();
		}
	}
}

bool PopupManager::_showBuyEquipGuide(void)
{
	_checkIsResolving();
	int _level=ServerDateManager::Get()->getUserBasicInfo().level;
	int _requireMaxLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipMaxLevel"));
	int _requireMinLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideBuyEquipMinLevel"));
	if(_level<=_requireMaxLevel&&_level>=_requireMinLevel)
	{
		std::string key="GBuyEquip";
		if(mDropStatus[key].asInt()!=1&&mDropStatus[key].asInt()!=2)
		{
			mDropStatus[key]=Json::Value(1);
			_sendGuideRecordPack(key);
			POP_FIGHT_GUIDE_BuyEquip();
			return true;
		}
	}
	return false;
}


void PopupManager::showCoinsEnoughGuide(void)
{
	_checkIsResolving();
	if(ServerDateManager::Get()->getUserBasicInfo().rechargenum==0)
	{
		if(ServerDateManager::Get()->getUserBasicInfo().silvercoins<StringConverter::parseInt(VaribleManager::Get()->getSetting("GuideCoinsLimit")))
		{
			int level=_getCurrGuideLevel();
			char key[128];
			sprintf(key,"GRecharge%d",level);
			if(mDropStatus[key].asInt()!=1)
			{
				mDropStatus[key]=Json::Value(1);
				_sendGuideRecordPack(key);
				POP_1STPAY_TIP_BOX(SilverNotEnough);
			}
		}
	}
}

void PopupManager::_sendGuideRecordPack(std::string _key)
{
	OPGuideRecord msg;
	msg.set_version(1);
	msg.set_type(_key);
	PacketManager::Get()->sendPakcet(OPCODE_GUIDE_RECORD_C, &msg);
}

void PopupManager::setShowStatsByType(std::string _type,int stats)
{
	mDropStatus[_type]=Json::Value(stats);
}

//弹出掉落工具指引
bool PopupManager::_pushNewActiveTool(std::string _type,unsigned int _id)
{
	_checkIsResolving();
	if(mDropStatus[_type].asInt()==1)
	{
		return false;
	}
	else
	{
		NewActivePromptTableManager::NewActivePromptItemListIterator it=NewActivePromptTableManager::Get()->getNewActivePromptListIterator();
		while(it.hasMoreElements())
		{
			NewActivePromptItem* item=it.getNext();
			if(item->dropType==_type)
			{
				mNewActiveToolPage->setInfo(item,_id);
				mShowNewActiveToolPage=true;
				mDropStatus[_type]=Json::Value(1);
				break;
			}
		}
	}
	return true;
}


void PopupManager::_showNewActiveTool()
{
	MsgMainFramePushPage msg;
	msg.pageName = "NewActivePromptToolPage"; 
	MessageManager::Get()->sendMessage(&msg);
	mShowNewActiveToolPage=false;
}

void PopupManager::pushAllPage(GetPropManager::TYPE _type, int id, int count,int skillId)
{
	mType = _type;
	mId = id;
	mCount = count;
	mSkillId=skillId;
	_showGetReward();
}

void PopupManager::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate* >(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_USER_UPGRADE_S)
			{
				mIsUpgrade=true;
			}
			else if(msg->opcode==OPCODE_GET_ADVENTURE_INFORET_S)
			{
				mReqGuideAdventure=false;
			}
		}
	}
	else if(message->getTypeId()==MSG_FIGHT_END)
	{
		const MsgFightEnd* msg=dynamic_cast<const MsgFightEnd*>(message);
		if(msg)
		{
			if(!mShowDrop)
			{
				popPage();
			}
		}
	}
}