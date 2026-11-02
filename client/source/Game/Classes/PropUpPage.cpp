
#include "stdafx.h"

#include "PropUpPage.h"
#include "GameMessages.h"

REGISTER_PAGE("PropUpPage",PropUpPage);

PropUpPage::PropUpPage(void)
{
}


PropUpPage::~PropUpPage(void)
{
}

void PropUpPage::Enter( MainFrame* )
{
	
	switch(mUsage)
	{
	case FOR_SHOP_BUYTOOL:
		CCB_FUNC(this,"mFunction1",CCLabelBMFont,setString(Language::Get()->getString("@MSGBOXCheckView").c_str()));//left
		CCB_FUNC(this,"mFunction",CCLabelBMFont,setString(Language::Get()->getString("@MSGBOXGoOnShopping").c_str()));//right
		CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@MSGBOXBuyingDone").c_str()));
		CCB_FUNC(this,"mPopMsg",CCLabelBMFont,setString(Language::Get()->getString("@MSGBOXBuyDoneStat").c_str()));
		break;
	}
		
}

void PropUpPage::Execute( MainFrame* )
{

}

void PropUpPage::Exit( MainFrame* )
{
	
}

void PropUpPage::load( void )
{
	//loadCcbiFile("PopUp.ccbi");
	loadCcbiFile("PopUpN.ccbi");
}

void PropUpPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose")//left
	{
		switch(mUsage)
		{
			case FOR_SHOP_BUYTOOL:
	// 			MsgMainFrameChangePage msg;
	// 			msg.pageName = "PackagePage";
	// 			MessageManager::Get()->sendMessage(&msg);
				break;
		}
	}
	else if(itemName=="onViewPackage")
	{
		switch(mUsage)
		{
		case FOR_SHOP_BUYTOOL:
			MsgMainFrameChangePage msg;
			msg.pageName = "PackagePage";
			MessageManager::Get()->sendMessage(&msg);
			break;
		} 
	}
	else if(itemName == "onFunction2" || itemName == "onXClose")//right
	{

	}
	MsgMainFramePopPage msg;
	msg.pageName = "PropUpPage";
	MessageManager::Get()->sendMessage(&msg);
}
