#pragma once

#include"CTaskScript.h"


class CTaskScript;
class CWayFind;
class CLifeModule
{
public:

	CLifeModule(CTaskScript* pTaskScript);
	~CLifeModule();

public:
	CTaskScript* m_pTool;
	dmsoft* m_pDm  = nullptr;
	void Tset();

	bool MaintenanceTool(CWayFind* pWayFind, CPoint MaintenanceCoordinate);
	bool checkMonster(bool disregardLifeAttack = false);
	void ClickRectangleA();
	void CleanMonster();
	bool BackMainScreen();
	void CleanScreen();
	bool checkDeath();
	bool checkPhysicalStrength();	//ºÏ≤ÈÃÂ¡¶

	
};

