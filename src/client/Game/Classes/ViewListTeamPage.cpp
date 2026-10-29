
#include "stdafx.h"

#include "ViewListTeamPage.h"
#include "MessageBoxPage.h"
#include "BattleArrayInfo.h"
#include "Title.h"
#include "TitleInfoPage.h"
#include "ViewTeamPage.h"
REGISTER_PAGE("ViewListTeamPage",ViewListTeamPage);
USING_NS_CC;

void DiscipleHeadContent::refreshContent( void )
{
	mMemberPic=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mHead"));
	mMemberFrame=dynamic_cast<CCMenuItemImage* >(this->getObjectByVariableName("mFrame"));
	mName=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mMemName"));
	mQuality=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mMemQuality"));
	mMemLV=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mMemLV"));
	_refreshDisciple();
}

void DiscipleHeadContent::_refreshDisciple()
{
	Disciple disciple(getContentID(),false,mType!=SelfTeam);
	int count=BlackBoard::Get()->getUpTBDiscipleById(getContentID());
	if(mName)
	{
		mName->setString(disciple.name().c_str());
	}
	if(mQuality)
	{
		mQuality->setVisible(true);
		mQuality->initWithFile(disciple.getQualityImageFile().c_str());
	}
	if(mMemLV)
	{
		mMemLV->setString(StringConverter::toString(disciple.level()).c_str());
	}
	if(mMemberPic)
	{
		mMemberPic->initWithFile(disciple.iconPic().c_str());
	}
	if(mMemberFrame)
	{
		mMemberFrame->setNormalImage(disciple.getFrameNormalSpirte());
		mMemberFrame->setSelectedImage(disciple.getFrameSelectedSpirte());
		mMemberFrame->setDisabledImage(disciple.getFrameDisabledSpirte());
		if(mType==SelfTeam)
		{
			mMemberFrame->setEnabled(false);
		}
		else
		{
			mMemberFrame->setEnabled(true);
		}
	}
	
}

void DiscipleHeadContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onDiscipleInfo")
	{
		BlackBoard::Get()->TeamShowIndex=mCurrPosition+1;
		MsgMainFramePushPage msg;
		msg.pageName = "ViewTeamPage";
		MessageManager::Get()->sendMessage(&msg);
		
		MsgMainFramePopPage msgPop;
		msgPop.pageName = "ViewListTeamPage";
		MessageManager::Get()->sendMessage(&msgPop);
	}
}

cocos2d::CCObject * DiscipleHeadContent::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getContainerNode()->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

void TitleContentPage::refreshContent( void )
{
	mTitlePic=dynamic_cast<CCSprite* >(this->getObjectByVariableName("mTitleHead"));
	mTitleFrame=dynamic_cast<CCMenuItemImage* >(this->getObjectByVariableName("mTitleFrame"));
	mName=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mTitleName"));
	mTitleLV=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mTitleLV"));
	_refreshPage();
}

void TitleContentPage::_refreshPage()
{
	Title title(getContentID(),mType!=SelfTeam);
	if(mName)
	{
		mName->setString(title.name().c_str());
	}

	if(mTitleLV)
	{
		mTitleLV->setString(StringConverter::toString(title.level()).c_str());
	}
	if(mTitlePic)
	{
		mTitlePic->initWithFile(title.iconPic().c_str());
	}
}

void TitleContentPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onTitleInfo")
	{
		BlackBoard::Get()->ShowTitle = getContentID();
		BasePage* bp  = CCBManager::Get()->getPage("TitleInfoPage");
		TitleInfoPage* page = bp?dynamic_cast<TitleInfoPage*>(bp):0;
		if(page)
			page->setIsTeamView(mType!=SelfTeam);
		MsgMainFramePushPage msg;
		msg.pageName = "TitleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

cocos2d::CCObject * TitleContentPage::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getContainerNode()->getVariable(variableName);
	if(!obj)
	{
// 		char errMsg[256];
// 		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
// 		CCMessageBox(errMsg,"error");
	}
	return obj;
}

ViewListTeamPage::ViewListTeamPage(void)
{
	mSelfHeight=0.0f;
	mEnemyHeight=0.0f;
}


ViewListTeamPage::~ViewListTeamPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void ViewListTeamPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	clearAllItem();
	mSelfTitleSymbol=cocos2d::CCSprite::create();
	mSelfTitleSymbol->setAnchorPoint(ccp(0,1));
	mSelfTitleSymbol->initWithFile(VaribleManager::Get()->getSetting("TeamBattle_Title_Pic").c_str());
	mSelfRootNode->addChild(mSelfTitleSymbol);
	mEnemyTitleSymbol=cocos2d::CCSprite::create();
	mEnemyTitleSymbol->setAnchorPoint(ccp(0,1));
	mEnemyTitleSymbol->initWithFile(VaribleManager::Get()->getSetting("TeamBattle_Title_Pic").c_str());
	mEnemyRootNode->addChild(mEnemyTitleSymbol);
	_buildItem();
}

