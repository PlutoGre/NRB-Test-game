
#include "stdafx.h"

#include "FightPage_finalRound.h"
#include "DataTableManager.h"
#include "FightTasks.h"
#include "FightConfigManager.h"
#include "FightCardSprite.h"
#include "StringConverter.h"
#include "NumberChangeAction.h"
#include "SoundManager.h"

const float SINGLE_ENTER_TIME = 0.05f;
const float FIGHTING_TOTAL_TIME = 5.0f;
const int MAX_NEWLINE_POSITION=8;

FightPage_finalRound::FightPage_finalRound(void)
{
}


FightPage_finalRound::~FightPage_finalRound(void)
{
}

void FightPage_finalRound::Enter( FightPage* page)
{
	page->mCurrRound=3;
	CCB_FUNC(page,"mControlCover",CCMenuItemImage,setVisible(false));
	mBloodScale=1.0f;
	mWillScale=1.0f;
	mTotalTime=0;
	mStatus = PREPARE;
	_createSlot(page);
	CCB_FUNC(page,"mSkipButtonNode",CCNode,setVisible(false));

	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("finalRound"));
}

void FightPage_finalRound::Execute( FightPage* page)
{
	float dt = GamePrecedure::Get()->getFrameTime();
	if(dt>1.0f)return;
	mTotalTime+=dt;

	if(mStatus == PREPARE)
	{
		mStatus = TITLING;
		_title_start(page);
	}
	else if(mStatus == TITLING)
	{
		if(mTotalTime>1.0f)
			mStatus = ENTERING;
	}
	else if(mStatus == ENTERING)
	{
		if(mPreEnter.empty())
		{
			mStatus = RUSHING_TO_CENTER;
			_rushToCenter(page);
			mTotalTime = 0;
		}
		else
			_enter();
	}
	else if(mStatus == RUSHING_TO_CENTER)
	{
		
		if(mTotalTime>0.7f)
		{
			_fight_start(page);
			mStatus = FIGHTING;
		}
	}
	else if(mStatus == FIGHTING)
	{
		
		if(_fight(page))
		{
			mStatus = SHOWING_FINAL;
			_show_final(page);
			mTotalTime=0;
		}
	}
	else if(mStatus == SHOWING_FINAL)
	{
		
		if(mTotalTime>1.5f)
			page->nextRound();
	}
}

void FightPage_finalRound::Exit( FightPage* )
{
	SoundManager::Get()->stopAllEffect();
	std::map<int, CCBContainer*>::iterator it = mBattleCharacter.begin();
	for(;it!=mBattleCharacter.end();++it)
	{
		it->second->release();
	}
	it = mFaces.begin();
	for(;it!=mFaces.end();++it)
	{
		it->second->release();
	}
	
	mBattleCharacter.clear();
	mPreEnter.clear();
	if(mBattleEffect)
	{
		mBattleEffect->removeFromParent();
		mBattleEffect->release();
		mBattleEffect = 0;
	}
}

