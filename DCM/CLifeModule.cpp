#include "pch.h"
#include "CLifeModule.h"
#include"CDCMEngine.h"
#include "obj.h"
#include"CTaskScript.h"
#include"CWayfind.h"

extern CDCMEngine* g_pEngine;

CLifeModule::CLifeModule(CTaskScript* pTaskScript)
{
	m_pTool = pTaskScript;
	m_pDm = m_pTool->m_pTaskDm;
}
CLifeModule::~CLifeModule()
{

}



void CLifeModule::Tset()
{
	while (true)
	{
		//SLEEPEX(5000);
		BackMainScreen();
	}
}

bool CLifeModule::BackMainScreen()
{
	//检查窗口位置
	long x1 = NULL, y1 = NULL, x2 = NULL, y2 = NULL;
	m_pTool->m_pDm->GetWindowRect((long)m_pTool->m_pWnd->hWnd, &x1, &y1, &x2, &y2);
	if (x1 != NULL && y1 != NULL) {
		m_pTool->m_pDm->MoveWindow((long)m_pTool->m_pWnd->hWnd, 0, 0);
		m_pTool->SleepEx(500);
	}

	//检查窗口是否存在
	if (!IsWindow(m_pTool->m_pWnd->hWnd)) {
		LogD(_T("窗口丢失 重新启动"));
		m_pTool->Restart();
	}
	//检查是否掉线
	if (m_pTool->Find(_T("网络中断"))) {
		LogD(_T("网络中断 重新启动"));
		m_pTool->Restart();
	}

	constexpr auto Max = 10;
	for (size_t i = 0; i < Max; i++) {
		CleanScreen();
		if (FIND(_T("主界面_图标_综合副本")) && FIND(_T("主界面_图标_游戏商城"))) {
			return true;
		}
		m_pTool->KeyPress(VK_ESC);
		m_pTool->SleepEx(500);
	}

	return false;
}
void CLifeModule::CleanScreen()
{
	bool IsAgain = false;

	for (size_t i = 0; i < 10; i++)
	{
		IsAgain = false;
		
		if (m_pTool->FindClick(_T("清除_叉号"))) {
			IsAgain = true;
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(300);
		}
		if (m_pTool->FindClick(_T("清除_叉号1"))) {
			IsAgain = true;
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(300);

		}
		if (m_pTool->FindClick(_T("清除_叉号2"))) {
			IsAgain = true;
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(300);
		}

		if (!IsAgain)
			break;
	}
}

bool CLifeModule::MaintenanceTool(CWayFind* pWayFind, CPoint MaintenanceCoordinate)
{
	BackMainScreen();
	if(!FIND(_T("装备需要修理")))
		return true;


	if (!MaintenanceCoordinate.x)
		return false;

	constexpr auto Max = 3;
	bool isDone = false;
	for (size_t i = 0; i < Max; i++)
	{
		int iWayRet = pWayFind->WayFindStart(MaintenanceCoordinate);
		if (iWayRet == CWayFind::STATE_Done) {
			isDone = true;	break;
		}
		else if (iWayRet == CWayFind::STATE_CDT_Useless) {
			pWayFind->detachmentStuck();
		}
		else if (iWayRet == CWayFind::STATE_Movement_timeout) {
			//传送
		}

		//检测怪物攻击或者挡住
		if (checkMonster()) {
			ClickRectangleA();
			CleanMonster();
		}
	}

	if (!isDone)
		return false;

	pWayFind->waitMovStop(3000);
	CPoint MaintenanceLogo{ NULL,NULL };
	if (m_pTool->Find(_T("修理工文字提示图标"), MaintenanceLogo)) {
		CLICK(CPoint(MaintenanceLogo.x + 10, MaintenanceLogo.y + 100));
	}
	else {
		return false;
	}

	if (!WOCRFINDCLICK(_T("修理道具_生活工具_修理佩戴装备"))) {
		return false;
	}
	if (!WOCRFINDCLICK(_T("修理道具_生活工具_批量维修"))) {
		return false;
	}
	if (!WOCRFINDCLICK(_T("修理道具_生活工具_批量维修_确定"))) {
		return false;
	}

	return true;
}

