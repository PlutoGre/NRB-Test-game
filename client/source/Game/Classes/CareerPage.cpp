
#include "stdafx.h"

#include "CareerPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "lib91.h"
#include "FightPage.h"
#include "MessageBoxPage.h"
#include "waitingManager.h"
#include "BlackBoard.h"
#include "PopupPage.h"
#include "CareerStoryPage.h"
#include "PopupNotEnoughPage.h"
#include "CCBContainer.h"
#include "InstructionManager.h"
#include "GetPropManager.h"
#include "TimeCalculator.h"
#include "particle_nodes/CCParticleSystemQuad.h"
#include "CareerQuickResultPage.h"
REGISTER_PAGE("CareerPage",CareerPage);
USING_NS_CC;

CareerPage::CareerPage(void)
{
	mIsFirstIn=true;
	mExpbarInitScale = -1.0f;
	mCurrChapterIndex=1;
	mLastClickCustomPassId=-1;
	mIsQuickCarrerInCool = false;
}


CareerPage::~CareerPage(void)
{
	mIsFirstIn=false;
	mCurrChapterIndex=1;
	mLastClickCustomPassId=-1;
}

void CareerPage::loadAttackInfo()
{
	if(mCurrChapterIndex==1&&mLastClickCustomPassId==-1)
	{
		mCurrChapterIndex=mCareerPageAttackInfo.currChapterId;
		mLastClickCustomPassId=mCareerPageAttackInfo.currCustomsPassId;
	}
	if(mCurrChapterIndex<=0)
	{
		mCurrChapterIndex=1;
	}
	if (mCareerPageAttackInfo.backwardsTime >=0)
	{
		TimeCalculator::Get()->createTimeCalcultor("coldtime",mCareerPageAttackInfo.backwardsTime);
	}

	mIsAttack=false;
}

void CareerPage::Enter( MainFrame* frame)
{
	mShouldPlayCoversationID = -1;

	MessageManager::Get()->regisiterMessageHandler(MSG_CAREER_CHANGEPAGE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_USER_BATTLERET_S,this);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_RESET_CAREER_COUNTRET_S,this);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_QUICK_CAREER_BATTLERET_S, this);// add by xinghui 2013/6/30

	//PacketManager::Get()->regisiterPacketHandler(OPCODE_QUICK_CAREER_BATTLERET_S, this);


	loadAttackInfo();

//     static bool initialized = false;
//     if(!initialized)
//     {
//         initialized = true;
	mNavContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mChapterNav"));
	mNavContent->setScrollDeaccelRate(0.8f);//滑动松手后的减速加速度
	mCustomsPassScrollView =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	mCustomsPassScrollView->setScrollDeaccelRate(0.85f);//滑动松手后的减速加速度
	mCustomsPassScrollViewViewHeight=mCustomsPassScrollView->getViewSize().height;
	if(mNavContent)
	{
		cocos2d::CCNode *node = cocos2d::CCNode::create();
		CCSize size = CCSize(0,0);
		std::list< CareerData::ChaptersData* >::const_iterator it =CareerConfigManager::Get()->getChaptersDataList().begin();
		mChapterContainerList.clear();
		int _index=0;
		bool _isSetIndex=false;
		for(;it!=CareerConfigManager::Get()->getChaptersDataList().end();++it)
		{
			CareerData::ChaptersData* chapters = *it;
			CCBContainer* chapterNavButton = CCBManager::Get()->createAndLoad("CareerChapterButton.ccbi");
			chapterNavButton->setListener(this,FACE_BUTTON_START+chapters->id);
			cocos2d::CCObject* obj = chapterNavButton->getVariable("mFrameBack");
			if(!obj)
			{
				CCMessageBox("Failed to find variable:mFrameBack","error");
			}
			CCMenuItemSprite* frame = dynamic_cast<CCMenuItemSprite*>(obj);
			if(frame && frame->getParent())
			{
				CCMenu* menu = dynamic_cast<CCMenu*>(frame->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
			}

			if(chapters->id>mCareerPageAttackInfo.currChapterId)
			{
				frame->setEnabled(false);
			}
			else
			{
				frame->setEnabled(true);
			}

			CCLabelBMFont* itemName=dynamic_cast<CCLabelBMFont*>(chapterNavButton->getVariable(("mName")));
			if(itemName)
			{
				itemName->setString(Language::Get()->getString(chapters->name).c_str());
			}

			node->addChild(chapterNavButton);
			chapterNavButton->setPosition(ccp(size.width,0));
			mNavWidth= chapterNavButton->getContentSize().width;
			size.width += chapterNavButton->getContentSize().width;
			size.height = chapterNavButton->getContentSize().height;
			chapterNavButton->retain();
			mChapterContainerList.push_back(std::make_pair(chapters->id,chapterNavButton));
			if(chapters->id==mCurrChapterIndex)
			{
				_isSetIndex=true;
				ShowCareerCustomsPass(chapters->id);
			}
			if(!_isSetIndex)
			{
				++_index;
			}
		}
		node->setContentSize(size);
		mNavContent->setContainer(node);
		mNavContent->setContentSize(size);
		resetNavOffset(_index,true);
	}
/*	}*/
	_refreshTitle();
}

void CareerPage::resetNavOffset(const unsigned int index,bool isEnter)
{
	float currNavOffsetX=mNavContent->getContentOffset().x;
	float maxOffsetX=0-index*mNavWidth;
	float minOffsetX=0-(index+1)*mNavWidth;
	if((currNavOffsetX-minOffsetX)>mNavContent->getViewSize().width)
	{
		if(isEnter)
		{
			mNavContent->setContentOffset(ccp(maxOffsetX+mNavWidth,mNavContent->getContentOffset().y));
		}
		else
		{
			mNavContent->setContentOffsetInDuration(ccp(maxOffsetX+mNavWidth,mNavContent->getContentOffset().y),0.2f);
		}
	}
	else
	{
		if(currNavOffsetX-maxOffsetX<mNavWidth)
		{
			float target=maxOffsetX+mNavContent->getViewSize().width-mNavWidth;
			if(target>0.0)
			{
				target=0.0;
			}
			if(isEnter)
			{
				mNavContent->setContentOffset(ccp(target,mNavContent->getContentOffset().y));
			}
			else
			{
				mNavContent->setContentOffsetInDuration(ccp(target,mNavContent->getContentOffset().y),0.2f);
			}
		}
	}
}

