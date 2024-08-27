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
	
	//iType = 1�ɼ� 2��ľ 3�ɿ� 4���� 5���� 6����  0��ʾ����
	//IsSpecial = ���¹���(����(�������)��ɫ�ȼ� ���ǵ��򶪵�)
	//IsClean = ���¹���(��ɫ�ȼ� ���ᶪ��ԭ����)
	bool ToolMaintenance(int iType, bool IsSpecial, bool IsClean);				
	bool MapMaintenanceTool(CWayFind* pWayFind, tagGameMapInfo* pGameMapInfo);			//��ͼά�޹���
	bool PetsMaintenanceTool();															//����ά�޹���//������Ϊ
	bool checkToolExistence(int iType =NULL); //iType = 1�ɼ� 2��ľ 3�ɿ� 4���� 5���� 6����  0��ʾ����

public:
	bool ActionPurchase(int iType/*1�ɼ� 2��ľ 3�ɿ� 4���� 5���� 6����*/, bool IsSpecial,bool IsClean);	
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