void FightPage_finalRound::_createSlot(FightPage* page)
{
	
	mEnterTime = 0;
	mSelfTotalLife[CURRENT] = mSelfTotalLife[MAX] =0;
	mSelfTotalPower[CURRENT] = mSelfTotalPower[MAX] = 0;
	mEnemyTotalLife[CURRENT] = mEnemyTotalLife[MAX] = 0;
	mEnemyTotalPower[CURRENT] = mEnemyTotalPower[MAX] = 0;
	mChangeStep = 0;

	mBattleEffect = CCBContainer::create();

	if(mBattleEffect)
	{
		mBattleEffect->retain();
		mBattleEffect->setVisible(false);
		mBattleEffect->loadCcbiFile("s_battle3th.ccbi");
		CCBContainer* aniNode=CCBContainer::create();
		bool isWin=false;
		if(page->getMessage()->result().winlose()<=OPUserBattleRet_Result_WINLOSE_JUSTWIN)
		{
			isWin=true;
		}
		aniNode->loadCcbiFile(isWin?"Battle_round03_B.ccbi":"Battle_round03_A.ccbi");
		if(aniNode)
		{
			aniNode->setAnchorPoint(ccp(0,0));
			mBattleEffect->addChild(aniNode,-1);
		}
		//mBattleEffect->runAnimation("Default Timeline");
		CCObject* obj = page->getVariable("mBigskill");
		if(obj)
		{
			CCNode* node = dynamic_cast<CCNode*>(obj);
			if(node)
			{
				CCObject* _obj=mBattleEffect->getVariable("mWillpower1");
				if(_obj)
				{
					CCSprite* _willpower1=dynamic_cast<CCSprite* >(_obj);
					if(_willpower1)
					{
						mWillScale=_willpower1->getScaleX();
					}
				}
				CCObject* _bloodObj=mBattleEffect->getVariable("mBlood1");
				if(_bloodObj)
				{
					CCSprite* _blood=dynamic_cast<CCSprite* >(_bloodObj);
					if(_blood)
					{
						mBloodScale=_blood->getScaleX();
					}
				}
				node->addChild(mBattleEffect);
			}
		}
	}

	cocos2d::CCNode* node =  dynamic_cast<cocos2d::CCNode*>(page->getVariable("mPeople"));
	int slotsize = page->getMessage()->round3().characters_size();
	int maxPositionSize = 0;
	for(int i=0;i<slotsize;++i)
	{
		if(page->getMessage()->round3().characters(i).position()>maxPositionSize)
			maxPositionSize = page->getMessage()->round3().characters(i).position();
	}
	for(int i=0;i<slotsize;++i)
	{
		//CCBContainer* battleCharacter = CCBContainer::create();
		//battleCharacter->loadCcbiFile("s_buff.ccbi");
		//node->addChild(battleCharacter);
		const OPUserBattleRet_Character& character = page->getMessage()->round3().characters().Get(i);
		int positionID = character.position();
		bool isNewLine=false;
		if(positionID>=MAX_NEWLINE_POSITION)
		{
			isNewLine=true;
		}
		if(positionID%FightPage::TEAM_COUNT == FightPage::TEAM_SELF)
		{
			mSelfTotalLife[MAX]+=character.life();
			mSelfTotalPower[MAX]+=character.power();
			mSelfLifes.push_back(std::make_pair(character.life(),positionID));
			mSelfPowers.push_back(std::make_pair(character.power(),positionID));
		}
		else
		{
			mEnemyTotalLife[MAX]+=page->getMessage()->round3().characters().Get(i).life();
			mEnemyTotalPower[MAX]+=page->getMessage()->round3().characters().Get(i).power();
			mEnemyLifes.push_back(std::make_pair(character.life(),positionID));
			mEnemyPowers.push_back(std::make_pair(character.power(),positionID));
		}
		float unit = FightPage::calcXunit(positionID,(MAX_NEWLINE_POSITION+2)>>1,isNewLine)-0.5f;
		mPreEnter.push_back(positionID);


		CCBContainer* battleFace = CCBContainer::create();
		if(battleFace)
		{
			battleFace->loadCcbiFile("s_battle3thico.ccbi");
			battleFace->setScale(0.7f);
			battleFace->retain();
			battleFace->setVisible(true);
		}
		bool _isSelf=false;
		if(positionID%FightPage::TEAM_COUNT == FightPage::TEAM_SELF)
		{
			battleFace->setPosition(ccp((unit+0.75f)*(battleFace->getContentSize().width*battleFace->getScale()),isNewLine?(-1)*battleFace->getContentSize().height:0));
			battleFace->setPositionX(battleFace->getPositionX()-FightConfigManager::Get()->getThicoOffSiteWidth());
			CCB_FUNC(mBattleEffect,"mSelfNode",CCNode,addChild(battleFace));
		}
		else
		{
			battleFace->setPosition(ccp((0-unit-2.0f+1.25f)*battleFace->getContentSize().width*battleFace->getScale(),isNewLine?(-1)*battleFace->getContentSize().height:0));
			_isSelf=true;
			battleFace->setPositionX(battleFace->getPositionX()+FightConfigManager::Get()->getThicoOffSiteWidth());
			CCB_FUNC(mBattleEffect,"mEnemyNode",CCNode,addChild(battleFace));
		}

		mFaces.insert(std::make_pair(positionID,battleFace));


		int resoureID = page->getMessage()->round3().characters().Get(i).resourceid();
		const DiscipleItem* item =  DiscipleTableManager::Get()->getDiscipleItemByID(resoureID);
		if(item)
		{
			std::string bgFightPic = VaribleManager::Get()->getSetting("ImageFileSelf");
			std::string ftFightPic = /*"person/poster_char_xiangjishi.png";//*/item->bodyPic;
			
			if(!_isSelf)
			{
				CCB_FUNC(battleFace,"mOwn2",CCSprite,initWithFile(VaribleManager::Get()->getSetting("ImageFileBloodEnemy").c_str()));
				bgFightPic = VaribleManager::Get()->getSetting("ImageFileEnemy");
			}
			else
			{
				CCB_FUNC(battleFace,"mOwn2",CCSprite,initWithFile(VaribleManager::Get()->getSetting("ImageFileBloodSelf").c_str()));
			}
			FightCardSprite *cardhead = FightCardSprite::create(bgFightPic.c_str());
			cardhead->setAnchorPoint(ccp(0,0));
			CCB_FUNC(battleFace,"mOwn1",CCNode,addChild(cardhead,1,101));
			if(ftFightPic == "none")ftFightPic = "person/poster_char_xiangjishi.png";
			cardhead = FightCardSprite::create(ftFightPic.c_str());
			if(!cardhead)cardhead = FightCardSprite::create("person/poster_char_xiangjishi.png");
			cardhead->setAnchorPoint(ccp(0,0));
			CCB_FUNC(battleFace,"mPoster",CCNode,addChild(cardhead,1,100)); 
			if(item->quality>0)
			{
				char key[128];
				sprintf(key,"MemImageFileQuality%d",item->quality);
				CCB_FUNC(battleFace,"mQuality",CCSprite,initWithFile(VaribleManager::Get()->getSetting(key).c_str()));
			}
			CCB_FUNC(battleFace,"mBlood",CCLabelBMFont,setString(StringConverter::toString(character.life()).c_str()));
			CCB_FUNC(battleFace,"mPosition",CCLabelBMFont,setString(StringConverter::toString((positionID+2)>>1).c_str()));
		}
		else
		{
			char errstr[64];
			sprintf(errstr,"failed to get disciple: resoureceID:%d", resoureID);
			CCMessageBox(errstr, "error");
			return;
		}

	}

	mSelfLifes.sort();
	mSelfPowers.sort();
	mEnemyLifes.sort();
	mEnemyPowers.sort();

	
	mSelfTotalLife[CURRENT] = mSelfTotalLife[MAX] ;
	mSelfTotalPower[CURRENT] = mSelfTotalPower[MAX] ;
	mEnemyTotalLife[CURRENT] = mEnemyTotalLife[MAX] ;
	mEnemyTotalPower[CURRENT] = mEnemyTotalPower[MAX] ;

	mChangeStep = ((mEnemyTotalLife[MAX]+mEnemyTotalPower[MAX]>mSelfTotalLife[MAX]+mSelfTotalPower[MAX])?
		((mSelfTotalLife[MAX]+mSelfTotalPower[MAX])/FIGHTING_TOTAL_TIME):((mEnemyTotalLife[MAX]+mEnemyTotalPower[MAX])/FIGHTING_TOTAL_TIME));

	CCBContainer* ccb = CCBContainer::create();
	ccb->loadCcbiFile("s_battle3thcontent.ccbi");
	mBattleEffect->setVisible(true);
	CCB_FUNC(mBattleEffect,"mBattleani",CCNode,addChild(ccb));
	CCB_FUNC(mBattleEffect,"mBattleani",CCNode,setVisible(false));
}

