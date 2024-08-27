#pragma once
#include"CTaskScript.h"
//#include "CWayFind.h"
//#include"CGameTeleport.h"
//#include"CLife_ToolMaintenance.h"
//#include "CLifeModule.h"
class CLifeModule;
class CWayFind;
class CGameTeleport;
class CLife_ToolMaintenance;
class CTaskScript;

class CLife_Fish
{
public:
	CLife_Fish();
	~CLife_Fish();
	bool Init(CTaskScript* pTaskScript, CString mapName);
	int  started();

private:
	struct tagLife_FishInfo
	{
		CPoint pFishCoordinate = CPoint(NULL, NULL);
		CPoint pFishDirectionCoordinate = CPoint(NULL, NULL);
		CRect rIdentifyBuoyArea = CRect(NULL, NULL, NULL, NULL);
	};

	tagLife_FishInfo Life_FishInfo;
	CTaskScript* m_pTool = nullptr;
	CLifeModule* m_pLifeModule = nullptr;
	CWayFind* m_pCWayFind = nullptr;
	tagGameMapInfo* m_pGameMapInfo{};
	CGameTeleport* m_pGameTeleport = nullptr;
	CLife_ToolMaintenance* m_pToolMaintenance = nullptr;
	bool  ClickMaintenance(int& iRetState); //iRetState =2 需要返回钓鱼地点
	bool initGameInfo(CString mapName);
	void Tset();
};

