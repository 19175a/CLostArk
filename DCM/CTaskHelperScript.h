#pragma once
#include "CTaskThread.h"

class CTaskHelperScript : public CTaskThread
{
public:
	CTaskHelperScript(tagWndInfo* pWnd);
	~CTaskHelperScript();

protected:
	virtual bool OnLoop();
};
