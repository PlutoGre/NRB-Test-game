
#include "stdafx.h"

#include "ChangeNamePage.h"
#include "GameMaths.h"
#include "json/json.h"
#include "GamePlatform.h"
#include "ServerDateManager.h"
#include "BlackBoard.h"
#include "MessageBoxPage.h"
#include "RestrictedWord.h"
#include "libOS.h"

REGISTER_PAGE("ChangeNamePage",ChangeNamePage);
USING_NS_CC;

ChangeNamePage::ChangeNamePage(void)
{
}


ChangeNamePage::~ChangeNamePage(void)
{
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
	
}

void ChangeNamePage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	libOS::getInstance()->registerListener(this);
	if(mNameMap.size()==0)
	{
		_readMap();
	}
	CCB_FUNC(this,"mName",CCLabelTTF,setString(ServerDateManager::Get()->getUserBasicInfo().name.c_str()));
}

void ChangeNamePage::_readMap()
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

			Json::Value steplist = data["steps"];
			if(!steplist.empty() && steplist.isArray())
			{
				for(int i = 0;i < steplist.size();++i)
				{
					Json::Value unit = steplist[i];

					if(unit["type"].asString() == "custom")
					{
						if(unit["func"].asString() == "chooseName")
						{
							std::string names = data["randomNameList"].asString();
							GameMaths::readStringToMap(names,mNameMap);
						}
					}
				}
			}

		}
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
}


void ChangeNamePage::Exit( MainFrame* frame)
{
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
}

void ChangeNamePage::Execute( MainFrame* )
{

}

void ChangeNamePage::load( void )
{
	loadCcbiFile("ChangeName.ccbi");
}

void ChangeNamePage::onInputboxEnter( const std::string& content)
{
	inputMethodCallBack(content);
}

void ChangeNamePage::inputMethodCallBack(const std::string& content)
{
	bool nameOK = true;
	int length = GameMaths::calculateStringCharacters(content);
	if(GameMaths::isStringHasUTF8mb4(content))
		nameOK = false;
	if(!RestrictedWord::Get()->isStringOK(content))
		nameOK = false;
// 	std::string forbiden1 = Language::Get()->getString("@FORBIDEN_DIAOYUDAO");
// 	std::string forbiden2 = Language::Get()->getString("@FORBIDEN_JIANGELIEDAO");
// 	if(GameMaths::hasSubString(content.c_str(), forbiden1.c_str()))
// 		nameOK = false;
// 	if(GameMaths::hasSubString(content.c_str(), forbiden2.c_str()))
// 		nameOK = false;
	if(content == "")
		nameOK = false;
	if(!nameOK)
	{
		std::string nameRepeated = Language::Get()->getString("@NameHaveForbbidenChar");
		CCMessageBox(nameRepeated.c_str(),"");
		return;
	}

	if(length<=7)
		mName = content;
	else
		mName = GameMaths::getStringSubCharacters(content,0,7);

	CCB_FUNC(this,"mName",CCLabelTTF,setString(mName.c_str()));
}

void ChangeNamePage::randomName()
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

void ChangeNamePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onRandom")
	{
		randomName();			
	}
	else if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName="ChangeNamePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onFunciton1")
	{
		OPChangeName info;
		info.set_version(1);
		info.set_name(mName);
		PacketManager::Get()->sendPakcet(OPCODE_CHANE_NAME_C,&info);
	}
	else if(itemName == "onInputName")
	{
		libOS::getInstance()->showInputbox(false);
	}
}

void ChangeNamePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void ChangeNamePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate* >(message);
		if(msg->opcode==OPCODE_CHANE_NAMERET_S)
		{
			std::string key="changeNameStatus";
			bool changOK=false;
			if(BlackBoard::Get()->hasVarible(key))
			{
				changOK=BlackBoard::Get()->getVaribleAsBool(key);
			}
			if(changOK)
			{
				UserBasicInfo& info=ServerDateManager::Get()->getUserBasicInfo();
				info.name=mName;
				MSG_BOX(Language::Get()->getString("@ChangeNameOK"));
				MsgMainFramePopPage msg;
				msg.pageName="ChangeNamePage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
}
