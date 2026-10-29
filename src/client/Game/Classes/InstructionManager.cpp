
#include "stdafx.h"

#include "InstructionManager.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCBContainer.h"
#include "CCBManager.h"
#include "MainFrame.h"
#include "json/json.h"
#include "FightPage.h"
#include "GameMessages.h"
#include "DiscipleRecruitPage.h"
#include "libOS.h"
#include "GameMaths.h"
#include "TeamPage.h"
#include "BlackBoard.h"
#include "ServerDateManager.h"
#include "GamePlatform.h"
#include "MessageBoxPage.h"
#include "UserDiscipleInfo.pb.h"
#include "UserBattle.pb.h"

#include "RestrictedWord.h"
#include "StringConverter.h"
#include "GetPropManager.h"
#include "GamePlatform.h"
#include "CareerPage.h"
#include "MainPage.h"

USING_NS_CC;
USING_NS_CC_EXT;
class InstructionStepStartAnimation: public InstructionStep, CCBContainer::CCBContainerListener
{
	CCBContainer * mCcb;
	enum{CHOOSEMAN_CCB = 100};
	virtual void show(CCBContainer *mInstruction)
	{
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(false));
		mCcb = CCBContainer::create();
		mCcb->loadCcbiFile("StartAnimation.ccbi");
		mCcb->setListener(this);
		mCcb->runAnimation("1");
		CCB_FUNC(mInstruction,"mOtherCCB",CCNode,addChild(mCcb,0,CHOOSEMAN_CCB));
	}
	virtual void onAnimationDone(const std::string& itemName)
	{
		//InstructionManager::Get()->onMenuItemAction(itemName,0,0);
	}

	virtual void next(CCBContainer *mInstruction)
	{
		CCB_FUNC(mInstruction,"mOtherCCB",CCNode,removeAllChildren());
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(true));
		//--begin xinzheng 2013-7-17
		CCApplication::sharedApplication()->purgeCachedData();
		//--end
	}
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
	{
		bool _chainAction=false;
		if(itemName == "on1to2")
		{
			mCcb->runAnimation("2");
		}
		else if(itemName == "on2to3")
		{
			mCcb->runAnimation("3");
		}
		else if(itemName == "on3to4")
		{
			mCcb->runAnimation("4");
		}
		else if(itemName=="on4toend")
		{
			InstructionManager::Get()->onMenuItemAction(itemName,sender,tag);
		}
	}
};
class InstructionStepChooseMan: public InstructionStep,public CCBContainer::CCBContainerListener
{
public:
	CCBContainer * mCcb;
	enum{CHOOSEMAN_CCB = 100};
	virtual void show(CCBContainer *mInstruction)
	{
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(false));
		mCcb = CCBContainer::create();
		mCcb->loadCcbiFile("GuidChooseMem.ccbi");
		mCcb->setListener(this);
		//CCB_FUNC(ccb,"mName1",CCLabelBMFont,setString(""));
		//CCB_FUNC(ccb,"mName2",CCLabelBMFont,setString(""));
		//CCB_FUNC(ccb,"mName3",CCLabelBMFont,setString(""));
		CCB_FUNC(mInstruction,"mOtherCCB",CCNode,addChild(mCcb,0,CHOOSEMAN_CCB));
	}
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
	{
		bool _chainAction=false;
		if(itemName == "onMan1")
			mCcb->runAnimation("selectA");
		if(itemName == "onMan2")
			mCcb->runAnimation("selectB");
		if(itemName == "onMan3")
			mCcb->runAnimation("selectC");
		if(itemName == "onChooseMemA")
		{
			InstructionManager::Get()->setDiscipleSelected(1);
			_chainAction=true;
		}
		if(itemName == "onChooseMemB")
		{
			InstructionManager::Get()->setDiscipleSelected(2);
			_chainAction=true;
		}
		if(itemName == "onChooseMemC")
		{
			InstructionManager::Get()->setDiscipleSelected(3);
			_chainAction=true;
		}
		if(_chainAction)
		{
			InstructionManager::Get()->onMenuItemAction(itemName,sender,tag);
		}
	}
	
	virtual void next(CCBContainer *mInstruction)
	{
		CCB_FUNC(mInstruction,"mOtherCCB",CCNode,removeAllChildren());
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(true));
		//--begin xinzheng 2013-7-17
		CCApplication::sharedApplication()->purgeCachedData();
		//--end
	}
};


