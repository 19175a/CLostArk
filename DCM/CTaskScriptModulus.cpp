#include "pch.h"
#include "CTaskScript.h"
#include "CDCMEngine.h"
#include "obj.h"

extern CDCMEngine* g_pEngine;

bool CTaskScript::CheckMainInterface(bool IsBackMain/*��������������Ƿ���Ҫ���ص�������*/)
{
	int Time = 10;
	while (Time--)
	{
		//�����Ǵ�������������ߵģ��ܶ���������Ĺ��ܶ����������ﲶ�񴥷�
		���½��¼�();
		���½�ʹ����Ʒ();
		if ( Find(_T("���������½ǼӺ�")) && Find(_T("����������ͼ��")))
		{
			if (Find(_T("�������̳�"))||Find(_T("���������")))
			{
				ʶ��������Lv();
				
				return true;
			}
		}

		if (IsBackMain)
		{
			Clean();
		}
		else
		{
			return false;
		}
	}
	return false;
}

void CTaskScript::Clean(CString strUiName)
{
	if (strUiName != _T(""))
	{
		if (FindClean(strUiName))
			return;
	}

	���½�ʹ����Ʒ();
	���½��¼�();

	bool IsOnLoop = false;
	do
	{


		if (Find(_T("�Ի�_�뿪����")))
		{
			Click(_T("����Ի�_���ȷ��"));
			Iostate(_T("����Ի�_���ȷ��"));
			IsOnLoop = true;
		}

		if (FindClick(_T("�Ի�_�Ի�NPCѡ�������������")))
		{
			Iostate(_T("�Ի�_�Ի�NPCѡ�������������"));
			IsOnLoop = true;
		}

		if (Find(_T("����Ի�_�Ƽ���Ҫʦ��")))
		{
			Iostate(_T("����Ի�_�Ƽ���Ҫʦ��"));
			Click(_T("�Ի�_ÿ����������ʾ"));
			Click(_T("����Ի�_����Ҫ"));
		}
		if (Find(_T("�Ի�_�µ��ֻ�")))
		{
			Click(_T("�Ի�_�µ��ֻص��ȷ��"));
			Iostate(_T("�Ի�_�µ��ֻص��ȷ��"));
		}


		if (FindClick(_T("�Ի�_������������")))
		{
			Iostate(_T("�Ի�_������������"));
			IsOnLoop = true;
		}

		if (Find(_T("�Ի�_ѡ������")))
		{
			Iostate(_T("�Ի�_ѡ������ȷ��"));
			Click(_T("�Ի�_ѡ������ȷ��"));
		}

		if (Find(_T("�Ի�_���������Ƶȼ�")))
		{
			Iostate(_T("�Ի�_���������Ƶȼ�ȷ��"));
			Click(_T("�Ի�_���������Ƶȼ�ȷ��"));
		}

		if (FindClick(_T("�Ի�_������������")))
		{
			Iostate(_T("�Ի�_������������"));
			IsOnLoop = true;
		}

		if (FindClick(_T("�����ײ��Ի�")))
		{
			Iostate(_T("�����ײ��Ի�"));
			IsOnLoop = true;
		}

		if (FindPoint(_T("�ƿ�����"),_T("���_�ƿ�����")))
		{
			Iostate(_T("�ƿ�����"));
		}
		
		if (FindPoint(_T("����ʦ�����"), _T("����ʦ�����_ȷ��")))
		{
			Iostate(_T("�ƿ�����"));
		}

		if (FindPoint(_T("�ع鵽������"), _T("�ع鵽�������������ͼ��")))
		{
			Iostate(_T("�ع鵽������"));
		}



		if (Find(_T("����Ի�_�ұ�"))) {
			FindClick(_T("����Ի�_�ұ�������"));
			Iostate(_T("����Ի�_�ұ�������"));
			IsOnLoop = true;
		}
		if (Find(_T("����Ի�_���"))) {
			FindClick(_T("����Ի�_���������"));
			Iostate(_T("����Ի�_���������"));
			IsOnLoop = true;
		}
		Lv�¼�����(iPeopleLv);	//����
		GetHttpCmd();

		if (FindClick(_T("������"))) { return; }
		if (FindClick(_T("������_������"))) { return; }
		if (FindClick(_T("�رյ�ͼ"))) { return; }
		if (FindClick(_T("�رշ��߽���"))) { return; }
		if (FindClick(_T("�رջ����"))) { return; }
		if (FindClick(_T("�����������"))) { return; }
		if (FindClick(_T("������"))) { return; }
		if (FindClick(_T("�ر�����"))) { return; }

		
		//�������� #�Ի�ʱû�����������������߹�������

		if (!CheckMainInterface())
		{
			Click(_T("�����Ի�"));
			Iostate(_T("�������� #�Ի�ʱû�����������������߹�������"));
			IsOnLoop = true;
		}

		if (!IsOnLoop)
			return;

		IsOnLoop = false;

	} while (true);
}

