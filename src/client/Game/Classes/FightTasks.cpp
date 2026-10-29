
#include "stdafx.h"

#include "FightTasks.h"
#include "FightPage_roundState.h"
#include "FightCardSprite.h"
#include "StringConverter.h"
#include "NumberChangeAction.h"
#include "GameMessages.h"
#include "SoundManager.h"
unsigned long int FightTask::sGeneralVertexZ = 0;

FightTask::FT_State FightTask::update( float dt )
{
	//if(mSlot.battle == 0 || mSlot.buff == 0)
	//if(mSlot.battle == 0)
		//return FT_FAILED;
	
	if(mStarted == false)
	{
		if(mStartTime>0)
		{
			mStartTime-=dt;
			onWaitTick(dt);
			return FT_WAIT;
		}
		if(mStartTime<=0)
		{
			mStarted = true;
			onStart();
			return FT_DOING;
		}
	}
	else
	{
		if(mDurationTime>0)
		{
			onTick(dt);
			mDurationTime-=dt;
			return FT_DOING;
		}
		if(mDurationTime<=0)
		{
			onEnd();
			return FT_DONE;
		}
	}
}

void FightTask::setAttribute( FightPage_roundState::SLOTMEMBER slot, float startTime, int* life)
{
	mSlot = slot;
	mStartTime = startTime;
	mStarted = false;
	mDurationTime = getDuration();
	pLife = life;
	
}

void FightTask::headTraceNode( CCNode* node)
{
	/*
	if(mSlot.headface && node)
	{
		if(mSlot.headface->getParent())
		{
			if(mSlot.headface->getParent() == node)
				return;

			mSlot.headface->removeFromParent();
		}
		node->addChild(mSlot.headface);
	}
	*/
}

void FightTask::_addColorTo( CCBContainer* container, ccColor4F c4f, float dt ,float delay, float restoreTime)
{
	CCNode* node = 0;
	CCNode* node2 = 0;
	CCB_FUNC_RETURN(container,"mOwn1",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_BG),node);
	CCB_FUNC_RETURN(container,"mPoster",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_HEAD),node2);
	if(node && node2)
	{
		FightCardSprite* sprite = dynamic_cast<FightCardSprite*>(node);
		FightCardSprite* sprite2 = dynamic_cast<FightCardSprite*>(node2);

		if(sprite) sprite->setAddColorTo(c4f,dt,delay,restoreTime);
		if(sprite2)	sprite2->setAddColorTo(c4f,dt,delay,restoreTime);
		
	}
}

void FightTask::_damageFlash( CCBContainer* container, float delay , float  duration)
{
	CCNode* node = 0;
	CCNode* node2 = 0;
	CCB_FUNC_RETURN(container,"mOwn1",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_BG),node);
	CCB_FUNC_RETURN(container,"mPoster",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_HEAD),node2);
	if(node && node2)
	{
		FightCardSprite* sprite = dynamic_cast<FightCardSprite*>(node);
		FightCardSprite* sprite2 = dynamic_cast<FightCardSprite*>(node2);

		if(sprite) sprite->setDamageFlash(delay,duration);
		if(sprite2)	sprite2->setDamageFlash(delay,duration);

	}
}

FightTask::FightTask()
{
	//taskcount++;
	//CCLOG("task count:%d",taskcount);
}

FightTask::~FightTask()
{
	//taskcount--;
}


unsigned long FightTask::taskcount = 0;




void FightTaskEnter::onStart()
{
	mSlot.battle->setVisible(false);
	/*
	if(mSlot.headface)mSlot.headface->setVisible(true);
	if(mSlot.buff && mSlot.buff->getLoaded())
	{
		mSlot.buff->setVisible(true);
		mSlot.buff->runAnimation("beginning");
		CCObject* headObj = mSlot.buff->getVariable("mAssailant");
		CCNode * head = headObj?dynamic_cast<CCNode*>(headObj):0;
		headTraceNode(head);
	}
	*/
}

void FightTaskEntrance::onStart()
{
	CCActionInterval* move = CCMoveTo::create(getDuration(), CCPointMake(mPoint.x,mPoint.y));
	mSlot.battle->runAction(CCSpawn::create(CCEaseExponentialOut::create((CCActionInterval*)(move->copy()->autorelease())),NULL));

	if(!mSoundPlayed)
	{
		SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("entrance"));
		mSoundPlayed = true;
	}
}