void FightPage_finalRound::_enter()
{
	mEnterTime-=GamePrecedure::Get()->getFrameTime();

	if(mEnterTime<=0)
	{
		mEnterTime = SINGLE_ENTER_TIME;
		if(!mPreEnter.empty())
		{
			int positionID = mPreEnter.front();
			std::map<int, CCBContainer*>::iterator it = mBattleCharacter.find(positionID);
			if(it!=mBattleCharacter.end())
			{
				it->second->setVisible(true);
				it->second->runAnimation("beginning");
			}
			mPreEnter.pop_front();
		}
	}
}


void FightPage_finalRound::_rushToCenter(FightPage* page)
{
	std::map<int, CCBContainer*>::iterator it = mFaces.begin();
	int totalCount = mBattleCharacter.size();
	int maxPositionSize = 0;
	for(int i=0;i<totalCount;++i)
	{
		if(page->getMessage()->round3().characters(i).position()>maxPositionSize)
			maxPositionSize = page->getMessage()->round3().characters(i).position();
	}
	/*
	for(int id = 0;it!=mBattleCharacter.end();++it,++id)
	{
		int positionID = it->first;//page->getMessage()->round3().characters().Get(id).position();
		float xDis = (FightPage::caculateXunit(positionID,maxPositionSize)+0.5f)*76.0f;
		float yDis = ((positionID%2==0)?-150.0f:150.0f);
		
		it->second->runAction(
			CCSequence::createWithTwoActions(
				CCMoveBy::create(0.5f,ccp(xDis*0.5f,yDis*0.5f)),
				CCMoveBy::create(0.2f,ccp(xDis*-1.5f,yDis*-1.5f))
			));
	}
	*/
	for(;it!=mFaces.end();++it)
	{
		int positionId=it->first;
		CCPoint _point=CCPoint(0,0);
		if(positionId%FightPage::TEAM_COUNT == FightPage::TEAM_SELF)
		{
			_point.setPoint(it->second->getPositionX()+FightConfigManager::Get()->getThicoOffSiteWidth(),it->second->getPositionY());
		}
		else
		{
			_point.setPoint(it->second->getPositionX()-FightConfigManager::Get()->getThicoOffSiteWidth(),it->second->getPositionY());
		}
		CCActionInterval* move = CCMoveTo::create(FightConfigManager::Get()->getThicoEntranceDurationTime(), CCPointMake(_point.x,_point.y));
		it->second->runAction(CCSpawn::create(CCEaseExponentialInOut::create((CCActionInterval*)(move->copy()->autorelease())),NULL));
	}
}

