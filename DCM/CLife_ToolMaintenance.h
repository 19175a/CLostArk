#pragma once
#include"CTaskScript.h"
//#include"CWayFind.h"
//#include"CLifeModule.h"
//#include"CGameTeleport.h"

class CLifeModule;
class CWayFind;
class CTaskScript;
class CGameTeleport;
class CLife_ToolMaintenance
{
public:
	struct tagToolMaintenanceInfo {
		CString strMapBmpName = _T("");
		CString	strAstarBmpName = _T("");
		CString strTeleportName = _T("");
		CPoint  pMaintenanceCoordinate = CPoint(NULL, NULL);
	};

	CLife_ToolMaintenance();
	~CLife_ToolMaintenance();
	void Init(CTaskScript* pTaskScript, CString strMapName);
	
	//iType = 1采集 2伐木 3采矿 4狩猎 5钓鱼 6考古  0表示所有
	//IsSpecial = 重新购买(霸体(钓鱼除外)绿色等级 不是的则丢掉)
	//IsClean = 重新购买(绿色等级 不会丢掉原本的)
	bool ToolMaintenance(int iType, bool IsSpecial, bool IsClean);				
	bool MapMaintenanceTool(CWayFind* pWayFind, tagGameMapInfo* pGameMapInfo);			//地图维修工具
	bool PetsMaintenanceTool();															//宠物维修工具//购买行为
	bool checkToolExistence(int iType =NULL); //iType = 1采集 2伐木 3采矿 4狩猎 5钓鱼 6考古  0表示所有

public:
	bool ActionPurchase(int iType/*1采集 2伐木 3采矿 4狩猎 5钓鱼 6考古*/, bool IsSpecial,bool IsClean);	
	bool ToMaintenance();
	bool LoadInfo(CString strMapName);


private:
	bool checkTool(int iType);
	
	tagGameMapInfo* m_pGameMapInfo = nullptr;
	tagToolMaintenanceInfo* m_pToolMaintenanceInfo = nullptr; 
	CGameTeleport* m_pGameTeleport = nullptr;
	CTaskScript* m_pTool = nullptr;
	CWayFind* m_pWayFind = nullptr;
	CLifeModule* m_pLifeModule = nullptr;
};

