#include "pch.h"
#include "CTaskManager.h"
#include "CDCMEngine.h"
#include "CTaskScript.h"
#include "CTaskHelperScript.h"
#include "CTaskHttpSrvThread.h"

extern CDCMEngine* g_pEngine;

CTaskManager::CTaskManager()
{

}

CTaskManager::~CTaskManager()
{

}

tagTaskInfo* CTaskManager::GetInfo(int id)
{
	for (int i = 0; i < g_pEngine->m_arrTask.GetCount(); i++)
	{
		if (g_pEngine->m_arrTask[i]->id == id)
		{
			return g_pEngine->m_arrTask[i];
		}
	}

	return NULL;
}

void CTaskManager::Start(tagWndInfo* pWndinfo, tagGameAcconutInfo* GameAcconutInfo)
{
	tagWndInfo* pInfo = pWndinfo;


	if (pInfo->hWnd == NULL || !IsWindow(pInfo->hWnd))
		LogE(_T("CTaskManager: ����:%d û���������޷�ִ������"), pInfo->id);

	//�����û�ֹͣ�� ���°�
	tagTaskInfo* pTask = GetInfo(pInfo->id);

	pTask = new tagTaskInfo;
	g_pEngine->m_arrTask.Add(pTask);

	//��ʼ����Ա��
	pTask->id = pInfo->id;
	pTask->pWnd = pInfo;
	pTask->pWnd->pTaskInfo = pTask;
	pTask->pStatus = new tagGameStatus;

	//�������߳�
	pTask->pTaskThread = new CTaskScript(pInfo);
	pTask->pAccountInfo = new tagGameAcconutInfo();
	pTask->pAccountInfo = GameAcconutInfo;
	pTask->pTaskThread->StartThread();

	LogN(_T("%s: �����������߳�"), pInfo->strTitle);

	Sleep(10);
	//int port = g_pEngine->GetPort(pTask->id);
	//pTask->pTaskHttpSrv = new CTaskHttpSrvThread(port);
	//pTask->pTaskHttpSrv->SetTaskInfoPtr(pTask);
	//pTask->pTaskHttpSrv->StartThread();
	//LogN(_T("%s: ����http�����߳�"), pInfo->strTitle);

	////���������߳�
	//while (true)
	//{
	//	if (pTask->pTaskThread->IsBind())
	//	{
	//		Sleep(100);

	//		pTask->pTaskHelperThread = new CTaskHelperScript(pInfo);
	//		pTask->pTaskHelperThread->StartThread();

	//		LogN(_T("%s: �����������߳�"), pInfo->strTitle);

	//		break;
	//	}
	//	else {
	//		LogN(_T("%s: �ȴ������̰߳����..."), pInfo->strTitle);
	//		Sleep(100);
	//	}
	//}

}