void ViewListTeamPage::Exit( MainFrame* frame)
{
	if(mSelfTitleSymbol)
	{
		mSelfTitleSymbol->removeFromParentAndCleanup(true);
	}
	if(mEnemyTitleSymbol)
	{
		mEnemyTitleSymbol->removeAllChildrenWithCleanup(true);
	}
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
}

void ViewListTeamPage::Execute( MainFrame* )
{

}

cocos2d::CCObject* ViewListTeamPage::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}


void ViewListTeamPage::load( void )
{
	loadCcbiFile("b_jiesuanD.ccbi");
	mSelfContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mSelfContent"));
	if(mSelfContent)
	{
		mSelfContent->setAnchorPoint(ccp(0,1));
		mSelfRootNode = mSelfContent->getContainer();
	}	

	mEnemyContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(getObjectByVariableName("mEnemyContent"));
	if(mEnemyContent)
	{
		mEnemyContent->setAnchorPoint(ccp(0,1));
		mEnemyRootNode = mEnemyContent->getContainer();
	}
}

void ViewListTeamPage::_buildItem()
{
	std::string _fightEnemyNameKey="fightEnemyNameKey";
	std::string _fightEnemyName="???";
	if(HAS_V(_fightEnemyNameKey))
	{
		_fightEnemyName=GET_V_S(_fightEnemyNameKey);
	}
	if(ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerName.length()!=0)
	{
		_fightEnemyName=ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerName;
	}
	CCB_FUNC(this,"mSelfTeamName",CCLabelTTF,setString(ServerDateManager::Get()->getUserBasicInfo().name.c_str()));
	CCB_FUNC(this,"mEnemyTeamName",CCLabelTTF,setString(_fightEnemyName.c_str()));
	CCB_FUNC(this,"mSelfLV",CCLabelBMFont,setString(StringConverter::toString(ServerDateManager::Get()->getUserBasicInfo().level).c_str()));
	CCB_FUNC(this,"mEnemyLV",CCLabelBMFont,setString(StringConverter::toString(ServerDateManager::Get()->getViewTeamBattleInfoItem().mPlayerLevel).c_str()));
	buildTitleItem(SelfTeam);
	mSelfTitleSymbol->setPosition(ccp(mSelfTitleSymbol->getContentSize().width*0.5,mSelfHeight+mTitleHeight+mSelfTitleSymbol->getContentSize().height*0.5f));
	mSelfHeight+=mTitleHeight+mSelfTitleSymbol->getContentSize().height;
	buildDiscipleItem(SelfTeam);
	buildTitleItem(EnemyTeam);
	mEnemyTitleSymbol->setPosition(ccp(mEnemyTitleSymbol->getContentSize().width*0.5,mEnemyHeight+mTitleHeight+mSelfTitleSymbol->getContentSize().height*0.5f));
	mEnemyHeight+=mTitleHeight+mEnemyTitleSymbol->getContentSize().height;
	buildDiscipleItem(EnemyTeam);
}

void ViewListTeamPage::clearAllItem( void )
{
	ContentList::iterator itms = mSelfDiscipleContentList.begin();
	while(itms != mSelfDiscipleContentList.end())
	{
		delete (*itms);
		++itms;
	}
	ContentList memListSelf;
	mSelfDiscipleContentList.swap(memListSelf);

	ContentList::iterator itts = mSelfTitleContentList.begin();
	while(itts != mSelfTitleContentList.end())
	{
		delete (*itts);
		++itts;
	}
	ContentList titleListSelf;
	mSelfTitleContentList.swap(titleListSelf);
	mSelfRootNode->removeAllChildren();

	ContentList::iterator itme = mEnemyDiscipleContentList.begin();
	while(itme != mEnemyDiscipleContentList.end())
	{
		delete (*itme);
		++itme;
	}
	ContentList memListEnemy;
	mEnemyDiscipleContentList.swap(memListEnemy);

	ContentList::iterator itte = mEnemyTitleContentList.begin();
	while(itte != mEnemyTitleContentList.end())
	{
		delete (*itte);
		++itte;
	}
	ContentList titleListEnemy;
	mEnemyTitleContentList.swap(titleListEnemy);
	mEnemyRootNode->removeAllChildren();

}

void ViewListTeamPage::buildDiscipleItem( unsigned int type )
{
	unsigned int battleEnableSize = type==SelfTeam?BattleArrayInfo::Get()->getBattleEnableSize():ServerDateManager::Get()->getViewTeamBattleInfoItem().getBattleArraySize();
	int i = battleEnableSize-1;
	for(; i>=0; --i)
	{
		if(type==SelfTeam)
		{
			const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(i);
			if(info&&(info->discipleId))
			{
				DiscipleHeadContent * item = new DiscipleHeadContent(info->discipleId,type,i);
				item->init();
				addDiscipleItem(type,item,i);
			}
		}
		else
		{
			const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(i);
			if(info&&(info->discipleId))
			{
				DiscipleHeadContent * item = new DiscipleHeadContent(info->discipleId,type,i);
				item->init();
				addDiscipleItem(type,item,i);
			}
		}
	}
}