class InstructionStepChooseName
	: public InstructionStep
	, public CCBContainer::CCBContainerListener
	, public libOSListener
	, public PacketHandlerGeneral
{
private:
	std::string mName;
	CCBContainer *mInstruction;
	CCBContainer *mPage;
	std::map<int, std::string> mNameMap;
	bool needReSend;
public:
	InstructionStepChooseName(const std::map<int, std::string> &names):needReSend(false){mNameMap.insert(names.begin(),names.end());}
	enum{CHOOSE_NAME_CCB = 101};
	virtual void show(CCBContainer *_instruction)
	{
		PacketManager::Get()->regisiterPacketHandler(OPCODE_INSTRUCTIONRET_S,this);
		mInstruction = _instruction;
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(false));
		mPage = CCBContainer::create();
		mPage->loadCcbiFile("ChangeName.ccbi");
		mPage->setListener(this);
		CCB_FUNC(mPage,"mXClose",CCNode,setVisible(false));
		CCB_FUNC(mInstruction,"mOtherCCB",CCNode,addChild(mPage,0,CHOOSE_NAME_CCB));
		libOS::getInstance()->registerListener(this);
		randomName();
	}
	virtual void onInputboxEnter(const std::string& content)
	{
        mName = "";
        CCB_FUNC(mPage,"mName",CCLabelTTF,setString(mName.c_str()));
		bool nameOK = true;
		int length = GameMaths::calculateStringCharacters(content);
		if(GameMaths::isStringHasUTF8mb4(content))
			nameOK = false;
		if(!RestrictedWord::Get()->isStringOK(content))
			nameOK = false;
//         std::string forbiden1 = Language::Get()->getString("@FORBIDEN_DIAOYUDAO");
//         std::string forbiden2 = Language::Get()->getString("@FORBIDEN_JIANGELIEDAO");
//         if(GameMaths::hasSubString(content.c_str(), forbiden1.c_str()))
//             nameOK = false;
//         if(GameMaths::hasSubString(content.c_str(), forbiden2.c_str()))
//             nameOK = false;
        if(content == "")
			nameOK = false;
		if(!nameOK)
        {
			//MSG_BOX_LAN("@NameHaveForbbidenChar");
            std::string nameRepeated = Language::Get()->getString("@NameHaveForbbidenChar");
           // CCMessageBox(nameRepeated.c_str(),"");
			MSG_BOX(nameRepeated);
            return;
        }

		if(length<=7)
			mName = content;
		else
			mName = GameMaths::getStringSubCharacters(content,0,7);
		CCB_FUNC(mPage,"mName",CCLabelTTF,setString(mName.c_str()));
	}
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet)
	{
		if(opcode == _getOpcode())
		{
			
			const OPInstructionRet* msg = dynamic_cast<const OPInstructionRet*>(packet);
			if(msg && msg->namepossible())
			{
				CCB_FUNC(mPage,"mXClose",CCNode,setVisible(true));
				InstructionManager::Get()->saveMessageAndName(msg, mName);
				InstructionManager::Get()->setMessage(msg);
				InstructionManager::Get()->onMenuItemAction("onFunciton1",0,0);
				
				//update package list
				OPUserToolInfo userToolInfo;
				userToolInfo.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_USER_TOOL_INFO_C, &userToolInfo);

				InstructionManager::Get()->updateDisciples(1);
				MainPage* page = dynamic_cast<MainPage*>(CCBManager::Get()->getPage("MainPage"));
				page->_refreshBattle();
			}
			else
			{
				std::string nameRepeated = Language::Get()->getString("@ChangeNameRepeated");
				//CCMessageBox(nameRepeated.c_str(),"");
				MSG_BOX(nameRepeated);
			}

		}
		else if(opcode == OPCODE_PLAYER_LOGINRET_S && needReSend)
		{
			_sendPacket();
		}
		
	}
	virtual void onSendPacketFailed(const int opcode){
		PacketHandlerGeneral::onSendPacketFailed(opcode);
		if(opcode == _getOpcode() - 1)
		{
			needReSend = true;
		}
	};
	virtual void onConnectFailed(std::string ip, int port){};
	virtual void onTimeout(const int opcode){onSendPacketFailed(opcode);};
	virtual void onPacketError(const int opcode){onSendPacketFailed(opcode);};
private:
	virtual int _getOpcode() {return OPCODE_INSTRUCTIONRET_S;};
	void _sendPacket()
	{
		OPInstruction pkt;
		pkt.set_version(1);
		pkt.set_name(mName.c_str());
		int sel = InstructionManager::Get()->getDiscipleSelected();
		pkt.set_discipleselecteditemid(sel);
		PacketManager::Get()->sendPakcet(_getOpcode() - 1,&pkt);
	}
public:
	void randomName()
	{

		struct cc_timeval now;
		CCTime::gettimeofdayCocos2d(&now, NULL);
		srand(now.tv_sec+now.tv_usec);

		std::string name("");
		int namecount = mNameMap.size();
		int namelength = rand()%2 +4;
		std::map<int, std::string> nameMap;
		mNameMap.swap(nameMap);

		for(int i=0;i<namelength;++i)
		{
			std::map<int, std::string>::iterator it = nameMap.end();
			while(it == nameMap.end())
			{
				it = nameMap.find(rand()%namecount);
			}
			name.append(it->second);
			mNameMap.insert(std::make_pair(it->first,it->second));
			nameMap.erase(it);
		}
		mNameMap.insert(nameMap.begin(),nameMap.end());
		onInputboxEnter(name);
	}
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
	{
		if(itemName == "onRandom")
		{
			randomName();			
		}
		else if(itemName == "onFunciton1")
		{
			//send a message
			_sendPacket();

		}
		else if(itemName == "onInputName")
		{
			libOS::getInstance()->showInputbox(false);
		}

		
	}

	virtual void next(CCBContainer *mInstruction)
	{
		InstructionManager::Get()->updateDisciples(1);
		libOS::getInstance()->removeListener(this);
		PacketManager::Get()->removePacketHandler(this);
		
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(true));
		
		CCB_FUNC(mInstruction,"mOtherCCB",CCNode,removeAllChildren());
		//--begin xinzheng 2013-7-17
		CCApplication::sharedApplication()->purgeCachedData();
		//--end
	}
};

