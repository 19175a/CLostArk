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
	if (!FIND(_T("װ����Ҫ����")))
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

		//�����﹥�����ߵ�ס
		if (m_pLifeModule->checkMonster()) {
			m_pLifeModule->ClickRectangleA();
			m_pLifeModule->CleanMonster();
		}
	}

	if (!isDone)
		return false;

	pWayFind->waitMovStop(3000);
	CPoint MaintenanceLogo{ NULL,NULL };
	MaintenanceLogo = m_pTool->FindMS(_T("����������ʾͼ��|����������ʾͼ��1"));
	if (MaintenanceLogo.x != NULL) {
		m_pTool->RClick(CPoint(MaintenanceLogo.x + 10, MaintenanceLogo.y + 20));
	}
	else {
		return false;
	}

	if (!WOCRFINDCLICK(_T("�������_�����_�������װ��"))) {
	}

	if (!m_pTool->WClick(_T("�����ͼ��"))) {
		return false;
	}

	if (!WOCRFINDCLICK(_T("�������_�����_����ά��"))) {
		return false;
	}
	if (!WOCRFINDCLICK(_T("�������_�����_����ά��_ȷ��"))) {
		return false;
	}

	m_pLifeModule->BackMainScreen();
	return true;
}
bool CLife_ToolMaintenance::PetsMaintenanceTool()
{
	m_pLifeModule->BackMainScreen();

	if (!m_pTool->WClick(_T("���½�_ָ��ͼ��"))) {
		LogD(_T("���½�_ָ��ͼ��"));
		return false;
	}
	if (!m_pTool->WClick(_T("����ͼ��"))) {
		LogD(_T("����ͼ��"));
		return false;
	}
	if (!m_pTool->WClick(_T("�������_Զ��ά�޹���ͼ��"))) {
		LogD(_T("�������_Զ��ά�޹���ͼ��"));
		return false;
	}
	if (!m_pTool->WClick(_T("����_�������_�����_����ά��"))) {
		LogD(_T("����_�������_�����_����ά��"));
		return false;
	}
	if (!m_pTool->WClick(_T("����_�������_�����_����ά��_ȷ��"))) {
		LogD(_T("����_�������_�����_����ά��_ȷ��"));
		return false;
	}

	return true;
}
bool CLife_ToolMaintenance::ActionPurchase(int iType, bool IsSpecial, bool IsClean)
{
	m_pLifeModule->BackMainScreen();

	//�����Լ����ϵĹ���
	bool IsExist = false;
	for (size_t i = 0; i < 3; i++) /*1�ɼ� 2��ľ 3�ɿ� 4���� 5���� 6����*/
	{
		if (!m_pTool->WFind(_T("�����_����"), NULL)) {
			m_pTool->KeyPress(VK_L);
			if (!m_pTool->WFind(_T("�����_����"))) {
				LogD(_T("�޷���:�����_����"));
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
		strUi = _T("��Ʒ����__�ɼ�����");

	}
	else if (iType == 2) {
		m_pTool->RClick(CPoint(645, 267));
		strUi = _T("��Ʒ����_��ľ����");
	}
	else if (iType == 3) {
		m_pTool->RClick(CPoint(684, 264));
		strUi = _T("��Ʒ����_�ɿ󹤾�");
	}
	else if (iType == 4) {
		m_pTool->RClick(CPoint(726, 258));
		strUi = _T("��Ʒ����_���Թ���");
	}
	else if (iType == 5) {
		m_pTool->RClick(CPoint(767, 258));
		strUi = _T("��Ʒ����_���㹤��");
	}
	else if (iType == 6) {
		m_pTool->RClick(CPoint(813, 260));
		strUi = _T("��Ʒ����_���Ź���");
	}
	else {
		LogD(_T("iType == 0"));
		return false;
	}
	m_pTool->SleepEx(800);
	m_pTool->KeyPress(VK_ESC); //�رմ���
	m_pTool->SleepEx(800);


	for (size_t i = 0; i < 15; i++)
	{
		LogD(_T("��ִ�д�����%d"), i);
		CPoint pTagPoint = CPoint(NULL, NULL);
		CPoint retPoint = CPoint(NULL, NULL);

		//�������Ƿ��пռ�
		if (!m_pTool->WFindRetPoint(_T("��Ʒ_����"), pTagPoint, NULL)) {
			m_pTool->KeyPress(VK_I);
			if (!m_pTool->WFindRetPoint(_T("��Ʒ_����"), pTagPoint)) {
				LogD(_T("�޷���:��Ʒ_����"));
				continue;
			}
		}

		//�������Ƿ���δ������
		std::vector<cv::Point> retOpencvPoint = m_pTool->OpencvFindPicEx(strUi, CRect(pTagPoint.x - 170, pTagPoint.y, pTagPoint.x + 200, pTagPoint.y + 423));
		for (size_t i = 0; i < retOpencvPoint.size(); i++)
		{
			m_pTool->Click(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
			m_pTool->SleepEx(300);

			if (m_pTool->Find(_T("�����_�;ö���"), retPoint)) {

				//������ж��Ƿ�IsSpecial
				if (IsSpecial) {

					m_pTool->SleepEx(300);
					if (strUi == _T("��Ʒ����_���㹤��")) {
						LogD(_T("���㹤�����軤������ ֱ�Ӹ���"));
						m_pTool->RClick(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
						m_pTool->KeyPress(VK_ESC);
						return true;
					}

					if (m_pTool->Find(_T("��������"), CRect(retPoint.x, retPoint.y, retPoint.x + 169, retPoint.y + 125))) {
						m_pTool->RClick(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y));
						m_pTool->KeyPress(VK_ESC);
						return true;
					}
					else {
						if (IsClean) {//�϶�����
							LogD(_T("����"));
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
				if (IsClean) {//�϶�����
					m_pTool->LeftClickDrag(CPoint(retOpencvPoint[i].x, retOpencvPoint[i].y), CPoint(584, 351));
					m_pTool->SleepEx(200);
					m_pTool->Click(CPoint(pTagPoint.x - 26, pTagPoint.y + 233));
					m_pTool->SleepEx(800);
				}

			}

		}


		m_pTool->Click(CPoint(pTagPoint.x-161, pTagPoint.y+27)); //�ϲ�������Ʒ
		m_pTool->SleepEx(800);
		m_pTool->Click(CPoint(pTagPoint.x - 161, pTagPoint.y + 27)); //�ϲ�������Ʒ
		m_pTool->SleepEx(800);

		if (!m_pTool->Find(_T("��Ʒ_�ո���"),CRect(pTagPoint.x-170, pTagPoint.y, pTagPoint.x+200, pTagPoint.y+423))) {
			m_pTool->Click(CPoint(pTagPoint.x-105, pTagPoint.y+405));//�ֽ�߼�-ϡ��-Ӣ��װ����Ʒ
			m_pTool->SleepEx(200);
			m_pTool->Click(CPoint(pTagPoint.x-508, pTagPoint.y+360));
			m_pTool->SleepEx(200);
			m_pTool->Click(CPoint(pTagPoint.x - 450, pTagPoint.y + 360));
			m_pTool->SleepEx(200);
			m_pTool->Click(CPoint(pTagPoint.x - 397, pTagPoint.y + 360));
			m_pTool->SleepEx(200);

			//�ֽ�
			m_pTool->Click(CPoint(pTagPoint.x - 245, pTagPoint.y + 394));
			m_pTool->SleepEx(800);
			m_pTool->Click(CPoint(pTagPoint.x - 395, pTagPoint.y + 235));
			m_pTool->SleepEx(800);
			m_pTool->Click(CPoint(pTagPoint.x - 195, pTagPoint.y)); //�رշֽ�
		}

		m_pTool->SleepEx(800);
		if (!m_pTool->Find(_T("��Ʒ_�ո���"), CRect(pTagPoint.x - 170, pTagPoint.y, pTagPoint.x + 200, pTagPoint.y + 423))) {
			LogD(_T("����Ʒ_�ո���"));
			return false;
		}
		
		if (m_pTool->Find(_T("���������_��"), retPoint)) {
			m_pTool->RClick(CPoint(retPoint.x + 40, retPoint.y + 50));
			if (!m_pTool->WFind(_T("���������_�̵����"))) {
				LogD(_T("���̵����"));
				continue;
			}
		}
		else { LogD(_T("���̵����")); continue; }

		
		//���� /*1�ɼ� 2��ľ 3�ɿ� 4���� 5���� 6����*/
		if (iType == 1) {
			m_pTool->Find(_T("�̵�_�ɼ�����"), retPoint);
		}
		else if (iType == 2) {
			m_pTool->Find(_T("�̵�_��ľ����"), retPoint);
		}
		else if (iType == 3) {
			m_pTool->Find(_T("�̵�_�ɿ󹤾�"), retPoint);
		}
		else if (iType == 4) {
			m_pTool->Find(_T("�̵�_���Թ���"), retPoint);
		}
		else if (iType == 5) {
			m_pTool->Find(_T("�̵�_���㹤��"), retPoint);
		}
		else if (iType == 6) {
			m_pTool->Find(_T("�̵�_���Ź���"), retPoint);
		}

		if (retPoint.x == NULL) {
			LogD(_T("δ������Ʒ����: %d"), iType);
			continue;
		}

		m_pTool->SleepEx(800);
		m_pTool->RClick(retPoint);//ѡ�� ����
		m_pTool->SleepEx(800);
		m_pTool->Click(CPoint(402, 561));
		m_pTool->SleepEx(800);

		m_pTool->KeyPress(VK_ESC); //���´���Ʒ����
		m_pTool->SleepEx(800);
		if (!m_pTool->WFindRetPoint(_T("��Ʒ_����"), pTagPoint, NULL)) {
			m_pTool->KeyPress(VK_I);
			if (!m_pTool->WFindRetPoint(_T("��Ʒ_����"), pTagPoint)) {
				LogD(_T("�޷���:��Ʒ_����"));
				continue;
			}
		}

	}

}

bool CLife_ToolMaintenance::checkToolExistence(int iType) 
{
	m_pLifeModule->BackMainScreen(); 
	bool Existence = false;
	for (size_t i = 0; i < 3; i++) /*1�ɼ� 2��ľ 3�ɿ� 4���� 5���� 6����*/
	{
		if (!m_pTool->WFind(_T("�����_����"), NULL)) {
			m_pTool->KeyPress(VK_L);
			if (!m_pTool->WFind(_T("�����_����"))) {
				LogD(_T("�޷���:�����_����"));
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
		if (m_pTool->WFind(_T("����ܹ��߿�_�ɼ�"), NULL))
			return true;
	}
	else if (iType == 2)
	{
		if (m_pTool->WFind(_T("����ܹ��߿�_��ľ"), NULL))
			return true;
	}
	else if (iType == 3)
	{
		if (m_pTool->WFind(_T("����ܹ��߿�_�ɿ�"), NULL))
			return true;
	}
	else if (iType == 4)
	{
		if (m_pTool->WFind(_T("����ܹ��߿�_����"), NULL))
			return true;
	}
	else if (iType == 5)
	{
		if (m_pTool->WFind(_T("����ܹ��߿�_����"), NULL))
			return true;
	}
	else if (iType == 6)
	{
		if (m_pTool->WFind(_T("����ܹ��߿�_����"), NULL))
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