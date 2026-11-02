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

typedef std::vector<ContentBase * > ContentList;
enum TeamType
{
	SelfTeam,
	EnemyTeam
};
class DiscipleHeadContent
	: public ContentBase
{
public:
	DiscipleHeadContent(unsigned int id,unsigned int _type,unsigned int currPosition):ContentBase(id){ this->mType=_type;this->mCurrPosition=currPosition;};
	virtual ~DiscipleHeadContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "b_jiesuanDcontent.ccbi";
		return ccbiFileName;
	}
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
private:
	void _refreshDisciple();
	cocos2d::CCSprite* mMemberPic;
	cocos2d::CCMenuItemImage* mMemberFrame;
	cocos2d::CCLabelBMFont* mMemLV;
	cocos2d::CCLabelBMFont* mName;
	cocos2d::CCSprite* mQuality;
	unsigned int mType;
	unsigned int mCurrPosition;
};

class TitleContentPage
	: public ContentBase
{
public:
	TitleContentPage(unsigned int id,unsigned int _type):ContentBase(id){ this->mType=_type;};
	virtual ~TitleContentPage(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "b_jiesuanDcontentB.ccbi";
		return ccbiFileName;
	}
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
private:
	void _refreshPage(void);
	cocos2d::CCSprite* mTitlePic;
	cocos2d::CCMenuItemImage* mTitleFrame;
	cocos2d::CCLabelBMFont* mTitleLV;
	cocos2d::CCLabelBMFont* mName;
	unsigned int mType;
};

class ViewListTeamPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	ViewListTeamPage(void);
	~ViewListTeamPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ViewListTeamPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;
	virtual void onReceiveMassage(const GameMessage * message);
private:
	void _buildItem();
	void buildDiscipleItem(unsigned int type);
	void addDiscipleItem(unsigned int type,ContentBase* item,unsigned int index);
	void buildTitleItem(unsigned int type);
	void addTitleItem(unsigned int type,ContentBase* item,unsigned int index);
	void clearAllItem(void);
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
	ContentList mSelfDiscipleContentList;
	ContentList mSelfTitleContentList;
	ContentList mEnemyDiscipleContentList;
	ContentList mEnemyTitleContentList;
	CCSprite* mSelfTitleSymbol;
	CCSprite* mEnemyTitleSymbol;
	float mSelfHeight;
	float mEnemyHeight;
	float mTitleHeight;
	CCNode* mSelfRootNode;
	CCNode* mEnemyRootNode;
	cocos2d::extension::CCScrollView* mSelfContent;
	cocos2d::extension::CCScrollView* mEnemyContent;
	
};