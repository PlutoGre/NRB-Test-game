
#include "stdafx.h"

#include "PopUpSurprisePage.h"
#include "Equip.h"
#include "DataTableManager.h"
#include "StringConverter.h"
#include "GameMessages.h"
#include "BlackBoard.h"
#include "PopupManager.h"
#include "GameMaths.h"
REGISTER_PAGE("PopUpSurprisePage",PopUpSurprisePage);

void PopUpSurprisePage::Enter( MainFrame* )
{
	updatePage();
}

void PopUpSurprisePage::Execute( MainFrame* )
{

}

void PopUpSurprisePage::Exit( MainFrame* )
{

}

void PopUpSurprisePage::load( void )
{
	loadCcbiFile("PopUpSurprise.ccbi");
}

void PopUpSurprisePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onFunction1")
	{
		switch (mType)
		{
		case GetPropManager::EQUIPMENT:
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "EquipPage";
				MessageManager::Get()->sendMessage(&msg);
				break;
			}
		case GetPropManager::TOOL:
			{
				MsgMainFrameChangePage msg;
				msg.pageName = "PackagePage";
				MessageManager::Get()->sendMessage(&msg);
				break;
			}
		case GetPropManager::TOOL_USE:
			
			break;

		case GetPropManager::SKILL:
			{

				MsgMainFrameChangePage msg;
				msg.pageName = "SkillPage";
				MessageManager::Get()->sendMessage(&msg);
				break;
			}

		case GetPropManager::SOUL:
			{
				BlackBoard::Get()->DisciplePageTab = 2;
				MsgMainFrameChangePage msg1;
				msg1.pageName = "DisciplePage";
				MessageManager::Get()->sendMessage(&msg1);
				break;
			}
		}
	}
	//else if(itemName == "Function2")
	
	{
		MsgMainFramePopPage msg;
		msg.pageName = "PopUpSurprisePage";
		MessageManager::Get()->sendMessage(&msg);
		PopupManager::Get()->popPage();
	}
}



void PopUpSurprisePage::updatePage()
{
	std::string mSummary = Language::Get()->getString("@PopUpContulation");
	std::string mName;
	std::string mNumber = (mCount>=1?StringConverter::toString(mCount):"");
	std::string mDescription;
	std::string mNote1 = "";
	std::string mFunctionLable1;
	std::string mFunctionLable2 = Language::Get()->getString("@Close");
	std::string mIcon;
	std::string mNumber1=StringConverter::toString(mCount);
	switch(mType)
	{
	case GetPropManager::EQUIPMENT:
		{
			Equip eqp(mId);
			const EquipItem* item =  EquipTableManager::Get()->getEquipItemByID(eqp.itemID());
			if(item)
			{
				mName = item->name;
				mDescription = item->describe;
				mFunctionLable1 = Language::Get()->getString("@PopUpToEquipList");
				mIcon = item->iconPic;
				mNumber1=StringConverter::toString(eqp.quantity());
			}
			break;
		}

	case GetPropManager::TOOL:
	case GetPropManager::TOOL_USE:
		{
			UserToolInfo* info = ServerDateManager::Get()->getUserToolInfoByID(mId);
			if(info)
			{
				const ToolItem* item = ToolTableManager::Get()->getToolItemByID(info->itemid);
				if(item)
				{
					mName = item->name;
					mDescription = item->describe;
					mIcon = item->iconPic;
					if(item->operationType == 0)
					{
						mFunctionLable1 = Language::Get()->getString("@PopUpToToolList");
						mType = GetPropManager::TOOL;
					}
					else
					{
						mFunctionLable1 = Language::Get()->getString("@PopUpToToolUse");
						mType = GetPropManager::TOOL_USE;
					}

				}
			}
		}
		break;

	case GetPropManager::SKILL:
		{
			SkillInfo* info = ServerDateManager::Get()->getSkillInfoByID(mId);
			if(info)
			{
				const SkillItem* item = SkillTableManager::Get()->getSkillItemByID(info->itemid);
				if(item)
				{
					mName = item->name;
					mDescription = item->describe;
					mFunctionLable1 = Language::Get()->getString("@PopUpToSkillList");
					mIcon = item->iconPic;
					mNumber1=StringConverter::toString(info->quantity);
				}
			}
			break;
		}

	case GetPropManager::SOUL:
		{
			const UserSoulInfo * info = ServerDateManager::Get()->getUserSoulInfoByID(mId);
			if(info)
			{
				const DiscipleItem* item = DiscipleTableManager::Get()->getDiscipleItemByID(info->itemid);
				if(item)
				{
					mName = item->name+Language::Get()->getString("@MPTabSoul");
					mDescription = item->describe;
					mFunctionLable1 = Language::Get()->getString("@PopUpToSoulList");
					mIcon = item->iconPic;
					mNumber1=StringConverter::toString(info->count);
				}
			}
			break;
		}
	}
			 
	mSummary.append(mName);
	CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@SurpriseTitle").c_str()));
	CCB_FUNC(this,"mSummary",CCLabelBMFont,setString(mSummary.c_str()));
	CCB_FUNC(this,"mName",CCLabelBMFont,setString(mName.c_str()));
	CCB_FUNC(this,"mNumber",CCLabelBMFont,setString(mNumber1.c_str()));
	CCB_FUNC(this,"mNumber1",CCLabelBMFont,setString(mNumber.c_str()));
	std::string des;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("SurpriseDescription"));
	GameMaths::stringAutoReturn(Language::Get()->getString(mDescription),des,width,lines);
	CCB_FUNC(this,"mDescription",CCLabelBMFont,setString(des.c_str()));
	width = StringConverter::parseInt(VaribleManager::Get()->getSetting("SurpriseNode"));
	GameMaths::stringAutoReturn(Language::Get()->getString(mNote1),des,width,lines);
	CCB_FUNC(this,"mNote1",CCLabelBMFont,setString(des.c_str()));
	CCB_FUNC(this,"mFunctionLable1",CCLabelBMFont,setString(mFunctionLable1.c_str()));
	CCB_FUNC(this,"mFunctionLable2",CCLabelBMFont,setString(mFunctionLable2.c_str()));
	CCB_FUNC(this,"mPic",CCSprite,setTexture(mIcon.c_str()));
}

