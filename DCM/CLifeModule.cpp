#include "pch.h"
#include "CLifeModule.h"
#include"CDCMEngine.h"
#include "obj.h"
#include"CTaskScript.h"
#include"CWayfind.h"

extern CDCMEngine* g_pEngine;

CLifeModule::CLifeModule(CTaskScript* pTaskScript)
{
	m_pTool = pTaskScript;
	m_pDm = m_pTool->m_pTaskDm;
}
CLifeModule::~CLifeModule()
{

}



void CLifeModule::Tset()
{
	while (true)
	{
		//SLEEPEX(5000);
		BackMainScreen();
	}
}

bool CLifeModule::BackMainScreen()
{
	//��鴰��λ��
	long x1 = NULL, y1 = NULL, x2 = NULL, y2 = NULL;
	m_pTool->m_pDm->GetWindowRect((long)m_pTool->m_pWnd->hWnd, &x1, &y1, &x2, &y2);
	if (x1 != NULL && y1 != NULL) {
		m_pTool->m_pDm->MoveWindow((long)m_pTool->m_pWnd->hWnd, 0, 0);
		m_pTool->SleepEx(500);
	}

	//��鴰���Ƿ����
	if (!IsWindow(m_pTool->m_pWnd->hWnd)) {
		LogD(_T("���ڶ�ʧ ��������"));
		m_pTool->Restart();
	}
	//����Ƿ����
	if (m_pTool->Find(_T("�����ж�"))) {
		LogD(_T("�����ж� ��������"));
		m_pTool->Restart();
	}

	constexpr auto Max = 10;
	for (size_t i = 0; i < Max; i++) {
		CleanScreen();
		if (FIND(_T("������_ͼ��_�ۺϸ���")) && FIND(_T("������_ͼ��_��Ϸ�̳�"))) {
			return true;
		}
		m_pTool->KeyPress(VK_ESC);
		m_pTool->SleepEx(500);
	}

	return false;
}
void CLifeModule::CleanScreen()
{
	bool IsAgain = false;

	for (size_t i = 0; i < 10; i++)
	{
		IsAgain = false;
		
		if (m_pTool->FindClick(_T("���_���"))) {
			IsAgain = true;
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(300);
		}
		if (m_pTool->FindClick(_T("���_���1"))) {
			IsAgain = true;
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(300);

		}
		if (m_pTool->FindClick(_T("���_���2"))) {
			IsAgain = true;
			m_pTool->MoveTo(0, 0);
			m_pTool->SleepEx(300);
		}

		if (!IsAgain)
			break;
	}
}

bool CLifeModule::MaintenanceTool(CWayFind* pWayFind, CPoint MaintenanceCoordinate)
{
	BackMainScreen();
	if(!FIND(_T("װ����Ҫ����")))
		return true;


	if (!MaintenanceCoordinate.x)
		return false;

	constexpr auto Max = 3;
	bool isDone = false;
	for (size_t i = 0; i < Max; i++)
	{
		int iWayRet = pWayFind->WayFindStart(MaintenanceCoordinate);
		if (iWayRet == CWayFind::STATE_Done) {
			isDone = true;	break;
		}
		else if (iWayRet == CWayFind::STATE_CDT_Useless) {
			pWayFind->detachmentStuck();
		}
		else if (iWayRet == CWayFind::STATE_Movement_timeout) {
			//����
		}

		//�����﹥�����ߵ�ס
		if (checkMonster()) {
			ClickRectangleA();
			CleanMonster();
		}
	}

	if (!isDone)
		return false;

	pWayFind->waitMovStop(3000);
	CPoint MaintenanceLogo{ NULL,NULL };
	if (m_pTool->Find(_T("����������ʾͼ��"), MaintenanceLogo)) {
		CLICK(CPoint(MaintenanceLogo.x + 10, MaintenanceLogo.y + 100));
	}
	else {
		return false;
	}

	if (!WOCRFINDCLICK(_T("�������_�����_�������װ��"))) {
		return false;
	}
	if (!WOCRFINDCLICK(_T("�������_�����_����ά��"))) {
		return false;
	}
	if (!WOCRFINDCLICK(_T("�������_�����_����ά��_ȷ��"))) {
		return false;
	}

	return true;
}

