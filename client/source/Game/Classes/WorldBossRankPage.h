#pragma once
#include "CustomPage.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "ContentBase.h"

class WorldBossRankContent
	: public ContentBase
{
public:
	WorldBossRankContent(unsigned int id):ContentBase(id){};
	virtual ~WorldBossRankContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossRankContent.ccbi";
		return ccbiFileName;
	}
private:
	cocos2d::CCObject * getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);
	unsigned int mPlayerId;
};
class WorldBossRankPage
	:public CustomPage
	,public State<MainFrame>
{
public:
	WorldBossRankPage(void);
	~WorldBossRankPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(WorldBossRankPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
private:
	typedef std::vector<ContentBase * > ContentList;
	ContentList mRankList;
	cocos2d::extension::CCScrollView* mContentScollView;
	cocos2d::CCLabelBMFont* mTitleLabel;
	cocos2d::CCLabelBMFont* mRankNoteLabel;
	cocos2d::CCObject * getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);
	void buildItem(void);
	void addItem(ContentBase* item);
	void clearAllItem(void);
	static int sortRank(unsigned int a,unsigned int b);
};

