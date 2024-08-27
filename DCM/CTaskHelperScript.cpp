#include "pch.h"
#include "CTaskHelperScript.h"


CTaskHelperScript::CTaskHelperScript(tagWndInfo* pWnd) : CTaskThread(pWnd)
{
	
}

CTaskHelperScript::~CTaskHelperScript()
{
}

bool CTaskHelperScript::OnLoop()
{
	SleepEx(1000);
	//Sleep(1000);

	return true;
}