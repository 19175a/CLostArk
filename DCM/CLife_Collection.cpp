#include "pch.h"
#include"CTaskScript.h"
#include "CLife_Collection.h"
#include"CDCMEngine.h"
#include"CGameTeleport.h"
#include"CLife_ToolMaintenance.h"
#include"CWayFind.h"
extern CDCMEngine* g_pEngine;
#define PATHMAPDATA g_pEngine->m_strWorkPath + _T("\\findMapData.ini")

CLife_Collection::CLife_Collection()
{

}
bool CLife_Collection::Init(CTaskScript* pTaskScript, CString mapName)
{
	m_pGameMapInfo = new tagGameMapInfo();
	m_pTool = pTaskScript;
	m_pLifeModule = new CLifeModule(pTaskScript);


	if (!initGameInfo(mapName))
		return CLife_Collection::STATE_Init_failure;

	m_pGameTeleport = new CGameTeleport();
	m_pGameTeleport->Init(pTaskScript);
	m_pGameTeleport->Load();
	CString strRet = m_pGameTeleport->GetCurrentCities(m_pGameMapInfo->TeleportName);

	m_pToolMaintenance = new CLife_ToolMaintenance();
	m_pToolMaintenance->Init(pTaskScript, strRet);
	m_pCWayFind = new CWayFind();
	m_pCWayFind->Init(m_pTool, m_pGameMapInfo);
	m_pCWayFind->LoadCardPointInfo(PATHMAPDATA, mapName + _T("_��������"));

	return true;
}

CLife_Collection::~CLife_Collection()
{
	if (m_pCWayFind) {
		delete m_pCWayFind;
	}
	if (m_pToolMaintenance) {
		delete m_pToolMaintenance;
	}
	if (m_pGameTeleport) {
		delete m_pGameTeleport;
	}
	if (m_pLifeModule) {
		delete m_pLifeModule;
	}
	if (m_pGameMapInfo) {
		delete m_pGameMapInfo;
	}

}

void CLife_Collection::Tset()
{
	initGameInfo(_T("��������޵�Ժ"));
	CPoint CurrentCoordinate = m_pCWayFind->GetGamePoint();
	if (m_pCWayFind->computation_distance_Sort(CurrentCoordinate, m_pGameMapInfo->wayFindCoordinate))
	{
		for (auto it = m_pGameMapInfo->wayFindCoordinate.cbegin(); it != m_pGameMapInfo->wayFindCoordinate.cend(); it++)
		{
			int ret = m_pCWayFind->WayFindStart(*it);
			LogD(_T("WayFindStart: %d"), ret);
		}

	}


	while (true)
	{
		m_pLifeModule->BackMainScreen();
	}

}

bool CLife_Collection::initGameInfo(CString mapName)
{
	CString ini_strContent = m_pTool->iniGetValue(PATHMAPDATA, _T("�ɼ�_mapInfo"), mapName);
	if (!ini_strContent) {
		LogD(_T("!!! %s"), mapName);
		return false;
	}

	CStringArray arrStrContent;
	Split(ini_strContent, arrStrContent, _T("|"));
	if (arrStrContent.GetCount() != 4)
		return false;
	m_pGameMapInfo->mapBmpName = arrStrContent[0];
	m_pGameMapInfo->astarBmpName = arrStrContent[1];
	m_pGameMapInfo->TeleportName = arrStrContent[2];
	m_pGameMapInfo->RepairmanCoordinates = m_pTool->strToPoint(arrStrContent[3]);
	if (arrStrContent.GetCount() == 5) {
		m_pGameMapInfo->tagUsefulCoordinate = m_pTool->strToPoint(arrStrContent[4]);
	}
	else {
		m_pGameMapInfo->tagUsefulCoordinate = m_pGameMapInfo->RepairmanCoordinates;
	}

	std::map<CString, CString> mapIniKeyValue{};
	if (!m_pTool->iniGetAllKeyValueAllNodeNames(PATHMAPDATA, _T("�ɼ�_") + mapName + _T("_coordinates"), mapIniKeyValue))
		return false;
	for (auto it = mapIniKeyValue.cbegin(); it != mapIniKeyValue.cend(); it++) {
		CStringArray arrContent;
		Split(it->second, arrContent, _T("|"));
		for (size_t i = 0; i < arrContent.GetCount(); i++)
			m_pGameMapInfo->wayFindCoordinate.emplace_back(m_pTool->strToPoint(arrContent[i]));
	}


	return true;
}

