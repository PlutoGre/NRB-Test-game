#pragma once
#include "AdventureBase.h"
#include "GameMaths.h"
#include "ContentBase.h"
class RewardContent
	: public ContentBase
{
public:
	RewardContent(unsigned int id):ContentBase(id){};
	virtual ~RewardContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "FightEndRewardFrame.ccbi";
		return ccbiFileName;
	}
};
class FightEndPage
	:public AdventureBase
{
public:
	FightEndPage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=false;
		mRewardContent = NULL;
	};
	virtual ~FightEndPage();;
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "FightEndPage.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
	
private:
	typedef std::map<unsigned int,cocos2d::CCNode* > ContentNodeList;
	ContentNodeList mNodeList;
	cocos2d::CCNode* getNodePanelByType(const unsigned int type);
	unsigned int mCurrNodeType;
	AdventureFightEndInfo* mFightInfo;
	void refreshMainPanel();
	void refreshAttrPanel();
	void refreshFightPanel();
	void refreshStepAttrPanel();
	void refreshRewardPanel();

	void _addFightStarRate(unsigned int rate);

	cocos2d::extension::CCScrollView* mRewardContent;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mRewardContentList;
	void buildRewardItem(void);
	void addRewardItem(ContentBase* item);
	void rebuildAllRewardItem(void);
	void clearAllRewardItem(void);
};
