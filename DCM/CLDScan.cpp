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
	CString strRet = GetLDlist(); //检测是否有雷电模拟器,检测扫码模拟器是否打开
	if (strRet.GetLength() < 1)
	{
		AfxMessageBox(_T("!!!脚本登入方式已雷电模拟器扫码登入，请下载模拟器"));
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
	int LdID = m_LdID; //检测模拟器是否运行，没有则运行  //检测游戏是否运行，没有则运行
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
			AfxMessageBox(_T("绑定雷电模拟器失败"));
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
		//检测窗口大小是否为：960,640
		long Width = NULL, Height = NULL;
		m_pPCDm->GetClientSize((long)m_pWndInfo->hWnd, &Width, &Height);
		if (Width != 960 || Height != 640)
		{
			m_pPCDm->SetClientSize((long)m_pWndInfo->hWnd, 960, 640);
		}
		else
		{
			//初始化设置大漠
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
	if (Find(m_pPCDm, 42, 581, 55, 597, _T("PC_主界面"), _T("050505")))
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

		Find(m_pPCDm, 450, 510, 474, 522, _T("PC_我知道了"), _T("050505"), true);
		Find(m_pPCDm, 845, 85, 857, 97, _T("PC_擦除"), _T("050505"), true);

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
			//检测有二维码刷新出来
			if (Find(m_pPCDm, 766, 577, 777, 584, _T("PC_扫码登入"), _T("050505"), false))
			{
				m_pPCDm->Capture(708, 340, 908, 539, m_strLDSharePath + _T("/123.bmp"));
				return true;
			}
			else
			{
				//点击“进入游戏” 弹出二维码
				Click(m_pPCDm, CPoint(494, 500));

			}
		}
		Sleep(1000);
	}
}


bool CLDScan::CheckGameMain()
{
	CPoint retPoint;
	bool find = Find(m_pLdDm, 906, 573, 925, 583, _T("模_游戏主界面"), _T("050505"));
	if (find)
		return true;

	return false;
}

bool CLDScan::BackMain()
{
	int Time = 10;
	while (Time--){
		if (Find(m_pLdDm, 906, 573, 925, 583, _T("模_游戏主界面"), _T("050505")))
			return true;
		
		Find(m_pLdDm, 449, 379, 476, 408, _T("模_扫码弹出登入"), _T("050505"),true);
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
		AfxMessageBox(_T("绑定雷电模拟器失败"));
		return false;
	}

	int Time = 15;
	while (Time--)
	{
		if (CheckGameMain())
		{
			////扫码方案
			////1.所有账号都在一个模拟器登入。
			////1.1 检查是否有账号登入
			//bool IsLogin = false;
			//bool IsScan = false;
			//if (Find(m_pLdDm, 19, 27, 53, 53, _T("模_主界面切换账号控件"), _T("050505"), true))
			//{
			//	//切换账号
			//	Sleep(2000);
			//	Find(m_pLdDm, 732, 175, 771, 210, _T("模_切换账号"), _T("050505"), true);
			//	Find(m_pLdDm, 416, 446, 444, 473, _T("模_其他账号登入"), _T("050505"), true, 3);
			//	Sleep(1000);
			//	//判断是否勾选协议
			//	CPoint retColorPoint;
			//	if (!m_pLdDm->FindColor(324, 466, 340, 480, _T("e50113-151515"), 0.9, 0, &retColorPoint.x, &retColorPoint.y))
			//		Click(m_pLdDm, CPoint(324, 466));

			//	//选择网易邮箱
			//	if (Find(m_pLdDm, 555, 419, 572, 436, _T("模_网易邮箱"), _T("050505"), true), 3) {
			//		Sleep(1000);
			//		Find(m_pLdDm, 555, 419, 572, 436, _T("模_网易邮箱"), _T("050505"), true, 1, 2000);	//有时候这里需要点击两次 网易邮箱
			//		if (Find(m_pLdDm, 452, 371, 480, 403, _T("模_登入"), _T("050505"), false), 3)
			//			IsLogin = true;
			//	}
			//}

			//if (IsLogin)
			//{
			//	Sleep(2000);
			//	if (Find(m_pLdDm, 437, 225, 463, 251, _T("模_账号"), _T("050505"), true)) {
			//		Sleep(1000);
			//		m_pLdDm->SendString(Ichild, m_pGameAcconut->strAcconut);
			//		Sleep(1000);
			//		Find(m_pLdDm, 851, 593, 876, 624, _T("模_下一步"), _T("050505"), true);
			//		Sleep(1000);
			//		m_pLdDm->SendString(Ichild, m_pGameAcconut->strPassword);
			//		Sleep(1000);
			//		Find(m_pLdDm, 851, 593, 876, 624, _T("模_下一步"), _T("050505"), true);

			//		//如果前面输入顺利，那么就会进入游戏主界面
			//		if (Find(m_pLdDm, 906, 573, 925, 583, _T("模_游戏主界面"), _T("050505"), false, 3))
			//			IsScan = true;
			//	}
			//}
			////2.一个账号一个模拟器，脚本不切换账号直接登入。


			//if (!IsScan) {
			//	BackMain();
			//	continue;
			//}

			//模_扫码
			bool IsSuccessful = false;
			do {
				if (Find(m_pLdDm, 35, 176, 43, 183, _T("模_扫码"), _T("050505"), true)) {
					if (!Find(m_pLdDm, 801, 281, 808, 289, _T("模_扫_图片"), _T("050505"), true, 3, 1500)) {break;};
					if (!Find(m_pLdDm, 39, 64, 44, 77, _T("模_扫_更多"), _T("050505"), true, 3, 1500)) { break; };
					if (!Find(m_pLdDm, 127, 512, 140, 522, _T("模_文件_文件管理器"), _T("050505"), true, 3, 1500)) { break; };
					int TIme = 10;
					while (TIme--) {
						Slide(m_pLdDm, CPoint(453, 412), CPoint(453, 412 - 160));
						Sleep(1000);
						if (Find(m_pLdDm, 279, 250, 566, 505, _T("模_扫_Pictures"), _T("050505"), true, 1, 1500))
							break;
					}
						
					Slide(m_pLdDm, CPoint(453, 412), CPoint(453, 412 - 160));
					Slide(m_pLdDm, CPoint(453, 412), CPoint(453, 412 - 160));
					Sleep(1000);
					if (Find(m_pLdDm, 279, 250, 566, 505, _T("模_扫_码"), _T("050505"), true, 2, 2000)) {
						if (Find(m_pLdDm, 450, 360, 481, 390, _T("模_扫_登入"), _T("050505"), true, 3, 1500))
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