bool CTaskScript::CheckMove()
{
	//��ɫ������һ����Ϊ�����ƶ���һ����Ϊ�ƶ����
	int iTime = 35;
	while (iTime--)
	{
		if (!Find(_T("��������")))
			return false;

		int ColorSizeTmp = GetColorNum(CRect(853, 22, 906, 39), _T("868381-111111|bac0c8-111111"), 0.85);
		SleepEx(1000);
		if (ColorSizeTmp == GetColorNum(CRect(853, 22, 906, 39), _T("868381-111111|bac0c8-111111"), 0.85) || ColorSizeTmp == NULL)
			return true;

		DeugString(_T("����") + ITostr(m_pWnd->id) + _T("  :�ȴ��ƶ���ϣ�����ʱ:") + ITostr(iTime));
	}

	return false;
}

bool CTaskScript::OpenControl(CString strUi/*res��Դ��Ocr��*/, CString _strFindString/*�ҵ���*/, int FindQuantity, CString strControl/*û���ҵ� ��ʹ�ÿ�ݼ���*/)
{
	int iTime = 10;
	while (iTime--)
	{
		CString FindString = _strFindString;
		int Quantity = FindQuantity;
		CString strRetOcr = ocr(strUi);
		if (Findstr(strRetOcr, FindString, Quantity))
		{
			DeugString(_T("����:") + ITostr(m_pWnd->id) + _T(" ") + strUi + _T("�ؼ��Ѿ��� �����ٴδ�"));
			return true;
		}

		Clean();
		m_pDm->KeyDownChar(strControl);
		SleepEx(GetRandom(10, 20));
		m_pDm->KeyUpChar(strControl);
		SleepEx(GetRandom(1000, 1500));
	}

}

bool CTaskScript::GetPeopleInfo()
{
	if (OpenControl(_T("��ɫ����"), _T("��ɫ����"), 2, _T("c")))
	{
		//��ȡ������Ϣ
		iPeopleLv = _ttoi(ocr(_T("��ɫLv")));
		strPeopleName = ocr(_T("��ɫ��"));
		strPeopleType = ocr(_T("��ɫְҵ"));
		CStringArray arrstrPeopleType;
		Split(strPeopleType, arrstrPeopleType, _T("\n"));
		if (arrstrPeopleType.GetCount() > 1)
			strPeopleType = arrstrPeopleType[0];

		strPeopleId = ocr(_T("��ɫId"));

		if (!IsReport)
		{
			void AddOnlieTeamUsersInfo(tagPeopleInfo PeoPeleInfo, tagTeamUsers TeamUsers);
			int  Id; //��Ա
			CString strTeamIp;
			CString strTeamPort;
			CString strTeamUrl;

			tagTeamUsers* pTeamUsers = GetTeamUsersInfo(m_pWnd->id);
			//���ͱ�������
			SendUser(m_pTeam->pTeamleader->leaderId, _T("��������"), ITostr(iPeopleLv) + _T("|") + strPeopleName + _T("|") + strPeopleId + _T("|") + strPeopleType + _T("?")
				+ ITostr(pTeamUsers->Id) + _T("|") + pTeamUsers->strTeamIp + _T("|") + pTeamUsers->strTeamPort + _T("|") + pTeamUsers->strTeamUrl);

		}

		return true;
	};

	return false;
}

