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
	bool Teleport(CString strTargetCitiesName, bool IsDisregard = false/*���ӵ�ǰ����������Ȼ����*/);
	CString GetCurrentCities(CString strCurrentInsideName); //strCurrentInsideName = ��ǰ����ִ������ĵ�ͼ��������

private:
	CTaskScript* m_pTool;
	std::vector<tagGameTeleportInfo*>* m_pvGameTeleportInfo;
	std::vector<tagTeleportIslandsinfo*>* m_pvTeleportIslandsinfo;
	CLifeModule* m_pLifeModule = nullptr;
};