void CareerPage::ShowCareerCustomsPass(int chapterId,int customsPassId)
{
	_clearPassContainer();
	CCSize showOffset=CCSize(0,0);
	CCSize reviseOffset=CCSize(0,0);
	if(mCustomsPassScrollView)
	{
		cocos2d::CCNode* node = cocos2d::CCNode::create();
		mCustomsPassContainerList.clear();
		CCSize size = CCSize(0,0);
		CareerData::ChaptersData* chapterData =CareerConfigManager::Get()->getChaptersDataByChaptersId(chapterId);
		std::list< CareerData::CustomsPassData* >::reverse_iterator it=chapterData->mCustomsPassList.rbegin();
		mCareerPageAttackInfo.currCustomsPassMaxId=(*it)->id;
		//if pass add this
		CCBContainer* chapterPlotNext = CCBManager::Get()->createAndLoad("CareerLongButton.ccbi");
		chapterPlotNext->setListener(this,CHAPTER_PLOT_NEXT);
		if(chapterPlotNext)
		{
			CCLabelBMFont* name=dynamic_cast<CCLabelBMFont*>(chapterPlotNext->getVariable(("mName")));
			if(name)
			{
				name->setString(Language::Get()->getString("@chapterPlotNext").c_str());
			}

			CCMenuItemSprite* nextButton = dynamic_cast<CCMenuItemSprite*>(chapterPlotNext->getVariable("mPassChapterButton"));
			if(nextButton && nextButton->getParent())
			{
				CCMenu* menu = dynamic_cast<CCMenu*>(nextButton->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
			}
			node->addChild(chapterPlotNext);
			if(mCareerPageAttackInfo.currCustomsPassMaxId<mCareerPageAttackInfo.currCustomsPassId)
			{
				chapterPlotNext->setVisible(true);
				chapterPlotNext->setPosition(ccp(0,size.height));
				size.height += (chapterPlotNext->getContentSize().height);
			}
			else
			{
				chapterPlotNext->setVisible(false);
			}
			chapterPlotNext->retain();
			mCustomsPassContainerList.push_back(std::make_pair(CHAPTER_PLOT_NEXT,chapterPlotNext));
		}

		CCBContainer* chapterPlotReload = CCBManager::Get()->createAndLoad("CareerLongButton.ccbi");
		chapterPlotReload->setListener(this,CHAPTER_PLOT_RELOAD);
		if(chapterPlotReload)
		{
			CCLabelBMFont* name=dynamic_cast<CCLabelBMFont*>(chapterPlotReload->getVariable(("mName")));
			if(name)
			{
				name->setString(Language::Get()->getString("@chapterPlotReload").c_str());
			}

			CCMenuItemSprite* reloadButton = dynamic_cast<CCMenuItemSprite*>(chapterPlotReload->getVariable("mPassChapterButton"));
			if(reloadButton && reloadButton->getParent())
			{
				CCMenu* menu = dynamic_cast<CCMenu*>(reloadButton->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
			}
			node->addChild(chapterPlotReload);
			if(mCareerPageAttackInfo.currCustomsPassMaxId<mCareerPageAttackInfo.currCustomsPassId)
			{
				chapterPlotReload->setVisible(true);
				chapterPlotReload->setPosition(ccp(0,size.height));
				size.height += (chapterPlotReload->getContentSize().height);
			}
			else
			{
				chapterPlotReload->setVisible(false);
			}
			chapterPlotReload->retain();
			mCustomsPassContainerList.push_back(std::make_pair(CHAPTER_PLOT_RELOAD,chapterPlotReload));
		}

		CCBContainer* chapterAddPower = CCBManager::Get()->createAndLoad("CareerAddPowerButton.ccbi");
		chapterAddPower->setListener(this,CHAPTER_ADD_POWER);
		if(chapterAddPower)
		{
			node->addChild(chapterAddPower);
			CCMenuItemSprite* addPowerBtn = dynamic_cast<CCMenuItemSprite*>(chapterAddPower->getVariable("mAddPowerButton"));
			if(addPowerBtn && addPowerBtn->getParent())
			{
				CCMenu* menu = dynamic_cast<CCMenu*>(addPowerBtn->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
			}
			if(mCareerPageAttackInfo.currCustomsPassMaxId<mCareerPageAttackInfo.currCustomsPassId)
			{
				bool showAddPower=true;
				bool canAddPower=true;
				std::map<int, int>::iterator itMap=mCareerPageAttackInfo.chapterAddPowerMap.find(chapterId);
				if(itMap!=mCareerPageAttackInfo.chapterAddPowerMap.end())
				{
					if(itMap->second<ALLREADY_ADD_POWER)
					{
						showAddPower=true;
						if(itMap->second==1)
						{
							canAddPower=false;
						}
						else
						{
							canAddPower=true;
						}
					}
					else
					{
						showAddPower=false;
					}
				}
				addPowerBtn->setEnabled(canAddPower);
				if(showAddPower)
				{
					chapterAddPower->setVisible(true);
					chapterAddPower->setPosition(ccp(0,size.height));
					size.height += (chapterAddPower->getContentSize().height);
				}
				else
				{
					chapterAddPower->setVisible(false);
				}
			}
			else
			{
				chapterAddPower->setVisible(false);
			}
			chapterAddPower->retain();
			mCustomsPassContainerList.push_back(std::make_pair(CHAPTER_ADD_POWER,chapterAddPower));
		}
		//end if pass add this

		bool isHavLastClick=false;
		unsigned int detailCustomId=0;
		for(;it!=chapterData->mCustomsPassList.rend();it++)
		{
			CareerData::CustomsPassData* customsPass = *it;
			if(mLastClickCustomPassId==customsPass->id)
			{
				isHavLastClick=true;
			}
			CCBContainer* customsPassPanel = CCBManager::Get()->createAndLoad("CareerCustomsPass.ccbi");
			CCB_FUNC(customsPassPanel,"mRuningCircle",CCNode,setVisible(false));
			customsPassPanel->setListener(this,FACE_BUTTON_START+customsPass->id);
			CCMenuItemSprite* toggleNormal = dynamic_cast<CCMenuItemSprite*>(customsPassPanel->getVariable("mToggleNormal"));
			CCMenuItemSprite* toggleElite = dynamic_cast<CCMenuItemSprite*>(customsPassPanel->getVariable("mToggleElite"));
			CCMenuItemSprite* toggleBoss = dynamic_cast<CCMenuItemSprite*>(customsPassPanel->getVariable("mToggleBoss"));
			if(toggleNormal && toggleNormal->getParent())
			{
				CCMenu* menu = dynamic_cast<CCMenu*>(toggleNormal->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
			}
			toggleNormal->setVisible(customsPass->monsterType==CareerData::NORMAL);
			toggleNormal->setEnabled(customsPass->monsterType==CareerData::NORMAL);
			toggleElite->setVisible(customsPass->monsterType==CareerData::ELITE);
			toggleElite->setEnabled(customsPass->monsterType==CareerData::ELITE);
			toggleBoss->setVisible(customsPass->monsterType==CareerData::BOSS);
			toggleBoss->setEnabled(customsPass->monsterType==CareerData::BOSS);

			std::map<int, CustomsInfo>::iterator itAttackCustomsPassInfo=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(customsPass->id);
			CustomsInfo ci(0,0);
			if(itAttackCustomsPassInfo!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
			{
				ci=itAttackCustomsPassInfo->second;
			}
			CCMenuItemSprite* attack = dynamic_cast<CCMenuItemSprite*>(customsPassPanel->getVariable("mAttackButton"));
			if(attack && attack->getParent())
			{
				CCMenu* attackMenu = dynamic_cast<CCMenu*>(attack->getParent());
				if(attackMenu)
				{
					attackMenu->setScrollViewChild(true);
				}
			}

			CCLabelBMFont* passDes=dynamic_cast<CCLabelBMFont*>(customsPassPanel->getVariable(("mPassDes")));
			if(passDes)
			{
				std::string des;
				int lines;
				int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("CareerPassPlot_Describe"));
				GameMaths::stringAutoReturn(Language::Get()->getString(customsPass->plot),des,width,lines);
				passDes->setString(des.c_str());
			}

			CCLabelBMFont* physicalConsume=dynamic_cast<CCLabelBMFont*>(customsPassPanel->getVariable(("mPhysicalConsume")));
			if(physicalConsume)
			{
				char physical[128];
				sprintf(physical,"%d",customsPass->consumeStrength);
				physicalConsume->setString(physical);
			}

			CCLabelBMFont* todayAttack=dynamic_cast<CCLabelBMFont*>(customsPassPanel->getVariable(("mTodayAttack")));
			if(todayAttack)
			{
				char attackNum[128];
				sprintf(attackNum,"%d%s%d",ci.customsPassCount,"/",customsPass->limitAttackNum);
				todayAttack->setString(attackNum);
			}

			if(ci.customsPassCount>=customsPass->limitAttackNum)
			{
				CCB_FUNC(customsPassPanel,"mCashSymbol",CCSprite,setVisible(true));
			}
			else
			{
				CCB_FUNC(customsPassPanel,"mCashSymbol",CCSprite,setVisible(false));
			}
		
			CCB_FUNC(customsPassPanel,"mSuggestLevel",CCLabelBMFont,setString(StringConverter::toString(customsPass->suggestLevel).c_str()));
			CCB_FUNC(customsPassPanel,"mWinRewardExp",CCLabelBMFont,setString(StringConverter::toString(customsPass->winRewardExp).c_str()));
			CCB_FUNC(customsPassPanel,"mWinRewardCoins",CCLabelBMFont,setString(StringConverter::toString(customsPass->winRewardCoins).c_str()));
			CCB_FUNC(customsPassPanel,"mName",CCLabelBMFont,setString(customsPass->name.c_str()));
			CCB_FUNC(customsPassPanel,"mName1",CCLabelBMFont,setString(customsPass->name.c_str()));
			CCB_FUNC(customsPassPanel,"mFace",CCSprite,initWithFile(customsPass->icon.c_str()));

			bool isHavReward=false;
			int _index=1;
			for(int i=0;i<WIN_REWARD_PROPS_LENGTH;++i)
			{
				if(customsPass->winRewardProps[i]._type==CareerData::CustomsPassData::WINREWARD::DON_NOT_HAVE)
				{
					char nodeKey[128];
					sprintf(nodeKey,"mRewardNode%d",_index);
					CCB_FUNC(customsPassPanel,nodeKey,CCNode,setVisible(false));
					continue;
				}
				else
				{
					std::string icon="";
					std::string name="";
					unsigned int quality=0;
					isHavReward=true;
					if(customsPass->winRewardProps[i]._type==CareerData::CustomsPassData::WINREWARD::EQUIPMENT)
					{
						const EquipItem* item=EquipTableManager::Get()->getEquipItemByID(customsPass->winRewardProps[i]._id);
						if(item)
						{
							icon=item->iconPic;
							name=item->name;
							quality=item->quality;
						}
					}
					else if(customsPass->winRewardProps[i]._type==CareerData::CustomsPassData::WINREWARD::SKILL)
					{
						const SkillItem* item=SkillTableManager::Get()->getSkillItemByID(customsPass->winRewardProps[i]._id);
						if(item)
						{
							icon=item->iconPic;
							name=item->name;
							quality=item->quality;
						}
					}
					char key[128];
					sprintf(key,"mRewardNode%d",_index);
					CCB_FUNC(customsPassPanel,key,CCNode,setVisible(true));
					if(icon.length()>0)
					{
						sprintf(key,"mRewardIco%d",_index);
						CCB_FUNC(customsPassPanel,key,CCSprite,initWithFile(icon.c_str()));
					}
					if(name.length()>0)
					{
						sprintf(key,"mRewardTex%d",_index);
						CCB_FUNC(customsPassPanel,key,CCLabelBMFont,setString(name.c_str()));
					}
					sprintf(key,"mRewardFrame%d",_index);
					CCB_FUNC(customsPassPanel,key,CCMenuItemImage,setNormalImage(::getFrameNormalSpirte(quality>0?quality:4)));
					CCB_FUNC(customsPassPanel,key,CCMenuItemImage,setNormalImage(::getFrameSelectedSpirte(quality>0?quality:4)));
				}
				++_index;
			}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			CCB_FUNC(customsPassPanel,"mQuickAttack",CCNode,setVisible(false));
#endif
			CCB_FUNC(customsPassPanel,"mReward",CCNode,setVisible(isHavReward));
			node->addChild(customsPassPanel);
			CCSize relativeSize = CCSize(0,0);
			cocos2d::CCNode *detailNode=dynamic_cast< cocos2d::CCNode* >(customsPassPanel->getVariable("mCustomsPassDetailNode"));
			std::map<int, int>::iterator itMap=mShowCustomsPassDetailMap.find(customsPass->id);
			if(detailNode&&detailNode->getParent())
			{
				if(itMap==mShowCustomsPassDetailMap.end())
				{
					detailNode->setVisible(false);
					relativeSize.height=detailNode->getContentSize().height;
				}
				else
				{
					detailCustomId=customsPass->id;
				}
			}
			customsPassPanel->retain();
			mCustomsPassContainerList.push_back(std::make_pair(customsPass->id,customsPassPanel));
			if(customsPass->id<=mCareerPageAttackInfo.currCustomsPassId||(mCareerPageAttackInfo.currCustomsPassMaxId>mCareerPageAttackInfo.currCustomsPassId&&customsPass->id==(mCareerPageAttackInfo.currCustomsPassId+1)))
			{
				customsPassPanel->setVisible(true);
				customsPassPanel->setPosition(ccp(0,size.height-relativeSize.height));
				size.width = customsPassPanel->getContentSize().width;
				size.height += (customsPassPanel->getContentSize().height-relativeSize.height);
				if(customsPass->id==customsPassId&&customsPassId!=-1)
				{
					showOffset.height=size.height;
					if(itMap==mShowCustomsPassDetailMap.end())
					{
						reviseOffset.height=REVISE_HEIGHT;
					}
				}
				if(customsPass->id==(mCareerPageAttackInfo.currCustomsPassId+1))
				{
					if(attack)
					{
						attack->setVisible(false);
					}
				}
				else
				{
					if(attack)
					{
						if(InstructionManager::Get()->loadLocalInstructionStep()>=InstructionManager::TOTAL_INSTRUCTION||ServerDateManager::Get()->getUserBasicInfo().level>2)
						{
							if(ci.customsPassEvaluate<=0)
							{
								CCB_FUNC(customsPassPanel,"mRuningCircle",CCNode,setVisible(true));
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCParticleSystemQuad,setPositionType(kCCPositionTypeGrouped));
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCParticleSystemQuad,setPositionType(kCCPositionTypeGrouped));
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCParticleSystemQuad,setPositionType(kCCPositionTypeGrouped));
								if(!mIsFirstIn)
								{
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCNode,scheduleUpdateWithPriority(1));
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCNode,scheduleUpdateWithPriority(1));
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCNode,scheduleUpdateWithPriority(1));
								}
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCParticleSystemQuad,resetSystem());
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCParticleSystemQuad,resetSystem());
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCParticleSystemQuad,resetSystem());
							}
							else
							{
								CCB_FUNC(customsPassPanel,"mRuningCircle",CCNode,setVisible(false));
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCParticleSystemQuad,stopSystem());
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCParticleSystemQuad,stopSystem());
								CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCParticleSystemQuad,stopSystem());
							}
						}
						attack->setVisible(true);
					}
				}
			}
			else
			{
				customsPassPanel->setVisible(false);
			}
			setEvaluatePostion(customsPassPanel,customsPass->id,ci);
		}

		CCBContainer* chapterDesPanel = CCBManager::Get()->createAndLoad("CareerChapterDes.ccbi");
		if(chapterDesPanel)
		{
			CCLabelBMFont* chapterDesFont=dynamic_cast<CCLabelBMFont*>(chapterDesPanel->getVariable(("mChapterDes")));
			if(chapterDesFont)
			{
				std::string des;
				int lines;
				int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("CareerChapterPlot_Describe"));
				GameMaths::stringAutoReturn(Language::Get()->getString(chapterData->plot),des,width,lines);
				chapterDesFont->setString(des.c_str());
			}
		}
		node->addChild(chapterDesPanel);
		chapterDesPanel->setPosition(ccp(0,size.height));
		size.height += (chapterDesPanel->getContentSize().height);
		chapterDesPanel->retain();
		mCustomsPassContainerList.push_back(std::make_pair(CHAPTER_DES_PANEL,chapterDesPanel));

		node->setContentSize(size);
		mCustomsPassScrollView->setContainer(node);
		mCustomsPassScrollView->forceRecaculateChildren();
		mCustomsPassScrollView->setContentSize(size);
		if(isHavLastClick)
		{
			ToggleCareerCustomsPass(mCurrChapterIndex,mLastClickCustomPassId);
		}
		else
		{
			if(detailCustomId!=0)
			{
				ToggleCareerCustomsPass(mCurrChapterIndex,detailCustomId);
			}
			else
			{
				float offset=mCustomsPassScrollView->minContainerOffset().y;
				if(showOffset.height!=0.0f)
				{
					offset+=size.height-showOffset.height;
					if(offset!=0.0f)
					{
						offset-=reviseOffset.height;
					}
				}
				if(mCustomsPassScrollViewViewHeight>=size.height)
				{
					offset=mCustomsPassScrollViewViewHeight-size.height;
				}
				else
				{
					mCustomsPassScrollView->setContentOffset(mLastOffset);
				}
				mCustomsPassScrollView->setContentOffset(ccp(0,offset));
			}
		}
		
	}
	//
	setChapterButtonStauts();
	//
}
void CareerPage::resetLastClickCustomPassId(unsigned int passId)
{
	mLastClickCustomPassId = passId;

}
void CareerPage::setEvaluatePostion(CCBContainer* container,int currPassId,CustomsInfo ci)
{
	CCSize size = CCSize(0,0);
	for(int i=1;i<=CAREEREVALUATELEVEL;++i)
	{
		char symbol[128];
		sprintf(symbol,"%s%d","mEvaluate",i);
		CCSprite* face= dynamic_cast<CCSprite*>(container->getVariable(symbol));
		CCMenuItemSprite* attack = dynamic_cast<CCMenuItemSprite*>(container->getVariable("mAttackButton"));
		face->setVisible(true);
		if(CAREEREVALUATELEVEL!=ci.customsPassEvaluate)
		{
			if(i>ci.customsPassEvaluate)
			{
				face->setVisible(false);
			}
			else
			{
				face->setVisible(true);
				size.width=(attack->getContentSize().width-face->getContentSize().width*ci.customsPassEvaluate)*0.5+face->getContentSize().width*(i-1);
			}
		}
	
	}
}

