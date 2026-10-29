
#include "stdafx.h"

#include "TranExpChooseMemberPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "MessageBoxPage.h"

REGISTER_PAGE("TranExpChooseMemberPage",TranExpChooseMemberPage);
USING_NS_CC;

void MemberContent::refreshContent( void )
{
	mSelect = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mEnhance"));
	mSelect->unselected();
	Disciple disciple(mID);
	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(disciple.iconPic().c_str());
	}
	CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFace"));
	if(frame)
	{
		frame->setColor(disciple.getFrameColor());
	}
	CCLabelBMFont* name=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPName"));
	if(name)
	{
		name->setString(Language::Get()->getString(disciple.name()).c_str());
#if defined(Macro_TestUseCCReViScrollViewFacade) && defined(_DEBUG)
		std::string strTmp = StringConverter::toString(this->m_iIdx) + Language::Get()->getString(disciple.name());
		name->setString(strTmp.c_str());
#endif
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
	CCLabelBMFont* valueKey=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mMPValueValue"));
	if(valueKey)
	{
		valueKey->setString(StringConverter::toString(disciple.getDiscipleValue()).c_str());
	}

#ifdef Macro_TestUseCCReViScrollViewFacade
	setSellect(m_pItemData->m_bSelected);
#endif

}

void MemberContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	//todo
	if(itemName=="onEnhance")
	{
		mOwnner->setSellectedContent(this);
	}

}

void MemberContent::setSellect( bool sellect )
{
	if(sellect)
	{
		mSelect->selected();
	}
	else
	{
		mSelect->unselected();
	}
}

#ifdef Macro_TestUseCCReViScrollViewFacade

void MemberContent::initItemView()
{
	mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
	mContentContainer->setListener(this);
	mContentContainer->retain();
	//
	this->m_pItemNode = mContentContainer;
}

void MemberContent::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	MemberContentItemData* pItemData = static_cast<MemberContentItemData*>(pItem);
	if (pItemData)
	{
		this->m_pItemData = pItemData;
		this->mID = pItemData->m_iMemberID;
#ifdef _DEBUG
		this->m_iIdx = pItemData->m_iIdx;
#endif
		this->refreshContent();
	}
}

#endif

TranExpChooseMemberPage::TranExpChooseMemberPage(void)
{
	membersRootNode = NULL;
	mCurSellectedContent = NULL;
	members = NULL;
	mContentList.clear();
	isDesOrder=false;
}

TranExpChooseMemberPage::~TranExpChooseMemberPage(void)
{
}

void TranExpChooseMemberPage::Enter( MainFrame* frame)
{
	isDesOrder=false;
	rebuildAllItem();

	mCurSellectedContent = NULL;
	unsigned int transferExpProvider = BlackBoard::Get()->TransferExp_Provider;
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		MemberContent* content = dynamic_cast<MemberContent* >(*itr);
		if(content->getContentID() == transferExpProvider)
		{
			mCurSellectedContent = content;
#ifdef Macro_TestUseCCReViScrollViewFacade
			mCurSellectedContent->m_pItemData->m_bSelected = true;
#endif
			mCurSellectedContent->setSellect(true);
			break;
		}
		++itr;
	}

	refreshPage();
}

void TranExpChooseMemberPage::Exit( MainFrame* frame)
{
	clearAllItem();
#ifdef Macro_TestUseCCReViScrollViewFacade
	delete m_pScrollViewFacade;
	m_pScrollViewFacade = NULL;
#endif
	unload();
	//
	membersRootNode = NULL;
	mTotoalExp = NULL;
	mExpChoosed = NULL;
	mExpNeeded = NULL;
}

void TranExpChooseMemberPage::Execute( MainFrame* )
{

}

