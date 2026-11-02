
#include "stdafx.h"

#include "TeacherTeachResultPage.h"
#include "AdventurePage.h"
#include "DataTableManager.h"
#include "Disciple.h"
REGISTER_PAGE("TeacherTeachResultPage",TeacherTeachResultPage);
USING_NS_CC;

void DiscipleAddExpContent::refreshContent(void)
{
	const DiscipleExpAddedInfo* info = ServerDateManager::Get()->getDiscipleExpAddedInfoByID(mID);
	Disciple disciple(info->discipleId);
	cocos2d::CCObject* faceObj = getObjectByVariableName("mFace",mContentContainer);
	cocos2d::CCSprite* facePic = dynamic_cast<cocos2d::CCSprite* >(faceObj);
	facePic->initWithFile(disciple.iconPic().c_str());

	cocos2d::CCObject* outLineObj = getObjectByVariableName("mOutLine",mContentContainer);
	cocos2d::CCSprite* OutLinePic = dynamic_cast<cocos2d::CCSprite* >(outLineObj);
	//OutLinePic->initWithFile(disciple.getQualityFrameNormalImageFile().c_str());
	OutLinePic->setTexture(disciple.getFrameNormalSpirte()->getTexture());
	OutLinePic->setColor(disciple.getFrameColor());

	cocos2d::CCObject* nameObj = getObjectByVariableName("mName",mContentContainer);
	cocos2d::CCLabelBMFont* nameLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(nameObj);
	nameLabel->setString(disciple.name().c_str());

	cocos2d::CCObject* addExpObj = getObjectByVariableName("mAddExp",mContentContainer);
	cocos2d::CCLabelBMFont* addExpLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(addExpObj);
	std::string addExpStr = "+"+StringConverter::toString(info->exp);
	addExpLabel->setString(addExpStr.c_str());

	cocos2d::CCObject* extraExpNodeObj = getObjectByVariableName("mExtraExpNode",mContentContainer);
	cocos2d::CCNode* extraExpNode = dynamic_cast<cocos2d::CCNode* >(extraExpNodeObj);
	if(info->extraExpAdded>0)
	{
		cocos2d::CCObject* extraExpObj = getObjectByVariableName("mAddExtraExp",mContentContainer);
		cocos2d::CCLabelBMFont* extraExpLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(extraExpObj);
		std::string extraExpStr = "+"+StringConverter::toString(info->extraExpAdded);
		extraExpLabel->setString(extraExpStr.c_str());
	}else
	{
		extraExpNode->setVisible(false);
	}

	cocos2d::CCObject* upLvNodeObj = getObjectByVariableName("mUpLvNode",mContentContainer);
	cocos2d::CCNode* upLvNode = dynamic_cast<cocos2d::CCNode* >(upLvNodeObj);
	if(info->level>info->preLevel)
	{
		cocos2d::CCObject* preLvObj = getObjectByVariableName("mPreLv",mContentContainer);
		cocos2d::CCLabelBMFont* preLvLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(preLvObj);
		preLvLabel->setString(StringConverter::toString(info->preLevel).c_str());

		cocos2d::CCObject* nowLvObj = getObjectByVariableName("mNowLv",mContentContainer);
		cocos2d::CCLabelBMFont* nowLvLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(nowLvObj);
		nowLvLabel->setString(StringConverter::toString(info->level).c_str());
	}else
	{
		upLvNode->setVisible(false);
	}
}
cocos2d::CCObject * DiscipleAddExpContent::getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer)
{
	cocos2d::CCObject* obj = ccbContainer->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

TeacherTeachResultPage::TeacherTeachResultPage(void)
{
}


TeacherTeachResultPage::~TeacherTeachResultPage(void)
{
}

void TeacherTeachResultPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_ADVENTURE_TEACHEXPINIT,this);
	initPage();
}

void TeacherTeachResultPage::Execute( MainFrame* frame)
{
	
}

void TeacherTeachResultPage::Exit( MainFrame* frame)
{
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
}

void TeacherTeachResultPage::load( void )
{
	loadCcbiFile("TeachResultPage.ccbi");
}

void TeacherTeachResultPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "TeacherTeachResultPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void TeacherTeachResultPage::initPage()
{
	CCObject* teacherPicObj=getObjectByVariableName("mTeacherPicSprite",this);
	mTeacherPicSprite = dynamic_cast<cocos2d::CCSprite*>(teacherPicObj);

	CCObject* disciplesPosObj=getObjectByVariableName("mDisciplesPosNode",this);
	mDisciplesPosNode = dynamic_cast<cocos2d::CCNode*>(disciplesPosObj);
}

cocos2d::CCObject * TeacherTeachResultPage::getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer)
{
	cocos2d::CCObject* obj = ccbContainer->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

void TeacherTeachResultPage::setPageByServerData(const unsigned int adventureId)
{
	const AdventureTeacherInfo* adventureTeacherinfo = ServerDateManager::Get()->getAdventureTeacherInfoByID(adventureId);
	if(adventureTeacherinfo!=NULL)
	{
		const AdventureTeacherItem* teacherItem	= AdventureTeacherTableManager::Get()->getAdventureTeacherByID(adventureTeacherinfo->teacherId);
		if(teacherItem!=NULL)
		{
			mTeacherPicSprite->initWithFile((teacherItem->iconPic).c_str());
		}
	}
	clearAllItem();
	buildDisciplesNodeItem();
	ServerDateManager::Get()->removeAllDiscipleExpAddedInfo();
	MsgAdventureRemoveItem msg;
	msg.index = adventureId;
	MessageManager::Get()->sendMessage(&msg);
}
void TeacherTeachResultPage::buildDisciplesNodeItem()
{
	DiscipleExpAddedInfoMapIterator itr = ServerDateManager::Get()->getDiscipleExpAddedIterator();
	while(itr.hasMoreElements())
	{
		unsigned int id = itr.getNext()->id;
		DiscipleAddExpContent * item = new DiscipleAddExpContent(id);
		item->init();
		addItem(item);
		
		/*if(index<2)
		{
			size.width = size.width+discipleAddExpIcon->getContentSize().width;
		}
		if(index%2==0)
		{
			size.height =size.height+discipleAddExpIcon->getContentSize().height;
		}
		float xPos = 0,yPos=0;
		size.height = discipleAddExpIcon->getContentSize().height;
		xPos = (index%2)*(discipleAddExpIcon->getContentSize().width+10);
		yPos = -((int)(index/2))*(discipleAddExpIcon->getContentSize().height);
		mDisciplesPosNode->addChild(discipleAddExpIcon);
		discipleAddExpIcon->setPosition(xPos,yPos);
		index++;*/
	}
	//mDisciplesPosNode->setContentSize(size);
}
void TeacherTeachResultPage::addItem(ContentBase* item)
{
	mContentList.push_back(item);
	mDisciplesPosNode->addChild(item->getContainerNode());
	float xPos = 0.0f,yPos=0.0f;
	xPos = ((mContentList.size()-1)%2)*(item->getContainerNode()->getContentSize().width);
	yPos = -((int)((mContentList.size()-1)/2)+1)*(item->getContainerNode()->getContentSize().height);
	item->getContainerNode()->setPosition(xPos,yPos);
}
void TeacherTeachResultPage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	mDisciplesPosNode->removeAllChildren();
}
void TeacherTeachResultPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_ADVENTURE_TEACHEXPINIT)
	{
		const MsgAdventureTeachExpDataInit * msg = dynamic_cast<const MsgAdventureTeachExpDataInit*>(message);
		setPageByServerData(msg->index);
	}
}
