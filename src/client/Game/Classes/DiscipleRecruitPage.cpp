
#include "stdafx.h"

#include "DiscipleRecruitPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "DiscipleCardPage.h"
#include "SoundManager.h"
REGISTER_PAGE("DiscipleRecruitPage",DiscipleRecruitPage);
USING_NS_CC;

DiscipleRecruitPage::DiscipleRecruitPage(void)
	:mCard(0)
{

}

DiscipleRecruitPage::~DiscipleRecruitPage(void)
{
	if(mCard)
		delete mCard;
}

void DiscipleRecruitPage::load( void )
{
	loadCcbiFile("RecruitResult.ccbi");
	if(!getChildByTag(ANIMATION_CHILD))
	{
		CCBContainer* ani = CCBManager::Get()->createAndLoad("RecruitAni.ccbi");
		addChild(ani,100,ANIMATION_CHILD);
	}
	//refreshPage();
	
}

void DiscipleRecruitPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onFunction1")
	{

		MsgMainFrameCoverHide msg;
		msg.pageName = "DiscipleRecruitPage";
		MessageManager::Get()->sendMessage(&msg);

		BlackBoard::Get()->DisciplePageTab = (mIsSoul?2:1);
		MsgMainFrameChangePage msg1;
		msg1.pageName = "DisciplePage";
		MessageManager::Get()->sendMessage(&msg1);
	}
	else if(itemName=="onContinue")
	{
		MsgMainFrameCoverHide msg1;
		msg1.pageName = "DiscipleRecruitPage";
		MessageManager::Get()->sendMessage(&msg1);
	}
}

void DiscipleRecruitPage::refreshPage( void )
{
	Disciple disciple(mItemID,true);
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
	CCLabelBMFont* will=dynamic_cast<CCLabelBMFont* >(getVariable("mWill"));
	if(will)
	{
		will->setString(StringConverter::toString(disciple.rp()).c_str());
	}
	CCLabelBMFont* buffInfo=dynamic_cast<CCLabelBMFont* >(getVariable("mYuanfen"));
	if(buffInfo)
	{
		buffInfo->setString(disciple.getYuanfenDescribe().c_str());
	}
}

void DiscipleRecruitPage::Enter( MainFrame* )
{
	//setAdditionalSoul(true,NULL);

	mActionTime = 0;
	mStat = STAT_PREPARE;
	if(!mCard)
	{
		mCard = new DiscipleCardPage(mID);
		mCard->setItemID(mIsSoul);
		mCard->setIsSoul(mIsSoul);
		mCard->init();
		mCard->getContainerNode()->setVisible(false);
		//
	}
	else
	{
		mCard->getContainerNode()->setVisible(false);
		//mCard->setContentID(mID);
		mCard->setContentIDWithRemoveTextureCache(mID);
		mCard->setItemID(mIsSoul);
		mCard->setIsSoul(mIsSoul);
		mCard->refreshContent();
	}
	mCard->getContainerNode()->removeFromParent();
	CCB_FUNC(this,"mMemberCard",CCNode,addChild(mCard->getContainerNode()));
	CCAction* action = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(1.0f,ccp(0,20.0f)),CCMoveBy::create(1.0f,ccp(0,-20.0f))));
	CCB_FUNC(this,"mCustomsPassDetailNode",CCNode,runAction(action));
	mItemID = 0;
	if(mIsSoul)
	{
		mItemID = mID;
		CCB_FUNC(this,"mYuanfen",CCNode,setVisible(false));
		CCB_FUNC(this,"mSoulNote",CCNode,setVisible(true));
		CCB_FUNC(this,"mNum",CCLabelBMFont,setString(StringConverter::toString(mAdditionalSoulCount_x).c_str()));
		std::string button = Language::Get()->getString("@RecruitButtonToSoulList");
		CCB_FUNC(this,"mToList",CCLabelBMFont,setString(button.c_str()));
	}
	else
	{
		UserDiscipleInfo* info = ServerDateManager::Get()->getUserDiscipleInfoByID(mID);
		if(info)
			mItemID = info->itemid;
		CCB_FUNC(this,"mYuanfen",CCNode,setVisible(true));
		CCB_FUNC(this,"mSoulNote",CCNode,setVisible(false));
		std::string button = Language::Get()->getString("@RecruitButtonToDiscipleList");
		CCB_FUNC(this,"mToList",CCLabelBMFont,setString(button.c_str()));
		Disciple disciple(mID);
		for(int i=1;i<=6;++i)
		{
			char key[128];
			sprintf(key,"mYuanfen%d",i);
			const YuanfenItem * yuanfen =YuanfenTableManager::Get()->getYuanfenItemByID(disciple.yuanfens(i-1));
			if(yuanfen)
			{
				CCB_FUNC(this,key,CCLabelBMFont,setString(yuanfen->name.c_str()));
				CCB_FUNC(this,key,CCLabelBMFont,setVisible(true));
				CCB_FUNC(this,key,CCLabelBMFont,setColor(StringConverter::parseColor3B(VaribleManager::Get()->getSetting("Yuanfen_Nonactivated"))));
			}
			else
			{
				CCB_FUNC(this,key,CCLabelBMFont,setVisible(false));
			}
		}
	}
	const DiscipleItem* item = DiscipleTableManager::Get()->getDiscipleItemByID(mItemID);
	if(item)
	{
		//set something
	}
	
	if(mAdditionalSoulHas)
	{
		Disciple additiionalSoul(mAdditiionalSoulID,true);
		CCB_FUNC(this,"mQuality",CCSprite,initWithFile(additiionalSoul.getQualityImageFile().c_str()));
		CCB_FUNC(this,"mName",CCLabelBMFont,setString(additiionalSoul.name().c_str()));
		CCB_FUNC(this,"mHeadPic",CCSprite,initWithFile(additiionalSoul.iconPic().c_str()));
		
		std::string num ="";
		if(mAdditionalSoulCount_y == 0)	
			num = StringConverter::toString(mAdditionalSoulCount_x);
		else
			num = StringConverter::toString(mAdditionalSoulCount_x) + "+" + StringConverter::toString(mAdditionalSoulCount_y);
		CCB_FUNC(this,"mNum",CCLabelBMFont,setString(num.c_str()));
		CCB_FUNC(this,"mFrame",CCMenuItemImage,setNormalImage(additiionalSoul.getFrameNormalSpirte()));
		for(int i = 0; i<6; i++)
		{
			Disciple disciple(mSoulIDs[i],true);
			std::string var = "mHead" + StringConverter::toString(i+1);
			CCB_FUNC(this,var,CCSprite,setTexture(disciple.iconPic().c_str()));
			var = "mFrame" + StringConverter::toString(i+1);
			CCB_FUNC(this,var,CCMenuItemImage,setNormalImage(disciple.getFrameNormalSpirte()));
		}

	}