void CTaskScript::Lv�¼�����(int _iLv)
{
	if (_iLv<=28)
	{
		return;
	}
	int iLv = _iLv;
	if (iLv <= 10 || iLv == 0)
	{
		//����_�Զ�ʰȡ
		if (FindPoint(_T("����_�Զ�ʰȡ"), _T("���_�����������")))
		{
			DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("�Զ�ʰȡ"));
		}
		if (Find(_T("����_�Զ�ʰȡ���Զ�����")))
		{
			Click(_T("���_����Զ�ʰȡ���Զ�����"));
		}
		if (Find(_T("����_�Զ�ʰȡ_ȡ����װ")))
		{
			Click(_T("���_�Զ�ʰȡ_ȡ����װ"));
		}
		if (Find(_T("����_�Զ�ʰȡ_ȡ����װ")))
		{
			Click(_T("���_�Զ�ʰȡ_ȡ����װ"));
		}
		if (Find(_T("����_�Զ�ʰȡ_ȡ����װ")))
		{
			Click(_T("���_�Զ�ʰȡ_ȡ����װ"));
		}

	}
	if (iLv <= 4 || iLv == 0)
	{
		FindPoint(_T("����л�����"), _T("���_����л�����"));
	}

	if (iLv >= 5 && iLv <= 10 || iLv == 0)
	{
		//����_���ܹ���
		

		//if (FindOcr(_T("����_���ܹ���_�����������"), _T("���µĹ���ָ��"), 6, true, _T("���_ͼ�����")))
		//{
		//	DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) +_T("����_���ܹ���_�µĹ���") );
		//}
		//
		//if (FindOcr(_T("����_���ܹ���_�鿴����"), _T("�㿪���˼���"), 5, true, _T("���_���ܹ���_�����漼��ͼ��")))
		//{
		//	DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("����_���ܹ���_�����漼��ͼ��"));
		//}

		CPoint Point = CPoint(NULL, NULL);
		if (Find(_T("����_���ܹ���_�л���ͬ����"), Point))
		{
			Click(Point.x -GetRandom(80,120), Point.y);
		}
		if (Find(_T("����_���ܹ���_ѡ���¼���")))
		{
			Click(_T("���_���ܹ���_ѡ���¼���"));
			Click(_T("���_���ܹ���_����������"));
		}
	}
	
	if (iLv >= 10 && iLv <= 15|| iLv == 0)
	{
		//��������
		if (Find(_T("����_���������")))
		{
			DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("����_���������"));
			Click(_T("���_�����������"));
		}
		if (FindClick(_T("����_��������_�������")))
		{

		}
		if (FindClick(_T("����_��������_�ٻ�����")))
		{

		}

	}

	if (iLv == 11 || iLv == 0)
	{
		//�������������Զ�ʹ�ûظ�ҩˮ


		FindPoint(_T("��������_�ոյĽ���"), _T("��������_���_����"));
		CPoint point;
		if (Find(_T("��������_�������"), point))
			Click(GetRandom(point.x + 115, point.x + 125), point.y);

		FindPoint(_T("��������_����Զ��ظ�����"), _T("��������_���_����Զ��ظ�����"));
		FindPoint(_T("��������_ѡȡ"), _T("��������_���_ѡȡ"));
		FindPoint(_T("��������_���"), _T("��������_���_���"));
		FindPoint(_T("��������_�����"), _T("��������_���_��Ӷ�"));
		FindPoint(_T("��������_�������"), _T("��������_���_�������"));
		if (Find(_T("��Ѫ����")))
		{
			long x = NULL, y = NULL;
			if (!m_pDm->FindColor(302, 419, 316, 429, _T("7ac420-222222"), 0.9, 0, &x, &y))
				FindClick(_T("��������_���_�Զ�������������"));
		}
	}

	if (iLv == 15 || iLv == 0)
	{
		//��������
		if (Find(_T("����_����")))
		{
			DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("����_����"));
			Click(_T("������_����ͼ��"));
		}
		if (Find(_T("����_���ǿ��")))
		{
			Click(_T("���_ǿ��"));
		}
		if (Find(_T("����_ѡ��ǿ����װ��")))
		{
			Click(_T("���_ѡ��װ��"));
		}
		if (Find(_T("����_���ǿ��װ��")))
		{
			Click(_T("���_ǿ��װ��"));
		}

	}
	
	if (iLv == 20 || iLv == 0)
	{
		//�������
		if (Find(_T("�������_���������ѡ��Ӻ�")))
		{
			DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("����_�������"));
			Click(_T("���_���������ѡ��Ӻ�"));
		}
		if (Find(_T("�������_��������ͼ��")))
		{
			Click(_T("���_��������ͼ��"));
		}
	}

	if (iLv == 21 || iLv == 0)
	{
		//��������
		if (Find(_T("��������_���������ѡ��Ӻ�")))
		{
			DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("����_��������"));
			Click(_T("���_���������ѡ��Ӻ�"));
		}
		//û�з��ָ���Դ��
		//if (Find(_T("��������_�����漼��ͼ��")))
		//{
		//	Click(_T("���_�����漼��ͼ��"));
		//}
		if (Find(_T("��������_�������")))
		{
			Click(_T("���_�������"));
		}
		if (Find(_T("��������_�����;")))
		{
			Click(_T("���_�����;"));
		}

		//����������ӵ�������
		if (Find(_T("���ܽ���")))
		{
			UpperCoordinates Slid;
			ComputeSlideData(CRect(611, 156, 627, 173), CRect(585, 462, 603, 477), &Slid);
			Slide(&Slid);
			SleepEx(1200);
		}
	}
	
	if (iLv == 21 || iLv == 0)
	{
		//������������
		if (Find(_T("��������_���������ѡ��Ӻ�")))
		{
			DeugString(_T("Lv�¼�����Lv: |") + ITostr(_iLv) + _T("����_������������"));
			Click(_T("���_���������ѡ��Ӻ�"));
		}
		//if (Find(_T("��������_�����漼��ͼ��")))
		//{
		//	Click(_T("���_�����漼��ͼ��"));
		//}
		if (Find(_T("������������_���������ť")))
		{
			Click(_T("���_���������ť"));
			Click(_T("���_�����Ϊ"));
		}
	}

	if (iLv == 28 || iLv == 0)
	{
		//����ʦ������
		FindPoint(_T("����ʦ������_�»"), _T("����ʦ������_���_�ͼ��"),1,3000);
		FindPoint(_T("����ʦ������_�����μ�"), _T("����ʦ������_���_�����μ�"));
	}

}