int CLife_Collection::started()
{
	//////��鹤���Ƿ����
	//while (true)
	//{
	//	m_pLifeModule->BackMainScreen();


	//}


	//while (true)
	//{
	//	LogD(_T("��鹤���Ƿ����"));
	//	if (!m_pToolMaintenance->checkToolExistence(NULL)) {
	//		LogD(_T("���߲����ڣ����򹤾���"));
	//		if (m_pToolMaintenance->ToolMaintenance(NULL, true, true)) {
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		LogD(_T("�й���"));
	//		break;
	//	}
	//}

	int ret = Collection_Carry();
	LogD(_T("Collection_Carry:%d"), ret);
	return ret;
}

int CLife_Collection::Collection_Carry()
{
	//�жϵ�ǰ�����Ƿ�ΪĿ�����
	//��һ����Ҫ�����������˳��
	m_pLifeModule->BackMainScreen();
	CPoint CurrentCoordinate = m_pCWayFind->GetGamePoint(0.75);
	if (CurrentCoordinate.x == NULL) {
		while (true)
		{
			LogD(_T("���ͻ�Ĭ������"));
			if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName,true)) {
				LogD(_T("���ͻ�Ĭ������ɹ�"));
				CurrentCoordinate = m_pCWayFind->GetGamePoint(0.75);
				if (CurrentCoordinate.x != NULL) {
					break;
				}
				LogD(_T("���ͻ�Ĭ������ʧ��"));
			}
			else
			{
				LogD(_T("���ͻ�Ĭ������ʧ��"));
			}

			if (m_pLifeModule->checkMonster()) {
				m_pLifeModule->ClickRectangleA();
				m_pLifeModule->CleanMonster();
			}
		}
	}

	CurrentCoordinate = m_pCWayFind->GetGamePoint(0.75);
	int Index = m_pCWayFind->computation_distance_Sort(CurrentCoordinate, m_pGameMapInfo->wayFindCoordinate);
	if (Index == -1) {
		LogD(_T("������������ʧ��"));
		return STATE_distance_Sort_failure;
	}

	if (Index == 0) {
		m_IndexCoordinate = Index;
		m_IndexCoordinate--;
	}

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
			break;
		}
	}

	bool IsNext = true;
	CPoint GoalsCoordinate{ NULL,NULL };
	int iAttackTime = NULL;
	while (true)
	{
		m_pLifeModule->BackMainScreen();
		m_pLifeModule->checkDeath();

		if (IsNext) {
			GoalsCoordinate = GetNextWayFindCoordinate(m_pGameMapInfo->wayFindCoordinate);
			iAttackTime = NULL;
		}


		if (!ClickMaintenance()) //ά��
		{
			if (m_pLifeModule->checkMonster()){
				m_pLifeModule->ClickRectangleA();
				m_pLifeModule->CleanMonster();
			}
			LogD(_T("������Χ�й��﹥����ǰ����һ�����곢��"));

			m_pCWayFind->WayFindStart(GoalsCoordinate);
			IsNext = true;
			continue;
		}

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
			m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName,true);
			LogD(_T("������ǰĿ�꣬ǰ����һ������"));
			IsNext = true;
			continue;
		}


		constexpr auto Max = 3;
		bool IsDone = false;
		for (size_t i = 0; i < Max; i++)
		{
			LogD(_T("ǰ��Ŀ��:%d,%d"), GoalsCoordinate.x, GoalsCoordinate.y);
			int retWayFind = m_pCWayFind->WayFindStart(GoalsCoordinate);
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
					}

					LogD(_T("������ǰĿ�꣬ǰ����һ������"));
					break;
				}
			}

			if (m_pLifeModule->checkMonster())
			{
				m_pLifeModule->ClickRectangleA();
				m_pLifeModule->CleanMonster();
			}

		}

		if (!IsDone) {
			IsNext = true;
			continue;
		}


		//�ɼ�����
		int retOperation = CollectionOperation(_T("�ɼ�_���λ�|�ɼ�_���λ�1|�ɼ�_���λ�2|�ɼ�_���λ�3|�ɼ�_���λ�4|�ɼ�_���λ�5"));
		if (retOperation == CollectionOperationSTATE::STATE_empty) {
			IsNext = true;
			LogD(_T("δ�ҵ�Ŀ��"));
		}
		else if (retOperation == CollectionOperationSTATE::STATE_Done) {
			IsNext = true;
			LogD(_T("�ɼ����"));
		}
		else if (retOperation == CollectionOperationSTATE::STATE_Attack) {
			m_pLifeModule->ClickRectangleA();
			m_pLifeModule->CleanMonster();
			LogD(_T("�����������·�������"));
			IsNext = false;
			iAttackTime++;
			if (iAttackTime >= 3) {
				LogD(_T("�����������϶� ������ǰ����:%d,%d"), GoalsCoordinate.x, GoalsCoordinate.y);
				IsNext = true;
				iAttackTime = NULL;
			}

			continue;
		}
		else if (retOperation == CollectionOperationSTATE::STATE_Strength_empty) {
			return CollectionState::STATE_DONE;
		}
		else
		{
			IsNext = true;
		}

	}
	return -1;
}

