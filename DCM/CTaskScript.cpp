#include "pch.h"
#include "CTaskScript.h"
#include "CDCMEngine.h"
#include "obj.h"
#include "DCMDlg.h"
#include"CKeyMouse.h"
#include "CGettingCoordinates.h"
#include"CWayFind.h"
#include "CUpgrade.h"
#include "CLife_Collection.h"
#include "CLife_ToolMaintenance.h"
#include "CLife_Fish.h"
#include"CLifeModule.h"
#include"CByPost.h"
extern CDCMEngine* g_pEngine;

CTaskScript::CTaskScript(tagWndInfo* pWnd) : CTaskThread(pWnd)
{
	m_pWnd = pWnd;
	m_pTeam = pWnd->pTaskInfo->pTeam;

}

void CTaskScript::Init()
{

}

CTaskScript::~CTaskScript()
{
	if (m_pLife_Fish)
		delete m_pLife_Fish;

	if (m_pLife_Collection) 
		delete m_pLife_Collection;

	if(m_pByPost)
		delete m_pByPost;

}

void CTaskScript::RegTaskFun(CString strName, FunType fun)
{
	m_mapFunc[strName] = fun;
}

CString CTaskScript::RunTaskFun(CString strName, CString strParam)
{
	if (m_mapFunc[strName] == NULL)
	{
		ASSERT(FALSE);
		return _T("");
	}

	m_pStatus->dwRunTick = GetTickCount();
	m_pStatus->strTime = GetTimeStr();

	return (this->*m_mapFunc[strName])(strParam);
}
bool CTaskScript::OnLoop()
{
	////测试yolo-------------------------
	//long retYolo = LoadAi(g_pEngine->m_strWorkPath + _T("\\res\\datasets\\ai.module"));
	//retYolo = AiYoloSetModel(0, g_pEngine->m_strWorkPath + _T("\\res\\datasets\\yolov5s_best.dmx"), _T("2694843593"));
	//retYolo = AiYoloUseModel(0);
	//CString retStrYolo =  AiYoloDetectObjects(0, 0, 1290, 832, 0.6, 0.45);


	m_pUpgrade = new CUpgrade();
	m_pUpgrade->Init(this);
	m_pUpgrade->started();
	
	int Time = NULL;
	while (true)
	{
		Time++;
		LogD(_T("测试yolo结束 %d"), Time);
		SleepEx(1000);
	}

	//---------------------------------
	LogD(_T("如果您在虚拟机使用脚本，请您用CTRL+ALT快捷键拿出鼠标"));
	LogD(_T("倒计时五秒"));
	SleepEx(5000);
	Click(CPoint(39, 16));
	checkGemaState();
	for (int i = 0; i < g_pEngine->m_pCDlg->m_lstTaskRun.GetItemCount(); i++) {

		int index = NULL;
		CString stTskMapName = _T("");
		int iTaskRet = -1;
		CString strTaskName = g_pEngine->m_pCDlg->m_lstTaskRun.GetItemText(i, 0);
		if (strTaskName == _T("采集")) {
			m_pLife_Collection = new CLife_Collection();
			index =  g_pEngine->m_pCDlg->m_dlg2.m_Life_Collection_ComboBox.GetCurSel();
			g_pEngine->m_pCDlg->m_dlg2.m_Life_Collection_ComboBox.GetLBText(index, stTskMapName);
			if (!m_pLife_Collection->Init(this, stTskMapName)) {
				LogD(_T("Life_Collection 初始化失败"));
				continue;
			}

			iTaskRet = m_pLife_Collection->started();
			LogD(_T("采集返回:%d"), iTaskRet);
			if (m_pLife_Collection) {
				delete m_pLife_Collection;
				m_pLife_Collection = NULL;
			}
			continue;
		}

		if (strTaskName == _T("钓鱼")) {
			m_pLife_Fish = new CLife_Fish();
			index = g_pEngine->m_pCDlg->m_dlg2.m_Life_Fish_ComboBox.GetCurSel();
			g_pEngine->m_pCDlg->m_dlg2.m_Life_Fish_ComboBox.GetLBText(index, stTskMapName);
			if (!m_pLife_Fish->Init(this, stTskMapName)) {
				LogD(_T("Life_Collection 初始化失败"));
				continue;
			}

			iTaskRet = m_pLife_Fish->started();
			LogD(_T("钓鱼返回:%d"), iTaskRet);
			if (m_pLife_Fish) {
				delete m_pLife_Fish;
				m_pLife_Fish = NULL;
			}

			continue;
		}

	}

	for (size_t i = 0; i < 5; i++)
	{
		//邮寄
		LogD(_T("邮寄"));
		CString strText;
		g_pEngine->m_pCDlg->m_dlg3.m_ByPostNameControl.GetWindowText(strText);
		if (strText != _T("")) {
			m_pByPost = new CByPost();
			m_pByPost->Init(this);
			if (m_pByPost->ByPoststarted(strText))
				break;
		}
		else
		{
			break;
		}

		if (m_pByPost)
			delete m_pByPost;
	}

	if (m_pWnd->IsWriteIniLogd) {
		Switch();
	}
	else
	{
		NoSwitch();
	}
	return true;
}

