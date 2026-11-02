#pragma once
#include "AdventureBase.h"
#include "support/CCPointExtension.h"

class LevelUpRewardItem
	: public ContentBase
{
public:
	LevelUpRewardItem(unsigned int id,unsigned int mParentID):ContentBase(id),mParentID(mParentID){};
	virtual ~LevelUpRewardItem(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void deleteLevelUpRewardItem();
	const int mParentID;

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "LevelUpRewardItem.ccbi";
		return ccbiFileName;
	}
};


class LevelUpRewardPage
	: public AdventureBase{
public:
	LevelUpRewardPage(unsigned int id,unsigned int curIndex):AdventureBase(id){
		mIsInitData = false;
		currentIndex = curIndex;
		membersRootNode = NULL;
	};
	virtual ~LevelUpRewardPage();;

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
	virtual const std::string& getCcbiFileName(void) const{
		static std::string ccbiFileName = "LevelUpReward.ccbi";
		return ccbiFileName;
	}
private:
	bool mIsShow;

	void changeRewardStatus();

	cocos2d::extension::CCScrollView* levelUpContent;

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	bool refreshFlag;

	friend class LevelUpRewardItem;
};
