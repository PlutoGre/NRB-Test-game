
#include "stdafx.h"

#include "DiscipleUpgradePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "MessageBoxPage.h"
#include "GamePackets.h"

REGISTER_PAGE("DiscipleUpgradePage",DiscipleUpgradePage);
USING_NS_CC;

DiscipleUpgradePage::DiscipleUpgradePage(void)
{

}

DiscipleUpgradePage::~DiscipleUpgradePage(void)
{
}

void DiscipleUpgradePage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	refreshPage();
}

void DiscipleUpgradePage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);;
}

void DiscipleUpgradePage::Execute( MainFrame* )
{

}

void DiscipleUpgradePage::load( void )
{
	loadCcbiFile("MemberBreakThrough.ccbi");
}

void DiscipleUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onQuit")
	{
		BlackBoard::Get()->isUpgradeDisciple = false;
		BlackBoard::Get()->UpgradeDiscipleBeforePotential = 0;
		BlackBoard::Get()->DisciplePageTab = 2;
		BlackBoard::Get()->UpgradeSoul = 0;
		MsgMainFrameChangePage msg;
		msg.pageName = "DisciplePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onBreakThrough")
	{

		Soul soul(BlackBoard::Get()->UpgradeSoul);
		Disciple disciple(soul.getDiscipleID());
		if(disciple.upgradelevel()<MAX_DISCIPLE_UPGRADE_LEVEL)
		{
			if(soul.count()<soul.getNeededNum())
			{
				MSG_BOX(Language::Get()->getString("@SoulNotEnough").c_str());
				return;
			}
			BlackBoard::Get()->UpgradeDiscipleBeforePotential = disciple.potentiality();
			BlackBoard::Get()->isUpgradeDisciple = true;

			OPUpgradeDisciple upgradeDisciple;
			upgradeDisciple.set_id(soul.getDiscipleID());
			PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_DISCIPLE_C,&upgradeDisciple);
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@DiscipleAlreadyUpgradeMaxLevel").c_str());
		}
		
	}
}

void DiscipleUpgradePage::refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	if(name)
	{
		name->setString(Language::Get()->getString(userBasicInfo.name).c_str());
	}
	CCLabelBMFont* level = dynamic_cast<CCLabelBMFont* >(getVariable("mPlayerLv"));
	if(level)
	{
		level->setString(StringConverter::toString(userBasicInfo.level).c_str());
	}
	CCLabelBMFont* gold = dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	if(gold)
	{
		gold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	}

	Soul soul(BlackBoard::Get()->UpgradeSoul);
	Disciple disciple(soul.getDiscipleID());
	CCSprite* headPic=dynamic_cast<CCSprite* >(getVariable("mHeadPic"));
	if(headPic)
	{
		headPic->initWithFile(disciple.iconPic().c_str());
	}
	CCSprite* frame=dynamic_cast<CCSprite* >(getVariable("mFrame"));
	if(frame)
	{
		frame->setColor(disciple.getFrameColor());
	}
	CCLabelBMFont* memberName=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName"));
	if(memberName)
	{
		memberName->setString(Language::Get()->getString(disciple.name()).c_str());
	}
	CCSprite* memberQuality=dynamic_cast<CCSprite* >(getVariable("mMemQuality"));
	if(memberQuality)
	{
		memberQuality->initWithFile(disciple.getQualityImageFile().c_str());
	}
	CCLabelBMFont* memberLV=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV"));
	if(memberLV)
	{
		memberLV->setString(StringConverter::toString(disciple.level()).c_str());
	}
	if(BlackBoard::Get()->isUpgradeDisciple)
	{
		unsigned int beforePotential = BlackBoard::Get()->UpgradeDiscipleBeforePotential;
		CCLabelBMFont* potential=dynamic_cast<CCLabelBMFont* >(getVariable("mPotential"));
		if(potential)
		{
			potential->setString(StringConverter::toString(beforePotential).c_str());
		}
		CCLabelBMFont* potentialAdd=dynamic_cast<CCLabelBMFont* >(getVariable("mPotentialAdd"));
		if(potentialAdd)
		{
			potentialAdd->setVisible(true);
			potentialAdd->setString(("+" + StringConverter::toString(disciple.potentiality() - beforePotential)).c_str());
		}
	}
	else
	{
		CCLabelBMFont* potential=dynamic_cast<CCLabelBMFont* >(getVariable("mPotential"));
		if(potential)
		{
			potential->setString(StringConverter::toString(disciple.potentiality()).c_str());
		}
		CCLabelBMFont* potentialAdd=dynamic_cast<CCLabelBMFont* >(getVariable("mPotentialAdd"));
		if(potentialAdd)
		{
			potentialAdd->setVisible(false);
		}
	}
	
	CCLabelBMFont* upgradeLevel=dynamic_cast<CCLabelBMFont* >(getVariable("mLevel"));
	if(upgradeLevel)
	{
		upgradeLevel->setString(StringConverter::toString(disciple.upgradelevel()).c_str());
	}
	CCLabelBMFont* soulOwn=dynamic_cast<CCLabelBMFont* >(getVariable("mSoulOwn"));
	if(soulOwn)
	{
		soulOwn->setString(StringConverter::toString(soul.count()).c_str());
	}
	CCLabelBMFont* soulNeeded=dynamic_cast<CCLabelBMFont* >(getVariable("mSoulNeeded"));
	if(soulNeeded)
	{
		soulNeeded->setString(StringConverter::toString(soul.getNeededNum()).c_str());
	}
}

void DiscipleUpgradePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "DiscipleUpgradePage")
		{
			refreshPage();
		}
	}
}
