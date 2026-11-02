
#include "stdafx.h"

#include "TrainResultPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"

#include "GamePackets.h"

REGISTER_PAGE("TrainResultPage",TrainResultPage);
USING_NS_CC;

TrainResultPage::TrainResultPage(void)
{
}

TrainResultPage::~TrainResultPage(void)
{
}

void TrainResultPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void TrainResultPage::Exit( MainFrame* frame)
{

}

void TrainResultPage::Execute( MainFrame* )
{

}

void TrainResultPage::load( void )
{
	loadCcbiFile("TrainResult.ccbi");
}

void TrainResultPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onGiveup")
	{
		OPDealTrainStatus dealTrainStatus;
		dealTrainStatus.set_id(BlackBoard::Get()->TrainDisciple);
		dealTrainStatus.set_type(2);
		PacketManager::Get()->sendPakcet(OPCODE_DEAL_TRAIN_STATUS_C,&dealTrainStatus);
	}
	else if(itemName=="onAccept")
	{
		OPDealTrainStatus dealTrainStatus;
		dealTrainStatus.set_id(BlackBoard::Get()->TrainDisciple);
		dealTrainStatus.set_type(1);
		PacketManager::Get()->sendPakcet(OPCODE_DEAL_TRAIN_STATUS_C,&dealTrainStatus);
	}
}

