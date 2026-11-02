#pragma once


#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CareerConfigManager.h"
#include "CustomPage.h"
#include "PacketManager.h"
#include "PackageLogic.h"
struct CustomsInfo
{
public:
	int customsPassEvaluate;
	int customsPassCount;
	CustomsInfo(void){}
	CustomsInfo(int evaluate,int passCount){customsPassEvaluate=evaluate,customsPassCount=passCount;}
};
class CareerPageAttackInfo:public PacketHandlerGeneral
{
public:
	int currChapterId;
	int currCustomsPassId;
	int currCustomsPassMaxId;
	int backwardsTime;
	std::map<int,int> chapterAddPowerMap;
	std::map<int,CustomsInfo> attackCustomsPassInfoMap;
	CareerPageAttackInfo(void);
	~CareerPageAttackInfo(void);
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet);
private:
	virtual int _getOpcode() {return OPCODE_GET_USER_CAREER_INFORET_S;};
};

class CareerPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
	, public PacketHandlerGeneral
{
public:
	CareerPage(void);
	~CareerPage(void);
	
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(CareerPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void ShowCareerCustomsPass(int chapterId,int customsPassId=-1);

	void ToggleCareerCustomsPass(int chapterId,int customsPassId=-1);

	void InitPassChapterButton();

	void changeChapterButtonStatus();

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	virtual void onTimeout(const int opcode);
	void loadAttackInfo();
	void resetLastClickCustomPassId(unsigned int passId);
	void setEvaluatePostion(CCBContainer* container,int currPassId,CustomsInfo ci);

	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet);
	
	CareerPageAttackInfo& getCareerAttackInfo(){return mCareerPageAttackInfo;}
private:

	//--begin xinzheng 2013-6-7
	/*
		选中某个章节，按钮高亮文字黄色
		在ShowCareerCustomsPass末尾调用
	*/
	void setChapterButtonStauts();
	//--end

	virtual int _getOpcode() {return OPCODE_USER_BATTLERET_S;};
	void showStory(int chapterID);
	enum BUTTON_TAG
	{
		FACE_BUTTON_START = 0,
		FACE_BUTTON_END = 10,
	};
	enum PANEL_TYPE
	{
		CHAPTER_DES_PANEL = 0,
		CHAPTER_PLOT_RELOAD =1,
		CHAPTER_PLOT_NEXT=2,
		CHAPTER_ADD_POWER=3,
	};
	enum ADD_POWER_TAG
	{
		NOT_ALLOW_ADD_POWER=1,
		ALLOW_ADD_POWER=2,
		ALLREADY_ADD_POWER=3,
	};
	bool mIsFirstIn;
	cocos2d::extension::CCScrollView* mCustomsPassScrollView;
	cocos2d::extension::CCScrollView* mNavContent;
	int mCurrChapterIndex;
	std::map<int,int> mShowCustomsPassDetailMap;
	float mCustomsPassScrollViewViewHeight;
	cocos2d::CCPoint mLastOffset;
	typedef std::list<std::pair<int,CCBContainer*> > ContentList;

	ContentList mCustomsPassContainerList;
	ContentList mChapterContainerList;
	bool mIsAttack;
	CareerPageAttackInfo mCareerPageAttackInfo;

	int mCurretFightingTag;
	int mShouldPlayCoversationID;

	float mNavWidth;
	int mLastClickCustomPassId;
	void _refreshTitle( void );
	void resetNavOffset(const unsigned int index,bool isEnter=false);
	bool _canAddPower(unsigned int chapterId);

	float mExpbarInitScale;

	void _clearPassContainer();
	void _clearNavContainer();
	void _refreshCurrCustomPage(unsigned int chapterId,unsigned int cusstomPassId);
	void _refreshCurrCustomPageQuickBtn(unsigned int chapterId);

	bool mIsQuickCarrerInCool;//是否处于冷却
};