void TranExpChooseMemberPage::load( void )
{
	loadCcbiFile("TranExpChooseMember.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mChooseMember"));
	membersRootNode = members->getContainer();

	mTotoalExp = dynamic_cast<CCLabelBMFont* >(getVariable("mTotoalExp"));
	mExpChoosed = dynamic_cast<CCLabelBMFont* >(getVariable("mExpChoosed"));
	mExpNeeded = dynamic_cast<CCLabelBMFont* >(getVariable("mExpNeeded"));

#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade = new cocos2d::extension::CCReViScrollViewFacade(members);
	m_pScrollViewFacade->init(Macro_FixedViewItemsMaxNum,Macro_FixedViewItemsMaxNum);
#endif

}

bool DiscipleFilter_TranExpChoose(unsigned int id)
{
	//当前接受传功弟子
	if(id == BlackBoard::Get()->TransferExp_Receiver)
		return false;
	Disciple disciple(id);
	if(disciple.level()<=1 || disciple.isInBattle())
		return false;

	return true;
}

void TranExpChooseMemberPage::buildItem( void )
{
// 	UserDiscipleInfoListIterator itr = ServerDateManager::Get()->getDiscipleIterator();
// 	while(itr.hasMoreElements())
// 	{
// 		unsigned int id = itr.getNext()->id;
// 		if(id == BlackBoard::Get()->TransferExp_Receiver)
// 			continue;
// 		Disciple disciple(id);
// 
// 		if(disciple.level()>1 && !disciple.isInBattle())
// 		{
// 			MemberContent * item = new MemberContent(id, this);
// 			item->init();
// 			addItem(item);
// 		}
// 	}
#ifdef Macro_TestUseCCReViScrollViewFacade
	{
		const int iMaxNode = (int)m_pScrollViewFacade->getMaxDynamicControledItemViewsNum();
		int iCount = 0;
		float fOneItemHeight = 0.f;//等高
		float fOneItemWidth = 0.f;
		DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getDiscipleCollection(DiscipleFilter_TranExpChoose);
		if(collect.rbegin() == collect.rend()){
			int fontWidth =members->getViewSize().width; 
			int fontHeight =members->getViewSize().height; 

			CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
			normalBMFontSingle->setString(Language::Get()->getString("@noEnoughEXPDiscipleToChange").c_str());
			normalBMFontSingle->setColor(ccBLACK);
			normalBMFontSingle->setScale(1);
			CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
			normalBMFontSingle->setPosition(pos);
			membersRootNode->addChild(normalBMFontSingle);

			CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
			normalBMFontSingle2->setString(Language::Get()->getString("@noEnoughEXPDiscipleToChange").c_str());
			normalBMFontSingle2->setColor(ccGRAY);
			normalBMFontSingle2->setScale(1);
			CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
			normalBMFontSingle2->setPosition(pos2);
			membersRootNode->addChild(normalBMFontSingle2);
		}
		if (!isDesOrder)
		{
			DiscipleDateCollect::DiscipleIDCollect::reverse_iterator itr = collect.rbegin();
			while(itr != collect.rend())
			{
				//
				MemberContentItemData* pItemData = new MemberContentItemData();
				pItemData->m_iMemberID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				MemberContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new MemberContent(*itr, this);
					pItem->initItemView();

					//pItem->refreshItemView(pItemData);//初始在m_pScrollViewFacade->setDynamicItemsStartPosition设置
					//
					if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
						fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
					if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
						fOneItemWidth = pItem->getContainerNode()->getContentSize().width;
				}
				m_pScrollViewFacade->addItem(pItemData, pItem);
				//
				++itr;
				++iCount;
			}
		}
		else
		{
			DiscipleDateCollect::DiscipleIDCollect::iterator itr = collect.begin();
			while(itr != collect.end())
			{
				//
				MemberContentItemData* pItemData = new MemberContentItemData();
				pItemData->m_iMemberID = *itr;
				pItemData->m_bSelected = false;
				pItemData->m_iIdx = iCount;
				//
				pItemData->m_ptPosition = ccp(0, fOneItemHeight*iCount);
				//
				MemberContent* pItem = NULL;
				if (iCount < iMaxNode)
				{
					//
					pItem = new MemberContent(*itr, this);
					pItem->initItemView();

					//pItem->refreshItemView(pItemData);//初始在m_pScrollViewFacade->setDynamicItemsStartPosition设置
					//
					if (fOneItemHeight < pItem->getContainerNode()->getContentSize().height)
						fOneItemHeight = pItem->getContainerNode()->getContentSize().height;
					if (fOneItemWidth < pItem->getContainerNode()->getContentSize().width)
						fOneItemWidth = pItem->getContainerNode()->getContentSize().width;
				}
				m_pScrollViewFacade->addItem(pItemData, pItem);
				//
				++itr;
				++iCount;
			}
		}
		//
		CCSize size = CCSize(fOneItemWidth, fOneItemHeight*iCount);
		members->setContentSize(size);
		/*
			注意ScrollView里除了受控的DynamicItems，还有其他Items
			ScrollView锚点(0.f,0.f)位于左下角
			height由下向上增长
			width由左向右增长
			下面两句顺序不能颠倒，意思是：最后添加的显示到可视区域左上角
		*/
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
		m_pScrollViewFacade->setDynamicItemsStartPosition(iCount-1);
		{
			//m_pScrollViewFacade->setDynamicItemsStartPosition(0);
		}
		//
	}