bool FightPage_finalRound::_fight(FightPage* page)
{
	
	float dt = GamePrecedure::Get()->getFrameTime();

	if(!mSelfPowers.empty())
	{
		float step = mChangeStep*dt;
		mSelfTotalPower[CURRENT]-=step;
		if(mSelfTotalPower[CURRENT]<0)
			mSelfTotalPower[CURRENT]=0;
		if(mSelfTotalPower[MAX]>0)
		{
			CCB_FUNC(mBattleEffect,"mWillpower1b",CCSprite,setVisible(true));
			float _scale=mSelfTotalPower[CURRENT]/mSelfTotalPower[MAX];

			CCObject* _obj=mBattleEffect->getVariable("mWillpower1");
			if(_obj)
			{
				CCSprite* _willpower1=dynamic_cast<CCSprite* >(_obj);
				if(_willpower1)
				{
					_willpower1->setScaleX(mWillScale*_scale);
					CCB_FUNC(mBattleEffect,"mWillpower1b",CCSprite,setPositionX(_willpower1->getPositionX()-_willpower1->getContentSize().width*_willpower1->getScaleX()));
				}
			}
			if(_scale==0.0f)
			{
				CCB_FUNC(mBattleEffect,"mWillpower1b",CCSprite,setVisible(false));
			}
		}
		else
		{
			CCB_FUNC(mBattleEffect,"mWillpower1b",CCSprite,setVisible(false));
		}
		while(step>0 && !mSelfPowers.empty())
		{
			POWERLIST::iterator it = mSelfPowers.begin();
			if(it!=mSelfPowers.end())
			{
				float lifevalue = it->first;
				int positionID = it->second;
				mSelfPowers.erase(it);

				lifevalue-=step;
				if(lifevalue>0)
				{
					mSelfPowers.push_front(std::make_pair(lifevalue,positionID));
					step = 0;
				}
				else
				{
					step = -lifevalue;
					std::map<int,CCBContainer*>::iterator it = mFaces.find(positionID);
					if(it!=mFaces.end())
					{
						_effect_shake();
						it->second->runAnimation("hurt");
					}
				}

			}

		}
	}
	else if(!mSelfLifes.empty())
	{
		float step = mChangeStep*dt;
		mSelfTotalLife[CURRENT]-=step;
		if(mSelfTotalLife[CURRENT]<0)
			mSelfTotalLife[CURRENT]=0;
		if(mSelfTotalLife[MAX]>0)
		{
			CCB_FUNC(mBattleEffect,"mBlood1b",CCSprite,setVisible(true));
			float _scale=mSelfTotalLife[CURRENT]/mSelfTotalLife[MAX];
			CCObject* _obj=mBattleEffect->getVariable("mBlood1");
			if(_obj)
			{
				CCSprite* _blood1=dynamic_cast<CCSprite* >(_obj);
				if(_blood1)
				{
					_blood1->setScaleX(mBloodScale*_scale);
					CCB_FUNC(mBattleEffect,"mBlood1b",CCSprite,setPositionX(_blood1->getPositionX()-_blood1->getContentSize().width*_blood1->getScaleX()));
				}
			}
			if(_scale==0.0f)
			{
				CCB_FUNC(mBattleEffect,"mBlood1b",CCSprite,setVisible(false));
			}
		}
		else
		{
			CCB_FUNC(mBattleEffect,"mBlood1b",CCSprite,setVisible(false));
		}
		while(step>0 && !mSelfLifes.empty())
		{
			POWERLIST::iterator it = mSelfLifes.begin();
			if(it!=mSelfLifes.end())
			{
				float lifevalue = it->first;
				int positionID = it->second;
				mSelfLifes.erase(it);

				lifevalue-=mChangeStep*dt;
				if(lifevalue>0)
				{
					mSelfLifes.push_front(std::make_pair(lifevalue,positionID));
					step = 0;
				}
				else
				{
					step = -lifevalue;
					std::map<int,CCBContainer*>::iterator it = mFaces.find(positionID);
					if(it!=mFaces.end())
					{
						_effect_shake();
						float dt = 0.5f;
						CCActionInterval* numberto = NumberChangeAction::create(dt,0);
						CCB_FUNC(it->second,"mBlood",CCLabelBMFont,runAction(numberto));
						it->second->runAnimation("dead");
					}
				}

			}
		}
	}
	else
	{
		return true;
	}

	if(!mEnemyPowers.empty())
	{
		float step = mChangeStep*dt;
		mEnemyTotalPower[CURRENT]-=step;
		if(mEnemyTotalPower[CURRENT]<0)
			mEnemyTotalPower[CURRENT]=0;
		if(mEnemyTotalPower[MAX]>0)
		{
			CCB_FUNC(mBattleEffect,"mWillpower2b",CCSprite,setVisible(true));
			float _scale=mEnemyTotalPower[CURRENT]/mEnemyTotalPower[MAX];
			CCObject* _obj=mBattleEffect->getVariable("mWillpower2");
			if(_obj)
			{
				CCSprite* _willpower2=dynamic_cast<CCSprite* >(_obj);
				if(_willpower2)
				{
					_willpower2->setScaleX(mWillScale*_scale);
					CCB_FUNC(mBattleEffect,"mWillpower2b",CCSprite,setPositionX(_willpower2->getPositionX()+_willpower2->getContentSize().width*_willpower2->getScaleX()));
				}
			}
			if(_scale==0.0f)
			{
				CCB_FUNC(mBattleEffect,"mWillpower2b",CCSprite,setVisible(false));
			}
		}
		else
		{
			CCB_FUNC(mBattleEffect,"mWillpower2b",CCSprite,setVisible(false));
		}
		while(step>0 && !mEnemyPowers.empty())
		{
			POWERLIST::iterator it = mEnemyPowers.begin();
			if(it!=mEnemyPowers.end())
			{
				float lifevalue = it->first;
				int positionID = it->second;
				mEnemyPowers.erase(it);

				lifevalue-=step;
				if(lifevalue>0)
				{
					mEnemyPowers.push_front(std::make_pair(lifevalue,positionID));
					step = 0;
				}
				else
				{
					step = -lifevalue;
					std::map<int,CCBContainer*>::iterator it = mFaces.find(positionID);
					if(it!=mFaces.end())
					{
						_effect_shake();
						it->second->runAnimation("hurt");
					}
				}

			}
		}
	}
	else if(!mEnemyLifes.empty())
	{
		float step = mChangeStep*dt;
		mEnemyTotalLife[CURRENT]-=step;
		if(mEnemyTotalLife[CURRENT]<0)
			mEnemyTotalLife[CURRENT]=0;
		if(mEnemyTotalLife[MAX]>0)
		{
			CCB_FUNC(mBattleEffect,"mBlood2b",CCSprite,setVisible(true));
			float _scale=mEnemyTotalLife[CURRENT]/mEnemyTotalLife[MAX];
			CCObject* _obj=mBattleEffect->getVariable("mBlood2");
			if(_obj)
			{
				CCSprite* _blood2=dynamic_cast<CCSprite* >(_obj);
				if(_blood2)
				{
					_blood2->setScaleX(mBloodScale*_scale);
					CCB_FUNC(mBattleEffect,"mBlood2b",CCSprite,setPositionX(_blood2->getPositionX()+_blood2->getContentSize().width*_blood2->getScaleX()));
				}	
			}
			if(_scale==0.0f)
			{
				CCB_FUNC(mBattleEffect,"mBlood2b",CCSprite,setVisible(false));
			}
		}
		else
		{
			CCB_FUNC(mBattleEffect,"mBlood2b",CCSprite,setVisible(false));
		}
		while(step>0 && !mEnemyLifes.empty())
		{
			POWERLIST::iterator it = mEnemyLifes.begin();
			if(it!=mEnemyLifes.end())
			{
				float lifevalue = it->first;
				int positionID = it->second;
				mEnemyLifes.erase(it);

				lifevalue-=mChangeStep*dt;
				if(lifevalue>0)
				{
					mEnemyLifes.push_front(std::make_pair(lifevalue,positionID));
					step = 0;
				}
				else
				{
					step = -lifevalue;
					std::map<int,CCBContainer*>::iterator it = mFaces.find(positionID);
					if(it!=mFaces.end())
					{
						_effect_shake();
						float dt = 0.5f;
						CCActionInterval* numberto = NumberChangeAction::create(dt,0);
						CCB_FUNC(it->second,"mBlood",CCLabelBMFont,runAction(numberto));
						it->second->runAnimation("dead");
					}
				}

			}
		}
	}
	else
	{
		return true;
	}
	
	return false;
}

