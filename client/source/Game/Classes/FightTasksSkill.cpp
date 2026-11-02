
#include "stdafx.h"


#include "FightTasksSkill.h"
#include "NumberChangeAction.h"
#include "FightCardSprite.h"
#include "StringConverter.h"
#include "SoundManager.h"
void FightTaskSkill::onStart()
{
	return;//temp
	if(!mSkill)
		return;
	if((*pLife)<=0)return;
	
	replaceNode();

}


void FightTaskSkill::onEnd()
{
		
}



FightTaskSkill::FightTaskSkill( int skillID , float finishTime, int positionID)
{
    mDurationTime = 0;
	mLastFinishTime = finishTime;
	mSkillID = skillID;
	mSkill = 0;
	mPositionID = positionID;
}

void FightTaskSkill::setAttribute( FightPage_roundState::SLOTMEMBER slot,float startTime ,int * life)
{
	mSlot = slot;
	mStarted = false;
	pLife = life;

	mSkill = SkillTableManager::Get()->getSkillItemByID(mSkillID);
	if(mSkill)
	{
		mStartTime = mLastFinishTime;
	}
	else
	{
		char log[128];
		sprintf(log,"Failed to get skill! id:%d",mSkillID);
		CCMessageBox(log,"error");
	}
}

void FightTaskSkill::initReplace( CCBContainer* tobeReplace )
{
	SkillItem::REPLACE_SPRITE_MAP::const_iterator it = mSkill->mReplaceMap.begin();
	for(;it!=mSkill->mReplaceMap.end();++it)
	{
		do 
		{
			SkillRep rep;

			CCObject *obj = tobeReplace->getVariable(it->first);
			if(!obj)break;
			CCSprite* sp = dynamic_cast<CCSprite*>(obj);
			if(!sp)break;

			rep.variable = sp;
			sp->retain();

			rep.filename = it->second.first;
			mReplaceVaiables.push_back(rep);
		} while (false);


	}
}

void FightTaskSkill::replaceNode()
{
	while(!mReplaceVaiables.empty())
	{
		SkillRep* repIt = &(mReplaceVaiables.front());
		if(repIt->variable)
		{
			repIt->variable->setTexture(repIt->filename.c_str());
			repIt->variable->release();
		}
		mReplaceVaiables.pop_front();
	}
}
//----------------------------------------------------------------------------------------------------------------

void FightTaskSkillSmall::onTick( float dt )
{
	//()()
	float eclipsedTime = mDuration - mDurationTime;
	if(!mDamaged &&  eclipsedTime > getDamageTime())
	{
		mDamaged = true;
		onDamaged();

		ccColor4F color = {0,0,0,0};
		_addColorTo(mSlot.battle,color,0.2f);
	}
	if(eclipsedTime>0.3f && !mFlashed)
	{
		mFlashed = true;
		ccColor4F color = {0.61f,0.26f,0,0};
		color.r = GET_FLASH_TIME(CT_SMALL_SKILL_PER2,CA_COLOR_R);
		color.g = GET_FLASH_TIME(CT_SMALL_SKILL_PER2,CA_COLOR_G);
		color.b = GET_FLASH_TIME(CT_SMALL_SKILL_PER2,CA_COLOR_B);
		float duration = GET_FLASH_TIME(CT_SMALL_SKILL_PER2,CA_GLOW_DURATION);
		_addColorTo(mSlot.battle,color,duration);
	}

}

void FightTaskSkillSmall::setAttribute( FightPage_roundState::SLOTMEMBER slot,float startTime, int*life )
{
	FightTaskSkill::setAttribute(slot,startTime,life);
	mDuration = FightConfigManager::Get()->getSkillTime(FightConfigManager::SMALL,FightConfigManager::DURATION);//1.4f;
	mDurationTime = mDuration;
	mNextDelay = FightConfigManager::Get()->getSkillTime(FightConfigManager::SMALL,FightConfigManager::NEXT_DELAY);//1.4f;
	mStartTime = startTime;
	mDamaged=false;
	initReplace(mSlot.battle);

	mFlashed = false;
}

