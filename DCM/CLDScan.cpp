#include "pch.h"
#include "CLDScan.h"
#include"obj.h"

CLDScan::CLDScan(int LdId, CString strLDSharePath, tagGameAcconutInfo* GameAcconut, CString strWorkPath, tagWndInfo* pWndInfo)
{
	m_LdID = LdId;
	m_pGameAcconut = GameAcconut;
	m_pWndInfo = pWndInfo;
	m_pLdDm = new dmsoft();
	m_pPCDm = new dmsoft();
	m_strWorkPath = strWorkPath;
	m_strLDSharePath = strLDSharePath;
	BindPC();

}

CLDScan::~CLDScan()
{
	if (m_pLdDm){
		m_pLdDm->UnBindWindow();
		delete m_pLdDm;
	}

	if (m_pPCDm){
		m_pPCDm->UnBindWindow();
		delete m_pPCDm;
	}
}

bool CLDScan::CheckLD()
{
	int LdID = m_LdID;
	CString strRet = GetLDlist(); //����Ƿ����׵�ģ����,���ɨ��ģ�����Ƿ��
	if (strRet.GetLength() < 1)
	{
		AfxMessageBox(_T("!!!�ű����뷽ʽ���׵�ģ����ɨ����룬������ģ����"));
		return false;
	}
	CStringArray strArray;
	Split(strRet, strArray, _T("\r\n"));
	CStringArray strArrLDinfo;
	Split(strArray[LdID], strArrLDinfo, _T(","));
	IfatherWnd = _ttoi(strArrLDinfo[2]);
	Ichild = _ttoi(strArrLDinfo[3]);
	wide = _ttoi(strArrLDinfo[7]);
	high = _ttoi(strArrLDinfo[8]);

	if (IfatherWnd > 1)
		return true;

	return false;
}

bool CLDScan::CheckGameIsStart()
{
	int LdID = m_LdID; //���ģ�����Ƿ����У�û��������  //�����Ϸ�Ƿ����У�û��������
	bool IsGameStart = false;
	for (int i = 0; i < 3; i++)
	{
		if (CheckLD())
		{
			IsGameStart = true;
			break;
		}
		else
		{
			RunWnd(LdID);
			Sleep(10 * 1000);
		}
	}

	if (IsGameStart)
	{
		ActivateGame(LdID);
		Sleep(5000);
		return true;
	}

	return false;
}


bool CLDScan::BindLD()
{
	if (CheckGameIsStart()) {
		long retBind = m_pLdDm->BindWindowEx(Ichild, _T("gdi"), _T("windows"), _T("windows"), _T(""), NULL);
		if (retBind) {
			m_pLdDm->SetPath(m_strWorkPath + _T("/res"));
			return true;
		}
		else{
			AfxMessageBox(_T("���׵�ģ����ʧ��"));
			return false;
		}
	}

	return false;
}

bool CLDScan::BindPC()
{
	bool IsBind = false;
	long retBind = m_pPCDm->BindWindowEx((long)m_pWndInfo->hWnd, _T("gdi"), _T("dx.mouse.position.lock.api"), _T("windows"), _T(""), NULL);
	if (retBind)
	{
		//��ⴰ�ڴ�С�Ƿ�Ϊ��960,640
		long Width = NULL, Height = NULL;
		m_pPCDm->GetClientSize((long)m_pWndInfo->hWnd, &Width, &Height);
		if (Width != 960 || Height != 640)
		{
			m_pPCDm->SetClientSize((long)m_pWndInfo->hWnd, 960, 640);
		}
		else
		{
			//��ʼ�����ô�Į
			m_pPCDm->SetPath(m_strWorkPath + _T("/res"));
			m_IsBindPC = true;
			IsBind = true;
		}
	}

	if (IsBind)
		return true;

	return false;

}

bool CLDScan::CheckPCgameMain()
{
	if (Find(m_pPCDm, 42, 581, 55, 597, _T("PC_������"), _T("050505")))
		return true;

	return false;
}

