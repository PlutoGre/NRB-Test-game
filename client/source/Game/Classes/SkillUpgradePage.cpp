
#include "stdafx.h"

#include "SkillUpgradePage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Skill.h"
#include "DataTableManager.h"
#include "MessageBoxPage.h"
#include "Popup1stPayTipPage.h"
REGISTER_PAGE("SkillUpgradePage",SkillUpgradePage);
USING_NS_CC;

SkillUpgradePage::SkillUpgradePage(void)
{

}

SkillUpgradePage::~SkillUpgradePage(void)
{
}

void SkillUpgradePage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	refreshPage();
}

void SkillUpgradePage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(MSG_SEVERINFO_UPDATE,this);
}

void SkillUpgradePage::Execute( MainFrame* )
{

}

void SkillUpgradePage::load( void )
{
	loadCcbiFile("SkillUpgrade.ccbi");	
}
void SkillUpgradePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_PLAYER_LOGINRET_S)
		{
			if(BlackBoard::Get()->IsTimeoutResponse)
			{
				OPUserCareerInfo career;
				career.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_SKILL_BOOK_INFO_C,&career);
				OPUserBasicInfo info;
				info.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_USER_BASIC_INFO_C,&info);
			}
		}
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S)
		{
			refreshTitle();
		}
		else if(info->opcode==OPCODE_GET_USER_CAREER_INFORET_S)
		{
			if(BlackBoard::Get()->IsTimeoutResponse)
			{
				BlackBoard::Get()->IsTimeoutResponse = false;
				refreshContent();
			}

		}
	}
}

void SkillUpgradePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onUpgrade")
	{
		
		Skill skill(BlackBoard::Get()->UpgradeSkill);
		if(ServerDateManager::Get()->getUserBasicInfo().silvercoins<skill.getConsumeMoneyNeeded())
		{
			if(!ServerDateManager::Get()->_getMarketInfo().firstPay)
			{
				MSG_BOX(Language::Get()->getString("@SilverCoinsNotEnougth"));
			}
			else
			{
				POP_1STPAY_TIP_BOX(SilverNotEnough);
			}
			return;
		}
		BlackBoard::Get()->SkillUpgradeBeforeLV = skill.level();
		BlackBoard::Get()->SkillUpgradeBeforeQuantity = skill.quantityStr();

		OPUpgradeSkill upgradeSkill;
		upgradeSkill.set_id(skill.id());
		upgradeSkill.clear_kid();
		unsigned int num = BlackBoard::Get()->getSkillConsumeNum();
		for(unsigned int index = 0; index<num; ++index)
		{
			upgradeSkill.add_kid(BlackBoard::Get()->getSkillConsume(index));
		}
		PacketManager::Get()->sendPakcet(OPCODE_UPGRADE_SKILL_C,&upgradeSkill);
	}
	else if(itemName=="onQuit")
	{
		BlackBoard::Get()->UpgradeSkill = 0;
		BlackBoard::Get()->resetSkillConsume();
		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpSkillPageName;
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onChooseSkill")
	{
		MsgMainFrameChangePage msg;
		msg.pageName = "SkillUpChoosePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
void SkillUpgradePage::refreshTitle(void)
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

}
void SkillUpgradePage::refreshContent(void)
{
	unsigned int SkillUpgradeReceiver = BlackBoard::Get()->UpgradeSkill;
	Skill skill(SkillUpgradeReceiver);

	CCSprite* prop=dynamic_cast<CCSprite* >(getVariable("mSkillPic"));
	if(prop)
	{
		prop->initWithFile(skill.iconPic().c_str());
	}
	CCSprite* frame=dynamic_cast<CCSprite* >(getVariable("mQualityFrame"));
	if(frame)
	{
		frame->setTexture(skill.getFrameNormalSpirte()->getTexture());
		frame->setColor(skill.getFrameColor());
	}
	CCLabelBMFont* skillName = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillName"));
	if(skillName)
	{
		skillName->setString(skill.name().c_str());
	}
	CCLabelBMFont* skillLevel=dynamic_cast<CCLabelBMFont* >(getVariable("mSkillLevel"));
	if(skillLevel)
	{
		skillLevel->setString(StringConverter::toString(skill.level()).c_str());
	}
	CCSprite* skillQuality=dynamic_cast<CCSprite* >(getVariable("mSkillQuality"));
	if(skillQuality)
	{
		skillQuality->initWithFile(skill.getQualityImageFile().c_str());
	}
	CCLabelBMFont* attackValue = dynamic_cast<CCLabelBMFont* >(getVariable("mSkillBuffValue"));
	if(attackValue)
	{
		attackValue->setString(skill.quantityStr().c_str());
	}
	CCSprite* skillBuffType=dynamic_cast<CCSprite* >(getVariable("mSkillBuffType"));
	if(skillBuffType)
	{
		skillBuffType->initWithFile(skill.getSkillAtkTypePic().c_str());
	}

	CCLabelBMFont* rateLabel = dynamic_cast<CCLabelBMFont* >(getVariable("mRateLabel"));
	if(rateLabel)
	{
		if(BlackBoard::Get()->hasSkillConsume())
			rateLabel->setVisible(true);
		else
			rateLabel->setVisible(false);
	}
	CCLabelBMFont* rateValue = dynamic_cast<CCLabelBMFont* >(getVariable("mRateValue"));
	if(rateValue)
	{
		if(BlackBoard::Get()->hasSkillConsume())
		{
			rateValue->setVisible(true);
			rateValue->setString(skill.getCurUpgradeRatioString().c_str());
		}
		else
			rateValue->setVisible(false);
	}
	CCLabelBMFont* moneyNeededLabel = dynamic_cast<CCLabelBMFont* >(getVariable("mMoneyNeededLabel"));
	if(moneyNeededLabel)
	{
		if(BlackBoard::Get()->hasSkillConsume())
		{
			moneyNeededLabel->setVisible(true);
		}
		else
			moneyNeededLabel->setVisible(false);
	}
	CCLabelBMFont* moneyNeeded = dynamic_cast<CCLabelBMFont* >(getVariable("mMoneyNeeded"));
	if(moneyNeeded)
	{
		if(BlackBoard::Get()->hasSkillConsume())
		{
			moneyNeeded->setVisible(true);
			moneyNeeded->setString(StringConverter::toString(skill.getConsumeMoneyNeeded()).c_str());
		}
		else
			moneyNeeded->setVisible(false);
	}
	CCSprite* coinPic=dynamic_cast<CCSprite* >(getVariable("mCoinPic"));
	if(coinPic)
	{
		if(BlackBoard::Get()->hasSkillConsume())
		{
			coinPic->setVisible(true);
		}
		else
			coinPic->setVisible(false);
	}
	CCLabelBMFont* selectCard = dynamic_cast<CCLabelBMFont* >(getVariable("mSelectCard"));
	if(selectCard)
	{
		if(BlackBoard::Get()->hasSkillConsume())
		{
			selectCard->setVisible(false);
		}
		else
		{
			selectCard->setVisible(true);
			selectCard->setString(Language::Get()->getString("@SelectConsumeCard").c_str());
		}
	}

	for(int i=0; i<5; i++)
	{
		unsigned int id = BlackBoard::Get()->getSkillConsume(i);
		if(id !=0 )
		{
			Skill skillConsume(BlackBoard::Get()->getSkillConsume(i));
			CCSprite* skillPic = dynamic_cast<CCSprite* >(getVariable("mSkillPic" + StringConverter::toString(i+1)));
			if(skillPic)
			{
				skillPic->initWithFile(skillConsume.iconPic().c_str());
			}
			CCMenuItemImage* skillFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mFrame" + StringConverter::toString(i+1)));
			if(skillFrame)
			{
				skillFrame->setNormalImage(skillConsume.getFrameNormalSpirte());
				skillFrame->setSelectedImage(skillConsume.getFrameSelectedSpirte());
			}
		}
		else
		{			
			CCSprite* skillPic = dynamic_cast<CCSprite* >(getVariable("mSkillPic" + StringConverter::toString(i+1)));
			if(skillPic)
			{
				skillPic->initWithFile(VaribleManager::Get()->getSetting("DefaultIcon_ImageFile").c_str());
			}
			CCMenuItemImage* skillFrame=dynamic_cast<CCMenuItemImage* >(getVariable("mFrame" + StringConverter::toString(i+1)));
			if(skillFrame)
			{
				skillFrame->setNormalImage(CCSprite::create(::getQualityFrameNormalImageFile().c_str()));
				skillFrame->setSelectedImage(CCSprite::create(::getQualityFrameSelectImageFile().c_str()));
			}
		}
	}

	CCMenuItemImage* button = dynamic_cast<CCMenuItemImage* >(getVariable("mUpgradeButton"));
	if(button)
	{
		if(BlackBoard::Get()->getSkillConsumeNum() != 0)
			button->setEnabled(true);
		else
			button->setEnabled(false);
	}

}
void SkillUpgradePage::refreshPage( void )
{
	refreshTitle();
	refreshContent();
}