void FightTaskSkillSmall::onStart()
{
	FightTaskSkill::onStart();
	
	if(mSlot.battle->getLoaded())
	{
		BasePage * bpage = CCBManager::Get()->getPage("FightPage");
		if(bpage)
		{
			if(mSkill&&mSkill->soundUrl!="none")
			{
				mSoundEffectHandle = SoundManager::Get()->playEffect(mSkill->soundUrl);
			}
			mSlot.battle->retain();
			mSlot.battle->removeFromParent();
			CCB_FUNC(bpage,"mAttackSmallskill",CCNode,addChild(mSlot.battle));
			mSlot.battle->release();
			//mShadow = CCBContainer::create();
			//mShadow->loadCcbiFile("battlePage.ccbi");
			//mShadow->retain();

			
			//CCB_FUNC(bpage,"mShadow",CCNode,addChild(mShadow));
			bpage->runAnimation("s_skilla");
			ENEMY_MAP::iterator itStart = mEnemys.begin();
			int enemyCount = mEnemys.size();
			if(itStart!=mEnemys.end() && enemyCount>0)
			{
				ENEMY_MAP::iterator it = itStart;
				do 
				{
					it->second.battle->retain();
					it->second.battle->removeFromParent();
					CCB_FUNC(bpage,"mDefense",CCNode,addChild(it->second.battle));
					it->second.battle->release();
					char hurtStr[64];
					
					if(mDamage.find(it->first)!=mDamage.end())
					{
						sprintf(hurtStr,"-%d",mDamage.find(it->first)->second);
					}
					else
					{
						sprintf(hurtStr,"-1");
					}
					CCB_FUNC(it->second.battle,"mHurt",CCLabelBMFont,setString(hurtStr));
					it++;
					if(it == mEnemys.end())
						it = mEnemys.begin();
				} while (itStart!=it);
			}
		}
		mSlot.battle->runAnimation("s_skilla");
		
		ccColor4F color = {0.68f,0.71f,0.33f,0};
		color.r = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_COLOR_R);
		color.g = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_COLOR_G);
		color.b = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_COLOR_B);
		float duration = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_GLOW_DURATION);
		_addColorTo(mSlot.battle,color,duration);

		ENEMY_MAP::iterator it = mEnemys.begin();
		for(;it!=mEnemys.end();++it)
		{
			unsigned int _damage=mDamage.find(it->first)->second;
			if(*it->second.plife!=-1)
			{
				(*it->second.plife)-=_damage;
				if((*it->second.plife)<=0)
				{
					(*it->second.plife)=0;
				}
				it->second.life = (*it->second.plife);
				it->second.battle->runAnimation((it->second.life==0?"deadb":"defenseb"));
				float df_delay = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_FLASH_DELAY);
				float df_duration = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_FLASH_DURATION);
				_damageFlash(it->second.battle,df_delay,df_duration);
			}
			else
			{
			    it->second.battle->runAnimation("defenseb");
			}
		}
	}
}

void FightTaskSkillSmall::onEnd()
{
	mSlot.battle->retain();
	mSlot.battle->removeFromParent();
	BasePage* page = CCBManager::Get()->getPage("FightPage");
	CCB_FUNC(page,"mPeople",CCNode,addChild(mSlot.battle));
	mSlot.battle->release();
	ENEMY_MAP::iterator it = mEnemys.begin();
	for(;it!=mEnemys.end();++it)
	{
		it->second.battle->retain();
		it->second.battle->removeFromParent();
		CCB_FUNC(page,"mPeople",CCNode,addChild(it->second.battle));
		it->second.battle->release();
	}
	mDamage.clear();
}

void FightTaskSkillSmall::addEnemy( FightPage_roundState::SLOTMEMBER slot,unsigned int _damage )
{
	int index = slot.position;
	if(mEnemys.find(index) == mEnemys.end())
	{
		mEnemys.insert(std::make_pair(index,slot));
		mDamage.insert(std::make_pair(index,_damage));
	}
}

void FightTaskSkillSmall::onDamaged()
{
	ENEMY_MAP::iterator it = mEnemys.begin();
	for(;it!=mEnemys.end();++it)
	{
		if((*it->second.plife)==-1)
		{
			continue;
		}
		float lifePersentage = 0;
		float dt = GET_FLASH_TIME(CT_SMALL_SKILL_PER1,CA_BLEEDING);
		lifePersentage =(float)it->second.life/((float)it->second.maxlife);
		CCActionInterval* numberto = NumberChangeAction::create(dt,it->second.life);
		CCB_FUNC(it->second.battle,"mBlood",CCLabelBMFont,runAction(numberto));
		CCNode* node = 0;
		CCNode* node2 = 0;
		CCB_FUNC_RETURN(it->second.battle,"mOwn1",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_BG),node);
		CCB_FUNC_RETURN(it->second.battle,"mPoster",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_HEAD),node2);
		if(node && node2)
		{
			FightCardSprite* sprite = dynamic_cast<FightCardSprite*>(node);
			if(sprite) sprite->setLifeTo(lifePersentage,dt);

			FightCardSprite* sprite2 = dynamic_cast<FightCardSprite*>(node2);
			if(sprite2)	sprite2->setLifeTo(lifePersentage,dt);
		}

	}
}

