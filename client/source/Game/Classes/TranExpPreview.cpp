
#include "stdafx.h"

#include "TranExpPreview.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("TranExpPreviewPage",TranExpPreviewPage);
USING_NS_CC;

TranExpPreviewPage::TranExpPreviewPage(void)
{
	mTransExpPercent = 0;
}

TranExpPreviewPage::~TranExpPreviewPage(void)
{
}

void TranExpPreviewPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	refreshPage();
}

void TranExpPreviewPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	BlackBoard::Get()->TrainingBolusID = 0;
}

void TranExpPreviewPage::Execute( MainFrame* )
{

}

void TranExpPreviewPage::load( void )
{
	loadCcbiFile("TranExpPreview.ccbi");
}

void TranExpPreviewPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onSellectMembers")
	{
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
		if(BlackBoard::Get()->TrainingBolusID > 0 )
		{
			const UserToolInfo * tool = ServerDateManager::Get()->getUserToolInfoByID(BlackBoard::Get()->TrainingBolusID);
			if(tool->count>0)
			{
				OPSwallowDisciple swallowDisciple;
				swallowDisciple.set_id(BlackBoard::Get()->TransferExp_Receiver);
				swallowDisciple.set_swallowedid(BlackBoard::Get()->TransferExp_Provider);
				swallowDisciple.set_pelletid(tool->itemid);
				PacketManager::Get()->sendPakcet(OPCODE_SWALLOW_DISCIPLE_C,&swallowDisciple);

				BlackBoard::Get()->TransferExp_Provider = 0;
				Disciple receiverDisciple(BlackBoard::Get()->TransferExp_Receiver);
				BlackBoard::Get()->TransferBeforeBlood = receiverDisciple.health();
				BlackBoard::Get()->TransferBeforeAttack = receiverDisciple.attack();
				BlackBoard::Get()->TransferBeforeDefense = receiverDisciple.defence();
				BlackBoard::Get()->TransferBeforeRP = receiverDisciple.rp();
				BlackBoard::Get()->TransferBeforeLV = receiverDisciple.level();
				BlackBoard::Get()->TransferBeforeExp = receiverDisciple.getLevelExp();
				BlackBoard::Get()->TransferBeforePotential = receiverDisciple.potentiality();
			}
			else
			{
				MsgMainFramePushPage msg;
				msg.pageName = "TrainingBolusChoosePage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
		else
		{
			MsgMainFramePushPage msg;
			msg.pageName = "TrainingBolusChoosePage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onTrainingBolus")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "TrainingBolusChoosePage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void TranExpPreviewPage::refreshPage( void )
{
	_refeshTitle();
	_refeshProvider();
	_refeshReceiver();
	_refeshTransExp();
}

void TranExpPreviewPage::_refeshTitle( void )
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

void TranExpPreviewPage::_refeshProvider( void )
{
	Disciple providerDisciple(BlackBoard::Get()->TransferExp_Provider);
	CCSprite* headPic1=dynamic_cast<CCSprite* >(getVariable("mHeadPic1"));
	if(headPic1)
	{
		headPic1->initWithFile(providerDisciple.iconPic().c_str());
	}
	CCMenuItemImage* face1=dynamic_cast<CCMenuItemImage* >(getVariable("mFace1"));
	if(face1)
	{
		face1->setNormalImage(providerDisciple.getFrameNormalSpirte());
		face1->setSelectedImage(providerDisciple.getFrameSelectedSpirte());
	}

	CCLabelBMFont* memberName1=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName1"));
	if(memberName1)
	{
		memberName1->setString(Language::Get()->getString(providerDisciple.name()).c_str());
	}
	CCSprite* memberQuality1=dynamic_cast<CCSprite* >(getVariable("mMemberQuality1"));
	if(memberQuality1)
	{
		memberQuality1->initWithFile(providerDisciple.getQualityImageFile().c_str());
	}
	CCLabelBMFont* memberLV1=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV1"));
	if(memberLV1)
	{
		memberLV1->setString(StringConverter::toString(providerDisciple.level()).c_str());
	}
	
}

void TranExpPreviewPage::_refeshReceiver( void )
{
	Disciple receiverDisciple(BlackBoard::Get()->TransferExp_Receiver);
	CCSprite* headPic=dynamic_cast<CCSprite* >(getVariable("mHeadPic"));
	if(headPic)
	{
		headPic->initWithFile(receiverDisciple.iconPic().c_str());
	}
	CCMenuItemImage* face=dynamic_cast<CCMenuItemImage* >(getVariable("mFace"));
	if(face)
	{
		face->setNormalImage(receiverDisciple.getFrameNormalSpirte());
		face->setSelectedImage(receiverDisciple.getFrameSelectedSpirte());
	}
	CCLabelBMFont* memberName=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName"));
	if(memberName)
	{
		memberName->setString(Language::Get()->getString(receiverDisciple.name()).c_str());
	}
	CCSprite* memberQuality=dynamic_cast<CCSprite* >(getVariable("mMemberQuality"));
	if(memberQuality)
	{
		memberQuality->initWithFile(receiverDisciple.getQualityImageFile().c_str());
	}
	CCLabelBMFont* memberLV=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV"));
	if(memberLV)
	{
		memberLV->setString(StringConverter::toString(receiverDisciple.level()).c_str());
	}
}

void TranExpPreviewPage::_refeshTransExp( void )
{
	const UserToolInfo * tool = NULL;
	const ToolItem * toolItem = NULL;
	mTransExpPercent = 0;
	unsigned int toolId = 0;
	if(BlackBoard::Get()->TrainingBolusID == 0 )
	{
		tool = ServerDateManager::Get()->getUserToolInfoByItemId(1503002);
		if(!tool)
		{
			tool = ServerDateManager::Get()->getUserToolInfoByItemId(1503001);
			if(!tool)
			{
				toolId = 1503002;
			}else
			{
				toolId = 1503001;
			}
		}else
		{
			toolId = 1503002;
		}
	}
	else
	{
		tool = ServerDateManager::Get()->getUserToolInfoByItemId(BlackBoard::Get()->TrainingBolusID);
		toolId = tool->itemid;
	}
	toolItem = ToolTableManager::Get()->getToolItemByID(toolId);
	if(toolItem)
	{
		BlackBoard::Get()->TrainingBolusID = 0;
		if(tool)
		{
			BlackBoard::Get()->TrainingBolusID = tool->id;
		}
		if(toolId == 1503001)
		{
			mTransExpPercent = 0.8f;
		}
		else if(toolId == 1503002)
		{
			mTransExpPercent = 1.0f;
		}

		CCSprite* prop=dynamic_cast<CCSprite* >(getVariable("mTrainingBolusPic"));
		if(prop && toolItem)
		{
			prop->initWithFile(toolItem->iconPic.c_str());
		}
		CCLabelBMFont* goodsName=dynamic_cast<CCLabelBMFont* >(getVariable("mGoodsName"));
		if(goodsName)
		{
			goodsName->setString(Language::Get()->getString(toolItem->name).c_str());
		}
	}

	Disciple receiverDisciple(BlackBoard::Get()->TransferExp_Receiver);
	Disciple providerDisciple(BlackBoard::Get()->TransferExp_Provider);
	
	unsigned int providerExp = providerDisciple.getProviderExp();
	providerExp *= mTransExpPercent;

	CCLabelBMFont* memberName2=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName2"));
	if(memberName2)
	{
		memberName2->setString(providerDisciple.name().c_str());
	}
	CCLabelBMFont* memberTrainEXP=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberTrainEXP"));
	if(memberTrainEXP)
	{
		memberTrainEXP->setString(StringConverter::toString(providerExp).c_str());
	}

	CCLabelBMFont* memberName3=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberName3"));
	if(memberName3)
	{
		memberName3->setString(receiverDisciple.name().c_str());
	}
	CCLabelBMFont* memberLV2=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV2"));
	if(memberLV2)
	{
		memberLV2->setString(StringConverter::toString(receiverDisciple.level()).c_str());
	}
	CCLabelBMFont* memberLV3=dynamic_cast<CCLabelBMFont* >(getVariable("mMemberLV3"));
	if(memberLV3)
	{
		memberLV3->setString(StringConverter::toString(receiverDisciple.getUpgradeLevel(providerExp)).c_str());
	}
	CCMenuItemImage* mTransferEXP = dynamic_cast<CCMenuItemImage* >(getVariable("mTransferEXP"));
	if(mTransferEXP)
	{
		if(!receiverDisciple.canUpgradeLevel())
		{
			mTransferEXP->setEnabled(false);
		}else{
		}
	}
}

void TranExpPreviewPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "TranExpPreviewPage")
		{
			refreshPage();
		}
	}
}