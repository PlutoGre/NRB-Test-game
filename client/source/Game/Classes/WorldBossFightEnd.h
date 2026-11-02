#pragma once
#include "AdventureBase.h"

class WorldBossRankItem
	: public ContentBase
{
public:
	WorldBossRankItem(unsigned int id):ContentBase(id){};
	virtual ~WorldBossRankItem(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	unsigned int mStatus;
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossRankContent.ccbi";
		return ccbiFileName;
	}
	unsigned int mPlayerId;
};


class WorldBossFightEnd
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:

	WorldBossFightEnd(void);
	~WorldBossFightEnd(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(WorldBossFightEnd);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	void refreshPage(void);
	

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossRank.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode*membersRootNode;
	cocos2d::extension::CCScrollView* members;
	void setPageData();
};