class InstructionStepCCB: public InstructionStep
{
public:
	std::string ccb;
	std::string func;
	int tag;
	virtual void show(CCBContainer *mInstruction)
	{
		InstructionStep::show(mInstruction);
		CCB_FUNC(mInstruction,"mContent",CCNode,setVisible(true));
	}
	virtual void next(CCBContainer *mInstruction)
	{
		InstructionManager::Get()->updateDisciples(3);
		BasePage* page = CCBManager::Get()->getPage(ccb);
		if(page && page->getLoaded())
		{
			if(tag == -1)
				page->onMenuItemAction(func,0);
			else
			{
				CCBContainer::CCBContainerListener* listener = dynamic_cast<CCBContainer::CCBContainerListener*>(page);
				if(listener)
					listener->onMenuItemAction(func,0,tag);
			}
		}
	}
};


class InstructionStepFightPageEnd : public InstructionStepCCB, public MessageHandler
{
private:
	CCBContainer *mInstruction;
public:
	InstructionStepFightPageEnd(){
		ccb = "FightPage";
		func = "onClose";
		tag = -1;
	}
	virtual void show(CCBContainer *_instruction)
	{
		mInstruction = _instruction;
		mInstruction->setVisible(false);
		MessageManager::Get()->regisiterMessageHandler(MSG_INSTRUCTION_NEXT,this);
	}
	virtual void onReceiveMassage(const GameMessage * message)
	{
		mInstruction->setVisible(true);
		InstructionStepCCB::show(mInstruction);
	}
	virtual void next(CCBContainer *mInstruction)
	{
		MessageManager::Get()->removeMessageHandler(this);
		InstructionStepCCB::next(mInstruction);
	}
};
class InstructionStepCustomAttack : public InstructionStep
{
public:
	enum COUNT
	{
		FIRST_TIME = 1,
		SECOND_TIME = 2,
	};
	COUNT count;
	virtual void show(CCBContainer *mInstruction)
	{
		InstructionStep::show(mInstruction);
		CareerPage* page = dynamic_cast<CareerPage*>(CCBManager::Get()->getPage("CareerPage"));
		if(count == FIRST_TIME)
		{
			page->getCareerAttackInfo().currCustomsPassId = 101;
			page->getCareerAttackInfo().attackCustomsPassInfoMap.clear();
			
		}
		else if(count == SECOND_TIME)
		{
			CareerPage* page = dynamic_cast<CareerPage*>(CCBManager::Get()->getPage("CareerPage"));
			page->getCareerAttackInfo().currCustomsPassId = 102;
			page->getCareerAttackInfo().attackCustomsPassInfoMap.clear();
			page->resetLastClickCustomPassId(102);
			CustomsInfo info;
			info.customsPassCount = 1;
			info.customsPassEvaluate = 3;
			page->getCareerAttackInfo().attackCustomsPassInfoMap.insert(std::make_pair(101,info));
		}
		page->ShowCareerCustomsPass(1,100+count);

	}
	virtual void next(CCBContainer *mInstruction)
	{
		BasePage* page = CCBManager::Get()->getPage("FightPage");
		if(page)
		{
			FightPage* fightPage =  dynamic_cast<FightPage*>(page);
			
			if(count == FIRST_TIME)
			{
				//fightPage->setMessage(&(InstructionManager::Get()->getMessage()->battle1()));
				//build fight here
				OPUserBattleRet* batt = InstructionManager::Get()->buildBattle1();
				fightPage->setMessage(batt);
				delete batt;

				CareerPage* page = dynamic_cast<CareerPage*>(CCBManager::Get()->getPage("CareerPage"));
				page->getCareerAttackInfo().currCustomsPassId = 102;
				page->getCareerAttackInfo().attackCustomsPassInfoMap.clear();

				CustomsInfo info;
				info.customsPassCount = 1;
				info.customsPassEvaluate = 3;
				page->getCareerAttackInfo().attackCustomsPassInfoMap.insert(std::make_pair(101,info));
			}
			else
			{
				fightPage->setMessage(&(InstructionManager::Get()->getMessage()->battle2()));

				CareerPage* page = dynamic_cast<CareerPage*>(CCBManager::Get()->getPage("CareerPage"));
				page->getCareerAttackInfo().currCustomsPassId = 103;
				page->getCareerAttackInfo().attackCustomsPassInfoMap.clear();

				CustomsInfo info;
				info.customsPassCount = 1;
				info.customsPassEvaluate = 3;
				page->getCareerAttackInfo().attackCustomsPassInfoMap.insert(std::make_pair(101,info));
				page->getCareerAttackInfo().attackCustomsPassInfoMap.insert(std::make_pair(102,info));

				page->ShowCareerCustomsPass(1,103);
			}

			MsgMainFrameCoverShow msg;
			msg.pageName = "FightPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		
	}
};
class InstructionStepShowGetOne : public InstructionStep
{
public:
	std::string page;
	virtual void next(CCBContainer *mInstruction)
	{
		mInstruction->setVisible(false);
		
		BasePage* obj = CCBManager::Get()->getPage("DiscipleRecruitPage");
		DiscipleRecruitPage* page = dynamic_cast<DiscipleRecruitPage*>(obj);
		if(page)
		{
			//temp code
			page->setID(InstructionManager::Get()->getMessage()->disciplerandom().id());
			page->setIsSoul(false);
			page->setAdditionalSoul(false);
			InstructionManager::Get()->updateDisciples(3);
			MsgMainFrameCoverShow msg;
			msg.pageName = "DiscipleRecruitPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
};

class InstructionStepChangeTeamPage : public InstructionStep, public MessageHandler
{
private:
	CCBContainer *mInstruction;
public:
	enum TAG
	{
		FIRST_TIME = 1,
		SECOND_TIME = 2,
		THIRD_TIME = 3,
	};
	TAG tag;

	virtual void show(CCBContainer *_instruction)
	{
		mInstruction = _instruction;
		mInstruction->setVisible(tag!=FIRST_TIME);
		InstructionStep::show(_instruction);
		MessageManager::Get()->regisiterMessageHandler(MSG_INSTRUCTION_NEXT,this);
	}
	virtual void onReceiveMassage(const GameMessage * message)
	{
		mInstruction->setVisible(true);
		InstructionStep::show(mInstruction);
	}
	virtual void next(CCBContainer *_instruction)
	{
		MessageManager::Get()->removeMessageHandler(this);

		MsgMainFrameCoverHide msg1;
		msg1.pageName = "InstructionStepChangeTeamPage";
		MessageManager::Get()->sendMessage(&msg1);
		if(tag == FIRST_TIME)
			InstructionManager::Get()->updateDisciples(1);
		else if(tag == SECOND_TIME)
			InstructionManager::Get()->updateDisciples(2);
		else
			InstructionManager::Get()->updateDisciples(3);

		BasePage* page = CCBManager::Get()->getPage("TeamPage");
		if(page)
		{
			TeamPage* tPage =  dynamic_cast<TeamPage*>(page);
			tPage->setTarget(2);
		}
		MsgMainFrameChangePage msg;
		msg.pageName = "TeamPage";
		MessageManager::Get()->sendMessage(&msg);
	}
};
class InstructionStepSelectCharacter : public InstructionStep
{
public:
	virtual void next(CCBContainer *_instruction)
	{
		BlackBoard::Get()->NeedChangeDisciple = false;
		BlackBoard::Get()->CurChangeDiscipleID = 0;
		BlackBoard::Get()->jumpDisciplePageName = "TeamPage";
		BlackBoard::Get()->CurBattlePosition = 1;

		MsgMainFrameChangePage msg;
		msg.pageName = "DiscipleChangePage";
		MessageManager::Get()->sendMessage(&msg);
	}
};

class InstructionStepChangePage : public InstructionStep
{
private:
	CCBContainer *mInstruction;
public:
	std::string pageName;

	virtual void show(CCBContainer *_instruction)
	{
		mInstruction = _instruction;
		InstructionStep::show(_instruction);
	}
	
	virtual void next(CCBContainer *_instruction)
	{
		MsgMainFrameChangePage msg;
		msg.pageName = pageName;
		MessageManager::Get()->sendMessage(&msg);
	}
};
InstructionManager::InstructionManager(void)
{
	mCurrentStep = 1;
	mPacketMessage = 0;
	mInstructionDone = true;
}


InstructionManager::~InstructionManager(void)
{
	if(mPacketMessage)
	{
		mPacketMessage->Clear();
		delete mPacketMessage;
	}
}

void InstructionManager::start( MainFrame* frame)
{
	int loaclStep = loadLocalInstructionStep();
	CCLOG("loaclStep:%d",loaclStep);

	UserBasicInfo& basicinfo = ServerDateManager::Get()->getUserBasicInfo();
	if(basicinfo.level>2)
	{
		mCurrentStep = TOTAL_INSTRUCTION+1;
		return;
	}
	else if(!mServerInstructionDone && loaclStep<=NAME_STEP)
	{
		ServerDateManager::Get()->getUserBasicInfo().name = "";
		mCurrentStep = loaclStep;
	}
	else if(!mServerInstructionDone && loaclStep>=TOTAL_INSTRUCTION)
	{
		ServerDateManager::Get()->getUserBasicInfo().name = "";
		mCurrentStep = 1;
	}
	else if(!mServerInstructionDone && loaclStep>NAME_STEP)
	{
		if(loadMessage())
		{
			mCurrentStep = loaclStep;

			//send name package again
			std::string stepKey = "InstructionName_"  + StringConverter::toString(mConnectedServerID) +"_"+ GamePrecedure::Get()->getUin();
			std::string name = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(stepKey.c_str(),"name");
			stepKey = "InstructionSelected_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
			int chooseID = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(stepKey.c_str(),1);

			OPInstruction pkt;
			pkt.set_version(1);
			pkt.set_name(name.c_str());
			pkt.set_discipleselecteditemid(chooseID);
			PacketManager::Get()->sendPakcet(OPCODE_INSTRUCTION_C,&pkt);
		}
		else
		{
			mCurrentStep = NAME_STEP;
		}
		
	}
	else if(mServerInstructionDone && loaclStep<=START_ISNTRUCTION)
	{
		//jump instruction
		mCurrentStep = TOTAL_INSTRUCTION+1;
		return;
	}
	else if(mServerInstructionDone && loaclStep>START_ISNTRUCTION && loaclStep<=NAME_STEP)
	{
		mCurrentStep = NAME_STEP;
	}
	else if(mServerInstructionDone && loaclStep>NAME_STEP)
	{
		if(loadMessage())
			mCurrentStep = loaclStep;
		else
		{
			mCurrentStep = NAME_STEP;
			//send name package again
		}
	}
	if(mCurrentStep>=TOTAL_INSTRUCTION)
		return;
	if(mCurrentStep>NAME_STEP)
		updateDisciples(1);
	if(mCurrentStep<ATTACK_FIRST)
	{
		UserBasicInfo& info = ServerDateManager::Get()->getUserBasicInfo();
		//info.silvercoins -= 96;
		info.exp=0;
		info.silvercoins=100;
	}
	if(mCurrentStep==5)
		mCurrentStep = 4;
	if(mCurrentStep==8)
		mCurrentStep = 7;
	if(mCurrentStep==12)
		mCurrentStep =11;
	if(mCurrentStep>17 && mCurrentStep<=20)
		mCurrentStep = 17;
	if(mCurrentStep>=14 && mCurrentStep <= 16)
		mCurrentStep = 13;
	

	mInstructionDone = false;
	mMainFrame = frame;
	createCCB();
	readSteps();
	if(mCurrentStep>3)
	{
		STEPMAP::iterator it = mSteps.find(mCurrentStep-1);
		it->second->next(mInstruction);
	}
	STEPMAP::iterator it = mSteps.find(mCurrentStep);
	if(it!=mSteps.end() && it->second)
		it->second->show(mInstruction);
	else
		finishUp();
}

void InstructionManager::createCCB()
{
	mInstruction = CCBContainer::create();
	mInstruction->loadCcbiFile("instruction.ccbi");
	mInstruction->setListener(this);
	CCB_FUNC(mMainFrame,"mInstruction",CCNode,addChild(mInstruction));
	CCB_FUNC(mInstruction,"mBackGround",CCLayer,setTouchEnabled(true));
	CCB_FUNC(mInstruction,"mBackGround",CCLayer,setTouchMode(kCCTouchesOneByOne));
	cocos2d::CCLayer* layer = CCLayer::create();
	CCSize maxsize(738.0f,1152.0f);
	layer->setContentSize(maxsize);
	layer->setPosition(0,0);
	layer->setAnchorPoint(ccp(0,0));
	layer->setTouchEnabled(true);
	layer->setTouchMode(cocos2d::kCCTouchesOneByOne);
	layer->setTag(999);
	CCB_FUNC(mMainFrame,"mInstruction",CCNode,addChild(layer,-1));
}

void InstructionManager::readSteps()
{
	Json::Reader jreader;
	Json::Value data;
	unsigned long filesize;
	char* pBuffer = (char*)getFileData(
		cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename("instructions.cfg").c_str(),
		"rt",&filesize);

	if(!pBuffer)
	{
		char msg[256];
		sprintf(msg,"Failed open file: instructions.cfg");
		cocos2d::CCMessageBox(msg,"ERROR!");
	}
	else
	{
		jreader.parse(pBuffer,data,false);
		CC_SAFE_DELETE_ARRAY(pBuffer);

		if(data["version"].asInt()==1)
		{
			InstructionStep * theStep = 0;
			
			Json::Value steplist = data["steps"];
			if(!steplist.empty() && steplist.isArray())
			{
				for(int i = 0;i < steplist.size();++i)
				{
					Json::Value unit = steplist[i];
					theStep = 0;
					
					if(unit["type"].asString() == "custom")
					{
						if(unit["func"].asString() == "startAnimation")
							theStep = new InstructionStepStartAnimation;
						else if(unit["func"].asString() == "chooseMan")
							theStep = new InstructionStepChooseMan;
						else if(unit["func"].asString() == "chooseName")
						{
							std::string names = data["randomNameList"].asString();
							GameMaths::readStringToMap(names,mNameMap);
							theStep = new InstructionStepChooseName(mNameMap);
						}
						else if(unit["func"].asString() == "customAttack")
						{
							InstructionStepCustomAttack* attack = new InstructionStepCustomAttack;
							attack->count = (InstructionStepCustomAttack::COUNT)(unit["tag"].asInt());
							theStep = attack;
						}
						else if(unit["func"].asString() == "FightPageEnd")
							theStep = new InstructionStepFightPageEnd;
						else if(unit["func"].asString() == "showGetOne")
							theStep = new InstructionStepShowGetOne;
						else if(unit["func"].asString() == "ChangeTeamPage")
						{
							InstructionStepChangeTeamPage* tPage = new InstructionStepChangeTeamPage;
							theStep = tPage;
							tPage->tag = (InstructionStepChangeTeamPage::TAG)(unit["tag"].asInt());
						}
						else if(unit["func"].asString() == "selectCharacter")
							theStep = new InstructionStepSelectCharacter;
					}
					else if(unit["type"].asString() == "ccbfunc")
					{
						InstructionStepCCB * theStep2 = new InstructionStepCCB;	
						theStep2->ccb = unit["ccb"].asString();
						theStep2->func = unit["func"].asString();
						theStep2->tag = (unit["tag"].empty()?-1:unit["tag"].asInt());
						theStep = theStep2;
					}
					else if(unit["type"].asString() == "changePage")
					{
						InstructionStepChangePage * theStep2 = new InstructionStepChangePage;
						theStep2->pageName = unit["page"].asString();
						theStep = theStep2;
					}

					if(theStep)
					{
						theStep->id = unit["id"].asInt();
						//theStep->button = (unit["button"].empty()?"":unit["button"].asString());
						theStep->position.x = unit["posX"].asDouble();
						theStep->position.y = unit["posY"].asDouble();
						theStep->scale = (unit["scale"].empty()?1.0f:unit["scale"].asDouble());
						theStep->rotation = (unit["rot"].empty()?0:unit["rot"].asDouble());
						theStep->guidPosition.x = unit["chPosX"].asDouble();
						theStep->guidPosition.y = unit["chPosY"].asDouble();
						theStep->guidString = unit["str"].asString();
						if(!unit["npcX"].empty())
						{
							theStep->npcPosition.x = unit["npcX"].asDouble();
							theStep->npcPosition.y = unit["npcY"].asDouble();
							theStep->npcScaleX = unit["npcScaleX"].asDouble();
							theStep->npcChange = true;
						}
						else
						{
							theStep->npcChange = false;
						}
						mSteps.insert(std::make_pair(theStep->id,theStep));
					}
					
					
				}
			}

		}
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
}

void InstructionManager::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	STEPMAP::iterator it = mSteps.find(mCurrentStep);

	if(it==mSteps.end())
	{
		finishUp();
	}
	else
	{
		InstructionStep* step = it->second;
		if(step)
		{
			step->next(mInstruction);
		}
		nextStep();
		STEPMAP::iterator it = mSteps.find(mCurrentStep);
		if(it == mSteps.end())
			finishUp();
		else
		{
			if(it->second)
			{
				//it->second->saveStep();
				saveLoaclInstructionStep();
				it->second->show(mInstruction);
			}
		}
	}
}


void InstructionManager::finishUp()
{
	mInstructionDone = true;
	mInstruction->removeFromParent();
	saveLoaclInstructionStep();

	STEPMAP::iterator itdel = mSteps.begin();
	for(;itdel != mSteps.end(); ++itdel)
	{
		delete itdel->second;
	}
	mSteps.clear();

	updateDisciples(-1);
	GamePrecedure::Get()->requestPackages();
	if(VaribleManager::Get()->getSetting("CDKeyPageOpenStatus")=="1")
	{
		MsgMainFramePushPage msg;
		msg.pageName="CDKeyInvitePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	CCLOG("instruction Finished!");
	CCB_FUNC(mMainFrame,"mInstruction",CCNode,removeChildByTag(999));

	//
	//extern void globalPurgeCachedData();
	//globalPurgeCachedData();
	CCApplication::sharedApplication()->purgeCachedData();
	//
}

void InstructionManager::nextStep()
{
	mCurrentStep++;
}

void InstructionManager::setMessage(const OPInstructionRet* msg )
{
	if(mPacketMessage)
	{
		mPacketMessage->Clear();
		delete mPacketMessage;
	}
	mPacketMessage = new OPInstructionRet;
	mPacketMessage->CopyFrom(*msg);
}

void InstructionManager::setStep( int step )
{
	if(step < 0)
		step = 1;

	mCurrentStep = step;
}

void InstructionManager::setConnectedServerState( int serverID , bool serverFinished)
{
	mConnectedServerID = serverID;
	mServerInstructionDone = serverFinished;
}

int InstructionManager::loadLocalInstructionStep()
{
	std::string stepKey = "InstructionStep_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
	//CCLOG("stepKey :%s ",stepKey);
	return cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(stepKey.c_str(),1);
}

void InstructionManager::saveLoaclInstructionStep()
{
	std::string stepKey = "InstructionStep_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
	cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(stepKey.c_str(),mCurrentStep);
	cocos2d::CCUserDefault::sharedUserDefault()->flush();

	CCLOG("InstructionStepSave:%d",mCurrentStep);
}
void InstructionManager::saveMessageAndName( const ::google::protobuf::Message* msg, const std::string& name)
{
	std::string stepKey = "InstructionName_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
	cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(stepKey.c_str(),name);
	cocos2d::CCUserDefault::sharedUserDefault()->flush();
	ServerDateManager::Get()->getUserBasicInfo().name = name;

	stepKey = "InstructionMessage_" + StringConverter::toString(mConnectedServerID) +"_" + GamePrecedure::Get()->getUin()+ ".msg";
	std::string tempFile(cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath());
	tempFile.append("/");
	tempFile.append(stepKey);

	unsigned char* dataBuffer = new unsigned char[msg->ByteSize()];
	if(msg->SerializeToArray(dataBuffer,1024*100))
		saveFileInPath(tempFile,"wb",dataBuffer,msg->ByteSize());
	
	if(dataBuffer)delete[]dataBuffer;
	
}

bool InstructionManager::loadMessage()
{
	unsigned char* dataBuffer;
	std::string stepKey = "InstructionMessage_" + StringConverter::toString(mConnectedServerID)+"_"+ GamePrecedure::Get()->getUin()  + ".msg";
	std::string tempFile(cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath());
	tempFile.append("/");
	tempFile.append(stepKey);

	unsigned long size;
	dataBuffer = getFileData(tempFile.c_str(),"rb",&size);
	bool ret = false;
	if(dataBuffer)
	{
		if(mPacketMessage)
		{
			mPacketMessage->Clear();
			delete mPacketMessage;
		}
		mPacketMessage = new OPInstructionRet;
		ret = mPacketMessage->ParseFromArray(dataBuffer,size);
		delete[] dataBuffer;
	}
	return ret;
}

void InstructionManager::setDiscipleSelected( int selID )
{
	std::string stepKey = "InstructionSelected_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
	cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(stepKey.c_str(),selID);
	cocos2d::CCUserDefault::sharedUserDefault()->flush();
}

int InstructionManager::getDiscipleSelected()
{
	std::string stepKey = "InstructionSelected_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
	return cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(stepKey.c_str(),1);
}

OPUserBattleRet * InstructionManager::buildBattle1()
{
	std::string stepKey = "InstructionSelected_" + StringConverter::toString(mConnectedServerID)+"_" + GamePrecedure::Get()->getUin();
	int selectedID = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(stepKey.c_str(),1);
	//return FightPage::receiveMessage();
	OPUserBattleRet *bat = new OPUserBattleRet;
	//round 1
	{
		bat->mutable_round1()->set_gotto(true);
		OPUserBattleRet_Character* cha = bat->mutable_round1()->add_characters();
		cha->set_position(0);
		cha->set_level(5);
		cha->set_life(202);
		int sourceID[3]={53,65,59};
		cha->set_resourceid(sourceID[selectedID-1]);
		int powerID[3]={172,180,200};
		cha->set_power(powerID[selectedID-1]);//180,200

		cha = bat->mutable_round1()->add_characters();
		cha->set_position(1);
		cha->set_level(5);
		cha->set_life(250);
		cha->set_resourceid(1011);
		cha->set_power(100);

		OPUserBattleRet_Action* act = bat->mutable_round1()->add_actions();
		act->set_positionid(0);
		act->set_type(OPUserBattleRet_Action_Type_ATTACK);
		act->set_skillid(1);
		OPUserBattleRet_Damage* dmg = act->add_damage();
		int harmID[3]={96,69,67};
		dmg->set_harmvalue(harmID[selectedID-1]);
		dmg->set_position(1);

		act = bat->mutable_round1()->add_actions();
		act->set_positionid(1);
		act->set_type(OPUserBattleRet_Action_Type_ATTACK);
		act->set_skillid(1);
		dmg = act->add_damage();
		int harmID2[3]={44,26,44};
		dmg->set_harmvalue(harmID2[selectedID-1]);
		dmg->set_position(0);
	}

	//round 2
	{
		bat->mutable_round2()->set_gotto(true);
		OPUserBattleRet_Character* cha = bat->mutable_round2()->add_characters();
		cha->set_position(0);
		cha->set_level(5);
		int lifeID[3]={158,177,158};
		cha->set_life(lifeID[selectedID-1]);
		int sourceID[3]={53,65,59};
		cha->set_resourceid(sourceID[selectedID-1]);
		int powerID[3]={172,180,200};
		cha->set_power(powerID[selectedID-1]);//180,200

		cha = bat->mutable_round2()->add_characters();
		cha->set_position(1);
		cha->set_level(5);
		int lifeID2[3]={154,181,183};
		cha->set_life(lifeID2[selectedID-1]);
		cha->set_resourceid(1011);
		cha->set_power(100);

		OPUserBattleRet_Action* act = bat->mutable_round2()->add_actions();
		act->set_positionid(1);
		act->set_type(OPUserBattleRet_Action_Type_SKILL);
		act->set_skillid(46);
		OPUserBattleRet_Damage* dmg = act->add_damage();
		int harmID[3]={75,56,74};
		dmg->set_harmvalue(harmID[selectedID-1]);
		dmg->set_position(0);

		act = bat->mutable_round2()->add_actions();
		act->set_positionid(0);
		act->set_type(OPUserBattleRet_Action_Type_SKILL);
		int skillID[3]={27,25,28};
		act->set_skillid(skillID[selectedID-1]);
		dmg = act->add_damage();
		int harmID2[3]={144,103,100};
		dmg->set_harmvalue(harmID2[selectedID-1]);
		dmg->set_position(1);
	}
	{
		bat->mutable_round3()->set_gotto(true);
		OPUserBattleRet_Character* cha = bat->mutable_round3()->add_characters();
		cha->set_position(0);
		cha->set_level(5);
		int lifeID[3]={83,121,84};
		cha->set_life(lifeID[selectedID-1]);
		int sourceID[3]={53,65,59};
		cha->set_resourceid(sourceID[selectedID-1]);
		int powerID[3]={172,180,201};
		cha->set_power(powerID[selectedID-1]);//180,200

		cha = bat->mutable_round3()->add_characters();
		cha->set_position(1);
		cha->set_level(5);
		int lifeID2[3]={10,78,83};
		cha->set_life(lifeID2[selectedID-1]);
		cha->set_resourceid(1011);
		cha->set_power(100);
	}
	{
		bat->mutable_result()->set_gotcoin(96);
		bat->mutable_result()->set_gotexp(30);
		bat->mutable_result()->set_gotgold(0);
		bat->mutable_result()->set_canviewteam(false);
		OPUserBattleRet_Result_CharacterResult *cha = bat->mutable_result()->add_characterresults();
		cha->set_gainexp(24);
		int sourceID[3]={53,65,59};
		cha->set_resourceid(sourceID[selectedID-1]);
		cha->set_isupgarade(false);
		cha->set_level(5);
	}
	return bat;

}


void InstructionManager::updateDisciples( int count )
{
	if(count == -1)
	{
		ServerDateManager::Get()->cleareDiscipleInfo();
		ServerDateManager::Get()->mUserBattleArray.clear();
		ServerDateManager::Get()->clearSkillInfo();
		return;
	}

	ServerDateManager::Get()->cleareDiscipleInfo();
	UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(mPacketMessage->discipleselected().id());
	userDiscipleInfo->itemid		= mPacketMessage->discipleselected().itemid();
	userDiscipleInfo->level			= mPacketMessage->discipleselected().level();
	userDiscipleInfo->exp			= mPacketMessage->discipleselected().exp();
	userDiscipleInfo->health		= mPacketMessage->discipleselected().health();
	userDiscipleInfo->attack		= mPacketMessage->discipleselected().attack();
	userDiscipleInfo->defence		= mPacketMessage->discipleselected().defence();
	userDiscipleInfo->rp			= mPacketMessage->discipleselected().rp();
	userDiscipleInfo->upgradelevel	= mPacketMessage->discipleselected().upgradelevel();
	userDiscipleInfo->potentiality	= mPacketMessage->discipleselected().potentiality();
	userDiscipleInfo->skill1		= mPacketMessage->discipleselected().skill1();
	userDiscipleInfo->battleid		= mPacketMessage->discipleselected().battleid();


	userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(mPacketMessage->disciplerandom().id());
	userDiscipleInfo->itemid		= mPacketMessage->disciplerandom().itemid();
	userDiscipleInfo->level			= mPacketMessage->disciplerandom().level();
	userDiscipleInfo->exp			= mPacketMessage->disciplerandom().exp();
	userDiscipleInfo->health		= mPacketMessage->disciplerandom().health();
	userDiscipleInfo->attack		= mPacketMessage->disciplerandom().attack();
	userDiscipleInfo->defence		= mPacketMessage->disciplerandom().defence();
	userDiscipleInfo->rp			= mPacketMessage->disciplerandom().rp();
	userDiscipleInfo->upgradelevel	= mPacketMessage->disciplerandom().upgradelevel();
	userDiscipleInfo->potentiality	= mPacketMessage->disciplerandom().potentiality();
	userDiscipleInfo->skill1		= mPacketMessage->disciplerandom().skill1();
	userDiscipleInfo->battleid		= mPacketMessage->disciplerandom().battleid();


	ServerDateManager::Get()->mUserBattleArray.clear();
	UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(0);
	userBattleInfo->id			=mPacketMessage->discipleselected().id();
	userBattleInfo->discipleId	=mPacketMessage->discipleselected().id();
	userBattleInfo->attack		=mPacketMessage->discipleselected().attack();
	userBattleInfo->defence		=mPacketMessage->discipleselected().defence();
	userBattleInfo->health		=mPacketMessage->discipleselected().health();
	userBattleInfo->rp			=mPacketMessage->discipleselected().rp();
	userBattleInfo->skill1		=mPacketMessage->discipleselected().skill1();
	userBattleInfo->skill2		=0;
	userBattleInfo->equip1		=0;
	userBattleInfo->equip2		=0;
	userBattleInfo->equip3		=0;
	userBattleInfo->position	=0;

	UserBattleInfo* userBattleInfo2 = ServerDateManager::Get()->getAndCreatUserBattleInfo(1);
	userBattleInfo2->position = 1;
	userBattleInfo2->discipleId	=0;

	GetPropManager::Get()->gotSkill(mPacketMessage->selectedskill());

	if(count>1)
	{
		userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(1);
		userBattleInfo->id			=mPacketMessage->disciplerandom().id();
		userBattleInfo->discipleId	=mPacketMessage->disciplerandom().id();
		userBattleInfo->attack		=mPacketMessage->disciplerandom().attack();
		userBattleInfo->defence		=mPacketMessage->disciplerandom().defence();
		userBattleInfo->health		=mPacketMessage->disciplerandom().health();
		userBattleInfo->rp			=mPacketMessage->disciplerandom().rp();
		userBattleInfo->skill1		=0;
		userBattleInfo->skill2		=0;
		userBattleInfo->equip1		=0;
		userBattleInfo->equip2		=0;
		userBattleInfo->equip3		=0;
		userBattleInfo->position	=1;


		GetPropManager::Get()->gotSkill(mPacketMessage->randomskill());
	}
}

void InstructionStep::show( CCBContainer *mInstruction )
{
	CCB_FUNC(mInstruction,"mRound",CCLayer,setScale(scale));
	CCB_FUNC(mInstruction,"mNode",CCNode,setPosition(position));
	CCB_FUNC(mInstruction,"mArrow",CCNode,setRotation(rotation));
	CCB_FUNC(mInstruction,"mGuider",CCNode,setPosition(guidPosition));
	if(npcChange)
	{
		CCB_FUNC(mInstruction,"mNPC",CCSprite,setPosition(npcPosition));
		CCB_FUNC(mInstruction,"mNPC",CCSprite,setScaleX(npcScaleX));
	}
	std::string words;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("InstructionDialogs"));
	GameMaths::stringAutoReturn(guidString,words,width,lines);
	CCB_FUNC(mInstruction,"mWords",CCLabelBMFont,setString(words.c_str()));
}