CPoint CLife_Collection::GetNextWayFindCoordinate(std::vector<CPoint>& vCoordinate)
{

	auto it = vCoordinate.begin();
	if (m_IndexCoordinate != -1 && m_IndexCoordinate + 1 >= vCoordinate.size()) {
		
		//std::reverse(vCoordinate.begin(), vCoordinate.end());
		//auto it = vCoordinate.begin();
		//m_IndexCoordinate++;
		//return it[NULL];
		LogD(_T("һ�ִ˽��� ���½�������"));
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
				}
			}
		}

		CurrentCoordinate = m_pCWayFind->GetGamePoint(0.8);
		int Index = m_pCWayFind->computation_distance_Sort(CurrentCoordinate, m_pGameMapInfo->wayFindCoordinate);
		if (Index == -1) {
			LogD(_T("������������ʧ��"));
			return it[NULL];
		}

		if (Index == 0) {
			m_IndexCoordinate = Index;
			m_IndexCoordinate--;
		}

	}

	it = vCoordinate.begin();
	m_IndexCoordinate++;
	return it[m_IndexCoordinate];
}

int  CLife_Collection::CollectionOperation(CString strArrUI)
{
	m_pCWayFind->waitMovStop(2500);
	CPoint lickPoint{ NULL,NULL };
	CStringArray arrStrui;
	Split(strArrUI, arrStrui, _T("|"));
	for (size_t i = 0; i < arrStrui.GetCount(); i++)
	{
		if (FINDPOINT(arrStrui[i], lickPoint))
		{
			lickPoint.x += 15; lickPoint.y += 65;
			break;
		}
	}
	if (!lickPoint.x)
		return CollectionOperationSTATE::STATE_empty;

	//if(!m_pLifeModule->checkPhysicalStrength())
	//	return CollectionOperationSTATE::STATE_Strength_empty;

	int tagTimr = m_pTool->GetTimeSeconds();
	while (true)
	{
		if (!FIND(_T("����״̬")))
			m_pTool->KeyPress(VK_B);

		if (WFIND(_T("����״̬"))) {
			SLEEPEX(1000);
			if (WFIND(_T("��������ֵ����"))) {
				break;
			}
			else {
				return CollectionOperationSTATE::STATE_Strength_empty;
			}
		}

		if ((m_pTool->GetTimeSeconds() - tagTimr) >= 20000) {
			return CollectionOperationSTATE::STATE_Strength_empty;
		}

	}

	//��һ�βɼ�
	RCLICK(lickPoint);
	int tagTime = m_pTool->GetTimeSeconds();
	while (true)
	{
		if (FIND(_T("���ڿ���"))) {
			LogD(_T("���ڲɼ�"));
			break;
		}

		LogD(_T("���ɼ���%d"), int((m_pTool->GetTimeSeconds() - tagTime) / 1000));
		if (m_pLifeModule->checkMonster(g_pEngine->disregardLifeAttack))
			return CollectionOperationSTATE::STATE_Attack;

		if ((m_pTool->GetTimeSeconds() - tagTime) > 4000)
			return CollectionOperationSTATE::STATE_Timeout;

		/*SLEEPEX(1000);*/
	}

	LogD(_T("����4"));
	//�ڶ��βɼ�
	bool IsSecond = false;
	tagTime = m_pTool->GetTimeSeconds();
	while (true)
	{
		if (!WFINDTIME(_T("���ڿ���"), 1500))
		{
			if (!IsSecond) {
				m_pTool->KeyPress(VK_G);
				IsSecond = true;
			}
			else
			{
				break;
			}
		}

		if (m_pLifeModule->checkMonster(g_pEngine->disregardLifeAttack))
			return CollectionOperationSTATE::STATE_Attack;

		LogD(_T("���ڲɼ���%d"), int((m_pTool->GetTimeSeconds() - tagTime) / 1000));


		if ((m_pTool->GetTimeSeconds() - tagTime) > 8000)
			return CollectionOperationSTATE::STATE_Done;

		//SLEEPEX(1000);
	}

	return CollectionOperationSTATE::STATE_Done;;
}