void CareerPage::InitPassChapterButton()
{

}

void CareerPage::changeChapterButtonStatus()
{
	if(mNavContent)
	{
		mShouldPlayCoversationID = mCurrChapterIndex;
		//showStory(mCurrChapterIndex);

		CCSize size = CCSize(0,0);
		std::list<std::pair<int,CCBContainer*> >::iterator it=mChapterContainerList.begin();
		for(;it!=mChapterContainerList.end();it++)
		{
			std::pair<int,CCBContainer*> ccbContainerPair=*it;
			CCBContainer* chapterNavButton =ccbContainerPair.second;
			if(ccbContainerPair.first<=mCareerPageAttackInfo.currChapterId)
			{
				cocos2d::CCObject* obj = chapterNavButton->getVariable("mFrameBack");
				if(!obj)
				{
					CCMessageBox("Failed to find variable:mFrameBack","error");
				}
				CCMenuItemSprite* frame = dynamic_cast<CCMenuItemSprite*>(obj);
				frame->setEnabled(true);
				if(ccbContainerPair.first==mCareerPageAttackInfo.currChapterId)
				{
					CareerData::ChaptersData* chapterData =CareerConfigManager::Get()->getChaptersDataByChaptersId(ccbContainerPair.first);
					std::list< CareerData::CustomsPassData* >::reverse_iterator it=chapterData->mCustomsPassList.rbegin();
					if(it!=chapterData->mCustomsPassList.rend())
					{
						mCareerPageAttackInfo.currCustomsPassMaxId=(*it)->id;
						it=--chapterData->mCustomsPassList.rend();
						mCareerPageAttackInfo.currCustomsPassId=(*it)->id;
						mCareerPageAttackInfo.chapterAddPowerMap.insert(std::map<int, int>::value_type (mCareerPageAttackInfo.currChapterId,NOT_ALLOW_ADD_POWER));
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	//
	setChapterButtonStauts();
	//
}

void CareerPage::ToggleCareerCustomsPass(int chapterId,int customsPassId)
{
	mLastClickCustomPassId=customsPassId;
	CCSize showOffset=CCSize(0,0);
	CCSize reviseOffset=CCSize(0,0);
	if(mCustomsPassScrollView)
	{
		cocos2d::CCNode* node = mCustomsPassScrollView->getContainer();
		CCSize size = CCSize(0,0);
		std::list<std::pair<int,CCBContainer*> >::iterator it=mCustomsPassContainerList.begin();
		unsigned int passNo = 0;
		for(;it!=mCustomsPassContainerList.end();it++)
		{
			std::pair<int,CCBContainer*> ccbContainerPair=*it;
			CCBContainer* customsPassPanel =ccbContainerPair.second;
			CCSize relativeSize = CCSize(0,0);
			std::map<int, int>::iterator itMap=mShowCustomsPassDetailMap.find(ccbContainerPair.first);
			if(ccbContainerPair.first==CHAPTER_DES_PANEL)
			{

			}
			else if(ccbContainerPair.first==CHAPTER_PLOT_RELOAD||ccbContainerPair.first==CHAPTER_PLOT_NEXT||ccbContainerPair.first==CHAPTER_ADD_POWER)
			{
				if(chapterId<mCareerPageAttackInfo.currChapterId)
				{
					customsPassPanel->setVisible(true);
					if(ccbContainerPair.first==CHAPTER_ADD_POWER)
					{
						CCMenuItemSprite* addPowerBtn = dynamic_cast<CCMenuItemSprite*>(customsPassPanel->getVariable("mAddPowerButton"));
						std::map<int, int>::iterator itMap=mCareerPageAttackInfo.chapterAddPowerMap.find(chapterId);
						bool showAddPower=true;
						bool canAddPower=true;
						if(itMap!=mCareerPageAttackInfo.chapterAddPowerMap.end())
						{
							if(itMap->second<ALLREADY_ADD_POWER)
							{
								showAddPower=true;
								if(itMap->second==1)
								{
									canAddPower=false;
								}
								else
								{
									canAddPower=true;
								}
							}
							else
							{
								showAddPower=false;
							}
						}
						addPowerBtn->setEnabled(canAddPower);
						if(showAddPower)
						{
							customsPassPanel->setVisible(true);
						}
						else
						{
							customsPassPanel->setVisible(false);
							continue;
						}
					}
				}
				else
				{
					continue;
				}
			}
			else
			{
				if(ccbContainerPair.first<=mCareerPageAttackInfo.currCustomsPassId||(mCareerPageAttackInfo.currCustomsPassMaxId>mCareerPageAttackInfo.currCustomsPassId&&ccbContainerPair.first==(mCareerPageAttackInfo.currCustomsPassId+1)))
				{
					CCB_FUNC(customsPassPanel,"mRuningCircle",CCNode,setVisible(false));
					customsPassPanel->setVisible(true);
					cocos2d::CCNode *detailNode=dynamic_cast< cocos2d::CCNode* >(customsPassPanel->getVariable("mCustomsPassDetailNode"));
					if(detailNode&&detailNode->getParent())
					{
						if(itMap==mShowCustomsPassDetailMap.end())
						{
							detailNode->setVisible(false);
							relativeSize.height=detailNode->getContentSize().height;
							CCB_FUNC(customsPassPanel,"mQuickAttack",CCNode,setVisible(false));
						}
						else
						{
							detailNode->setVisible(true);
						}
					}
					
					std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(ccbContainerPair.first);
					CustomsInfo ci(0,0);
					if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
					{
						ci=itMap->second;
					}
					int limitCount=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(chapterId,ccbContainerPair.first)->limitAttackNum;
					CCLabelBMFont* todayAttack=dynamic_cast<CCLabelBMFont*>(customsPassPanel->getVariable(("mTodayAttack")));
					if(todayAttack)
					{
						char attackNum[128];
						
						sprintf(attackNum,"%d%s%d",ci.customsPassCount,"/",limitCount);
						todayAttack->setString(attackNum);
					}
					CCSprite* cashSymbol=dynamic_cast<CCSprite*>(customsPassPanel->getVariable(("mCashSymbol")));
					if(cashSymbol)
					{
						if(ci.customsPassCount>=limitCount)
						{
							cashSymbol->setVisible(true);
						}
						else
						{
							cashSymbol->setVisible(false);
						}
					}
					setEvaluatePostion(customsPassPanel,ccbContainerPair.first,ci);
					CCMenuItemSprite* attack = dynamic_cast<CCMenuItemSprite*>(customsPassPanel->getVariable("mAttackButton"));
					if(ccbContainerPair.first==(mCareerPageAttackInfo.currCustomsPassId+1))
					{
						if(attack)
						{
							attack->setVisible(false);
						}
					}
					else
					{
						if(attack)
						{
							if(InstructionManager::Get()->loadLocalInstructionStep()>=InstructionManager::TOTAL_INSTRUCTION-1||ServerDateManager::Get()->getUserBasicInfo().level>2)
							{
								if(ci.customsPassEvaluate<=0)
								{
									CCB_FUNC(customsPassPanel,"mRuningCircle",CCNode,setVisible(true));
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCParticleSystemQuad,setPositionType(kCCPositionTypeGrouped));
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCParticleSystemQuad,setPositionType(kCCPositionTypeGrouped));
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCParticleSystemQuad,setPositionType(kCCPositionTypeGrouped));
									if(!mIsFirstIn)
									{
										CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCNode,scheduleUpdateWithPriority(1));
										CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCNode,scheduleUpdateWithPriority(1));
										CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCNode,scheduleUpdateWithPriority(1));
									}
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle1",CCParticleSystemQuad,resetSystem());
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle2",CCParticleSystemQuad,resetSystem());
									CCB_FUNC(customsPassPanel,"mRuningCircleParticle3",CCParticleSystemQuad,resetSystem());
								}
							}
							attack->setVisible(true);
						}
					}
				}
				else
				{
					customsPassPanel->setVisible(false);
					continue;
				}
			}
			customsPassPanel->setPosition(ccp(0,size.height-relativeSize.height));
			size.width = customsPassPanel->getContentSize().width;
			size.height += (customsPassPanel->getContentSize().height-relativeSize.height);
			if(ccbContainerPair.first==customsPassId&&customsPassId!=-1)
			{
				showOffset.height=size.height;
				if(itMap==mShowCustomsPassDetailMap.end())
				{
					//reviseOffset.height=REVISE_HEIGHT;
				}
				if(relativeSize.height>0)
				{
					showOffset.height += (customsPassPanel->getContentSize().height-relativeSize.height);
				}
				
			}
		}

		node->setContentSize(size);
		mCustomsPassScrollView->setContentSize(size);
		mCustomsPassScrollView->forceRecaculateChildren();
		float offset=mCustomsPassScrollView->minContainerOffset().y;
		if(showOffset.height!=0.0f)
		{
			offset+=size.height-showOffset.height;
			if(offset!=0.0f)
			{
				offset-=reviseOffset.height;
			}
		}
		if(mCustomsPassScrollViewViewHeight>=size.height)
		{
			offset=mCustomsPassScrollViewViewHeight-size.height;
		}
		else
		{
			mCustomsPassScrollView->setContentOffset(mLastOffset);
		}
		mCustomsPassScrollView->setContentOffsetInDuration(ccp(0,offset),0.0f);
	}
}

