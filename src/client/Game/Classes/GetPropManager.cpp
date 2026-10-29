
#include "stdafx.h"

#include "GetPropManager.h"

#include "GameMessages.h"
#include "PopUpSurprisePage.h"
#include "BlackBoard.h"
#include "PopupManager.h"
#include "WorldBossGiftPreviewPage.h"
#include "StoneInlaidPage.h"

GetPropManager::GetPropManager(void)
{
}


GetPropManager::~GetPropManager(void)
{
}

void GetPropManager::popUpPage( TYPE _type, int _id, int _count,int skillId)
{
	/*
	这里是注释的原来的掉落装备等其它物品的代码，下面做了些调整 ------sadan
	BasePage* bp = CCBManager::Get()->getPage("PopUpSurprisePage");
	if(bp)
	{
		PopUpSurprisePage* page = dynamic_cast<PopUpSurprisePage*>(bp);
		if(page)
		{
			page->setInfo(_type,_id,_count);
			MsgMainFramePushPage msg;
			msg.pageName = "PopUpSurprisePage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	*/

	PopupManager::Get()->pushAllPage(_type,_id,_count,skillId);

}

void GetPropManager::popUpResultPage(OPUserBattleRet* mBattleMsg){
	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	WorldBossGiftPreviewPage::PACKCONTENT_LIST _list;
	CCBContainer* ccb = CCBManager::Get()->getPage("WorldBossGiftPreviewPage");
	WorldBossGiftPreviewPage* page = dynamic_cast<WorldBossGiftPreviewPage*>(ccb);
	page->setTitle("@WorldBossGiftText");

	int gold = mBattleMsg->result().gotgold();
	if(gold!=0){
		userBasicInfo.goldcoins += gold;
		const UserPropertyItem * userPropertyItem = UserPropertyTableManager::Get()->getUserPropertyItemByID(1001);
		WorldBossGiftPreviewPage::PackContent content(userPropertyItem->name,userPropertyItem->iconPic,gold,gold);
		_list.push_back(content);
	}

	int coin = mBattleMsg->result().gotcoin();
	if(coin!=0){
		userBasicInfo.silvercoins += coin;
		const UserPropertyItem * userPropertyItem = UserPropertyTableManager::Get()->getUserPropertyItemByID(1002);
		WorldBossGiftPreviewPage::PackContent content(userPropertyItem->name,userPropertyItem->iconPic,coin,coin);
		_list.push_back(content);
	}

	int exp = mBattleMsg->result().gotexp();
	if(exp!=0){
		userBasicInfo.exp += exp;
		const UserPropertyItem * userPropertyItem = UserPropertyTableManager::Get()->getUserPropertyItemByID(1003);
		WorldBossGiftPreviewPage::PackContent content(userPropertyItem->name,userPropertyItem->iconPic,exp,exp);
		_list.push_back(content);
	}

	switch(mBattleMsg->result().gotitem().type())
	{
		case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_TOOL:
		case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_TOOL_USE: 
			if(mBattleMsg->result().gotitem().has_tool())
			{
				int toolItemId = mBattleMsg->result().gotitem().tool().itemid();
				int toolCount = mBattleMsg->result().gotitem().tool().count();
				const ToolItem* tool = ToolTableManager::Get()->getToolItemByID(mBattleMsg->result().gotitem().tool().itemid());
				if(tool)
				{
					WorldBossGiftPreviewPage::PackContent content(tool->name,tool->iconPic,mBattleMsg->result().gotitem().count(),mBattleMsg->result().gotitem().count());
					_list.push_back(content);
				}
		
			}
			gotTool(mBattleMsg->result().gotitem().tool());
			break;
		case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_EQUIPMENT:
			if(mBattleMsg->result().gotitem().has_equip()){
				const EquipItem* equip = EquipTableManager::Get()->getEquipItemByID(mBattleMsg->result().gotitem().equip().itemid());
				if(equip){
//					WorldBossGiftPreviewPage::PackContent content(equip->name,equip->iconPic,mBattleMsg->result().gotitem().equip().count(),mBattleMsg->result().gotitem().equip().count());
					WorldBossGiftPreviewPage::PackContent content(equip->name,equip->iconPic,1,1);
					_list.push_back(content);
				}
			}
			gotEquipment(mBattleMsg->result().gotitem().equip());
			break;
		case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SKILL:
			if(mBattleMsg->result().gotitem().has_skill()){
				const SkillItem* skill = SkillTableManager::Get()->getSkillItemByID(mBattleMsg->result().gotitem().skill().itemid());
				if(skill){
//					WorldBossGiftPreviewPage::PackContent content(skill->name,skill->iconPic,mBattleMsg->result().gotitem().skill().count(),mBattleMsg->result().gotitem().skill().count());
					WorldBossGiftPreviewPage::PackContent content(skill->name,skill->iconPic,1,1);
					_list.push_back(content);
				}
			}
			gotSkill(mBattleMsg->result().gotitem().skill());
			break;
		case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SOUL:
			if(mBattleMsg->result().gotitem().has_soul()){
				const DiscipleItem* soul = DiscipleTableManager::Get()->getDiscipleItemByID(mBattleMsg->result().gotitem().soul().itemid());
				if(soul){
					WorldBossGiftPreviewPage::PackContent content(soul->name,soul->iconPic,mBattleMsg->result().gotitem().count(),mBattleMsg->result().gotitem().count());
					_list.push_back(content);
				}
			}
			gotSoul(mBattleMsg->result().gotitem().soul());
			break;
		case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SKILLBOOK:
			if(mBattleMsg->result().gotitem().has_skillbook()){
				const SkillBookTable::SkillBookItem* skillBook = SkillBookTableManager::Get()->getSkillBookItemByID(mBattleMsg->result().gotitem().skillbook().skillid(),mBattleMsg->result().gotitem().skillbook().partid());
				if(skillBook){
					WorldBossGiftPreviewPage::PackContent content(skillBook->name,skillBook->iconPic,mBattleMsg->result().gotitem().count(),mBattleMsg->result().gotitem().count());
					_list.push_back(content);
				}
			}
			gotSkillBook(mBattleMsg->result().gotitem().skillbook());
			break;
	}

	if(_list.size()==0){
		return;
	}
	page->setContent(_list);
	if(BlackBoard::Get()->worldbossFinalFight == 1){

		page->setTitle("@WorldBossFinalKill");

	}else{
		page->setTitle("@WorldBossGetReward");

	}
	page->setPackMsg("@WorldBossGetReward");


	MsgMainFramePushPage msg;
	msg.pageName = "WorldBossGiftPreviewPage";
	MessageManager::Get()->sendMessage(&msg);

}

