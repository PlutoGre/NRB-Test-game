
#include "stdafx.h"

#include "SettingsPage.h"
#include "Language.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "SoundManager.h"
#include "lib91.h"
#include "MessageBoxPage.h"


REGISTER_PAGE("SettingsPage",SettingsPage);
USING_NS_CC;

SettingsPage::SettingsPage(void)
{
}


SettingsPage::~SettingsPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}


void SettingsPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	cocos2d::extension::CCScrollView* settingsScrollView =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("settings"));
	if(settingsScrollView)
	{
		cocos2d::CCNode* node=cocos2d::CCNode::create();
		CCSize size=CCSize(0,0);
		scrollViewContent=CCBContainer::create();
		scrollViewContent->loadCcbiFile("settingMember.ccbi");
		scrollViewContent->setListener(this);
		std::string s("@SettingOpenLabel");
		const UserSettingInfo settingInfo=ServerDateManager::Get()->getUserSettingInfo();
		if(scrollViewContent)
		{
			bool isOpen=false;
			if(settingInfo.music==1)
			{
				isOpen=true;
			}
			SettingItemButtonAndLabel("Music",true,isOpen);
			isOpen=false;
			if(settingInfo.sound==1)
			{
				isOpen=true;
			}
			SettingItemButtonAndLabel("Sound",true,isOpen);
			isOpen=false;
			if(settingInfo.chat==1)
			{
				isOpen=true;
			}
			SettingItemButtonAndLabel("Chat",true,isOpen);

			SettingItemButton("Activity");
			SettingItemButton("Help");
			SettingItemButton("FeedBack");
			SettingItemButton("About");

			scrollViewContent->setPosition(ccp(0,size.height));
			size.width = scrollViewContent->getContentSize().width;
			size.height += scrollViewContent->getContentSize().height;
			node->addChild(scrollViewContent);
		}
		node->setContentSize(size);
		settingsScrollView->setContainer(node);
		settingsScrollView->setContentSize(size);


		settingsScrollView->setContentOffset(ccp(0,settingsScrollView->minContainerOffset().y));
	}
	else
	{
		CCLOG("settingsScrollView not exist");
	}
	_refreshPage();
}