void CareerPage::_refreshCurrCustomPageQuickBtn(unsigned int chapterId)
{
	if (mCustomsPassScrollView)
	{
		bool _vipNotEnough=true;
		bool _vipNotEnoughGold=true;
		if (ServerDateManager::Get()->getUserBasicInfo().viplevel<StringConverter::parseInt(VaribleManager::Get()->getSetting("EnableQuickVIPLevel")))
		{
			_vipNotEnough = true;
		}else
		{
			_vipNotEnough = false;
		}
		if (ServerDateManager::Get()->getUserBasicInfo().viplevel<StringConverter::parseInt(VaribleManager::Get()->getSetting("EnableUserDiamonVipLevel")))
		{
			_vipNotEnoughGold = true;
		}else
		{
			_vipNotEnoughGold = false;
		}
		long timeleft = 0;
		if (TimeCalculator::Get()->getTimeLeft("coldtime", timeleft))
		{
			if(timeleft>0)
			{
				mIsQuickCarrerInCool = true;
			}
			else
			{
				timeleft=0;
				mIsQuickCarrerInCool = false;
			}
		}
		else
		{
			mIsQuickCarrerInCool = false;
		}
		mCareerPageAttackInfo.backwardsTime=timeleft;
		std::list<std::pair<int, CCBContainer*> >::iterator it = mCustomsPassContainerList.begin();
		for (;it!=mCustomsPassContainerList.end();++it)
		{
			std::pair<int, CCBContainer*> ccbContainerPair = *it;
			
			if (ccbContainerPair.first<mCareerPageAttackInfo.currCustomsPassId && ccbContainerPair.first!=CHAPTER_PLOT_NEXT && ccbContainerPair.first!=CHAPTER_PLOT_RELOAD && ccbContainerPair.first!= CHAPTER_ADD_POWER && ccbContainerPair.first!= CHAPTER_DES_PANEL)
			{
				CCB_FUNC(ccbContainerPair.second, "mQuickAttack", CCNode, setVisible(true));//add by xinghui
				CCNode* customDetailNode = dynamic_cast<CCNode*>(ccbContainerPair.second->getVariable("mCustomsPassDetailNode"));
				if(!customDetailNode->isVisible())
				{
					CCB_FUNC(ccbContainerPair.second, "mQuickAttack", CCNode, setVisible(false));
					continue;
				}
				std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(ccbContainerPair.first);
				CustomsInfo ci(0,0);
				if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
				{
					ci=itMap->second;
				}

				int _canAttack = 0;
				//add by xinghui
				if (CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(chapterId,ccbContainerPair.first))
				{
					_canAttack=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(chapterId,ccbContainerPair.first)->limitAttackNum-ci.customsPassCount;
				}else{
					continue;
				}
				
				CCMenuItemImage* quickCareerBattleButton = dynamic_cast<CCMenuItemImage*>(ccbContainerPair.second->getVariable("mQuickAttackButton"));
				if (mShowCustomsPassDetailMap.find(ccbContainerPair.first)!=mShowCustomsPassDetailMap.end() && mShowCustomsPassDetailMap.find(ccbContainerPair.first)->second == 1)
				{
					CCB_FUNC(ccbContainerPair.second, "mQuickAttack", CCNode, setVisible(true));
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackTime",CCLabelBMFont,setVisible(false));
				}

				if (_vipNotEnough)
				{
					quickCareerBattleButton->setNormalImage(CCSprite::create(VaribleManager::Get()->getSetting("QuickCareerBtnGray").c_str()));
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackTime",CCLabelBMFont,setVisible(false));
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackN",CCLabelBMFont,setString(Language::Get()->getString("@QuickCareerText").c_str()));
				}
				if (_canAttack<= 0)
				{
					CCB_FUNC(ccbContainerPair.second, "mQuickAttack", CCNode, setVisible(false));
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackTime",CCLabelBMFont,setVisible(false));

					CCSprite* cashSymbol=dynamic_cast<CCSprite*>(ccbContainerPair.second->getVariable(("mCashSymbol")));
					if(cashSymbol)
					{
						cashSymbol->setVisible(true);
					}

					continue;//add by xinghui
				}
				if (mIsQuickCarrerInCool)
				{
					CCB_FUNC(ccbContainerPair.second, "mQuickAttack", CCNode, setVisible(true));
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackTime",CCLabelBMFont,setVisible(true));
					quickCareerBattleButton->setNormalImage(CCSprite::create(VaribleManager::Get()->getSetting("QuickCareerBtnGray").c_str()));
					std::list<std::string> replaceList;
					char str[64];
					int hh = timeleft/3600;
					int mm = (timeleft-3600*hh)/60;
					int ss = timeleft%60;
					sprintf(str,"%02d:%02d:%02d",hh,mm,ss);
					replaceList.push_back(str);
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackTime",CCLabelBMFont,setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@QuickCareerSurplusTime"),&replaceList).c_str()));
					CCB_FUNC(ccbContainerPair.second,"mQuickAttackN",CCLabelBMFont,setString(Language::Get()->getString("@QuickCareerText").c_str()));
				}
				else
				{
					if(_canAttack>0)
					{
						CCB_FUNC(ccbContainerPair.second, "mQuickAttack", CCNode, setVisible(true));
						_canAttack=_canAttack>ServerDateManager::Get()->getUserBasicInfo().power?ServerDateManager::Get()->getUserBasicInfo().power :_canAttack;
						CCB_FUNC(ccbContainerPair.second,"mQuickAttackTime",CCLabelBMFont,setVisible(false));
						//add by xinghui
						if (_canAttack>StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickCareerMaxNum").c_str()))
						{
							_canAttack = StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickCareerMaxNum").c_str());
						}
						//end
						if(_canAttack>0)
						{
							std::list<std::string> strList;
							strList.push_back(StringConverter::toString(_canAttack));
							CCB_FUNC(ccbContainerPair.second, "mQuickAttackN", CCLabelBMFont, setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@QuickCareerTextWithCount"), &strList).c_str()));
							if (!_vipNotEnough)
							{
								quickCareerBattleButton->setNormalImage(CCSprite::create(VaribleManager::Get()->getSetting("QuickCareerBtnNormal").c_str()));//add by xinghui
							}
						}
						else
						{
							CCB_FUNC(ccbContainerPair.second, "mQuickAttackN", CCLabelBMFont, setString(Language::Get()->getString("@QuickCareerText").c_str()));
							quickCareerBattleButton->setNormalImage(CCSprite::create(VaribleManager::Get()->getSetting("QuickCareerBtnGray").c_str()));//add by xinghui
						}
					}
				}
			}
		}
	}
}