#else
	DiscipleDateCollect::DiscipleIDCollect collect = DiscipleDateCollect::Get()->getDiscipleCollection(DiscipleFilter_TranExpChoose);
	DiscipleDateCollect::DiscipleIDCollect::reverse_iterator itr = collect.rbegin(); 
	if(itr == collect.rend()){
		int fontWidth =members->getViewSize().width; 
		int fontHeight =members->getViewSize().height; 

		CCLabelBMFont* normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
		normalBMFontSingle->setString(Language::Get()->getString("@noEnoughEXPDiscipleToChange").c_str());
		normalBMFontSingle->setColor(ccBLACK);
		normalBMFontSingle->setScale(1);
		CCPoint pos = ccp(fontWidth/2+2,-fontHeight/2-2);
		normalBMFontSingle->setPosition(pos);
		membersRootNode->addChild(normalBMFontSingle);

		CCLabelBMFont* normalBMFontSingle2 = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  
		normalBMFontSingle2->setString(Language::Get()->getString("@noEnoughEXPDiscipleToChange").c_str());
		normalBMFontSingle2->setColor(ccGRAY);
		normalBMFontSingle2->setScale(1);
		CCPoint pos2 = ccp(fontWidth/2,-fontHeight/2);
		normalBMFontSingle2->setPosition(pos2);
		membersRootNode->addChild(normalBMFontSingle2);
	}
	for(int i = collect.size() - 1; itr != collect.rend(); ++itr, --i)
	{
		MemberContent * item = new MemberContent((*itr), this);
		item->init();
		addItem(item);
	}
#endif
}

void TranExpChooseMemberPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	//membersRootNode->setContentSize(size);
	members->setContentSize(size);
	members->setContentOffset(ccp(0,members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void TranExpChooseMemberPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void TranExpChooseMemberPage::clearAllItem( void )
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	m_pScrollViewFacade->clearAllItems();
#endif
	//
	if(mContentList.size()>0)
	{
		ContentList::iterator itr = mContentList.begin();
		while(itr != mContentList.end())
		{
			delete (*itr);
			++itr;
		}
		ContentList list;
		mContentList.swap(list);
	}
	membersRootNode->removeAllChildren();
	mCurSellectedContent = NULL;
}

void TranExpChooseMemberPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void TranExpChooseMemberPage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
		}
		++itr;
	}
}

