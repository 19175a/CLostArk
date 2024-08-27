#pragma once
#include "CTaskThread.h"
class CWayFind;
class CByPost;
class CGameTeleport;
class CLifeModule;
class CLife_Collection;
struct tagGameStatus;
class CLife_ToolMaintenance;
class CLife_Fish;
class CDCMDlg;
class CUpgrade;

#define FIND(strUi)(m_pTool->Find(strUi))
#define FINDPOINT(strUi,ppoint)(m_pTool->Find(strUi,ppoint))
#define WFIND(strUi)(m_pTool->WFind(strUi))
#define WFINDTIME(strUi,Itime)(m_pTool->WFind(strUi,Itime))
#define FINDCILICK(strUi)(m_pTool-> FindClick(strUi))
#define KEYPRESS(vk)(m_pTool-> KeyPress(vk))
#define SLEEPEX(iTime)(m_pTool->SleepEx(iTime))
#define OCR(ocrUi)(m_pTool->Ocr(ocrUi))
#define CLICK(Point)(m_pTool->Click(Point))
#define RCLICK(Point)(m_pTool->RClick(Point))
#define WOCRFINDCLICK(strUI)(m_pTool->WOcrFindCick(strUI))
#define WOCR(strUI)(m_pTool->WOcrFind(strUI))

#define VK_ESC 27
#define VK_B   66
#define VK_G   71
#define VK_M   77
#define VK_I   73
#define VK_L   76
#define VK_E   69

class CTaskScript : public CTaskThread
{
public:
	CTaskScript(tagWndInfo* pWnd);
	~CTaskScript();
	CString ITostr(int Istay);
protected:
	bool OnLoop();
	void TsetOnLoop();
	void Init();
	void checkGemaState();
	BOOL KillProcess(DWORD ProcessId);

protected:
	void RegTaskFun(CString strName, FunType fun);
	CString RunTaskFun(CString strName, CString strParam);

public:
	CByPost*				m_pByPost;
	tagWndInfo*				m_pWnd;
	dmsoft*					m_pTaskDm;
	tagGameStatus*			m_pStatus;
	CMapFunc				m_mapFunc;
	int Tsd;
public:
	//控制状态
	void Restart(); 
	void Switch();
	void NoSwitch();


private:
	bool TsetOunLL = false;
//TaskHtttp通讯 相关
	tagTeam* m_pTeam;
	CLock	 m_lkHtttpCmd;
	//CLifeModule* m_pLifeModule = NULL;
public:
	//Life
	CLife_Fish* m_pLife_Fish = NULL;
	CLife_Collection* m_pLife_Collection = NULL;
	//Upgrade::CMobileMapPath* m_pMobileMapPath = NULL;
	CUpgrade* m_pUpgrade = NULL;
};

class class1 {

};