void CareerPage::_refreshCurrCustomPage(unsigned int chapterId,unsigned int cusstomPassId)
{
	if(mCustomsPassScrollView)
	{
		cocos2d::CCNode* node = mCustomsPassScrollView->getContainer();
		std::list<std::pair<int,CCBContainer*> >::iterator it=mCustomsPassContainerList.begin();
		for(;it!=mCustomsPassContainerList.end();it++)
		{
			std::pair<int,CCBContainer*> ccbContainerPair=*it;
			CCBContainer* customsPassPanel =ccbContainerPair.second;
			if(ccbContainerPair.first==cusstomPassId)
			{
				std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(ccbContainerPair.first);
				CustomsInfo ci(0,0);
				if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
				{
					ci=itMap->second;
				}
				int limitCount=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(chapterId,ccbContainerPair.first)->limitAttackNum;
				CCLabelBMFont* todayAttack=dynamic_cast<CCLabelBMFont*>(customsPassPanel->getVariable(("mTodayAttack")));
				if(todayAttack)
				{
					char attackNum[128];

					sprintf(attackNum,"%d%s%d",ci.customsPassCount,"/",limitCount);
					todayAttack->setString(attackNum);
				}
				CCSprite* cashSymbol=dynamic_cast<CCSprite*>(customsPassPanel->getVariable(("mCashSymbol")));
				if(cashSymbol)
				{
					if(ci.customsPassCount>=limitCount)
					{
						cashSymbol->setVisible(true);
					}
					else
					{
						cashSymbol->setVisible(false);
					}
				}
				setEvaluatePostion(customsPassPanel,ccbContainerPair.first,ci);
			}
		}
	}
}

void CareerPage::_clearPassContainer()
{
	mCustomsPassScrollView->getContainer()->removeAllChildren();
	ContentList::iterator itr = mCustomsPassContainerList.begin();
	while(itr != mCustomsPassContainerList.end())
	{
		if(itr->second)
		{
			itr->second->release();
		}
		++itr;
	}
	mCustomsPassContainerList.clear();
}

void CareerPage::_clearNavContainer()
{
	mNavContent->getContainer()->removeAllChildren();
	ContentList::iterator itr = mChapterContainerList.begin();
	while(itr != mChapterContainerList.end())
	{
		if(itr->second)
			itr->second->release();
		++itr;
	}
	mChapterContainerList.clear();
}


void CareerPage::Exit( MainFrame* frame)
{
	mIsFirstIn=false;
	MessageManager::Get()->removeMessageHandler(this);
	PacketManager::Get()->removePacketHandler(this);
	_clearPassContainer();
	_clearNavContainer();
}

void CareerPage::Execute( MainFrame* )
{
	static float durationTime=0;
	durationTime+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>0.2f)
	{
		durationTime -=0.2f;
		_refreshCurrCustomPageQuickBtn(mCurrChapterIndex);
	}
}

void CareerPage::load( void )
{
	loadCcbiFile("CareerPage.ccbi");
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
}

void CareerPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onCareerChapterButton")
	{
		if(mCurrChapterIndex!=tag)
		{
			mCurrChapterIndex=tag;
			MsgCareerChangePage msg;
			msg.pageID = tag;
			MessageManager::Get()->sendMessage(&msg);
		}
		resetNavOffset(tag);
	}
	else if(itemName=="onCareerAttackButton")
	{
		mIsAttack=true;
		mCurretFightingTag = tag;
		if(ServerDateManager::Get()->getUserBasicInfo().power>0)
		{
			std::string _fightEnemyNameKey="fightEnemyNameKey";
			if(HAS_V(_fightEnemyNameKey))
			{
				SET_V(_fightEnemyNameKey,CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,tag)->name);
			}
			else
			{
				ADD_V(_fightEnemyNameKey,CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,tag)->name);
			}
			std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(tag);
			if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
			{
				CustomsInfo& ci=itMap->second;
				int limitCount=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,tag)->limitAttackNum;				
				if(ci.customsPassCount>=limitCount)
				{
					const UserBasicInfo& basicInfo = ServerDateManager::Get()->getUserBasicInfo();
					BlackBoard::Get()->setCareerAttackChapterId(mCurrChapterIndex);
					BlackBoard::Get()->setCareerAttackCustomsId(tag);
					if(basicInfo.viplevel>=3)
					{
						POP_BOX(CareerAttackNumLimit);
					}
					else
					{
						//std::string str = Language::Get()->getString("@CareerGotLimit");
						//MSG_BOX(str);
						POP_BOX(CareerAttackNumVipTip);
						CCLOG("onMenuItemAction onCareerAttackButton today attack limit customsId:%d,attackCount/limitCount:%d/%d",tag,ci.customsPassCount,limitCount);
					}
					return;
				}
			}
			OPUserBattle pkt;
			pkt.set_opponentid(0);
			pkt.set_version(1);
			pkt.set_type(OPUserBattle_Type_CARRER);
			pkt.set_stage(tag);
			PacketManager::Get()->sendPakcet(OPCODE_USER_BATTLE_C,&pkt);
			//lib91::getInstance()->setWaiting(true);
			//waitingManager::Get()->startWaiting(OPCODE_USER_BATTLERET_S);
		}
		else
		{
			POP_NOT_ENOUGH_BOX(PowerNotEnough);
		}
	}
	else if(itemName=="onToggleButton")
	{
		mLastOffset=mCustomsPassScrollView->getContentOffset();
		if(!mIsAttack)
		{
			std::map<int, int>::iterator it=mShowCustomsPassDetailMap.find(tag);

			if(it!=mShowCustomsPassDetailMap.end())
			{
				mShowCustomsPassDetailMap.erase(tag);
			}
			else
			{
				mShowCustomsPassDetailMap.insert(std::map<int, int>::value_type (tag,1));
			}
		}
		mIsAttack=false;
		ToggleCareerCustomsPass(mCurrChapterIndex,tag);
	}
	else if(itemName=="onPassChapterButton")
	{
		if(tag==CHAPTER_PLOT_RELOAD)
		{
			showStory(mCurrChapterIndex);
		}
		else if(tag==CHAPTER_PLOT_NEXT)
		{
			if(mCurrChapterIndex+1 <= mCareerPageAttackInfo.currChapterId)
				onMenuItemAction( "onCareerChapterButton", sender, mCurrChapterIndex+1 );
		}
	}
	else if(itemName=="onAddPowerButton")
	{
		if(_canAddPower(mCurrChapterIndex))
		{
			mCareerPageAttackInfo.chapterAddPowerMap[mCurrChapterIndex]=ALLREADY_ADD_POWER;
			ToggleCareerCustomsPass(mCurrChapterIndex);
			OPAddPowerCareer msg;
			msg.set_version(1);
			msg.set_chapterid(mCurrChapterIndex);
			PacketManager::Get()->sendPakcet(OPCODE_ADDPOWER_CAREER_C, &msg);
		}
		else
		{
			MSG_BOX(Language::Get()->getString("@CareerCanNotAddPower"));
		}
	}
	else if (itemName == "onQuickAttackButton")
	{
		mCurretFightingTag = tag;
		if (ServerDateManager::Get()->getUserBasicInfo().viplevel<StringConverter::parseInt(VaribleManager::Get()->getSetting("EnableQuickVIPLevel")))
		{
			MSG_BOX_LAN("@QuickCareetNeedVipLevel");
		}
		else if (mIsQuickCarrerInCool && ServerDateManager::Get()->getUserBasicInfo().viplevel<StringConverter::parseInt(VaribleManager::Get()->getSetting("EnableUserDiamonVipLevel")))
		{
			MSG_BOX_LAN("@QuickCareerDiamondNeedVipLevel");
		}
		else if(ServerDateManager::Get()->getUserBasicInfo().power<=0)
		{
			POP_NOT_ENOUGH_BOX(PowerNotEnough);
		}
		else if (mIsQuickCarrerInCool && ServerDateManager::Get()->getUserBasicInfo().viplevel>=StringConverter::parseInt(VaribleManager::Get()->getSetting("EnableUserDiamonVipLevel")))
		{
			POP_NOT_ENOUGH_BOX(SpendDiamondClearCoolTime);
		}
		else
		{
			std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(tag);
			CustomsInfo ci(0,0);
			if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
			{
				ci=itMap->second;
			}
			int _canAttack=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,tag)->limitAttackNum-ci.customsPassCount;
			_canAttack=_canAttack>ServerDateManager::Get()->getUserBasicInfo().power?ServerDateManager::Get()->getUserBasicInfo().power :_canAttack;
			if(_canAttack<=0)
			{
				MSG_BOX_LAN("@CareerQuickAttackCountError");
				return;
			}
			//add by xinghui
			if (_canAttack>StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickCareerMaxNum").c_str()))
			{
				_canAttack = StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickCareerMaxNum").c_str());
			}
			//end
			OPQuickCareerBattle quickBattle;
			quickBattle.set_version(1);
			quickBattle.set_stageid(tag);
			quickBattle.set_battletimes(_canAttack);
			quickBattle.set_type(OPQuickCareerBattle_Type_CARRER);
			quickBattle.set_opponentid(1);
			PacketManager::Get()->sendPakcet(OPCODE_QUICK_CAREER_BATTLE_C, &quickBattle);
		}
	}
}

bool CareerPage::_canAddPower(unsigned int chapterId)
{
	if(mCareerPageAttackInfo.chapterAddPowerMap.find(chapterId)!=mCareerPageAttackInfo.chapterAddPowerMap.end())
	{
		unsigned int type=mCareerPageAttackInfo.chapterAddPowerMap.find(chapterId)->second;
		if(type==ALLREADY_ADD_POWER)
		{
			return false;
		}
	}
	CareerData::ChaptersData* chapterData =CareerConfigManager::Get()->getChaptersDataByChaptersId(chapterId);
	std::list< CareerData::CustomsPassData* >::reverse_iterator it=chapterData->mCustomsPassList.rbegin();
	bool _canAdd=true;
	for(;it!=chapterData->mCustomsPassList.rend();++it)
	{
		unsigned int passId=(*it)->id;
		if(mCareerPageAttackInfo.attackCustomsPassInfoMap.find(passId)!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
		{
			if(mCareerPageAttackInfo.attackCustomsPassInfoMap.find(passId)->second.customsPassEvaluate!=CAREEREVALUATELEVEL)
			{
				_canAdd=false;
				break;
			}
		}
		else
		{
			_canAdd=false;
			break;
		}
	}
	return _canAdd;
}

void CareerPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{
	if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void CareerPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_CAREER_CHANGEPAGE)
	{
		const MsgCareerChangePage* msg = dynamic_cast<const MsgCareerChangePage*>(message);
		if(msg)
			ShowCareerCustomsPass(msg->pageID);
	}
	else if(message->getTypeId() == MSG_FIGHT_END)
	{
		if(mShouldPlayCoversationID>0)
		{
			showStory(mShouldPlayCoversationID);
			mShouldPlayCoversationID = -1;
		}
		_refreshTitle();
	}
	else if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S||info->opcode==OPCODE_ADD_POWERRET_S||info->opcode==OPCODE_ADDPOWER_CAREERRET_S)
		{
			_refreshTitle();
		}
		else if(info->opcode==OPCODE_RESET_CAREER_COUNTRET_S)
		{
			_refreshCurrCustomPage(mCurrChapterIndex,mCurretFightingTag);
			_refreshTitle();
		}
		else if(info->opcode==OPCODE_PLAYER_LOGINRET_S)
		{
			if(BlackBoard::Get()->IsTimeoutResponse)
			{
				OPUserCareerInfo career;
				career.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_USER_CAREER_INFO_C,&career);
				OPUserBasicInfo info;
				info.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_USER_BASIC_INFO_C,&info);
			}
		}
		else if(info->opcode==OPCODE_GET_USER_CAREER_INFORET_S)
		{
			if(BlackBoard::Get()->IsTimeoutResponse)
			{
				BlackBoard::Get()->IsTimeoutResponse = false;
				_refreshCurrCustomPage(mCurrChapterIndex,mCurretFightingTag);
			}
			
		}
	}
}