void GetPropManager::gotTool( const OPUserToolInfoRet_ToolInfo& info )
{
	UserToolInfo * userToolInfo = ServerDateManager::Get()->getAndCreatToolInfo(info.id());
	userToolInfo->itemid	= info.itemid();
	userToolInfo->count	= info.count();
	ServerDateManager::Get()->creatUserToolKeyMapByToolItem(userToolInfo);
}

void GetPropManager::gotEquipment( const OPUserEquipInfoRet_EquipInfo& info )
{
	UserEquipInfo * userEquipInfo = ServerDateManager::Get()->getAndCreatUserEquipInfo(info.id());
	userEquipInfo->itemid		= info.itemid();
	userEquipInfo->level		= info.level();
	userEquipInfo->refinexp		= info.refinexp();
	userEquipInfo->refinelevel	= info.refinelevel();
	for(int i=0; i < info.pinfo_size(); ++i)
	{
		const OPUserEquipInfoRet_EquipInfo_PropertyInfo& pInfo = info.pinfo(i);
		if(userEquipInfo->propertyInfo.find(pInfo.type()) == userEquipInfo->propertyInfo.end()){
			userEquipInfo->propertyInfo.insert(std::make_pair(pInfo.type(),pInfo.num()));
		}else{
			userEquipInfo->propertyInfo.find(pInfo.type())->second = pInfo.num();
		}
	}
	for(int index = 0; index < info.stoneinfo_size() ; index++){
		userEquipInfo->setStoneInfoByPosition(index,info.stoneinfo(index));

	}
	userEquipInfo->buffvalue = info.buffvalue();
	

	
}

void GetPropManager::gotSkill( const OPUserSkillInfoRet_SkillInfo& info )
{
	SkillInfo * skillInfo = ServerDateManager::Get()->getAndCreatSkillInfo(info.id());
	skillInfo->itemid		= info.itemid();
	skillInfo->level		= info.level();
	skillInfo->quantity		= info.quantity();
	skillInfo->consume		= info.consume();
	skillInfo->isBornSkill	= info.isborn();
}

void GetPropManager::gotSoul( const OPUserSoulInfoRet_SoulInfo& info )
{
	UserSoulInfo * userSoulInfo = ServerDateManager::Get()->getAndCreatSoulInfo(info.id());
	userSoulInfo->itemid	= info.itemid();
	userSoulInfo->count	= info.count();
}

void GetPropManager::gotDisciple(const OPUserDiscipleInfoRet_DiscipleInfo& info)
{
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


void GetPropManager::gotSkillBook(const OPGetSkillBookInfoRet_SkillBookPartItem& info)
{
	if(ServerDateManager::Get()->getAdventureItemInfoMapTotalNum()==0)
	{
		OPAdventureInfo msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_ADVENTURE_INFO_C, &msg);
	}
	else
	{
		if(info.partid()==0)
		{
			ServerDateManager::Get()->addSkillBookPieceChanceBySkillId(info.skillid(),info.count());
		}
		else
		{
			ServerDateManager::Get()->addSkillBookPartBySkillIdAndPartId(info.skillid(),info.partid(),info.count());
		}
	}
}