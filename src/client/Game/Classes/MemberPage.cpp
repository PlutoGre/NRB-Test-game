
#include "stdafx.h"

#include "MemberPage.h"

REGISTER_PAGE("MemberPage",MemberPage);
USING_NS_CC;

MemberPage::MemberPage(void)
{
}


MemberPage::~MemberPage(void)
{
}


void MemberPage::Enter( MainFrame* frame)
{
	loadMemberConfig();

	for(unsigned int i=1;i<=MEMBERTABLENGTH;++i)
	{
		char tab[128];
		sprintf(tab,"mTab_%d",i);
		CCObject* obj=getVariable(tab);
		if(!obj)
		{
			char errMsg[256];
			sprintf(errMsg,"Failed to find variable:mTab_%d",i);
			CCMessageBox(errMsg,"error");
		}
		CCMenuItemSprite* tabSprite =  dynamic_cast<cocos2d::CCMenuItemSprite* >(obj);
		if(tabSprite)
		{
			mMPTabList.insert(TAB_BUTTON_LIST::value_type(i,tabSprite));
		}
	}
	switchMPTab();
}

void MemberPage::toggleContentView()
{	
	cocos2d::extension::CCScrollView* members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	if(members)
	{
		cocos2d::CCNode *node = cocos2d::CCNode::create();
		CCSize size = CCSize(0,0);

		MEMBERENTITY_LIST* itmap=new MEMBERENTITY_LIST;
		if(mCurrTabIndex==MEMBER)
		{
			itmap=cloneEntityList(&mMPMemberList);
		}
		else if(mCurrTabIndex==SOUL)
		{
			itmap=cloneEntityList(&mMPSoulList);
		}

		MEMBERENTITY_LIST::reverse_iterator it=itmap->rbegin();
		int i=0;
		for(;it!=itmap->rend();it++)
		{
			MemberEntity* entity=(*it)->second;
			CCBContainer* memberContent =CCBContainer::create();
			memberContent->loadCcbiFile("MembersContent.ccbi");;
			memberContent->setListener(this,i);
			CCMenuItemSprite* frame = dynamic_cast<CCMenuItemSprite*>(memberContent->getVariable("mFace"));
			if(frame && frame->getParent())
			{
				CCMenu* menu = dynamic_cast<CCMenu*>(frame->getParent());
				if(menu)
				{
					menu->setScrollViewChild(true);
				}
			}

			CCSprite* mHeadPic=dynamic_cast<CCSprite* >(memberContent->getVariable("mHeadPic"));
			if(mHeadPic)
			{
				mHeadPic->setTexture(CharacterConsts::Get()->getCharacter(entity->id)->facePic.c_str());
			}

			CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPName"));
			if(name)
			{
				name->setString(Language::Get()->getString(CharacterConsts::Get()->getCharacter(entity->id)->name).c_str());
			}

			CCLabelBMFont* quality=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPQuality"));
			if(quality)
			{
				char qualityKey[128];
				sprintf(qualityKey,"%s%d","@mMPQuality_",entity->quality);
				quality->setString(Language::Get()->getString(qualityKey).c_str());
			}

			CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPLevelName"));
			bool isSoul=entity->memberType==SOUL?true:false;

			if(levelName)
			{
				if(isSoul)
				{
					levelName->setString(Language::Get()->getString("@MPQuantity").c_str());
				}
				else
				{
					levelName->setString(Language::Get()->getString("@MPLevel").c_str());
				}
			}

			CCLabelBMFont* levelValue=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPLevel"));
			if(levelValue)
			{
				char level[128];
				sprintf(level,"%d",entity->level);
				levelValue->setString(level);
			}

			CCLabelBMFont* valueKey=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPValueKey"));
			if(valueKey)
			{
				if(isSoul)
				{
					valueKey->setVisible(false);
				}
				else
				{
					valueKey->setVisible(true);
				}
			}

			CCSprite* valueSymbol=dynamic_cast<CCSprite* >(memberContent->getVariable("mMPValueSymbol"));
			if(valueSymbol)
			{
				if(isSoul)
				{
					valueSymbol->setVisible(false);
				}
				else
				{
					valueSymbol->setVisible(true);
				}
			}

			CCLabelBMFont* mpValue=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPValueValue"));
			if(mpValue)
			{
				if(isSoul)
				{
					mpValue->setVisible(false);
				}
				else
				{
					mpValue->setVisible(true);
					char mpValueStr[128];
					sprintf(mpValueStr,"%d",entity->memberValue);
					mpValue->setString(mpValueStr);
				}
			}

			CCLabelBMFont* mpInBattle=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPInBattle"));
			if(mpInBattle)
			{
				if(isSoul)
				{
					mpInBattle->setVisible(false);
				}
				else
				{
					mpInBattle->setVisible(true);
				}
			}

			CCMenuItemSprite* oprButton1=dynamic_cast<CCMenuItemSprite* >(memberContent->getVariable("mOprButton_1"));
			if(oprButton1)
			{
				if(isSoul)
				{
					oprButton1->setVisible(false);
				}
				else
				{
					oprButton1->setVisible(true);
				}
			}

			CCLabelBMFont* mOprName1=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mOprName_1"));
			if(mOprName1)
			{
				if(isSoul)
				{
					mOprName1->setVisible(false);
				}
				else
				{
					mOprName1->setVisible(true);
				}
			}

			CCLabelBMFont* mOprName2=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mOprName_2"));
			CCMenuItemSprite* oprButton2=dynamic_cast<CCMenuItemSprite* >(memberContent->getVariable("mOprButton_2"));
			CCLabelBMFont* soulTip=dynamic_cast<CCLabelBMFont* >(memberContent->getVariable("mMPSoulDetail"));

			if(mOprName2)
			{
				if(isSoul)
				{
					//TODO:test data
					MEMBERENTITY_MAP::iterator itMember=mMPMemberList.find(entity->id);
					char soulTipKey[256];
					if(itMember!=mMPMemberList.end())
					{
						sprintf(soulTipKey,"@MPSoulTipBreak");
						mOprName2->setString(Language::Get()->getString("@MPBreak").c_str());
					}
					else
					{
						sprintf(soulTipKey,"@MPSoulTipRecruit");
						mOprName2->setString(Language::Get()->getString("@MPRecruit").c_str());
					}
					
					if(oprButton2)
					{
						//TODO:test 40
						if(entity->level>40)
						{
							soulTip->setVisible(false);
							oprButton2->setVisible(true);
							mOprName2->setVisible(true);
						}
						else
						{
							char needSoul[256];
							sprintf(needSoul,"%d",(30-entity->level));
							soulTip->setString(Language::Get()->getString(soulTipKey).c_str());
							soulTip->setVisible(true);
							oprButton2->setVisible(false);
							mOprName2->setVisible(false);
						}
					}
				}
				else
				{
					soulTip->setVisible(false);
					if(oprButton2)
					{
						oprButton2->setVisible(true);
					}
					mOprName2->setString(Language::Get()->getString("@MPTrain").c_str());
				}
			}


			node->addChild(memberContent);
			memberContent->setPosition(ccp(0,memberContent->getContentSize().height*i));

			size.width = memberContent->getContentSize().width;
			size.height += memberContent->getContentSize().height;
			i++;
		}
		node->setContentSize(size);
		members->setContainer(node);
		members->setContentSize(size);
		members->setContentOffset(ccp(0,members->minContainerOffset().y));
	}
}