void TrainResultPage::refreshPage( void )
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

	Disciple disciple( BlackBoard::Get()->TrainDisciple);
	CCSprite* headPic=dynamic_cast<CCSprite* >(getVariable("mHeadPic"));
	if(headPic)
	{
		headPic->initWithFile(disciple.iconPic().c_str());
	}
	CCMenuItemImage* face=dynamic_cast<CCMenuItemImage* >(getVariable("mFace"));
	if(face)
	{
		face->setNormalImage(disciple.getFrameNormalSpirte());
		face->setSelectedImage(disciple.getFrameSelectedSpirte());
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

	CCLabelBMFont* blood=dynamic_cast<CCLabelBMFont* >(getVariable("mBlood"));
	if(blood)
	{
		blood->setString(StringConverter::toString(disciple.health()).c_str());
	}
	CCLabelBMFont* attack=dynamic_cast<CCLabelBMFont* >(getVariable("mAttack"));
	if(attack)
	{
		attack->setString(StringConverter::toString(disciple.attack()).c_str());
	}
	CCLabelBMFont* defense=dynamic_cast<CCLabelBMFont* >(getVariable("mDefense"));
	if(defense)
	{
		defense->setString(StringConverter::toString(disciple.defence()).c_str());
	}
	CCLabelBMFont* internalForce=dynamic_cast<CCLabelBMFont* >(getVariable("mInternalForce"));
	if(internalForce)
	{
		internalForce->setString(StringConverter::toString(disciple.rp()).c_str());
	}

	const TrainDiscipleInfo& train = ServerDateManager::Get()->getTrainDiscipleInfo();

	CCLabelBMFont* newBlood = dynamic_cast<CCLabelBMFont* >(getVariable("mNewBlood"));
	if(newBlood)
	{
		if(train.minus.pro == 3)
		{
			newBlood->setString(StringConverter::toString(disciple.health() - train.minus.num).c_str());
			CCLabelBMFont* bloodChange=dynamic_cast<CCLabelBMFont* >(getVariable("mBloodChange"));
			bloodChange->setString(("-" + StringConverter::toString(train.minus.num)).c_str());
			bloodChange->setVisible(true);
		}
		else if(train.add.pro == 3)
		{
			newBlood->setString(StringConverter::toString(disciple.health ()+ train.add.num).c_str());
			CCLabelBMFont* bloodChange=dynamic_cast<CCLabelBMFont* >(getVariable("mBloodChange"));
			bloodChange->setString(("+" + StringConverter::toString(train.add.num)).c_str());
			bloodChange->setVisible(true);
		}
		else
		{
			newBlood->setString(StringConverter::toString(disciple.health()).c_str());
			CCLabelBMFont* bloodChange=dynamic_cast<CCLabelBMFont* >(getVariable("mBloodChange"));
			bloodChange->setVisible(false);
		}
	}
	
	CCLabelBMFont* newAttack = dynamic_cast<CCLabelBMFont* >(getVariable("mNewAttack"));
	if(newAttack)
	{
		if(train.minus.pro == 1)
		{
			newAttack->setString(StringConverter::toString(disciple.attack() - train.minus.num).c_str());
			CCLabelBMFont* attackChange=dynamic_cast<CCLabelBMFont* >(getVariable("mAtkChange"));
			attackChange->setString(("-" + StringConverter::toString(train.minus.num)).c_str());
			attackChange->setVisible(true);
		}
		else if(train.add.pro == 1)
		{
			newAttack->setString(StringConverter::toString(disciple.attack() + train.add.num).c_str());
			CCLabelBMFont* attackChange=dynamic_cast<CCLabelBMFont* >(getVariable("mAtkChange"));
			attackChange->setString(("+" + StringConverter::toString(train.add.num)).c_str());
			attackChange->setVisible(true);
		}
		else
		{
			newAttack->setString(StringConverter::toString(disciple.attack()).c_str());
			CCLabelBMFont* attackChange=dynamic_cast<CCLabelBMFont* >(getVariable("mAtkChange"));
			attackChange->setVisible(false);
		}
	}

	CCLabelBMFont* newDefense = dynamic_cast<CCLabelBMFont* >(getVariable("mNewDefense"));
	if(newDefense)
	{
		if(train.minus.pro == 2)
		{
			newDefense->setString(StringConverter::toString(disciple.defence() - train.minus.num).c_str());
			CCLabelBMFont* defenseChange=dynamic_cast<CCLabelBMFont* >(getVariable("mDefChange"));
			defenseChange->setString(("-" + StringConverter::toString(train.minus.num)).c_str());
			defenseChange->setVisible(true);
		}
		else if(train.add.pro == 2)
		{
			newDefense->setString(StringConverter::toString(disciple.defence() + train.add.num).c_str());
			CCLabelBMFont* defenseChange=dynamic_cast<CCLabelBMFont* >(getVariable("mDefChange"));
			defenseChange->setString(("+" + StringConverter::toString(train.add.num)).c_str());
			defenseChange->setVisible(true);
		}
		else
		{
			newDefense->setString(StringConverter::toString(disciple.defence()).c_str());
			CCLabelBMFont* defenseChange=dynamic_cast<CCLabelBMFont* >(getVariable("mDefChange"));
			defenseChange->setVisible(false);
		}
	}

	CCLabelBMFont* newInterndForce = dynamic_cast<CCLabelBMFont* >(getVariable("mNewInternalForce"));
	if(newInterndForce)
	{
		if(train.minus.pro == 4)
		{
			newInterndForce->setString(StringConverter::toString(disciple.rp() - train.minus.num).c_str());
			CCLabelBMFont* IFChange=dynamic_cast<CCLabelBMFont* >(getVariable("mIFChange"));
			IFChange->setString(("-" + StringConverter::toString(train.minus.num)).c_str());
			IFChange->setVisible(true);
		}
		else if(train.add.pro == 4)
		{
			newInterndForce->setString(StringConverter::toString(disciple.rp() + train.add.num).c_str());
			CCLabelBMFont* IFChange=dynamic_cast<CCLabelBMFont* >(getVariable("mIFChange"));
			IFChange->setString(("+" + StringConverter::toString(train.add.num)).c_str());
			IFChange->setVisible(true);
		}
		else
		{
			newInterndForce->setString(StringConverter::toString(disciple.rp()).c_str());
			CCLabelBMFont* IFChange=dynamic_cast<CCLabelBMFont* >(getVariable("mIFChange"));
			IFChange->setVisible(false);
		}
	}

	CCLabelBMFont* potential=dynamic_cast<CCLabelBMFont* >(getVariable("mPotential"));
	if(potential)
	{
		potential->setString(StringConverter::toString(disciple.potentiality()).c_str());
	}
	CCLabelBMFont* trainningBolus=dynamic_cast<CCLabelBMFont* >(getVariable("mTrainningBolus"));
	if(trainningBolus)
	{
		const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(1502001);
		if(info)
		{
			unsigned int trainType = BlackBoard::Get()->TrainType;
			unsigned int useTrainningBolus = 0;
			if(trainType == 1 || trainType == 2)
			{
				useTrainningBolus = 5;
			}
			if(trainType == 3 || trainType == 4)
			{
				useTrainningBolus = 50;
			}
			BlackBoard::Get()->ConsumeTrainningBolus = useTrainningBolus;
			trainningBolus->setString(StringConverter::toString(info->count - useTrainningBolus).c_str());
		}
	}
}

void TrainResultPage::onReceiveMassage( const GameMessage * message )
{

}