float FightTaskSkillSmall::getDamageTime()
{
	float dam = FightConfigManager::Get()->getSkillTime(FightConfigManager::SMALL,FightConfigManager::DAMAGE_TIME);
	return dam;//0.0f;
}


//----------------------------------------------------------------------------------------------------------------
void FightTaskSkillBig::setAttribute( FightPage_roundState::SLOTMEMBER slot,float startTime, int*life )
{
	if(mSkillNode)
		mSkillNode->release();
	mSkillNode = 0;
	FightTaskSkill::setAttribute(slot,startTime,life);
	if(mSkill->type == SkillItem::TYPE_MULTIPLE)
	{
		mNextDelay = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::NEXT_DELAY);//3.0f;//mLastFinishTime - startTime + mDuration;
		mDuration = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::DURATION);//3.0f;//mSkill->duration;
	}
	else
	{
		mNextDelay = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::NEXT_DELAY);//2.0f;
		mDuration = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::DURATION);//2.0f;
	}
	mDurationTime = mDuration;

}

void FightTaskSkillBig::onStart()
{
	mPreparedEnd = false;
	mDamaged = false;
	if(!mSkillNode)
		onLoad();
	if(mSkill&&mSkill->soundUrl!="none")
	{
		mSoundEffectHandle = SoundManager::Get()->playEffect(mSkill->soundUrl);
	}
	if(mAttackDiscipleItem&&mAttackDiscipleItem->soundUrl!="none")
	{
		mSoundEffectHandle = SoundManager::Get()->playEffect(mAttackDiscipleItem->soundUrl);
	}
}

void FightTaskSkillBig::onEnd()
{
	if(mSkillNode)
	{
		mSkillNode->removeFromParentAndCleanup(true);
		mSkillNode->release();
		mSkillNode = 0;
	}
}

void FightTaskSkillBig::onTick( float dt )
{
	if(!mPreparedEnd && mDuration - mDurationTime > getPreparedEndTime())
	{
		mPreparedEnd = true;
		onPrepareEnd();
	}
	if(!mDamaged &&  mDuration - mDurationTime > getDamageTime())
	{
		mDamaged = true;
		onDamaged();
	}
}

void FightTaskSkillBig::onLoad()
{
	if(mSkillNode)return;
	BasePage * bpage = CCBManager::Get()->getPage("FightPage");
	if(bpage)
	{

		mSkillNode = CCBContainer::create();
		mSkillNode->loadCcbiFile(mSkill->ccbFile);
		mSkillNode->retain();
		mSkillNode->setVisible(false);

		const DiscipleItem* item =  DiscipleTableManager::Get()->getDiscipleItemByID(mSlot.resourceID);
		if(item)
		{
			std::string headname = item->bodyPic;
			if(headname == "none")
				headname = "person/poster_char_xiangjishi.png";

			CCB_FUNC(mSkillNode,"mAssailantHead1",CCSprite,setTexture(headname.c_str()));
			CCB_FUNC(mSkillNode,"mAssailantHead2",CCSprite,setTexture(headname.c_str()));
			CCB_FUNC(mSkillNode,"mAssailantPoster",CCSprite,setTexture(headname.c_str()));
		}

		CCB_FUNC(mSkillNode,"mSkillNameTex",CCLabelBMFont,setString(mSkill->name.c_str()));
		CCB_FUNC(bpage,"mBigskill",CCNode,addChild(mSkillNode));


		initReplace(mSkillNode);
		replaceNode();
	}
}

FightTaskSkillBig::~FightTaskSkillBig()
{
	onEnd();
}

//-------------------------------------------------------------------------------------
void FightTaskSkillBigSingle::setEnemy( FightPage_roundState::SLOTMEMBER slot ,unsigned int _damage, int*life)
{
	mEnemy = slot;
	mDamage=_damage;
	mLife = life;
}

