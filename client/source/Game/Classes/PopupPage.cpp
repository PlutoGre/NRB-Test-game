
#include "stdafx.h"

#include "PopupPage.h"
#include "BlackBoard.h"
#include "PopupNotEnoughPage.h"
REGISTER_PAGE("PopupPage",PopupPage);
USING_NS_CC;
PopupPage::PopupPage(void)
{

}

PopupPage::~PopupPage(void)
{
}

void PopupPage::load( void )
{
	loadCcbiFile("PopUp.ccbi");
	mMsg=dynamic_cast<CCLabelBMFont* >(getVariable("mPopMsg"));
	mTitle=dynamic_cast<CCLabelBMFont* >(getVariable("mTitle"));
	mFunction1=dynamic_cast<CCLabelBMFont* >(getVariable("mFunction1"));
	mFunction2=dynamic_cast<CCLabelBMFont* >(getVariable("mFunction"));
}

void PopupPage::showPage(Popup_Page_Type _type)
{
	MsgMainFramePushPage msg;
	msg.pageName="PopupPage";
	MessageManager::Get()->sendMessage(&msg);
	BasePage* obj = CCBManager::Get()->getPage("PopupPage");
	PopupPage* page = dynamic_cast<PopupPage*>(obj);
	if(page)
	{
		page->mType=_type;
	}
}

void PopupPage::initPage()
{
	if(mType==SnatchSkillBookFightWin)
	{
		mMsgKey="@SnatchSkillBookMsgFightWin";
		mTitleKey="@SnatchSkillBookTitleFail";
		mFunction1Key="@Close";
		mFunction2Key="@SnatchAgain";
		MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	}
	else if(mType==SnatchSkillBookFightFail)
	{
		mMsgKey="@SnatchSkillBookMsgFightFail";
		mTitleKey="@SnatchSkillBookTitleFail";
		mFunction1Key="@Close";
		mFunction2Key="@SnatchAgain";
		MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	}
	else if(mType==CareerAttackNumLimit)
	{
		mMsgKey="@CareerAttackNumLimitMsg";
		mTitleKey="@CareerAttackNumLimitTitle";
		mFunction1Key="@Close";
		mFunction2Key="@Confirm";
	}
	else if(mType==NotEnoughGold)
	{
		mMsgKey="@BuyFailMsg";
		mTitleKey="@BUYNotEnoughRMB";
		mFunction1Key="@Close";
		mFunction2Key="@Recharge";
	}
	else if(mType==ChangeDiscipleConfirm)
	{
		mMsgKey="@ChangeDiscipleConfirmTitle";
		mTitleKey="@ChangeDiscipleConfirmMsg";
		mFunction1Key="@Close";
		mFunction2Key="@ChangeDisciple";
	}
	else if(mType==CareerAttackNumVipTip)
	{
		mMsgKey="@CareerAttackNumVipTip";
		mTitleKey="@CareerAttackNumLimitTitle";
		mFunction1Key="@Close";
		mFunction2Key="@Recharge";
	}
	else if(mType==Script)
	{
	}
		
}
void PopupPage::Enter( MainFrame* )
{
	_refreshPage();
}

void PopupPage::_refreshPage()
{
	initPage();
	if(mMsg)
	{
		std::string outstr;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("PopupPage_Msg"));
		if(mType==CareerAttackNumLimit)
		{
			std::list<std::string> strList;
			CareerData::CustomsPassData* cData= CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(BlackBoard::Get()->getCareerAttackChapterId(),BlackBoard::Get()->getCareerAttackCustomsId());
			strList.push_back(StringConverter::toString(cData->limitAttackNum));
			strList.push_back(StringConverter::toString(3));
			if(cData->monsterType==CareerData::BOSS)
			{
				strList.push_back("10");
			}
			else
			{
				strList.push_back("5");
			}
			GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString(mMsgKey),&strList),outstr,width,lines);
		}
		else if(mType==CareerAttackNumVipTip)
		{
			char buff[128];
			CareerData::CustomsPassData* cData= CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(BlackBoard::Get()->getCareerAttackChapterId(),BlackBoard::Get()->getCareerAttackCustomsId());
			if(cData->monsterType==CareerData::BOSS)
			{
				sprintf(buff,Language::Get()->getString(mMsgKey).c_str(),10);
			}
			else
			{
				sprintf(buff,Language::Get()->getString(mMsgKey).c_str(),5);
			}

			GameMaths::stringAutoReturn(buff,outstr,width,lines);
		}
		else
		{
			GameMaths::stringAutoReturn(Language::Get()->getString(mMsgKey),outstr,width,lines);
		}
		mMsg->setString(outstr.c_str());
	}
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString(mTitleKey).c_str());
	}
	if(mFunction1)
	{
		mFunction1->setString(Language::Get()->getString(mFunction1Key).c_str());
	}
	if(mFunction2)
	{
		mFunction2->setString(Language::Get()->getString(mFunction2Key).c_str());
	}
}

void PopupPage::Execute( MainFrame* )
{
	
}

void PopupPage::Exit( MainFrame* )
{
	MessageManager::Get()->removeMessageHandler(this);
}

void PopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	bool isClose=false;
	if(itemName=="onClose")
	{
		isClose=true;
	}
	else if(itemName=="onXClose")
	{
		isClose=true;
	}
	else if(itemName=="onFunction1")
	{
		if(mType==SnatchSkillBookFightWin||mType==SnatchSkillBookFightFail)
		{
			if(ServerDateManager::Get()->getUserBasicInfo().vitality<=0)
			{
				POP_NOT_ENOUGH_BOX(VitalityNotEnough);
				isClose=true;
			}
			else
			{
				OPSnatchSkillBook msg;
				msg.set_version(1);
				msg.set_playerid(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->playerId);
				msg.set_bookid(BlackBoard::Get()->getSkillBookId());
				msg.set_isnpc(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->isNpc);
				PacketManager::Get()->sendPakcet(OPCODE_SNATCH_SKILL_BOOK_INFO_C, &msg);
				isClose=false;
			}
		}
		else if(mType == Script && mFunScriptHandle !=0)
		{
			CCScriptEngineManager::sharedManager()->getScriptEngine()->executeEvent(mFunScriptHandle, "onFunction1");
		}
	}
	else if(itemName=="onFunction2")
	{
		if(mType==SnatchSkillBookFightWin||mType==SnatchSkillBookFightFail)
		{
			if(ServerDateManager::Get()->getUserBasicInfo().vitality<=0)
			{
				POP_NOT_ENOUGH_BOX(VitalityNotEnough);
				isClose=true;
			}
			else
			{
				OPSnatchSkillBook msg;
				msg.set_version(1);
				msg.set_playerid(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->playerId);
				msg.set_bookid(BlackBoard::Get()->getSkillBookId());
				msg.set_isnpc(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->isNpc);
				PacketManager::Get()->sendPakcet(OPCODE_SNATCH_SKILL_BOOK_INFO_C, &msg);
				isClose=false;
			}
		}
		else if(mType==CareerAttackNumLimit)
		{
			if(ServerDateManager::Get()->getUserBasicInfo().viplevel>=3)
			{
				int _coins=5;
				CareerData::CustomsPassData* cData= CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(BlackBoard::Get()->getCareerAttackChapterId(),BlackBoard::Get()->getCareerAttackCustomsId());
				if(cData->monsterType==CareerData::BOSS)
				{
					_coins=10;
				}
				else
				{
					_coins=5;
				}
				if(ServerDateManager::Get()->getUserBasicInfo().goldcoins>=_coins)
				{
					isClose=true;
					OPResetCareerCount msg;
					msg.set_version(1);
					msg.set_id(BlackBoard::Get()->CareerAttackCustomsId);
					PacketManager::Get()->sendPakcet(OPCODE_RESET_CAREER_COUNT_C, &msg);
				}
				else
				{
					mType=NotEnoughGold;
					_refreshPage();
				}
			}
			else
			{
				isClose=true;
				MsgMainFramePushPage msg;
				msg.pageName = "RechargePage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
		else if(mType==NotEnoughGold)
		{
			isClose=true;
			MsgMainFramePushPage msg;
			msg.pageName = "RechargePage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else if(mType==ChangeDiscipleConfirm)
		{
			isClose=true;
			std::string key="changeDiscipleConfirmKey";
			if(HAS_V(key))
			{
				OPChangeDisciple changeDisciple;
				changeDisciple.set_discipleid(GET_V_UINT(key));
				changeDisciple.set_position( BlackBoard::Get()->CurBattlePosition );
				PacketManager::Get()->sendPakcet(OPCODE_CHANGE_DISCIPLE_C,&changeDisciple);
			}
		}
		else if(mType==CareerAttackNumVipTip)
		{
			isClose=true;
			MsgMainFramePushPage msg;
			msg.pageName = "RechargePage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else if(mType == Script && mFunScriptHandle !=0)
		{
			CCScriptEngineManager::sharedManager()->getScriptEngine()->executeEvent(mFunScriptHandle, "onFunction2");
		}
	}
	if(isClose)
	{
		MsgMainFramePopPage msg;
		msg.pageName="PopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}



void PopupPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

void PopupPage::onReceiveMassage(const GameMessage * message)
{
	if(message->getTypeId()==MSG_FIGHT_END)
	{
		bool isClose=true;
		if(mType==SnatchSkillBookFightWin||mType==SnatchSkillBookFightFail)
		{
			std::string key="snatchSkillBookRetKey";
			unsigned int ret=GET_V_UINT(key);
			if(ret==1)
			{
				MsgMainFramePushPage pushMsg;
				pushMsg.pageName = "ObtainSkillBookPage";
				MessageManager::Get()->sendMessage(&pushMsg);
			}
			else
			{
				if(ret==0)
				{
					POP_BOX(SnatchSkillBookFightWin);
				}
				else
				{
					POP_BOX(SnatchSkillBookFightFail);
				}
				isClose=false;
			}
		}
		if(isClose)
		{
			MsgMainFramePopPage msg;
			msg.pageName = "PopupPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
}
void PopupPage::Pop_Box( Popup_Page_Type _type )
{
	dynamic_cast<PopupPage*>(CCBManager::Get()->getPage("PopupPage"))->showPage(_type);
}

void PopupPage::Pop_Box(std::string msgKey, std::string titleKey, std::string function1Key, std::string function2Key, int fun1handle)
{
	dynamic_cast<PopupPage*>(CCBManager::Get()->getPage("PopupPage"))->showPage(Script, msgKey, titleKey, function1Key ,function2Key ,fun1handle);
}

void PopupPage::showPage( Popup_Page_Type _type, std::string msgKey, std::string titleKey, std::string function1Key, std::string function2Key, int fun1handle )
{
	mMsgKey="@BuyFailMsg";
	mTitleKey="@BuyFailTitle";
	mFunction1Key="@Close";
	mFunction2Key="@Recharge";

	mFunScriptHandle = fun1handle;

	showPage(_type);
}
