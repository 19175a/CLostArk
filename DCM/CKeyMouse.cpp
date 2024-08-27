#include "pch.h"
#include "CKeyMouse.h"

CKeyMouse::CKeyMouse(CString strPath)
{
	m_DLLInfo = LoadLibrary(strPath);
	if (m_DLLInfo == nullptr) {
		ASSERT(false);
		LogE(_T("!!!!!!!CKeyMouse"));
	}


}
CKeyMouse::~CKeyMouse()
{
	
}
CKeyMouse::CKeyMouse()
{

}


void CKeyMouse::End()
{
	FreeLibrary(m_DLLInfo);
	typedef  char* (_stdcall* _End)();
	_End DLLRightClick = (_End)GetProcAddress(m_DLLInfo, "End");
	_End();
}

void CKeyMouse::MoveTo(int x, int y)
{
	typedef  char* (_stdcall* _MoveTo)(int, int);
	_MoveTo DLLMoveTo = (_MoveTo)GetProcAddress(m_DLLInfo, "MoveTo");
	DLLMoveTo(x, y);
}

void CKeyMouse::RightClick()
{
	typedef  char* (_stdcall* _RightClick)();
	_RightClick DLLRightClick = (_RightClick)GetProcAddress(m_DLLInfo, "RightClick");
	DLLRightClick();
}
void CKeyMouse::LeftClick()
{
	typedef  char* (_stdcall* _LeftClick)();
	_LeftClick DLLeftClick = (_LeftClick)GetProcAddress(m_DLLInfo, "LeftClick");
	DLLeftClick();
}
void CKeyMouse::KeyPress(int Encodi)
{
	typedef  char* (_stdcall* _keyboard)(int);
	_keyboard  DLLkeyboard = (_keyboard)GetProcAddress(m_DLLInfo, "KeyPress");
	DLLkeyboard(Encodi);
}

//×ó¼ü
void CKeyMouse::LeftDown()
{
	typedef  char* (_stdcall* _LeftDown)();
	_LeftDown DLLLeftDown = (_LeftDown)GetProcAddress(m_DLLInfo, "LeftDown");
	DLLLeftDown();
}

void CKeyMouse::LeftUp()
{
	typedef  char* (_stdcall* _LeftUp)();
	_LeftUp DLLLLeftUp = (_LeftUp)GetProcAddress(m_DLLInfo, "LeftUp");
	DLLLLeftUp();
}

//ÓÒ¼ü
void CKeyMouse::RightDown()
{
	typedef  char* (_stdcall* _RightDown)();
	_RightDown DLLRightDown = (_RightDown)GetProcAddress(m_DLLInfo, "RightDown");
	DLLRightDown();
}

void CKeyMouse::RightUp()
{
	typedef  char* (_stdcall* _RightUp)();
	_RightUp DLLRightUp = (_RightUp)GetProcAddress(m_DLLInfo, "RightUp");
	DLLRightUp();
}


