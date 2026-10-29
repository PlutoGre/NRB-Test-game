#pragma once
#include "AdventureBase.h"
#define SkillBookMaxNum 6
class SkillBookPage
	:public AdventureBase
{
public:
	SkillBookPage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=false;
	};
	virtual ~SkillBookPage(){ mShowList.clear();};
	virtual void refreshContent(void);
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void updatePage(bool isShow,float dt=0.0f); 
	void showSuccessDialog();
	void exitPage();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "SkillBook.ccbi";
		return ccbiFileName;
	}
	//cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
	void refreshPage();
private:	
	typedef std::map<unsigned int,unsigned int> SkillBookShowList;
	SkillBookShowList mShowList;
	cocos2d::CCSprite* mSkillPic;
	cocos2d::CCLabelBMFont* mSkillName;
	cocos2d::CCLabelBMFont* mSkillCount;
	const AdventureItemInfo* mAdventureItemInfo;
	const SkillItem* mSkill;
	SkillBookTable* mSkillTable;
	unsigned int mSkillBookCount;
	bool mIsShow;
	bool checkSkillPartEnough();
	inline unsigned int getSkillPartCountByPartId(unsigned int index);
};
