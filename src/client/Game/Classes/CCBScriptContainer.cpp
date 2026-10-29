
#include "stdafx.h"

#include "CCBScriptContainer.h"
#include "script_support/CCScriptSupport.h"
#include "CCLuaEngine.h"
#include "Gamelua.h"
#include "MessageManager.h"

//#define LUA_LINK_THIS mLualink=this;
#define RUN_SCRIPT_FUN(funname) \
	if(mScriptFunHandler) \
	{ \
		cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine(); \
		pEngine->executeEvent(mScriptFunHandler,funname,this,"CCBScriptContainer"); \
	} 

CCBScriptContainer* CCBScriptContainer::createFromNew( const std::string& filename , int tag)
{ 
	CCBScriptContainer *pRet = new CCBScriptContainer(); 
	pRet->setTag(tag);
	pRet->mScriptFileName = filename;
	if (pRet && pRet->init()) 
	{ 
		pRet->autorelease(); 
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

CCBScriptContainer::~CCBScriptContainer()
{
	unregisterFunctionHandler();
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
	PacketManager::Get()->removePacketHandler(this);
}

bool CCBScriptContainer::init()
{
	if(!CCBContainer::init())
		return false;
	
	mMessage = NULL;
    mScriptFunHandler = NULL;
    
	if(mScriptFileName=="")return false;

	std::string funname = "luaCreat_" + mScriptFileName;

	// register lua engine
	cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine();

	pEngine->executeString(("require \""+mScriptFileName+"\"").c_str());

	CCScriptEngineManager::sharedManager()->getScriptEngine()->executeGlobalFunctionByName(funname.c_str(),this,"CCBScriptContainer");
	RUN_SCRIPT_FUN("luaInit");

	return true;
}

void CCBScriptContainer::registerFunctionHandler(int nHandler)
{
	unregisterFunctionHandler();
	mScriptFunHandler = nHandler;
}

void CCBScriptContainer::unregisterFunctionHandler(void)
{
	if (mScriptFunHandler)
	{
		CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(mScriptFunHandler);
		mScriptFunHandler = 0;
	}
}

void CCBScriptContainer::onAnimationDone( const std::string& animationName )
{
	RUN_SCRIPT_FUN("luaOnAnimationDone");
}

void CCBScriptContainer::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	RUN_SCRIPT_FUN(itemName.c_str());
}


void CCBScriptContainer::Enter( MainFrame* )
{
	RUN_SCRIPT_FUN("luaEnter");
}

void CCBScriptContainer::Execute( MainFrame* )
{
	RUN_SCRIPT_FUN("luaExecute");
}

void CCBScriptContainer::Exit( MainFrame* )
{
	MessageManager::Get()->removeMessageHandler(this);
	PacketManager::Get()->removePacketHandler(this);
	RUN_SCRIPT_FUN("luaExit");
}

void CCBScriptContainer::load( void )
{
	//loadCcbiFile(mCcbfilename);
	RUN_SCRIPT_FUN("luaLoad");
}

void CCBScriptContainer::unload( void )
{
	MessageManager::Get()->removeMessageHandler(this);
	PacketManager::Get()->removePacketHandler(this);

	RUN_SCRIPT_FUN("luaUnLoad");
	CCBContainer::unload();
	
}

static CCBScriptContainer* _searchChild(cocos2d::CCArray* childern, int tag)
{
	if(childern && childern->count() > 0)
	{
		CCObject* child;
		CCARRAY_FOREACH(childern, child)
		{
			CCNode* pNode = (CCNode*) child;
			if(pNode && pNode->getTag() == tag && dynamic_cast<CCBScriptContainer*>(pNode))
			{
				return dynamic_cast<CCBScriptContainer*>(pNode);
			}
			else
			{
				CCBScriptContainer* ret = _searchChild(pNode->getChildren(),tag);
				if(ret) return ret;
			}
		}
	}
	return 0;
}
CCBScriptContainer* CCBScriptContainer::searchChildByTag( int tag )
{
	return _searchChild(getChildren(),tag);
}

CCBScriptContainer* CCBScriptContainer::searchParentByTag( int tag )
{
	CCNode* pNode = this;
	while(pNode->getParent()!=0)
	{
		pNode = pNode->getParent();
		if(pNode->getTag() == tag && dynamic_cast<CCBScriptContainer*>(pNode))
		{
			return dynamic_cast<CCBScriptContainer*>(pNode);
		}
	}
	return 0;
}

void CCBScriptContainer::setTag( int tag )
{
	CCNode::setTag(tag);
}

int CCBScriptContainer::getTag()
{
	return CCNode::getTag();
}

CCBScriptContainer* CCBScriptContainer::create( const std::string& filename , int tag)
{
	CCBScriptContainer* pRet = 0;

	std::map<std::string,std::list<CCBScriptContainer*> >::iterator mapit = ccbsPool.find(filename);
	if(mapit==ccbsPool.end())
	{
		pRet = CCBScriptContainer::createFromNew(filename,tag);
		std::list<CCBScriptContainer*> ccbclist;
		ccbclist.push_back(pRet);
		pRet->retain();
		ccbsPool.insert(std::make_pair(filename,ccbclist));
		return pRet;
	}
	else
	{
		std::list<CCBScriptContainer*>::iterator listit = mapit->second.begin();
		for(;listit!=mapit->second.end();++listit)
		{
			pRet = *listit;
			if(pRet->isSingleReference())
			{
				if(pRet->init())
				{
					pRet->setTag(tag);
					pRet->mScriptFileName = filename;
					return pRet;
				}
			}
		}

		pRet = CCBScriptContainer::createFromNew(filename,tag);
		mapit->second.push_back(pRet);
		pRet->retain();
		return pRet;
	}
}

void CCBScriptContainer::purgeCachedData( void )
{
	std::map<std::string,std::list<CCBScriptContainer*> >::iterator mapit = ccbsPool.begin();
	for(;mapit!=ccbsPool.end();++mapit)
	{
		std::list<CCBScriptContainer*> listSwap;
		listSwap.swap(mapit->second);
		std::list<CCBScriptContainer*>::iterator listit = listSwap.begin();
		for(;listit!=listSwap.end();++listit)
		{
			CCBScriptContainer* pRet = *listit;
			if(pRet->isSingleReference())
			{
				pRet->release();
			}
			else
				mapit->second.push_back(pRet);
		}
	}
}

void CCBScriptContainer::registerMessage( int msgID )
{
	MessageManager::Get()->regisiterMessageHandler(msgID,this);
}

void CCBScriptContainer::registerPacket( int opcode )
{
	PacketManager::Get()->regisiterPacketHandler(opcode,this);
}

void CCBScriptContainer::onReceiveMassage( const GameMessage * message )
{
	mMessage = message->clone();
	RUN_SCRIPT_FUN("luaGameMessage");
}

void CCBScriptContainer::onReceivePacket( const int opcode, const std::string& str )
{
	mPktBuffer = str;
	mRecOpcode = opcode;

	RUN_SCRIPT_FUN("luaReceivePacket");
}

void CCBScriptContainer::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	std::string str;
	packet->SerializeToString(&str);

	mPktBuffer = str;
	mRecOpcode = opcode;

	RUN_SCRIPT_FUN("luaReceivePacket");
}

