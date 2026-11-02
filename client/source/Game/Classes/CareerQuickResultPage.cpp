#include "CareerQuickResultPage.h"
#include "StringConverter.h"
#include "GameMessages.h"
#include "GameMaths.h"
#include "PopupManager.h"
REGISTER_PAGE("CareerQuickResultPage", CareerQuickResultPage);
CareerQuickResultPage::CareerQuickResultPage(void)
{
	mViewScroll = NULL;
}


CareerQuickResultPage::~CareerQuickResultPage(void)
{
}

void CareerQuickResultPage::Enter( MainFrame* )
{
	mViewScroll = dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mlianchuangContent"));
	if (mViewScroll)
	{
		CCB_FUNC(this,"mlianSubBtnTitle",CCLabelBMFont,setString(Language::Get()->getString("@lianTitle").c_str()));
		mViewScroll->setScrollDeaccelRate(0.85f);
		cocos2d::CCNode* node = cocos2d::CCNode::create();
		std::list<std::string>::reverse_iterator itShowText = mShowText.rbegin();
		std::list<std::string>::reverse_iterator itAccidentText = mAccidentText.rbegin();
		std::list<std::pair<std::string, std::string> >::reverse_iterator itPropertyText = mPropertyText.rbegin();
		CCSize size = CCSize(0,0);
		for (int index = mShowText.size(); itShowText!=mShowText.rend();++itShowText,++itAccidentText,++itPropertyText,index--)
		{
			CCBContainer* content = CCBManager::Get()->createAndLoad("lianchuangContent.ccbi");

			std::list<std::string > replaceList;
			replaceList.push_back(StringConverter::toString(index));
			CCB_FUNC(content,"mlianSubTitle",CCLabelBMFont,setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@CareerQuickResultTitleIndex"),&replaceList).c_str()));
			CCB_FUNC(content,"mlianSubMsgn1",CCLabelBMFont,setString(itPropertyText->second.c_str()));
			CCB_FUNC(content,"mlianSubMsgan2",CCLabelBMFont,setString(itPropertyText->first.c_str()));

			std::string outstr;
			int lines;//add by xinghui
			int width=StringConverter::parseInt(VaribleManager::Get()->getSetting("CareerQuickResultDetail"),20);//add by xinghui
			GameMaths::stringAutoReturn(*itShowText,outstr,width,lines);
			CCB_FUNC(content,"mlianSubMsga",CCLabelBMFont,setString(outstr.c_str()));
			CCB_FUNC(content,"mlianSubMsgb",CCLabelBMFont,setString((*itAccidentText).c_str()));

			node->addChild(content);

			content->setPosition(ccp(0, size.height));
			size.height += content->getContentSize().height;
			size.width = content->getContentSize().width;
		}
		
		node->setContentSize(size);
		mViewScroll->setContainer(node);
		mViewScroll->forceRecaculateChildren();
		mViewScroll->setContentSize(size);
		mViewScroll->setContentOffset(ccp(0, mViewScroll->getViewSize().height - mViewScroll->getContentSize().height*mViewScroll->getScaleY()));
	}
	this->mShowText.clear();
	this->mAccidentText.clear();
	this->mPropertyText.clear();
}

void CareerQuickResultPage::Execute( MainFrame* )
{
}

void CareerQuickResultPage::Exit( MainFrame* )
{
	this->mShowText.clear();
	this->mAccidentText.clear();
	this->mPropertyText.clear();
}

void CareerQuickResultPage::load( void )
{
	loadCcbiFile("lianchuang.ccbi");
}

void CareerQuickResultPage::setShowTextList( std::list<std::string> showText )
{
	this->mShowText = showText;
}

void CareerQuickResultPage::setAccidentText( std::list<std::string> accidentText )
{
	this->mAccidentText = accidentText;
}

void CareerQuickResultPage::setTitle( std::string title )
{
	mTitle = title;
}

void CareerQuickResultPage::setPropertyText( std::list<std::pair<std::string, std::string> > propertyText )
{
	mPropertyText = propertyText;
}

void CareerQuickResultPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if (itemName == "onclose")
	{
		PopupManager::Get()->popPage();
		MsgMainFramePopPage msg;
		msg.pageName = "CareerQuickResultPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "CareerQuickResultPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