bool CLifeModule::checkMonster(bool disregardLifeAttack)
{
	//return false;

	int tagNumerical = _ttoi(OCR(_T("血量数值范围")));
	SLEEPEX(500);
	int tagNumerical2 = _ttoi(OCR(_T("血量数值范围")));
	if (tagNumerical2 < tagNumerical) {
		LogD(_T("被怪物攻击tag血量:%d , 当前血量%d"), tagNumerical, tagNumerical2);
		if (disregardLifeAttack) {
			return false;
		}
		return true;
	}

	return false;
}

void CLifeModule::ClickRectangleA()
{
	CLICK(CPoint(556, 325));
	SLEEPEX(300);
	CLICK(CPoint(750, 341));
	SLEEPEX(300);
	CLICK(CPoint(728, 471));
	SLEEPEX(300);
	CLICK(CPoint(540, 461));
	SLEEPEX(300);
}

void CLifeModule::CleanMonster()
{
	struct Skills
	{
		int Skills_VK = NULL;
		CRect Skills_Find_Rect = CRect(NULL, NULL, NULL, NULL);
	};

	CString list = _T("81&444,716,470,734|87&479,717,504,734|69&513,718,539,736|82&549,719,573,734|65&459,751,487,769|83&495,753,521,767|68&525,750,556,768|70&562,749,589,769");
	CStringArray  arrStrlist;
	Split(list, arrStrlist, _T("|"));
	std::vector<Skills> vSkills{};
	for (size_t i = 0; i < arrStrlist.GetCount(); i++)
	{
		Skills tmpSkills;
		CStringArray tmpArr;
		Split(arrStrlist[i], tmpArr, _T("&"));
		tmpSkills.Skills_VK = _ttoi(tmpArr[0]);

		CStringArray tmpArr1;
		Split(arrStrlist[i], tmpArr1, _T(","));
		tmpSkills.Skills_Find_Rect = CRect(_ttoi(tmpArr1[0]), _ttoi(tmpArr1[1]), _ttoi(tmpArr1[2]), _ttoi(tmpArr1[3]));

		vSkills.push_back(tmpSkills);
	}

	int tagTime = m_pTool->GetTimeSeconds();
	while (true)
	{
		CPoint retpMS = m_pTool->FindMS(_T("血条10|血条30|血条20"));
		if (!retpMS.x)
			return

			LogD(_T("发现怪物攻击它"));
		if (FIND(_T("生活状态"))) {
			m_pTool->KeyPress(VK_B);
			SLEEPEX(1500);
		}

		for (auto it = vSkills.cbegin(); it != vSkills.cend(); it++)
		{
			CString strOcr = OCR(it->Skills_Find_Rect);
			if (!strOcr) {
				m_pTool->DLLMoveTo(retpMS.x, retpMS.y);
				SLEEPEX(100);
				m_pTool->KeyPress(it->Skills_VK);
			}
			break;
		}
		m_pTool->SleepEx(500);

		if (m_pTool->GetTimeSeconds() - tagTime >= 5000) {
			break;
		}

	}

}

bool CLifeModule::checkDeath()
{
	if (FIND(_T("已死亡"))) {
		LogD(_T("角色已死亡"));
		WOCRFINDCLICK(_T("据点复活"));
		return true;
	}
}

bool CLifeModule::checkPhysicalStrength() {

	int tagTimr = m_pTool->GetTimeSeconds();

	while (true)
	{
		BackMainScreen();
		if (!FIND(_T("生活状态")))
			m_pTool->KeyPress(VK_B);

		if (WFIND(_T("生活状态"))) {
			SLEEPEX(1000);
			if (WFIND(_T("生活体力值还有"))) {
				return true;
			}
			else{
				return false;
			}
		}

		if ((m_pTool->GetTimeSeconds() - tagTimr) >= 20000) {
			return  false;
		}
			
	}

	return  false;
}
