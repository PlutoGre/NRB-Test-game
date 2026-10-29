#pragma once
#include "AdventureBase.h"
#include "json/json.h"
#include "GamePlatform.h"
class ContinueContentItem
	: public ContentBase
{
public:
	ContinueContentItem(unsigned int id,unsigned int mParentID):ContentBase(id),mParentID(mParentID){};
	virtual ~ContinueContentItem(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void deleteContinueContentItemItem();
	const int mParentID;

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "ContinueLoginRewardContent.ccbi";
		return ccbiFileName;
	}
private:
	Json::Value mLoginStatus;
	const AdventureContinueLoginItem* mItem;
	void _refreshBtn();
	int _getStatus();
	int _getLoginCount();
};


class AdventureContinuePage
	: public AdventureBase{
public:
	AdventureContinuePage(unsigned int id,unsigned int curIndex):AdventureBase(id){
		mIsInitData = false;
		currentIndex = curIndex;
		membersRootNode = NULL;
	};
	virtual ~AdventureContinuePage();

	virtual void refreshContent(void);
	void loadData();
	void updatePage(bool isShow,float dt=0.0f);
	void exitPage();

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);

	void showSuccessDialog();

	unsigned int currentIndex;

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "ContinueLoginReward.ccbi";
		return ccbiFileName;
	}
private:
	bool mIsShow;
	void changeRewardStatus();
	cocos2d::extension::CCScrollView* mContent;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersRootNode;
	bool refreshFlag;
	friend class ContinueContentItem;
};