void TranExpChooseMemberPage::reOrderAllItem()
{
#ifdef Macro_TestUseCCReViScrollViewFacade
	this->rebuildAllItem();
	return;
#endif
	if(mContentList.size()>0)
	{
		membersRootNode->removeAllChildren();
		if(isDesOrder)
		{
			ContentList::iterator itr = mContentList.begin(); 
			for(int i=0; itr != mContentList.end(); ++itr,++i)
			{
				ContentBase * item = *itr;
				membersRootNode->addChild(item->getContainerNode());
				CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * i);
				item->getContainerNode()->setPosition(pos);
			}
		}
		else
		{
			ContentList::reverse_iterator itr = mContentList.rbegin(); 
			for(int i=0; itr != mContentList.rend(); ++itr,++i)
			{
				ContentBase * item = *itr;
				membersRootNode->addChild(item->getContainerNode());
				CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height * i);
				item->getContainerNode()->setPosition(pos);
			}
		}
	}
}

void TranExpChooseMemberPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onConfirm")
	{
		if(mCurSellectedContent == NULL)
		{
			BlackBoard::Get()->TransferExp_Provider = 0;
			MsgMainFrameChangePage msg;
			msg.pageName = "TransferEXPPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		else
		{
			unsigned int transferExpProvider = mCurSellectedContent->getContentID();
			BlackBoard::Get()->TransferExp_Provider = transferExpProvider;
			MsgMainFrameChangePage msg;
			msg.pageName = "TranExpPreviewPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(itemName=="onOrderByDesc")
	{
		isDesOrder = !isDesOrder;
		reOrderAllItem();
		refreshPage();
	}
}

void TranExpChooseMemberPage::refreshPage( void )
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

	Disciple receiverDisciple(BlackBoard::Get()->TransferExp_Receiver);
	if(mExpNeeded)
	{
		mExpNeeded->setString(StringConverter::toString(receiverDisciple.getLevelupNeededExp()).c_str());
	}
	
	unsigned int totalExp = 0;
	UserDiscipleInfoListIterator itr = ServerDateManager::Get()->getDiscipleIterator();
	while(itr.hasMoreElements())
	{
		unsigned int id = itr.getNext()->id;
		if(id == BlackBoard::Get()->TransferExp_Receiver)
			continue;
		Disciple disciple(id);

		if(disciple.level()>1 && !disciple.isInBattle())
		{
			totalExp += disciple.getProviderExp();
		}
	}
	if(mTotoalExp)
		mTotoalExp->setString(StringConverter::toString(totalExp).c_str());

	if(mExpChoosed)
		mExpChoosed->setString(StringConverter::toString(0).c_str());

	CCLabelBMFont* funcLable=dynamic_cast<CCLabelBMFont* >(getVariable("mFuncLabel"));
	if(funcLable)
	{
		if(isDesOrder)
			funcLable->setString(Language::Get()->getString("@OrderByDesc").c_str());
		else
			funcLable->setString(Language::Get()->getString("@OrderByAsce").c_str());
	}
}

void TranExpChooseMemberPage::setSellectedContent( MemberContent* member )
{
	if(mCurSellectedContent)
	{
#ifdef Macro_TestUseCCReViScrollViewFacade
		mCurSellectedContent->m_pItemData->m_bSelected = false;
#endif
		mCurSellectedContent->setSellect(false);
	}

	if(mCurSellectedContent != member)
	{
		mCurSellectedContent = member;
#ifdef Macro_TestUseCCReViScrollViewFacade
		mCurSellectedContent->m_pItemData->m_bSelected = true;
#endif
		mCurSellectedContent->setSellect(true);

		if(mExpChoosed)
		{
			Disciple providerDisciple(mCurSellectedContent->getContentID());
			mExpChoosed->setString(StringConverter::toString(providerDisciple.getProviderExp()).c_str());
		}
	}
	else
	{
		mCurSellectedContent = NULL;
		if(mExpChoosed)
		{
			mExpChoosed->setString(StringConverter::toString(0).c_str());
		}
	}	

}