void FightTaskEntrance::onEnd()
{
	mSoundPlayed = false;
}


bool FightTaskEntrance::mSoundPlayed = false;

void FightTaskQueue::onStart()
{
	CCActionInterval* move = CCMoveTo::create(getDuration(), CCPointMake(mPoint.x,mPoint.y));
	mSlot.battle->runAction(CCSpawn::create(CCEaseExponentialInOut::create((CCActionInterval*)(move->copy()->autorelease())),NULL));
	if(!mSoundPlayed)
	{
		SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("queue"));
		mSoundPlayed = true;
	}
}

void FightTaskQueue::onEnd()
{
	mSoundPlayed = false;
}

bool FightTaskQueue::mSoundPlayed = false;



void FightTaskBuff::onStart()
{
	//mSlot.battle->setVisible(false);
	

	if(mSlot.battle->getLoaded())
	{
		BasePage * bpage = CCBManager::Get()->getPage("FightPage");
		if(bpage)
		{
			if(mBuffCount==0)
				bpage->runAnimation("s_buff_start");

			mSlot.battle->retain();
			mSlot.battle->removeFromParent();
			CCB_FUNC(bpage,"mAttackSmallskill",CCNode,addChild(mSlot.battle));
			mSlot.battle->release();
		}
	}
	mBuffCount++;

	if(mSlot.battle->getLoaded())
	{
		if(mSkillItem&&mSkillItem->soundUrl!="none")
		{
			SoundManager::Get()->playEffect(mSkillItem->soundUrl);
		}
		if(type == ATTACK)
		{
			ccColor4F color = {0,0.79f,0.39f,0};
			color.r = GET_FLASH_TIME(CT_BUFF_ATTACK,CA_COLOR_R);
			color.g = GET_FLASH_TIME(CT_BUFF_ATTACK,CA_COLOR_G);
			color.b = GET_FLASH_TIME(CT_BUFF_ATTACK,CA_COLOR_B);
			float delay = 0,duration = 0.3f,resotre = 0.3f;
			delay = GET_FLASH_TIME(CT_BUFF_ATTACK,CA_GLOW_DELAY);
			duration = GET_FLASH_TIME(CT_BUFF_ATTACK,CA_GLOW_DURATION);
			resotre = GET_FLASH_TIME(CT_BUFF_ATTACK,CA_GLOW_RESTORE);
			_addColorTo(mSlot.battle,color,duration,delay,resotre);
			mSlot.battle->runAnimation("buffa");
		}
		else if(type == DEFENSE)
		{
			ccColor4F color = {0,0.18f,0.60f,0};
			color.r = GET_FLASH_TIME(CT_BUFF_DEFENSE,CA_COLOR_R);
			color.g = GET_FLASH_TIME(CT_BUFF_DEFENSE,CA_COLOR_G);
			color.b = GET_FLASH_TIME(CT_BUFF_DEFENSE,CA_COLOR_B);
			float delay = 0,duration = 0.3f,resotre = 0.3f;
			delay = GET_FLASH_TIME(CT_BUFF_DEFENSE,CA_GLOW_DELAY);
			duration = GET_FLASH_TIME(CT_BUFF_DEFENSE,CA_GLOW_DURATION);
			resotre = GET_FLASH_TIME(CT_BUFF_DEFENSE,CA_GLOW_RESTORE);
			_addColorTo(mSlot.battle,color,duration,delay,resotre);
			mSlot.battle->runAnimation("buffb");
		}
		else if(type == WILL)
		{
			ccColor4F color = {0.36f,0.36f,0.0f,0};
			color.r = GET_FLASH_TIME(CT_BUFF_WILL,CA_COLOR_R);
			color.g = GET_FLASH_TIME(CT_BUFF_WILL,CA_COLOR_G);
			color.b = GET_FLASH_TIME(CT_BUFF_WILL,CA_COLOR_B);
			float delay = 0,duration = 0.3f,resotre = 0.3f;
			delay = GET_FLASH_TIME(CT_BUFF_WILL,CA_GLOW_DELAY);
			duration = GET_FLASH_TIME(CT_BUFF_WILL,CA_GLOW_DURATION);
			resotre = GET_FLASH_TIME(CT_BUFF_WILL,CA_GLOW_RESTORE);
			_addColorTo(mSlot.battle,color,duration,delay,resotre);
			mSlot.battle->runAnimation("buffc");
		}
		/*	
		if(type == ATTACK)
		mSlot.battle->runAnimation("buff_attack");
		else if(type == DEFENSE)
		mSlot.battle->runAnimation("buff_defense");
		else if(type == WILL)
		mSlot.battle->runAnimation("buff_will");
		CCObject* headObj = mSlot.buff->getVariable("mAssailant");
		CCNode * head = headObj?dynamic_cast<CCNode*>(headObj):0;
		headTraceNode(head);
		*/
	}
	
}