void CTaskScript::���½�ʹ����Ʒ()
{
	FindClick(_T("���½�ʹ����Ʒ_װ��"));
	FindPoint(_T("ʹ�ô�����"), _T("�ؼ�ʹ�ô�����"));
	FindPoint(_T("ʹ�úӵ�"), _T("�ؼ�ʹ��"));
	FindClick(_T("���½ǿؼ�ʹ��"));

}

void CTaskScript::���½��¼�()
{
	if (Find(_T("���½��¼�_�������¼���")))
		Click(_T("���_�������������¼�������"));

}

int CTaskScript::ʶ��������Lv()
{
	CString retDmOcr = DmOcr(_T("�������ɫ�ȼ�"));
	if (retDmOcr != _T(""))
	{
		DeugString(_T("�������ɫLv:")+ retDmOcr);
		iPeopleLv = _ttoi(retDmOcr);
		return _ttoi(retDmOcr);
	}
	return NULL;
}


bool CTaskScript::CheckBattle(bool IsAwait)
{
	int iTime = 1 * 60 * 1000;
	while (iTime--)
	{
		if (Find(_T("ս��_�һ�ͼ��")))
		{
			if (Find(_T("ս��_ͼ��һ���")))
			{
				if (!IsAwait)
					return  true;
				Iostate(_T("�һ���"));
			}
			else
			{
				return false;
			}
			CTime currentTime = CTime::GetCurrentTime();
			CString CstrTime = currentTime.Format(_T("%m-%d%H%M"));
			CString tagBmp = ITostr(m_pWnd->id) + CstrTime + _T(".bmp");
			m_pDm->Capture(850, 520, 878, 550, tagBmp);
			SleepEx(1000);
			long x = NULL, y = NULL;
			if (m_pDm->FindPic(850-1, 520-1, 878+1, 550+1, tagBmp,_T("020202"),0.9,0,&x,&y) != -1)
			{
				if (!IsAwait)
				{
					m_pDm->DeleteFile(tagBmp);
					return  true;
				}
				else
				{
					m_pDm->DeleteFile(tagBmp);
					Iostate(_T("ս����"));
				}
			}

		}
		else
		{
			return false;
		}
		SleepEx(1000);
	}
	DeugString(_T("����Ƿ���ս��"));
	return false;
}

