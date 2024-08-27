#pragma once
#include"CTaskScript.h"
#include"CLifeModule.h"
class CTaskScript;
class CLifeModule;
class CGameTeleport
{
public:
	CGameTeleport();
	~CGameTeleport();
	void Init(CTaskScript * pTool);
	bool Load();
	bool Teleport(CString strTargetCitiesName, bool IsDisregard = false/*无视当前所在坐标依然传送*/);
	CString GetCurrentCities(CString strCurrentInsideName); //strCurrentInsideName = 当前所在执行生活的地图传送名字

private:
	CTaskScript* m_pTool;
	std::vector<tagGameTeleportInfo*>* m_pvGameTeleportInfo;
	std::vector<tagTeleportIslandsinfo*>* m_pvTeleportIslandsinfo;
	CLifeModule* m_pLifeModule = nullptr;
};

