#include "pch.h"
#include"CTaskScript.h"
#include "CLife_ToolMaintenance.h"
#include"CDCMEngine.h"
#include"CGameTeleport.h"
#include"CWayFind.h"

extern CDCMEngine* g_pEngine;
CLife_ToolMaintenance::CLife_ToolMaintenance()
{

}
CLife_ToolMaintenance::~CLife_ToolMaintenance()
{
	if (m_pGameMapInfo)
		delete m_pGameMapInfo;
	if (m_pWayFind)
		delete m_pWayFind;
	if (m_pLifeModule)
		delete m_pLifeModule;
	if (m_pGameTeleport)
		delete m_pGameTeleport;

}

void CLife_ToolMaintenance::Init(CTaskScript* pTaskScript, CString strMapName)
{


	m_pTool = pTaskScript;
	m_pToolMaintenanceInfo = new tagToolMaintenanceInfo();

	if (!LoadInfo(strMapName)) {
		LogD(_T("CLife_ToolMaintenance !!!Init"));
		return;
	}

	m_pGameMapInfo = new tagGameMapInfo();
	m_pGameMapInfo->mapBmpName = m_pToolMaintenanceInfo->strMapBmpName;
	m_pGameMapInfo->astarBmpName = m_pToolMaintenanceInfo->strAstarBmpName;
	m_pGameMapInfo->tagUsefulCoordinate = m_pToolMaintenanceInfo->pMaintenanceCoordinate;
	m_pWayFind = new CWayFind();
	m_pWayFind->Init(m_pTool, m_pGameMapInfo);
	m_pLifeModule = new CLifeModule(pTaskScript);
	m_pGameTeleport = new CGameTeleport();
	m_pGameTeleport->Init(pTaskScript);
	m_pGameTeleport->Load();
}

bool CLife_ToolMaintenance::ToolMaintenance(int iType, bool IsSpecial, bool IsClean)
{
	if (!ToMaintenance())
		return false;

	if (iType == NULL) {
		for (size_t i = 1; i < 7; i++)
		{
			if (!ActionPurchase(i, true, true))
				return false;

			m_pLifeModule->BackMainScreen();
			if (m_pWayFind->WayFindStart(m_pToolMaintenanceInfo->pMaintenanceCoordinate) != CWayFind::WayFindState::STATE_Done) {
				LogD(_T("!!!ToMaintenance WayFindStart failure"));
				return false;
			}
		}
	}
	else
	{
		if (!ActionPurchase(iType, true, true))
			return false;
	}

	return true;
}

