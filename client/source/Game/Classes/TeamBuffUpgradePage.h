#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "ContentBase.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "PopupPage.h"
#include "DataTableManager.h"
#include "Disciple.h"
enum MemberType
{
	TeamBufferDiscipleType=1,
	TeamBufferSoulType=2,
};
class TeamBuffUpgradeContent
	: public ContentBase
{
public:
	TeamBuffUpgradeContent(unsigned int id,unsigned int _type):ContentBase(id){ this->mType=_type;};
	virtual ~TeamBuffUpgradeContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TeamBuffUpContent.ccbi";
		return ccbiFileName;
	}
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
private:
	void _refreshPage(void);
	cocos2d::CCSprite* mMemberPic;
	cocos2d::CCMenuItemImage* mMemberFrame;
	cocos2d::CCLabelBMFont* mNumber;
	cocos2d::CCLabelBMFont* mName;
	cocos2d::CCSprite* mQuality;
	cocos2d::CCSprite* mSoul;
	cocos2d::CCSprite* mNumberBg;
	unsigned int mType;
	void _refreshDisciple();
	void _refreshSoul();
};

class TeamBuffUpgradePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	TeamBuffUpgradePage(void);
	~TeamBuffUpgradePage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TeamBuffUpgradePage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	void _refreshPage(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;
	virtual void onReceiveMassage(const GameMessage * message);
	void buildMemberItem(void);
	void addMemberItem(ContentBase* item,unsigned int index);
	void buildSoulItem(void);
	void addSoulItem(ContentBase* item,unsigned int index);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeMemberItem(ContentBase* item);
	void removeSoulItem(ContentBase* item);
	unsigned int mId;
	void showPage(unsigned int id);
	void AddBoldness(bool type,unsigned int id, unsigned int count);
private:
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
	void loadData();
	typedef std::vector<ContentBase * > ContentList;
	ContentList mMemberContentList;
	ContentList mSoulContentList;
	CCNode* membersRootNode;
	CCNode* soulsRootNode;
	cocos2d::extension::CCScrollView* mMembers;
	cocos2d::extension::CCScrollView* mSouls;
	cocos2d::CCSprite* mBuffPic;
	cocos2d::CCSprite* mBuffTypePic1;
	cocos2d::CCSprite* mBuffTypePic2;
	cocos2d::CCLabelBMFont* mBuffName;
	cocos2d::CCLabelBMFont* mBuffLV;
	cocos2d::CCLabelBMFont* mBuffLV1;
	cocos2d::CCLabelBMFont* mBuffCurrValue;
	cocos2d::CCLabelBMFont* mBuffNextValue;
	cocos2d::CCLabelBMFont* mSoulNum;
	cocos2d::CCLabelBMFont* mMaxNum;
	const TeamBuffItem* mTeamBuffItem;
	cocos2d::CCSprite* mSquareBg;
	cocos2d::CCSprite* mSquarePic;
	cocos2d::CCSprite* mSquareTop;
};

#define  TeamBuffUpgrade(teamBuffId) dynamic_cast<TeamBuffUpgradePage*>(CCBManager::Get()->getPage("TeamBuffUpgradePage"))->showPage(teamBuffId);
#define  TeamBuffAddBoldness(type,id,count) dynamic_cast<TeamBuffUpgradePage*>(CCBManager::Get()->getPage("TeamBuffUpgradePage"))->AddBoldness(type,id,count);