void FightTaskSkillBigSingle::onStart()
{
	FightTaskSkillBig::onStart();
	mSkillNode->setVisible(true);
	mSkillNode->runAnimation(mSkill->timeLine);
	if(mPositionID%FightPage::TEAM_COUNT == FightPage::TEAM_ENEMY)
	{
		CCB_FUNC(mSkillNode,"mTurn1",CCNode,setRotation(180.0f));
		CCB_FUNC(mSkillNode,"mTurn2",CCNode,setRotation(180.0f));
		CCB_FUNC(mSkillNode,"mTurn3",CCNode,setRotation(180.0f));
		CCB_FUNC(mSkillNode,"mAssailantHead1",CCNode,setRotation(180.0f));
		CCB_FUNC(mSkillNode,"mAssailantHead2",CCNode,setRotation(180.0f));
		CCB_FUNC(mSkillNode,"mAssailantPoster",CCNode,setRotation(180.0f));
	}
	mSlot.battle->retain();
	mSlot.battle->removeFromParent();
	CCB_FUNC(mSkillNode,"mAssailant",CCNode,addChild(mSlot.battle));
	mSlot.battle->release();

	CCPoint selfPos = FightConfigManager::Get()->getSkillBigSingleSelfPosition();
	CCPoint enemyPos = FightConfigManager::Get()->getSkillBigSingleEnemyfPosition();
	float moveDur = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::ENTER_DURATION);

	mEnemy.battle->retain();
	mEnemy.battle->removeFromParent();
	CCB_FUNC(mSkillNode,"mDefender",CCNode,addChild(mEnemy.battle));
	mEnemy.battle->release();

	CCMoveTo *actSelf = CCMoveTo::create(moveDur,selfPos);
	CCMoveTo *actEnemy = CCMoveTo::create(moveDur,enemyPos);
	//modify by dylan at 20130717 
	//std::string timeline = mSkill->ccbFile.substr(0,mSkill->ccbFile.find_first_of('.'));
	std::string timeline = mSkill->type==1?"s_slsa":"s_slga";
	if((*mLife)!=-1)
	{
		(*mLife)-=mDamage;
		if((*mLife)<=0)
		{
			(*mLife)=0;
		}
		mEnemy.life = *mLife;
	}
	

	if(mSlot.position%FightPage::TEAM_COUNT == FightPage::TEAM_SELF)
	{
		mSlot.battle->runAnimation(timeline);
		mSlot.battle->runAction(actSelf);

		mEnemy.battle->runAnimation((mEnemy.life==0?"deadd":"defensed"));
		mEnemy.battle->runAction(actEnemy);
		
	}
	else
	{
		mSlot.battle->runAnimation(timeline);
		mSlot.battle->runAction(actEnemy);

		mEnemy.battle->runAnimation((mEnemy.life==0?"deadd":"defensed"));
		mEnemy.battle->runAction(actSelf);
	}
	float df_delay = GET_FLASH_TIME(CT_BIG_SINGLE,CA_FLASH_DELAY);
	float df_duration = GET_FLASH_TIME(CT_BIG_SINGLE,CA_FLASH_DURATION);
	_damageFlash(mEnemy.battle,df_delay,df_duration);

	ccColor4F color = {0.82f,0.51f,0,0};
	color.r = GET_FLASH_TIME(CT_BIG_SINGLE,CA_COLOR_R);
	color.g = GET_FLASH_TIME(CT_BIG_SINGLE,CA_COLOR_G);
	color.b = GET_FLASH_TIME(CT_BIG_SINGLE,CA_COLOR_B);
	float delay = 1.9f,duration = 1.0f,resotre = 0.1f;
	delay = GET_FLASH_TIME(CT_BIG_SINGLE,CA_GLOW_DELAY);
	duration = GET_FLASH_TIME(CT_BIG_SINGLE,CA_GLOW_DURATION);
	resotre = GET_FLASH_TIME(CT_BIG_SINGLE,CA_GLOW_RESTORE);
	_addColorTo(mSlot.battle,color,duration,delay,resotre);
}

float FightTaskSkillBigSingle::getPreparedEndTime()
{
	float moveDur = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::EXIT_START);
	return moveDur;//5.7f;
}