bool CLife_ToolMaintenance::MapMaintenanceTool(CWayFind* pWayFind, tagGameMapInfo* pGameMapInfo)
{
	m_pLifeModule->BackMainScreen();
	if (!FIND(_T("装备需要修理")))
		return true;


	if (!pGameMapInfo->RepairmanCoordinates.x)
		return false;

	constexpr auto Max = 3;
	bool isDone = false;
	for (size_t i = 0; i < Max; i++)
	{
		int iWayRet = pWayFind->WayFindStart(pGameMapInfo->RepairmanCoordinates);
		if (iWayRet == CWayFind::STATE_Done) {
			isDone = true;	break;
		}
		else if (iWayRet == CWayFind::STATE_CDT_Useless) {
			pWayFind->detachmentStuck();
		}

		//检测怪物攻击或者挡住
		if (m_pLifeModule->checkMonster()) {
			m_pLifeModule->ClickRectangleA();
			m_pLifeModule->CleanMonster();
		}
	}

	if (!isDone)
		return false;

	pWayFind->waitMovStop(3000);
	CPoint MaintenanceLogo{ NULL,NULL };
	MaintenanceLogo = m_pTool->FindMS(_T("修理工文字提示图标|修理工文字提示图标1"));
	if (MaintenanceLogo.x != NULL) {
		m_pTool->RClick(CPoint(MaintenanceLogo.x + 10, MaintenanceLogo.y + 20));
	}
	else {
		return false;
	}

	if (!WOCRFINDCLICK(_T("修理道具_生活工具_修理佩戴装备"))) {
	}

	if (!m_pTool->WClick(_T("生活工具图标"))) {
		return false;
	}

	if (!WOCRFINDCLICK(_T("修理道具_生活工具_批量维修"))) {
		return false;
	}
	if (!WOCRFINDCLICK(_T("修理道具_生活工具_批量维修_确定"))) {
		return false;
	}

	m_pLifeModule->BackMainScreen();
	return true;
}
bool CLife_ToolMaintenance::PetsMaintenanceTool()
{
	m_pLifeModule->BackMainScreen();

	if (!m_pTool->WClick(_T("右下角_指南图标"))) {
		LogD(_T("右下角_指南图标"));
		return false;
	}
	if (!m_pTool->WClick(_T("宠物图标"))) {
		LogD(_T("宠物图标"));
		return false;
	}
	if (!m_pTool->WClick(_T("宠物界面_远程维修工具图标"))) {
		LogD(_T("宠物界面_远程维修工具图标"));
		return false;
	}
	if (!m_pTool->WClick(_T("宠物_修理道具_生活工具_批量维修"))) {
		LogD(_T("宠物_修理道具_生活工具_批量维修"));
		return false;
	}
	if (!m_pTool->WClick(_T("宠物_修理道具_生活工具_批量维修_确定"))) {
		LogD(_T("宠物_修理道具_生活工具_批量维修_确定"));
		return false;
	}

	return true;
}
bool CLife_ToolMaintenance::ActionPurchase(int iType, bool IsSpecial, bool IsClean)
{
	m_pLifeModule->BackMainScreen();

	//脱下自己身上的工具
	bool IsExist = false;
	for (size_t i = 0; i < 3; i++) /*1采集 2伐木 3采矿 4狩猎 5钓鱼 6考古*/
	{
		if (!m_pTool->WFind(_T("生活技能_界面"), NULL)) {
			m_pTool->KeyPress(VK_L);
			if (!m_pTool->WFind(_T("生活技能_界面"))) {
				LogD(_T("无法打开:生活技能_界面"));
				continue;
			}

			SLEEPEX(300);
			IsExist = true;
		}
		else
		{
			break;
		}

	}
	if (!IsExist)
		return false;

	CString strUi = _T("");

	if (iType == 1) {
		m_pTool->RClick(CPoint(597, 264));
		strUi = _T("物品背包__采集工具");

	}
	else if (iType == 2) {
		m_pTool->RClick(CPoint(645, 267));
		strUi = _T("物品背包_伐木工具");
	}
	else if (iType == 3) {
		m_pTool->RClick(CPoint(684, 264));
		strUi = _T("物品背包_采矿工具");
	}
	else if (iType == 4) {
		m_pTool->RClick(CPoint(726, 258));
		strUi = _T("物品背包_狩猎工具");
	}
	else if (iType == 5) {
		m_pTool->RClick(CPoint(767, 258));
		strUi = _T("物品背包_钓鱼工具");
	}
	else if (iType == 6) {
		m_pTool->RClick(CPoint(813, 260));
		strUi = _T("物品背包_考古工具");
	}
	else {
		LogD(_T("iType == 0"));
		return false;
	}
	m_pTool->SleepEx(800);
	m_pTool->KeyPress(VK_ESC); //关闭窗口
	m_pTool->SleepEx(800);


	for (size_t i = 0; i < 15; i++)
	{
		LogD(_T("已执行次数：%d"), i);
		CPoint pTagPoint = CPoint(NULL, NULL);
		CPoint retPoint = CPoint(NULL, NULL);

		//检查包裹是否有空间
		if (!m_pTool->WFindRetPoint(_T("物品_界面"), pTagPoint, NULL)) {
			m_pTool->KeyPress(VK_I);
			if (!m_pTool->WFindRetPoint(_T("物品_界面"), pTagPoint)) {
				LogD(_T("无法打开:物品_界面"));
				continue;
			}
		}

		//检查包裹是否有未穿戴的
		std::vector<cv::Point> retOpencvPoint = m_pTool->OpencvFindPicEx(strUi, CRect(pTagPoint.x - 170, pTagPoint.y, pTagPoint.x + 200, pTagPoint.y + 423));
		for (size_t i = 0; i < retOpencvPoint.size(); i++)
		{
			m_pTool->Click(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
			m_pTool->SleepEx(300);

			if (m_pTool->Find(_T("生活工具_耐久度满"), retPoint)) {

				//购买后判断是否IsSpecial
				if (IsSpecial) {

					m_pTool->SleepEx(300);
					if (strUi == _T("物品背包_钓鱼工具")) {
						LogD(_T("钓鱼工具无需护甲免疫 直接更换"));
						m_pTool->RClick(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
						m_pTool->KeyPress(VK_ESC);
						return true;
					}

					if (m_pTool->Find(_T("护甲免疫"), CRect(retPoint.x, retPoint.y, retPoint.x + 169, retPoint.y + 125))) {
						m_pTool->RClick(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
						m_pTool->KeyPress(VK_ESC);
						return true;
					}
					else {
						if (IsClean) {//拖动丢弃
							LogD(_T("丢弃"));
							m_pTool->LeftClickDrag(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y), CPoint(584, 351));
							m_pTool->SleepEx(200);
							m_pTool->Click(CPoint(pTagPoint.x - 26, pTagPoint.y + 233));
							m_pTool->SleepEx(800);
						}
						continue;
					}

				}
				else
				{
					m_pTool->RClick(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
					m_pTool->KeyPress(VK_ESC);
					return true;
				}
			}
			else
			{ //IsClean
				if (IsClean) {//拖动丢弃
					m_pTool->LeftClickDrag(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y), CPoint(584, 351));
					m_pTool->SleepEx(200);
					m_pTool->Click(CPoint(pTagPoint.x - 26, pTagPoint.y + 233));
					m_pTool->SleepEx(800);
				}

			}

		}


		m_pTool->Click(CPoint(pTagPoint.x-161, pTagPoint.y+27)); //合并排序物品
		m_pTool->SleepEx(800);
		m_pTool->Click(CPoint(pTagPoint.x - 161, pTagPoint.y + 27)); //合并排序物品
		m_pTool->SleepEx(800);

		if (!m_pTool->Find(_T("物品_空格子"),CRect(pTagPoint.x-170, pTagPoint.y, pTagPoint.x+200, pTagPoint.y+423))) {
			m_pTool->Click(CPoint(pTagPoint.x-105, pTagPoint.y+405));//分解高级-稀有-英雄装备物品
			m_pTool->SleepEx(200);
			m_pTool->Click(CPoint(pTagPoint.x-508, pTagPoint.y+360));
			m_pTool->SleepEx(200);
			m_pTool->Click(CPoint(pTagPoint.x - 450, pTagPoint.y + 360));
			m_pTool->SleepEx(200);
			m_pTool->Click(CPoint(pTagPoint.x - 397, pTagPoint.y + 360));
			m_pTool->SleepEx(200);

			//分解
			m_pTool->Click(CPoint(pTagPoint.x - 245, pTagPoint.y + 394));
			m_pTool->SleepEx(800);
			m_pTool->Click(CPoint(pTagPoint.x - 395, pTagPoint.y + 235));
			m_pTool->SleepEx(800);
			m_pTool->Click(CPoint(pTagPoint.x - 195, pTagPoint.y)); //关闭分解
		}

		m_pTool->SleepEx(800);
		if (!m_pTool->Find(_T("物品_空格子"), CRect(pTagPoint.x - 170, pTagPoint.y, pTagPoint.x + 200, pTagPoint.y + 423))) {
			LogD(_T("无物品_空格子"));
			return false;
		}
		
		if (m_pTool->Find(_T("生活工具商人_字"), retPoint)) {
			m_pTool->RClick(CPoint(retPoint.x + 40, retPoint.y + 50));
			if (!m_pTool->WFind(_T("生活工具商人_商店界面"))) {
				LogD(_T("无商店界面"));
				continue;
			}
		}
		else { LogD(_T("无商店界面")); continue; }

		
		//购买 /*1采集 2伐木 3采矿 4狩猎 5钓鱼 6考古*/
		if (iType == 1) {
			m_pTool->Find(_T("商店_采集工具"), retPoint);
		}
		else if (iType == 2) {
			m_pTool->Find(_T("商店_伐木工具"), retPoint);
		}
		else if (iType == 3) {
			m_pTool->Find(_T("商店_采矿工具"), retPoint);
		}
		else if (iType == 4) {
			m_pTool->Find(_T("商店_狩猎工具"), retPoint);
		}
		else if (iType == 5) {
			m_pTool->Find(_T("商店_钓鱼工具"), retPoint);
		}
		else if (iType == 6) {
			m_pTool->Find(_T("商店_考古工具"), retPoint);
		}

		if (retPoint.x == NULL) {
			LogD(_T("未发现物品类型: %d"), iType);
			continue;
		}

		m_pTool->SleepEx(800);
		m_pTool->RClick(retPoint);//选中 后购买
		m_pTool->SleepEx(800);
		m_pTool->Click(CPoint(402, 561));
		m_pTool->SleepEx(800);

		m_pTool->KeyPress(VK_ESC); //重新打开物品背包
		m_pTool->SleepEx(800);
		if (!m_pTool->WFindRetPoint(_T("物品_界面"), pTagPoint, NULL)) {
			m_pTool->KeyPress(VK_I);
			if (!m_pTool->WFindRetPoint(_T("物品_界面"), pTagPoint)) {
				LogD(_T("无法打开:物品_界面"));
				continue;
			}
		}

	}

}

bool CLife_ToolMaintenance::checkToolExistence(int iType) 
{
	m_pLifeModule->BackMainScreen(); 
	bool Existence = false;
	for (size_t i = 0; i < 3; i++) /*1采集 2伐木 3采矿 4狩猎 5钓鱼 6考古*/
	{
		if (!m_pTool->WFind(_T("生活技能_界面"), NULL)) {
			m_pTool->KeyPress(VK_L);
			if (!m_pTool->WFind(_T("生活技能_界面"))) {
				LogD(_T("无法打开:生活技能_界面"));
				continue;
			}
			Existence = true;
			SLEEPEX(300);
		}
		else
		{
			break;
			Existence = true;
		}
	}

	if (!Existence) {
		return false;
	}
	Existence = false;
	if (iType == NULL) {
		for (size_t i = 1; i < 7; i++)
		{
			if (checkTool(i)) {
				return false;
			}
		}

		return true;
	}
	else
	{
		if (checkTool(iType))
			return false;
	}


	return true;
}

bool CLife_ToolMaintenance::checkTool(int iType)
{
	if (iType == 1) {
		if (m_pTool->WFind(_T("生活技能工具空_采集"), NULL))
			return true;
	}
	else if (iType == 2)
	{
		if (m_pTool->WFind(_T("生活技能工具空_伐木"), NULL))
			return true;
	}
	else if (iType == 3)
	{
		if (m_pTool->WFind(_T("生活技能工具空_采矿"), NULL))
			return true;
	}
	else if (iType == 4)
	{
		if (m_pTool->WFind(_T("生活技能工具空_狩猎"), NULL))
			return true;
	}
	else if (iType == 5)
	{
		if (m_pTool->WFind(_T("生活技能工具空_钓鱼"), NULL))
			return true;
	}
	else if (iType == 6)
	{
		if (m_pTool->WFind(_T("生活技能工具空_考古"), NULL))
			return true;
	}

	return false;
}


bool CLife_ToolMaintenance::LoadInfo(CString strMapName)
{
	CString strInfo = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//findMapData.ini"), _T("ToolMaintenanceInfo"), strMapName);
	if (strInfo == _T("")) {
		LogD(_T("!!!LoadInfo no %s:"), strInfo);
		return false;
	}

	CStringArray arrInfo;
	Split(strInfo, arrInfo, _T("|"));
	if (arrInfo.GetCount() != 4) {
		LogD(_T("!!!LoadInfo%s:"), strInfo);
		return false;
	}

	m_pToolMaintenanceInfo->strMapBmpName = arrInfo[0];
	m_pToolMaintenanceInfo->strAstarBmpName = arrInfo[1];
	m_pToolMaintenanceInfo->strTeleportName = arrInfo[2];
	m_pToolMaintenanceInfo->pMaintenanceCoordinate = m_pTool->strToPoint(arrInfo[3]);
	return true;
}

bool CLife_ToolMaintenance::ToMaintenance()
{
	if (!m_pGameTeleport->Teleport(m_pToolMaintenanceInfo->strTeleportName)) {
		LogD(_T("!!!ToMaintenance Teleport failure"));
		return false;
	}

	if (m_pWayFind->WayFindStart(m_pToolMaintenanceInfo->pMaintenanceCoordinate) != CWayFind::WayFindState::STATE_Done) {
		LogD(_T("!!!ToMaintenance WayFindStart failure"));
		return false;
	}

	return true;
}