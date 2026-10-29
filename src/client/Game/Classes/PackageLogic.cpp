
#include "stdafx.h"

#include "PackageLogic.h"
#include "ServerDateManager.h"
#include "GameMessages.h"
#include "BlackBoard.h"
#include "GamePrecedure.h"
#include "CCBManager.h"
#include "GetPropManager.h"
#include "StringConverter.h"
#include "cocos2d.h"
#include "DiscipleRecruitPage.h"
#include "libOS.h"
#include "PropUpPage.h"
#include "PropInfoPage.h"
#include "MessageBoxPage.h"
#include "PackPreviewPage.h"
#include "FightPage.h"
#include "SoundManager.h"
#include "Arena.h"
#include "ThreadSocket.h"
#include "TimeCalculator.h"
#include "LoginPacket.h"
#include "SeverConsts.h"
#include "WorldBossJoinBattle.h"
#include "StoneInlaidPage.h"


void PacketHandlerGeneral::onSendPacketFailed( const int opcode )
{
	if(opcode != _getOpcode() - 1)
		return;

	LoginPacket::Get()->sendPacket();
}

void PacketHandlerGeneral::onTimeout( const int opcode )
{
	if(opcode != _getOpcode())
		return;

	LoginPacket::Get()->sendPacket();
	//MSG_BOX("@ReceivedTimeout");
}

void PacketHandlerGeneral::onPacketError( const int opcode )
{
	if(opcode != _getOpcode())
		return;

	GamePrecedure::Get()->requestPackages();
	//MSG_BOX("@ReceivedErrorPackage");
}

class LoginHandle : public PacketHandlerGeneral, public libOSListener
{ 
public: 
		static const int mOpcode = OPCODE_PLAYER_LOGINRET_S; 
		LoginHandle() 
		{ 
			PacketManager::Get()->regisiterPacketHandler(mOpcode,this); 
		} 
		virtual void onMessageboxEnter( int tag)
		{
			if(tag == 102)
			{
				exit(0);
			}
		}
		virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet)
		{
			const OPLoginRet * loginRet = dynamic_cast<const OPLoginRet*>(packet);
			int SeverGameVersion,SeverAppVersion,SeverResVersion;
			int LocalGameVersion,LocalAppVersion,LocalResVersion;
			std::string versionStr = loginRet->resourceversion();
			if(versionStr!="")
			{
				SeverConsts::Get()->checkVersion(versionStr,SeverGameVersion,SeverAppVersion,SeverResVersion);
				SeverConsts::Get()->checkVersion(SeverConsts::Get()->getVersion(),LocalGameVersion,LocalAppVersion,LocalResVersion);

				bool isOk = true;
				if(LocalGameVersion<SeverGameVersion)
					isOk = false;
				if(LocalGameVersion == SeverGameVersion && LocalAppVersion<SeverAppVersion)
					isOk = false;
				if(LocalGameVersion == SeverGameVersion && LocalAppVersion==SeverAppVersion && LocalResVersion < SeverResVersion)
					isOk = false;
				if(isOk == false)
				{
					libOS::getInstance()->registerListener(this);
					std::string lan = Language::Get()->getString("@LOGIN_VERSION_FAILED");
					libOS::getInstance()->showMessagebox(lan,102);
					return;
				}
			}
			if(loginRet && loginRet->status() == 5)
			{
				std::string out = Language::Get()->getString("@LoadingFrameSeversConnectionIsGray");
				CCMessageBox(out.c_str(),Language::Get()->getString("@WarmlyTip").c_str());
				return;
			}
			if(loginRet && loginRet->status() == 4)
			{
				return;
			}
			if(loginRet && !GamePrecedure::Get()->isInLoadingScene())
			{
				GamePrecedure::Get()->reRequestPackages();
			}
			
		}
		virtual void onSendPacketFailed(const int opcode){
			//Show a messagebox when server is stoped
			if(SeverConsts::Get()->isServerInUpdatingCode()&& (!GamePrecedure::Get()->hasAlertServerUpdating())&&(GamePrecedure::Get()->isInLoadingScene()))
			{
				//CCMessageBox(Language::Get()->getString("@ServerInUpdateCode").c_str(),"Tip");
				CCMessageBox(SeverConsts::Get()->getServerInUpdateMsg().c_str(), Language::Get()->getString("@WarmlyTip").c_str());
				GamePrecedure::Get()->setAlertServerUpdating(true);
			}
		}
		virtual void onConnectFailed(std::string ip, int port){}
private: 
		 virtual int _getOpcode() { return mOpcode;};
}; 
LoginHandle * _handler_loginHandle = new LoginHandle;


