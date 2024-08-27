#pragma once
#include"CTaskScript.h"
//#include "CWayFind.h"
//#include"CGameTeleport.h"
//#include"CLife_ToolMaintenance.h"
//#include "CLifeModule.h"
class CTaskScript;
class CLifeModule;
class CWayFind;
class CGameTeleport;
class CLife_ToolMaintenance;
class CLife_Collection
{
	enum CollectionState
	{
		STATE_Init_failure	 = 0,
		STATE_DONE			 =1,
		STATE_distance_Sort_failure = 2
	};
	enum CollectionOperationSTATE
	{
		STATE_empty = -1,
		STATE_Done = 1,
		STATE_Attack =2,
		STATE_Strength_empty =3,
		STATE_Timeout =4
	};


public:
	CLife_Collection();
	~CLife_Collection();

public:
	bool Init(CTaskScript* pTaskScript, CString mapName);
	CTaskScript* m_pTool = nullptr;
	CLifeModule* m_pLifeModule	= nullptr;
	CWayFind* m_pCWayFind = nullptr;
	tagGameMapInfo* m_pGameMapInfo{};
	CGameTeleport* m_pGameTeleport = nullptr;
	CLife_ToolMaintenance* m_pToolMaintenance = nullptr;

public:
	void Tset();
	int  started();
	bool initGameInfo(CString mapName);
	int  Collection_Carry();
	bool  ClickMaintenance();
	int  CollectionOperation(CString strArrUI);//"|"·Ö¸î

	int m_IndexCoordinate = NULL;
	CPoint GetNextWayFindCoordinate(std::vector<CPoint>& vCoordinate);

private:

};