bool CTaskScript::�Զ�Ѱ·(CPoint Ŀ���)
{
	if (CheckMainInterface(true))
	{
		FindClick(_T("�ٷ�Ѱ·_�򿪵�ͼ"));
		if (FindClick(_T("�ٷ�Ѱ·_��λͼ��")))
		{
			CPoint PointX = CPoint(NULL, NULL);
			if (Find(_T("��������Xͼ��"),PointX))
			{
				Click(CPoint(GetRandom(PointX.x + 70, PointX.x + 100), PointX.y));
				SleepEx(GetRandom(1200, 1500));
				CString PointX = ITostr(Ŀ���.x);
				for (int i = 0; i < PointX.GetLength(); i++)
				{
					char myChar = PointX.GetAt(i); 
					CString x(myChar);
					CString Findx = _T("����") + x;
					CPoint retPoint;
					FindClick(Findx);
				}
				FindClick(_T("ȷ��"));
			}
			SleepEx(GetRandom(1500, 2000));
			CPoint PointY = CPoint(NULL, NULL);
			if (Find(_T("��������Yͼ��"), PointY))
			{
				Click(CPoint(GetRandom(PointY.x + 70, PointY.x + 100), PointY.y));
				SleepEx(GetRandom(1200, 1500));
				CString PointY = ITostr(Ŀ���.y);
				for (int i = 0; i < PointY.GetLength(); i++)
				{
					char myChar = PointY.GetAt(i); 
					CString y(myChar);
					CPoint retPoint;
					FindClick(_T("����")+y);
				}
				FindClick(_T("ȷ��"));
			}

			SleepEx(GetRandom(1500, 2000));
			FindClick(_T("ǰ��"));
			FindClick(_T("�رյ�ͼ"));
		}
	}
	return false;
}

bool CTaskScript::GetActiveTask(CString _TaskName/*��Ҫ��Ӧres������*/)
{
	BOOL Confirmation = FALSE;	INT  Time = 3; 	CString TaskName = _TaskName;

	while (Time--){
		CPoint TaskNamePoint(NULL, NULL);
		if (Find(TaskName,TaskNamePoint)){
			if (Find(_T("�����_�μ�"), CRect(TaskNamePoint.x + 175, TaskNamePoint.y, TaskNamePoint.x + 253, TaskNamePoint.y + 51))){
				Click(CPoint(TaskNamePoint.x + 200 + GetRandom(0, 20), TaskNamePoint.y + 20 + GetRandom(0, 10)));
				return TRUE;
			}
			else{return FALSE;}
		}

		if (!Confirmation){
			UpperCoordinates Upset;
			ComputeSlideData(CRect(350, 427, 414, 463), CRect(373, 192, 425, 223), &Upset);
			Slide(&Upset);
			SleepEx(GetRandom(1000, 1500));
		}
	}
	return FALSE;

	if (!Confirmation){

	}
}
