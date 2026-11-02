#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"

#include <string>
#include <map>

class CCBContainer : public cocos2d::CCNode
	, public cocos2d::extension::CCBSelectorResolver
	, public cocos2d::extension::CCBMemberVariableAssigner
{
private:
	static unsigned long containterCount;
public:
	class CCBContainerListener
	{
	public:
		virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};
		virtual void onAnimationDone(const std::string& animationName){};
	};

	CREATE_FUNC(CCBContainer);

	CCBContainer(void);
	CCBContainer(CCBContainerListener*, int tag = 0);
	virtual ~CCBContainer(void);

	virtual void onAnimationDone(const std::string& animationName){};
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender){};
	virtual void unload();
	virtual bool getLoaded();

	virtual void loadCcbiFile(const std::string& filename, bool froceLoad = false);
	void runAnimation(const std::string& actionname);
	cocos2d::CCObject* getVariable(const std::string& variablename);
	/**
	 * 按 CCB 回调名称查找菜单项，不改变节点的所有权。
	 * @param actionName CCB 中绑定的回调名称。
	 * @return 当前容器中的菜单项；未找到时返回 NULL。
	 */
	cocos2d::CCMenuItem* getMenuItemByAction(const std::string& actionName) const;
	void setListener(CCBContainerListener* listener, int tag = 0);

	virtual bool init();

	virtual std::string dumpInfo();

	friend class cocos2d::extension::CCNodeLoader;
	friend class cocos2d::extension::CCBAnimationManager;
private:
	//Do NOT use the fellow functions
	/** use this to cancel the selector method*/
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, const char* pSelectorName){return NULL;}

	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject* pTarget, const char* pMemberVariableName, cocos2d::CCNode* pNode) ;
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelectorWithSender(cocos2d::CCObject * pTarget, const char* pSelectorName, cocos2d::CCNode* sender);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, const char* pSelectorName){return NULL;}
	void _animationDone();
	void _menuItemClicked(cocos2d::CCObject * pSender);
private:
	cocos2d::extension::CCBAnimationManager *mActionManager;
	typedef std::map<std::string, cocos2d::CCObject*> VARIABLE_MAP;
	VARIABLE_MAP mVariables;

	typedef std::map<cocos2d::CCObject*, std::string> MENUITEM_MAP;
	MENUITEM_MAP mMenus;

	CCBContainerListener* mCCBContainerListener;

	int mCCBTag;
	std::string mLoadedCCBFile;
};

/**
example : 
CCBContainer *page = some_ccb_file;
CCB_FUNC(page,"mSkillBig",CCNode,removeAllChildren());
*/
#define CCB_FUNC(_ccb_,_var_,_type_,_func_) { \
	CCObject* __obj__ = _ccb_->getVariable(_var_); \
	_type_* __node__ = __obj__?dynamic_cast<_type_*>(__obj__):0; \
	if(__node__) __node__-> _func_;}

#define CCB_FUNC_R(_ccb_,_var_,_type_,_func_) { \
	CCObject* __obj__ = _ccb_.getVariable(_var_); \
	_type_* __node__ = __obj__?dynamic_cast<_type_*>(__obj__):0; \
	if(__node__) __node__-> _func_;}

#define CCB_FUNC_RETURN(_ccb_,_var_,_type_,_func_,_ret_) { \
	CCObject* __obj__ = _ccb_->getVariable(_var_); \
	_type_* __node__ = __obj__?dynamic_cast<_type_*>(__obj__):0; \
	if(__node__) _ret_ = __node__-> _func_;}