void CTaskManager::Stop(int id)
{
	for (int i = 0; i < g_pEngine->m_arrTask.GetCount(); i++)
	{
		tagWndInfo* pInfo = g_pEngine->m_arrTask[i]->pWnd;

		if (pInfo->id != id && id != -1)
			continue;

		if (g_pEngine->m_arrTask[i]->pStatus)
		{
			delete g_pEngine->m_arrTask[i]->pStatus;
			g_pEngine->m_arrTask[i]->pStatus = NULL;
		}

		if (g_pEngine->m_arrTask[i]->pTaskThread != NULL)
		{
			//ֹͣʱ �����߳�ִ�� ����û�л����˳�
			g_pEngine->m_arrTask[i]->pTaskThread->Resume();
			//g_pEngine->m_arrTask[i]->pTaskHelperThread->Resume();


			//ֹͣ���߳�
			if (g_pEngine->m_arrTask[i]->pTaskThread->StopThread(5000))
			{
				LogE(_T("CTaskManager: ֹͣ�������̳߳ɹ� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
				if (g_pEngine->m_arrTask[i]->pTaskThread) {
					delete g_pEngine->m_arrTask[i]->pTaskThread;
					g_pEngine->m_arrTask[i]->pTaskThread = NULL;
				}
				if (g_pEngine->m_arrTask[i]->pAccountInfo) {
					delete g_pEngine->m_arrTask[i]->pAccountInfo;
					g_pEngine->m_arrTask[i]->pAccountInfo = NULL;
				}
				if (g_pEngine->m_arrTask[i]->pStatus) {
					delete g_pEngine->m_arrTask[i]->pStatus;
					g_pEngine->m_arrTask[i]->pStatus = NULL;
				}

			}
			else {
				LogE(_T("CTaskManager: ֹͣ�������̳߳�ʱ ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
			}

			////ֹͣ�����߳�
			//if (g_pEngine->m_arrTask[i]->pTaskHelperThread->StopThread(5000))
			//{
			//	LogE(_T("CTaskManager: ֹͣ�������ɹ� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
			//	Sleep(500);
			//	if (g_pEngine->m_arrTask[i]->pTaskHelperThread)
			//		delete g_pEngine->m_arrTask[i]->pTaskHelperThread;

			//	g_pEngine->m_arrTask[i]->pTaskHelperThread = NULL;
			//}
			//else {
			//	LogE(_T("CTaskManager: ֹͣ��������ʱ ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
			//}

			////ֹͣhttp�߳�
			//g_pEngine->m_arrTask[i]->pTaskHttpSrv->Stop();
			//if (g_pEngine->m_arrTask[i]->pTaskHttpSrv->StopThread(5000))
			//{
			//	SAFE_DELETE(g_pEngine->m_arrTask[i]->pTaskHttpSrv);
			//	LogE(_T("CTaskManager: ֹͣhttp�̳߳ɹ� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
			//	Sleep(500);
			//	if (g_pEngine->m_arrTask[i]->pTaskHttpSrv)
			//		delete g_pEngine->m_arrTask[i]->pTaskHttpSrv;

			//	g_pEngine->m_arrTask[i]->pTaskHttpSrv = NULL;
			//}
		}
		else
		{
			//LogE(_T("CTaskManager: �����߳�δ���� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
		}

		//�����������
		for (int j = 0; j <g_pEngine->m_arrWnd.GetCount() ; j++)
		{
			if (!g_pEngine->m_arrWnd[j])
				continue;
			if (g_pEngine->m_arrWnd[j]->id == pInfo->id)
			{
				LogE(_T("CTaskManager: ����������� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
				delete g_pEngine->m_arrWnd[j];
				g_pEngine->m_arrWnd[j] = NULL;
				break;
			}
		}

	}
}

void CTaskManager::Suspend(int id)
{
	for (int i = 0; i < g_pEngine->m_arrTask.GetCount(); i++)
	{
		bool bDo = false;
		if (id == -1)
		{
			bDo = true;
		}
		else if (g_pEngine->m_arrTask[i]->id == id)
		{
			bDo = true;
		}

		if (bDo)
		{
			if (g_pEngine->m_arrTask[i]->pTaskThread/* && g_pEngine->m_arrTask[i]->pTaskHelperThread*/)
			{
				g_pEngine->m_arrTask[i]->pTaskThread->Suspend();
				//g_pEngine->m_arrTask[i]->pTaskHelperThread->Suspend();

				LogN(_T("CTaskManager: ��ͣ����ִ�� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
			}
		}
	}
}

void CTaskManager::Resume(int id)
{
	for (int i = 0; i < g_pEngine->m_arrTask.GetCount(); i++)
	{
		bool bDo = false;
		if (id == -1)
		{
			bDo = true;
		}
		else if (g_pEngine->m_arrTask[i]->id == id)
		{
			bDo = true;
		}

		if (bDo)
		{
			g_pEngine->m_arrTask[i]->pTaskThread->Resume();
			//g_pEngine->m_arrTask[i]->pTaskHelperThread->Resume();
			LogN(_T("CTaskManager: �ָ�����ִ�� ����:%s"), g_pEngine->m_arrTask[i]->pWnd->strTitle);
		}
	}
}

void CTaskManager::AddTask(int id, CStringArray& arrTask)
{
	for (int i = 0; i < g_pEngine->m_arrTask.GetCount(); i++)
	{
		if (g_pEngine->m_arrTask[i]->pTaskThread)
		{
			g_pEngine->m_arrTask[i]->pTaskThread->AddTask(arrTask);
		}
	}
}