bool  CLife_Collection::ClickMaintenance()
{
	bool MaintenanceSuccess = false;
	bool IsTeleportBack = false;
	if (FIND(_T("װ����Ҫ����")))
	{
		while (true)
		{
			LogD(_T("װ����Ҫ����"));
			if (g_pEngine->m_tagToolMaintenanceMode == 1) {
				if (m_pToolMaintenance->ToolMaintenance(NULL, false, false)) {
					MaintenanceSuccess = true;
					IsTeleportBack = true;
					break;
				}
				else
				{
					IsTeleportBack = true;
					LogD(_T("Ĭ�ϳ���ά��ʧ��"));
					break;
				}
			}

			if (g_pEngine->m_tagToolMaintenanceMode == 2) {
				if (m_pToolMaintenance->ToolMaintenance(NULL, true, true)) {
					MaintenanceSuccess = true;
					IsTeleportBack = true;
					break;
				}
				else{
					IsTeleportBack = true;
					LogD(_T("Ĭ�ϳ���ά��ʧ��"));
					break;
				}
			}

			if (g_pEngine->m_tagToolMaintenanceMode == 3) {
				if (m_pGameMapInfo->RepairmanCoordinates.x != NULL) {
					if (m_pToolMaintenance->MapMaintenanceTool(m_pCWayFind, m_pGameMapInfo)) {
						MaintenanceSuccess = true;
						break;
					}
					else{
						g_pEngine->m_tagToolMaintenanceMode == 2;
						LogD(_T("��ͼά��ʧ�ܷ���Ĭ�ϳ���ά��"));
						continue;
					}
				}
				else{
					LogD(_T("��ǰִ�еĵ�ͼ:%s ά�޹�����Ϊ�� ִ��Ĭ�ϴ���ά������"), m_pGameMapInfo->TeleportName);
					LogD(_T("ά��ʧ�ܷ���Ĭ�ϳ���ά��"));
					g_pEngine->m_tagToolMaintenanceMode == 2;
					continue;
				}

			}

			if (g_pEngine->m_tagToolMaintenanceMode == 4) {
				if (m_pToolMaintenance->PetsMaintenanceTool()) {
					MaintenanceSuccess = true;
					break;
				}
				else{
					LogD(_T("����ά��ʧ�ܷ���Ĭ�ϳ���ά��"));
					g_pEngine->m_tagToolMaintenanceMode == 2;
				}
			}
		}

		while (MaintenanceSuccess)
		{
			if (IsTeleportBack) {
				LogD(_T("���ͻ�Ĭ������"));
				if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName)) {
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