void FightTaskBuff::onEnd()
{
	mBuffCount--;
	BasePage * bpage = CCBManager::Get()->getPage("FightPage");
	if(bpage && mBuffCount==0)
	{
		bpage->runAnimation("s_buff_end");
	}
	


	mSlot.battle->retain();
	mSlot.battle->removeFromParent();
	BasePage* page = CCBManager::Get()->getPage("FightPage");
	CCB_FUNC(page,"mPeople",CCNode,addChild(mSlot.battle));
	mSlot.battle->release();

}

int FightTaskBuff::mBuffCount = 0;

void FightTaskAttack::onStart()
{
	if((*pLife)<=0&&(*pLife)!=-1)return;
	//mSlot.buff->setVisible(false);
	mSlot.battle->setVisible(true);
	if(mSlot.battle->getLoaded())
	{
		SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("FightNormalAttack"));
		mSlot.battle->runAnimation("attack");
		mSlot.battle->setZOrder((float)(++sGeneralVertexZ));
		/*
		CCObject* headObj = mSlot.battle->getVariable("mAssailant");
		CCNode * head = headObj?dynamic_cast<CCNode*>(headObj):0;
		headTraceNode(head);
		*/
	}
}

void FightTaskDefense::onStart()
{
	//mSlot.buff->setVisible(false);
	mSlot.battle->setVisible(true);
	

	if(mSlot.battle->getLoaded())
	{
		if((*pLife)==-1)
		{
			mSlot.battle->runAnimation("defensea");
			CCObject* hurtObj = mSlot.battle->getVariable("mHurt");
			CCLabelBMFont * hurt = hurtObj?dynamic_cast<CCLabelBMFont*>(hurtObj):0;
			if(hurt)
			{
				char hurtStr[64];
				sprintf(hurtStr,"-%d",mDamage);
				hurt->setString(hurtStr);
			}
			float delay =0.6f, duration = 0.1f;
			delay = GET_FLASH_TIME(CT_NORMAL_ATTACK,CA_FLASH_DELAY);
			duration = GET_FLASH_TIME(CT_NORMAL_ATTACK,CA_FLASH_DURATION);
			_damageFlash(mSlot.battle,delay,duration);
		}
		else
		{
			if((*pLife)>0)
			{
			(*pLife)-=mDamage;
			if((*pLife)>0)
			{
				mSlot.battle->runAnimation("defensea");
				
			}
			else
			{
				(*pLife) = 0;
				mSlot.battle->runAnimation("dead");
			}
			
			CCObject* hurtObj = mSlot.battle->getVariable("mHurt");
			CCLabelBMFont * hurt = hurtObj?dynamic_cast<CCLabelBMFont*>(hurtObj):0;
			if(hurt)
			{
				char hurtStr[64];
				sprintf(hurtStr,"-%d",mDamage);
				hurt->setString(hurtStr);
			}

			/*
			CCObject* headObj = mSlot.battle->getVariable("mDefender");
			CCNode * head = headObj?dynamic_cast<CCNode*>(headObj):0;
			headTraceNode(head);
			*/
			float delay =0.6f, duration = 0.1f;
			delay = GET_FLASH_TIME(CT_NORMAL_ATTACK,CA_FLASH_DELAY);
			duration = GET_FLASH_TIME(CT_NORMAL_ATTACK,CA_FLASH_DURATION);
			_damageFlash(mSlot.battle,delay,duration);
			
			}
		}
		
		
	}
}


