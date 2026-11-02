
#include "stdafx.h"

#include "TranExpResult.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "MessageBoxPage.h"
#include "Disciple.h"

REGISTER_PAGE("TranExpResultPage",TranExpResultPage);
USING_NS_CC;

TranExpResultPage::TranExpResultPage(void)
{

}

TranExpResultPage::~TranExpResultPage(void)
{
}

void TranExpResultPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void TranExpResultPage::Exit( MainFrame* frame)
{

}

void TranExpResultPage::Execute( MainFrame* )
{

}

void TranExpResultPage::load( void )
{
	loadCcbiFile("TranExpResult.ccbi");
}

void TranExpResultPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onChooseMember")
	{
		BlackBoard::Get()->TransferExp_Provider = 0;
		MsgMainFrameChangePage msg;
		msg.pageName = "TranExpChooseMemberPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onQuit")
	{
		BlackBoard::Get()->TransferExp_Receiver = 0;
		BlackBoard::Get()->TransferExp_Provider = 0;
		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpDisciplePageName;
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onTransferEXP")
	{
		if(BlackBoard::Get()->TransferExp_Provider != 0)
		{
			if(BlackBoard::Get()->TransferExp_Receiver == 0)
				return;

			Disciple disciple(BlackBoard::Get()->TransferExp_Receiver);
			
			if(disciple.canUpgradeLevel())
			{
				BlackBoard::Get()->TransferExp_Provider = 0;
				BlackBoard::Get()->TransferBeforeBlood = disciple.health();
				BlackBoard::Get()->TransferBeforeAttack = disciple.attack();
				BlackBoard::Get()->TransferBeforeDefense = disciple.defence();
				BlackBoard::Get()->TransferBeforeRP = disciple.rp();
				BlackBoard::Get()->TransferBeforeLV = disciple.level();
				BlackBoard::Get()->TransferBeforeExp = disciple.getLevelExp();
				BlackBoard::Get()->TransferBeforePotential = disciple.potentiality();

				MsgMainFrameChangePage msg;
				msg.pageName = "TranExpResultPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{
				MSG_BOX(Language::Get()->getString("@CannotTranExp"));
			}
			
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@ChooseTranEXPDisciple"));
		}
	}
}

void TranExpResultPage::refreshPage( void )
{
	_refeshTitle();
	_refeshReceiver();
	_refeshBeforePro();
	_refeshAfterPro();
}

void TranExpResultPage::_refeshTitle( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mPlayerName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
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
}

void TranExpResultPage::_refeshReceiver( void )
{
	if(BlackBoard::Get()->TransferExp_Receiver == 0)
		return;

	Disciple disciple(BlackBoard::Get()->TransferExp_Receiver);
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
// 	CCSprite* headPic1=dynamic_cast<CCSprite* >(getVariable("mHeadPic1"));
// 	if(headPic1)
// 	{
// 		unsigned int prividerID = BlackBoard::Get()->TransferExp_Provider;
// 		if(prividerID != 0)
// 		{
// 			Disciple disciple_privider(prividerID);
// 			headPic1->initWithFile(prividerID.iconPic().c_str());
// 		}
// 		else
// 			headPic1->setVisible(false);
// 		
// 	}

	CCLabelBMFont* memberName=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName"));
	if(memberName)
	{
		memberName->setString(Language::Get()->getString(disciple.name()).c_str());
	}
	CCSprite* memberQuality=dynamic_cast<CCSprite* >(getVariable("mMemberQuality"));
	if(memberQuality)
	{
		memberQuality->initWithFile(disciple.getQualityImageFile().c_str());
	}
	CCLabelBMFont* memberLV=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV"));
	if(memberLV)
	{
		memberLV->setString(StringConverter::toString(disciple.level()).c_str());
	}
}

