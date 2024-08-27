#pragma once

#include"CWndManager.h"
#include "WHThread.h"
#include "CTaskThread.h"

//���׵磬�����淢��û���׵�ģ���� ���Զ�������
//��PC����Ҫ�ⲿ������Ϸ���ڣ������ṹ�崫��


class CLDScan :public CWndManager
{
public:
	CLDScan(int LdId, CString strLDSharePath,tagGameAcconutInfo* GameAcconut, CString strWorkPath,tagWndInfo* pWndInfo);
	~CLDScan();
public:
	//ɨ��
	bool LdScan();

private:
	int					IfatherWnd = NULL;
	int					Ichild = NULL;
	int					wide = NULL, high = NULL;
	int					LdId = NULL;
	int					m_LdID	= NULL;
	tagGameAcconutInfo* m_pGameAcconut = NULL;
	CString				m_strWorkPath = NULL;
	tagWndInfo*			m_pWndInfo = NULL;
	bool				m_IsBindPC = false;
	dmsoft*				m_pLdDm = NULL;
	dmsoft*				m_pPCDm	= NULL;
	CString				m_strLDSharePath = _T("");
private:
	bool BindLD();
	//����Ƿ�����Ϸ������
	bool CheckGameMain();
	//����������
	bool BackMain();
	//���LD
	bool CheckLD();
	//chekcGameSatrt
	bool CheckGameIsStart();

private:
	bool BindPC();
	//����Ƿ�����Ϸ������
	bool CheckPCgameMain();
	//����������
	bool BackPCGameMain();
	//�����ά��ͼƬ
	bool SaveBmp();

private:
	bool Find(dmsoft* pDm, int x1, int y1, int x2, int y2, CString pic_name, CString delta_color, bool IsClick = false, int Time = 1,int ClickSleep =NULL);
	void Click(dmsoft* pDm,CPoint point);
	void Slide(dmsoft* pDm,CPoint Since,CPoint Final);
	

public:
	//virtual bool OnEventThreadRun();
	//virtual bool OnEventThreadStart();
	//virtual bool OnEventThreadConclude();
};

