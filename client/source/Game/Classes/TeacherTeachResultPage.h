#pragma once
#include "CustomPage.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "GameMessages.h"
#include "ContentBase.h"

class DiscipleAddExpContent
	: public ContentBase
{
public:
	DiscipleAddExpContent(unsigned int id):ContentBase(id){};
	virtual ~DiscipleAddExpContent(){};

	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};
	virtual void init(void)
	{
		mContentContainer = CCBContainer::create();
		mContentContainer->loadCcbiFile(getCcbiFileName());
		mContentContainer->retain();
		refreshContent();
	};

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "MemberAddExpIcon.ccbi";
		return ccbiFileName;
	}
private: 
	cocos2d::CCObject * getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);
};

class TeacherTeachResultPage :
	public CustomPage
	,public MessageHandler
	,public State<MainFrame>
{
public:
	TeacherTeachResultPage(void);
	~TeacherTeachResultPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(TeacherTeachResultPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	void onReceiveMassage( const GameMessage * message );
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
private:
	cocos2d::CCSprite* mTeacherPicSprite;
	cocos2d::CCNode* mDisciplesPosNode;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	void initPage();
	void setPageByServerData(const unsigned int adventureId);
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);
	void buildDisciplesNodeItem();
	void addItem(ContentBase* item);
	void clearAllItem(void);
};

