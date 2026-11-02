#ifndef __MOVELABEL_H__
#define __MOVELABEL_H__
#include "cocos2d.h"
#include "ServerDateManager.h"
#include "Language.h"
USING_NS_CC;
struct MoveBoardCastLabel:public CCNode
{
	MoveBoardCastLabel();
	~MoveBoardCastLabel();
	void init(int x,int y,int width,int height);
	void initStr(CCSize dimensions,CCTextAlignment hAlignment,const char* fontName,int fontSize);
	void run();
	void update();
	int getRows(const char* str);
	
	CCRect mRect;
	CCLabelTTF* mLabelFirst;
	CCLabelTTF* mLabelSecond;
	CCSize mDimensionsFirst;
	CCSize mDimensionsSecond;
	CCTextAlignment mAlignment;
	const char* mFontName;
	int mFontSize;

	CCPoint mPosition;
	BoardCastInfo* mBoardCastInfo;
	float mStartFirstX,mStartSecondX;
	void initFirstLabel();
	void initSecondLabel();

	void changeFirstLabel();
	void changeSecondLabel();
	void getNextMsg();
	void getCurrMsgInfo();
	unsigned int mCurrIndex;
	std::string msg;
	bool mIsHaveBoardCast;
};
#endif