bool CLDScan::BackPCGameMain()
{
	int Time = 10;
	while (Time--)
	{
		if (CheckPCgameMain())
			return true;

		Find(m_pPCDm, 450, 510, 474, 522, _T("PC_��֪����"), _T("050505"), true);
		Find(m_pPCDm, 845, 85, 857, 97, _T("PC_����"), _T("050505"), true);

		Sleep(1500);
	}
	return false;
}

bool CLDScan::SaveBmp()
{
	int Time = 10;
	while (Time--)
	{
		if (m_IsBindPC && BackPCGameMain())
		{
			//����ж�ά��ˢ�³���
			if (Find(m_pPCDm, 766, 577, 777, 584, _T("PC_ɨ�����"), _T("050505"), false))
			{
				m_pPCDm->Capture(708, 340, 908, 539, m_strLDSharePath + _T("/123.bmp"));
				return true;
			}
			else
			{
				//�����������Ϸ�� ������ά��
				Click(m_pPCDm, CPoint(494, 500));

			}
		}
		Sleep(1000);
	}
}


bool CLDScan::CheckGameMain()
{
	CPoint retPoint;
	bool find = Find(m_pLdDm, 906, 573, 925, 583, _T("ģ_��Ϸ������"), _T("050505"));
	if (find)
		return true;

	return false;
}

bool CLDScan::BackMain()
{
	int Time = 10;
	while (Time--){
		if (Find(m_pLdDm, 906, 573, 925, 583, _T("ģ_��Ϸ������"), _T("050505")))
			return true;
		
		Find(m_pLdDm, 449, 379, 476, 408, _T("ģ_ɨ�뵯������"), _T("050505"),true);
		m_pLdDm->KeyPressChar(_T("ESC"));
		Sleep(1500);
	}
	return false;
}

