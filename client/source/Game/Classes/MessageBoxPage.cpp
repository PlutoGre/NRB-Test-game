
#include "stdafx.h"

#include "MessageBoxPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "CCBManager.h"
#include "Language.h"
#include "GameMessages.h"

USING_NS_CC;
MessageBoxPage::MessageBoxPage(void)
{

}

MessageBoxPage::~MessageBoxPage(void)
{
}

void MessageBoxPage::load( void )
{
	loadCcbiFile("MessageBox.ccbi");
}

void MessageBoxPage::showMsg( std::string msgString )
{
	load();
	CCLabelBMFont* message=dynamic_cast<CCLabelBMFont* >(getVariable("mMsg"));
	if(message)
	{
		message->setString(Language::Get()->getString(msgString).c_str());
	}

	MsgMainFrameMSGShow msg;
	MessageManager::Get()->sendMessage(&msg);
}

void MessageBoxPage::Enter( MainFrame* )
{
	this->stopAllActions();
	this->setPosition(0, 0);
	this->runAction(CCSequence::create(CCShow::create(),
		CCMoveBy::create(0.3f, CCPoint(0,20)), 
		CCDelayTime::create(1.0f),  
		CCHide::create(), 
		NULL));
}

void MessageBoxPage::Execute( MainFrame* )
{

}

void MessageBoxPage::Exit( MainFrame* )
{

}

void MessageBoxPage::Msg_Box(std::string msgString)
{
	MessageBoxPage::Get()->showMsg(msgString);
}

void MessageBoxPage::Msg_Box_Lan( std::string msgString )
{
	MessageBoxPage::Get()->showMsg(Language::Get()->getString(msgString));
}
