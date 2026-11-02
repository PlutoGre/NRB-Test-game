
#include "stdafx.h"

#include "TrainPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"

#include "GamePackets.h"

REGISTER_PAGE("TrainPage",TrainPage);
USING_NS_CC;

TrainPage::TrainPage(void)
{
	mCurSelectNode = NULL;
	mTrainType = T_1_Train;
}

TrainPage::~TrainPage(void)
{
}

void TrainPage::Enter( MainFrame* frame)
{
	selectType(mTrainType);
	refreshPage();
}

void TrainPage::Exit( MainFrame* frame)
{

	mCurSelectNode = NULL;//加一句，屏蔽一处崩溃 by zhengxin add by dylan at 20130604
}

void TrainPage::Execute( MainFrame* )
{

}

void TrainPage::load( void )
{
	loadCcbiFile("Train.ccbi");
}

void TrainPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onQuit")
	{
		BlackBoard::Get()->TrainDisciple = 0;
		MsgMainFrameChangePage msg;
		msg.pageName = BlackBoard::Get()->jumpDisciplePageName;
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onTrain")
	{
		BlackBoard::Get()->TrainType = mTrainType;

		OPTrainDisciple trainDisciple;
		trainDisciple.set_id(BlackBoard::Get()->TrainDisciple);
		trainDisciple.set_type(mTrainType);
		PacketManager::Get()->sendPakcet(OPCODE_TRAIN_DISCIPLE_C,&trainDisciple);
	}
	else if(itemName=="onSelect1")
	{
		selectType(T_1_Train);
	}
	else if(itemName=="onSelect2")
	{
		selectType(T_1_TrainCarefully);
	}
	else if(itemName=="onSelect3")
	{
		selectType(T_10_Train);
	}
	else if(itemName=="onSelect4")
	{
		selectType(T_10_TrainCarefully);
	}
}

void TrainPage::refreshPage( void )
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
			trainningBolus->setString(StringConverter::toString(info->count).c_str());
		}
		else
		{
			trainningBolus->setString("0");
		}
	}
}

void TrainPage::selectType( TrainType type )
{
	if(mCurSelectNode)
		mCurSelectNode->unselected();

	unsigned int useTrainningBolus = 5;
	unsigned int useTrainningCoin = 0;
	if(type == T_1_Train)
	{
		mCurSelectNode = dynamic_cast<CCMenuItemImage* >(getVariable("mSelect1"));
		mTrainType = T_1_Train;
	}
	else if(type == T_1_TrainCarefully)
	{
		mCurSelectNode = dynamic_cast<CCMenuItemImage* >(getVariable("mSelect2"));
		mTrainType = T_1_TrainCarefully;
		useTrainningCoin = 10;
	}
	else if(type == T_10_Train)
	{
		mCurSelectNode = dynamic_cast<CCMenuItemImage* >(getVariable("mSelect3"));
		mTrainType = T_10_Train;
		useTrainningBolus *= 10;
	}
	else if(type == T_10_TrainCarefully)
	{
		mCurSelectNode = dynamic_cast<CCMenuItemImage* >(getVariable("mSelect4"));
		mTrainType = T_10_TrainCarefully;
		useTrainningBolus *= 10;
		useTrainningCoin = 88;
	}

	if(mCurSelectNode)
		mCurSelectNode->selected();

	
	CCMenuItemImage* train = dynamic_cast<CCMenuItemImage* >(getVariable("mTrain"));
	if(train)
	{
		const UserToolInfo * info = ServerDateManager::Get()->getUserToolInfoByItemId(1502001);
		const UserBasicInfo& baseInfo = ServerDateManager::Get()->getUserBasicInfo();
		if(info && info->count >= useTrainningBolus 
			&& baseInfo.goldcoins >= useTrainningCoin)
		{
			train->setEnabled(true);
		}
		else
			train->setEnabled(false);
	}
	
}

void TrainPage::onReceiveMassage( const GameMessage * message )
{

}