void FightTaskSkillBigSingle::onPrepareEnd()
{
	float moveDur = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::EXIT_DURATION);

	mSlot.battle->retain();
	mSlot.battle->removeFromParent();
	BasePage* page = CCBManager::Get()->getPage("FightPage");
	CCB_FUNC(page,"mPeople",CCNode,addChild(mSlot.battle));
	mSlot.battle->release();

	CCMoveTo *act = CCMoveTo::create(moveDur,mSlot.mStopPoint);
	mSlot.battle->runAction(act);

	mEnemy.battle->retain();
	mEnemy.battle->removeFromParent();
	CCB_FUNC(page,"mPeople",CCNode,addChild(mEnemy.battle));
	mEnemy.battle->release();

	CCMoveTo *actEnemy = CCMoveTo::create(moveDur,mEnemy.mStopPoint);
	mEnemy.battle->runAction(actEnemy);

}

void FightTaskSkillBigSingle::onDamaged()
{
	if(mEnemy.life!=-1)
	{
		float lifePersentage = 0;
		float dt = GET_FLASH_TIME(CT_BIG_SINGLE,CA_BLEEDING);
		lifePersentage = (float)mEnemy.life/((float)mEnemy.maxlife);
		CCActionInterval* numberto = NumberChangeAction::create(dt,mEnemy.life);
		CCB_FUNC(mEnemy.battle,"mBlood",CCLabelBMFont,runAction(numberto));
		CCNode* node = 0;
		CCNode* node2 = 0;
		CCB_FUNC_RETURN(mEnemy.battle,"mOwn1",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_BG),node);
		CCB_FUNC_RETURN(mEnemy.battle,"mPoster",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_HEAD),node2);
		if(node && node2)
		{
			FightCardSprite* sprite = dynamic_cast<FightCardSprite*>(node);
			if(sprite) sprite->setLifeTo(lifePersentage,dt);

			FightCardSprite* sprite2 = dynamic_cast<FightCardSprite*>(node2);
			if(sprite2)	sprite2->setLifeTo(lifePersentage,dt);

		}
	}
	char hurtStr[64];
	sprintf(hurtStr,"-%d",mDamage);
	CCB_FUNC(mEnemy.battle,"mHurt",CCLabelBMFont,setString(hurtStr));
}

float FightTaskSkillBigSingle::getDamageTime()
{
	float dam = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::DAMAGE_TIME);
	return dam;//0.0f;
}

//------------------------------------------------------------------------------------------
void FightTaskSkillBigMultiple::onStart()
{
	FightTaskSkillBig::onStart();

	mSkillNode->setVisible(true);
	mSkillNode->runAnimation(mSkill->timeLine);

	mSlot.battle->retain();
	mSlot.battle->removeFromParent();
	CCB_FUNC(mSkillNode,"mAssailant",CCNode,addChild(mSlot.battle));
	mSlot.battle->release();
	//modify by dylan at 20130717 
	//std::string timeline = mSkill->ccbFile.substr(0,mSkill->ccbFile.find_first_of('.'));
	std::string timeline = mSkill->type==1?"s_slsa":"s_slga";
	mSlot.battle->runAnimation(timeline);

	CCPoint attackerPos = FightConfigManager::Get()->getSkillBigMulitpleAttackerPosition();
	float defenderDis = FightConfigManager::Get()->getSkillBigMulitpleDefenderDistance();
	float moveDur = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::ENTER_DURATION);

	CCMoveTo *act = CCMoveTo::create(moveDur,attackerPos);
	mSlot.battle->runAction(act);

	int oppoPosition = (mPositionID%FightPage::TEAM_COUNT==0)?mPositionID+1:mPositionID-1;
	ENEMY_MAP::iterator itStart = mEnemys.find( oppoPosition );
	int enemyCount = mEnemys.size();
	if(itStart!=mEnemys.end() && enemyCount>0)
	{
		float dis = defenderDis;
		float angle = 0;
		
		ENEMY_MAP::iterator it = itStart;
		do 
		{
			float x = attackerPos.x + dis*sin(angle);
			float y = attackerPos.y + dis*cos(angle);
			angle += 3.14f*2.0f / (float)enemyCount;

			it->second.battle->retain();
			it->second.battle->removeFromParent();
			CCB_FUNC(mSkillNode,"mDefender",CCNode,addChild(it->second.battle));
			it->second.battle->release();

			CCMoveTo *actEnemy = CCMoveTo::create(moveDur,ccp(x,y));
			it->second.battle->runAction(actEnemy);
			char hurtStr[64];
			sprintf(hurtStr,"-%d",mDamage.find(it->first)->second);
			CCB_FUNC(it->second.battle,"mHurt",CCLabelBMFont,setString(hurtStr));

			LIFE_MAP::iterator lifeIt = mLifes.find(it->first);
			if(lifeIt!=mLifes.end())
			{
				if(*(lifeIt->second)!=-1)
				{
					unsigned int _damage=mDamage.find(it->first)->second;
					*(lifeIt->second)-=_damage;
					if(*(lifeIt->second)<=0)
					{
						*(lifeIt->second) =0;
					}
					it->second.life = *(lifeIt->second);
				}
			}

			it->second.battle->runAnimation((it->second.life==0?"deadc":"defensec"));
			float df_delay = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_FLASH_DELAY);
			float df_duration = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_FLASH_DURATION);
			_damageFlash(it->second.battle,df_delay,df_duration);


			it++;
			if(it == mEnemys.end())
				it = mEnemys.begin();
		} while (itStart!=it);
	}

	ccColor4F color = {0.82f,0.51f,0,0};
	color.r = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_COLOR_R);
	color.g = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_COLOR_G);
	color.b = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_COLOR_B);
	float delay = 1.9f,duration = 1.0f,resotre = 0.1f;
	delay = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_GLOW_DELAY);
	duration = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_GLOW_DURATION);
	resotre = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_GLOW_RESTORE);
	_addColorTo(mSlot.battle,color,duration,delay,resotre);

}