void CareerPage::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	if(opcode == _getOpcode())
	{
		std::string debugstr = packet->DebugString();

		//lib91::getInstance()->setWaiting(false);
		const OPUserBattleRet* msg = dynamic_cast<const OPUserBattleRet*>(packet);
		if(msg)
		{
			bool isShowThreeEvaluation=false;
			int gotpoint = 0;
			switch(msg->result().winlose())
			{
			case OPUserBattleRet_Result_WINLOSE_BIGWIN:
				gotpoint = 3;
				break;
			case OPUserBattleRet_Result_WINLOSE_WIN:
				gotpoint = 2;
				break;
			case OPUserBattleRet_Result_WINLOSE_JUSTWIN:
				gotpoint = 1;
				break;
			case OPUserBattleRet_Result_WINLOSE_JUSTLOSE:
				gotpoint = -1;
				break;
			case OPUserBattleRet_Result_WINLOSE_LOSE:
				gotpoint = -2;
				break;
			case OPUserBattleRet_Result_WINLOSE_BIGLOSE:
				gotpoint = -3;
				break;
			}
			bool hideFightSkip = true;
			std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(mCurretFightingTag);
			if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
			{
				CustomsInfo& ci=itMap->second;
				if(ci.customsPassEvaluate>=1)
					hideFightSkip = false;
			}
			std::string fightRetKey="careerFightFail";
			if(gotpoint>0)
			{
				if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
				{
					CustomsInfo& ci=itMap->second;
					
					int limitCount=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,mCurretFightingTag)->limitAttackNum;
					if(ci.customsPassCount>=limitCount)
					{
						char strout[128];
						sprintf(strout,"onMenuItemAction onCareerAttackButton today attack limit customsId:%d,attackCount/limitCount:%d/%d",mCurretFightingTag,ci.customsPassCount,limitCount);
						CCMessageBox(strout,"Error!");
					}
					else
					{
						if(gotpoint>ci.customsPassEvaluate)
						{
							ci.customsPassEvaluate = gotpoint;
							if(ci.customsPassEvaluate==3)
							{
								isShowThreeEvaluation=true;
							}
						}
						++ci.customsPassCount;
						mCareerPageAttackInfo.attackCustomsPassInfoMap[mCurretFightingTag]=ci;
					}
				}
				else
				{
					CustomsInfo ci;
					ci.customsPassCount = 1;//CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,mCurretFightingTag)->limitAttackNum;
					ci.customsPassEvaluate = gotpoint;
					if(ci.customsPassEvaluate==3)
					{
						isShowThreeEvaluation=true;
					}
					mCareerPageAttackInfo.attackCustomsPassInfoMap.insert(std::map<int,CustomsInfo>::value_type (mCurretFightingTag,ci));
				}
				if(mCurretFightingTag==mCareerPageAttackInfo.currCustomsPassId)
				{
					if(mCareerPageAttackInfo.currCustomsPassId==mCareerPageAttackInfo.currCustomsPassMaxId)
					{
						mCareerPageAttackInfo.chapterAddPowerMap[mCareerPageAttackInfo.currChapterId]=ALLOW_ADD_POWER;
						mCareerPageAttackInfo.currChapterId++;
						changeChapterButtonStatus();
						if(CareerConfigManager::Get()->getChaptersDataByChaptersId(mCareerPageAttackInfo.currChapterId)!=NULL)
						{
							mCurrChapterIndex = mCareerPageAttackInfo.currChapterId;
							MsgCareerChangePage msg;
							msg.pageID = mCurrChapterIndex;
							MessageManager::Get()->sendMessage(&msg);
							resetNavOffset(mCurrChapterIndex);
						}
					}
					else
					{
						mCareerPageAttackInfo.currCustomsPassId++;
						ToggleCareerCustomsPass(mCurrChapterIndex,mCareerPageAttackInfo.currCustomsPassId);
					}
				}
				else
				{
					_refreshCurrCustomPage(mCurrChapterIndex,mCurretFightingTag);
				}
				if(HAS_V(fightRetKey))
				{
					SET_V(fightRetKey,true);
				}
				else
				{
					ADD_V(fightRetKey,true);
				}
			}
			else
			{
				if(HAS_V(fightRetKey))
				{
					SET_V(fightRetKey,false);
				}
				else
				{
					ADD_V(fightRetKey,false);
				}
			}
			if(msg->has_userpower())
			{
				ServerDateManager::Get()->getUserBasicInfo().power = msg->userpower(); 
			}
			BasePage* bp = CCBManager::Get()->getPage("FightPage");
			if(bp)
			{
				FightPage* fp = dynamic_cast<FightPage*>(bp);
				fp->setMessage(msg,isShowThreeEvaluation,hideFightSkip);
			}
			std::string fightCareerKey="fromCareerFight";
			if(HAS_V(fightCareerKey))
			{
				SET_V(fightCareerKey,true);
			}
			else
			{
				ADD_V(fightCareerKey,true);
			}
			MsgMainFrameCoverShow msg2;
			msg2.pageName = "FightPage";
			MessageManager::Get()->sendMessage(&msg2);
		}
	}
	else if(opcode == OPCODE_RESET_CAREER_COUNTRET_S)
	{
		const OPResetCareerCountRet* msg = dynamic_cast<const OPResetCareerCountRet*>(packet);
		if(msg)
		{
			if(msg->status()==1)
			{
				mCareerPageAttackInfo.attackCustomsPassInfoMap[mCurretFightingTag].customsPassCount = 0;
			}
		}
	}
	else if (opcode == OPCODE_QUICK_CAREER_BATTLERET_S)
	{
		std::list<std::string> showTextList;
		std::list<std::string> accidentObtainList;
		std::list<std::pair<std::string, std::string> > propertyList;
		const OPQuickCareerBattleRet * msg = dynamic_cast<const OPQuickCareerBattleRet *>(packet);
		if (msg->stats() == 0)
		{
			TimeCalculator::Get()->createTimeCalcultor("coldtime", msg->backwardstimes());
			int _consumePower=ServerDateManager::Get()->getUserBasicInfo().power-msg->userpower();
			std::map<int, CustomsInfo>::iterator itMap=mCareerPageAttackInfo.attackCustomsPassInfoMap.find(mCurretFightingTag);
			CustomsInfo ci(0,0);
			if(itMap!=mCareerPageAttackInfo.attackCustomsPassInfoMap.end())
			{
				ci=itMap->second;
			}
			int _canAttack=CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,mCurretFightingTag)->limitAttackNum-ci.customsPassCount;
			_canAttack=_canAttack>ServerDateManager::Get()->getUserBasicInfo().power?ServerDateManager::Get()->getUserBasicInfo().power :_canAttack;
			//add by xinghui
			if (_canAttack>StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickCareerMaxNum")))
			{
				_canAttack = StringConverter::parseInt(VaribleManager::Get()->getSetting("QuickCareerMaxNum"));
			}
			//end
			ServerDateManager::Get()->getUserBasicInfo().power = msg->userpower();
			for (int i=0; i<msg->result_size();i++)
			{
				std::string showText = "";
				ServerDateManager::Get()->getUserBasicInfo().exp += msg->result(i).gotexp();
				ServerDateManager::Get()->getUserBasicInfo().silvercoins += msg->result(i).gotcoin();//add by xinghui
				propertyList.push_back(std::list<std::pair<std::string, std::string> >::value_type(StringConverter::toString(msg->result(i).gotcoin()), StringConverter::toString(msg->result(i).gotexp())));
				for (int j=0; j<msg->result(i).characterresults_size();j++)
				{
					OPUserBattleRet_Result_CharacterResult character = msg->result(i).characterresults(j);
					UserDiscipleInfo* userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByItemID(character.resourceid());
					userDiscipleInfo->exp += character.gainexp();
					std::string discipleName = DiscipleTableManager::Get()->getDiscipleItemByID(userDiscipleInfo->itemid)->name;
					showText += discipleName + "+" + StringConverter::toString(character.gainexp())+",";
					userDiscipleInfo->level = character.level();
					if (character.isupgarade())
					{
						userDiscipleInfo->upgradelevel++;
					}
				}
				std::string accidentObtain = "";
				if (msg->result(i).has_gotitem())
				{
					OPUserBattleRet_Result_GotItemResult getItemRresult = msg->result(i).gotitem();
					switch(getItemRresult.type())
					{
					case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_TOOL:
						GetPropManager::Get()->gotTool(getItemRresult.tool());
						accidentObtain = StringConverter::toString(getItemRresult.count())+"x"+ToolTableManager::Get()->getToolItemByID(getItemRresult.tool().itemid())->name;
						break;
					case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_EQUIPMENT:
						GetPropManager::Get()->gotEquipment(getItemRresult.equip());
						accidentObtain = StringConverter::toString(getItemRresult.count())+"x"+EquipTableManager::Get()->getEquipItemByID(getItemRresult.equip().itemid())->name;
						break;
					case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SKILL:
						GetPropManager::Get()->gotSkill(getItemRresult.skill());
						accidentObtain = StringConverter::toString(getItemRresult.count())+"x"+SkillTableManager::Get()->getSkillItemByID(getItemRresult.skill().itemid())->name;
						break;
					case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SOUL:
						GetPropManager::Get()->gotSoul(getItemRresult.soul());
						break;
					case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SKILLBOOK:
						GetPropManager::Get()->gotSkillBook(getItemRresult.skillbook());
						accidentObtain = StringConverter::toString(getItemRresult.count())+"x"+SkillBookTableManager::Get()->getSkillBookItemByID(getItemRresult.skillbook().skillid(),getItemRresult.skillbook().partid())->name;
						break;
					}
					
				}
				accidentObtainList.push_back(accidentObtain);
				showTextList.push_back(showText);
			}

			if(_consumePower<_canAttack)
			{
				mCareerPageAttackInfo.attackCustomsPassInfoMap.find(mCurretFightingTag)->second.customsPassCount += _consumePower;
			}
			else
			{
				mCareerPageAttackInfo.attackCustomsPassInfoMap.find(mCurretFightingTag)->second.customsPassCount += _canAttack;
			}
			
			std::list<std::pair<int, CCBContainer*> >::iterator it = mCustomsPassContainerList.begin();
			for (;it!=mCustomsPassContainerList.end();++it)
			{
				if (it->first == mCurretFightingTag)
				{
					char attackNum[128];
					sprintf(attackNum, "%d%s%d", mCareerPageAttackInfo.attackCustomsPassInfoMap.find(mCurretFightingTag)->second.customsPassCount, "/",CareerConfigManager::Get()->getCustomsPassDataByChaptersIdAndPassId(mCurrChapterIndex,mCurretFightingTag)->limitAttackNum);
					CCB_FUNC(it->second,"mTodayAttack",CCLabelBMFont, setString(attackNum));
				}
			}	
			_refreshTitle();

			mIsQuickCarrerInCool = true;

			CareerQuickResultPage* _careerQuickResultPage = dynamic_cast<CareerQuickResultPage*>(CCBManager::Get()->getPage("CareerQuickResultPage"));
			if (!showTextList.empty()&& _careerQuickResultPage)
			{
				_careerQuickResultPage->setShowTextList(showTextList);
			}
			if (!accidentObtainList.empty() && _careerQuickResultPage)
			{
				_careerQuickResultPage->setAccidentText(accidentObtainList);
			}
			if (_careerQuickResultPage)
			{
				_careerQuickResultPage->setPropertyText(propertyList);
			}
			
			MsgMainFramePushPage msg;
			msg.pageName = "CareerQuickResultPage";
			MessageManager::Get()->sendMessage(&msg);
			showTextList.clear();
			accidentObtainList.clear();
			propertyList.clear();
		}
		else if (msg->stats() == 1)
		{
			MSG_BOX_LAN("@CareerQuickRecoverTime");
			TimeCalculator::Get()->createTimeCalcultor("coldtime", 0);
			ServerDateManager::Get()->getUserBasicInfo().goldcoins = msg->usergolds();
			_refreshTitle();
			mIsQuickCarrerInCool = false;
		}
		else if (msg->stats() == 2)
		{
			MSG_BOX_LAN("@ErrorInfo");
		}
	}
}

