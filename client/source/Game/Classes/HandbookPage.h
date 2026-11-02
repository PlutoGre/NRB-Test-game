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

class HandbookPersonContentItemData
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int	m_iDiscipleID;
};

class HandbookPersonContent
	: public ContentBase
	, public cocos2d::extension::CCReViSvItemNodeFacade
{
public:
	HandbookPersonContent(unsigned int id):ContentBase(id){ mIsShowDetail=false;};
	virtual ~HandbookPersonContent(){
		if (!mStrTxt.empty())
		{
			CCTextureCache::sharedTextureCache()->removeTextureForKey(mStrTxt.c_str());
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(mStrTxt.c_str());
		}
	};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void initItemView();
	virtual void refreshItemView(cocos2d::extension::CCReViSvItemData* pItem);
	
	unsigned int mStatus;
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "HandbookContent.ccbi";
		return ccbiFileName;
	}
	bool mIsShowDetail;
private:
	std::string mStrTxt;
};

class HandbookEquipmentContentItemData
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int	m_iEquipmentID;
};

class HandbookEquipmentContent
	: public ContentBase
	, public cocos2d::extension::CCReViSvItemNodeFacade
{
public:
	HandbookEquipmentContent(unsigned int id):ContentBase(id){mIsShowDetail=false;};
	virtual ~HandbookEquipmentContent(){
		if (!mStrTxt.empty())
		{
			CCTextureCache::sharedTextureCache()->removeTextureForKey(mStrTxt.c_str());
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(mStrTxt.c_str());
		}
	};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void initItemView();
	virtual void refreshItemView(cocos2d::extension::CCReViSvItemData* pItem);
	
	unsigned int mStatus;
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "HandbookContent.ccbi";
		return ccbiFileName;
	}
	bool mIsShowDetail;
private:
	std::string mStrTxt;
};

class HandbookSkillContentItemData
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int	m_iSkillID;
};

class HandbookSkillContent
	: public ContentBase
	, public cocos2d::extension::CCReViSvItemNodeFacade
{
public:
	HandbookSkillContent(unsigned int id):ContentBase(id){mIsShowDetail=false;};
	virtual ~HandbookSkillContent(){
		if (!mStrTxt.empty())
		{
			CCTextureCache::sharedTextureCache()->removeTextureForKey(mStrTxt.c_str());
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName(mStrTxt.c_str());
		}
	};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void initItemView();
	virtual void refreshItemView(cocos2d::extension::CCReViSvItemData* pItem);
	
	unsigned int mStatus;
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "HandbookContent.ccbi";
		return ccbiFileName;
	}
	bool mIsShowDetail;
private:
	std::string mStrTxt;
};

class HandbookPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
	, public CCBContainer::CCBContainerListener{
public:
	HandbookPage(void);
	~HandbookPage(void);
	virtual PAGE_TYPE getPageType(){return PT_CONTENT;}
	CREATE_FUNC(HandbookPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	void refreshPage(void);
	virtual void onMenuItemAction(const std::string& itemName,cocos2d::CCObject* sender,int tag);
	virtual void onMenuItemAction(const std::string& itemName,cocos2d::CCObject* sender);
	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	void onReceiveMassage( const GameMessage * message );
	bool flag;
	
private:
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode*membersRootNode;
	cocos2d::extension::CCScrollView* members;
	unsigned int mCurrTabIndex;
	unsigned int contentNum;
	cocos2d::extension::CCReViScrollViewFacade* m_pScrollViewFacade;
	bool initFlag;


};

typedef bool (*HandbookFilter)(unsigned int);
typedef bool (*HandbookSort)(unsigned int,unsigned int);

bool HandbookFilter_default(unsigned int id);

bool HandbookSort_default(unsigned int id1,unsigned int id2);
bool HandbookSort_person(unsigned int id1,unsigned int id2);
bool HandbookSort_equip(unsigned int id1,unsigned int id2);
bool HandbookSort_skill(unsigned int id1,unsigned int id2);


class HandbookDateCollect : public Singleton<HandbookDateCollect>
{
public:
	typedef std::list<unsigned int> HandbookIDCollect;
	HandbookIDCollect getHandbookDiscipleCollection(HandbookFilter sFilter = HandbookFilter_default , HandbookSort sSort = HandbookSort_person);
	HandbookIDCollect getHandbookEquipCollection(HandbookFilter sFilter = HandbookFilter_default , HandbookSort sSort = HandbookSort_equip);
	HandbookIDCollect getHandbookSkillCollection(HandbookFilter sFilter = HandbookFilter_default , HandbookSort sSort = HandbookSort_skill);


};