void FightTaskDefense::onEnd()
{
	if((*pLife)!=-1)
	{
		float lifePersentage = 0;
		float dt = GET_FLASH_TIME(CT_NORMAL_ATTACK,CA_BLEEDING);
		if(mSlot.maxlife>0)
		{
			lifePersentage =(*pLife)/((float)mSlot.maxlife);
			CCActionInterval* numberto = NumberChangeAction::create(dt,(*pLife));
			CCB_FUNC(mSlot.battle,"mBlood",CCLabelBMFont,runAction(numberto));
		}
		CCNode* node = 0;
		CCNode* node2 = 0;
		CCB_FUNC_RETURN(mSlot.battle,"mOwn1",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_BG),node);
		CCB_FUNC_RETURN(mSlot.battle,"mPoster",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_HEAD),node2);
		if(node && node2)
		{
			FightCardSprite* sprite = dynamic_cast<FightCardSprite*>(node);
			if(sprite) sprite->setLifeTo(lifePersentage,dt);

			FightCardSprite* sprite2 = dynamic_cast<FightCardSprite*>(node2);
			if(sprite2)	sprite2->setLifeTo(lifePersentage,dt);
		}
	}
	
}

void FightTaskRoundOver::onStart()
{
	if(mPage)
	{
		CCObject* obj = mPage->getVariable("mRound");
		if(!obj)return;
		CCNode* node = dynamic_cast<CCNode*>(obj);
		if(!node)return;
		CCB_FUNC(mPage,"mRound",CCNode,setVisible(true));
		if(node->getChildrenCount()>0)
		{
			CCB_FUNC(mPage,"mRound",CCNode,setVisible(true));
			CCObject* obj2 = node->getChildren()->objectAtIndex(0);
			CCBContainer* finalPage = dynamic_cast<CCBContainer*>(obj2);

			if(finalPage)
			{
				if(mRound == 1)
					finalPage->runAnimation("round1end");
				else if(mRound == 2)
					finalPage->runAnimation("round2end");
				else if(mRound == 3)
					finalPage->runAnimation("round3end");
			}
		}
	}
}

void FightTaskRoundOver::onEnd()
{
	CCB_FUNC(mPage,"mRound",CCNode,setVisible(false));
}

FightTaskRoundOver::FightTaskRoundOver( int round, CCBContainer* page ) :mRound(round),mPage(page)
{

}

FightTaskRoundStart::FightTaskRoundStart( int round, CCBContainer* page ) :mRound(round),mPage(page)
{
	mLoadMessageSent = false;

	CCObject* obj = page->getVariable("mRound");
	if(!obj)return;
	CCNode* node = dynamic_cast<CCNode*>(obj);
	if(!node)return;

	bool createNew = true;
 	if(node->getChildrenCount()!=0)
 	{
 		CCObject* obj2 = node->getChildren()->objectAtIndex(0);
 		CCBContainer* finalPage = dynamic_cast<CCBContainer*>(obj2);
 
 		if(finalPage)
 		{
 			createNew = false;
 		}
 	}

	if(createNew)
	{
		CCBContainer*  _round= CCBContainer::create();
		_round->loadCcbiFile("s_curtain.ccbi");
		CCB_FUNC(mPage,"mRound",CCNode,addChild(_round));
	}
	
}

void FightTaskRoundStart::onStart()
{
	if(mPage)
	{
		SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("startTitle"));

		CCObject* obj = mPage->getVariable("mRound");
		if(!obj)return;
		CCNode* node = dynamic_cast<CCNode*>(obj);
		if(!node)return;
		if(node->getChildrenCount()>0)
		{
			CCB_FUNC(mPage,"mRound",CCNode,setVisible(true));
			CCObject* obj2 = node->getChildren()->objectAtIndex(0);
			CCBContainer* finalPage = dynamic_cast<CCBContainer*>(obj2);

			if(finalPage)
			{
				if(mRound == 1)
					finalPage->runAnimation("round1start");
				else if(mRound == 2)
					finalPage->runAnimation("round2start");
				else if(mRound == 3)
					finalPage->runAnimation("round3start");
			}
		}
	}
}

void FightTaskRoundStart::onEnd()
{
	CCB_FUNC(mPage,"mRound",CCNode,setVisible(false));
}

void FightTaskRoundStart::onTick( float dt )
{
	float loadtime = FightConfigManager::Get()->getRoundStartLoadTime();
	if(!mLoadMessageSent && getDuration()-mDurationTime>loadtime)
	{
		mLoadMessageSent = true;
		MsgFightRoundLoadSkill msg;
		MessageManager::Get()->sendMessage(&msg);
	}
}

void FightTaskShake::onStart()
{
	if(mPage)
	{
// 		if(mType == TYPE_1)
// 			mPage->runAnimation("shake1");
// 		else
// 			mPage->runAnimation("shake2");
	}
}
