#pragma once

#include"CWndManager.h"
#include "WHThread.h"
#include "CTaskThread.h"

//绑定雷电，类里面发现没有雷电模拟器 会自动启动绑定
//绑定PC，需要外部启动游戏窗口，并将结构体传入


class CLDScan :public CWndManager
{
public:
	CLDScan(int LdId, CString strLDSharePath,tagGameAcconutInfo* GameAcconut, CString strWorkPath,tagWndInfo* pWndInfo);
	~CLDScan();
public:
	//扫码
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
	//检测是否在游戏主界面
	bool CheckGameMain();
	//返回主界面
	bool BackMain();
	//检查LD
	bool CheckLD();
	//chekcGameSatrt
	bool CheckGameIsStart();

private:
	bool BindPC();
	//检测是否在游戏主界面
	bool CheckPCgameMain();
	//返回主界面
	bool BackPCGameMain();
	//保存二维码图片
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