void ViewListTeamPage::addDiscipleItem(unsigned int type, ContentBase* item ,unsigned int index)
{
	if(type==SelfTeam)
	{
		mSelfDiscipleContentList.push_back(item);
		mSelfRootNode->addChild(item->getContainerNode());
		CCPoint pos = ccp(0,mSelfHeight+item->getContainerNode()->getContentSize().height*(mSelfDiscipleContentList.size()-1));
		item->getContainerNode()->setPosition(pos);
		CCSize size = CCSize(mSelfRootNode->getContentSize().width,mSelfHeight+item->getContainerNode()->getContentSize().height*(mSelfDiscipleContentList.size()));
		mSelfRootNode->setContentSize(size);
		mSelfContent->forceRecaculateChildren();
		mSelfContent->setContentOffset(ccp(0, mSelfContent->getViewSize().height - mSelfContent->getContentSize().height*mSelfContent->getScaleY()));
	}
	else
	{
		mEnemyDiscipleContentList.push_back(item);
		mEnemyRootNode->addChild(item->getContainerNode());
		CCPoint pos = ccp(0,mEnemyHeight+item->getContainerNode()->getContentSize().height*(mEnemyDiscipleContentList.size()-1));
		item->getContainerNode()->setPosition(pos);
		CCSize size = CCSize(mEnemyRootNode->getContentSize().width,mEnemyHeight+item->getContainerNode()->getContentSize().height*(mEnemyDiscipleContentList.size()));
		mEnemyRootNode->setContentSize(size);
		mEnemyContent->forceRecaculateChildren();
		mEnemyContent->setContentOffset(ccp(0, mEnemyContent->getViewSize().height - mEnemyContent->getContentSize().height*mEnemyContent->getScaleY()));
	}
}

void ViewListTeamPage::buildTitleItem( unsigned int type )
{
	if(type==SelfTeam)
	{
		TitleInfoListIterator itr = ServerDateManager::Get()->getTitleIterator();
		int i=0;
		while(itr.hasMoreElements())
		{
			TitleInfo* tInfo = itr.getNext();
			TitleContentPage * item = new TitleContentPage(tInfo->id,type);
			item->init();
			addTitleItem(type,item,i);
			i++;
		}
	}
	else
	{
		TitleInfoListIterator itr = ServerDateManager::Get()->getViewTeamBattleInfoItem().getTitleIterator();
		int i=0;
		while(itr.hasMoreElements())
		{
			TitleInfo* tInfo = itr.getNext();
			TitleContentPage * item = new TitleContentPage(tInfo->id,type);
			item->init();
			addTitleItem(type,item,i);
			i++;
		}
	}
}

void ViewListTeamPage::addTitleItem(unsigned int type, ContentBase* item,unsigned int index)
{
	mTitleHeight=item->getContainerNode()->getContentSize().height;
	if(type==SelfTeam)
	{
		mSelfTitleContentList.push_back(item);
		mSelfRootNode->addChild(item->getContainerNode());
		CCPoint pos = ccp(item->getContainerNode()->getContentSize().width*(mSelfTitleContentList.size()-3*((mSelfTitleContentList.size()-1)/3)-1),item->getContainerNode()->getContentSize().height*floorf((mSelfTitleContentList.size()-1)/3));
		item->getContainerNode()->setPosition(pos);
		CCSize size = CCSize(mSelfRootNode->getContentSize().width,item->getContainerNode()->getContentSize().height*floorf((mSelfTitleContentList.size()-1)/3));
		mSelfHeight=size.height;
		mSelfRootNode->setContentSize(size);
		mSelfContent->forceRecaculateChildren();
	}
	else
	{
		mEnemyTitleContentList.push_back(item);
		mEnemyRootNode->addChild(item->getContainerNode());
		CCPoint pos = ccp(item->getContainerNode()->getContentSize().width*(mEnemyTitleContentList.size()-3*((mEnemyTitleContentList.size()-1)/3)-1),item->getContainerNode()->getContentSize().height*floorf((mEnemyTitleContentList.size()-1)/3));
		item->getContainerNode()->setPosition(pos);
		CCSize size = CCSize(mEnemyRootNode->getContentSize().width,item->getContainerNode()->getContentSize().height*floorf((mEnemyTitleContentList.size()-1)/3));
		mEnemyHeight=size.height;
		mEnemyRootNode->setContentSize(size);
		mEnemyContent->forceRecaculateChildren();
	}
}

void ViewListTeamPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ViewListTeamPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onGiveUp")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "ViewListTeamPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void ViewListTeamPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}

void ViewListTeamPage::onReceiveMassage(const GameMessage * message)
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg)
		{
		}
	}
}