void CTaskScript::Restart()
{
	LogD(_T("结束游戏 等待重启"));
	CString strPid = m_pDm->EnumProcess(_T("LOSTARK.exe"));
	KillProcess(_ttoi(strPid));
	for (size_t i = 0; i < 10; i++)
	{
		SleepEx(1000);

	}
	g_pEngine->m_State = CDCMEngine::TESKSTATE::_SwitchEx;
	g_pEngine->m_strParam = ITostr(m_pWnd->id);


	int Time = NULL;
	while (true)
	{
		Time++;
		LogD(_T("正在结束线程 重启%d"), Time);
		SleepEx(1000);
	}
}
void CTaskScript::Switch()
{
	LogD(_T("结束游戏 切换下个账号"));
	CString strPid = m_pDm->EnumProcess(_T("LOSTARK.exe"));
	KillProcess(_ttoi(strPid));
	for (size_t i = 0; i < 10; i++)
	{
		SleepEx(1000);

	}
	g_pEngine->m_State = CDCMEngine::TESKSTATE::_Switch;
	g_pEngine->m_strParam = ITostr(m_pWnd->id);

	int Time = NULL;
	while (true)
	{
		Time++;
		LogD(_T("正在结束线程 切换下个账号%d"), Time);
		SleepEx(1000);
	}
}
void CTaskScript::NoSwitch()
{
	LogD(_T("结束游戏 不切换下个账号"));
	CString strPid = m_pDm->EnumProcess(_T("LOSTARK.exe"));
	KillProcess(_ttoi(strPid));
	for (size_t i = 0; i < 10; i++)
	{
		SleepEx(1000);

	}
	g_pEngine->m_State = CDCMEngine::TESKSTATE::_SwitchEx;
	g_pEngine->m_strParam = ITostr(m_pWnd->id);

	int Time = NULL;
	while (true)
	{
		Time++;
		LogD(_T("正在结束线程 不切换下个账号%d"), Time);
		SleepEx(1000);
	}
}

void CTaskScript::checkGemaState()
{
	if (Find(_T("主界面_图标_综合副本")) && Find(_T("主界面_图标_游戏商城"))) {
		return;
	}

	//检查是否在登入游戏选择游戏大区状态
	bool Isexistence = false;
	if (Find(_T("游戏登入界面_选择图标"))||Find(_T("游戏开始"))) {
		LogD(_T("游戏登入界面"));

		while (true)
		{
			if (m_pWnd->pTaskInfo->pAccountInfo->strRegion == _T("P服")) {
				if (FindClick(_T("选择大区_P服"))) {
					Isexistence = true;
				}

			}
			else if (m_pWnd->pTaskInfo->pAccountInfo->strRegion == _T("A服")) {
				if (FindClick(_T("选择大区_A服"))) {
					Isexistence = true;
				}
			}
			else if (m_pWnd->pTaskInfo->pAccountInfo->strRegion == _T("K服")) {
				if (FindClick(_T("选择大区_K服"))) {
					Isexistence = true;
				}
			}
			SleepEx(1000);
			FindClick(_T("游戏登入界面_选择图标"));

			Isexistence = false;
			if (WClick(_T("游戏开始"), 10000)) {
				Isexistence = true;
			}

			for (size_t i = 0; i < 10; i++)
			{
				bool IsAgain = false;
				if (FindClick(_T("清除_叉号"))) IsAgain = true;
				if (FindClick(_T("清除_叉号1"))) IsAgain = true;
				if (FindClick(_T("清除_叉号2"))) IsAgain = true;

				if (!IsAgain)
					break;
			}

			if (Find(_T("主界面_图标_综合副本")) && Find(_T("主界面_图标_游戏商城"))) {
				return;
			}
		}
	}





}

BOOL CTaskScript::KillProcess(DWORD ProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessId);
	if (hProcess == NULL)
		return FALSE;
	if (!TerminateProcess(hProcess, 0))
		return FALSE;
	return TRUE;
}


void CTaskScript::TsetOnLoop()
{


}


CString CTaskScript::ITostr(int Istay)
{
	CString str;
	str.Format(_T("%d"), Istay);
	return str;
}