void TranExpResultPage::_refeshBeforePro( void )
{
	CCLabelBMFont* blood=dynamic_cast<CCLabelBMFont* >(getVariable("mBlood"));
	if(blood)
	{
		blood->setString(StringConverter::toString(BlackBoard::Get()->TransferBeforeBlood).c_str());
	}
	CCLabelBMFont* attack=dynamic_cast<CCLabelBMFont* >(getVariable("mAttack"));
	if(attack)
	{
		attack->setString(StringConverter::toString(BlackBoard::Get()->TransferBeforeAttack).c_str());
	}
	CCLabelBMFont* defense=dynamic_cast<CCLabelBMFont* >(getVariable("mDefense"));
	if(defense)
	{
		defense->setString(StringConverter::toString(BlackBoard::Get()->TransferBeforeDefense).c_str());
	}
	CCLabelBMFont* internalForce=dynamic_cast<CCLabelBMFont* >(getVariable("mInternalForce"));
	if(internalForce)
	{
		internalForce->setString(StringConverter::toString(BlackBoard::Get()->TransferBeforeRP).c_str());
	}
	CCLabelBMFont* memberLV1=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV1"));
	if(memberLV1)
	{
		memberLV1->setString(StringConverter::toString(BlackBoard::Get()->TransferBeforeLV).c_str());
	}

	if(BlackBoard::Get()->TransferExp_Receiver == 0)
		return;

	Disciple disciple(BlackBoard::Get()->TransferExp_Receiver);

	CCLabelBMFont* memberExp1=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberExp1"));
	if(memberExp1)
	{
		std::string exp("");
		exp +=StringConverter::toString(BlackBoard::Get()->TransferBeforeExp);
		exp +="/";
		exp += StringConverter::toString(disciple.getExpForLevelUpgrade(BlackBoard::Get()->TransferBeforeLV));
		memberExp1->setString(exp.c_str());
	}
	CCLabelBMFont* potential=dynamic_cast<CCLabelBMFont* >(getVariable("mPotential"));
	if(potential)
	{
		potential->setString(StringConverter::toString(BlackBoard::Get()->TransferBeforePotential).c_str());
	}
}

void TranExpResultPage::_refeshAfterPro( void )
{
	if(BlackBoard::Get()->TransferExp_Receiver == 0)
		return;

	Disciple disciple(BlackBoard::Get()->TransferExp_Receiver);
	CCLabelBMFont* blood1=dynamic_cast<CCLabelBMFont* >(getVariable("mBlood1"));
	if(blood1)
	{
		blood1->setString(StringConverter::toString(disciple.health()).c_str());
	}
	CCLabelBMFont* attack1=dynamic_cast<CCLabelBMFont* >(getVariable("mAttack1"));
	if(attack1)
	{
		attack1->setString(StringConverter::toString(disciple.attack()).c_str());
	}
	CCLabelBMFont* defense1=dynamic_cast<CCLabelBMFont* >(getVariable("mDefense1"));
	if(defense1)
	{
		defense1->setString(StringConverter::toString(disciple.defence()).c_str());
	}
	CCLabelBMFont* internalForce1=dynamic_cast<CCLabelBMFont* >(getVariable("mInternalForce1"));
	if(internalForce1)
	{
		internalForce1->setString(StringConverter::toString(disciple.rp()).c_str());
	}
	CCLabelBMFont* memberLV2=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV2"));
	if(memberLV2)
	{
		memberLV2->setString(StringConverter::toString(disciple.level()).c_str());
	}
	CCLabelBMFont* memberExp2=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberExp2"));
	if(memberExp2)
	{
		std::string exp("");
		exp +=StringConverter::toString(disciple.getLevelExp());
		exp +="/";
		exp += StringConverter::toString(disciple.getExpForLevelUpgrade(disciple.level()));
		memberExp2->setString(exp.c_str());
	}
	CCLabelBMFont* potential1=dynamic_cast<CCLabelBMFont* >(getVariable("mPotential1"));
	if(potential1)
	{
		potential1->setString(StringConverter::toString(disciple.potentiality()).c_str());
	}
}
