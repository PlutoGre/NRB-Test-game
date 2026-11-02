#pragma once
#include "CareerData.h"
#include "Singleton.h"
#include <map>
#include <set>
namespace cocos2d{
	class CCNode;
};

class CareerConfigManager: public Singleton<CareerConfigManager>
{
private:
	std::list< CareerData::ChaptersData* > mChaptersList;
	std::map<int,int> mPassNPC;
	friend class PassNPCReader;
	friend class PassReader;
public:
	void init(const std::string& Chapter,const std::string& pass, const std::string& passNPC);
	
	void addChaptersData(CareerData::ChaptersData* chaptersData)
	{
		mChaptersList.push_back(chaptersData);
	};
	const std::list< CareerData::ChaptersData* >& getChaptersDataList()
	{
		return mChaptersList;
	};
	CareerData::ChaptersData* getChaptersDataByChaptersId(int id);
	CareerData::CustomsPassData* getCustomsPassDataByChaptersIdAndPassId(int chapterId,int customsId);

};