DEFINE_CREATE_REGISTER_HANDLER(UserBasicInfoHandle,OPCODE_GET_USER_BASIC_INFORET_S)
{
		const OPUserBasicInfoRet * userBasicInfoRet = dynamic_cast<const OPUserBasicInfoRet*>(packet);
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		std::string basicSyncKey="basicSyncKey";
		bool isSync=false;
		if(BlackBoard::Get()->hasVarible(basicSyncKey))
		{
			isSync=BlackBoard::Get()->getVaribleAsBool(basicSyncKey);
		}
		if(!isSync)
		{
			userBasicInfo.name				= userBasicInfoRet->name();
			userBasicInfo.level				= userBasicInfoRet->level();
			userBasicInfo.exp				= userBasicInfoRet->exp();	
			userBasicInfo.maxpower			= userBasicInfoRet->maxpower();
			userBasicInfo.maxvitality		= userBasicInfoRet->maxvitality();
			userBasicInfo.viplevel			= userBasicInfoRet->viplevel();
			userBasicInfo.rechargenum		= userBasicInfoRet->rechargenum();
			userBasicInfo.silvercoins		= userBasicInfoRet->silvercoins();
			userBasicInfo.goldcoins			= userBasicInfoRet->goldcoins();
			userBasicInfo.tutorialstep		= userBasicInfoRet->tutorialstep();
			
			userBasicInfo.titleTotalNum		= userBasicInfoRet->titletotalnum();
			userBasicInfo.totalTimeStamp	= GamePrecedure::Get()->getTotalTime();
			
			UserSettingInfo& settingInfo=ServerDateManager::Get()->getUserSettingInfo();
			settingInfo.music=userBasicInfoRet->music();
			settingInfo.sound=userBasicInfoRet->sound();
			SoundManager::Get()->setMusicOn(settingInfo.music == 1);
			SoundManager::Get()->setEffectOn(settingInfo.sound == 1);
			settingInfo.chat=userBasicInfoRet->chat();
			userBasicInfo.unReadCount=userBasicInfoRet->unreadmsgcount();
			userBasicInfo.readTime=userBasicInfoRet->readmsgtime();
			userBasicInfo.dropStatus=userBasicInfoRet->dropstatus();
		}
		userBasicInfo.power				= userBasicInfoRet->power();
		userBasicInfo.poweraddtime		= userBasicInfoRet->poweraddtime();
		userBasicInfo.vitality			= userBasicInfoRet->vitality();
		userBasicInfo.vitalityaddtim	= userBasicInfoRet->vitalityaddtime();
		userBasicInfo.todayAddedPowerNum = userBasicInfoRet->todayaddedpowernum();
		userBasicInfo.todayBuyPowerNum = userBasicInfoRet->todaybuypowernum();//add by xinghui
		userBasicInfo.todayAddedVitalityNum = userBasicInfoRet->todayaddedvitalitynum();
		userBasicInfo.todayBuyVitalityNum = userBasicInfoRet->todaybuyvitalitynum();//add by xinghui
		userBasicInfo.servertime		= userBasicInfoRet->servertime();	
		if(userBasicInfoRet->has_actstatus())
		{
			userBasicInfo.actStatus=userBasicInfoRet->actstatus();
		}
		GamePrecedure::Get()->setServerTime(userBasicInfo.servertime);
		if(!isSync)
		{
			if(userBasicInfo.tutorialstep==1)
			{
				if(userBasicInfoRet->hasgotcontinuelogin()==1)
				{
					if(!BlackBoard::Get()->hasVarible("hasContinue"))
						BlackBoard::Get()->addVarible("hasContinue",false);
					else
						BlackBoard::Get()->setVarible("hasContinue",false);
				}
				else
				{
					if(!BlackBoard::Get()->hasVarible("hasContinue"))
						BlackBoard::Get()->addVarible("hasContinue",true);
					else
						BlackBoard::Get()->setVarible("hasContinue",true);
				}
			}
		}
		ServerDateManager::Get()->resetBasicNextSyncTime();				
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(GerUserMarketInfoHandle,OPCODE_GET_USER_MARKET_INFORET_S)
{
	const OPUserMarketInfoRet * market = dynamic_cast<const OPUserMarketInfoRet*>(packet);
	MarketInfo& marketInfo = ServerDateManager::Get()->_getMarketInfo();
	marketInfo.servertime		= market->servertime();
	struct cc_timeval now;
	if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
		marketInfo.msgGotTime = now.tv_sec;
	else
		marketInfo.msgGotTime		= GamePrecedure::Get()->getTotalTime();

	marketInfo.recuritTenCount	= market->tenleavetime();
	marketInfo.recurittime1		= market->tentime();
	marketInfo.recurittime2		= market->hundredtime();
	marketInfo.recurittime3		= market->wanlitime();
	marketInfo.recuritCost1		= market->tencost();
	marketInfo.recuritCost2		= market->hundredcost();
	marketInfo.recuritCost3		= market->wanlicost();
	marketInfo.firstbaili		= market->firstbaili();
	marketInfo.firstwanli		= market->firstwanli();
	marketInfo.firstPay			= market->firstpay();


	TimeCalculator::Get()->createTimeCalcultor("marketinfo_recurittime1",marketInfo.recurittime1);
	TimeCalculator::Get()->createTimeCalcultor("marketinfo_recurittime2",marketInfo.recurittime2);
	TimeCalculator::Get()->createTimeCalcultor("marketinfo_recurittime3",marketInfo.recurittime3);

	marketInfo.gotVipGifts.clear();
	for(int i=0;i<market->gotgiftid_size();++i)
	{
		marketInfo.gotVipGifts.insert(market->gotgiftid(i));
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserToolInfoHandle,OPCODE_GET_USER_TOOL_INFORET_S)
{
	const OPUserToolInfoRet * userToolInfoRet = dynamic_cast<const OPUserToolInfoRet*>(packet);

	for(int index=0; index < userToolInfoRet->tools_size(); ++index)
	{
		GetPropManager::Get()->gotTool(userToolInfoRet->tools(index));
		
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPUseToolHandle,OPCODE_USE_TOOLRET_S)
{
	const int OPEN_TREASURE_REWARD = 1;
	const int OPEN_GIFT_BOX_REWARD = 2;
	enum OpType
	{
		OPTYPE_ADD_POWER=1,
		OPTYPE_ADD_VATALITY,
		OPTYPE_ADD_CHALLENGE_TIMRES,
		OPTYPE_OPEN_TREASURE,
		OPTYPE_OPEN_GIFT_BOX,
		OPTYPE_ADD_DISCIPLE_EXP,
		OPTYPE_ADD_DISCIPLE_EXP_FAILD
	};
	int toolId = BlackBoard::Get()->getUseToolId();
	int useToolCount = BlackBoard::Get()->getUseToolCount();
	UserToolInfo * useToolInfo = ServerDateManager::Get()->getAndCreatToolInfo(toolId);
	useToolInfo->count	= useToolInfo->count-useToolCount;	
	const unsigned int pairTypeItemId = ResManager::Get()->getTreasurePairType(useToolInfo->itemid);
	if(pairTypeItemId!=0)
	{
		const unsigned int pairId = ServerDateManager::Get()->getUserToolInfoByItemId(pairTypeItemId)->id;
		UserToolInfo * pairInfo = ServerDateManager::Get()->getAndCreatToolInfo(pairId);
		pairInfo->count	= pairInfo->count-useToolCount;
	}
	const OPUseToolRet * useToolRet = dynamic_cast<const OPUseToolRet*>(packet);
	int rewardSize = useToolRet->reward_size();
	if(rewardSize>0)
	{
		RewardDataMap tempRewardDataList;
		for(int index = 0;index < rewardSize; ++index){
			const OPRewardInfoRet_RewardInfo &info = useToolRet->reward(index);
			RewardData* rewardInfo = NULL;
			if(tempRewardDataList.find(index)!=tempRewardDataList.end())
			{
				rewardInfo = tempRewardDataList.find(index)->second;
				rewardInfo->count = info.count();
				rewardInfo->itemId = info.itemid();
				rewardInfo->itemType = info.itemtype();
			}
			else
			{
				rewardInfo = new RewardData();
				rewardInfo->id = index;
				rewardInfo->count = info.count();
				rewardInfo->itemId = info.itemid();
				rewardInfo->itemType = info.itemtype();
				tempRewardDataList.insert(RewardDataMap::value_type(index,rewardInfo));
			}
		}

		for(int index=0; index < rewardSize; ++index)
		{
			const OPRewardInfoRet_RewardInfo &info = useToolRet->reward(index);
			RewardData * rewardInfo = ServerDateManager::Get()->getAndCreateRewardData(index);
			rewardInfo->count = (dynamic_cast<RewardData* > (tempRewardDataList.find(index)->second))->count;
			rewardInfo->itemId = (dynamic_cast<RewardData* > (tempRewardDataList.find(index)->second))->itemId;
			rewardInfo->itemType = (dynamic_cast<RewardData* > (tempRewardDataList.find(index)->second))->itemType;
		}
		ServerDateManager::Get()->rewardDataToResInfo();
		ServerDateManager::Get()->sortResInfoList();
	}
	
	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	//TODO Add float message "Add power/vitality/chanllengeTimes success!";
	if(useToolRet->has_power())
	{
		userBasicInfo.power = useToolRet->power();
	}
	if(useToolRet->has_vitality())
	{
		userBasicInfo.vitality = useToolRet->vitality();
	}
	if(useToolRet->has_exp())
	{
		userBasicInfo.exp = useToolRet->exp();
	}
	if(useToolRet->has_exp())
	{
		userBasicInfo.exp = useToolRet->exp();
	}
	if(useToolRet->has_challengetimes())
	{
		ServerDateManager::Get()->getUserArenaInfo().arenaNum = useToolRet->challengetimes();
	}
	if(useToolRet->has_goldcoins())
	{
		userBasicInfo.goldcoins = useToolRet->goldcoins();
	}
	if(useToolRet->has_silvercoins())
	{
		userBasicInfo.silvercoins = useToolRet->silvercoins();
	}
	if(useToolRet->soulinfo_size()>0)
	{
		for(int index=0; index < useToolRet->soulinfo_size(); ++index)
		{
			GetPropManager::Get()->gotSoul(useToolRet->soulinfo(index));		
		}
	}
	if(useToolRet->skillinfo_size()>0)
	{
		for(int index=0; index < useToolRet->skillinfo_size(); ++index)
		{
			GetPropManager::Get()->gotSkill(useToolRet->skillinfo(index));		
		}
	}
	if(useToolRet->equipinfo_size())
	{
		for(int index=0; index < useToolRet->equipinfo_size(); ++index)
		{
			GetPropManager::Get()->gotEquipment(useToolRet->equipinfo(index));		
		}
	}
	if(useToolRet->toolinfo_size())
	{
		for(int index=0; index < useToolRet->toolinfo_size(); ++index)
		{
			GetPropManager::Get()->gotTool(useToolRet->toolinfo(index));		
		}
	}
	if(useToolRet->optype()==OPTYPE_ADD_POWER)
	{
		ServerDateManager::Get()->getUserBasicInfo().todayAddedPowerNum+=1;
		MSG_BOX(Language::Get()->getString("@UseToolSuccess"));
	}
	else if(useToolRet->optype()==OPTYPE_ADD_VATALITY)
	{
		ServerDateManager::Get()->getUserBasicInfo().todayAddedVitalityNum+=1;
		MSG_BOX(Language::Get()->getString("@UseToolSuccess"));
	}
	else if(useToolRet->optype()==OPTYPE_ADD_CHALLENGE_TIMRES)
	{
		std::string s = Language::Get()->getString("@AddChallengetimes");
		char buff[128] = {0};
		sprintf(buff , s.c_str() , ServerDateManager::Get()->getUserArenaInfo().arenaNum);
		MSG_BOX(buff );
	}
	else if ( useToolRet->optype() == OPTYPE_ADD_DISCIPLE_EXP )
	{
		for ( int i=0, count=useToolRet->discipleinfo_size(); i<count; i++ ) 
		{
			//ServerDateManager::Get()->updateDiscipleInfo(useToolRet->discipleinfo(i));
			const OPUserDiscipleInfoRet_DiscipleInfo& discipleInfo = useToolRet->discipleinfo(i);
			UserDiscipleInfo* userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(discipleInfo.id());
			userDiscipleInfo->level			= discipleInfo.level();
			userDiscipleInfo->exp			= discipleInfo.exp();
			userDiscipleInfo->health		= discipleInfo.health();
			userDiscipleInfo->attack		= discipleInfo.attack();
			userDiscipleInfo->defence		= discipleInfo.defence();
			userDiscipleInfo->rp			= discipleInfo.rp();
			userDiscipleInfo->upgradelevel	= discipleInfo.upgradelevel();
			userDiscipleInfo->potentiality	= discipleInfo.potentiality();
			userDiscipleInfo->skill1		= discipleInfo.skill1();
			userDiscipleInfo->battleid		= discipleInfo.battleid();
		}
		for ( int j=0, count=useToolRet->battlearrayinfo_size(); j<count; j++ )
		{
			const OPUserBattleArrayRet_BattleArrayInfo& battleArrayInfo = useToolRet->battlearrayinfo(j);
			UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(battleArrayInfo.id());
			userBattleInfo->id			= battleArrayInfo.id();
			userBattleInfo->discipleId	= battleArrayInfo.discipleid();
			userBattleInfo->attack		= battleArrayInfo.attack();
			userBattleInfo->defence		= battleArrayInfo.defence();
			userBattleInfo->health		= battleArrayInfo.health();
			userBattleInfo->rp			= battleArrayInfo.rp();
			userBattleInfo->position	= battleArrayInfo.position();
		}
		MSG_BOX(Language::Get()->getString("@DrinkWineSuccess"));
	}
	else if(useToolRet->optype() == OPTYPE_ADD_DISCIPLE_EXP_FAILD)
	{
		MSG_BOX(Language::Get()->getString("@noDiscipleCanAddExp"));
	}else
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PackPreviewPage";
		MessageManager::Get()->sendMessage(&msg);
		MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
		if(useToolRet->optype()==OPTYPE_OPEN_TREASURE)
		{
			packMsg->index=OPEN_TREASURE_REWARD;
			BlackBoard::Get()->continueOpenBoxNum = useToolCount;
		}
		else if(useToolRet->optype()==OPTYPE_OPEN_GIFT_BOX)
		{

				const ToolItem* tool = ToolTableManager::Get()->getToolItemByID(useToolInfo->itemid);
				ResInfoMap::const_iterator it = tool->includeOther.begin();
				for(;it!=tool->includeOther.end();++it)
				{
					ResInfo * resInfo = ServerDateManager::Get()->getAndCreateResInfo(it->second->id);
					resInfo->count = it->second->count;
					resInfo->itemId = it->second->itemId;
					resInfo->type = it->second->type;
					resInfo->describe = it->second->describe;
					resInfo->icon = it->second->icon;
					resInfo->name = it->second->name;
					resInfo->quality = it->second->quality;
				}

			packMsg->index=OPEN_GIFT_BOX_REWARD;
		}
		MessageManager::Get()->sendMessage(packMsg);
	}
	MsgMainFrameRefreshPage refMsg;
	refMsg.pageName = "PackagePage";
	MessageManager::Get()->sendMessage(&refMsg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserDiscipleInfoHandle,OPCODE_GET_USER_DISCIPLE_INFORET_S)
{
	const OPUserDiscipleInfoRet * userDiscipleInfoRet = dynamic_cast<const OPUserDiscipleInfoRet*>(packet);

	for(int index=0; index < userDiscipleInfoRet->disciple_size(); ++index)
	{
		const OPUserDiscipleInfoRet_DiscipleInfo& info = userDiscipleInfoRet->disciple(index);
		UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(info.id());
		userDiscipleInfo->itemid		= info.itemid();
		userDiscipleInfo->level			= info.level();
		userDiscipleInfo->exp			= info.exp();
		userDiscipleInfo->health		= info.health();
		userDiscipleInfo->attack		= info.attack();
		userDiscipleInfo->defence		= info.defence();
		userDiscipleInfo->rp			= info.rp();
		userDiscipleInfo->upgradelevel	= info.upgradelevel();
		userDiscipleInfo->potentiality	= info.potentiality();
		userDiscipleInfo->skill1		= info.skill1();
		userDiscipleInfo->battleid		= info.battleid();
		userDiscipleInfo->preLevel      = info.level();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(SkillInfoHandle,OPCODE_GET_USER_SKILL_INFORET_S)
{
	const OPUserSkillInfoRet * skillInfoRet = dynamic_cast<const OPUserSkillInfoRet*>(packet);

	for(int index=0; index < skillInfoRet->skill_size(); ++index)
	{
		GetPropManager::Get()->gotSkill(skillInfoRet->skill(index));
		
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserEquipleInfoHandle,OPCODE_GET_USER_EQUIP_INFORET_S)
{
	const OPUserEquipInfoRet * userEquipInfoRet = dynamic_cast<const OPUserEquipInfoRet*>(packet);

	for(int index=0; index < userEquipInfoRet->equip_size(); ++index)
	{
		GetPropManager::Get()->gotEquipment(userEquipInfoRet->equip(index));
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserSoulInfoHandle,OPCODE_GET_USER_SOUL_INFORET_S)
{
	const OPUserSoulInfoRet * userSoulInfoRet = dynamic_cast<const OPUserSoulInfoRet*>(packet);

	for(int index=0; index < userSoulInfoRet->soul_size(); ++index)
	{
		 GetPropManager::Get()->gotSoul(userSoulInfoRet->soul(index));		
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(TitleInfoHandle,OPCODE_GET_TITLE_INFORET_S)
{
	const OPTitleInfoRet * titleInfoRet = dynamic_cast<const OPTitleInfoRet*>(packet);

	for(int index=0; index < titleInfoRet->titles_size(); ++index)
	{
		const OPTitleInfoRet_TitleItem& titleItem = titleInfoRet->titles(index);
		TitleInfo* titleInfo = ServerDateManager::Get()->getAndCreatTitleInfo(titleItem.titleid() / 1000);
		titleInfo->itemid = titleItem.titleid();
		if(titleItem.has_gettime())
			titleInfo->startime = titleItem.gettime();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(BuyToolHandle,OPCODE_BUY_TOOLSRET_S)
{
	const OPBuyToolRet * tool = dynamic_cast<const OPBuyToolRet*>(packet);
	if(tool)
	{
		if(tool->status() == 1)
		{
			UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();

			if(tool->has_goldcoins())
			{
				userBasicInfo.goldcoins = tool->goldcoins();
			}
			if(tool->has_silvercoins())
			{
				userBasicInfo.silvercoins = tool->silvercoins();
			}

			for(int index=0; index < tool->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(tool->skillinfo(index));		
			}
			for(int index=0; index < tool->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(tool->equipinfo(index));		
			}
			for(int index=0; index < tool->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(tool->toolinfo(index));	
			}
			if(BlackBoard::Get()->MarketBuyItemId!=1503)
			{
				const ToolItem* toolItem = ToolTableManager::Get()->getToolItemByID(BlackBoard::Get()->MarketBuyItemId);
				if((toolItem!=NULL)&&toolItem->pageID==GIFTBOXPAGE)
				{
					MsgMainFramePushPage msg;
					msg.pageName = "PackPreviewPage";
					MessageManager::Get()->sendMessage(&msg);
					MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
					ResInfoMap::const_iterator it = toolItem->includeOther.begin();
					for(;it!=toolItem->includeOther.end();++it)
					{
						ResInfo * resInfo = ServerDateManager::Get()->getAndCreateResInfo(it->second->id);
						resInfo->count = it->second->count;
						resInfo->itemId = it->second->itemId;
						resInfo->type = it->second->type;
						resInfo->describe = it->second->describe;
						resInfo->icon = it->second->icon;
						resInfo->name = it->second->name;
						resInfo->quality = it->second->quality;
					}
					packMsg->index=OPEN_GIFT_BOX_REWARD;
					MessageManager::Get()->sendMessage(packMsg);
					if(toolItem->buyCountLimit==1)
					{
						MarketInfo& marketInfo = ServerDateManager::Get()->_getMarketInfo();
						marketInfo.gotVipGifts.insert(BlackBoard::Get()->MarketBuyItemId);
						BlackBoard::Get()->MarketBuyItemId = 0;
					}
				}else
				{
					//if the item is YinBei,do not show the page
					if(BlackBoard::Get()->MarketBuyItemId==PUNCHHOLE || BlackBoard::Get()->MarketBuyItemId==REMOVESTONE ){
						MSG_BOX(Language::Get()->getString("@MSGBOXBuyingDone"));

					}
					else if(BlackBoard::Get()->MarketBuyItemId!=YINBEI)
					{
						BasePage* bp  = CCBManager::Get()->getPage("PropUpPage");
						PropUpPage* page = bp?dynamic_cast<PropUpPage*>(bp):0;
						if(page)
							page->setUsage(PropUpPage::FOR_SHOP_BUYTOOL);
						MsgMainFramePushPage msg2;
						msg2.pageName = "PropUpPage";
						MessageManager::Get()->sendMessage(&msg2);
					}else
					{
						BlackBoard::Get()->MarketBuyItemId = 0;
					}
					
				}
			}
			else
			{
				MsgMainFrameRefreshPage msg1;
				msg1.pageName = "TranExpPreviewPage";
				MessageManager::Get()->sendMessage(&msg1);
				BlackBoard::Get()->MarketBuyItemId = 0;
			}
			

		}
		else if(tool->status() == 2)
		{
			std::string out = Language::Get()->getString("@BUYNotEnoughRMB");
			MSG_BOX(out);
		}
		else if(tool->status() == 3)
		{
			std::string out = Language::Get()->getString("@BUYLimitTime");
			MSG_BOX(out);
		}
		else if(tool->status() == 4)
		{
			std::string out = Language::Get()->getString("@BUYVipNotEnough");
			MSG_BOX(out);
		}
		else if(tool->status() == 5)
		{
			std::string out = Language::Get()->getString("@BUYToolNotExist");
			MSG_BOX(out);
		}

		MsgMainFramePopPage msg;
		msg.pageName = "BuyPropsCountPage";
		MessageManager::Get()->sendMessage(&msg);

	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserBattleArrayHandle,OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S)
{
	const OPUserBattleArrayRet * userBattleArrayRet = dynamic_cast<const OPUserBattleArrayRet*>(packet);

	for(int index=0; index < userBattleArrayRet->battle_size(); ++index)
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = userBattleArrayRet->battle(index);
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(info.id());
		userBattleInfo->id			=info.id();
		userBattleInfo->discipleId	=info.discipleid();
		userBattleInfo->attack		=info.attack();
		userBattleInfo->defence		=info.defence();
		userBattleInfo->health		=info.health();
		userBattleInfo->rp			=info.rp();
		userBattleInfo->skill1		=info.skill1();
		userBattleInfo->skill2		=info.skill2();
		userBattleInfo->equip1		=info.equip1();
		userBattleInfo->equip2		=info.equip2();
		userBattleInfo->equip3		=info.equip3();
		userBattleInfo->position	=info.position();

		userBattleInfo->fateInfos.clear();
		for(int i=0; i < info.fates_size(); ++i)
		{
			const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
			userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserSysMsgInfoHandle,OPCODE_GET_USER_SYSMSG_INFORET_S)
{

	const OPUserSysMsgInfoRet * userSysMsgInfoRet = dynamic_cast<const OPUserSysMsgInfoRet*>(packet);
	if(userSysMsgInfoRet->has_readtime())
	{
		ServerDateManager::Get()->getUserBasicInfo().readTime=userSysMsgInfoRet->readtime();
	}
	for(int index=0; index < userSysMsgInfoRet->sysmsg_size(); ++index)
	{
		const OPUserSysMsgInfoRet_SysMsgInfo& info = userSysMsgInfoRet->sysmsg(index);
		UserSysMsgInfo * userSysMsgInfo = ServerDateManager::Get()->getAndCreatSysMsgEntity(info.id());
		userSysMsgInfo->id			= info.id();
		userSysMsgInfo->relationId  = info.relationid();
		userSysMsgInfo->sysMsgType  = info.msgtype();
		if(info.has_uid())
		{
			userSysMsgInfo->uId			= info.uid();
		}
		if(info.has_uname())
		{
			userSysMsgInfo->name		=info.uname();
		}
		if(info.has_msginfo())
		{
			userSysMsgInfo->info		=info.msginfo();
		}
		if(info.has_msgcontent())
		{
			userSysMsgInfo->content		=info.msgcontent();
		}
		if(info.has_fightret())
		{
			userSysMsgInfo->fightRet=info.fightret();
		}
		userSysMsgInfo->oprType		=info.oprtype();
		userSysMsgInfo->eventTime	=info.eventtime();
		if(info.has_addenemytype())
		{
			userSysMsgInfo->addEnemyType=info.addenemytype();
		}
		else
		{
			userSysMsgInfo->addEnemyType=1;
		}
		unsigned int readTime=ServerDateManager::Get()->getUserBasicInfo().readTime;
		if(userSysMsgInfo->eventTime>readTime)
		{
			++ServerDateManager::Get()->getUserBasicInfo().unReadCount;
		}
		ServerDateManager::Get()->clearSysMsgRewardMap(info.id());
		for(int i=0;i<info.reward_size();++i)
		{
			const OPRewardInfoRet_RewardInfo rewardItem=info.reward(i);
			RewardData* item=ServerDateManager::Get()->createSysMsgRewardById(info.id(),i);
			item->itemType=rewardItem.itemtype();
			item->itemId=rewardItem.itemid();
			item->count=rewardItem.count();
		}
		
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserFriendsInfoHandle,OPCODE_GET_USER_FRIENDS_INFORET_S)
{
	const OPUserFriendsInfoRet * userFriendsInfoRet = dynamic_cast<const OPUserFriendsInfoRet*>(packet);

	if(userFriendsInfoRet)
	{
		if(userFriendsInfoRet->type()==1)
		{
			for(int index=0; index < userFriendsInfoRet->friendsinfo_size(); ++index)
			{
				const OPUserFriendsInfoRet_FriendsInfo& info = userFriendsInfoRet->friendsinfo(index);
				UserFriendsInfo * userFriendsInfo = ServerDateManager::Get()->getAndCreatFriendInfo(info.id());
				userFriendsInfo->id			= info.id();
				userFriendsInfo->name		=info.name();
				userFriendsInfo->level		=info.level();
				userFriendsInfo->type		=info.type();
			}
		}
		else
		{
			ServerDateManager::Get()->clearInviteFriendList();
			for(int index=0; index < userFriendsInfoRet->friendsinfo_size(); ++index)
			{
				const OPUserFriendsInfoRet_FriendsInfo& info = userFriendsInfoRet->friendsinfo(index);
				UserFriendsInfo * userFriendsInfo = ServerDateManager::Get()->getAndCreatInviteFriendInfo(info.id());
				userFriendsInfo->id			= info.id();
				userFriendsInfo->name		= info.name();
				userFriendsInfo->level		= info.level();
				userFriendsInfo->type		= 5;
			}
			if(userFriendsInfoRet->friendsinfo_size()==0)
			{
				MSG_BOX(Language::Get()->getString("@SelFriendNoSize"));
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserChatInfoHandle,OPCODE_CHAT_PUSHRET_S)
{
	const  OPChatPushRet* userChatInfoRet = dynamic_cast<const OPChatPushRet*>(packet);
	int _lastSize=ServerDateManager::Get()->getChatInfoTotalNum();
	for(int index=0; index < userChatInfoRet->msgs_size(); ++index)
	{
		const OPChatPushRet_MsgInfo& info = userChatInfoRet->msgs(index);
		UserChatInfo * userChatInfo = ServerDateManager::Get()->getAndCreatChatInfo(info.id());
		userChatInfo->id			= info.id();
		userChatInfo->uId			= info.playerid();
		userChatInfo->vipLevel		= info.viplevel();
		userChatInfo->name			= info.name();
		userChatInfo->msg			= info.msg();
		userChatInfo->eventTime		= info.sendtime();
		userChatInfo->msgType		= info.type();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserSendChatInfoHandle,OPCODE_CHAT_SEND_MSGRET_S)
{
	const  OPChatSendMsgInfoRet* userChatInfoRet = dynamic_cast<const OPChatSendMsgInfoRet*>(packet);
	if(userChatInfoRet)
	{
		if(userChatInfoRet->has_tool())
		{
			GetPropManager::Get()->gotTool(userChatInfoRet->tool());
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPEnterChatPageRetHandle,OPCODE_ENTER_CHAT_PAGERET_S)
{
	const  OPEnterChatPageRet* enterRet = dynamic_cast<const OPEnterChatPageRet*>(packet);
	if(enterRet)
	{
		if(enterRet->status()==0)
		{

		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPQuitChatPageRetHandle,OPCODE_QUIT_CHAT_PAGERET_S)
{
	const  OPQuitChatPageRet* quitRet = dynamic_cast<const OPQuitChatPageRet*>(packet);
	if(quitRet)
	{
		if(quitRet->status()==0)
		{

		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPSendMessageHandle,OPCODE_SEND_FRIEND_MESSAGERET_S)
{
	const  OPSendMessageRet* sendMsgRet = dynamic_cast<const OPSendMessageRet*>(packet);
	if(sendMsgRet && sendMsgRet->status() == 1)
	{
		
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPDisposeSysMsgHandle,OPCODE_DISPOSE_USER_SYSMSG_INFORET_S)
{
	const  OPDisposeSysMsgRet* ret = dynamic_cast<const OPDisposeSysMsgRet*>(packet);
	if(ret && ret->status() == 1)
	{
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(ret->has_power())
		{
			userBasicInfo.power = ret->power(); 
		}
		if(ret->has_vitality())
		{
			userBasicInfo.vitality = ret->vitality();
		}
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_challengetimes())
		{
			ServerDateManager::Get()->getUserArenaInfo().arenaNum = ret->challengetimes();
			std::string s = Language::Get()->getString("@AddChallengetimes");
			char buff[128] = {0};
			sprintf(buff , s.c_str() , ret->challengetimes());
			MSG_BOX(buff );
		}
		if(ret->has_goldcoins())
		{
			userBasicInfo.goldcoins = ret->goldcoins();
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
		if(ret->soulinfo_size()>0)
		{
			for(int index=0; index < ret->soulinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSoul(ret->soulinfo(index));		
			}
		}
		if(ret->skillinfo_size()>0)
		{
			for(int index=0; index < ret->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
			}
		}
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->discipleinfo_size()>0)
		{
			for(int index=0; index < ret->discipleinfo_size(); ++index)
			{
				const OPUserDiscipleInfoRet_DiscipleInfo& info = ret->discipleinfo(index);
				UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(info.id());
				userDiscipleInfo->itemid		= info.itemid();
				userDiscipleInfo->level			= info.level();
				userDiscipleInfo->exp			= info.exp();
				userDiscipleInfo->health		= info.health();
				userDiscipleInfo->attack		= info.attack();
				userDiscipleInfo->defence		= info.defence();
				userDiscipleInfo->rp			= info.rp();
				userDiscipleInfo->upgradelevel	= info.upgradelevel();
				userDiscipleInfo->potentiality	= info.potentiality();
				userDiscipleInfo->skill1		= info.skill1();
				userDiscipleInfo->battleid		= info.battleid();
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPDisposeFriendsInfoHandle,OPCODE_DISPOSE_USER_FRIENDS_INFORET_S)
{
	const  OPDisposeFriendsInfoRet* disposeRet = dynamic_cast<const OPDisposeFriendsInfoRet*>(packet);
	if(disposeRet)
	{
		std::string disposeRetFriendId="disposeRetFriendIdKey";
		unsigned int _id=disposeRet->status()<0?0:disposeRet->status();
		if(HAS_V(disposeRetFriendId))
		{
			SET_V(disposeRetFriendId,_id);
		}
		else
		{
			ADD_V(disposeRetFriendId,_id);
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(ShopListHandle,OPCODE_GET_SHOP_ITEM_LISTRET_S)
{
	const  OPShopListRet* shoplist = dynamic_cast<const OPShopListRet*>(packet);
	for(int index=0; index < shoplist->itemlist_size(); ++index)
	{
		const OPShopListRet_ShopItem & info = shoplist->itemlist(index);
		ShopItem * shopitem = ServerDateManager::Get()->getAndCreatShopItem(index);
		shopitem->productID = info.productid();
		shopitem->productName = info.productname();
		shopitem->productCount = info.amount();
		shopitem->productPrice = info.productprice();
		shopitem->productOrignalPrice = info.addamount();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(TrainDiscipleHandle,OPCODE_TRAIN_DISCIPLERET_S)
{
	const  OPTrainDiscipleRet* trainDiscipleRet = dynamic_cast<const OPTrainDiscipleRet*>(packet);
	TrainDiscipleInfo& trainDiscipleInfo = ServerDateManager::Get()->getTrainDiscipleInfo();
	trainDiscipleInfo.id				= trainDiscipleRet->id();
	trainDiscipleInfo.minus.pro			= trainDiscipleRet->minus().pro();
	trainDiscipleInfo.minus.num			= trainDiscipleRet->minus().num();
	trainDiscipleInfo.add.pro			= trainDiscipleRet->add().pro();
	trainDiscipleInfo.add.num			= trainDiscipleRet->add().num();

	ServerDateManager::Get()->getUserBasicInfo().goldcoins = trainDiscipleRet->goldcoins();

	MsgMainFrameChangePage msg;
	msg.pageName = "TrainResultPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(TrainStatusHandle,OPCODE_DEAL_TRAIN_STATUSRET_S)
{
	const  OPDealTrainStatusRet* dealTrainStatus = dynamic_cast<const OPDealTrainStatusRet*>(packet);
	UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByID(dealTrainStatus->id());
	if(userDiscipleInfo)
	{
		userDiscipleInfo->potentiality = dealTrainStatus->potentiality();
		
		int size = dealTrainStatus->pro_size();
		for(int index = 0; index<size; ++index)
		{
			const OPDealTrainStatusRet_PropertyInfo& pro = dealTrainStatus->pro(index);
			if(pro.type() == 1)
			{
				userDiscipleInfo->attack = pro.num();
			}
			else if(pro.type() == 2)
			{
				userDiscipleInfo->defence = pro.num();
			}
			else if(pro.type() == 3)
			{
				userDiscipleInfo->health = pro.num();
			}
			else if(pro.type() == 4)
			{
				userDiscipleInfo->rp = pro.num();
			}
		}
		UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(1502001);
		info->count = info->count-BlackBoard::Get()->ConsumeTrainningBolus;

		if(dealTrainStatus->has_battle())
		{
			const OPUserBattleArrayRet_BattleArrayInfo& info = dealTrainStatus->battle();
			UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getUserBattleInfo(info.position());
			userBattleInfo->id			=info.id();
			userBattleInfo->discipleId	=info.discipleid();
			userBattleInfo->attack		=info.attack();
			userBattleInfo->defence		=info.defence();
			userBattleInfo->health		=info.health();
			userBattleInfo->rp			=info.rp();
			userBattleInfo->skill1		=info.skill1();
			userBattleInfo->skill2		=info.skill2();
			userBattleInfo->equip1		=info.equip1();
			userBattleInfo->equip2		=info.equip2();
			userBattleInfo->equip3		=info.equip3();
			userBattleInfo->position	=info.position();

			userBattleInfo->fateInfos.clear();
			for(int i=0; i < info.fates_size(); ++i)
			{
				const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
				userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
			}
		}
		
	}

	MsgMainFrameChangePage msg;
	msg.pageName = "TrainPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(SwallowDiscipleHandle,OPCODE_SWALLOW_DISCIPLERET_S)
{
	const  OPSwallowDiscipleRet* swallowDiscipleRet = dynamic_cast<const OPSwallowDiscipleRet*>(packet);
	const OPSwallowDiscipleRet_DiscipleInfo& swallowDiscipleRet_DiscipleInfo = swallowDiscipleRet->disciple();
	const OPSwallowDiscipleRet_ToolInfo& swallowDiscipleRet_ToolInfo = swallowDiscipleRet->tool();
	UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByID(swallowDiscipleRet_DiscipleInfo.id());
	if(userDiscipleInfo)
	{
		userDiscipleInfo->level			= swallowDiscipleRet_DiscipleInfo.level();
		userDiscipleInfo->exp			= swallowDiscipleRet_DiscipleInfo.exp();
		userDiscipleInfo->health		= swallowDiscipleRet_DiscipleInfo.health();
		userDiscipleInfo->attack		= swallowDiscipleRet_DiscipleInfo.attack();
		userDiscipleInfo->defence		= swallowDiscipleRet_DiscipleInfo.defence();
		userDiscipleInfo->rp			= swallowDiscipleRet_DiscipleInfo.rp();
		userDiscipleInfo->potentiality	= swallowDiscipleRet_DiscipleInfo.potentiality();
	}
	UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByItemId(swallowDiscipleRet_ToolInfo.id());
	if(userToolInfo)
	{
		userToolInfo->count = swallowDiscipleRet_ToolInfo.count();
	}

	if(swallowDiscipleRet->has_battle())
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = swallowDiscipleRet->battle();
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getUserBattleInfo(info.position());
		userBattleInfo->id			=info.id();
		userBattleInfo->discipleId	=info.discipleid();
		userBattleInfo->attack		=info.attack();
		userBattleInfo->defence		=info.defence();
		userBattleInfo->health		=info.health();
		userBattleInfo->rp			=info.rp();
		userBattleInfo->skill1		=info.skill1();
		userBattleInfo->skill2		=info.skill2();
		userBattleInfo->equip1		=info.equip1();
		userBattleInfo->equip2		=info.equip2();
		userBattleInfo->equip3		=info.equip3();
		userBattleInfo->position	=info.position();

		userBattleInfo->fateInfos.clear();
		for(int i=0; i < info.fates_size(); ++i)
		{
			const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
			userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
		}
	}

	ServerDateManager::Get()->removeDiscipleInfoById(swallowDiscipleRet->swallowedid());

	MsgMainFrameChangePage msg;
	msg.pageName = "TranExpResultPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(BoardCastInfoHandle,OPCODE_GET_BOARDCAST_INFORET_S)
{

	const OPBoardCastInfoRet * boardCastInfoRet = dynamic_cast<const OPBoardCastInfoRet*>(packet);
	for(int index=0; index < boardCastInfoRet->boardcastinfo_size(); ++index)
	{
		const OPBoardCastInfoRet_BoardCast& info = boardCastInfoRet->boardcastinfo(index);
		BoardCastInfo * userSysMsgInfo = ServerDateManager::Get()->getAndCreatBoardCastInfoEntity(index);
		userSysMsgInfo->id			= index;
		userSysMsgInfo->name		=info.name();
		userSysMsgInfo->itemType	=info.type();
		for(int i=0;i<info.params_size();++i)
		{
			userSysMsgInfo->params.push_back(info.params(i));
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END	

DEFINE_CREATE_REGISTER_HANDLER(ChangeSettingHandle,OPCODE_CHANGE_SETTINGRET_S)
{
	const  OPChangeSettingRet* ret = dynamic_cast<const OPChangeSettingRet*>(packet);
	if(ret && ret->status() == 1)
	{

	}
}
DEFINE_CREATE_REGISTER_HANDLER_END	

DEFINE_CREATE_REGISTER_HANDLER(ContinueLoginInfoHandle,OPCODE_GET_CONTINUE_LOGINRET_S)
{
	const OPContinueLoginRet * continueLoginRet = dynamic_cast<const OPContinueLoginRet*>(packet);
	if(continueLoginRet)
	{
		ContinueLoginInfo& continueLoginInfo = ServerDateManager::Get()->getContinueLoginInfo();
		continueLoginInfo.count=continueLoginRet->count();
		for(int index=0; index < continueLoginRet->rewardlist_size(); ++index)
		{
			const OPContinueLoginRet_RewardIdList& info = continueLoginRet->rewardlist(index);
			ContinueLoginInfo::ContinueReward* contiReward=ServerDateManager::Get()->_getContinueReward(index);
			contiReward->itemId=info.id();
			contiReward->itemType=info.type();
			if(info.has_quantity())
			{
				contiReward->quantity=info.quantity();
			}
			else
			{
				contiReward->quantity=1;
			}
		}
 		MsgMainFramePushPage msg;
 		msg.pageName = "ContinueLoginPage";
 		MessageManager::Get()->sendMessage(&msg);
	}
	
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(RewardContinueLoginInfoHandle,OPCODE_REWARD_CONTINUE_LOGINRET_S)
{
	const  OPRewardContinueLoginRet* ret = dynamic_cast<const OPRewardContinueLoginRet*>(packet);
	if(ret && ret->status() == 1)
	{
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(ret->has_goldcoins())
		{
			userBasicInfo.goldcoins = ret->goldcoins();
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
		if(ret->soulinfo_size()>0)
		{
			for(int index=0; index < ret->soulinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSoul(ret->soulinfo(index));		
			}
		}
		if(ret->skillinfo_size()>0)
		{
			for(int index=0; index < ret->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
			}
		}
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

NEW_DEFINE_CREATE_REGISTER_HANDLER(OPUpgradeSkillHandle,OPCODE_UPGRADE_SKILLRET_S)
{
	const  OPUpgradeSkillRet* upgradeSkillRet = dynamic_cast<const OPUpgradeSkillRet*>(packet);
	
	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	userBasicInfo.silvercoins = upgradeSkillRet->silvercoins();

	const  OPUpgradeSkillRet_SkillInfo& upgradeSkillRet_SkillInfo = upgradeSkillRet->skill();
	SkillInfo* skillInfo = ServerDateManager::Get()->getSkillInfoByID(upgradeSkillRet_SkillInfo.id());
	skillInfo->level			=upgradeSkillRet_SkillInfo.level();
	skillInfo->quantity			=upgradeSkillRet_SkillInfo.quantity();
	skillInfo->consume			=upgradeSkillRet_SkillInfo.consume();

	if(upgradeSkillRet->has_battle())
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = upgradeSkillRet->battle();
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getUserBattleInfo(info.position());
		userBattleInfo->id			=info.id();
		userBattleInfo->discipleId	=info.discipleid();
		userBattleInfo->attack		=info.attack();
		userBattleInfo->defence		=info.defence();
		userBattleInfo->health		=info.health();
		userBattleInfo->rp			=info.rp();
		userBattleInfo->skill1		=info.skill1();
		userBattleInfo->skill2		=info.skill2();
		userBattleInfo->equip1		=info.equip1();
		userBattleInfo->equip2		=info.equip2();
		userBattleInfo->equip3		=info.equip3();
		userBattleInfo->position	=info.position();

		userBattleInfo->fateInfos.clear();
		for(int i=0; i < info.fates_size(); ++i)
		{
			const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
			userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
		}
	}

	for(int index = 0; index<upgradeSkillRet->kid_size();++index)
	{
		ServerDateManager::Get()->removeSkillInfoById(upgradeSkillRet->kid(index));
	}

	MsgMainFrameChangePage msg;
	msg.pageName = "SkillUpgradeResultPage";
	MessageManager::Get()->sendMessage(&msg);
}
TIMEOUT_HNDLER
{
	if(!BlackBoard::Get()->IsTimeoutResponse)
	{
		BlackBoard::Get()->IsTimeoutResponse = true;
	}
}
NEW_DEFINE_CREATE_REGISTER_HANDLER_END(OPUpgradeSkillHandle)

DEFINE_CREATE_REGISTER_HANDLER(AdventureItemInfoHandle,OPCODE_GET_ADVENTURE_INFORET_S)
{
	const  OPAdventureInfoRet* ret = dynamic_cast<const OPAdventureInfoRet*>(packet);
	ServerDateManager::Get()->resetAdventureMaxId();
	for(int index=0; index < ret->adventureitem_size(); ++index)
	{
		const OPAdventureInfoRet_AdventureItem& info = ret->adventureitem(index);
		AdventureItemInfo * adventureItem = ServerDateManager::Get()->getAndCreatAdventureItemInfo(info.id());
		adventureItem->id			= info.id();
		adventureItem->adventureId	=info.adventureid();
		if(info.has_itemid())
		{
			adventureItem->itemId	=info.itemid();
		}
		else
		{
			adventureItem->itemId	=0;
		}
		adventureItem->count		=info.itemcount();
		adventureItem->beginTime	=info.begintime();
		adventureItem->endTime		=info.endtime();
		if(info.has_version())
		{
			adventureItem->version=info.version();
		}
		else
		{
			adventureItem->version=0;
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPSkillBookInfoHandle,OPCODE_GET_SKILL_BOOK_INFORET_S)
{
	const  OPGetSkillBookInfoRet* ret = dynamic_cast<const OPGetSkillBookInfoRet*>(packet);
	for(int index=0; index < ret->skillbookpartitem_size(); ++index)
	{
		const OPGetSkillBookInfoRet_SkillBookPartItem& info = ret->skillbookpartitem(index);
		SkillBookInfo::SkillBookItem* item=ServerDateManager::Get()->getSkillBookItemBySkillIdAndPartId(ret->skillid(),info.partid());
		item->partId=info.partid();
		item->partCount=info.count();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPPieceSkillBookInfoHandle,OPCODE_PIECE_SKILL_BOOKRET_S)
{
	const  OPPieceSkillBookRet* ret = dynamic_cast<const OPPieceSkillBookRet*>(packet);
	for(int index=0; index < ret->skillbookpartitem_size(); ++index)
	{
		const OPGetSkillBookInfoRet_SkillBookPartItem& info = ret->skillbookpartitem(index);
		GetPropManager::Get()->gotSkillBook(info);
	}
	for(int index=0;index<ret->skillinfo_size();++index)
	{
		GetPropManager::Get()->gotSkill(ret->skillinfo(index));
		unsigned int _id=ret->skillinfo(index).id();
		ADD_OR_SET_V("SkillSurprise",_id);
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetSkillBookLootHandle,OPCODE_GET_SKILL_BOOK_LOOT_INFORET_S)
{
	ServerDateManager::Get()->clearSkillBookLoot();
	const  OPSkillBookLootRet* ret = dynamic_cast<const OPSkillBookLootRet*>(packet);
	if(ret)
	{
		for(int index=0; index < ret->skillbooklootplayer_size(); ++index)
		{
			const OPSkillBookLootRet_SkillBookLootPlayer& info = ret->skillbooklootplayer(index);
			SkillBookLootInfo* bootLoot=ServerDateManager::Get()->getAndCreatSkillBookLootInfoById(info.id());
			bootLoot->id=info.id();
			bootLoot->playerId=info.playerid();
			bootLoot->playerName=info.playername();
			bootLoot->playerLevel=info.playerlevel();
			bootLoot->isNpc=info.isnpc();
			for(int i=0;i<info.teamitem_size();++i)
			{
				const OPSkillBookLootRet_SkillBookLootPlayer_PlayerTeamItem& team=info.teamitem(i);
				SkillBookLootInfo::PlayerTeamItem* teamItem=bootLoot->addPlayerTeamItemById(team.id());
				teamItem->memberId=team.memberid();
			}
		}
	}
	
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPSnatchSkillBookHandle,OPCODE_SNATCH_SKILL_BOOK_INFORET_S)
{
	const  OPSnatchSkillBookRet* ret = dynamic_cast<const OPSnatchSkillBookRet*>(packet);
	if(ret)
	{
		UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
		info.vitality=ret->vitality();
		std::string key="snatchSkillBookRetKey";
		if(ret->ret()==1)
		{
			ServerDateManager::Get()->addSkillBookPartBySkillIdAndPartId(BlackBoard::Get()->getSkillBookSkillId(),BlackBoard::Get()->getSkillBookId(),1);
		}
		unsigned int retStatus=ret->ret();
		if(HAS_V(key))
		{
			SET_V(key,retStatus);
		}
		else
		{
			ADD_V(key,retStatus);
		}
		if(ret->has_battle())
		{
			const OPUserBattleRet* msg = dynamic_cast<const OPUserBattleRet*>(&ret->battle());
			if(msg)
			{
				BasePage* bp = CCBManager::Get()->getPage("FightPage");
				if(bp)
				{
					FightPage* fp = dynamic_cast<FightPage*>(bp);
					fp->setMessage(msg);
					fp->setFightType(SkillBook);
				}
				MsgMainFrameCoverShow msg2;
				msg2.pageName = "FightPage";
				MessageManager::Get()->sendMessage(&msg2);
				bool fightLose=false;
				switch(msg->result().winlose())
				{
				case OPUserBattleRet_Result_WINLOSE_BIGWIN:
					fightLose=true;;
					break;
				case OPUserBattleRet_Result_WINLOSE_WIN:
					fightLose=true;
					break;
				case OPUserBattleRet_Result_WINLOSE_JUSTWIN:
					fightLose=true;
					break;
				}
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPUpgradeDiscipleHandle,OPCODE_UPGRADE_DISCIPLERET_S)
{
	const  OPUpgradeDiscipleRet* upgradeDiscipleRet = dynamic_cast<const OPUpgradeDiscipleRet*>(packet);

	UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByID(upgradeDiscipleRet->id());
	if(userDiscipleInfo)
	{
		userDiscipleInfo->upgradelevel = upgradeDiscipleRet->upgradelevel();
		userDiscipleInfo->potentiality = upgradeDiscipleRet->potentiality();
	}

	UserSoulInfo * userSoulInfo = ServerDateManager::Get()->getUserSoulInfoByDiscipleID(upgradeDiscipleRet->id());
	if(userSoulInfo)
		userSoulInfo->count = upgradeDiscipleRet->soulcount();

	MsgMainFrameRefreshPage msg;
	msg.pageName = "DiscipleUpgradePage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPRecruitDiscipleHandle,OPCODE_RECRUIT_DISCIPLERET_S)
{
	const OPRecruitDiscipleRet* recruitDiscipleRet = dynamic_cast<const OPRecruitDiscipleRet*>(packet);
	const OPUserDiscipleInfoRet_DiscipleInfo& info = recruitDiscipleRet->disciple();
	UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(info.id());
	if(userDiscipleInfo)
	{
		userDiscipleInfo->itemid		= info.itemid();
		userDiscipleInfo->level			= info.level();
		userDiscipleInfo->exp			= info.exp();
		userDiscipleInfo->health		= info.health();
		userDiscipleInfo->attack		= info.attack();
		userDiscipleInfo->defence		= info.defence();
		userDiscipleInfo->rp			= info.rp();
		userDiscipleInfo->upgradelevel	= info.upgradelevel();
		userDiscipleInfo->potentiality	= info.potentiality();
		userDiscipleInfo->skill1		= info.skill1();
	}
	if(recruitDiscipleRet->has_skill())
	{
		GetPropManager::Get()->gotSkill(recruitDiscipleRet->skill());
	}
	if(recruitDiscipleRet->has_soul())
	{
		GetPropManager::Get()->gotSoul(recruitDiscipleRet->soul());
	}
	BasePage* obj = CCBManager::Get()->getPage("DiscipleRecruitPage");
	DiscipleRecruitPage* page = dynamic_cast<DiscipleRecruitPage*>(obj);
	if(page)
	{
		page->setID(info.id());
		page->setIsSoul(false);
		page->setAdditionalSoul(false);

		MsgMainFrameCoverShow msg;
		msg.pageName = "DiscipleRecruitPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPMarketRecruitDiscipleHandle,OPCODE_MARKET_RECRUIT_DISCIPLERET_S)
{
	BasePage* obj = CCBManager::Get()->getPage("DiscipleRecruitPage");
	DiscipleRecruitPage* page = obj?dynamic_cast<DiscipleRecruitPage*>(obj):0;

	const OPMarketRecruitDiscipleRet* recruitDiscipleRet = dynamic_cast<const OPMarketRecruitDiscipleRet*>(packet);
	if(recruitDiscipleRet->has_disciple())
	{
		const OPUserDiscipleInfoRet_DiscipleInfo& info = recruitDiscipleRet->disciple();
		UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(info.id());
		if(userDiscipleInfo)
		{
			userDiscipleInfo->itemid		= info.itemid();
			userDiscipleInfo->level			= info.level();
			userDiscipleInfo->exp			= info.exp();
			userDiscipleInfo->health		= info.health();
			userDiscipleInfo->attack		= info.attack();
			userDiscipleInfo->defence		= info.defence();
			userDiscipleInfo->rp			= info.rp();
			userDiscipleInfo->upgradelevel	= info.upgradelevel();
			userDiscipleInfo->potentiality	= info.potentiality();
			userDiscipleInfo->skill1		= info.skill1();
		}

		if(page)
		{
			page->setIsSoul(false);
			page->setID(info.id());
		}
	}
	if(recruitDiscipleRet->has_soul())
	{
		const OPUserSoulInfoRet_SoulInfo& info = recruitDiscipleRet->soul();
		UserSoulInfo * userSoulInfo = ServerDateManager::Get()->getAndCreatSoulInfo(info.id());
		userSoulInfo->itemid	= info.itemid();
		userSoulInfo->count	= info.count();
		GetPropManager::Get()->gotSoul(info);
		if(page)
		{
			page->setIsSoul(true,recruitDiscipleRet->has_soulcount()?recruitDiscipleRet->soulcount():1);
			page->setID(info.itemid());
		}
	}
	if(recruitDiscipleRet->has_additional_soul() && recruitDiscipleRet->additionalsoullist_size() == 6)
	{
		const OPUserSoulInfoRet_SoulInfo& info = recruitDiscipleRet->additional_soul();
		UserSoulInfo * userSoulInfo = ServerDateManager::Get()->getAndCreatSoulInfo(info.id());
		userSoulInfo->itemid	= info.itemid();
		userSoulInfo->count	= info.count();
		GetPropManager::Get()->gotSoul(info);

		unsigned int soulId[6];
		for(int i = 0; i<6; i++)
		{
			soulId[i] = recruitDiscipleRet->additionalsoullist(i);
		}

		if(page)
			page->setAdditionalSoul(true,
			soulId,
			info.itemid(),
			recruitDiscipleRet->has_additionalsoulcount()?recruitDiscipleRet->additionalsoulcount():1,
			recruitDiscipleRet->has_additionalsoulcount_give()?recruitDiscipleRet->has_additionalsoulcount_give():0);
		
	}
	else
	{
		page->setAdditionalSoul(false);
	}
	if(recruitDiscipleRet->has_usermarketinforet())
	{
		const OPUserMarketInfoRet * market = dynamic_cast<const OPUserMarketInfoRet*>(&recruitDiscipleRet->usermarketinforet());
		MarketInfo& marketInfo = ServerDateManager::Get()->_getMarketInfo();
		marketInfo.servertime		= market->servertime();
		marketInfo.msgGotTime		= GamePrecedure::Get()->getTotalTime();
		marketInfo.recuritTenCount	= market->tenleavetime();
		marketInfo.recurittime1		= market->tentime();
		marketInfo.recurittime2		= market->hundredtime();
		marketInfo.recurittime3		= market->wanlitime();
		marketInfo.recuritCost1		= market->tencost();
		marketInfo.recuritCost2		= market->hundredcost();
		marketInfo.recuritCost3		= market->wanlicost();
		marketInfo.firstbaili		= market->firstbaili();
		marketInfo.firstwanli		= market->firstwanli();
		marketInfo.gotVipGifts.clear();
		for(int i=0;i<market->gotgiftid_size();++i)
		{
			marketInfo.gotVipGifts.insert(market->gotgiftid(i));
		}
		if(marketInfo.recuritTenCount>0)
		{
			TimeCalculator::Get()->createTimeCalcultor("marketinfo_recurittime1",marketInfo.recurittime1);
		}
		TimeCalculator::Get()->createTimeCalcultor("marketinfo_recurittime2",marketInfo.recurittime2);
		TimeCalculator::Get()->createTimeCalcultor("marketinfo_recurittime3",marketInfo.recurittime3);
	}
	
	
	if(recruitDiscipleRet->has_skill())
	{
		GetPropManager::Get()->gotSkill(recruitDiscipleRet->skill());
	}
	if(recruitDiscipleRet->has_goldcoins())
	{
		UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
		info.goldcoins=recruitDiscipleRet->goldcoins();
	}
	if(page)
	{
        //lib91::getInstance()->setWaiting(false);
		MsgMainFrameCoverShow msg;
		msg.pageName = "DiscipleRecruitPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPSellEquipHandle,OPCODE_SELL_EQUIPRET_S)
{
	const OPSellEquipRet * userBasicInfoRet = dynamic_cast<const OPSellEquipRet*>(packet);
	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	userBasicInfo.silvercoins				= userBasicInfoRet->silvercoins();

	for(int index = 0; index<userBasicInfoRet->ids_size();++index)
	{
		ServerDateManager::Get()->removeEquipInfoById(userBasicInfoRet->ids(index));
	}

	MsgMainFrameRefreshPage msg;
	msg.pageName = "EquipSellPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPAddPowerHandler,OPCODE_ADD_POWERRET_S)
{
	const  OPAddPowerRet* ret = dynamic_cast<const OPAddPowerRet*>(packet);
	if(ret)
	{

		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		userBasicInfo.power=ret->power();
		if(ret->status()==301)
		{
			userBasicInfo.todayAddedPowerNum +=1;
			//add by xinghui
			userBasicInfo.todayBuyPowerNum += 1;
			userBasicInfo.goldcoins = userBasicInfo.goldcoins-(ToolTableManager::Get()->getToolItemByID(HAIXIANFAN)->price*userBasicInfo.todayBuyPowerNum);//add by xinghui
			char buff[128];
			std::string str = Language::Get()->getString("@AdventureAddPowerSuccess");
			sprintf(buff,str.c_str(),ret->power());
			MSG_BOX(buff);
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(GetAdventurePowerInfoHandle,OPCODE_GET_ADVENTURE_POWERINFORET_S)
{
	const  OPGetAdventurePowerRet* ret = dynamic_cast<const OPGetAdventurePowerRet*>(packet);
	if(ret&&ret->status()==1)
	{
		UserAdventureDataInfo& userAddPowerInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
		userAddPowerInfo.addPowerInfo.amStatus=ret->amstatus();
		userAddPowerInfo.addPowerInfo.pmStatus=ret->pmstatus();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetBoundlessHoleHandle,OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFORET_S)
{
	const  OPGetBoundlessHoleRet* ret = dynamic_cast<const OPGetBoundlessHoleRet*>(packet);
	if(ret&&ret->status()==1)
	{
		UserAdventureDataInfo& userAdventureDataInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
		userAdventureDataInfo.boundlessHoldeInfo.accumulateCount=ret->accumulatecount();
		userAdventureDataInfo.boundlessHoldeInfo.currCount=ret->currcount();
		userAdventureDataInfo.boundlessHoldeInfo.todayStatus=ret->todaystatus();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPAddBoundlessHoleHandle,OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFORET_S)
{
	const  OPAddBoundlessHoleRet* ret = dynamic_cast<const OPAddBoundlessHoleRet*>(packet);
	if(ret)
	{
		UserAdventureDataInfo& userAdventureDataInfo=ServerDateManager::Get()->getUserAdventureDataInfo();
		userAdventureDataInfo.boundlessHoldeInfo.currCount = ret->currcount();
		userAdventureDataInfo.boundlessHoldeInfo.todayStatus= ret->todaystatus();
		userAdventureDataInfo.boundlessHoldeInfo.accumulateCount=ret->accumulatecount();
		if(ret->todaystatus()==2)
		{
			MSG_BOX(Language::Get()->getString("@KissInterrupted").c_str());
		}else
		{
			if(ret->currcount()>0)
			{
				MSG_BOX(Language::Get()->getString("@KissSuccess").c_str());
			}
		}
		
		if(ret->has_skillinfo())
		{
			GetPropManager::Get()->gotSkill(ret->skillinfo());		
		}
		if(ret->has_equipinfo())
		{
			GetPropManager::Get()->gotEquipment(ret->equipinfo());
		}
		if(ret->has_reward())
		{
			const OPRewardInfoRet_RewardInfo &info = ret->reward();
			ResInfo * resInfo = ServerDateManager::Get()->getAndCreateResInfo(1);
			resInfo->count = info.count();
			resInfo->itemId = info.itemid();
			resInfo->type = info.itemtype();
			ResManager::Get()->setResInfo(resInfo);
			MsgMainFramePushPage msg;
			msg.pageName = "PackPreviewPage";
			MessageManager::Get()->sendMessage(&msg);
			MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
			packMsg->index=3;
			MessageManager::Get()->sendMessage(packMsg);

		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPFightEndBattleHandle, OPCODE_FIGHTEND_BATTLERET_S)
{
	const  OPFightEndBattleRet* battleRet = dynamic_cast<const OPFightEndBattleRet*>(packet);
	if(battleRet&&battleRet->status()==1)
	{
		const  OPFightEndInfoRet* ret = dynamic_cast<const OPFightEndInfoRet*>(&battleRet->fightendinfo());
		if(ret&&ret->status()==1)
		{
			AdventureFightEndInfo* adventureFightEndInfo=ServerDateManager::Get()->getAdventureFightEndInfo();
			
			adventureFightEndInfo->mPanelType=ret->paneltype();

			//mTodayFightInfo
			const OPFightEndInfoRet_PassBarrierInfo todayBarrierInfo=ret->todayfightinfo();
			adventureFightEndInfo->mTodayFightInfo.attackCount			=todayBarrierInfo.attackcount();
			adventureFightEndInfo->mTodayFightInfo.surplusAttackCount	=todayBarrierInfo.surplusattackcount();
			adventureFightEndInfo->mTodayFightInfo.currBarrier			=todayBarrierInfo.currbarrier();
			adventureFightEndInfo->mTodayFightInfo.currObtainStar		=todayBarrierInfo.currobtainstar();
			adventureFightEndInfo->mTodayFightInfo.maxPassBarrier		=todayBarrierInfo.maxpassbarrier();
			adventureFightEndInfo->mTodayFightInfo.maxObtainStar			=todayBarrierInfo.maxobtainstar();
			adventureFightEndInfo->mTodayFightInfo.stepObtainStar		=todayBarrierInfo.stepobtainstar();
			adventureFightEndInfo->mTodayFightInfo.currSurplusStar		=todayBarrierInfo.currsurplusstar();
			adventureFightEndInfo->mTodayFightInfo.exceedObtainStar		=todayBarrierInfo.exceedobtainstar();
			//TotalAdditionStatInfo
			const OPFightEndInfoRet_TotalAdditionStatInfo todayAddition=todayBarrierInfo.todayaddition();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.hp		=todayAddition.hp();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.attack  =todayAddition.attack();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.defense =todayAddition.defense();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.will	=todayAddition.will();
			//fightTeamInfo
			if(ret->has_teaminfo())
			{
				ServerDateManager::Get()->removeFightEndAllTeamItem();
				const OPFightEndInfoRet_FightTeamInfo fightTeamInfo=ret->teaminfo();
				for(int i=0;i<fightTeamInfo.teamitem_size();++i)
				{
					const OPFightEndInfoRet_FightTeamInfo_FightTeamItem info = fightTeamInfo.teamitem(i);
					AdventureFightEndInfo::FightTeamItem* fightTeamItem=ServerDateManager::Get()->addFightEndFightItemById(info.id());
					fightTeamItem->id=info.id();
					fightTeamItem->memberId=info.memberid();
					fightTeamItem->selfMember=info.selfmember();
					fightTeamItem->opponentMember=info.opponentmember();
					fightTeamItem->rewardKeyId=info.rewardkeyid();
				}
			}

			//stepAdditionItem
			if(ret->has_stepadditioninfo())
			{
				ServerDateManager::Get()->removeFightEndAllStepAdditionItem();
				const OPFightEndInfoRet_StepAdditionInfo stepAdditionInfo=ret->stepadditioninfo();
				for(int i=0;i<stepAdditionInfo.stepadditem_size();++i)
				{
					const OPFightEndInfoRet_StepAdditionInfo_StepAdditionItem info = stepAdditionInfo.stepadditem(i);
					AdventureFightEndInfo::StepAdditionItem* stepAdditionItem=ServerDateManager::Get()->addFightEndStepAdditionItemById(info.id());
					stepAdditionItem->id=info.id();
					stepAdditionItem->type=info.type();
					stepAdditionItem->percentage=info.percentage();
					stepAdditionItem->requireStar=info.requirestar();
				}
			}

			//rewardItem
			if(ret->has_rewardinfo())
			{
				ServerDateManager::Get()->removeFightEndAllRewardItem();
				const OPFightEndInfoRet_RewardInfo rewardItemInfo=ret->rewardinfo();
				for(int i=0;i<rewardItemInfo.rewarditem_size();++i)
				{
					const OPFightEndInfoRet_RewardInfo_RewardItem info = rewardItemInfo.rewarditem(i);
					AdventureFightEndInfo::RewardItem* rewardItem=ServerDateManager::Get()->addFightEndRewardItemById(i);
					rewardItem->id=i;
					rewardItem->itemId=info.rewardid();
					rewardItem->itemType=info.rewardtype();
					rewardItem->count=info.rewardquantity();
				}
			}
			//todayranking
			if(ret->has_todayranking())
			{
				adventureFightEndInfo->mCalculateTodayRanking=ret->todayranking();
			}
			adventureFightEndInfo->mContinueInRankAmount=ret->continueinrankamount();
			if(ret->has_isnewstatus())
			{
				adventureFightEndInfo->mIsNewStatus=ret->isnewstatus();
			}
			else
			{
				adventureFightEndInfo->mIsNewStatus=1;
			}
		}
		
		if(battleRet->has_battle())
		{
			const OPUserBattleRet* msg = dynamic_cast<const OPUserBattleRet*>(&battleRet->battle());
			if(msg)
			{
				BasePage* bp = CCBManager::Get()->getPage("FightPage");
				if(bp)
				{
					FightPage* fp = dynamic_cast<FightPage*>(bp);
					fp->setMessage(msg);
					fp->setFightType(FightEndBattle);
				}
				MsgMainFrameCoverShow msg2;
				msg2.pageName = "FightPage";
				MessageManager::Get()->sendMessage(&msg2);
				bool fightLose=false;
				switch(msg->result().winlose())
				{
				case OPUserBattleRet_Result_WINLOSE_BIGWIN:
					fightLose=true;;
					break;
				case OPUserBattleRet_Result_WINLOSE_WIN:
					fightLose=true;
					break;
				case OPUserBattleRet_Result_WINLOSE_JUSTWIN:
					fightLose=true;
					break;
				}
				std::string fightLoseKey="adventureFightEndRet";
				if(BlackBoard::Get()->hasVarible(fightLoseKey))
				{
					BlackBoard::Get()->setVarible(fightLoseKey,fightLose);
				}
				else
				{
					BlackBoard::Get()->addVarible(fightLoseKey,fightLose);
				}	
			}
		}
		unsigned int toolId=0;
		for(int index=0; index < battleRet->toolinfo_size(); ++index)
		{
			GetPropManager::Get()->gotTool(battleRet->toolinfo(index));
			toolId=battleRet->toolinfo(index).id();
			if(battleRet->toolinfo(index).itemid()/1000 == 3501)
			{
				ServerDateManager::Get()->_getMarketInfo().gotVipGifts.insert(battleRet->toolinfo(index).itemid());
			}
		}
		
		std::string fightEndToolKey="adventureFightEndToolId";
		if(BlackBoard::Get()->hasVarible(fightEndToolKey))
		{
			BlackBoard::Get()->setVarible(fightEndToolKey,toolId);
		}
		else
		{
			BlackBoard::Get()->addVarible(fightEndToolKey,toolId);
		}
	}
	
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPFightEndAddYestAttrHandle, OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S)
{
	const  OPFightEndAddYestAttrRet* yestAddRet = dynamic_cast<const OPFightEndAddYestAttrRet*>(packet);
	if(yestAddRet&&yestAddRet->status()==1)
	{
		const  OPFightEndInfoRet* ret = dynamic_cast<const OPFightEndInfoRet*>(&yestAddRet->fightendinfo());
		if(ret&&ret->status()==1)
		{
			AdventureFightEndInfo* adventureFightEndInfo=ServerDateManager::Get()->getAdventureFightEndInfo();

			adventureFightEndInfo->mPanelType=ret->paneltype();
			//mTodayFightInfo
			const OPFightEndInfoRet_PassBarrierInfo todayBarrierInfo=ret->todayfightinfo();
			adventureFightEndInfo->mTodayFightInfo.attackCount			=todayBarrierInfo.attackcount();
			adventureFightEndInfo->mTodayFightInfo.surplusAttackCount	=todayBarrierInfo.surplusattackcount();
			adventureFightEndInfo->mTodayFightInfo.currBarrier			=todayBarrierInfo.currbarrier();
			adventureFightEndInfo->mTodayFightInfo.currObtainStar		=todayBarrierInfo.currobtainstar();
			adventureFightEndInfo->mTodayFightInfo.maxPassBarrier		=todayBarrierInfo.maxpassbarrier();
			adventureFightEndInfo->mTodayFightInfo.maxObtainStar			=todayBarrierInfo.maxobtainstar();
			adventureFightEndInfo->mTodayFightInfo.stepObtainStar		=todayBarrierInfo.stepobtainstar();
			adventureFightEndInfo->mTodayFightInfo.currSurplusStar		=todayBarrierInfo.currsurplusstar();
			adventureFightEndInfo->mTodayFightInfo.exceedObtainStar		=todayBarrierInfo.exceedobtainstar();
			//TotalAdditionStatInfo
			const OPFightEndInfoRet_TotalAdditionStatInfo todayAddition=todayBarrierInfo.todayaddition();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.hp		=todayAddition.hp();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.attack  =todayAddition.attack();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.defense =todayAddition.defense();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.will	=todayAddition.will();
			//fightTeamInfo
			if(ret->has_teaminfo())
			{
				ServerDateManager::Get()->removeFightEndAllTeamItem();
				const OPFightEndInfoRet_FightTeamInfo fightTeamInfo=ret->teaminfo();
				for(int i=0;i<fightTeamInfo.teamitem_size();++i)
				{
					const OPFightEndInfoRet_FightTeamInfo_FightTeamItem info = fightTeamInfo.teamitem(i);
					AdventureFightEndInfo::FightTeamItem* fightTeamItem=ServerDateManager::Get()->addFightEndFightItemById(info.id());
					fightTeamItem->id=info.id();
					fightTeamItem->memberId=info.memberid();
					fightTeamItem->selfMember=info.selfmember();
					fightTeamItem->opponentMember=info.opponentmember();
					fightTeamItem->rewardKeyId=info.rewardkeyid();
				}
			}
			adventureFightEndInfo->mContinueInRankAmount=ret->continueinrankamount();
			if(ret->has_isnewstatus())
			{
				adventureFightEndInfo->mIsNewStatus=ret->isnewstatus();
			}
			else
			{
				adventureFightEndInfo->mIsNewStatus=1;
			}
		}
	}

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPFightEndTempAddStepAttrHandle, OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S)
{
	const OPFightEndTempAddStepAttrRet* tempAddStepRet=dynamic_cast<const OPFightEndTempAddStepAttrRet*>(packet);
	if(tempAddStepRet&&tempAddStepRet->status()==1)
	{
		const  OPFightEndInfoRet* ret = dynamic_cast<const OPFightEndInfoRet*>(&tempAddStepRet->fightendinfo());
		if(ret&&ret->status()==1)
		{
			AdventureFightEndInfo* adventureFightEndInfo=ServerDateManager::Get()->getAdventureFightEndInfo();

			adventureFightEndInfo->mPanelType=ret->paneltype();
			//mTodayFightInfo
			const OPFightEndInfoRet_PassBarrierInfo todayBarrierInfo=ret->todayfightinfo();
			adventureFightEndInfo->mTodayFightInfo.attackCount			=todayBarrierInfo.attackcount();
			adventureFightEndInfo->mTodayFightInfo.surplusAttackCount	=todayBarrierInfo.surplusattackcount();
			adventureFightEndInfo->mTodayFightInfo.currBarrier			=todayBarrierInfo.currbarrier();
			adventureFightEndInfo->mTodayFightInfo.currObtainStar		=todayBarrierInfo.currobtainstar();
			adventureFightEndInfo->mTodayFightInfo.maxPassBarrier		=todayBarrierInfo.maxpassbarrier();
			adventureFightEndInfo->mTodayFightInfo.maxObtainStar			=todayBarrierInfo.maxobtainstar();
			adventureFightEndInfo->mTodayFightInfo.stepObtainStar		=todayBarrierInfo.stepobtainstar();
			adventureFightEndInfo->mTodayFightInfo.currSurplusStar		=todayBarrierInfo.currsurplusstar();
			adventureFightEndInfo->mTodayFightInfo.exceedObtainStar		=todayBarrierInfo.exceedobtainstar();
			//TotalAdditionStatInfo
			const OPFightEndInfoRet_TotalAdditionStatInfo todayAddition=todayBarrierInfo.todayaddition();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.hp		=todayAddition.hp();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.attack  =todayAddition.attack();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.defense =todayAddition.defense();
			adventureFightEndInfo->mTodayFightInfo.todayAddition.will	=todayAddition.will();
			//fightTeamInfo
			if(ret->has_teaminfo())
			{
				ServerDateManager::Get()->removeFightEndAllTeamItem();
				const OPFightEndInfoRet_FightTeamInfo fightTeamInfo=ret->teaminfo();
				for(int i=0;i<fightTeamInfo.teamitem_size();++i)
				{
					const OPFightEndInfoRet_FightTeamInfo_FightTeamItem info = fightTeamInfo.teamitem(i);
					AdventureFightEndInfo::FightTeamItem* fightTeamItem=ServerDateManager::Get()->addFightEndFightItemById(info.id());
					fightTeamItem->id=info.id();
					fightTeamItem->memberId=info.memberid();
					fightTeamItem->selfMember=info.selfmember();
					fightTeamItem->opponentMember=info.opponentmember();
					fightTeamItem->rewardKeyId=info.rewardkeyid();
				}
			}
			//rewardItem
			if(ret->has_rewardinfo())
			{
				ServerDateManager::Get()->removeFightEndAllRewardItem();
				const OPFightEndInfoRet_RewardInfo rewardItemInfo=ret->rewardinfo();
				for(int i=0;i<rewardItemInfo.rewarditem_size();++i)
				{
					const OPFightEndInfoRet_RewardInfo_RewardItem info = rewardItemInfo.rewarditem(i);
					AdventureFightEndInfo::RewardItem* rewardItem=ServerDateManager::Get()->addFightEndRewardItemById(i);
					rewardItem->id=i;
					rewardItem->itemId=info.rewardid();
					rewardItem->itemType=info.rewardtype();
					rewardItem->count=info.rewardquantity();
				}
			}
			adventureFightEndInfo->mContinueInRankAmount=ret->continueinrankamount();
			if(ret->has_isnewstatus())
			{
				adventureFightEndInfo->mIsNewStatus=ret->isnewstatus();
			}
			else
			{
				adventureFightEndInfo->mIsNewStatus=1;
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPFightEndGetRewardHandle, OPCODE_GET_FIGHTEND_REWARD_INFORET_S)
{
	const OPFightEndGetRewardRet* rewardRet=dynamic_cast<const OPFightEndGetRewardRet*>(packet);
	
	if(rewardRet&&rewardRet->status()==1)
	{
		const  OPFightEndInfoRet* ret = dynamic_cast<const OPFightEndInfoRet*>(&rewardRet->fightendinfo());
		if(ret&&ret->status()==1)
		{
			AdventureFightEndInfo* adventureFightEndInfo=ServerDateManager::Get()->getAdventureFightEndInfo();
			adventureFightEndInfo->mPanelType=ret->paneltype();

			//mTodayFightInfo
			const OPFightEndInfoRet_PassBarrierInfo todayBarrierInfo=ret->todayfightinfo();
			adventureFightEndInfo->mTodayFightInfo.attackCount			=todayBarrierInfo.attackcount();
			adventureFightEndInfo->mTodayFightInfo.surplusAttackCount	=todayBarrierInfo.surplusattackcount();
			adventureFightEndInfo->mTodayFightInfo.currBarrier			=todayBarrierInfo.currbarrier();
			adventureFightEndInfo->mTodayFightInfo.currObtainStar		=todayBarrierInfo.currobtainstar();
			adventureFightEndInfo->mTodayFightInfo.maxPassBarrier		=todayBarrierInfo.maxpassbarrier();
			adventureFightEndInfo->mTodayFightInfo.maxObtainStar			=todayBarrierInfo.maxobtainstar();
			adventureFightEndInfo->mTodayFightInfo.stepObtainStar		=todayBarrierInfo.stepobtainstar();
			adventureFightEndInfo->mTodayFightInfo.currSurplusStar		=todayBarrierInfo.currsurplusstar();
			adventureFightEndInfo->mTodayFightInfo.exceedObtainStar		=todayBarrierInfo.exceedobtainstar();
			//fightTeamInfo
			if(ret->has_teaminfo())
			{
				ServerDateManager::Get()->removeFightEndAllTeamItem();
				const OPFightEndInfoRet_FightTeamInfo fightTeamInfo=ret->teaminfo();
				for(int i=0;i<fightTeamInfo.teamitem_size();++i)
				{
					const OPFightEndInfoRet_FightTeamInfo_FightTeamItem info = fightTeamInfo.teamitem(i);
					AdventureFightEndInfo::FightTeamItem* fightTeamItem=ServerDateManager::Get()->addFightEndFightItemById(info.id());
					fightTeamItem->id=info.id();
					fightTeamItem->memberId=info.memberid();
					fightTeamItem->selfMember=info.selfmember();
					fightTeamItem->opponentMember=info.opponentmember();
					fightTeamItem->rewardKeyId=info.rewardkeyid();
				}
			}
			adventureFightEndInfo->mContinueInRankAmount=ret->continueinrankamount();
			if(ret->has_isnewstatus())
			{
				adventureFightEndInfo->mIsNewStatus=ret->isnewstatus();
			}
			else
			{
				adventureFightEndInfo->mIsNewStatus=1;
			}
		}
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		bool isHavReward=false;
		if(rewardRet->has_goldcoins())
		{
			if(rewardRet->goldcoins()>userBasicInfo.goldcoins)
			{
				isHavReward=true;
			}
			userBasicInfo.goldcoins = rewardRet->goldcoins();
		}
		if(rewardRet->has_silvercoins())
		{
			if(rewardRet->silvercoins()>userBasicInfo.silvercoins)
			{
				isHavReward=true;
			}
			userBasicInfo.silvercoins = rewardRet->silvercoins();
		}
		for(int index=0; index < rewardRet->toolinfo_size(); ++index)
		{
			isHavReward=true;
			GetPropManager::Get()->gotTool(rewardRet->toolinfo(index));	
			if(rewardRet->toolinfo(index).itemid()/1000 == 3501)
			{
				ServerDateManager::Get()->_getMarketInfo().gotVipGifts.insert(rewardRet->toolinfo(index).itemid());
			}
		}
		if(isHavReward)
		{
			PackPreviewPageShow(FIGHT_END_REWARD);
		}
		
	}

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetAdventureFightEndHandle, OPCODE_GET_FIGHTEND_INFORET_S)
{
	const  OPFightEndInfoRet* ret = dynamic_cast<const OPFightEndInfoRet*>(packet);
	if(ret&&ret->status()==1)
	{
		AdventureFightEndInfo* adventureFightEndInfo=ServerDateManager::Get()->getAdventureFightEndInfo();
		ServerDateManager::Get()->clearFightEndAllMap();

		adventureFightEndInfo->mPanelType=ret->paneltype();
		//mYestFightInfo
		const OPFightEndInfoRet_PassBarrierInfo yestBarrierInfo=ret->yestfightinfo();
		adventureFightEndInfo->mYestFightInfo.maxPassBarrier	=yestBarrierInfo.maxpassbarrier();
		adventureFightEndInfo->mYestFightInfo.maxObtainStar	=yestBarrierInfo.maxobtainstar();
		adventureFightEndInfo->mYestFightInfo.addition		=yestBarrierInfo.addition();
		
		//mTodayFightInfo
		const OPFightEndInfoRet_PassBarrierInfo todayBarrierInfo=ret->todayfightinfo();
		adventureFightEndInfo->mTodayFightInfo.attackCount			=todayBarrierInfo.attackcount();
		adventureFightEndInfo->mTodayFightInfo.surplusAttackCount	=todayBarrierInfo.surplusattackcount();
		adventureFightEndInfo->mTodayFightInfo.currBarrier			=todayBarrierInfo.currbarrier();
		adventureFightEndInfo->mTodayFightInfo.currObtainStar		=todayBarrierInfo.currobtainstar();
		adventureFightEndInfo->mTodayFightInfo.maxPassBarrier		=todayBarrierInfo.maxpassbarrier();
		adventureFightEndInfo->mTodayFightInfo.maxObtainStar			=todayBarrierInfo.maxobtainstar();
		adventureFightEndInfo->mTodayFightInfo.stepObtainStar		=todayBarrierInfo.stepobtainstar();
		adventureFightEndInfo->mTodayFightInfo.currSurplusStar		=todayBarrierInfo.currsurplusstar();
		adventureFightEndInfo->mTodayFightInfo.exceedObtainStar		=todayBarrierInfo.exceedobtainstar();
		//TotalAdditionStatInfo
		const OPFightEndInfoRet_TotalAdditionStatInfo todayAddition=todayBarrierInfo.todayaddition();
		adventureFightEndInfo->mTodayFightInfo.todayAddition.hp		=todayAddition.hp();
		adventureFightEndInfo->mTodayFightInfo.todayAddition.attack  =todayAddition.attack();
		adventureFightEndInfo->mTodayFightInfo.todayAddition.defense =todayAddition.defense();
		adventureFightEndInfo->mTodayFightInfo.todayAddition.will	=todayAddition.will();
		//fightTeamInfo
		if(ret->has_teaminfo())
		{
			const OPFightEndInfoRet_FightTeamInfo fightTeamInfo=ret->teaminfo();
			for(int i=0;i<fightTeamInfo.teamitem_size();++i)
			{
				const OPFightEndInfoRet_FightTeamInfo_FightTeamItem info = fightTeamInfo.teamitem(i);
				AdventureFightEndInfo::FightTeamItem* fightTeamItem=ServerDateManager::Get()->addFightEndFightItemById(info.id());
				fightTeamItem->id=info.id();
				fightTeamItem->memberId=info.memberid();
				fightTeamItem->selfMember=info.selfmember();
				fightTeamItem->opponentMember=info.opponentmember();
				fightTeamItem->rewardKeyId=info.rewardkeyid();
			}
		}

		//stepAdditionItem
		if(ret->has_stepadditioninfo())
		{
			const OPFightEndInfoRet_StepAdditionInfo stepAdditionInfo=ret->stepadditioninfo();
			for(int i=0;i<stepAdditionInfo.stepadditem_size();++i)
			{
				const OPFightEndInfoRet_StepAdditionInfo_StepAdditionItem info = stepAdditionInfo.stepadditem(i);
				AdventureFightEndInfo::StepAdditionItem* stepAdditionItem=ServerDateManager::Get()->addFightEndStepAdditionItemById(info.id());
				stepAdditionItem->id=info.id();
				stepAdditionItem->type=info.type();
				stepAdditionItem->percentage=info.percentage();
				stepAdditionItem->requireStar=info.requirestar();
			}
		}

		//rewardItem
		if(ret->has_rewardinfo())
		{
			ServerDateManager::Get()->removeFightEndAllRewardItem();
			const OPFightEndInfoRet_RewardInfo rewardItemInfo=ret->rewardinfo();
			for(int i=0;i<rewardItemInfo.rewarditem_size();++i)
			{
				const OPFightEndInfoRet_RewardInfo_RewardItem info = rewardItemInfo.rewarditem(i);
				AdventureFightEndInfo::RewardItem* rewardItem=ServerDateManager::Get()->addFightEndRewardItemById(i);
				rewardItem->id=i;
				rewardItem->itemId=info.rewardid();
				rewardItem->itemType=info.rewardtype();
				rewardItem->count=info.rewardquantity();
			}
		}
		//todayranking
		if(ret->has_todayranking())
		{
			adventureFightEndInfo->mCalculateTodayRanking=ret->todayranking();
		}
		adventureFightEndInfo->mContinueInRankAmount=ret->continueinrankamount();
		if(ret->has_isnewstatus())
		{
			adventureFightEndInfo->mIsNewStatus=ret->isnewstatus();
		}
		else
		{
			adventureFightEndInfo->mIsNewStatus=1;
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPFightEndRankInfoHandle, OPCODE_GET_FIGHTEND_RANK_INFORET_S)
{
	const  OPFightEndRankInfoRet* ret = dynamic_cast<const OPFightEndRankInfoRet*>(packet);
	if(ret&&ret->status()==1)
	{
		FightEndRankInfo& fightInfo=ServerDateManager::Get()->getFightEndRankInfo();
		ServerDateManager::Get()->clearFightEndRankItemByIndex(ret->ranktype());
		for(int i=0;i<ret->rankitem_size();++i)
		{
			const OPFightEndRankInfoRet_RankItem info = ret->rankitem(i);
			FightEndRankInfo::RankItem* rankItem=ServerDateManager::Get()->addRankItemByIndexAndId(ret->ranktype(),info.id());
			rankItem->id=info.id();
			rankItem->playerId=info.playerid();
			rankItem->level=info.level();
			rankItem->playerName=info.playername();
			rankItem->passBattle=info.passbattle();
			rankItem->obtainStar=info.obtainstar();
			rankItem->sussessionCount=info.sussessioncount();
		}
		fightInfo.mPlayerId=ret->playerid();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UpgradeRoleHandle, OPCODE_UPGRADE_ROLERET_S)
{
	const  OPUpgradeRoleRet* ret = dynamic_cast<const OPUpgradeRoleRet*>(packet);
	if(ret&&ret->status()==1)
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerUpgradePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPUpgradeEquipHandle,OPCODE_UPGRADE_EQUIPRET_S)
{
	const OPUpgradeEquipRet * upgradeEquipRet = dynamic_cast<const OPUpgradeEquipRet*>(packet);

	const OPUpgradeEquipRet_EquipInfo& info = upgradeEquipRet->equip();
	UserEquipInfo * userEquipInfo = ServerDateManager::Get()->getAndCreatUserEquipInfo(info.id());
	userEquipInfo->itemid		= info.itemid();
	userEquipInfo->level		= info.level();
	for(int i=0; i < info.pinfo_size(); ++i)
	{
		const OPUpgradeEquipRet_EquipInfo_PropertyInfo& pInfo = info.pinfo(i);
		userEquipInfo->propertyInfo[pInfo.type()] = pInfo.num();
	}
	
	if(upgradeEquipRet->has_battle())
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = upgradeEquipRet->battle();
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getUserBattleInfo(info.position());
		userBattleInfo->id			=info.id();
		userBattleInfo->discipleId	=info.discipleid();
		userBattleInfo->attack		=info.attack();
		userBattleInfo->defence		=info.defence();
		userBattleInfo->health		=info.health();
		userBattleInfo->rp			=info.rp();
		userBattleInfo->skill1		=info.skill1();
		userBattleInfo->skill2		=info.skill2();
		userBattleInfo->equip1		=info.equip1();
		userBattleInfo->equip2		=info.equip2();
		userBattleInfo->equip3		=info.equip3();
		userBattleInfo->position	=info.position();

		userBattleInfo->fateInfos.clear();
		for(int i=0; i < info.fates_size(); ++i)
		{
			const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
			userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
		}
	}

	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	userBasicInfo.silvercoins				= upgradeEquipRet->silvercoins();

// 	MsgMainFrameChangePage msg;
// 	msg.pageName = "EquipEnhanceResultPage";
// 	MessageManager::Get()->sendMessage(&msg);
	if(upgradeEquipRet->has_updatetimes()){
		BlackBoard::Get()->quickEnhanceTimes = upgradeEquipRet->updatetimes();
		if(GET_V_B("EquipEnhanceResultPage"))
		{
			MsgMainFrameRefreshPage msg1;
			msg1.pageName = "EquipEnhanceResultPage";
			MessageManager::Get()->sendMessage(&msg1);
		}
		else
		{
			MsgMainFrameChangePage msg1;
			msg1.pageName = "EquipEnhanceResultPage";
			MessageManager::Get()->sendMessage(&msg1);
		}
		MsgMainFramePushPage msg;
		msg.pageName = "EquipQuickEnhancePage";
		MessageManager::Get()->sendMessage(&msg);
	}else{
		MsgMainFramePushPage msg;
		msg.pageName = "EquipEnhanceAniPage";
		MessageManager::Get()->sendMessage(&msg);

	}

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(HandbookInfoHandle, OPCODE_HANDBOOK_INFORET_S)
{
	const  OPHandbookInfoRet* ret = dynamic_cast<const OPHandbookInfoRet*>(packet);
	for(int index=0;index <ret->personinfo_size();++index){
		const OPHandbookInfoRet_PersonInfo& info = ret->personinfo(index);
		HandbookPersonInfo * handbookPersonInfo = ServerDateManager::Get()->getAndCreatHandbookPersonInfo(info.id());
		handbookPersonInfo->id = info.id();
		handbookPersonInfo->userid = info.userid();
		handbookPersonInfo->userstatus = info.userstatus();
	}
	for(int index=0;index < ret->equipmentinfo_size();++index){
		const OPHandbookInfoRet_EquipmentInfo& info = ret->equipmentinfo(index);
		HandbookEquipmentInfo * handbookEquipmentInfo = ServerDateManager::Get()->getAndCreatHandbookEquipmentInfo(info.id());
		handbookEquipmentInfo->id = info.id();
		handbookEquipmentInfo->equipmentid = info.equipmentid();
		handbookEquipmentInfo->equipmentstatus = info.equipmentstatus();
	}
	for(int index=0;index < ret->skillinfo_size();++index){
		const OPHandbookInfoRet_SkillInfo& info = ret->skillinfo(index);
		HandbookSkillInfo * handbookSkillInfo = ServerDateManager::Get()->getAndCreatHandbookSkillInfo(info.id());
		handbookSkillInfo->id = info.id();
		handbookSkillInfo->skillid = info.skillid();
		handbookSkillInfo->skillstatus = info.skillstatus();
	}
}

DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetAdventureTeacherInfoHandle,OPCODE_GET_ADVENTURE_TEACHERINFORET_S)
{
	const  OPGetTeacherInfoRet* ret = dynamic_cast<const OPGetTeacherInfoRet*>(packet);
	if(ret&&ret->status()==1)
	{
		AdventureTeacherInfo* info=ServerDateManager::Get()->getAndCreateAdventureTeacherInfo(ret->adventureitemid());
		info->exp = ret->exp();
		info->endTime = ret->endtime();
		info->teacherId = ret->teacherid();
		info->serverTime = ret->servertime();
		info->totalTimeStamp = GamePrecedure::Get()->getTotalTime();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPAddTeachExpHandle,OPCODE_ADD_ADVENTURE_TEACHEXPRET_S)
{
	const OPAddTeachExpRet * addTeachExpRet = dynamic_cast<const OPAddTeachExpRet*>(packet);

	if(addTeachExpRet&&addTeachExpRet->status()==1)
	{
		for(int index=0; index < addTeachExpRet->expinfo_size(); ++index)
		{
			const OPAddTeachExpRet_DiscipleExpInfo& info = addTeachExpRet->expinfo(index);
			DiscipleExpAddedInfo * discipleExpAddedInfo = ServerDateManager::Get()->getAndCreateDiscipleExpAddedInfo(info.id());
			UserDiscipleInfo* discipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByItemID(info.discipleid());
			discipleExpAddedInfo->discipleId = info.discipleid();
			discipleExpAddedInfo->exp = info.exp();
			discipleInfo->exp = info.exp();
			discipleExpAddedInfo->expAdded = info.expadd();
			discipleExpAddedInfo->extraExpAdded = info.extraexpadd();
			discipleExpAddedInfo->preLevel = discipleInfo->level;
			discipleExpAddedInfo->level = info.level();
			discipleInfo->level = info.level();
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPChangeSkillHandle, OPCODE_CHANGE_SKILLRET_S)
{
	const OPChangeSkillRet * upgradeSkillRet = dynamic_cast<const OPChangeSkillRet*>(packet);

	for (int index = 0; index < upgradeSkillRet->battle_size();++index)
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = upgradeSkillRet->battle(index);

		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(info.id());
		if(userBattleInfo)
		{
			userBattleInfo->id			=info.id();
			userBattleInfo->attack		=info.attack();
			userBattleInfo->defence		=info.defence();
			userBattleInfo->health		=info.health();
			userBattleInfo->rp			=info.rp();
			userBattleInfo->skill1		=info.skill1();
			userBattleInfo->skill2		=info.skill2();

			userBattleInfo->fateInfos.clear();
			for(int i=0; i < info.fates_size(); ++i)
			{
				const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
				userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
			}
		}
	}
	
	MsgMainFrameChangePage msg;
	msg.pageName = "TeamPage";
	MessageManager::Get()->sendMessage(&msg);

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPChangeEquipHandle, OPCODE_CHANGE_EQUIPRET_S)
{
	const OPChangeEquipRet * upgradeEquipRet = dynamic_cast<const OPChangeEquipRet*>(packet);

	for(int index=0; index < upgradeEquipRet->battle_size(); ++index)
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = upgradeEquipRet->battle(index);
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(info.id());
		userBattleInfo->id			=info.id();
		userBattleInfo->attack		=info.attack();
		userBattleInfo->defence		=info.defence();
		userBattleInfo->health		=info.health();
		userBattleInfo->rp			=info.rp();
		userBattleInfo->equip1		=info.equip1();
		userBattleInfo->equip2		=info.equip2();
		userBattleInfo->equip3		=info.equip3();

		userBattleInfo->fateInfos.clear();
		for(int i=0; i < info.fates_size(); ++i)
		{
			const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
			userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
		}
	}
	
	MsgMainFrameChangePage msg;
	msg.pageName = "TeamPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPAdventureLevelUpRewardHandle, OPCODE_ADVENTURE_LEVEL_UP_REWARDRET_S)
{
	const  OPAdventureLevelUpRewardRet* rewardRet = dynamic_cast<const OPAdventureLevelUpRewardRet*>(packet);
	ServerDateManager::Get()->setAdventureLevelUpRewardInfoSize(rewardRet->levelupreward_size());
	for(int index = 0 ; index < rewardRet->levelupreward_size();++index){
		const OPAdventureLevelUpRewardRet_AdventureLevelUpRewardInfo& info = rewardRet->levelupreward(index);
		AdventureLevelUpRewardInfo * adventureLevelUpRewardInfo = ServerDateManager::Get()->getAndCreatAdventureLevelUpRewardInfo(info.id());
		adventureLevelUpRewardInfo->id = info.id();
		adventureLevelUpRewardInfo->needLevel = info.needlevel();
		adventureLevelUpRewardInfo->rechargenum = info.rechargenum();
		adventureLevelUpRewardInfo->goldcoins = info.goldcoins();
		adventureLevelUpRewardInfo->upVipLevel = info.upviplevel();
		adventureLevelUpRewardInfo->rewardStatus = info.rewardstatus();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPAdventureLevelUpGetRewardHandle, OPCODE_ADVENTURE_LEVEL_UP_GET_REWARDRET_S)
{
	const  OPAdventureLevelUpGetRewardRet* rewardRet = dynamic_cast<const OPAdventureLevelUpGetRewardRet*>(packet);
	if(rewardRet)
	{
		ServerDateManager::Get()->creatAdventureLevelUpGetRewardInfo(rewardRet->rechargenum(),rewardRet->goldcoins());
		UserBasicInfo& basicInfo=ServerDateManager::Get()->getUserBasicInfo();
		if(rewardRet->has_viplevel())
		{
			basicInfo.viplevel=rewardRet->viplevel();
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetWorldBossInfoHandle, OPCODE_GET_WORLD_BOSS_INFORET_S)
{
	
	const OPGetWorldBossInfoRet * worldBossInfoRet = dynamic_cast<const OPGetWorldBossInfoRet*>(packet);

	WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();

	worldBossItem.servertime = worldBossInfoRet->servertime();
	worldBossItem.finished = worldBossInfoRet->finished();
	worldBossItem.bossLevel = worldBossInfoRet->bosslevel();

	ServerDateManager::Get()->clearWorldBossDamage();
	
	int tempInt = 0;
	for(int tempIndex = 2; tempIndex>0 ; tempIndex--){
		for(int index = 0 ; index < worldBossInfoRet->battlearray_size();++index){
			const OPGetWorldBossInfoRet_WorldBossBattleArray& info = worldBossInfoRet->battlearray(worldBossInfoRet->battlearray_size() - index - 1);
			if(tempIndex%2==0&&info.isfinal()==false){
				WorldBossBattleItem* worldbossBattleItem = ServerDateManager::Get()->getAndCreateWorldBossBattleItem(tempInt+1);
				worldbossBattleItem->id = tempInt+1;
				worldbossBattleItem->rank = info.rank();
				worldbossBattleItem->name = info.name();
				worldbossBattleItem->level = info.level();
				worldbossBattleItem->damageBlood = info.damageblood();
				worldbossBattleItem->playerId = info.playerid();
				worldbossBattleItem->isFinal = info.isfinal();
				tempInt++;
			}else if(tempIndex%2==1&&info.isfinal()==true){
				WorldBossBattleItem* worldbossBattleItem = ServerDateManager::Get()->getAndCreateWorldBossBattleItem(tempInt+1);
				worldbossBattleItem->id = tempInt+1;
				worldbossBattleItem->rank = info.rank();
				worldbossBattleItem->name = info.name();
				worldbossBattleItem->level = info.level();
				worldbossBattleItem->damageBlood = info.damageblood();
				worldbossBattleItem->playerId = info.playerid();
				worldbossBattleItem->isFinal = info.isfinal();
				tempInt++;
			}
		}
	}

	WorldBossJoinBattle::countLeftTime(worldBossItem.finished);

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetWorldBossRefreshHandle, OPCODE_WORLDBOSS_REFRESHRET_S)
{

	const OPWorldBossRefreshRet * worldBossRefreshRet = dynamic_cast<const OPWorldBossRefreshRet*>(packet);
	WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();

	worldBossItem.servertime = worldBossRefreshRet->servertime();
	worldBossItem.finished = worldBossRefreshRet->finished();
	worldBossItem.bossTotalBlood = worldBossRefreshRet->bosstotalblood();
	worldBossItem.bossLeftBlood = worldBossRefreshRet->bossleftblood();
	worldBossItem.attackNum = worldBossRefreshRet->attacknum();
	worldBossItem.damageBlood = worldBossRefreshRet->damageblood();
	worldBossItem.useFireTime = worldBossRefreshRet->usefiretime();

	TimeCalculator::Get()->createTimeCalcultor("waitforfighttime",worldBossRefreshRet->waitforfighttime());

	ServerDateManager::Get()->clearWorldBossBattle();

	int tempInt = 0;
	for(int tempIndex = 2; tempIndex>0 ; tempIndex--){
		for(int index = 0 ; index < worldBossRefreshRet->battlearray_size();++index){
			const OPGetWorldBossInfoRet_WorldBossBattleArray& info = worldBossRefreshRet->battlearray(worldBossRefreshRet->battlearray_size() - index - 1);
			if(tempIndex%2==0&&info.isfinal()==false){
				WorldBossBattleItem* worldbossBattleItem = ServerDateManager::Get()->getAndCreateWorldBossBattleItem(tempInt+1);
				worldbossBattleItem->id = tempInt+1;
				worldbossBattleItem->rank = info.rank();
				worldbossBattleItem->name = info.name();
				worldbossBattleItem->level = info.level();
				worldbossBattleItem->damageBlood = info.damageblood();
				worldbossBattleItem->playerId = info.playerid();
				worldbossBattleItem->isFinal = info.isfinal();
				tempInt++;
			}else if(tempIndex%2==1&&info.isfinal()==true){
				WorldBossBattleItem* worldbossBattleItem = ServerDateManager::Get()->getAndCreateWorldBossBattleItem(tempInt+1);
				worldbossBattleItem->id = tempInt+1;
				worldbossBattleItem->rank = info.rank();
				worldbossBattleItem->name = info.name();
				worldbossBattleItem->level = info.level();
				worldbossBattleItem->damageBlood = info.damageblood();
				worldbossBattleItem->playerId = info.playerid();
				worldbossBattleItem->isFinal = info.isfinal();
				tempInt++;
			}
		}
	}

	for(int index = 0 ; index < worldBossRefreshRet->damagearray_size();++index){
		const OPWorldBossRefreshRet_WorldBossDamageArray& info = worldBossRefreshRet->damagearray(worldBossRefreshRet->damagearray_size() - index - 1);
		WorldBossDamageItem* worldbossDamageItem = ServerDateManager::Get()->getAndCreateWorldBossDamageItem(index);
		worldbossDamageItem->id = index;
		worldbossDamageItem->name = info.name();
		worldbossDamageItem->attackNum = info.attacknum();
		worldbossDamageItem->damageBlood = info.damageblood();
		worldbossDamageItem->discipleId = info.discipleid();

	}

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetWorldBossFightHandle, OPCODE_WORLDBOSS_FIGHTRET_S)
{
	const OPWorldBossFightRet * worldBossFightRet = dynamic_cast<const OPWorldBossFightRet*>(packet);

	WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();

	worldBossItem.servertime = worldBossFightRet->servertime();
	worldBossItem.finished = worldBossFightRet->finished();
	worldBossItem.bossTotalBlood = worldBossFightRet->bosstotalblood();
	worldBossItem.bossLeftBlood = worldBossFightRet->bossleftblood();
	worldBossItem.attackNum = worldBossFightRet->attacknum();
	worldBossItem.damageBlood = worldBossFightRet->damageblood();
	worldBossItem.useFireTime = worldBossFightRet->usefiretime();

	BlackBoard::Get()->worldbossFinalFight = 0;
	if(worldBossFightRet->has_isfinalfight()){
		BlackBoard::Get()->worldbossFinalFight = 1;
	}

	TimeCalculator::Get()->createTimeCalcultor("waitforfighttime",worldBossFightRet->waitforfighttime());

	int tempInt = 0;
	for(int tempIndex = 2; tempIndex>0 ; tempIndex--){
		for(int index = 0 ; index < worldBossFightRet->battlearray_size();++index){
			const OPGetWorldBossInfoRet_WorldBossBattleArray& info = worldBossFightRet->battlearray(worldBossFightRet->battlearray_size() - index - 1);
			if(tempIndex%2==0&&info.isfinal()==false){
				WorldBossBattleItem* worldbossBattleItem = ServerDateManager::Get()->getAndCreateWorldBossBattleItem(tempInt+1);
				worldbossBattleItem->id = tempInt+1;
				worldbossBattleItem->rank = info.rank();
				worldbossBattleItem->name = info.name();
				worldbossBattleItem->level = info.level();
				worldbossBattleItem->damageBlood = info.damageblood();
				worldbossBattleItem->playerId = info.playerid();
				worldbossBattleItem->isFinal = info.isfinal();
				tempInt++;
			}else if(tempIndex%2==1&&info.isfinal()==true){
				WorldBossBattleItem* worldbossBattleItem = ServerDateManager::Get()->getAndCreateWorldBossBattleItem(tempInt+1);
				worldbossBattleItem->id = tempInt+1;
				worldbossBattleItem->rank = info.rank();
				worldbossBattleItem->name = info.name();
				worldbossBattleItem->level = info.level();
				worldbossBattleItem->damageBlood = info.damageblood();
				worldbossBattleItem->playerId = info.playerid();
				worldbossBattleItem->isFinal = info.isfinal();
				tempInt++;
			}
		}
	}

	for(int index = 0 ; index < worldBossFightRet->damagearray_size();++index){
		const OPWorldBossRefreshRet_WorldBossDamageArray& info = worldBossFightRet->damagearray(worldBossFightRet->damagearray_size() - index - 1);
		WorldBossDamageItem* worldbossDamageItem = ServerDateManager::Get()->getAndCreateWorldBossDamageItem(index);
		worldbossDamageItem->id = index;
		worldbossDamageItem->name = info.name();
		worldbossDamageItem->attackNum = info.attacknum();
		worldbossDamageItem->damageBlood = info.damageblood();
		worldbossDamageItem->discipleId = info.discipleid();

	}

	if(worldBossItem.finished == 0||(worldBossItem.finished == 1&&worldBossFightRet->has_isfinalfight())){
		const OPUserBattleRet* msg = dynamic_cast<const OPUserBattleRet*>(&worldBossFightRet->fightinfo());
		if(msg)
		{
			BasePage* bp = CCBManager::Get()->getPage("FightPage");
			if(bp)
			{
				FightPage* fp = dynamic_cast<FightPage*>(bp);
				fp->setMessage(msg);
				fp->setFightType(WorldBoss);
			}
			MsgMainFrameCoverShow msg2;
			msg2.pageName = "FightPage";
			MessageManager::Get()->sendMessage(&msg2);
		}
	}


	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	std::string basicSyncKey="basicSyncKey";
	bool isSync=false;
	if(BlackBoard::Get()->hasVarible(basicSyncKey))
	{
		isSync=BlackBoard::Get()->getVaribleAsBool(basicSyncKey);
	}
	if(!isSync)
	{
		userBasicInfo.goldcoins	= worldBossFightRet->usergoldnum();
	}

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPTeamBuffInfoHandle, OPCODE_GET_TEAM_BUFF_INFORET_S)
{
	const OPTeamBuffInfoRet * teamBuffRet = dynamic_cast<const OPTeamBuffInfoRet*>(packet);
	if(teamBuffRet)
	{
		for(int i=0; i < teamBuffRet->teambuff_size(); ++i)
		{
			const OPTeamBuffInfoRet_TeamBuff& teamBuff = teamBuffRet->teambuff(i);
			TeamBuffInfo* teamBuffItem=ServerDateManager::Get()->getAndCreatTeamBuffById(teamBuff.teamid());
			teamBuffItem->teamId=teamBuff.teamid();
			teamBuffItem->teamLevel=teamBuff.teamlevel();
			teamBuffItem->currBoldness=teamBuff.currboldness();
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPUpgradeTeamBuffInfoHandle, OPCODE_UPGRADE_TEAM_BUFF_INFORET_S)
{
	const OPUpgradeTeamBuffInfoRet * upgradeRet = dynamic_cast<const OPUpgradeTeamBuffInfoRet*>(packet);
	if(upgradeRet)
	{
		for(int i=0;i<upgradeRet->battle_size();++i)
		{
			const OPUserBattleArrayRet_BattleArrayInfo infoBattle=upgradeRet->battle().Get(i);
			UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(infoBattle.id());
			userBattleInfo->id			=infoBattle.id();
			userBattleInfo->discipleId	=infoBattle.discipleid();
			userBattleInfo->attack		=infoBattle.attack();
			userBattleInfo->defence		=infoBattle.defence();
			userBattleInfo->health		=infoBattle.health();
			userBattleInfo->rp			=infoBattle.rp();
			userBattleInfo->position	=infoBattle.position();
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPChangeDiscipleHandle, OPCODE_CHANGE_DISCIPLERET_S)
{
 	const OPChangeDiscipleRet * changeDiscipleRet = dynamic_cast<const OPChangeDiscipleRet*>(packet);
	
	for(int index=0; index < changeDiscipleRet->battle_size(); ++index)
	{
		const OPUserBattleArrayRet_BattleArrayInfo& info = changeDiscipleRet->battle(index);
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(info.id());
		userBattleInfo->id			=info.id();
		userBattleInfo->discipleId	=info.discipleid();
		userBattleInfo->attack		=info.attack();
		userBattleInfo->defence		=info.defence();
		userBattleInfo->health		=info.health();
		userBattleInfo->rp			=info.rp();
		userBattleInfo->skill1		=info.skill1();
		userBattleInfo->skill2		=info.skill2();
		userBattleInfo->equip1		=info.equip1();
		userBattleInfo->equip2		=info.equip2();
		userBattleInfo->equip3		=info.equip3();
		userBattleInfo->position	=info.position();

		userBattleInfo->fateInfos.clear();
		for(int i=0; i < info.fates_size(); ++i)
		{
			const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
			userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
		}
	}
	MsgMainFrameChangePage msg;
	msg.pageName = "TeamPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPErrorReporterHandle, OPCODE_ERROR_RET_S)
{
	const OPErrorReporter * errorReporterRet = dynamic_cast<const OPErrorReporter*>(packet);
	PacketManager::Get()->_boardcastPacketError(errorReporterRet->errorid());
	//CCMessageBox((StringConverter::toString(errorReporterRet->errorid())+ ":" + errorReporterRet->errormsg()).c_str(),"error");
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPServerDisconnectHandle, OPCODE_SERVER_DISCONNECT_S)
{
	const OPServerDisConnectRet * serverDisconnect = dynamic_cast<const OPServerDisConnectRet*>(packet);
	if(serverDisconnect->disconnectid() == 1)
	{
		PacketManager::Get()->setNeedReConnect();
	}
	//PacketManager::Get()->_boardcastPacketError(errorReporterRet->errorid());
	//CCMessageBox((StringConverter::toString(errorReporterRet->errorid())+ ":" + errorReporterRet->errormsg()).c_str(),"error");
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPChangeBattlePositionHandle, OPCODE_CHANGE_BATTLE_POSITIONRET_S)
{
	const OPChangeBattlePositionRet * changeBattlePositionRet = dynamic_cast<const OPChangeBattlePositionRet*>(packet);
	for(int index=0; index < changeBattlePositionRet->positions_size(); ++index)
	{
		const PositionId& pID = changeBattlePositionRet->positions(index);
		UserBattleInfo* info = ServerDateManager::Get()->getAndCreatUserBattleInfo(pID.id());
		info->position = pID.position();
	}
	MsgMainFramePopPage msg;
	msg.pageName = "EmbattlePage";
	MessageManager::Get()->sendMessage(&msg);

	MsgMainFrameRefreshPage msg1;
	msg1.pageName = "TeamPage";
	MessageManager::Get()->sendMessage(&msg1);
}
DEFINE_CREATE_REGISTER_HANDLER_END
DEFINE_CREATE_REGISTER_HANDLER(OPCareerResetHandle, OPCODE_RESET_CAREER_COUNTRET_S)
{
	const OPResetCareerCountRet * ret = dynamic_cast<const OPResetCareerCountRet*>(packet);
	if(ret)
	{
		if(ret->status()==1)
		{
			UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
			info.goldcoins = ret->gold();
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@ResetCareerCountError"));
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(UserArenaInfoHandle,OPCODE_USER_ARENA_INFORET_S)
{
	const OPUserArenaInfoRet * userArenaInfoRet = dynamic_cast<const OPUserArenaInfoRet*>(packet);
	UserArenaInfo& userArenaInfo = ServerDateManager::Get()->getUserArenaInfo();
	userArenaInfo.arenaRank			= userArenaInfoRet->arenarank();			
	userArenaInfo.arenaScore		= userArenaInfoRet->arenascore();		
	userArenaInfo.arenaNum			= userArenaInfoRet->arenanum();			
	userArenaInfo.rewardScore		= userArenaInfoRet->rewardscore();		
	userArenaInfo.rewardRefreshTime	= userArenaInfoRet->rewardrefreshtime();

	for (int i = 0; i<userArenaInfoRet->rewardids_size(); i++)
	{
		userArenaInfo.rewardIDs[userArenaInfoRet->rewardids(i)] = userArenaInfoRet->rewardids(i);
	}
	for (int i = 0; i<userArenaInfoRet->gotrewardids_size(); i++)
	{
		userArenaInfo.gotRewardIDs[userArenaInfoRet->gotrewardids(i)] = userArenaInfoRet->gotrewardids(i);
	}
	BlackBoard::Get()->loseArenaData = 1;
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(GetArenaRewardHandle,OPCODE_GET_ARENA_REWARDRET_S)
{
	const OPGetArenaRewardRet * getArenaReward = dynamic_cast<const OPGetArenaRewardRet*>(packet);
	UserArenaInfo& userArenaInfo = ServerDateManager::Get()->getUserArenaInfo();		
	userArenaInfo.arenaScore		= getArenaReward->arenascore();		

	for (int i = 0; i<getArenaReward->rewardids_size(); i++)
	{
		userArenaInfo.rewardIDs[getArenaReward->rewardids(i)] = getArenaReward->rewardids(i);
	}
	for (int i = 0; i<getArenaReward->gotrewardids_size(); i++)
	{
		userArenaInfo.gotRewardIDs[getArenaReward->gotrewardids(i)] = getArenaReward->gotrewardids(i);
	}
	GetPropManager::Get()->gotTool(getArenaReward->reward());

	unsigned int id = GET_V_UINT("ArenaRewardID");
	ArenaReward arenaReward(id);
	if(arenaReward.type() == 1)
	{
		MsgMainFramePushPage msg;
		msg.pageName = "ArenaRewardAniPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else
	{
		UserArenaInfo& userArenaInfo = ServerDateManager::Get()->getUserArenaInfo();
		userArenaInfo.rewardIDs.erase(id);

		MsgMainFramePushPage msg;
		msg.pageName = "ArenaRewardPage";
		MessageManager::Get()->sendMessage(&msg);
	}

	MsgMainFrameRefreshPage msg;
	msg.pageName = "ArenaPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(RefreshArenaScoreHandle,OPCODE_REFRESH_ARENARET_S)
{
	const OPRefreshArenaScoreRet * getArenaReward = dynamic_cast<const OPRefreshArenaScoreRet*>(packet);
	UserArenaInfo& userArenaInfo = ServerDateManager::Get()->getUserArenaInfo();		
	userArenaInfo.arenaScore		= getArenaReward->arenascore();		

// 	MsgMainFrameRefreshPage msg;
// 	msg.pageName = "ArenaPage";
// 	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(GetArenaOpponentListHandle,OPCODE_GET_ARENA_OPPONENT_LISTRET_S)
{
	const OPGetArenaOpponentListRet * getArenaReward = dynamic_cast<const OPGetArenaOpponentListRet*>(packet);

	ServerDateManager::Get()->clearAllArenaOpponentInfoList();

	for(int index=0; index < getArenaReward->arenaopponents_size(); ++index)
	{
		const OPGetArenaOpponentListRet_ArenaOpponentInfo& info = getArenaReward->arenaopponents(index);
		ArenaOpponentInfo * arenaOpponentInfo = ServerDateManager::Get()->getAndCreatArenaOpponentInfo(info.id());
		
		arenaOpponentInfo->id = info.id();
		arenaOpponentInfo->name = info.name();
		arenaOpponentInfo->level = info.level();
		arenaOpponentInfo->arenaRank = info.arenarank();
		arenaOpponentInfo->rewardScore =info.rewardscore();
		arenaOpponentInfo->arenaType = info.arenatype();
		
		int size = info.dicipleitemid_size();
		if(size == 1)
		{
			arenaOpponentInfo->dicipleItemID1 = info.dicipleitemid(0);
			arenaOpponentInfo->dicipleItemID2 = 0;
			arenaOpponentInfo->dicipleItemID3 = 0;
		}
		else if(size == 2)
		{
			arenaOpponentInfo->dicipleItemID1 = info.dicipleitemid(0);
			arenaOpponentInfo->dicipleItemID2 = info.dicipleitemid(1);
			arenaOpponentInfo->dicipleItemID3 = 0;
		}
		else if(size >2)
		{
			arenaOpponentInfo->dicipleItemID1 = info.dicipleitemid(0);
			arenaOpponentInfo->dicipleItemID2 = info.dicipleitemid(1);
			arenaOpponentInfo->dicipleItemID3 = info.dicipleitemid(2);
		}
		else
		{
			arenaOpponentInfo->dicipleItemID1 = 0;
			arenaOpponentInfo->dicipleItemID2 = 0;
			arenaOpponentInfo->dicipleItemID3 = 0;
		}
	}

	MsgMainFrameRefreshPage msg;
	msg.pageName = "ArenaPage";
	MessageManager::Get()->sendMessage(&msg);
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(ChallengeOpponentHandle,OPCODE_CHALLENGE_OPPONENTRET_S)
{
	const OPChallengeOpponentRet * callengeOpponentRet = dynamic_cast<const OPChallengeOpponentRet*>(packet);
	bool state = callengeOpponentRet->status();
	if(state)
	{
// 		UserArenaInfo& userArenaInfo = ServerDateManager::Get()->getUserArenaInfo();
// 		userArenaInfo.arenaRank			= callengeOpponentRet->arenarank();			
// 		userArenaInfo.arenaScore		= callengeOpponentRet->arenascore();		
// 		userArenaInfo.arenaNum			= callengeOpponentRet->arenanum();			
// 		userArenaInfo.rewardScore		= callengeOpponentRet->rewardscore();		
// 		userArenaInfo.rewardRefreshTime	= callengeOpponentRet->rewardrefreshtime();
		ServerDateManager::Get()->getUserBasicInfo().vitality = callengeOpponentRet->vitality();

		OPUserArenaInfo userArenaInfo;
		userArenaInfo.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_USER_ARENA_INFO_C,&userArenaInfo);

		if(callengeOpponentRet->has_userbattleret())
		{
			const OPUserBattleRet& battle = callengeOpponentRet->userbattleret();
			BasePage* bp = CCBManager::Get()->getPage("FightPage");
			if(bp)
			{
				FightPage* fp = dynamic_cast<FightPage*>(bp);
				fp->setMessage(&battle);
				fp->setFightType(ArenaBattle);
			}
			MsgMainFrameCoverShow msg;
			msg.pageName = "FightPage";
			MessageManager::Get()->sendMessage(&msg);
			if(battle.result().winlose() <= 2)
			{
				OPGetArenaOpponentList getArenaOpponentList;
				getArenaOpponentList.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_ARENA_OPPONENT_LIST_C,&getArenaOpponentList);
			}
		}
	}
	else
	{
		MSG_BOX_LAN("@Opponent_rank_changed")
	}

}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPUserLevelUpHandle,OPCODE_USER_UPGRADE_S)
{
	const OPUserUpgrade * ret = dynamic_cast<const OPUserUpgrade*>(packet);
	if(ret)
	{
		UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
		info.goldcoins+=ret->addgold();
		info.silvercoins+=ret->addsilvercoins();
		if(ret->has_level())
		{
			info.level=ret->level();
		}
		if(ret->has_exp())
		{
			//info.exp=ret->exp();
		}
		if(ret->has_newbattle())
		{
			const OPUserBattleArrayRet_BattleArrayInfo& infoBattle = ret->newbattle();
			UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(infoBattle.id());
			userBattleInfo->id			=infoBattle.id();
			userBattleInfo->discipleId	=infoBattle.discipleid();
			userBattleInfo->attack		=infoBattle.attack();
			userBattleInfo->defence		=infoBattle.defence();
			userBattleInfo->health		=infoBattle.health();
			userBattleInfo->rp			=infoBattle.rp();
			userBattleInfo->skill1		=infoBattle.skill1();
			userBattleInfo->skill2		=infoBattle.skill2();
			userBattleInfo->equip1		=infoBattle.equip1();
			userBattleInfo->equip2		=infoBattle.equip2();
			userBattleInfo->equip3		=infoBattle.equip3();
			userBattleInfo->position	=infoBattle.position();

			userBattleInfo->fateInfos.clear();
			for(int i=0; i < infoBattle.fates_size(); ++i)
			{
				const OPUserBattleArrayRet_FateInfo& fateInfo = infoBattle.fates(i);
				userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPUserGMHandle,OPCODE_UPDATE_USERINFO_BY_GM_RET_S)
{
	const OPUpdateUserInfoByGMRet * ret = dynamic_cast<const OPUpdateUserInfoByGMRet*>(packet);
	if(ret)
	{
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		//TODO Add float message "Add power/vitality/chanllengeTimes success!";
		if(ret->has_power())
		{
			userBasicInfo.power = ret->power(); 
		}
		if(ret->has_vitality())
		{
			userBasicInfo.vitality = ret->vitality();
		}
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_level())
		{
			userBasicInfo.level = ret->level();
		}
		if(ret->has_viplevel())
		{
			userBasicInfo.viplevel=ret->viplevel();
		}
		if(ret->has_challengetimes())
		{
			ServerDateManager::Get()->getUserArenaInfo().arenaNum = ret->challengetimes();
			std::string s = Language::Get()->getString("@AddChallengetimes");
			char buff[128] = {0};
			sprintf(buff , s.c_str() , ret->challengetimes());
			MSG_BOX(buff );
		}
		if(ret->has_goldcoins())
		{
			userBasicInfo.goldcoins = ret->goldcoins();
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
		if(ret->soulinfo_size()>0)
		{
			for(int index=0; index < ret->soulinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSoul(ret->soulinfo(index));		
			}
		}
		if(ret->skillinfo_size()>0)
		{
			for(int index=0; index < ret->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
			}
		}
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->discipleinfo_size()>0)
		{
			for(int index=0; index < ret->discipleinfo_size(); ++index)
			{
				const OPUserDiscipleInfoRet_DiscipleInfo& info = ret->discipleinfo(index);
				UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(info.id());
				userDiscipleInfo->itemid		= info.itemid();
				userDiscipleInfo->level			= info.level();
				userDiscipleInfo->exp			= info.exp();
				userDiscipleInfo->health		= info.health();
				userDiscipleInfo->attack		= info.attack();
				userDiscipleInfo->defence		= info.defence();
				userDiscipleInfo->rp			= info.rp();
				userDiscipleInfo->upgradelevel	= info.upgradelevel();
				userDiscipleInfo->potentiality	= info.potentiality();
				userDiscipleInfo->skill1		= info.skill1();
				userDiscipleInfo->battleid		= info.battleid();
			}
		}
		
		MSG_BOX(Language::Get()->getString("@GMOprRet"));
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPAddVitalityHandler,OPCODE_ADD_VITALITYRET_S)
{
	const  OPAddVitalityRet* ret = dynamic_cast<const OPAddVitalityRet*>(packet);
	if(ret&&ret->status()==301)
	{
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		userBasicInfo.vitality=ret->vitality();
		userBasicInfo.todayBuyVitalityNum += 1;//add by xinghui
		userBasicInfo.goldcoins = userBasicInfo.goldcoins-(ToolTableManager::Get()->getToolItemByID(JISUZHEN)->price*userBasicInfo.todayBuyVitalityNum);//add by xinghui
		userBasicInfo.todayAddedVitalityNum +=1;
		std::string s = Language::Get()->getString("@AddVitalitys");
		char buff[128] = {0};
		sprintf(buff , s.c_str() , userBasicInfo.vitality);
		MSG_BOX(buff );
	}
}
 DEFINE_CREATE_REGISTER_HANDLER_END

//NEW_DEFINE_CREATE_REGISTER_HANDLER(OPAddVitalityHandler,OPCODE_ADD_VITALITYRET_S)
//{
//	const  OPAddVitalityRet* ret = dynamic_cast<const OPAddVitalityRet*>(packet);
//	if(ret&&ret->status()==301)
//	{
//		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
//		userBasicInfo.vitality=ret->vitality();
//		userBasicInfo.goldcoins = userBasicInfo.goldcoins-ToolTableManager::Get()->getToolItemByID(JISUZHEN)->price;
//		userBasicInfo.todayAddedVitalityNum +=1;
//	}
//}
//SEND_FAILED_HNDLER
//{
//
//}
//CONNECT_FAILED_HNDLER
//{
//
//}
//TIMEOUT_HNDLER
//{
//
//}
//NEW_DEFINE_CREATE_REGISTER_HANDLER_END(OPAddVitalityHandler)

DEFINE_CREATE_REGISTER_HANDLER(OPAddPowerCareerHandle,OPCODE_ADDPOWER_CAREERRET_S)
{
	const  OPAddPowerCareerRet* ret = dynamic_cast<const OPAddPowerCareerRet*>(packet);
	if(ret)
	{
		if(ret->status()==0)
		{
			UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
			userBasicInfo.power=ret->power();
		}
		else
		{

		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END
DEFINE_CREATE_REGISTER_HANDLER(OPAddArenaNumHandler,OPCODE_ADD_ARENA_NUMRET_S)
{
	const  OPAddArenaNumRet* ret = dynamic_cast<const OPAddArenaNumRet*>(packet);
	if(ret&&ret->status()==301)
	{
		UserArenaInfo& arenaInfo = ServerDateManager::Get()->getUserArenaInfo();
		arenaInfo.arenaNum=ret->arenanum();
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		userBasicInfo.goldcoins = userBasicInfo.goldcoins-ToolTableManager::Get()->getToolItemByID(TIAOZHANSHU)->price;
		std::string s = Language::Get()->getString("@AddChallengetimes");
		char buff[128] = {0};
		sprintf(buff , s.c_str() , arenaInfo.arenaNum);
		MSG_BOX(buff );
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPChangeNameHandler,OPCODE_CHANE_NAMERET_S)
{
	const  OPChangeNameRet* ret = dynamic_cast<const OPChangeNameRet*>(packet);
	if(ret)
	{
		bool changeOK=false;
		if(ret->status()==1)
		{
			MSG_BOX(Language::Get()->getString("@ChangeNameToolEnough"));
		}
		else if(ret->status()==2)
		{
			MSG_BOX(Language::Get()->getString("@ChangeNameRepeat"));
		}
		else
		{
			changeOK=true;
			if(ret->toolinfo_size())
			{
				for(int index=0; index < ret->toolinfo_size(); ++index)
				{
					GetPropManager::Get()->gotTool(ret->toolinfo(index));		
				}
			}
			MsgMainFrameRefreshPage refMsg;
			refMsg.pageName = "PackagePage";
			MessageManager::Get()->sendMessage(&refMsg);
		}
		std::string key="changeNameStatus";
		if(BlackBoard::Get()->hasVarible(key))
		{
			BlackBoard::Get()->setVarible(key,changeOK);
		}
		else
		{
			BlackBoard::Get()->addVarible(key,changeOK);
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END
DEFINE_CREATE_REGISTER_HANDLER(OPUpdateTitleHandle,OPCODE_UPDATE_TITLE_S)
{
	const OPUpdateTitle * ret = dynamic_cast<const OPUpdateTitle*>(packet);
	if(ret)
	{
		ServerDateManager::Get()->getUserBasicInfo().titleTotalNum = ret->titletotalnum();
		TitleInfo* titleInfo = ServerDateManager::Get()->getAndCreatTitleInfo(ret->titleid() / 1000);
		
		if(titleInfo->itemid>0)
		{
			BlackBoard::Get()->PreTitleId = titleInfo->itemid;
		}
		titleInfo->itemid = ret->titleid();
		BlackBoard::Get()->UpdateTitleType = ret->titleid()/1000;
		if(BlackBoard::Get()->UpdateTitleType!=1&&BlackBoard::Get()->UpdateTitleType!=3)
		{
			if(BlackBoard::Get()->PreTitleId==0)
			{
				MsgMainFramePushPage msg;
				msg.pageName = "TitleGetNewPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{
				MsgMainFramePushPage msg;
				msg.pageName = "TitleLVUpPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPRewardAdventureContinueHandle,OPCODE_ADVENTURE_CONTINUELOGIN_REWARDRET_S)
{
	const OPRewardAdventureContinueLoginRet * ret = dynamic_cast<const OPRewardAdventureContinueLoginRet*>(packet);
	if(ret)
	{
		UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
		info.mContinueLoginInfo=ret->loginstatus();
		int rewardSize = ret->reward_size();
		if(rewardSize>0)
		{
			for(int index=0; index < rewardSize; ++index)
			{
				const OPRewardInfoRet_RewardInfo &info = ret->reward(index);
				RewardData * rewardInfo = ServerDateManager::Get()->getAndCreateRewardData(index);
				rewardInfo->count = info.count();
				rewardInfo->itemId = info.itemid();
				rewardInfo->itemType = info.itemtype();
			}
			ServerDateManager::Get()->rewardDataToResInfo();
		}

		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(ret->has_power())
		{
			userBasicInfo.power = ret->power();
		}
		if(ret->has_vitality())
		{
			userBasicInfo.vitality = ret->vitality();
		}
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_challengetimes())
		{
			ServerDateManager::Get()->getUserArenaInfo().arenaNum = ret->challengetimes();
			std::string s = Language::Get()->getString("@AddChallengetimes");
			char buff[128] = {0};
			sprintf(buff , s.c_str() , ServerDateManager::Get()->getUserArenaInfo().arenaNum);
			MSG_BOX(buff );
		}
		if(ret->has_goldcoins())
		{
			userBasicInfo.goldcoins = ret->goldcoins();
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
		if(ret->soulinfo_size()>0)
		{
			for(int index=0; index < ret->soulinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSoul(ret->soulinfo(index));		
			}
		}
		if(ret->skillinfo_size()>0)
		{
			for(int index=0; index < ret->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
			}
		}
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->disciple_size())
		{
			for(int index=0; index < ret->disciple_size(); ++index)
			{
				GetPropManager::Get()->gotDisciple(ret->disciple(index));		
			}
		}
		MsgMainFramePushPage msg;
		msg.pageName = "PackPreviewPage";
		MessageManager::Get()->sendMessage(&msg);
		MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
		packMsg->index=OPEN_TREASURE_REWARD;
		MessageManager::Get()->sendMessage(packMsg);
	}
	
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetAdventureContinueHandle,OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFORET_S)
{
	const  OPGetAdventureContinueLoginRet* ret = dynamic_cast<const OPGetAdventureContinueLoginRet*>(packet);
	if(ret)
	{
		UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
		info.mContinueLoginInfo=ret->loginstatus();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPRecordReadMsgTimeHandle,OPCODE_RECORDREADMSGTIMERET_S)
{
	const  OPRecordReadMsgTimeRet* ret = dynamic_cast<const OPRecordReadMsgTimeRet*>(packet);
	if(ret)
	{
		UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
		info.readTime=ret->servertime();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPCdkeyRewardRetHandle,OPCODE_CDKEY_REWARDRET_S)
{
	const OPCdkeyRewardRet * ret = dynamic_cast<const OPCdkeyRewardRet*>(packet);
	if(ret)
	{
		bool _isOk=false;
		if(ret->status()==1)
		{
			_isOk=true;
			MSG_BOX(Language::Get()->getString("@CDKeyRetSuccess"));
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@CDKeyRetError"));
		}
		int rewardSize = ret->reward_size();
		if(rewardSize>0)
		{
			for(int index=0; index < rewardSize; ++index)
			{
				const OPRewardInfoRet_RewardInfo &info = ret->reward(index);
				RewardData * rewardInfo = ServerDateManager::Get()->getAndCreateRewardData(index);
				rewardInfo->count = info.count();
				rewardInfo->itemId = info.itemid();
				rewardInfo->itemType = info.itemtype();
			}
			ServerDateManager::Get()->rewardDataToResInfo();
		}

		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(ret->has_power())
		{
			userBasicInfo.power = ret->power();
		}
		if(ret->has_vitality())
		{
			userBasicInfo.vitality = ret->vitality();
		}
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_challengetimes())
		{
			ServerDateManager::Get()->getUserArenaInfo().arenaNum = ret->challengetimes();
			std::string s = Language::Get()->getString("@AddChallengetimes");
			char buff[128] = {0};
			sprintf(buff , s.c_str() , ServerDateManager::Get()->getUserArenaInfo().arenaNum);
			MSG_BOX(buff );
		}
		if(ret->has_goldcoins())
		{
			userBasicInfo.goldcoins = ret->goldcoins();
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
		if(ret->soulinfo_size()>0)
		{
			for(int index=0; index < ret->soulinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSoul(ret->soulinfo(index));		
			}
		}
		if(ret->skillinfo_size()>0)
		{
			for(int index=0; index < ret->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
			}
		}
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->disciple_size())
		{
			for(int index=0; index < ret->disciple_size(); ++index)
			{
				GetPropManager::Get()->gotDisciple(ret->disciple(index));		
			}
		}
		if(_isOk)
		{
			MsgMainFramePushPage msg;
			msg.pageName = "PackPreviewPage";
			MessageManager::Get()->sendMessage(&msg);
			MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
			packMsg->index=OPEN_TREASURE_REWARD;
			MessageManager::Get()->sendMessage(packMsg);
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGuideBuyToolHandle,OPCODE_GUIDE_BUY_TOOLRET_S)
{
	const OPGuideBuyToolRet * ret = dynamic_cast<const OPGuideBuyToolRet*>(packet);
	if(ret)
	{
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(ret->has_exp())
		{
			userBasicInfo.exp = ret->exp();
		}
		if(ret->has_goldcoins())
		{
			userBasicInfo.goldcoins = ret->goldcoins();
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
		if(ret->skillinfo_size()>0)
		{
			for(int index=0; index < ret->skillinfo_size(); ++index)
			{
				GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
			}
		}
		bool isEquip=false;
		int id=0;
		if(ret->equipinfo_size())
		{
			isEquip=true;
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				id=ret->equipinfo(index).id();
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(isEquip)
		{
			BlackBoard::Get()->ShowEquip = id;
			MsgMainFramePushPage msg;
			msg.pageName = "EquipSurprisePage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGuideRecordRetHandle,OPCODE_GUIDE_RECORDRET_S)
{
	const OPGuideRecordRet * ret = dynamic_cast<const OPGuideRecordRet*>(packet);
	if(ret)
	{

	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPAddInviteKeyHandle,OPCODE_ADD_INVITEKEYRET_S)
{
	const OPAddInviteKeyRet * ret = dynamic_cast<const OPAddInviteKeyRet*>(packet);
	if(ret)
	{
		if(ret->status()==1)
		{
			if(ret->toolinfo_size())
			{
				for(int index=0; index < ret->toolinfo_size(); ++index)
				{
					GetPropManager::Get()->gotTool(ret->toolinfo(index));		
				}
			}
			MsgMainFramePopPage msgPop;
			msgPop.pageName="CDKeyInvitePage";
			MessageManager::Get()->sendMessage(&msgPop);
			MSG_BOX(Language::Get()->getString("@InviteKeySuccess"));
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@InviteKeyError"));
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPGetInviteRewardHandle,OPCODE_GET_INVITE_REWARDRET_S)
{
	const OPGetInviteRewardRet * ret = dynamic_cast<const OPGetInviteRewardRet*>(packet);
	if(ret)
	{
		if(ret->status()!=0)
		{
			UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
			info.mInviteRewardStatus=ret->rewardstatus();
			int rewardSize = ret->reward_size();
			if(rewardSize>0)
			{
				for(int index=0; index < rewardSize; ++index)
				{
					const OPRewardInfoRet_RewardInfo &info = ret->reward(index);
					RewardData * rewardInfo = ServerDateManager::Get()->getAndCreateRewardData(index);
					rewardInfo->count = info.count();
					rewardInfo->itemId = info.itemid();
					rewardInfo->itemType = info.itemtype();
				}
				ServerDateManager::Get()->rewardDataToResInfo();
			}

			UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
			if(ret->has_power())
			{
				userBasicInfo.power = ret->power();
			}
			if(ret->has_vitality())
			{
				userBasicInfo.vitality = ret->vitality();
			}
			if(ret->has_exp())
			{
				userBasicInfo.exp = ret->exp();
			}
			if(ret->has_exp())
			{
				userBasicInfo.exp = ret->exp();
			}
			if(ret->has_challengetimes())
			{
				ServerDateManager::Get()->getUserArenaInfo().arenaNum = ret->challengetimes();
				std::string s = Language::Get()->getString("@AddChallengetimes");
				char buff[128] = {0};
				sprintf(buff , s.c_str() , ServerDateManager::Get()->getUserArenaInfo().arenaNum);
				MSG_BOX(buff );
			}
			if(ret->has_goldcoins())
			{
				userBasicInfo.goldcoins = ret->goldcoins();
			}
			if(ret->has_silvercoins())
			{
				userBasicInfo.silvercoins = ret->silvercoins();
			}
			if(ret->soulinfo_size()>0)
			{
				for(int index=0; index < ret->soulinfo_size(); ++index)
				{
					GetPropManager::Get()->gotSoul(ret->soulinfo(index));		
				}
			}
			if(ret->skillinfo_size()>0)
			{
				for(int index=0; index < ret->skillinfo_size(); ++index)
				{
					GetPropManager::Get()->gotSkill(ret->skillinfo(index));		
				}
			}
			if(ret->equipinfo_size())
			{
				for(int index=0; index < ret->equipinfo_size(); ++index)
				{
					GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
				}
			}
			if(ret->toolinfo_size())
			{
				for(int index=0; index < ret->toolinfo_size(); ++index)
				{
					GetPropManager::Get()->gotTool(ret->toolinfo(index));		
				}
			}
			if(ret->disciple_size())
			{
				for(int index=0; index < ret->disciple_size(); ++index)
				{
					GetPropManager::Get()->gotDisciple(ret->disciple(index));		
				}
			}
			MsgMainFramePushPage msg;
			msg.pageName = "PackPreviewPage";
			MessageManager::Get()->sendMessage(&msg);
			MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
			packMsg->index=OPEN_TREASURE_REWARD;
			MessageManager::Get()->sendMessage(packMsg);
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@InviteRewardError"));
		}
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPUserInviteStatusHandle,OPCODE_USER_INVITESTATUSRET_S)
{
	const  OPUserInviteStatusRet* ret = dynamic_cast<const OPUserInviteStatusRet*>(packet);
	if(ret)
	{
		UserAdventureDataInfo& info=ServerDateManager::Get()->getUserAdventureDataInfo();
		info.mInviteKey=ret->invitekey();
		info.mInviteStatus=ret->invitestatus();
		info.mInviteRewardStatus=ret->rewardstatus();
	}
}
DEFINE_CREATE_REGISTER_HANDLER_END

class OPHeartbeatHandle : public PacketHandlerGeneral, public libOSListener
{ 
public: 
	static const int mOpcode = OPCODE_HEARTBEATRET_S; 
	std::vector<unsigned int> serverStopTimes;
	OPHeartbeatHandle() 
	{
		PacketManager::Get()->regisiterPacketHandler(mOpcode,this); 
		serverStopTimes.clear();
	} 
	virtual void onMessageboxEnter(lib91*, int tag)
	{
		if(tag == OPCODE_HEARTBEATRET_S)
		{
			exit(0);
		}
// 		else if(tag == OPCODE_HEARTBEATRET_S +1)
// 		{
// 			lib91::getInstance()->removeListener(this);
// 		}
	}
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet)
	{
		const  OPHeartbeatRet* ret = dynamic_cast<const OPHeartbeatRet*>(packet);
		if(ret)
		{
			ServerDateManager::Get()->getUserBasicInfo().servertime=ret->servertime();
			GamePrecedure::Get()->setServerTime(ret->servertime());
		}

		if(serverStopTimes.empty())
		{
			std::vector<std::string> vectring = GameMaths::tokenize(VaribleManager::Get()->getSetting("ServerStopTime")," ");
			std::vector<std::string>::iterator itr = vectring.begin();
			while(itr!=vectring.end())
			{
				serverStopTimes.push_back(StringConverter::parseUnsignedInt(*itr));
				itr++;
			}
			serverStopTimes.push_back(0);
			std::sort(serverStopTimes.begin(),serverStopTimes.end());
		}

		if(ret->has_server_stop_time())
		{
			libOS::getInstance()->registerListener(this);
			if(ret->server_stop_time() == 0)
			{
				unsigned int reopentime = ret->server_reopen_time();
				std::string lan = Language::Get()->getString("@ServerStopMSG");
				char msg[256];
				sprintf(msg,lan.c_str(),reopentime);
				libOS::getInstance()->showMessagebox(msg,OPCODE_HEARTBEATRET_S);
			}
			else
			{
				unsigned int stoptime = ret->server_stop_time();
				unsigned int temptime = serverStopTimes[serverStopTimes.size()-1];
				if(stoptime < temptime)
				{
					do 
					{
						serverStopTimes.pop_back();
					} 
					while (stoptime < serverStopTimes[serverStopTimes.size()-1]);
					
					stoptime = ceil((float)stoptime/60.0f);
					unsigned int reopentime = ret->server_reopen_time();
					std::string lan = Language::Get()->getString("@ServerStopAfterMSG");
					char msg[256];
					sprintf(msg,lan.c_str(),stoptime,reopentime);
					libOS::getInstance()->showMessagebox(msg/*,OPCODE_HEARTBEATRET_S+1*/);
				}
			}
		}
	}
	virtual void onSendPacketFailed(const int opcode){};
	virtual void onConnectFailed(std::string ip, int port){}
private: 
	virtual int _getOpcode() { return mOpcode;};
}; 
OPHeartbeatHandle * _handler_OPHeartbeatHandle = new OPHeartbeatHandle; 

// DEFINE_CREATE_REGISTER_HANDLER(OPHeartbeatHandle,OPCODE_HEARTBEATRET_S)
// {
// 	
// 	const  OPHeartbeatRet* ret = dynamic_cast<const OPHeartbeatRet*>(packet);
// 	if(ret)
// 	{
// 		ServerDateManager::Get()->getUserBasicInfo().servertime=ret->servertime();
// 		GamePrecedure::Get()->setServerTime(ret->servertime());
// 	}
// 
// 	static std::vector<unsigned int> serverStopTimes;
// 	if(serverStopTimes.empty())
// 	{
// 		std::vector<std::string> vectring = GameMaths::tokenize(VaribleManager::Get()->getSetting("ServerStopTime")," ");
// 		std::vector<std::string>::iterator itr = vectring.begin();
// 		while(itr!=vectring.end())
// 		{
// 			serverStopTimes.push_back(StringConverter::parseUnsignedInt(*itr));
// 			itr++;
// 		}
// 		serverStopTimes.push_back(0);
// 		std::sort(serverStopTimes.begin(),serverStopTimes.end());
// 	}
// 
// 	if(ret->has_serverstoptime())
// 	{
// 		//有停服信息
// 		if(ret->serverstoptime() == 0)
// 		{
// 			//时间到
// 		}
// 		else
// 		{
// 			//时间未
// 		}
// 	}
// 	
// }
// DEFINE_CREATE_REGISTER_HANDLER_END

DEFINE_CREATE_REGISTER_HANDLER(OPChargeRebateHandle,OPCODE_CHARGE_REBATERET_S)
{
	const  OPChargeRebateRet* ret = dynamic_cast<const OPChargeRebateRet*>(packet);
	if(ret)
	{
		ChargeRebateItem& chargeRebateItem = ServerDateManager::Get()->getChargeRebateItem();
		chargeRebateItem.id = 1;
		chargeRebateItem.servertime = ret->servertime();
		chargeRebateItem.chargeType = ret->chargetype();
		chargeRebateItem.recurittime = ret->recurittime();
		chargeRebateItem.minRecharge = ret->minrecharge();
		chargeRebateItem.maxRecharge = ret->maxrecharge();
		chargeRebateItem.refund = ret->refund();
		chargeRebateItem.rechargeInfact = ret->rechargeinfact();
		chargeRebateItem.rebateBeginTime = ret->rebatebegintime();
		chargeRebateItem.rebateEndTime = ret->rebateendtime();
		chargeRebateItem.getRewardToday = ret->getrewardtoday();
		chargeRebateItem.exchangeRate = ret->exchangerate();
		chargeRebateItem.userGoldCoins = ret->usergoldcoins();
		chargeRebateItem.getRewardStat = ret->getrewardstat(); 
		chargeRebateItem.refundGoldCoins = ret->refundgoldcoins();
		chargeRebateItem.rebateDuringDays = ret->rebateduringdays();
   		TimeCalculator::Get()->createTimeCalcultor("chargerebate_recurittime",chargeRebateItem.recurittime);


		if(chargeRebateItem.getRewardStat == 1){

			const UserPropertyItem * userPropertyItem = UserPropertyTableManager::Get()->getUserPropertyItemByID(1001);
			ResInfo * resInfo = ServerDateManager::Get()->getAndCreateResInfo(1);
			resInfo->count = chargeRebateItem.refundGoldCoins;
			resInfo->name = userPropertyItem->name;
			resInfo->icon = userPropertyItem->iconPic;

			MsgMainFramePushPage msg;
			msg.pageName = "PackPreviewPage";
			MessageManager::Get()->sendMessage(&msg);
			MsgPackPreviewSourceMsg* packMsg=new MsgPackPreviewSourceMsg;
			packMsg->index=OPEN_TREASURE_REWARD;
			MessageManager::Get()->sendMessage(packMsg);

			UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
			std::string basicSyncKey="basicSyncKey";
			bool isSync=false;
			if(BlackBoard::Get()->hasVarible(basicSyncKey))
			{
				isSync=BlackBoard::Get()->getVaribleAsBool(basicSyncKey);
			}
			if(!isSync)
			{
				userBasicInfo.goldcoins	= chargeRebateItem.userGoldCoins;
			}
		}

	}
}
DEFINE_CREATE_REGISTER_HANDLER_END


				
DEFINE_CREATE_REGISTER_HANDLER(OPFortuneComeHandle,OPCODE_FORTUNE_COMERET_S)
{
	const OPFortuneComeRet* ret = dynamic_cast<const OPFortuneComeRet*>(packet);
	if(ret){
		FortuneComeItem& fortuneComeItem = ServerDateManager::Get()->getFortuneComeItem();
		fortuneComeItem.servertime = ret->servertime();
		fortuneComeItem.lastDays = ret->lastdays();
		fortuneComeItem.chanceNum = ret->chancenum();
		fortuneComeItem.recurittime = ret->recurittime();
		fortuneComeItem.needGold = ret->needgold();
		fortuneComeItem.userGold = ret->usergold();
		fortuneComeItem.rewardGold = ret->rewardgold();
		fortuneComeItem.rewardStat = ret->rewardstat();
		fortuneComeItem.costGold = ret->costgold();

		TimeCalculator::Get()->createTimeCalcultor("fortunecome_recurittime",fortuneComeItem.recurittime);

		if(fortuneComeItem.rewardStat == 1){
			BlackBoard::Get()->fortuneComeFlag = 0;
			UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
			std::string basicSyncKey="basicSyncKey";
			bool isSync=false;
			if(BlackBoard::Get()->hasVarible(basicSyncKey))
			{
				isSync=BlackBoard::Get()->getVaribleAsBool(basicSyncKey);
			}
			if(!isSync)
			{
				userBasicInfo.goldcoins	= fortuneComeItem.userGold;
			}

		}


	}

}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPShowUserBattleArrayHandle,OPCODE_SHOW_USER_BATTLERET_S)
{
	const OPShowUserBattleArrayRet* showBattleRet=dynamic_cast<const OPShowUserBattleArrayRet*>(packet);
	if(showBattleRet)
	{
		ViewTeamBattleInfoItem& _teamBattleInfo=ServerDateManager::Get()->getViewTeamBattleInfoItem();
		_teamBattleInfo.clearTeamBattleInfoItem();
		_teamBattleInfo.mPlayerName=showBattleRet->playername();
		_teamBattleInfo.mPlayerLevel=showBattleRet->playerlevel();
		if(showBattleRet->has_titlenum())
		{
			_teamBattleInfo.mPlayerTitleNum=showBattleRet->titlenum();
		}
		//battle
		const OPUserBattleArrayRet * userBattleArrayRet = dynamic_cast<const OPUserBattleArrayRet*>(&showBattleRet->userbattlearray());
		for(int index=0; index < userBattleArrayRet->battle_size(); ++index)
		{
			const OPUserBattleArrayRet_BattleArrayInfo& info = userBattleArrayRet->battle(index);
			UserBattleInfo* userBattleInfo = _teamBattleInfo.getAndCreatUserBattleInfo(info.id());
			userBattleInfo->id			=info.id();
			userBattleInfo->discipleId	=info.discipleid();
			userBattleInfo->attack		=info.attack();
			userBattleInfo->defence		=info.defence();
			userBattleInfo->health		=info.health();
			userBattleInfo->rp			=info.rp();
			userBattleInfo->skill1		=info.skill1();
			userBattleInfo->skill2		=info.skill2();
			userBattleInfo->equip1		=info.equip1();
			userBattleInfo->equip2		=info.equip2();
			userBattleInfo->equip3		=info.equip3();
			userBattleInfo->position	=info.position();

			userBattleInfo->fateInfos.clear();
			for(int i=0; i < info.fates_size(); ++i)
			{
				const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
				userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
			}
		}

		//disciple
		if(showBattleRet->has_userdisciple())
		{
			const OPUserDiscipleInfoRet * userDiscipleInfoRet = dynamic_cast<const OPUserDiscipleInfoRet*>(&showBattleRet->userdisciple());

			for(int index=0; index < userDiscipleInfoRet->disciple_size(); ++index)
			{
				const OPUserDiscipleInfoRet_DiscipleInfo& info = userDiscipleInfoRet->disciple(index);
				UserDiscipleInfo * userDiscipleInfo = _teamBattleInfo.getAndCreatDiscipleInfo(info.id());
				userDiscipleInfo->itemid		= info.itemid();
				userDiscipleInfo->level			= info.level();
				userDiscipleInfo->exp			= info.exp();
				userDiscipleInfo->health		= info.health();
				userDiscipleInfo->attack		= info.attack();
				userDiscipleInfo->defence		= info.defence();
				userDiscipleInfo->rp			= info.rp();
				userDiscipleInfo->upgradelevel	= info.upgradelevel();
				userDiscipleInfo->potentiality	= info.potentiality();
				userDiscipleInfo->skill1		= info.skill1();
				userDiscipleInfo->battleid		= info.battleid();
				userDiscipleInfo->preLevel      = info.level();
			}
		}
		
		//equip
		if(showBattleRet->has_userequip())
		{
			const OPUserEquipInfoRet * userEquipInfoRet = dynamic_cast<const OPUserEquipInfoRet*>(&showBattleRet->userequip());

			for(int index=0; index < userEquipInfoRet->equip_size(); ++index)
			{
				const OPUserEquipInfoRet_EquipInfo& info =userEquipInfoRet->equip(index);
				UserEquipInfo * userEquipInfo = _teamBattleInfo.getAndCreatUserEquipInfo(info.id());
				userEquipInfo->itemid		= info.itemid();
				userEquipInfo->level		= info.level();
				userEquipInfo->refinexp		= info.refinexp();
				userEquipInfo->refinelevel	= info.refinelevel();
				for(int i=0; i < info.pinfo_size(); ++i)
				{
					const OPUserEquipInfoRet_EquipInfo_PropertyInfo& pInfo = info.pinfo(i);
					userEquipInfo->propertyInfo.insert(std::make_pair(pInfo.type(),pInfo.num()));
				}
				for(int index = 0; index < info.stoneinfo_size() ; index++)
				{
					userEquipInfo->setStoneInfoByPosition(index,info.stoneinfo(index));
				}
				userEquipInfo->buffvalue = info.buffvalue();
			}
		}

		//skill

		if(showBattleRet->has_userskill())
		{
			const OPUserSkillInfoRet * skillInfoRet = dynamic_cast<const OPUserSkillInfoRet*>(&showBattleRet->userskill());

			for(int index=0; index < skillInfoRet->skill_size(); ++index)
			{
				const OPUserSkillInfoRet_SkillInfo& info =skillInfoRet->skill(index);
				SkillInfo * skillInfo = _teamBattleInfo.getAndCreatSkillInfo(info.id());
				skillInfo->itemid		= info.itemid();
				skillInfo->level		= info.level();
				skillInfo->quantity		= info.quantity();
				skillInfo->consume		= info.consume();
				skillInfo->isBornSkill	= info.isborn();
			}
		}

		//title
		if(showBattleRet->has_usertitle())
		{
			const OPTitleInfoRet * titleInfoRet = dynamic_cast<const OPTitleInfoRet*>(&showBattleRet->usertitle());

			for(int index=0; index < titleInfoRet->titles_size(); ++index)
			{
				const OPTitleInfoRet_TitleItem& titleItem = titleInfoRet->titles(index);
				TitleInfo* titleInfo = _teamBattleInfo.getAndCreatTitleInfo(titleItem.titleid() / 1000);
				titleInfo->itemid = titleItem.titleid();
				if(titleItem.has_gettime())
					titleInfo->startime = titleItem.gettime();
			}
		}
		std::string _viewTeamPageName="ViewTeamPageKey";
		std::string _pageName="ViewTeamPage";
		if(HAS_V(_viewTeamPageName))
		{
			_pageName=GET_V_S(_viewTeamPageName);
		}
		MsgMainFramePushPage msgTeam;
		msgTeam.pageName = _pageName;
		MessageManager::Get()->sendMessage(&msgTeam);
	}
	
}
DEFINE_CREATE_REGISTER_HANDLER_END



DEFINE_CREATE_REGISTER_HANDLER(OPUpgradeStoneInfoHandle,OPCODE_UPGRADE_STONEINFORET_S)
{
	const OPUpgradeStoneInfoRet* ret = dynamic_cast<const OPUpgradeStoneInfoRet*>(packet);
	if(ret){
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->has_silvercoins())
		{
			userBasicInfo.silvercoins = ret->silvercoins();
		}
	}
	MsgMainFrameRefreshPage msg1;
	msg1.pageName="PackagePage";
	MessageManager::Get()->sendMessage(&msg1);

	CCBContainer* ccb = CCBManager::Get()->getPage("StoneInlaidPage");
	StoneInlaidPage* alertPage = dynamic_cast<StoneInlaidPage*>(ccb);
	alertPage->chooseNum = 3;

	MsgMainFrameRefreshPage msg2;
	msg2.pageName="StoneInlaidPage";
	MessageManager::Get()->sendMessage(&msg2);
}
DEFINE_CREATE_REGISTER_HANDLER_END



DEFINE_CREATE_REGISTER_HANDLER(OPInlaidStoneInfoHandle,OPCODE_INLAID_STONEINFORET_S)
{

	const OPInlaidStoneInfoRet* ret = dynamic_cast<const OPInlaidStoneInfoRet*>(packet);
	if(ret){
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->has_battle())
		{
			const OPUserBattleArrayRet_BattleArrayInfo& info = ret->battle();
			UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getUserBattleInfo(info.position());
			userBattleInfo->id			=info.id();
			userBattleInfo->discipleId	=info.discipleid();
			userBattleInfo->attack		=info.attack();
			userBattleInfo->defence		=info.defence();
			userBattleInfo->health		=info.health();
			userBattleInfo->rp			=info.rp();
			userBattleInfo->skill1		=info.skill1();
			userBattleInfo->skill2		=info.skill2();
			userBattleInfo->equip1		=info.equip1();
			userBattleInfo->equip2		=info.equip2();
			userBattleInfo->equip3		=info.equip3();
			userBattleInfo->position	=info.position();

			userBattleInfo->fateInfos.clear();
			for(int i=0; i < info.fates_size(); ++i)
			{
				const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
				userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
			}
		}

	}
	CCBContainer* ccb = CCBManager::Get()->getPage("StoneInlaidPage");
	StoneInlaidPage* alertPage = dynamic_cast<StoneInlaidPage*>(ccb);
	alertPage->chooseNum = 1;

	MsgMainFrameRefreshPage msg1;
	msg1.pageName = "StoneInlaidPage";
	MessageManager::Get()->sendMessage(&msg1);

}
DEFINE_CREATE_REGISTER_HANDLER_END


DEFINE_CREATE_REGISTER_HANDLER(OPRemoveStoneInfoHandle,OPCODE_REMOVE_STONEINFORET_S)
{

	const OPRemoveStoneInfoRet* ret = dynamic_cast<const OPRemoveStoneInfoRet*>(packet);
	if(ret){
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}
		if(ret->has_battle())
		{
			const OPUserBattleArrayRet_BattleArrayInfo& info = ret->battle();
			UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getUserBattleInfo(info.position());
			userBattleInfo->id			=info.id();
			userBattleInfo->discipleId	=info.discipleid();
			userBattleInfo->attack		=info.attack();
			userBattleInfo->defence		=info.defence();
			userBattleInfo->health		=info.health();
			userBattleInfo->rp			=info.rp();
			userBattleInfo->skill1		=info.skill1();
			userBattleInfo->skill2		=info.skill2();
			userBattleInfo->equip1		=info.equip1();
			userBattleInfo->equip2		=info.equip2();
			userBattleInfo->equip3		=info.equip3();
			userBattleInfo->position	=info.position();

			userBattleInfo->fateInfos.clear();
			for(int i=0; i < info.fates_size(); ++i)
			{
				const OPUserBattleArrayRet_FateInfo& fateInfo = info.fates(i);
				userBattleInfo->fateInfos.insert(std::make_pair(fateInfo.fateid(), fateInfo.status()));
			}
		}

	}
	CCBContainer* ccb = CCBManager::Get()->getPage("StoneInlaidPage");
	StoneInlaidPage* alertPage = dynamic_cast<StoneInlaidPage*>(ccb);
	alertPage->chooseNum = 1;

	MsgMainFrameRefreshPage msg1;
	msg1.pageName = "StoneInlaidPage";
	MessageManager::Get()->sendMessage(&msg1);

}
DEFINE_CREATE_REGISTER_HANDLER_END



DEFINE_CREATE_REGISTER_HANDLER(OPPunchStoneInfoHandle,OPCODE_PUNCH_STONEINFORET_S)
{

	const OPPunchStoneInfoRet* ret = dynamic_cast<const OPPunchStoneInfoRet*>(packet);
	if(ret){
		if(ret->equipinfo_size())
		{
			for(int index=0; index < ret->equipinfo_size(); ++index)
			{
				GetPropManager::Get()->gotEquipment(ret->equipinfo(index));		
			}
		}
		if(ret->toolinfo_size())
		{
			for(int index=0; index < ret->toolinfo_size(); ++index)
			{
				GetPropManager::Get()->gotTool(ret->toolinfo(index));		
			}
		}

	}
	CCBContainer* ccb = CCBManager::Get()->getPage("StoneInlaidPage");
	StoneInlaidPage* alertPage = dynamic_cast<StoneInlaidPage*>(ccb);
	alertPage->chooseNum = 1;

	MsgMainFrameRefreshPage msg1;
	msg1.pageName = "StoneInlaidPage";
	MessageManager::Get()->sendMessage(&msg1);

}
DEFINE_CREATE_REGISTER_HANDLER_END



