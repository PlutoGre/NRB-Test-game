
#include "stdafx.h"

#include "ResManager.h"
#include "DataTableManager.h"
#include "Language.h"
#include "GameMaths.h"
#include "MessageBoxPage.h"
#include "BlackBoard.h"
ResInfo* ResManager::getResInfoByTypeAndId(unsigned int type,unsigned int itemId,unsigned int count = 0,unsigned int partId)
{
		ResInfo* info =new ResInfo();
		info->count = count;
		info->itemId = itemId;
		info->type = type;
		info->partId=partId;
		setResInfo(info);
		
	return info;
}
void ResManager::setResInfo(ResInfo* info)
{
	const unsigned int resType = getResMainType(info->type);
	switch(resType)
	{
	case TOOLS_TYPE:
		{
			const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(info->itemId);
			info->name = toolItem->name;
			info->describe = toolItem->describe;
			info->icon = toolItem->iconPic;
			info->quality = 0;
		}
		break;
	case SKILL_TYPE:
		{
			const SkillItem* skillItem = SkillTableManager::Get()->getSkillItemByID(info->itemId);
			info->name = skillItem->name;
			info->describe = skillItem->describe;	
			info->icon = skillItem->iconPic;
			info->quality = skillItem->quality;
		}
		break;
	case SKILL_PIECE_TYPE:
		{
			const SkillItem* skillPieceItem = SkillTableManager::Get()->getSkillItemByID(info->itemId);
			info->name = skillPieceItem->name+Language::Get()->getString("@SkillPiece");
			info->describe = skillPieceItem->describe;
			info->icon = skillPieceItem->iconPic;
			info->quality = skillPieceItem->quality;
		}
		break;
	case EQUIP_TYPE:
		{
			const EquipItem* equipItem = EquipTableManager::Get()->getEquipItemByID(info->itemId);
			if(equipItem)
			{
				info->name = equipItem->name;
				info->describe = equipItem->describe;
				info->icon = equipItem->iconPic;
				info->quality = equipItem->quality;
			}
			else
			{
				char errMsg[256];
				sprintf(errMsg,"equip Id:%d is not exist!",info->itemId);
				MSG_BOX(errMsg);
			}
		}

		break;
	case SOUL_TYPE:
		{
			const DiscipleItem* soulItem = DiscipleTableManager::Get()->getDiscipleItemByID(info->itemId);
			info->name = soulItem->name+Language::Get()->getString("@MPTabSoul");
			info->describe = soulItem->describe;
			info->icon = soulItem->iconPic;
			info->quality = soulItem->quality;
		}
		break;
	case DISCIPLE_TYPE:
		{
			const DiscipleItem* discipleItem = DiscipleTableManager::Get()->getDiscipleItemByID(info->itemId);
			info->name = discipleItem->name;
			info->describe = discipleItem->describe;
			info->icon = discipleItem->iconPic;
			info->quality = discipleItem->quality;
		}
		break;
	case USER_PROPERTY_TYPE:
		{
			const UserPropertyItem* userPropertyItem = UserPropertyTableManager::Get()->getUserPropertyItemByID(info->itemId);
			info->name = userPropertyItem->name;
			info->describe = userPropertyItem->describe;
			info->icon = userPropertyItem->iconPic;
			info->quality = 0;
		}
		break;
	case INTERFACE_TYPE:
		break;
	default:
		break;
	}
}
ResInfoMap ResManager::getResInfoListByResBagStr(const std::string& resBagStr)
{
	ResInfoMap resInfoList;
	std::vector<std::string> resItemsVec = GameMaths::tokenize(resBagStr,",");
	int len = resItemsVec.size();
	for(int i=0;i<len;i++)
	{
		std::vector<std::string> itemVec = GameMaths::tokenize(resItemsVec.at(i),":");
		ResInfo* info = new ResInfo;
		info->type = atoi(itemVec.at(0).c_str());
		info->itemId = atoi(itemVec.at(1).c_str());
		info->count = atoi(itemVec.at(2).c_str());
		info->id = i+1;
		setResInfo(info);
		resInfoList.insert(ResInfoMap::value_type(i+1,info));
	}

	return resInfoList;
	
}

const unsigned int ResManager::getTreasurePairType(unsigned int treasureId)
{
	if(mPairList.find(treasureId)!=mPairList.end())
	{
		return mPairList.find(treasureId)->second;
	}
	return 0;

}
const unsigned int ResManager::getResMainType(unsigned int resId)
{
	if(mResList.find(resId)!=mResList.end())
	{
		return mResList.find(resId)->second;
	}
	return 0;

};
const unsigned int ResManager::getVipDailyLimit(unsigned int vipLevel)
{
	if(mVipDailyLimitList.find(vipLevel)!=mVipDailyLimitList.end())
	{
		return mVipDailyLimitList.find(vipLevel)->second;
	}
	return 0;
}
const unsigned int ResManager::getNextDailyLimitVipLevel(unsigned int vipLevel)
{
	const unsigned int limit = getVipDailyLimit(vipLevel);
	for(int i = vipLevel+1;i<VIP_MAX_LEVEL;i++)
	{
		if(limit<getVipDailyLimit(i))
		{
			return i;
		}
	}
	return 0;
}
void ResManager::useToolByIDAndCount(unsigned int toolId,unsigned int toolItemId,unsigned int toolCount)
{
	BlackBoard::Get()->setUseToolId(toolId);
	BlackBoard::Get()->setUseToolCount(toolCount);
	OPUseTool useTool;
	useTool.set_id(toolItemId);
	useTool.set_num(toolCount);
	PacketManager::Get()->sendPakcet(OPCODE_USE_TOOL_C,&useTool);
}

int ResManager::getResInfoListByResBagStrCount( const std::string& ResBagStr )
{
	return getResInfoListByResBagStr(ResBagStr).size();
}

ResInfo* ResManager::getResInfoListByResBagStrByIndex( const std::string& ResBagStr,int index )
{
	ResInfoMap::iterator it = getResInfoListByResBagStr(ResBagStr).begin();
	for(int i=0;i<index;++i)
		it++;
	return it->second;
}