void CCBScriptContainer::onSendPacketFailed( const int opcode )
{
	RUN_SCRIPT_FUN("luaSendPacketFailed");
}

void CCBScriptContainer::onConnectFailed( std::string ip, int port )
{
	RUN_SCRIPT_FUN("luaConnectFailed");
}

void CCBScriptContainer::onTimeout( const int opcode )
{
	RUN_SCRIPT_FUN("luaTimeout");
}

void CCBScriptContainer::onPacketError( const int opcode )
{
	RUN_SCRIPT_FUN("luaPacketError");
}

void CCBScriptContainer::sendPakcet( int opcode, char* buff, int length, bool needWaiting /*= true*/ )
{
	mSendOpcode = opcode;
	PacketManager::Get()->sendPakcet(opcode,buff,length,needWaiting);
}

void CCBScriptContainer::removeMessage( int msgID )
{

	MessageManager::Get()->removeMessageHandler(msgID,this);
}

void CCBScriptContainer::removePacket( int opcode )
{
	PacketManager::Get()->removePacketHandler(opcode,this);
}

void CCBScriptContainer::loadCcbiFile( const std::string& filename, bool froceLoad /*= false*/ )
{
	CCBContainer::loadCcbiFile(filename, froceLoad);
}

void CCBScriptContainer::runAnimation( const std::string& actionname )
{
	CCBContainer::runAnimation(actionname);
}

std::string CCBScriptContainer::dumpInfo()
{
	return CCBContainer::dumpInfo();
}

void CCBScriptContainer::onInputboxEnter( const std::string& content )
{
	mInputBoxContent = content;
	RUN_SCRIPT_FUN("luaInputboxEnter");
}

void CCBScriptContainer::onMessageboxEnter( int tag )
{
	mMessageBoxTag = tag;
	RUN_SCRIPT_FUN("luaMessageboxEnter");
}

void CCBScriptContainer::registerLibOS()
{
	libOS::getInstance()->registerListener(this);
}

void CCBScriptContainer::removeLibOS()
{
	libOS::getInstance()->removeListener(this);
}

std::map<std::string,std::list<CCBScriptContainer*> > CCBScriptContainer::ccbsPool;