/************************************************************************/
/* 设置无小按钮节点                                                   */
/************************************************************************/
void SettingsPage::SettingItemButton(std::string type)
{
	std::string buttonLabel="mNodeShow"+type;
	CCMenuItemSprite* itemShowButton = dynamic_cast<CCMenuItemSprite*>(scrollViewContent->getVariable(buttonLabel));
	if(itemShowButton && itemShowButton->getParent())
	{
		CCMenu* menu = dynamic_cast<CCMenu*>(itemShowButton->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
	}
}

/************************************************************************/
/* 设置有小按钮节点                                                     */
/************************************************************************/
void SettingsPage::SettingItemButtonAndLabel(std::string type,bool scollViewChild, bool isOpen)
{
	std::string buttonLabel="@SettingOpenLabel";
	std::string nameLabel="m"+type+"ToggleNameLabel";
	std::string openLabel="m"+type+"Open";
	std::string closeLabel="m"+type+"Close";
	if(!isOpen)
	{
		buttonLabel="@SettingCloseLabel";
	}

	CCLabelBMFont* itemButtonLabel=dynamic_cast<CCLabelBMFont*>(scrollViewContent->getVariable((nameLabel)));
	if(itemButtonLabel)
	{
		itemButtonLabel->setString(Language::Get()->getString(buttonLabel).c_str());
	}
	CCMenuItemSprite* itemButtonOpen = dynamic_cast<CCMenuItemSprite*>(scrollViewContent->getVariable(openLabel));
	CCMenuItemSprite* itemButtonClose = dynamic_cast<CCMenuItemSprite*>(scrollViewContent->getVariable(closeLabel));
	if(scollViewChild)
	{
		CCMenu* itemMenu;

		if(itemButtonOpen && itemButtonOpen->getParent())
		{
			itemMenu = dynamic_cast<CCMenu*>(itemButtonOpen->getParent());
			if(itemMenu)
			{
				itemMenu->setScrollViewChild(true);
			}
		}
	}
	
	if(itemButtonOpen && itemButtonClose)
	{
		if(isOpen)
		{
			itemButtonOpen->setVisible(false);
			itemButtonClose->setVisible(true);
		}
		else
		{
			itemButtonOpen->setVisible(true);
			itemButtonClose->setVisible(false);
		}
	}
}

void SettingsPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void SettingsPage::Execute( MainFrame* )
{

}

void SettingsPage::load( void )
{
	loadCcbiFile("SettingsPage.ccbi");
}

void SettingsPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	bool isSendPacket=false;
	std::string type;
	unsigned int oprRet=0;
	UserSettingInfo& settingInfo=ServerDateManager::Get()->getUserSettingInfo();
	if(itemName == "onSettingMusicOpen")
	{
		SoundManager::Get()->setMusicOn(true);
		isSendPacket=true;
		type="music";
		oprRet=1;
		settingInfo.music=oprRet;
		SettingItemButtonAndLabel("Music",false,true);
	}
	else if(itemName == "onSettingMusicClose")
	{
		SoundManager::Get()->setMusicOn(false);
		isSendPacket=true;
		type="music";
		oprRet=0;
		settingInfo.music=oprRet;
		SettingItemButtonAndLabel("Music",false,false);
	}
	else if(itemName == "onSettingSoundOpen")
	{
		SoundManager::Get()->setEffectOn(true);
		isSendPacket=true;
		type="sound";
		oprRet=1;
		settingInfo.sound=oprRet;
		SettingItemButtonAndLabel("Sound",false,true);
	}
	else if(itemName == "onSettingSoundClose")
	{
		SoundManager::Get()->setEffectOn(false);
		isSendPacket=true;
		type="sound";
		oprRet=0;
		settingInfo.sound=oprRet;
		SettingItemButtonAndLabel("Sound",false,false);
	}
	else if(itemName == "onSettingChatOpen")
	{
		isSendPacket=true;
		type="chat";
		oprRet=1;
		settingInfo.chat=oprRet;
		SettingItemButtonAndLabel("Chat",false,true);
	}
	else if(itemName == "onSettingChatClose")
	{
		isSendPacket=true;
		type="chat";
		oprRet=0;
		settingInfo.chat=oprRet;
		SettingItemButtonAndLabel("Chat",false,false);
	}
	else if(itemName=="onShowActivityButton")
	{
		if(VaribleManager::Get()->getSetting("AnnouncementPageOpenStatus")=="1")
		{
			MsgMainFramePushPage msg;
			msg.pageName="AnnouncementPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@NoAnnouncement").c_str());
		}
	}
	else if(itemName=="onShowHelpButton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName="HelpPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowFeedBackButton")
	{
// 		CCLOG("onMenuItemAction onShowFeedBackButton");
// 		MainFrame* mf = dynamic_cast<MainFrame*>(CCBManager::Get()->getPage("MainFrame"));
// 		if(mf)
// 		{
// 			mf->pushPage("LeaveMsgPage");
// 		}
		std::string mail = VaribleManager::Get()->getSetting("FeedBack_email");
		std::string title = VaribleManager::Get()->getSetting("FeedBack_title");
		std::string body = VaribleManager::Get()->getSetting("FeedBack_body");
		//lib91::getInstance()->mailto(mail,"",title,body);
        
        if((libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="PP") ||(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="AppleGarden")||(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="TB")||(libPlatformManager::getInstance()->getPlatform()->getClientChannel()=="ITO") )
        {
            MsgMainFrameChangePage msg;
            msg.pageName="AboutPage";
            MessageManager::Get()->sendMessage(&msg);
        }
        else
            libPlatformManager::getPlatform()->userFeedBack();
        
	}
	else if(itemName=="onShowAboutButton")
	{
		MsgMainFrameChangePage msg;
		msg.pageName="AboutPage";
		MessageManager::Get()->sendMessage(&msg);
	}
    else if(itemName=="onShowCommunity")
	{
        libPlatformManager::getPlatform()->switchUsers();
	}
	else if(itemName=="onInputCDKey")
	{
		MsgMainFramePushPage msg;
		msg.pageName="CDKeyMsgPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	if(isSendPacket)
	{
		OPChangeSetting info;
		info.set_version(1);
		info.set_type(type);
		info.set_oprret(oprRet);
		PacketManager::Get()->sendPakcet(OPCODE_CHANGE_SETTING_C,&info);
	}
}

void SettingsPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_CHANGE_SETTINGRET_S)
		{

		}
		if(msg->opcode==OPCODE_GET_USER_BASIC_INFORET_S)
		{
			_refreshPage();
		}
	}
}

void SettingsPage::_refreshPage()
{
	const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
	CCLabelBMFont* mCoin=dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	CCLabelBMFont* mGold=dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	CCLabelTTF* mPlayerName=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	CCLabelBMFont* mLevel=dynamic_cast<CCLabelBMFont* >(getVariable("mPlayerLv"));
	if(mCoin)
	{
		mCoin->setString(StringConverter::toString(info.silvercoins).c_str());
	}
	if(mGold)
	{
		mGold->setString(StringConverter::toString(info.goldcoins).c_str());
	}
	if(mPlayerName)
	{
		mPlayerName->setString(info.name.c_str());
	}
	if(mLevel)
	{
		mLevel->setString(StringConverter::toString(info.level).c_str());
	}
}