inline MemberPage::MEMBERENTITY_LIST* MemberPage::cloneEntityList(MemberPage::MEMBERENTITY_MAP* entityList)
{
	MemberPage::MEMBERENTITY_LIST* cloneMap=new MemberPage::MEMBERENTITY_LIST;
	MEMBERENTITY_MAP::iterator it=entityList->begin();
	for(;it!=entityList->end();++it)
	{
		cloneMap->push_back(it);
	}
	cloneMap->sort(&MemberPage::cmp);
	return cloneMap;
}

bool MemberPage::cmp(const MEMBERENTITY_MAP::iterator a,const MEMBERENTITY_MAP::iterator b)
{
	return a->second->level>b->second->level;
}

void MemberPage::loadMemberConfig()
{
	mMPMemberList.clear();
	mMPSoulList.clear();
	//TODO:test data 
	for(int i=0;i<20;i++)
	{
		MemberEntity* me=new MemberEntity;
		me->id=1;
		me->quality=(rand()%4+1);
		me->level=(rand()%100+1);
		me->memberValue=200+(rand()%2000);
		me->memberType=1;
		mMPMemberList.insert(MEMBERENTITY_MAP::value_type(i,me));
	}
	
	for(int i=0;i<25;i++)
	{
		MemberEntity* me=new MemberEntity;
		me->id=1;
		me->quality=(rand()%4+1);
		me->level=(rand()%100+1);
		me->memberValue=200+(rand()%2000);
		me->memberType=2;
		mMPSoulList.insert(MEMBERENTITY_MAP::value_type(i,me));
	}

	mCurrTabIndex=MEMBER;
	mMPTabList.clear();
}

void MemberPage::switchMPTab()
{
	TAB_BUTTON_LIST::iterator it=mMPTabList.begin();
	for(;it!=mMPTabList.end();++it)
	{
		CCMenuItemSprite* tabSprite=it->second;
		if(mCurrTabIndex!=it->first)
		{
			tabSprite->setEnabled(true);
		}
		else
		{
			tabSprite->setEnabled(false);
		}
	}
	
	toggleContentView();
	++mCurrTabIndex;
	if(mCurrTabIndex>MEMBERTABLENGTH)
	{
		mCurrTabIndex=MEMBER;
	}
}

void MemberPage::Exit( MainFrame* frame)
{
}

void MemberPage::Execute( MainFrame* )
{

}

void MemberPage::load( void )
{
	loadCcbiFile("MembersPage.ccbi");
}

void MemberPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onShowMemberInfo")
	{
		CCMessageBox("MemberPage onShowMemberInfo ","debug");
	}
	else if(itemName=="onOprButton_1")
	{
		CCMessageBox("MemberPage onOprButton_1 ","debug");
	}
	else if(itemName=="onOprButton_2")
	{
		CCMessageBox("MemberPage onOprButton_2 ","debug");
	}
}

void MemberPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onToggleMemberButton"||itemName=="onToggleSoulButton")
	{
		switchMPTab();
	}
}

