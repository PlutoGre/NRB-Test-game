#pragma once

#include "CBaseNode.h"

//¹Ø¼üÖ¡Êý¾Ý
class CCFrameNode : public CCBaseNode
{
public:
	static CCFrameNode *create();

public:
	CCFrameNode();
	~CCFrameNode(void);

	void		copy(CCBaseNode* _fv);

	std::string toString();

protected:
	void reset();

public:
	float	offR;
	int		frame;
	int		duration;
};