void CareerPage::showStory( int chapterID )
{
	BasePage* bpage = CCBManager::Get()->getPage("CareerStoryPage");
	if(bpage)
	{
		CareerStoryPage* page = dynamic_cast<CareerStoryPage*>(bpage);
		if(page)
		{
			page->setChapter(chapterID);

			MsgMainFrameCoverShow msg;
			msg.pageName = "CareerStoryPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}

}


CareerPageAttackInfo::CareerPageAttackInfo( void )
{
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_CAREER_INFORET_S,this);
	backwardsTime = 0;
}

CareerPageAttackInfo::~CareerPageAttackInfo( void )
{
	PacketManager::Get()->removePacketHandler(this);
}

void CareerPageAttackInfo::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	if(opcode == _getOpcode())
	{
		const OPUserCareerInfoRet * msg = dynamic_cast<const OPUserCareerInfoRet*>(packet);
		if(msg)
		{
			currChapterId = msg->currentchapter();
			currCustomsPassId = msg->currentpoint();

			if (msg->has_backwardstime())
			{
				backwardsTime = msg->backwardstime();
			}

			if(currCustomsPassId/100>currChapterId)
			{
				char outstr[128];
				sprintf(outstr,"pass id(%d) is suitable for chapter id(%d)!!",currChapterId,currCustomsPassId);
				CCMessageBox(outstr,"error!");
				currChapterId = currCustomsPassId/100;
			}
			currCustomsPassMaxId;//used for logic

			chapterAddPowerMap.clear();
			attackCustomsPassInfoMap.clear();

			for(int i=0;i<msg->chapteraddpower_size();++i)
			{
				chapterAddPowerMap.insert(std::make_pair(msg->chapteraddpower().Get(i).id(),3));
			}

			
			for(int i=0;i<msg->pointevaluate_size();++i)
			{
				CustomsInfo info;
				info.customsPassEvaluate =msg->pointevaluate(i).value(); 
				info.customsPassCount = 0;
				attackCustomsPassInfoMap.insert(std::make_pair(msg->pointevaluate().Get(i).id(),info));
			}

			for(int i=0;i<msg->pointcount_size();++i)
			{
				int id = msg->pointcount(i).id();	
				int val = msg->pointcount(i).value();	
				std::map<int,CustomsInfo>::iterator it = attackCustomsPassInfoMap.find(id);
				if(it!=attackCustomsPassInfoMap.end())
					it->second.customsPassCount = val;
					
			}
		}
	}
}

void CareerPage::_refreshTitle( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar)
	{
		if(mExpbarInitScale<0)
			mExpbarInitScale = expbar->getScaleX();

		float expscale = 0;
		if(ServerDateManager::Get()->getNextLevelExp()>0)
			expscale = ServerDateManager::Get()->getUserExpPercent();
		if(expscale>1.0f)expscale=1.0f;
		expbar->setScaleX(expscale*mExpbarInitScale);
	}
	CCLabelBMFont* exp = dynamic_cast<CCLabelBMFont* >(getVariable("mExp"));
	if(exp)
	{
		exp->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
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
	CCLabelBMFont* ti = dynamic_cast<CCLabelBMFont* >(getVariable("mTi"));
	if(ti)
	{
		ti->setString((StringConverter::toString(userBasicInfo.power)+"/"+StringConverter::toString(userBasicInfo.maxpower)).c_str());
	}
	CCLabelBMFont* qi = dynamic_cast<CCLabelBMFont* >(getVariable("mQi"));
	if(qi)
	{
		qi->setString((StringConverter::toString(userBasicInfo.vitality)+"/"+StringConverter::toString(userBasicInfo.maxvitality)).c_str());
	}
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(userBasicInfo.level).c_str()));
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{
		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			vipLevel->setVisible(true);
			vipLevel->initWithFile(vipImage.c_str());
		}else
		{
			vipLevel->setVisible(false);
		}
	}
}
 void CareerPage::onTimeout(const int opcode)
{
	if(opcode == _getOpcode())
	{
		if(!BlackBoard::Get()->IsTimeoutResponse)
		{
			BlackBoard::Get()->IsTimeoutResponse = true;
		}
		
	}

 }

 void CareerPage::setChapterButtonStauts()
 {
	 if(mNavContent)
	 {
		 //
		 std::list<std::pair<int,CCBContainer*> >::iterator it=mChapterContainerList.begin();
		 for(;it!=mChapterContainerList.end();it++)
		 {
			 std::pair<int,CCBContainer*> ccbContainerPair=*it;
			 CCBContainer* chapterNavButton =ccbContainerPair.second;
			 //if(ccbContainerPair.first<=mCurrChapterIndex)
			 {
				 CCLabelBMFont* itemName=dynamic_cast<CCLabelBMFont*>(chapterNavButton->getVariable(("mName")));
				 CCMenuItemSprite* itemBtn = dynamic_cast<CCMenuItemSprite*>(chapterNavButton->getVariable("mFrameBack"));

				 if(ccbContainerPair.first==mCurrChapterIndex)
				 {
					 if(itemName)
					 {
						 //itemName->setColor(ccYELLOW);
					 }
					 if (itemBtn)
					 {
						//itemBtn->selected();
						itemBtn->setNormalImage(::getContentSelectedSpirte("CareerNav"));
						itemBtn->setSelectedImage(::getContentSelectedSpirte("CareerNav"));
					 }
					 //
				 }
				 else if (ccbContainerPair.first<=mCareerPageAttackInfo.currChapterId)
				 {
					 if(itemName)
					 {
						 //itemName->setColor(ccWHITE);
					 }
					 if (itemBtn)
					 {
						//itemBtn->unselected();
						itemBtn->setNormalImage(::getContentUnselectedSpirte("CareerNav"));
						itemBtn->setSelectedImage(::getContentUnselectedSpirte("CareerNav"));
					 }
				 }
			 }
			 //else
			 //{
			 //break;
			 //}
		 }
	 }
 }
