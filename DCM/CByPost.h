#pragma once
#include"CTaskScript.h"
//#include"CWayFind.h"
//#include"CLifeModule.h"
//#include"CGameTeleport.h"

class CLifeModule;
class CWayFind;
class CTaskScript;
class CGameTeleport;
class CByPost
{
public:
	CByPost();
	~CByPost();
	void Init(CTaskScript* pTool);
public:
	bool ByPoststarted(CString strTmpName);
private:
	bool ByPost(CString strTmpName);
	bool ToGoal();
	bool LoadInfo();
private:
	struct tagByPostInfo
	{
		CString strMapName = _T("");
		CString strAstarBmpName = _T("");
		CString strTeleportName = _T("");
		CPoint  pByPostCoordinate = CPoint(NULL, NULL);
		std::map<CString, std::vector<CString>> mapLife{}; //Key¿‡–Õ£¨ValueÕº∆¨√˚"|"
	};

	tagByPostInfo* m_pByPostInfo = NULL;
	tagGameMapInfo* m_pGameMapInfo = NULL;
	CGameTeleport* m_pGameTeleport = NULL;
	CTaskScript* m_pTool = NULL;
	CWayFind* m_pWayFind = NULL;
	CLifeModule* m_pLifeModule = NULL;
};