bool CLDScan::LdScan()
{

	if (!SaveBmp())
		return false;

	if (!BindLD()) {
		AfxMessageBox(_T("���׵�ģ����ʧ��"));
		return false;
	}

	int Time = 15;
	while (Time--)
	{
		if (CheckGameMain())
		{
			////ɨ�뷽��
			////1.�����˺Ŷ���һ��ģ�������롣
			////1.1 ����Ƿ����˺ŵ���
			//bool IsLogin = false;
			//bool IsScan = false;
			//if (Find(m_pLdDm, 19, 27, 53, 53, _T("ģ_�������л��˺ſؼ�"), _T("050505"), true))
			//{
			//	//�л��˺�
			//	Sleep(2000);
			//	Find(m_pLdDm, 732, 175, 771, 210, _T("ģ_�л��˺�"), _T("050505"), true);
			//	Find(m_pLdDm, 416, 446, 444, 473, _T("ģ_�����˺ŵ���"), _T("050505"), true, 3);
			//	Sleep(1000);
			//	//�ж��Ƿ�ѡЭ��
			//	CPoint retColorPoint;
			//	if (!m_pLdDm->FindColor(324, 466, 340, 480, _T("e50113-151515"), 0.9, 0, &retColorPoint.x, &retColorPoint.y))
			//		Click(m_pLdDm, CPoint(324, 466));

			//	//ѡ����������
			//	if (Find(m_pLdDm, 555, 419, 572, 436, _T("ģ_��������"), _T("050505"), true), 3) {
			//		Sleep(1000);
			//		Find(m_pLdDm, 555, 419, 572, 436, _T("ģ_��������"), _T("050505"), true, 1, 2000);	//��ʱ��������Ҫ������� ��������
			//		if (Find(m_pLdDm, 452, 371, 480, 403, _T("ģ_����"), _T("050505"), false), 3)
			//			IsLogin = true;
			//	}
			//}

			//if (IsLogin)
			//{
			//	Sleep(2000);
			//	if (Find(m_pLdDm, 437, 225, 463, 251, _T("ģ_�˺�"), _T("050505"), true)) {
			//		Sleep(1000);
			//		m_pLdDm->SendString(Ichild, m_pGameAcconut->strAcconut);
			//		Sleep(1000);
			//		Find(m_pLdDm, 851, 593, 876, 624, _T("ģ_��һ��"), _T("050505"), true);
			//		Sleep(1000);
			//		m_pLdDm->SendString(Ichild, m_pGameAcconut->strPassword);
			//		Sleep(1000);
			//		Find(m_pLdDm, 851, 593, 876, 624, _T("ģ_��һ��"), _T("050505"), true);

			//		//���ǰ������˳������ô�ͻ������Ϸ������
			//		if (Find(m_pLdDm, 906, 573, 925, 583, _T("ģ_��Ϸ������"), _T("050505"), false, 3))
			//			IsScan = true;
			//	}
			//}
			////2.һ���˺�һ��ģ�������ű����л��˺�ֱ�ӵ��롣


			//if (!IsScan) {
			//	BackMain();
			//	continue;
			//}

			//ģ_ɨ��
			bool IsSuccessful = false;
			do {
				if (Find(m_pLdDm, 35, 176, 43, 183, _T("ģ_ɨ��"), _T("050505"), true)) {
					if (!Find(m_pLdDm, 801, 281, 808, 289, _T("ģ_ɨ_ͼƬ"), _T("050505"), true, 3, 1500)) {break;};
					if (!Find(m_pLdDm, 39, 64, 44, 77, _T("ģ_ɨ_����"), _T("050505"), true, 3, 1500)) { break; };
					if (!Find(m_pLdDm, 127, 512, 140, 522, _T("ģ_�ļ�_�ļ�������"), _T("050505"), true, 3, 1500)) { break; };
					int TIme = 10;
					while (TIme--) {
						Slide(m_pLdDm, CPoint(453, 412), CPoint(453, 412 - 160));
						Sleep(1000);
						if (Find(m_pLdDm, 279, 250, 566, 505, _T("ģ_ɨ_Pictures"), _T("050505"), true, 1, 1500))
							break;
					}
						
					Slide(m_pLdDm, CPoint(453, 412), CPoint(453, 412 - 160));
					Slide(m_pLdDm, CPoint(453, 412), CPoint(453, 412 - 160));
					Sleep(1000);
					if (Find(m_pLdDm, 279, 250, 566, 505, _T("ģ_ɨ_��"), _T("050505"), true, 2, 2000)) {
						if (Find(m_pLdDm, 450, 360, 481, 390, _T("ģ_ɨ_����"), _T("050505"), true, 3, 1500))
						{
							IsSuccessful = true;
							break;
						}
					}
				}

			} while (false);
			
			if (IsSuccessful) 
			{
				return true;
			}
		}
		else {
			BackMain();
		}
	}
}


bool CLDScan::Find(dmsoft* pDm, int x1, int y1, int x2, int y2, CString pic_name, CString delta_color, bool IsClick, int Time, int ClickSleep)
{
	CPoint retPoint;
	for (int i = 0; i < Time; i++) {
		long retFind = pDm->FindPic(x1 - 10, y1 - 10, x2 + 20, y2 + 20, pic_name + _T(".bmp"), delta_color, 0.9, 0, &retPoint.x, &retPoint.y);
		if (retFind != -1) {
			if (IsClick) {
				Click(pDm, retPoint);
				Sleep(ClickSleep);
			}

			return true;
		}
		Sleep(1000);
	}
	return false;
}

void CLDScan::Click(dmsoft* pDm, CPoint point) {
	pDm->MoveTo(point.x, point.y);
	Sleep(80 + rand() % 100);
	pDm->LeftClick();
}

void CLDScan::Slide(dmsoft* pDm, CPoint Since, CPoint Final) {
	pDm->EnableRealMouse(1, 100, 100);
	pDm->MoveTo(Since.x, Since.y);
	pDm->RightDown();
	pDm->MoveTo(Final.x, Final.y);
	pDm->RightUp();
	pDm->EnableRealMouse(0, 10, 30);
}