bool CLifeModule::checkMonster(bool disregardLifeAttack)
{
	//return false;

	int tagNumerical = _ttoi(OCR(_T("Ѫ����ֵ��Χ")));
	SLEEPEX(500);
	int tagNumerical2 = _ttoi(OCR(_T("Ѫ����ֵ��Χ")));
	if (tagNumerical2 < tagNumerical) {
		LogD(_T("�����﹥��tagѪ��:%d , ��ǰѪ��%d"), tagNumerical, tagNumerical2);
		if (disregardLifeAttack) {
			return false;
		}
		return true;
	}

	return false;
}

void CLifeModule::ClickRectangleA()
{
	CLICK(CPoint(556, 325));
	SLEEPEX(300);
	CLICK(CPoint(750, 341));
	SLEEPEX(300);
	CLICK(CPoint(728, 471));
	SLEEPEX(300);
	CLICK(CPoint(540, 461));
	SLEEPEX(300);
}

void CLifeModule::CleanMonster()
{
	struct Skills
	{
		int Skills_VK = NULL;
		CRect Skills_Find_Rect = CRect(NULL, NULL, NULL, NULL);
	};

	CString list = _T("81&444,716,470,734|87&479,717,504,734|69&513,718,539,736|82&549,719,573,734|65&459,751,487,769|83&495,753,521,767|68&525,750,556,768|70&562,749,589,769");
	CStringArray  arrStrlist;
	Split(list, arrStrlist, _T("|"));
	std::vector<Skills> vSkills{};
	for (size_t i = 0; i < arrStrlist.GetCount(); i++)
	{
		Skills tmpSkills;
		CStringArray tmpArr;
		Split(arrStrlist[i], tmpArr, _T("&"));
		tmpSkills.Skills_VK = _ttoi(tmpArr[0]);

		CStringArray tmpArr1;
		Split(arrStrlist[i], tmpArr1, _T(","));
		tmpSkills.Skills_Find_Rect = CRect(_ttoi(tmpArr1[0]), _ttoi(tmpArr1[1]), _ttoi(tmpArr1[2]), _ttoi(tmpArr1[3]));

		vSkills.push_back(tmpSkills);
	}

	int tagTime = m_pTool->GetTimeSeconds();
	while (true)
	{
		CPoint retpMS = m_pTool->FindMS(_T("Ѫ��10|Ѫ��30|Ѫ��20"));
		if (!retpMS.x)
			return

			LogD(_T("���ֹ��﹥����"));
		if (FIND(_T("����״̬"))) {
			m_pTool->KeyPress(VK_B);
			SLEEPEX(1500);
		}

		for (auto it = vSkills.cbegin(); it != vSkills.cend(); it++)
		{
			CString strOcr = OCR(it->Skills_Find_Rect);
			if (!strOcr) {
				m_pTool->DLLMoveTo(retpMS.x, retpMS.y);
				SLEEPEX(100);
				m_pTool->KeyPress(it->Skills_VK);
			}
			break;
		}
		m_pTool->SleepEx(500);

		if (m_pTool->GetTimeSeconds() - tagTime >= 5000) {
			break;
		}

	}

}

bool CLifeModule::checkDeath()
{
	if (FIND(_T("������"))) {
		LogD(_T("��ɫ������"));
		WOCRFINDCLICK(_T("�ݵ㸴��"));
		return true;
	}
}

bool CLifeModule::checkPhysicalStrength() {

	int tagTimr = m_pTool->GetTimeSeconds();

	while (true)
	{
		BackMainScreen();
		if (!FIND(_T("����״̬")))
			m_pTool->KeyPress(VK_B);

		if (WFIND(_T("����״̬"))) {
			SLEEPEX(1000);
			if (WFIND(_T("��������ֵ����"))) {
				return true;
			}
			else{
				return false;
			}
		}

		if ((m_pTool->GetTimeSeconds() - tagTimr) >= 20000) {
			return  false;
		}
			
	}

	return  false;
}
