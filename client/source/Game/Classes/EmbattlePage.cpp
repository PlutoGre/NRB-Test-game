
#include "stdafx.h"

#include "EmbattlePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "CCBManager.h"
#include "Language.h"
#include "GameMessages.h"
#include "GamePackets.h"

REGISTER_PAGE("EmbattlePage",EmbattlePage);
USING_NS_CC;

void EmbattleContent::refreshContent( void )
{
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mSelect"));
	mButtonLabel = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mButtonLabel"));
	mChangeButton = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mChangeButton"));
	setSellect(false, false);

	const UserBattleInfo* info = ServerDateManager::Get()->getAndCreatUserBattleInfo(mBattleID);
	if(info)
	{
		Disciple disciple(info->discipleId);
		CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
		if(prop)
		{
			prop->initWithFile(disciple.iconPic().c_str());
		}
		CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame"));
		if(frame)
		{
			frame->setTexture(disciple.getFrameNormalSpirte()->getTexture());
			frame->setColor(disciple.getFrameColor());
		}

		CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mName"));
		if(name)
		{
			name->setString(Language::Get()->getString(disciple.name()).c_str());
		}
		CCSprite* quality=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mMemQualityPic"));
		if(quality)
		{
			quality->initWithFile(disciple.getQualityImageFile().c_str());
		}
		CCLabelBMFont* levelName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPLevel"));
		if(levelName)
		{
			levelName->setString(StringConverter::toString(disciple.level()).c_str());
		}
	}
}

void EmbattleContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	//todo
	if(itemName=="onChangePosition")
	{
		mOwnner->swapBattle(this);
	}
	else if(itemName=="onSelected")
	{
		mOwnner->setSellectedContent(this);
	}
}

void EmbattleContent::setSellect( bool sellect, bool change)
{
	if(sellect)
	{
		mSelect->selected();
	}
	else
	{
		mSelect->unselected();
	}

	if(change)
	{
		mButtonLabel->setVisible(true);
		mChangeButton->setVisible(true);
		mSelect->setEnabled(false);
	}
	else
	{
		mButtonLabel->setVisible(false);
		mChangeButton->setVisible(false);
		mSelect->setEnabled(true);
	}
}

EmbattlePage::EmbattlePage(void)
{
	mContentNode = NULL;
	mCurSellectedContent = NULL;
	mContentList.clear();
}

EmbattlePage::~EmbattlePage(void)
{
}

void EmbattlePage::load( void )
{
	loadCcbiFile("Embattle.ccbi");

	mContentNode =  dynamic_cast<cocos2d::CCNode*>(getVariable("mContentNode"));
	mCurSellectedContent = NULL;
	rebuildAllItem();
}

void EmbattlePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onCancle")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "EmbattlePage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onConfirm")
	{
		OPChangeBattlePosition changeBattlePosition;
		ContentList::iterator itr = mContentList.begin();
		while(itr != mContentList.end())
		{
			EmbattleContent* content = dynamic_cast<EmbattleContent*>(*itr);
			PositionId* pID = changeBattlePosition.add_positions();
			pID->set_id(content->getBattleID());
			pID->set_position(content->getContentID());
			++itr;
		}
		for(int i = 0; i<MaxBattleDiscipleNum; ++i)
		{
			const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(i);
			if(info)
			{
				if(!info->discipleId)
				{
					PositionId* pID = changeBattlePosition.add_positions();
					pID->set_id(info->id);
					pID->set_position(info->position);
				}
			}
			else
			{
				break;
			}
		}
		PacketManager::Get()->sendPakcet(OPCODE_CHANGE_BATTLE_POSITION_C,&changeBattlePosition);
	}
}

void EmbattlePage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void EmbattlePage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	mContentNode->removeAllChildren();
}

void EmbattlePage::buildItem( void )
{
	for(int i = MaxBattleDiscipleNum-1; i>=0; --i)
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(i);
		if(info && info->discipleId !=0 )
		{
			EmbattleContent * item = new EmbattleContent(i, this);
			item->setBattleID(info->id);
			item->init();
			addItem(item);
		}
	}
}

void EmbattlePage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	mContentNode->addChild(item->getContainerNode());

	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1)+20.0f);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size()+20.0f);
	mContentNode->setContentSize(size);
}

void EmbattlePage::setSellectedContent( EmbattleContent* member )
{
	if(!mCurSellectedContent)
	{
		mCurSellectedContent = member;
		member->setSellect(true, false);
		ContentList::iterator itr = mContentList.begin();
		while(itr != mContentList.end())
		{
			if((*itr) != member)
			{
				(dynamic_cast<EmbattleContent*>(*itr))->setSellect(false, true);
			}
			++itr;
		}
	}
	else if(member == mCurSellectedContent)
	{
		mCurSellectedContent = NULL;
		ContentList::iterator itr = mContentList.begin();
		while(itr != mContentList.end())
		{
// 			member->setSellect(false, false);
// 			if((*itr) != member)
// 			{
// 				(dynamic_cast<EmbattleContent*>(*itr))->setSellect(false, false);
// 			}
			(dynamic_cast<EmbattleContent*>(*itr))->setSellect(false, false);
			++itr;
		}
	}
}

void EmbattlePage::swapBattle( EmbattleContent* member )
{
	if(member != mCurSellectedContent)
	{
		unsigned int id = member->getBattleID();
		member->setBattleID(mCurSellectedContent->getBattleID());
		member->refreshContent();
		mCurSellectedContent->setBattleID(id);
		mCurSellectedContent->refreshContent();
		setSellectedContent(mCurSellectedContent);

	}

	
}

void EmbattlePage::unload( void )
{
	clearAllItem();
	CustomPage::unload();
	mContentNode =  NULL;
	mCurSellectedContent = NULL;
}