void FightTaskSkillBigMultiple::addEnemy( FightPage_roundState::SLOTMEMBER slot,unsigned int _damage, int* life)
{
	int index = slot.position;
	if(mEnemys.find(index) == mEnemys.end())
	{
		mEnemys.insert(std::make_pair(index,slot));
		mDamage.insert(std::make_pair(index,_damage));
		mLifes.insert(std::make_pair(index,life));
	}
}

float FightTaskSkillBigMultiple::getPreparedEndTime()
{
	float dur = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::EXIT_START);
	return dur;//5.7f;
}

void FightTaskSkillBigMultiple::onPrepareEnd()
{
	float dur = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::EXIT_DURATION);

	mSlot.battle->retain();
	mSlot.battle->removeFromParent();
	BasePage* page = CCBManager::Get()->getPage("FightPage");
	CCB_FUNC(page,"mPeople",CCNode,addChild(mSlot.battle));

	CCMoveTo *act = CCMoveTo::create(dur,mSlot.mStopPoint);
	mSlot.battle->runAction(act);
	mSlot.battle->release();

	ENEMY_MAP::iterator it = mEnemys.begin();
	for(;it!=mEnemys.end();++it)
	{
		it->second.battle->retain();
		it->second.battle->removeFromParent();
		CCB_FUNC(page,"mPeople",CCNode,addChild(it->second.battle));
		CCMoveTo *actEnemy = CCMoveTo::create(dur,it->second.mStopPoint);
		it->second.battle->runAction(actEnemy);
		it->second.battle->release();

	}
}

void FightTaskSkillBigMultiple::onDamaged()
{
	ENEMY_MAP::iterator it = mEnemys.begin();
	for(;it!=mEnemys.end();++it)
	{
		if(it->second.life==-1)
		{
			continue;
		}
		float lifePersentage = 0;
		float dt = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_BLEEDING);
		lifePersentage = (float)it->second.life/((float)it->second.maxlife);
		CCActionInterval* numberto = NumberChangeAction::create(dt,it->second.life);
		CCB_FUNC(it->second.battle,"mBlood",CCLabelBMFont,runAction(numberto));
		CCNode* node = 0;
		CCNode* node2 = 0;
		CCB_FUNC_RETURN(it->second.battle,"mOwn1",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_BG),node);
		CCB_FUNC_RETURN(it->second.battle,"mPoster",CCNode,getChildByTag(FightPage_roundState::SLOTMEMBER::TAG_HEAD),node2);
		if(node && node2)
		{
			FightCardSprite* sprite = dynamic_cast<FightCardSprite*>(node);
			if(sprite) sprite->setLifeTo(lifePersentage,dt);

			FightCardSprite* sprite2 = dynamic_cast<FightCardSprite*>(node2);
			if(sprite2)	sprite2->setLifeTo(lifePersentage,dt);
		}
	}
	mDamage.clear();
}

float FightTaskSkillBigMultiple::getDamageTime()
{
	float dam = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::DAMAGE_TIME);
	return dam;//0.0f;
}
