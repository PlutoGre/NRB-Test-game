
#include "stdafx.h"

#include "CareerConfigManager.h"
#include "json/json.h"
#include "cocos2d.h"
#include "TableReader.h"
#include "StringConverter.h"
#include "DataTableManager.h"

USING_NS_CC;


class ChaptersReader: public TableReader
{
protected:
	virtual void readline(std::stringstream& _stream)
	{
		CareerData::ChaptersData * data = new CareerData::ChaptersData;
		std::string activityEntrance;
		std::string conversations;
		_stream
			>> data->id 
			>> data->name 
			>> data->plot
			>> activityEntrance
			>> conversations;

		size_t  starter = conversations.find_first_of('{');
		size_t	ender = conversations.find_first_of('}');
		while(starter!=std::string::npos && ender!=std::string::npos && ender>starter+1)
		{
			do 
			{
				std::string sub = conversations.substr(starter+1,ender-starter-1);

				size_t colon = sub.find_first_of(":");
				if(colon==std::string::npos || sub.length()<=colon+1)
					break;

				std::string icon1 = sub.substr(0,colon);
				std::string icon2 = "";
				std::string dialogStr = "";

				size_t flagAt = sub.find_first_of(':',colon+1);
				if(flagAt<sub.length() && flagAt!=std::string::npos)
				{
					icon2 = sub.substr(colon+1,flagAt-colon-1);
					dialogStr = sub.substr(flagAt+1,sub.length()-flagAt-1);

					CareerData::ChaptersData::DIALOG dialog;
					dialog.icon1 = icon1;
					dialog.icon2 = icon2;
					dialog.dialogs = dialogStr;
					data->dialogs.push_back(dialog);

				}
				

			} while (false);

			starter = conversations.find_first_of('{',ender);
			ender =  conversations.find_first_of('}',ender+1);
		}
		CareerConfigManager::Get()->addChaptersData(data);
	};
};
class PassNPCReader : public TableReader
{
protected:
	virtual void readline(std::stringstream& _stream)
	{
		int id;
		int nouse;
		int firstCharID;
		_stream>>id>>nouse>>firstCharID;
		CareerConfigManager::Get()->mPassNPC.insert(std::make_pair(id,firstCharID));
	};
};
class PassReader: public TableReader
{
protected:
	virtual void readline(std::stringstream& _stream)
	{
		CareerData::CustomsPassData* data=new CareerData::CustomsPassData();
		int chapterID;
		int bossType;
		std::string dropItems;
		int passnpc;
		_stream 
			>> data->id
			>> chapterID
			>> data->name 
			>> bossType 
			>> data->consumeStrength 
			>> data->limitAttackNum 
			>> data->winRewardExp 
			>> data->winRewardCoins
			>> dropItems
			>> passnpc
			>> data->suggestLevel
			>> data->plot
			>> data->icon;

		data->monsterType = (CareerData::MONSTERTYPE)bossType;
		if(data->icon == "none")
		{
			std::map<int,int>::iterator it = CareerConfigManager::Get()->mPassNPC.find(passnpc);
			if(it!=CareerConfigManager::Get()->mPassNPC.end())
			{
				const DiscipleItem *item = DiscipleTableManager::Get()->getDiscipleItemByID(it->second);
				if(item)
					data->icon = item->iconPic;
			}
		}
		int i=0;
		while(i<WIN_REWARD_PROPS_LENGTH 
			&& data->winRewardProps[i]._type!=CareerData::CustomsPassData::WINREWARD::DON_NOT_HAVE)
			i++;

		size_t lastStart = 0;
		i = 0;
		while(lastStart!=std::string::npos && lastStart < dropItems.length())
		{
			size_t colom  = dropItems.find_first_of(',',lastStart);
			
			if(colom==std::string::npos)
				colom = dropItems.length();

			std::string subItem = dropItems.substr(lastStart,colom-lastStart);
			{
				size_t flag1 = subItem.find_first_of('_');
				size_t flag2 = subItem.find_first_of('_',flag1+1);
				if(flag1!=std::string::npos && flag2!=std::string::npos && flag2>flag1+1 && subItem.length()>flag2+1)
				{
					 data->winRewardProps[i]._type = (CareerData::CustomsPassData::WINREWARD::TYPE)(StringConverter::parseInt(subItem.substr(0,flag1)));
					 data->winRewardProps[i]._id = (StringConverter::parseInt(subItem.substr(flag1+1,flag2 - flag1 - 1)));
					 data->winRewardProps[i]._count = (StringConverter::parseInt(subItem.substr(flag1+1,subItem.length() - flag2 - 1)));
					 i++;
				}
			}
			lastStart = colom+1;
		}
		CareerConfigManager::Get()->getChaptersDataByChaptersId(chapterID)->mCustomsPassList.push_back(data);
	};
};


void CareerConfigManager::init( const std::string& Chapter,const std::string& pass, const std::string& passNPC )
{
	ChaptersReader chapReader;
	chapReader.parse(Chapter,1);
	PassNPCReader passNpcReader;
	passNpcReader.parse(passNPC,1);
	PassReader passReader;
	passReader.parse(pass,1);
}


CareerData::ChaptersData* CareerConfigManager::getChaptersDataByChaptersId(int id)
{
	CareerData::ChaptersData* ret = 0;
	std::list< CareerData::ChaptersData* >::iterator it =mChaptersList.begin();
	for(;it!=mChaptersList.end();++it)
	{
		CareerData::ChaptersData* chapters = *it;
		if(chapters && chapters->id==id)
		{
			ret=chapters;
			break;
		}
	}
	return ret;
}

CareerData::CustomsPassData* CareerConfigManager::getCustomsPassDataByChaptersIdAndPassId(int chapterId,int customsId)
{
	CareerData::CustomsPassData* ret = NULL;//modify by xinghui
	std::list< CareerData::ChaptersData* >::iterator it =mChaptersList.begin();
	for(;it!=mChaptersList.end();++it)
	{
		CareerData::ChaptersData* chapters = *it;
		if(chapters && chapters->id==chapterId)
		{
			std::list< CareerData::CustomsPassData* >::iterator itPass =chapters->mCustomsPassList.begin();
			for(;itPass!=chapters->mCustomsPassList.end();++itPass)
			{
				CareerData::CustomsPassData* customsPass = *itPass;
				if(customsPass&&customsPass->id==customsId)
				{
					ret=customsPass;
					break;
				}
			}
			break;
		}
	}
	return ret;
}

