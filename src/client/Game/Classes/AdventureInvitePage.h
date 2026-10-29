#pragma once
#include "AdventureBase.h"
#include "json/json.h"
#include "GamePlatform.h"
class InviteContentItem
	: public ContentBase
{
public:
	InviteContentItem(unsigned int id,unsigned int mParentID):ContentBase(id),mParentID(mParentID){};
	virtual ~InviteContentItem(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void deleteContinueContentItemItem();
	const int mParentID;

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "InviteFriendsContent.ccbi";
		return ccbiFileName;
	}
private:
	Json::Value mInviteStatus;
	Json::Value mRewardStatus;
	const AdventureInviteItem* mItem;
	void _refreshBtn();
	int _getRewardStatus();
	int _getInviteCount();
};


class AdventureInvitePage
	: public AdventureBase{
public:
	AdventureInvitePage(unsigned int id,unsigned int curIndex):AdventureBase(id){
		mIsInitData = false;
		currentIndex = curIndex;
		membersRootNode = NULL;
	};
	virtual ~AdventureInvitePage();;

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
		static std::string ccbiFileName = "InviteFriends.ccbi";
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
	friend class InviteContentItem;
};