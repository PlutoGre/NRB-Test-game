
#include "stdafx.h"

#include "ChatPage.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "libOS.h"
#include "RestrictedWord.h"
#include "GameMaths.h"
#include "MessageBoxPage.h"
REGISTER_PAGE("ChatPage",ChatPage);
USING_NS_CC;

ChatPage::ChatPage(void)
{
	mSurplusSecTime=0;
}


ChatPage::~ChatPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
	libOS::getInstance()->removeListener(this);
}


void ChatPage::Enter( MainFrame* frame)
{
	ServerDateManager::Get()->clearChatInfo();
	_loginChatServer();
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	libOS::getInstance()->registerListener(this);
	loadConfig();
	mPropNum=dynamic_cast<CCLabelBMFont* >(getVariable("mTrumpetNum"));
	refreshTrumpetNum();
}

void ChatPage::_refreshSendBtnStatus()
{
	if(mSurplusSecTime>0)
	{
		std::string _str=Language::Get()->getString("@MsgSend")+" "+StringConverter::toString(mSurplusSecTime)+"s";
		CCB_FUNC(this,"mButtenTex",CCLabelBMFont,setString(_str.c_str()));
		CCB_FUNC(this,"mSendButten",CCMenuItemImage,setEnabled(false));
	}
	else
	{
		CCB_FUNC(this,"mButtenTex",CCLabelBMFont,setString(Language::Get()->getString("@MsgSend").c_str()));
		CCB_FUNC(this,"mSendButten",CCMenuItemImage,setEnabled(true));
	}
}

void ChatPage::_loginChatServer()
{
	OPEnterChatPage info;
	info.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_ENTER_CHAT_PAGE_C,&info);
}

void ChatPage::_quitChatServer()
{
	OPQuitChatPage info;
	info.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_QUIT_CHAT_PAGE_C,&info);
}

void ChatPage::refreshTrumpetNum()
{
	const UserToolInfo * userToolInfo=ServerDateManager::Get()->getUserToolInfoByItemId(mTrumpetId);

	unsigned int count=0;
	if(userToolInfo)
	{
		if(userToolInfo->count>0)
		{
			count=userToolInfo->count;
			mCanSendMsg=true;
		}
		else
		{
			mCanSendMsg=false;
		}
	}
	if(mPropNum)
	{
		char c[128];
		sprintf(c,"%d",count);
		mPropNum->setString(c);
	}
}

void ChatPage::showChatContentView()
{	
	cocos2d::extension::CCScrollView* scrollContent;
	scrollContent=dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	mFriendsContainerList.clear();
	
	if(scrollContent)
	{
		cocos2d::CCNode* node = cocos2d::CCNode::create();
		CCSize size = CCSize(0,0);
		UserChatInfoListIterator element=ServerDateManager::Get()->getChatIterator();
		int i=0;

		std::list<unsigned int> myList;
		while(element.hasMoreElements())
		{
			const UserChatInfo* info=element.getNext();
			unsigned int id = info->id;
			myList.push_back(id);
		}
		myList.sort(&_sSort);
		std::list<unsigned int >::reverse_iterator it=myList.rbegin();
		float _height=0.0f;
		for(;it!=myList.rend();++it)
		{
			const UserChatInfo* info=ServerDateManager::Get()->getUserChatInfoByID(*it);
			char nameStr[128];
			if(info->vipLevel<=0)
			{
				sprintf(nameStr,"%s:",info->name.c_str());
			}
			else
			{
				sprintf(nameStr,"%s(V%s):",info->name.c_str(),StringConverter::toString(info->vipLevel).c_str());
			}
			
			CCLabelTTF* name = CCLabelTTF::create(nameStr, "Arial", 28);
			name->setAnchorPoint(cocos2d::CCPoint(0,0));
			name->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Chat_PlayerName")));
			node->addChild(name);
			const std::string vipImage = ::getVipLevelImageFile(info->vipLevel);
			
			std::string s=info->msg.c_str();
			std::string outstr;
			int lines;
			GameMaths::stringAutoReturn(s,outstr,14,lines);
			CCLabelTTF* content = CCLabelTTF::create(outstr.c_str(), "Arial",28);
			content->setHorizontalAlignment(kCCTextAlignmentLeft);
			content->setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Chat_Content")));
			content->setAnchorPoint(cocos2d::CCPoint(0,0));
			node->addChild(content);
			_height+=content->getContentSize().height;
			name->setPosition(ccp(5,_height-name->getContentSize().height));
			content->setPosition(ccp(name->getContentSize().width+5,_height-content->getContentSize().height));
			size.width = content->getContentSize().width+name->getContentSize().width;
			size.height =_height;
			i++;
		}
		myList.clear();
		node->setContentSize(size);
		scrollContent->setContainer(node);
		scrollContent->setContentSize(size);

		if(size.height<scrollContent->getViewSize().height)
		{
			scrollContent->setContentOffset(ccp(0,scrollContent->maxContainerOffset().y));
		}
		else
		{
			scrollContent->setContentOffset(ccp(0,scrollContent->minContainerOffset().y));
		}
	}
}

