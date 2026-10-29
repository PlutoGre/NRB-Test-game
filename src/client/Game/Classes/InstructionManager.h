#pragma once
#include "Singleton.h"
#include "cocos2d.h"
#include "CCBContainer.h"
#include "Instruction.pb.h"

#include <map>

class MainFrame;
class CCBContainer;

class InstructionStep
{
public:
	int id;
	cocos2d::CCPoint position;
	float scale;
	float rotation;
	cocos2d::CCPoint guidPosition;
	cocos2d::CCPoint npcPosition;
	float npcScaleX;
	bool npcChange;
	std::string guidString;
	virtual void show(CCBContainer *mInstruction);
	virtual void next(CCBContainer *mInstruction){};
};
class InstructionManager 
	: public CCBContainer::CCBContainerListener
	, public Singleton<InstructionManager>
{
public:
	InstructionManager(void);
	~InstructionManager(void);

	static InstructionManager* getInstance(){return InstructionManager::Get();}
	enum
	{
		START_ISNTRUCTION = 1,
		NAME_STEP = 3,
		ATTACK_FIRST = 6,
		TOTAL_INSTRUCTION = 23,
	};
	void setConnectedServerState(int serverID, bool serverFinished);
	void saveMessageAndName(const ::google::protobuf::Message*, const std::string& name);
	bool loadMessage();
	void updateDisciples(int count);
	void setDiscipleSelected(int selID);
	int getDiscipleSelected();
	OPUserBattleRet *buildBattle1();

	int loadLocalInstructionStep();
	void saveLoaclInstructionStep();

	void setStep(int step);
	void setMessage(const OPInstructionRet* msg);
	OPInstructionRet * getMessage(){return mPacketMessage;}

	void start(MainFrame* frame);
	public:virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

private:
	void createCCB();
	void readSteps();
	void finishUp();

	void nextStep();//save step
	void doNext(InstructionStep* step);
	void showNext(InstructionStep* step);

private:
	MainFrame *mMainFrame;
	CCBContainer *mInstruction;
	typedef std::map<int, InstructionStep*> STEPMAP;
	STEPMAP mSteps;
	int mCurrentStep;
	std::map<int, std::string> mNameMap;
	OPInstructionRet* mPacketMessage;
	bool mInstructionDone;
	int mConnectedServerID;
	bool mServerInstructionDone;
};

