#include "pch.h"
#include"CTaskScript.h"
#include "CByPost.h"
#include"CDCMEngine.h"
#include"CGameTeleport.h"
#include"CWayFind.h"
#include"obj.h"
extern CDCMEngine* g_pEngine;
CByPost::CByPost()
{

}
CByPost::~CByPost()
{
	if (m_pByPostInfo)
		delete m_pByPostInfo;
	if (m_pGameMapInfo)
		delete m_pGameMapInfo;
	if (m_pWayFind)
		delete m_pWayFind;
	if (m_pLifeModule)
		delete m_pLifeModule;
	if (m_pGameTeleport)
		delete m_pGameTeleport;
}
void CByPost::Init(CTaskScript* pTool)
{
	m_pTool = pTool;
	m_pByPostInfo = new tagByPostInfo();
	if (!LoadInfo()) {
		LogD(_T("!!!CByPost Init"));
		return;
	}

	m_pGameMapInfo = new tagGameMapInfo();
	m_pGameMapInfo->mapBmpName = m_pByPostInfo->strMapName;
	m_pGameMapInfo->astarBmpName = m_pByPostInfo->strAstarBmpName;
	m_pGameMapInfo->tagUsefulCoordinate = m_pByPostInfo->pByPostCoordinate;

	m_pWayFind = new CWayFind();
	m_pWayFind->Init(m_pTool, m_pGameMapInfo);
	m_pLifeModule = new CLifeModule(m_pTool);
	m_pGameTeleport = new CGameTeleport();
	m_pGameTeleport->Init(m_pTool);
	m_pGameTeleport->Load();
}

bool CByPost::ToGoal()
{
	if (!m_pGameTeleport->Teleport(m_pByPostInfo->strTeleportName)) {
		LogD(_T("!!!CByPost Teleport failure"));
		return false;
	}

	if (m_pWayFind->WayFindStart(m_pByPostInfo->pByPostCoordinate) != CWayFind::WayFindState::STATE_Done) {
		LogD(_T("!!!CByPost WayFindStart failure"));
		return false;
	}

	return true;
}

bool CByPost::LoadInfo()
{
	CString strMapName = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//OtherData.ini"), _T("ByPost_Life"), _T("MapName"));
	if (strMapName == _T(""))
		return false;

	CString strABmpName = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//OtherData.ini"), _T("ByPost_Life"), _T("ABmpName"));
	if (strABmpName == _T(""))
		return false;

	CString strByPostCoordinate = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//OtherData.ini"), _T("ByPost_Life"), _T("ByPostCoordinate"));
	if (strByPostCoordinate == _T(""))
		return false;

	CString strkeyLifeName = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//OtherData.ini"), _T("ByPost_Life"), _T("keyLifeName"));
	if (strkeyLifeName == _T(""))
		return false;

	CString strTeleportName = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//OtherData.ini"), _T("ByPost_Life"), _T("TeleportName"));
	if (strTeleportName == _T(""))
		return false;

	m_pByPostInfo->strMapName = strMapName;
	m_pByPostInfo->strAstarBmpName = strABmpName;
	m_pByPostInfo->strTeleportName = strTeleportName;
	m_pByPostInfo->pByPostCoordinate = m_pTool->strToPoint(strByPostCoordinate);
	
	CStringArray arrAy;
	Split(strkeyLifeName, arrAy, _T("|"));
	if (!arrAy.GetCount())
		return false;

	for (size_t i = 0; i < arrAy.GetCount(); i++)
	{
		CString str = m_pTool->iniGetValue(g_pEngine->m_strWorkPath + _T("//OtherData.ini"), _T("ByPost_Life_bmpName"), arrAy[i]);
		std::vector<CString > tmpv = {};
		CStringArray tmpArr;
		Split(str, tmpArr, _T("|"));
		for (size_t k = 0; k < tmpArr.GetCount(); k++)
		{
			tmpv.push_back(tmpArr[k]);
		}
		m_pByPostInfo->mapLife.insert(std::make_pair(arrAy[i], tmpv));
	}

	if (!m_pByPostInfo->mapLife.size())
		return false;

	return true;
}

bool CByPost::ByPost(CString strTmpName)
{
	m_pLifeModule->BackMainScreen();
	CPoint tmpPoint = CPoint(NULL, NULL);
	if (!m_pTool->Find(_T("邮寄_邮寄员"), tmpPoint))
		return false;

	m_pTool->RClick(CPoint(tmpPoint.x, tmpPoint.y + 65));
	m_pTool->SleepEx(800);

	if (!m_pTool->WClick(_T("邮寄_邮件界面")))
		return false;

	if(!m_pTool->WClick(_T("邮件界面_写信图标")))
		return false;

	m_pTool->SleepEx(1000);

	
	m_pTool->m_pDm->SendString((long)m_pTool->m_pWnd->hWnd, strTmpName);
	m_pTool->SleepEx(1000);
	m_pTool->Click(CPoint(630, 180));
	m_pTool->SleepEx(1000);
	m_pTool->Click(CPoint(629, 234));

	for (auto it = m_pByPostInfo->mapLife.begin(); it != m_pByPostInfo->mapLife.end(); it++)
	{
		for (size_t i = 0; i < 15; i++) {
			if (!m_pTool->FindClick(_T("邮寄_已打开箭头")))
				break;
			m_pTool->SleepEx(1000);
		}

		//节点类型
		if (!m_pTool->FindClick(it->first)) {
			LogD(_T("未找到：%x"), it->first);
			continue;
		}
			
		
		m_pTool->SleepEx(1000);
		m_pTool->Click(CPoint(840, 532));//点击页面往下
		m_pTool->SleepEx(1000);
		for (size_t k = 0; k < it->second.size(); k++)
		{
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(1000);
			if (m_pTool->FindRClick(it->second[k])) {
				m_pTool->SleepEx(1000);
				if (!m_pTool->Find(_T("邮寄_输入数量确定"))) {
					continue;
				}

				m_pTool->Click(CPoint(675, 384));//输入数量
				m_pTool->SleepEx(200);
				for (size_t m = 0; m < 6; m++){
					m_pTool->KeyPress(49);
					m_pTool->SleepEx(200);
				}
				m_pTool->MoveTo(0, 0);
				m_pTool->SleepEx(1000);
				if (!m_pTool->WClick(_T("邮寄_输入数量确定"))) {
					LogD(_T("邮寄操作异常"));
				}

				m_pTool->SleepEx(1000);
			}

		}

	}

	if (!m_pTool->WClick(_T("邮寄_发送"))) {
		LogD(_T("邮寄发送失败"));
		return false;
	}

	m_pTool->SleepEx(10000);
	return true;
}
bool CByPost::ByPoststarted(CString strTmpName)
{
	if(!ToGoal())
		return false;

	if(!ByPost(strTmpName))
		return false;

	return true;
}