#include "pch.h"
#include"CTaskScript.h"
#include "CLife_Fish.h"
#include"CDCMEngine.h"
#include"CGameTeleport.h"
#include"CLife_ToolMaintenance.h"
#include"CWayFind.h"

extern CDCMEngine* g_pEngine;
CLife_Fish::CLife_Fish()
{


}
CLife_Fish::~CLife_Fish()
{
	if (m_pCWayFind)
		delete m_pCWayFind;

	if (m_pToolMaintenance)
		delete m_pToolMaintenance;

	if(m_pGameTeleport)
		delete m_pGameTeleport;

	if (m_pLifeModule)
		delete m_pLifeModule;
	
	if (m_pGameMapInfo)
		delete m_pGameMapInfo;
}
bool CLife_Fish::Init(CTaskScript* pTaskScript, CString mapName)
{
	m_pGameMapInfo = new tagGameMapInfo();
	m_pTool = pTaskScript;
	m_pLifeModule = new CLifeModule(pTaskScript);

	if (!initGameInfo(mapName))
		return false;

	m_pGameTeleport = new CGameTeleport();
	m_pGameTeleport->Init(pTaskScript);
	m_pGameTeleport->Load();
	CString strRet = m_pGameTeleport->GetCurrentCities(m_pGameMapInfo->TeleportName);

	m_pToolMaintenance = new CLife_ToolMaintenance();
	m_pToolMaintenance->Init(pTaskScript, strRet);
	m_pCWayFind = new CWayFind();
	m_pCWayFind->Init(m_pTool, m_pGameMapInfo);
	m_pCWayFind->LoadCardPointInfo(g_pEngine->m_strWorkPath + _T("\\findMapData.ini"), mapName + _T("_��������"));


	return true;
}
bool CLife_Fish::initGameInfo(CString mapName)
{
	CString ini_strContent = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("\\findMapData.ini"), _T("����_mapInfo"), mapName);
	if (!ini_strContent) {
		LogD(_T("!!! %s"), mapName);
		return false;
	}

	CStringArray arriniInfo;
	Split(ini_strContent, arriniInfo, _T("|"));
	if (arriniInfo.GetCount() != 7) {
		LogD(_T("CLife_Fish initGameInfo %d"), mapName);
		return false;
	}

	m_pGameMapInfo->mapBmpName = arriniInfo[0];
	m_pGameMapInfo->astarBmpName = arriniInfo[1];
	m_pGameMapInfo->TeleportName = arriniInfo[2];
	m_pGameMapInfo->RepairmanCoordinates = m_pTool->strToPoint(arriniInfo[3]);

	Life_FishInfo.pFishCoordinate = m_pTool->strToPoint(arriniInfo[4]);
	Life_FishInfo.pFishDirectionCoordinate = m_pTool->strToPoint(arriniInfo[5]);
	m_pGameMapInfo->tagUsefulCoordinate = Life_FishInfo.pFishCoordinate;
	CStringArray arrRectInfo;
	Split(arriniInfo[6], arrRectInfo, _T(","));
	if (arrRectInfo.GetCount() != 4) {
		LogD(_T("CLife_Fish initGameInfo %d"), arriniInfo[6]);
		return false;
	}
	Life_FishInfo.rIdentifyBuoyArea.left = _ttoi(arrRectInfo[0]);
	Life_FishInfo.rIdentifyBuoyArea.top = _ttoi(arrRectInfo[1]);
	Life_FishInfo.rIdentifyBuoyArea.right = _ttoi(arrRectInfo[2]);
	Life_FishInfo.rIdentifyBuoyArea.bottom = _ttoi(arrRectInfo[3]);

	return true;
}
int  CLife_Fish::started()
{
	int exceptionTime = NULL;
	while (true)
	{
		//��鹤���Ƿ����
		while (true)
		{
			LogD(_T("��鹤���Ƿ����"));
			if (!m_pToolMaintenance->checkToolExistence(NULL)) {
				LogD(_T("���߲����ڣ����򹤾���"));
				if (m_pToolMaintenance->ToolMaintenance(NULL, true, true)) {
					break;
				}
			}
			else
			{
				LogD(_T("�й���"));
				break;
			}
		}

		m_pLifeModule->BackMainScreen();
		//ȷ���Ƿ���Ŀ�����
		CPoint CurrentCoordinate = m_pCWayFind->GetGamePoint(0.8);
		if (CurrentCoordinate.x == NULL) {
			while (true)
			{
				LogD(_T("���ͻ�Ĭ������"));
				if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName)) {
					LogD(_T("���ͻ�Ĭ������ɹ�"));
					break;
				}
				else
				{
					LogD(_T("���ͻ�Ĭ������ʧ��"));
					continue;
				}
			}
		}


		m_pLifeModule->BackMainScreen();
		m_pLifeModule->checkDeath();

		int iRetState = NULL;


		//Ѱ·����
		constexpr auto MaxDetachment = 3;
		bool IsDetachment = false;
		//��鵱ǰ�����Ƿ����
		for (size_t i = 0; i < MaxDetachment; i++)
		{
			if (m_pCWayFind->detachmentStuck()) {
				LogD(_T("У�鵱ǰ����ɹ�"));
				IsDetachment = true;
				break;
			}
			else {
				LogD(_T("��ǰ���겻����%d"), i);
			}
		}

		if (!IsDetachment) {
			LogD(_T("���ͻ�Ĭ������"));
			m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName);
		}


		constexpr auto Max = 3;
		bool IsDone = false;
		for (size_t i = 0; i < Max; i++)
		{
			int retWayFind = m_pCWayFind->WayFindStart(Life_FishInfo.pFishCoordinate);
			if (retWayFind == CWayFind::WayFindState::STATE_Done) {
				IsDone = true; break;
			}
			else if (retWayFind == CWayFind::WayFindState::STATE_Movement_timeout) {
				if (m_pCWayFind->detachmentStuck()) {
					LogD(_T("У�鵱ǰ����ɹ�"));
				}
				else {

					if (m_pLifeModule->checkMonster())
					{
						m_pLifeModule->ClickRectangleA();
						m_pLifeModule->CleanMonster();
						break;
					}

					LogD(_T("У�鵱ǰ����ʧ��"));
					break;
				}
			}

			if (m_pLifeModule->checkMonster()){
				m_pLifeModule->ClickRectangleA();
				m_pLifeModule->CleanMonster();
				break;
			}
		}

		if (!IsDone) {
			continue;
		}

		for (size_t i = 0; i < 4; i++) { // ����ƶ�������꣨�����׸ͷ������꣩
			m_pTool->RClick(Life_FishInfo.pFishDirectionCoordinate);
			m_pTool->SleepEx(m_pTool->GetRandom(300, 500));
		}

		//���㲿��
		while (true)
		{
			m_pLifeModule->BackMainScreen();
			//ά��
			if (!ClickMaintenance(iRetState)) {
				break;
			}
			if (iRetState == 2) {
				LogD(_T("��Ҫ����Ŀ���������"));
				break;
			}
			
			while (true)
			{
				LogD(_T("Check����״̬"));
				m_pLifeModule->BackMainScreen();
				if (!FIND(_T("����״̬")))
					m_pTool->KeyPress(VK_B);

				if (WFIND(_T("����״̬"))) {
					SLEEPEX(1000);
					if (WFIND(_T("��������ֵ����"))) {
						break;
					}
					else {
						return 1;
					}
				}

			}

			if (!FIND(_T("����״̬")))
				m_pTool->KeyPress(VK_B);

			int tagFishTime = NULL;
			if (FIND(_T("����״̬"))) {
				tagFishTime = m_pTool->GetTimeSeconds();
				m_pTool->DLLMoveTo(Life_FishInfo.pFishDirectionCoordinate.x, Life_FishInfo.pFishDirectionCoordinate.y);
				m_pTool->SleepEx(m_pTool->GetRandom(800, 1300));
				m_pTool->KeyPress(VK_E);
				m_pTool->SleepEx(m_pTool->GetRandom(3500, 4000));
			}
			else
			{
				exceptionTime++;
				if (exceptionTime >= 3) {
					LogD(_T("�����쳣"));
					exceptionTime = NULL;
					break;
				}
				continue;
			}

			while (true)
			{
				if (m_pTool->Find(_T("��Ư"))) {
					LogD(_T("�����Ϲ�"));
					m_pTool->KeyPress(VK_E);
					m_pTool->SleepEx(m_pTool->GetRandom(6000, 6500));
					break;
				}

				if (m_pTool->GetTimeSeconds() - tagFishTime >= 16000) {
					exceptionTime++;
					LogD(_T("�׸ͳ�ʱ%d"), exceptionTime);
					break;
				}

				m_pTool->SleepEx(100);
			}

		}


	}


	return -1;

}
void CLife_Fish::Tset()
{

	return;
}
bool CLife_Fish::ClickMaintenance(int& iRetState)
{
	bool MaintenanceSuccess = false;
	bool IsTeleportBack = false;

	if (FIND(_T("װ����Ҫ����")))
	{
		LogD(_T("װ����Ҫ����"));

		if (g_pEngine->m_tagToolMaintenanceMode == 1) {
			if (m_pToolMaintenance->ToolMaintenance(NULL, false, false)) {
				MaintenanceSuccess = true;
				IsTeleportBack = true;
			}
			else
			{
				IsTeleportBack = true;
			}
		}

		if (g_pEngine->m_tagToolMaintenanceMode == 2) {
			if (m_pToolMaintenance->ToolMaintenance(NULL, true, true)) {
				MaintenanceSuccess = true;
				IsTeleportBack = true;
			}
			else
			{
				IsTeleportBack = true;
			}
		}

		if (g_pEngine->m_tagToolMaintenanceMode == 3) {
			if (m_pGameMapInfo->RepairmanCoordinates.x != NULL) {
				if (m_pToolMaintenance->MapMaintenanceTool(m_pCWayFind, m_pGameMapInfo)) {
					MaintenanceSuccess = true;
				}
			}
			else
			{
				LogD(_T("��ǰִ�еĵ�ͼ:%s ά�޹�����Ϊ�� ִ��Ĭ�ϴ���ά������"), m_pGameMapInfo->TeleportName);
			}

		}

		if (g_pEngine->m_tagToolMaintenanceMode == 4) {
			if (m_pToolMaintenance->PetsMaintenanceTool()) {
				MaintenanceSuccess = true;
			}

		}

		while (MaintenanceSuccess)
		{
			if (IsTeleportBack) {
				LogD(_T("���ͻ�Ĭ������"));
				if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName)) {
					iRetState = 2;
					LogD(_T("���ͻ�Ĭ������ɹ�"));
					break;
				}
				else
				{
					LogD(_T("���ͻ�Ĭ������ʧ��"));
				}
			}
			else
			{
				iRetState = 2;
				LogD(_T("ά�޳ɹ�"));
				return true;
			}

		}
	}
	else
	{
		//����ά��
		return true;
	}

	if (MaintenanceSuccess) {
		LogD(_T("ά�޳ɹ�"));
		return true;
	}

	LogD(_T("ά��ʧ��"));
	return false;
}