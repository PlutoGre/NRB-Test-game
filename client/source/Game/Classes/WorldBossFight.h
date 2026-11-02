#pragma once
#include "AdventureBase.h"


class WorldBossFight
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	WorldBossFight();
	~WorldBossFight();
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(WorldBossFight);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossJoinBattle.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:

	void setPageData();
};