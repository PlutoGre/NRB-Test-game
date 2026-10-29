#pragma once
#include <string>
#include <list>
#define REVISE_HEIGHT 150.0f
#define WIN_REWARD_PROPS_LENGTH 8
#define CAREEREVALUATELEVEL 3
class CareerData
{
public: 
	enum MONSTERTYPE
	{
		NORMAL = 0,
		ELITE = 1,
		BOSS = 2,
	};
	class CustomsPassData
	{
	public:
		int id;
		std::string name;
		std::string plot;
		std::string icon;
		CareerData::MONSTERTYPE monsterType;
		int consumeStrength;
		int limitAttackNum;
		int suggestLevel;
		int winRewardExp;
		int winRewardCoins;
		struct WINREWARD
		{
			enum TYPE
			{
				DON_NOT_HAVE,
				SKILL = 41000,
				EQUIPMENT = 50000
			};
			TYPE _type;
			int _id;
			int _count;
			WINREWARD():_type(DON_NOT_HAVE){}
		};
		WINREWARD winRewardProps[WIN_REWARD_PROPS_LENGTH];
	private:
		
	};
	class ChaptersData
	{
	public:
		struct DIALOG
		{
			std::string icon1;
			std::string icon2;
			std::string dialogs;
		};
		typedef std::list<DIALOG> DIALOG_LIST;
		typedef std::list< CareerData::CustomsPassData* > CUSTOM_PASS_DATA_LIST;
		int id;
		std::string name;
		std::string plot;
		int limitLevel;
		CUSTOM_PASS_DATA_LIST mCustomsPassList;
		DIALOG_LIST dialogs;

		void addCustomsPass(CareerData::CustomsPassData* customsPass)
		{
			mCustomsPassList.push_back(customsPass);
		};
	private:
		
	};

	class PassNPCData
	{
	public:
		int id;
		int character1ID;
	};
private:
};