// 	if( mActionTime < 0 && mStat == STAT_PREPARE)
// 	{
// 		mActionTime = 2.0f;
// 		mStat = STAT_ANIMATION;
// 
// 	
// 
// 		CCBContainer* ani = dynamic_cast<CCBContainer*>(getChildByTag(ANIMATION_CHILD));
// 		if(ani)
// 		{
// 			ani->setVisible(true);
// 			ani->runAnimation("Default Timeline");
// 			SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("DiscipleRecruit"));
// 		}
// 	}
	refreshPage();
}

void DiscipleRecruitPage::Execute( MainFrame* )
{
	CCB_FUNC(mCard->getContainerNode(),"mShadowPic",CCSprite,setVisible(mIsSoul));
	float dt = GamePrecedure::Get()->getFrameTime();
	mActionTime-=dt;

	if( mActionTime < 0 && mStat == STAT_PREPARE)
	{
		mActionTime = 2.0f;
		mStat = STAT_ANIMATION;
		
		if(!getChildByTag(ANIMATION_CHILD))
		{
			CCBContainer* ani = CCBManager::Get()->createAndLoad("RecruitAni.ccbi");
			addChild(ani,100,ANIMATION_CHILD);
		}

		CCBContainer* ani = dynamic_cast<CCBContainer*>(getChildByTag(ANIMATION_CHILD));
		if(ani)
		{
			ani->setVisible(true);
			ani->runAnimation("Default Timeline");
			SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("DiscipleRecruit"));
		}
	}
	else if( mActionTime<0 && mStat == STAT_ANIMATION)
	{
		MsgInstructionNext insMsg;
		MessageManager::Get()->sendMessage(&insMsg);

		if(mIsSoul)
		{
			//set something visible

			CCB_FUNC(this,"mSoulNum",CCLabelBMFont,setString(StringConverter::toString(mSoulCount).c_str()));
		}
		CCBContainer* ani = dynamic_cast<CCBContainer*>(getChildByTag(ANIMATION_CHILD));
		if(ani)
		{
			ani->setVisible(false);
		}
		if(mAdditionalSoulHas)
		{
			int curSellect = 0;
			for (int i = 0;i<6;i++,curSellect++)
			{
				if(mAdditiionalSoulID == mSoulIDs[i])
					break;
			}

			runAnimation("SoulList" + StringConverter::toString(curSellect+1));
			mStat = STAT_SELECTING_SOUL;
		}
		else
		{
			runAnimation("RegularRec");
			mStat = STAT_GENERAL;
		}
		mCard->getContainerNode()->setVisible(true);
	}

}

void DiscipleRecruitPage::Exit( MainFrame* )
{
	if(mCard)
		delete mCard;
	mCard = NULL;
	mAdditionalSoulHas = false;
}

void DiscipleRecruitPage::setAdditionalSoul( bool has, unsigned int soulIDs[], int id /*= 0*/, int count_x /*= 0*/, int count_y  /*= 0*/)
{
// 	mAdditionalSoulHas = true;
// 	mAdditiionalSoulID = 65;
// 	mAdditionalSoulCount_x = 1;
// 	mAdditionalSoulCount_y = 0;
// 
// 	mSoulIDs[0] = 2;
// 	mSoulIDs[1] = 5;
// 	mSoulIDs[2] = 7;
// 	mSoulIDs[3] = 15;
// 	mSoulIDs[4] = 65;
// 	mSoulIDs[5] = 23;

	mAdditionalSoulHas = has;
	mAdditiionalSoulID = id;
	mAdditionalSoulCount_x = count_x;
	mAdditionalSoulCount_y = count_y;

	for(int i = 0; i<6; i++)
	{
		mSoulIDs[i] = soulIDs[i];
	}
}
