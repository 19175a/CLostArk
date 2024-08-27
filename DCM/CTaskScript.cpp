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
	////����yolo-------------------------
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
		LogD(_T("����yolo���� %d"), Time);
		SleepEx(1000);
	}

	//---------------------------------
	LogD(_T("������������ʹ�ýű���������CTRL+ALT��ݼ��ó����"));
	LogD(_T("����ʱ����"));
	SleepEx(5000);
	Click(CPoint(39, 16));
	checkGemaState();
	for (int i = 0; i < g_pEngine->m_pCDlg->m_lstTaskRun.GetItemCount(); i++) {

		int index = NULL;
		CString stTskMapName = _T("");
		int iTaskRet = -1;
		CString strTaskName = g_pEngine->m_pCDlg->m_lstTaskRun.GetItemText(i, 0);
		if (strTaskName == _T("�ɼ�")) {
			m_pLife_Collection = new CLife_Collection();
			index =  g_pEngine->m_pCDlg->m_dlg2.m_Life_Collection_ComboBox.GetCurSel();
			g_pEngine->m_pCDlg->m_dlg2.m_Life_Collection_ComboBox.GetLBText(index, stTskMapName);
			if (!m_pLife_Collection->Init(this, stTskMapName)) {
				LogD(_T("Life_Collection ��ʼ��ʧ��"));
				continue;
			}

			iTaskRet = m_pLife_Collection->started();
			LogD(_T("�ɼ�����:%d"), iTaskRet);
			if (m_pLife_Collection) {
				delete m_pLife_Collection;
				m_pLife_Collection = NULL;
			}
			continue;
		}

		if (strTaskName == _T("����")) {
			m_pLife_Fish = new CLife_Fish();
			index = g_pEngine->m_pCDlg->m_dlg2.m_Life_Fish_ComboBox.GetCurSel();
			g_pEngine->m_pCDlg->m_dlg2.m_Life_Fish_ComboBox.GetLBText(index, stTskMapName);
			if (!m_pLife_Fish->Init(this, stTskMapName)) {
				LogD(_T("Life_Collection ��ʼ��ʧ��"));
				continue;
			}

			iTaskRet = m_pLife_Fish->started();
			LogD(_T("���㷵��:%d"), iTaskRet);
			if (m_pLife_Fish) {
				delete m_pLife_Fish;
				m_pLife_Fish = NULL;
			}

			continue;
		}

	}

	for (size_t i = 0; i < 5; i++)
	{
		//�ʼ�
		LogD(_T("�ʼ�"));
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
	LogD(_T("������Ϸ �ȴ�����"));
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
		LogD(_T("���ڽ����߳� ����%d"), Time);
		SleepEx(1000);
	}
}
void CTaskScript::Switch()
{
	LogD(_T("������Ϸ �л��¸��˺�"));
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
		LogD(_T("���ڽ����߳� �л��¸��˺�%d"), Time);
		SleepEx(1000);
	}
}
void CTaskScript::NoSwitch()
{
	LogD(_T("������Ϸ ���л��¸��˺�"));
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
		LogD(_T("���ڽ����߳� ���л��¸��˺�%d"), Time);
		SleepEx(1000);
	}
}

void CTaskScript::checkGemaState()
{
	if (Find(_T("������_ͼ��_�ۺϸ���")) && Find(_T("������_ͼ��_��Ϸ�̳�"))) {
		return;
	}

	//����Ƿ��ڵ�����Ϸѡ����Ϸ����״̬
	bool Isexistence = false;
	if (Find(_T("��Ϸ�������_ѡ��ͼ��"))||Find(_T("��Ϸ��ʼ"))) {
		LogD(_T("��Ϸ�������"));

		while (true)
		{
			if (m_pWnd->pTaskInfo->pAccountInfo->strRegion == _T("P��")) {
				if (FindClick(_T("ѡ�����_P��"))) {
					Isexistence = true;
				}

			}
			else if (m_pWnd->pTaskInfo->pAccountInfo->strRegion == _T("A��")) {
				if (FindClick(_T("ѡ�����_A��"))) {
					Isexistence = true;
				}
			}
			else if (m_pWnd->pTaskInfo->pAccountInfo->strRegion == _T("K��")) {
				if (FindClick(_T("ѡ�����_K��"))) {
					Isexistence = true;
				}
			}
			SleepEx(1000);
			FindClick(_T("��Ϸ�������_ѡ��ͼ��"));

			Isexistence = false;
			if (WClick(_T("��Ϸ��ʼ"), 10000)) {
				Isexistence = true;
			}

			for (size_t i = 0; i < 10; i++)
			{
				bool IsAgain = false;
				if (FindClick(_T("���_���"))) IsAgain = true;
				if (FindClick(_T("���_���1"))) IsAgain = true;
				if (FindClick(_T("���_���2"))) IsAgain = true;

				if (!IsAgain)
					break;
			}

			if (Find(_T("������_ͼ��_�ۺϸ���")) && Find(_T("������_ͼ��_��Ϸ�̳�"))) {
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
