#include "ActivityPopPage.h"
#include "GameMessages.h"
#include "Disciple.h"
#include "DiscipleHandInfoPage.h"
#include"GameMaths.h"

REGISTER_PAGE("ActivityPopPage",ActivityPopPage);
USING_NS_CC;
ActivityPopPage::ActivityPopPage(void)
{
}


ActivityPopPage::~ActivityPopPage(void)
{
}

void ActivityPopPage::load( void )
{
	loadCcbiFile("ActivityPopWindow.ccbi");
}

void ActivityPopPage::Enter( MainFrame* )
{
	_refreshPage();
}

void ActivityPopPage::Execute( MainFrame* )
{

}

void ActivityPopPage::Exit( MainFrame* )
{
	
}

void ActivityPopPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onPopInfo")
	{
		if(mType==SendSoulTwo||mType==SendSoulThree)
		{
			const ActivityPopItem* item = ActivityPopTableManager::Get()->getActivityPopDataByID(mType);
			ShowDiscipleInfoPopup(item->referenceID,true);
		}
	}
	MsgMainFramePopPage msg;
	msg.pageName = "ActivityPopPage";
	MessageManager::Get()->sendMessage(&msg);
}

void ActivityPopPage::showPage(Activity_Pop_Type _type)
{
	BasePage* obj = CCBManager::Get()->getPage("ActivityPopPage");
	ActivityPopPage* page = dynamic_cast<ActivityPopPage*>(obj);
	if(page)
	{
		page->mType=_type;
	}
	MsgMainFramePushPage msg;
	msg.pageName="ActivityPopPage";
	MessageManager::Get()->sendMessage(&msg);
	
}

void ActivityPopPage::_refreshPage()
{
	if(mType==SendSoulTwo||mType==SendSoulThree)
	{
		const ActivityPopItem* item = ActivityPopTableManager::Get()->getActivityPopDataByID(mType);
		if(item!=NULL)
		{
			Disciple disciple(item->referenceID,true);
			CCB_FUNC(this,"mFrame",CCMenuItemImage,setNormalImage(::getFrameNormalSpirte(disciple.quality()>0?disciple.quality():4)));
			CCB_FUNC(this,"mFrame",CCMenuItemImage,setSelectedImage(::getFrameNormalSpirte(disciple.quality()>0?disciple.quality():4)));
			CCB_FUNC(this,"mPopHead",CCSprite,setTexture(disciple.iconPic().c_str()));
			CCB_FUNC(this,"mPopName",CCLabelBMFont,setString(item->name.c_str()));
			
			std::string s;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("ActivityPopPage_Describe"));
			GameMaths::stringAutoReturn(item->describe, s, width, lines);
			CCB_FUNC(this,"mPopTex",CCLabelBMFont,setString(s.c_str()));
		}
		
	}
}