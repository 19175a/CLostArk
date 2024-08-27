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
	m_pCWayFind->LoadCardPointInfo(PATHMAPDATA, mapName + _T("_卡点数据"));

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
	initGameInfo(_T("海德里克修道院"));
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
	CString ini_strContent = m_pTool->iniGetValue(PATHMAPDATA, _T("采集_mapInfo"), mapName);
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
	if (!m_pTool->iniGetAllKeyValueAllNodeNames(PATHMAPDATA, _T("采集_") + mapName + _T("_coordinates"), mapIniKeyValue))
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
	//////检查工具是否存在
	//while (true)
	//{
	//	m_pLifeModule->BackMainScreen();


	//}


	//while (true)
	//{
	//	LogD(_T("检查工具是否存在"));
	//	if (!m_pToolMaintenance->checkToolExistence(NULL)) {
	//		LogD(_T("工具不存在，购买工具中"));
	//		if (m_pToolMaintenance->ToolMaintenance(NULL, true, true)) {
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		LogD(_T("有工具"));
	//		break;
	//	}
	//}

	int ret = Collection_Carry();
	LogD(_T("Collection_Carry:%d"), ret);
	return ret;
}

int CLife_Collection::Collection_Carry()
{
	//判断当前城市是否为目标城市
	//第一次需要计算距离排序顺序
	m_pLifeModule->BackMainScreen();
	CPoint CurrentCoordinate = m_pCWayFind->GetGamePoint(0.75);
	if (CurrentCoordinate.x == NULL) {
		while (true)
		{
			LogD(_T("传送回默认坐标"));
			if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName,true)) {
				LogD(_T("传送回默认坐标成功"));
				CurrentCoordinate = m_pCWayFind->GetGamePoint(0.75);
				if (CurrentCoordinate.x != NULL) {
					break;
				}
				LogD(_T("传送回默认坐标失败"));
			}
			else
			{
				LogD(_T("传送回默认坐标失败"));
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
		LogD(_T("计算所有坐标失败"));
		return STATE_distance_Sort_failure;
	}

	if (Index == 0) {
		m_IndexCoordinate = Index;
		m_IndexCoordinate--;
	}

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


		if (!ClickMaintenance()) //维修
		{
			if (m_pLifeModule->checkMonster()){
				m_pLifeModule->ClickRectangleA();
				m_pLifeModule->CleanMonster();
			}
			LogD(_T("可能周围有怪物攻击，前往下一个坐标尝试"));

			m_pCWayFind->WayFindStart(GoalsCoordinate);
			IsNext = true;
			continue;
		}

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
			m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName,true);
			LogD(_T("放弃当前目标，前往下一个坐标"));
			IsNext = true;
			continue;
		}


		constexpr auto Max = 3;
		bool IsDone = false;
		for (size_t i = 0; i < Max; i++)
		{
			LogD(_T("前往目标:%d,%d"), GoalsCoordinate.x, GoalsCoordinate.y);
			int retWayFind = m_pCWayFind->WayFindStart(GoalsCoordinate);
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
					}

					LogD(_T("放弃当前目标，前往下一个坐标"));
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


		//采集部分
		int retOperation = CollectionOperation(_T("采集_梦游花|采集_梦游花1|采集_梦游花2|采集_梦游花3|采集_梦游花4|采集_梦游花5"));
		if (retOperation == CollectionOperationSTATE::STATE_empty) {
			IsNext = true;
			LogD(_T("未找到目标"));
		}
		else if (retOperation == CollectionOperationSTATE::STATE_Done) {
			IsNext = true;
			LogD(_T("采集完成"));
		}
		else if (retOperation == CollectionOperationSTATE::STATE_Attack) {
			m_pLifeModule->ClickRectangleA();
			m_pLifeModule->CleanMonster();
			LogD(_T("被攻击，重新返回坐标"));
			IsNext = false;
			iAttackTime++;
			if (iAttackTime >= 3) {
				LogD(_T("被攻击次数较多 放弃当前坐标:%d,%d"), GoalsCoordinate.x, GoalsCoordinate.y);
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
		LogD(_T("一轮此结束 重新结算坐标"));
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
				}
			}
		}

		CurrentCoordinate = m_pCWayFind->GetGamePoint(0.8);
		int Index = m_pCWayFind->computation_distance_Sort(CurrentCoordinate, m_pGameMapInfo->wayFindCoordinate);
		if (Index == -1) {
			LogD(_T("计算所有坐标失败"));
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
		if (!FIND(_T("生活状态")))
			m_pTool->KeyPress(VK_B);

		if (WFIND(_T("生活状态"))) {
			SLEEPEX(1000);
			if (WFIND(_T("生活体力值还有"))) {
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

	//第一次采集
	RCLICK(lickPoint);
	int tagTime = m_pTool->GetTimeSeconds();
	while (true)
	{
		if (FIND(_T("正在砍伐"))) {
			LogD(_T("正在采集"));
			break;
		}

		LogD(_T("检查采集：%d"), int((m_pTool->GetTimeSeconds() - tagTime) / 1000));
		if (m_pLifeModule->checkMonster(g_pEngine->disregardLifeAttack))
			return CollectionOperationSTATE::STATE_Attack;

		if ((m_pTool->GetTimeSeconds() - tagTime) > 4000)
			return CollectionOperationSTATE::STATE_Timeout;

		/*SLEEPEX(1000);*/
	}

	LogD(_T("运行4"));
	//第二次采集
	bool IsSecond = false;
	tagTime = m_pTool->GetTimeSeconds();
	while (true)
	{
		if (!WFINDTIME(_T("正在砍伐"), 1500))
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

		LogD(_T("正在采集：%d"), int((m_pTool->GetTimeSeconds() - tagTime) / 1000));


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
	if (FIND(_T("装备需要修理")))
	{
		while (true)
		{
			LogD(_T("装备需要修理"));
			if (g_pEngine->m_tagToolMaintenanceMode == 1) {
				if (m_pToolMaintenance->ToolMaintenance(NULL, false, false)) {
					MaintenanceSuccess = true;
					IsTeleportBack = true;
					break;
				}
				else
				{
					IsTeleportBack = true;
					LogD(_T("默认城市维修失败"));
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
					LogD(_T("默认城市维修失败"));
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
						LogD(_T("地图维修失败返回默认城市维修"));
						continue;
					}
				}
				else{
					LogD(_T("当前执行的地图:%s 维修工坐标为空 执行默认传送维修区域"), m_pGameMapInfo->TeleportName);
					LogD(_T("维修失败返回默认城市维修"));
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
					LogD(_T("宠物维修失败返回默认城市维修"));
					g_pEngine->m_tagToolMaintenanceMode == 2;
				}
			}
		}

		while (MaintenanceSuccess)
		{
			if (IsTeleportBack) {
				LogD(_T("传送回默认坐标"));
				if (m_pGameTeleport->Teleport(m_pGameMapInfo->TeleportName)) {
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