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
	m_pCWayFind->LoadCardPointInfo(g_pEngine->m_strWorkPath + _T("\\findMapData.ini"), mapName + _T("_卡点数据"));


	return true;
}
bool CLife_Fish::initGameInfo(CString mapName)
{
	CString ini_strContent = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("\\findMapData.ini"), _T("钓鱼_mapInfo"), mapName);
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
		//检查工具是否存在
		while (true)
		{
			LogD(_T("检查工具是否存在"));
			if (!m_pToolMaintenance->checkToolExistence(NULL)) {
				LogD(_T("工具不存在，购买工具中"));
				if (m_pToolMaintenance->ToolMaintenance(NULL, true, true)) {
					break;
				}
			}
			else
			{
				LogD(_T("有工具"));
				break;
			}
		}

		m_pLifeModule->BackMainScreen();
		//确认是否处于目标地区
		CPoint CurrentCoordinate = m_pCWayFind->GetGamePoint(0.8);
		if (CurrentCoordinate.x == NULL) {
			while (true)
			{
				LogD(_T("传送回默认坐标"));
				if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName)) {
					LogD(_T("传送回默认坐标成功"));
					break;
				}
				else
				{
					LogD(_T("传送回默认坐标失败"));
					continue;
				}
			}
		}


		m_pLifeModule->BackMainScreen();
		m_pLifeModule->checkDeath();

		int iRetState = NULL;


		//寻路部分
		constexpr auto MaxDetachment = 3;
		bool IsDetachment = false;
		//检查当前坐标是否可用
		for (size_t i = 0; i < MaxDetachment; i++)
		{
			if (m_pCWayFind->detachmentStuck()) {
				LogD(_T("校验当前坐标成功"));
				IsDetachment = true;
				break;
			}
			else {
				LogD(_T("当前坐标不可用%d"), i);
			}
		}

		if (!IsDetachment) {
			LogD(_T("传送回默认坐标"));
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
					LogD(_T("校验当前坐标成功"));
				}
				else {

					if (m_pLifeModule->checkMonster())
					{
						m_pLifeModule->ClickRectangleA();
						m_pLifeModule->CleanMonster();
						break;
					}

					LogD(_T("校验当前坐标失败"));
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

		for (size_t i = 0; i < 4; i++) { // 最后移动点击坐标（矫正抛竿方向坐标）
			m_pTool->RClick(Life_FishInfo.pFishDirectionCoordinate);
			m_pTool->SleepEx(m_pTool->GetRandom(300, 500));
		}

		//钓鱼部分
		while (true)
		{
			m_pLifeModule->BackMainScreen();
			//维修
			if (!ClickMaintenance(iRetState)) {
				break;
			}
			if (iRetState == 2) {
				LogD(_T("需要返回目标钓鱼坐标"));
				break;
			}
			
			while (true)
			{
				LogD(_T("Check生活状态"));
				m_pLifeModule->BackMainScreen();
				if (!FIND(_T("生活状态")))
					m_pTool->KeyPress(VK_B);

				if (WFIND(_T("生活状态"))) {
					SLEEPEX(1000);
					if (WFIND(_T("生活体力值还有"))) {
						break;
					}
					else {
						return 1;
					}
				}

			}

			if (!FIND(_T("生活状态")))
				m_pTool->KeyPress(VK_B);

			int tagFishTime = NULL;
			if (FIND(_T("生活状态"))) {
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
					LogD(_T("钓鱼异常"));
					exceptionTime = NULL;
					break;
				}
				continue;
			}

			while (true)
			{
				if (m_pTool->Find(_T("鱼漂"))) {
					LogD(_T("有鱼上钩"));
					m_pTool->KeyPress(VK_E);
					m_pTool->SleepEx(m_pTool->GetRandom(6000, 6500));
					break;
				}

				if (m_pTool->GetTimeSeconds() - tagFishTime >= 16000) {
					exceptionTime++;
					LogD(_T("抛竿超时%d"), exceptionTime);
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

	if (FIND(_T("装备需要修理")))
	{
		LogD(_T("装备需要修理"));

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
				LogD(_T("当前执行的地图:%s 维修工坐标为空 执行默认传送维修区域"), m_pGameMapInfo->TeleportName);
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
				LogD(_T("传送回默认坐标"));
				if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName)) {
					iRetState = 2;
					LogD(_T("传送回默认坐标成功"));
					break;
				}
				else
				{
					LogD(_T("传送回默认坐标失败"));
				}
			}
			else
			{
				iRetState = 2;
				LogD(_T("维修成功"));
				return true;
			}

		}
	}
	else
	{
		//无需维修
		return true;
	}

	if (MaintenanceSuccess) {
		LogD(_T("维修成功"));
		return true;
	}

	LogD(_T("维修失败"));
	return false;
}