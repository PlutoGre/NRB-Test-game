#pragma once
#include "GetPropManager.h"
#include "GameMessages.h"
#include "PopUpSurprisePage.h"
#include "BlackBoard.h"
#include "NewActivePromptPage.h"
#include "NewActivePromptToolPage.h"
#include "json/json.h"
#include "GamePlatform.h"

class PopupManager
	: public Singleton<PopupManager>
	,public MessageHandler
{

public:
	PopupManager(void);
	~PopupManager(void);
	void pushAllPage(GetPropManager::TYPE _type, int _id, int _count,int _skillId );
	void onReceiveMassage( const GameMessage * message );
	void init();
	void popPage();
	void showNewActiveGrade(void);
	bool checkShowBuyExp();
	bool checkShowBuyEquip();
	void showCoinsEnoughGuide();
	bool showStoneGuide();
	void setStoneGuide();
	void setShowStatsByType(std::string _type,int stats);
	bool checkShowContinueAd();
private:
	void _showGetReward(void);
	bool _pushNewActiveTool(std::string _type,unsigned int _id=0);
	void _showUpgrade(void);
	void _showNewActiveTool();
	bool _showFightGuide();
	void _checkIsResolving();
	int _getCurrGuideLevel();
	void _showBuyExpGuide();
	bool _showBuyEquipGuide();
	void _sendGuideRecordPack(std::string _key);
	NewActivePromptToolPage* mNewActiveToolPage;
	bool mShowNewActiveToolPage;
	GetPropManager::TYPE mType;
	unsigned int mId;
	int mCount;
	int mSkillId;
	bool mIsUpgrade;
	bool mShowDrop;
	Json::Value mDropStatus;
	bool mIsResolving;
	typedef std::vector<int > LvList;
	LvList mGuideLVList;
	int mCurrLevel;
	int mGuideLevel;
	bool mReqGuideAdventure;
	bool mStoneGuideFlag;
};