void ChatPage::loadConfig()
{
	mTrumpetId=1101002;
	mCanSendMsg=false;
	if(ServerDateManager::Get()->getChatInfoTotalNum()==0)
	{
	}
	else
	{
		showChatContentView();
	}
}

void ChatPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	libOS::getInstance()->removeListener(this);
	_quitChatServer();
}

void ChatPage::Execute( MainFrame* )
{
	if(mSurplusSecTime>0)
	{
		mDurationTime+=GamePrecedure::Get()->getFrameTime();
		if(mDurationTime>1.0f)
		{
			mDurationTime -=1.0f;
			mSurplusSecTime--;
			_refreshSendBtnStatus();
		}
	}
	
}

void ChatPage::load( void )
{
	loadCcbiFile("ChatPage.ccbi");
}

void ChatPage::onInputboxEnter(const std::string& content)
{
	if(content.length()<=0)
	{
		return;
	}
    bool nameOK = true;
	int length = GameMaths::calculateStringCharacters(content);
	if(GameMaths::isStringHasUTF8mb4(content))
		nameOK = false;
	if(!RestrictedWord::Get()->isStringOK(content))
		nameOK = false;
	if(content == "")
		nameOK = false;
	if(!nameOK)
	{
		std::string nameRepeated = Language::Get()->getString("@NameHaveForbbidenChar");
		CCMessageBox(nameRepeated.c_str(),"");
		return;
	}
	std::string cutContent=GameMaths::stringCutWidthLen(content,60);
	if(cutContent.length()!=content.length())
	{
		MSG_BOX(Language::Get()->getString("@ChatMsgMax"));
	}
	OPChatSendMsgInfo info;
	info.set_version(1);
	info.set_msg(cutContent);
	info.set_type(WORLD_CHAT);
	PacketManager::Get()->sendPakcet(OPCODE_CHAT_SEND_MSG_C,&info);
}

bool ChatPage::_sSort(unsigned int id1,unsigned int id2)
{
	return id1<id2;
}

void ChatPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void ChatPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onBuyTrumpetButton")
	{
		MainFrame* mf = dynamic_cast<MainFrame*>(CCBManager::Get()->getPage("MainFrame"));
		if(mf)
		{
			mf->pushPage("BuyPropPopupPage");
		}
		MsgBuyPropPopupPage msg;
		msg.toolId = mTrumpetId;
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onMsgSendButton")
	{
		if(!mCanSendMsg)
		{
			MainFrame* mf = dynamic_cast<MainFrame*>(CCBManager::Get()->getPage("MainFrame"));
			if(mf)
			{
				mf->pushPage("BuyPropPopupPage");
			}
			MsgBuyPropPopupPage msg;
			msg.toolId = mTrumpetId;
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			libOS::getInstance()->showInputbox(true);
		}
	}
}

void ChatPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_CHAT_PUSHRET_S)
		{
			showChatContentView();
		}
		else if(msg->opcode==OPCODE_BUY_TOOLSRET_S)
		{
			refreshTrumpetNum();
		}
		else if(msg->opcode==OPCODE_CHAT_SEND_MSGRET_S)
		{
			refreshTrumpetNum();
			mSurplusSecTime=ChatNextTime;
			mDurationTime=0.0f;
			_refreshSendBtnStatus();
			showChatContentView();
		}
	}
}