void FightPage_finalRound::_show_final(FightPage* page)
{
	//CCB_FUNC(mBattleEffect,"mBattleEffect",CCNode,runAction(CCScaleTo::actionWithDuration(0.5f,3,3)));
	mBattleEffect->runAnimation("ending");
}


void FightPage_finalRound::_title_start(FightPage* page)
{
	FightTaskRoundStart task(3,page);
	task.onStart();

}

void FightPage_finalRound::_fight_start( FightPage * page )
{
	std::map<int, CCBContainer*>::iterator it = mFaces.begin();
	for(;it!=mFaces.end();++it)
	{
		it->second->setVisible(true);
		it->second->runAnimation("appear");
	}
	CCB_FUNC(mBattleEffect,"mBattleani",CCNode,setVisible(true));
}

void FightPage_finalRound::_effect_shake()
{
// 	CCAction* act = CCSequence::actionOneTwo(
// 		CCScaleTo::actionWithDuration(0.15f,1.2,1.2),
// 		CCScaleTo::actionWithDuration(0.15f,1,1)
// 		);
	CCAction* act = CCSequence::createWithTwoActions(
		CCMoveBy::create(0.1f,ccp(0,-30.0f)),
		CCMoveBy::create(0.1f,ccp(0,30.0f))
		);

	CCB_FUNC(mBattleEffect,"mBattleEffect",CCNode,runAction(act));
}
