#include "pch.h"
#include "CTaskThread.h"
#include "obj.h"
#include "CDCMEngine.h"
#include "hurl.h"
#include "requests.h"
#include "CAiTool.h"
#include"CTaskScript.h"


extern CDCMEngine* g_pEngine;

CTaskThread::CTaskThread(tagWndInfo* pWnd)
{
	m_pWnd = pWnd;

	m_pDm = new dmsoft;

	m_bind = false;

	m_bSuspend = false;
	m_pCKeyMouse = CKeyMouse(g_pEngine->m_strWorkPath + _T("/Classkey.dll"));
	//m_TaskScript = new CTaskScript(this);

	//CoInitializeEx(NULL, 0);
}

CTaskThread::~CTaskThread()
{
	if (m_pDm)
	{
		m_pDm->UnBindWindow();
		delete m_pDm;
	}

	//CoUninitialize();

}

void CTaskThread::Suspend()
{
	m_bSuspend = true;
}

void CTaskThread::Resume()
{
	m_bSuspend = false;
}

void CTaskThread::Restart()
{
	::PostMessage(g_pEngine->m_pMainDlg->GetSafeHwnd(), WM_RESTART_MSG, m_pWnd->id, 0);
}

void CTaskThread::Switch(int IsWriteIniLogd)
{
	::PostMessage(g_pEngine->m_pMainDlg->GetSafeHwnd(), WM_SWITCH_MSG, m_pWnd->id, IsWriteIniLogd);
}

void CTaskThread::AddTask(CStringArray& arrTask)
{
	for (int i = 0; i < arrTask.GetCount(); i++)
	{
		AddTask(arrTask[i], _T(""), 1);
	}
}

void CTaskThread::AddTask(CString strName, CString strParam, int count)
{
	tagTaskCmd cmd;
	cmd.strName = strName;
	cmd.strParam = strParam;
	cmd.iCount = count;

	m_arrTaskCmd.Add(cmd);
}

bool CTaskThread::GetTask(tagTaskCmd* cmd)
{
	for (int i = 0; i < m_arrTaskCmd.GetCount(); i++)
	{
		if (m_arrTaskCmd[i].iCount > 0)
		{
			cmd->strName = m_arrTaskCmd[i].strName;
			cmd->strParam = m_arrTaskCmd[i].strParam;
			cmd->iCount = m_arrTaskCmd[i].iCount;

			m_arrTaskCmd[i].iCount = 0;

			return true;
		}
	}

	return false;
}

void CTaskThread::SetTaskCount(int count)
{
	for (int i = 0; i < m_arrTaskCmd.GetCount(); i++)
		m_arrTaskCmd[i].iCount = count;
}

bool CTaskThread::GetRandTask(tagTaskCmd* cmd)
{
	int i = 0;
	for (; i < m_arrTaskCmd.GetCount(); i++)
	{
		if (m_arrTaskCmd[i].iCount > 0)
		{
			break;
		}
	}

	if (i == m_arrTaskCmd.GetCount())
		return false;

	do
	{
		int iRand = rand() % m_arrTaskCmd.GetCount();

		if (m_arrTaskCmd[iRand].iCount > 0)
		{
			cmd->strName = m_arrTaskCmd[iRand].strName;
			cmd->strParam = m_arrTaskCmd[iRand].strParam;
			cmd->iCount = m_arrTaskCmd[iRand].iCount;

			m_arrTaskCmd[iRand].iCount = 0;

			return true;
		}
	} while (true);

	return false;
}

bool CTaskThread::OnEventThreadRun()
{
	if (!m_bind)
	{

		m_bind = true;

		long Width = NULL, Height = NULL;
		m_pDm->MoveWindow((long)m_pWnd->hWnd, 0, 0);

		return true;

	}

	bool bRet = true;
	try
	{
		bRet = OnLoop();
	}
	catch (StopThreadException& e)
	{
		AfxEndThread(0);
		return false;
	}
	catch (...) {}

	Sleep(1000);

	return bRet;
}

bool CTaskThread::OnEventThreadStart()
{
	return true;
}

bool CTaskThread::OnEventThreadConclude()
{
	LogD(_T(""));

	return true;
}

int CTaskThread::GetSwitchToWndId()
{
	for (int i = 0; i < g_pEngine->m_WndIni.arrSwitch.GetCount(); i++)
	{
		CString strItem = g_pEngine->m_WndIni.arrSwitch[i];

		CStringArray arrTemp;
		Split(strItem, arrTemp, _T("-"));

		if (_ttoi(arrTemp[0]) == m_pWnd->id)
			return _ttoi(arrTemp[1]);
	}

	return -1;
}

bool CTaskThread::Bind()
{
	return true;
	if (m_pWnd->hWnd == NULL || !IsWindow(m_pWnd->hWnd))
		return false;


	long ret = 0;

	try
	{
		ret = 1;
		//ret = m_pDm->BindWindowEx((long)m_pWnd->hWnd,
		//	g_pEngine->m_WndIni.strDisplay,
		//	g_pEngine->m_WndIni.strMouse,
		//	g_pEngine->m_WndIni.strKeypad,
		//	g_pEngine->m_WndIni.strPublic,
		//	g_pEngine->m_WndIni.iMode);


		//if (g_pEngine->tagOnDraw)
		//{

		//	if (!m_pWnd->pTaskInfo->bDrawWnd)
		//	{
		//		//绘制初始化
		//		m_pDm->GetWindowRect((long)m_pWnd->hWnd, &x1, &y1, &x2, &y2);
		//		m_hDrawWnd = m_pDm->CreateFoobarRect(000, x1 + 8, y1 + 30, x2 - 19, y2 - 30);	 //创建一个圆角矩形窗口		
		//		m_pDm->FoobarSetFont(m_hDrawWnd, _T("宋体"), 20, 0);							//设置指定Foobar窗口的字体
		//		m_pDm->FoobarSetTrans(m_hDrawWnd, 1, _T("000000"), 1.0);						//设置指定Foobar窗口的是否透明
		//		m_pWnd->pTaskInfo->bDrawWnd = true;
		//	}
		//}

	}
	catch (CException* e)
	{
		LogE(_T("%s: 开始绑定异常"), m_pWnd->strTitle);
	}

	if (ret == 1)
		return true;

	return false;
}

void CTaskThread::SleepEx(DWORD dwMilsec)
{
	DWORD dwStep = 100;

	if (dwMilsec < dwStep)
	{
		if (!m_bRun) {
			AfxEndThread(0);//强制结束线程
			throw StopThreadException("Stop Thread");
			return;
		}

		Sleep(dwMilsec);

		while (m_bSuspend)
			Sleep(dwStep);

		return;
	}

	int i = 0;
	while ((i++ < dwMilsec / dwStep))
	{
		if (!m_bRun) {
			AfxEndThread(0);//强制结束线程
			throw StopThreadException("Stop Thread");
			return;
		}

		Sleep(dwStep);
	}

	Sleep(dwMilsec % dwStep);

	while (m_bSuspend)
		Sleep(dwStep);
}

void CTaskThread::RandSleepEx(DWORD dwMilsec)
{
	SleepEx(dwMilsec + rand() % dwMilsec);
}

void CTaskThread::Click(CPoint ptClk)
{
	Click(ptClk.x, ptClk.y);
}

void CTaskThread::Click(int x, int y)
{
	DLLMoveTo(x, y);
	Sleep(GetRandom(50, 80));
	DLLLeftDown();
	Sleep(GetRandom(50, 80));
	DLLLeftUp();
	Sleep(GetRandom(50, 80));
}

//DLLCKeyMouse
void CTaskThread::DLLMoveTo(int x, int y)
{
	g_pEngine->m_CKeyMouse->MoveTo(x, y);
}

void CTaskThread::DLLRightClick()
{
	g_pEngine->m_CKeyMouse->RightClick();
}

void CTaskThread::DLLLeftClick()
{
	g_pEngine->m_CKeyMouse->LeftClick();
}


void CTaskThread::DLLLeftDown()
{
	g_pEngine->m_CKeyMouse->LeftDown();
}

void CTaskThread::DLLLeftUp()
{
	g_pEngine->m_CKeyMouse->LeftUp();
}

void CTaskThread::DLLLRightDown()
{
	g_pEngine->m_CKeyMouse->RightDown();
}

void CTaskThread::DLLRightUp()
{
	g_pEngine->m_CKeyMouse->RightUp();
}

void CTaskThread::LeftClickDrag(CPoint StartPoint, CPoint EndPoint)
{
	DLLMoveTo(StartPoint.x, StartPoint.y);
	SleepEx(GetRandom(50, 100));
	DLLLeftDown();

	SleepEx(GetRandom(300, 350));
	DLLMoveTo(EndPoint.x, EndPoint.y);
	SleepEx(GetRandom(300, 350));
	DLLLeftUp();
	SleepEx(GetRandom(20, 50));
}


void CTaskThread::DClick(CPoint ptClk)
{
	DClick(ptClk.x, ptClk.y);
}

void CTaskThread::DClick(int x, int y)
{
	DClick(x, y);
}

void CTaskThread::RClick(CPoint ptClk)
{
	RClick(ptClk.x, ptClk.y);
}

void CTaskThread::RClick(int x, int y)
{
	DRClick(x, y);
}

void CTaskThread::DRClick(CPoint ptClk)
{
	DRClick(ptClk.x, ptClk.y);
}

void CTaskThread::DRClick(int x, int y)
{
	DLLMoveTo(x, y);
	Sleep(GetRandom(50, 100));
	DLLLRightDown();
	Sleep(GetRandom(50, 100));
	DLLRightUp();
	Sleep(GetRandom(50, 100));
}

void CTaskThread::KeyDown(long vk)
{
	m_pDm->KeyDown(vk);
}

void CTaskThread::KeyPress(long vk)
{
	g_pEngine->m_CKeyMouse->KeyPress(vk);
}

void CTaskThread::KeyUp(long vk)
{
	m_pDm->KeyUp(vk);
}

void CTaskThread::w(int time)
{
	m_pDm->KeyDown(87);
	SleepEx(time);
	m_pDm->KeyUp(87);
}

void CTaskThread::a(int time)
{
	m_pDm->KeyDown(65);
	SleepEx(time);
	m_pDm->KeyUp(65);
}

void CTaskThread::s(int time)
{
	m_pDm->KeyDown(83);
	SleepEx(time);
	m_pDm->KeyUp(83);
}

void CTaskThread::d(int time)
{
	m_pDm->KeyDown(68);
	SleepEx(time);
	m_pDm->KeyUp(68);
}

void CTaskThread::wa(int time)
{
	m_pDm->KeyDown(87);
	m_pDm->KeyDown(65);
	SleepEx(time);
	m_pDm->KeyUp(87);
	m_pDm->KeyUp(65);
}

void CTaskThread::wd(int time)
{
	m_pDm->KeyDown(87);
	m_pDm->KeyDown(68);
	SleepEx(time);
	m_pDm->KeyUp(87);
	m_pDm->KeyUp(68);
}

void CTaskThread::sd(int time)
{
	m_pDm->KeyDown(83);
	m_pDm->KeyDown(68);
	SleepEx(time);
	m_pDm->KeyUp(83);
	m_pDm->KeyUp(68);
}

void CTaskThread::as(int time)
{
	m_pDm->KeyDown(65);
	m_pDm->KeyDown(83);
	SleepEx(time);
	m_pDm->KeyUp(65);
	m_pDm->KeyUp(83);
}

CString CTaskThread::DmOcr(CString strUI, CRect rtArea)
{
	tagResItem* pItem = g_pEngine->GetResItem(strUI);
	if (!pItem)
	{
		LogE(_T("%s: 没有发现该资源项"), strUI);
		return false;
	}

	if (rtArea.left != 0)
		pItem->rtArea = rtArea;

	m_pDm->UseDict(pItem->iDir);
	SetDraw(m_hDrawWnd, UpperCoordinates(pItem->rtArea.left - 1, pItem->rtArea.top - 1, pItem->rtArea.right + 1, pItem->rtArea.bottom + 1), pItem->strRes, _T("ff6ca1"));

	return m_pDm->Ocr(pItem->rtArea.left, pItem->rtArea.top, pItem->rtArea.right, pItem->rtArea.bottom, pItem->strDeltaClr, pItem->dSim);
}

bool CTaskThread::Find(CString strUI, CPoint& ptFind, CRect rtArea)
{
	tagResItem* pItem = g_pEngine->GetResItem(strUI);
	if (!pItem)
	{
		LogE(_T("%s: 没有发现该资源项"), strUI);
		return false;
	}

	if (rtArea.left != 0)
	{
		pItem->rtArea = rtArea;
	}

	long x, y = 0;
	long ret = -1;
	if (pItem->iType == 0)
	{
		ret = m_pDm->FindPic(pItem->rtArea.left - 1,
			pItem->rtArea.top - 1,
			pItem->rtArea.right + 1,
			pItem->rtArea.bottom + 1,
			g_pEngine->m_strWorkPath + _T("\\res\\") + pItem->strRes, pItem->strDeltaClr, pItem->dSim, pItem->iDir, &x, &y);
		if (ret != -1)
		{
			//DeugString(_T("找到了：") + pItem->strRes);
			SetDraw(m_hDrawWnd, UpperCoordinates(pItem->rtArea.left - 1, pItem->rtArea.top - 1, pItem->rtArea.right + 1, pItem->rtArea.bottom + 1), pItem->strRes, _T("ff0000"));
		}

	}
	else if (pItem->iType == 1)
	{
		ret = m_pDm->FindColor(pItem->rtArea.left - 1,
			pItem->rtArea.top - 1,
			pItem->rtArea.right + 1,
			pItem->rtArea.bottom + 1,
			pItem->strRes, pItem->dSim, pItem->iDir, &x, &y);

		if (ret != -1)
		{
			//DeugString(_T("找到了：") + pItem->strRes);
			SetDraw(m_hDrawWnd, UpperCoordinates(pItem->rtArea.left - 1, pItem->rtArea.top - 1, pItem->rtArea.right + 1, pItem->rtArea.bottom + 1), pItem->strRes, _T("3fc13f"));
		}

	}
	else if (pItem->iType == 2)
	{
		m_pDm->UseDict(pItem->iDir);
		ret = m_pDm->FindStr(pItem->rtArea.left,
			pItem->rtArea.top,
			pItem->rtArea.right,
			pItem->rtArea.bottom,
			pItem->strRes, pItem->strDeltaClr, pItem->dSim, &x, &y);

		if (ret != -1)
		{
			//DeugString(_T("找到了：") + pItem->strRes);
			SetDraw(m_hDrawWnd, UpperCoordinates(pItem->rtArea.left - 1, pItem->rtArea.top - 1, pItem->rtArea.right + 1, pItem->rtArea.bottom + 1), pItem->strRes, _T("fece35"));
		}
	}
	else if (pItem->iType == 5)
	{
		int iRandW = rand() % (pItem->rtArea.Width() / 2);
		int iRandH = rand() % (pItem->rtArea.Height() / 2);

		x = pItem->rtArea.CenterPoint().x + ((rand() % 100 > 50) ? iRandW : -iRandW);
		y = pItem->rtArea.CenterPoint().y + ((rand() % 100 > 50) ? iRandH : -iRandH);
		//DeugString(_T("点击了：") + strUI);

	}
	else if (pItem->iType == 6) //opencv找图
	{
#ifdef OPENCV_NEED
		double sim = cvfind(strUI, ptFind);
		if (sim != 0)
		{
			ptFind.x = ptFind.x;
			ptFind.y = ptFind.y;

			if (ret != -1)
			{
				//DeugString(_T("点击了：") + pItem->strRes);
				SetDraw(m_hDrawWnd, UpperCoordinates(pItem->rtArea.left - 1, pItem->rtArea.top - 1, pItem->rtArea.right + 1, pItem->rtArea.bottom + 1), pItem->strName, _T("3fc13f"));
			}

			return true;
		}
		else
		{
			return false;
		}
#endif	
	}

	ptFind.x = x;
	ptFind.y = y;

	return x != -1;
}

bool CTaskThread::Find(CString strUI, CRect rtArea)
{
	CPoint ptFind;
	return Find(strUI, ptFind, rtArea);
}

bool CTaskThread::FindOcr(CString strUi, CString strFindString, int qa, bool IsClick, CString strClickUi, int Time, int clickSleep)
{
	for (int i = 0; i < Time; i++)
	{
		CString retOcr = Ocr(strUi);
		if (retOcr != _T(""))
		{
			if (Findstr(retOcr, strFindString, qa))
			{
				if (IsClick)
				{
					FindClick(strClickUi);
					SleepEx(clickSleep);
				}
				return true;
			}

		}
		if (Time != 1)
		{
			SleepEx(1000);
		}


	}
	return false;
}

CPoint CTaskThread::FindMS(CString strUI, int iTime)
{
	for (int i = 0; i < iTime; i++)
	{
		CPoint ptFind;
		CStringArray arrTmp;
		Split(strUI, arrTmp, _T("|"));
		for (int i = 0; i < arrTmp.GetSize(); i++)
		{
			CPoint point = NULL;
			if (Find(arrTmp.GetAt(i), point))
			{
				return point;
			}
		}
	}
	return CPoint(0, 0);
}


bool CTaskThread::Find(CString strUI, CPoint& ptFind, int iTime/*找多少次*/)
{
	for (int i = 0; i < iTime; i++)
	{
		CPoint retPoint = CPoint(NULL, NULL);
		if (Find(strUI, retPoint))
		{
			ptFind.x = retPoint.x;
			ptFind.y = retPoint.y;
			return true;
		}
		SleepEx(1000);
	}

	return false;
}

bool CTaskThread::FindClick(CString strUI)
{
	CPoint ptFind;
	if (Find(strUI, ptFind)) {
		Click(ptFind);
		return true;
	}

	return false;
}

bool CTaskThread::FindRClick(CString strUI)
{
	CPoint ptFind;
	if (Find(strUI, ptFind)) {
		RClick(ptFind);
		return true;
	}

	return false;

}

CString CTaskThread::dmFindPicEx(CString strUi)
{
	tagResItem* pItem = g_pEngine->GetResItem(strUi);
	if (!pItem)
	{
		LogE(_T("%s: 没有发现该资源项"), strUi);
		return _T("");
	}
	CString	ret = m_pDm->FindPicEx(pItem->rtArea.left - 1,
		pItem->rtArea.top - 1,
		pItem->rtArea.right + 1,
		pItem->rtArea.bottom + 1,
		g_pEngine->m_strWorkPath + _T("\\res\\") + pItem->strRes, pItem->strDeltaClr, pItem->dSim, pItem->iDir);

	return ret;
}

std::vector<cv::Point> CTaskThread::OpencvFindPicEx(CString	strUI, CRect rtArea)
{

	tagResItem* pItem = g_pEngine->GetResItem(strUI);
	if (!pItem)
	{
		LogE(_T("%s: 没有发现该资源项"), strUI);
		return {};
	}

	DWORD dwTick = GetTickCount();
	CRect rtUI;
	if (rtArea.top) {
		rtUI = rtArea;
	}
	else
	{
	   rtUI = pItem->rtArea;
	}
	

	int iChannels = 4;
	cv::Mat screenMat;
	screenMat.create(cv::Size(rtUI.Width(), rtUI.Height()), CV_MAKETYPE(CV_8U, iChannels));

	int iImgSize = rtUI.Width() * rtUI.Height() * iChannels;
	long screenData = m_pDm->GetScreenData(rtUI.left, rtUI.top, rtUI.right, rtUI.bottom); //取得32位图信息
	memcpy(screenMat.data, (void*)screenData, iImgSize);



	CString strPath = g_pEngine->m_strWorkPath + _T("/res/") + pItem->strRes;
	std::string strPathA = CT2A(strPath.GetString());
	cv::Mat targetImage = screenMat;
	cv::Mat image_24bit = cv::imread(strPathA);
	cv::Mat image_32bit(image_24bit.rows, image_24bit.cols, CV_8UC4);
	cv::cvtColor(image_24bit, image_32bit, cv::COLOR_BGR2BGRA);
	//cv::imwrite(strPathA, image_32bit);
	cv::Mat templateImage = image_32bit;
	cv::Mat resultImage(targetImage.rows, targetImage.cols, CV_32FC1);

	// 使用模板匹配算法
	matchTemplate(targetImage, templateImage, resultImage, cv::TM_CCOEFF_NORMED);

	// 定义阈值，用于确定匹配程度
	double threshold = 0.9;

	std::vector<cv::Point> matches;
	for (int y = 0; y < resultImage.rows; ++y) {
		for (int x = 0; x < resultImage.cols; ++x) {
			if (resultImage.at<float>(y, x) >= threshold) {
				matches.push_back(cv::Point(x+ rtUI.left, y+ rtUI.top));
			}
		}
	}

	//// 标记匹配的区域
	//for (cv::Point match : matches) {
	//	cv::rectangle(targetImage, match, cv::Point(match.x + templateImage.cols, match.y + templateImage.rows), cv::Scalar(0, 0, 255), 2);
	//	//std::cout <<"("<< match.x << "," << match.y << ")" << std::endl;
	//}
	//// 显示结果
	//cv::imshow("Result", templateImage);
	//cv::waitKey(0);

	return  matches;
}



bool CTaskThread::FindSkewClick(CString strUI, CPoint Skew, int iTime, int ClickSleep)
{
	for (int i = 0; i < iTime; i++)
	{
		CPoint ptFind;
		if (Find(strUI, ptFind))
		{
			ptFind.x += Skew.x;
			ptFind.y += Skew.y;
			Click(GetRandom(ptFind.x, ptFind.x + 10), GetRandom(ptFind.y, ptFind.y + 10));
			SleepEx(GetRandom(ClickSleep, 2500));
			return true;
		}

		if (iTime != 1)
			SleepEx(1000);
	}

	return false;
}

bool CTaskThread::FindPoint(CString strUI, CString PointUi, int iTime, int ClickSleep)
{
	for (int i = 0; i < iTime; i++)
	{
		if (Find(strUI))
		{
			FindClick(PointUi);
			SleepEx(GetRandom(ClickSleep, 2000));
			return true;
		}
	}
	return false;
}


#ifdef OPENCV_NEED
double CTaskThread::cvfind(CString strUI, CPoint& ptFind)
{
	tagResItem* pItem = g_pEngine->GetResItem(strUI);
	if (!pItem)
	{
		LogE(_T("%s: 没有发现该资源项"), strUI);
		return false;
	}

	int type = pItem->matPic.type();
	if (pItem->matPic.type() != 24)
	{
		LogE(_T("待查找的图片不是32位图像，无法使用opencv的方式查找，名称:%s"), pItem->strName);
		return 0.0;
	}

	DWORD dwTick = GetTickCount();
	CRect rtUI = pItem->rtArea;

	int iChannels = 4;
	cv::Mat screenMat;
	screenMat.create(cv::Size(rtUI.Width(), rtUI.Height()), CV_MAKETYPE(CV_8U, iChannels));

	int iImgSize = rtUI.Width() * rtUI.Height() * iChannels;
	long screenData = m_pDm->GetScreenData(rtUI.left, rtUI.top, rtUI.right, rtUI.bottom); //取得32位图信息
	memcpy(screenMat.data, (void*)screenData, iImgSize);

	cv::Mat result;
	int result_cols = screenMat.cols - pItem->matPic.cols + 1;
	int result_rows = screenMat.rows - pItem->matPic.rows + 1;
	if (result_cols < 0 || result_rows < 0)
	{
		screenMat.release();
		return 0.0f;
	}

	//TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5
	result.create(result_rows, result_cols, CV_MAKETYPE(CV_8U, iChannels));

	int match_method = cv::TM_CCOEFF_NORMED;
	matchTemplate(screenMat, pItem->matPic, result, match_method);

	double minVal = 0;
	double maxVal = 0;
	cv::Point minLoc = cv::Point(0, 0);
	cv::Point maxLoc = cv::Point(0, 0);
	cv::Point matchLoc = cv::Point(0, 0);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	if (maxVal >= pItem->dSim)
	{
		ptFind.x = maxLoc.x + pItem->matPic.cols / 2;
		ptFind.y = maxLoc.y + pItem->matPic.rows / 2;

		ptFind.x += rtUI.left;
		ptFind.y += rtUI.top;
	}
	else {
		maxVal = 0.0f;
		ptFind.x = 0;
		ptFind.y = 0;
	}


	screenMat.release();
	result.release();

	return maxVal;
}


cv::Mat CTaskThread::cv截图转换图像(long x1, long y1, long x2, long y2)
{
	if (x1 == x2 || y1 == y2) {
		LogD(_T("坐标异常"));

	}


	int iChannels = 4;

	int 宽 = x2 - x1;
	int 高 = y2 - y1;
	cv::Mat screenMat;
	cv::Mat screenMat2;
	screenMat.create(cv::Size(宽, 高), CV_MAKETYPE(CV_8U, iChannels));//释放内存重新创建图像

	int iImgSize = 宽 * 高 * iChannels;
	long screenData = m_pDm->GetScreenData(x1, y1, x2, y2); //取得32位图信息
	memcpy(screenMat.data, (void*)screenData, iImgSize);
	cv::cvtColor(screenMat, screenMat2, cv::COLOR_BGR2GRAY);// cv::COLOR_BGR2GRAY == 灰度 

	screenMat.release();

	return screenMat2;
}

cv::Mat CTaskThread::cv截图转换图像(dmsoft* dm, long x1, long y1, long x2, long y2)
{
	if (x1 == x2 || y1 == y2) {
		LogD(_T("坐标异常"));

	}

	int iChannels = 4;

	int 宽 = x2 - x1;
	int 高 = y2 - y1;
	cv::Mat screenMat;
	cv::Mat screenMat2;
	screenMat.create(cv::Size(宽, 高), CV_MAKETYPE(CV_8U, iChannels));//释放内存重新创建图像

	int iImgSize = 宽 * 高 * iChannels;
	long screenData = dm->GetScreenData(x1, y1, x2, y2); //取得32位图信息
	memcpy(screenMat.data, (void*)screenData, iImgSize);
	cv::cvtColor(screenMat, screenMat2, cv::COLOR_BGR2GRAY);// cv::COLOR_BGR2GRAY == 灰度 

	screenMat.release();

	return screenMat2;

}

bool CTaskThread::cv模板匹配(cv::Mat& max_img, cv::Mat& min_img, CPoint& 坐标, double 相似度阈值)
{
	int iChannels = 4;
	cv::Mat 结果图像;
	int result_cols = max_img.cols - min_img.cols + 1;//构建结果图像尺寸
	int result_rows = max_img.rows - min_img.rows + 1;//构建结果图像尺寸
	if (result_cols < 0 || result_rows < 0)
		min_img.release();

	结果图像.create(result_rows, result_cols, CV_MAKETYPE(CV_8U, iChannels));//构造函数创建图像

	//TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5
	int match_method = cv::TM_CCOEFF_NORMED;
	cv::matchTemplate(max_img, min_img, 结果图像, match_method);
	double minVal = 0;
	double maxVal = 0;
	cv::Point minLoc = cv::Point(0, 0);
	cv::Point maxLoc = cv::Point(0, 0);
	cv::Point matchLoc = cv::Point(0, 0);
	minMaxLoc(结果图像, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());   // 寻找<结果图像>中的最大和最小值，以及它们所处的像素位置  
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	if (maxVal >= 相似度阈值)
	{
		坐标.x = maxLoc.x + min_img.cols / 2;
		坐标.y = maxLoc.y + min_img.rows / 2;
	}
	else {
		坐标.x = 0;
		坐标.y = 0;
	}

	return 坐标.x != 0;
}

bool CTaskThread::cv模板匹配(cv::Mat& max_img, cv::Mat& min_img, double 相似度阈值)
{
	CPoint 局_坐标;

	return cv模板匹配(max_img, min_img, 局_坐标, 相似度阈值);
}

double CTaskThread::cv模板匹配返回相似度(cv::Mat& max_img, cv::Mat& min_img)
{
	int iChannels = 4;
	cv::Mat 结果图像;
	int result_cols = max_img.cols - min_img.cols + 1;//构建结果图像尺寸
	int result_rows = max_img.rows - min_img.rows + 1;//构建结果图像尺寸
	if (result_cols < 0 || result_rows < 0)
		min_img.release();

	结果图像.create(result_rows, result_cols, CV_MAKETYPE(CV_8U, iChannels));//构造函数创建图像

	cv::matchTemplate(max_img, min_img, 结果图像, cv::TM_CCOEFF_NORMED);
	double minVal = 0;
	double maxVal = 0;
	cv::Point minLoc = cv::Point(0, 0);
	cv::Point maxLoc = cv::Point(0, 0);
	cv::Point matchLoc = cv::Point(0, 0);
	minMaxLoc(结果图像, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());   // 寻找<结果图像>中的最大和最小值，以及它们所处的像素位置  

	结果图像.release();
	return maxVal;
}

cv::Mat CTaskThread::cv读入图像(cv::String 图片路径, int 读入模式)
{
	double 缩放 = 1.0;

	return cv读入图像(图片路径, 读入模式, 缩放);;
}

cv::Mat CTaskThread::cv读入图像(cv::String 图片路径, int 读入模式, double 缩放大小)
{
	cv::Mat 图像 = cv::imread(图片路径, 读入模式);
	cv::Mat 缩放后图像;
	cv::resize(图像, 缩放后图像, cv::Size(0, 0), 缩放大小, 缩放大小);

	//cv::imwrite(图片路径 + "hhh", 缩放后图像);
	//cv::imwrite("D:\\Data\\大聪明寻路\\DCM_命运方舟\\DCM\\Debug\\res\\重做地图\\二值化罗格希尔2.bmp", 缩放后图像); //保存放大后的地图

	return 缩放后图像;
}

double CTaskThread::cv动态缩放测试比例(cv::Mat& 大图, cv::Mat& 小图, bool 显示结果图, int 大小图, double 开始比例, int 次数)
{
	double 比例 = 开始比例;
	double 最佳比例 = 0.2;
	double 匹配返回相似度 = 0.1;
	double 相似度 = 0.2;
	CPoint 坐标 = CPoint(0, 0);
	cv::Mat 待缩放图像;
	cv::Mat 缩放后图像;
	cv::Mat 显示图像;
	cv::Size 尺寸 = cv::Size(0, 0);

	if (大小图 == 1)
	{
		for (int i = 0; i < 次数; i++)
		{
			待缩放图像 = 大图;
			cv::resize(待缩放图像, 缩放后图像, 尺寸, 比例, 比例);
			匹配返回相似度 = cv模板匹配返回相似度(缩放后图像, 小图);
			if (匹配返回相似度 > 相似度)
			{
				相似度 = 匹配返回相似度;
				最佳比例 = 比例;
			}
			比例 += 0.1;
		}

		LogE(_T("大图缩放最佳比例:%f,相似度:%f"), 最佳比例, 相似度);
		if (显示结果图)
		{
			cv::resize(大图, 显示图像, 尺寸, 最佳比例, 最佳比例);
			cv模板匹配(显示图像, 小图, 坐标);
			cv::Point 画图坐标 = cv::Point(int(坐标.x), int(坐标.y));
			cv::circle(显示图像, 画图坐标, 3, cv::Scalar(255, 255, 255));
			cv::imshow("显示图像", 显示图像);
			cv::waitKey();
		}
	}
	else
	{
		for (int i = 0; i < 次数; i++)
		{
			待缩放图像 = 小图;
			cv::resize(待缩放图像, 缩放后图像, 尺寸, 比例, 比例);
			匹配返回相似度 = cv模板匹配返回相似度(大图, 缩放后图像);
			if (匹配返回相似度 > 相似度)
			{
				相似度 = 匹配返回相似度;
				最佳比例 = 比例;

			}
			比例 += 0.1;
		}

		LogE(_T("小图缩放最佳比例:%f,相似度:%f"), 最佳比例, 相似度);

		if (显示结果图)
		{
			cv::resize(小图, 显示图像, 尺寸, 最佳比例, 最佳比例);
			cv模板匹配(大图, 显示图像, 坐标);
			cv::Point 画图坐标 = cv::Point(int(坐标.x), int(坐标.y));
			cv::circle(大图, 画图坐标, 3, cv::Scalar(255, 255, 255));
			cv::imshow("显示图像", 大图);
			cv::waitKey();
		}

	}

	//cv::imwrite("D:\\Data\\大聪明寻路\\DCM_命运方舟\\DCM\\Debug\\res\\重做地图\\罗格希尔缩放后.bmp", 缩放后图像);

	待缩放图像.release();
	缩放后图像.release();
	显示图像.release();

	return 最佳比例;
}

CPoint CTaskThread::斜率公式(CPoint 点1, CPoint 点2, CPoint 原点, int 长度)
{
	double 弧度 = atan2(点2.y - 点1.y, 点2.x - 点1.x);
	CPoint ptRet = CPoint(0, 0);
	ptRet.x = 原点.x + 长度 * cos(弧度);
	ptRet.y = 原点.y + 长度 * sin(弧度);

	return ptRet;
}

double CTaskThread::GetPtDis(CPoint pt1, CPoint pt2)
{
	return sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2));
}

#endif

int CTaskThread::Yolo(std::vector<YoloObject>& objs)
{
	g_pEngine->m_lkYolo.Lock();

	long screenData = 0;
	screenData = m_pDm->GetScreenData(0, 0, g_pEngine->m_iWidth, g_pEngine->m_iHeight);
	if (screenData == 0)
		return 0;

	CYolo7& yolo7 = CYolo7::get_instance();
	int count = yolo7.Detect((BYTE*)screenData, objs);

	g_pEngine->m_lkYolo.UnLock();

	return count;
}

void CTaskThread::MoveTo(int x, int y)
{
	DLLMoveTo(x, y);
	//m_pDm->MoveTo(x, y);
}

void CTaskThread::MoveR(int rx, int ry)
{
	m_pDm->MoveR(rx, ry);
}

void CTaskThread::LeftClick()
{
	m_pDm->LeftClick();
}

void CTaskThread::LeftDown()
{
	m_pDm->LeftDown();
}

void CTaskThread::LeftUp()
{
	m_pDm->LeftUp();
}

void CTaskThread::RightClick()
{
	m_pDm->RightClick();
}

void CTaskThread::RightDown()
{
	m_pDm->RightDown();
}

void CTaskThread::RightUp()
{
	m_pDm->RightUp();
}

void CTaskThread::WheelDown()
{
	m_pDm->WheelDown();
}

void CTaskThread::WheelUp()
{
	m_pDm->WheelUp();
}

bool CTaskThread::clk(CPoint ptClk)
{
	Click(ptClk);

	return true;
}

bool CTaskThread::clk(CString strUI, int iCount, int iSleep)
{
	int i = 0;
	while (i++ < iCount)
	{
		CPoint ptFind;
		if (Find(strUI, ptFind))
		{

			Click(ptFind);
		}
		else {
			return false;
		}

		SleepEx(iSleep);
	}

	return true;
}

bool CTaskThread::Click(CString strUI, int iCount, int iSleep)
{
	return clk(strUI, 1, GetRandom(iSleep, 2000));
}

bool CTaskThread::cw(CString strUI, CString strWaitUI, bool bShow, int iCount, int iSleep)
{
	tagResItem* pItem = g_pEngine->GetResItem(strUI);
	if (!pItem)
	{
		ASSERT(FALSE);
		LogE(_T("%s: 没有发现该资源项"), strUI);
		return false;
	}

	if (!bShow)
	{
		if (!Find(strWaitUI))
			return true;
	}
	else {
		if (Find(strWaitUI))
			return true;
	}

	int i = 0;
	while (i++ < iCount)
	{
		if (!clk(strUI))
		{
			LogN(_T("cw: %s 条件未满足, 无法点击!"), strUI);
		}

		iSleep = iSleep + rand() % (iSleep / 2);
		SleepEx(iSleep);

		if (bShow ? Find(strWaitUI) : !Find(strWaitUI))
			return true;
	}

	return false;
}

bool CTaskThread::WClick(CString strUI, int iSleep)
{
	int  CurrentTime = GetTimeSeconds();
	while (true)
	{
		CPoint FindPoint{ NULL ,NULL };
		if (Find(strUI, FindPoint))
		{
			Click(FindPoint);
			return true;
		}

		if (GetTimeSeconds() - CurrentTime >= iSleep)
			return  false;

		SleepEx(100);
	}

	return false;
}
bool CTaskThread::WClick(CString strUI, CRect Rct, int iSleep)
{
	int  CurrentTime = GetTimeSeconds();
	while (true)
	{
		CPoint FindPoint{ NULL ,NULL };
		if (Find(strUI, FindPoint), Rct)
		{
			Click(FindPoint);
			return true;
		}

		if (GetTimeSeconds() - CurrentTime >= iSleep)
			return  false;

		SleepEx(100);
	}

	return false;
}
bool CTaskThread::WFind(CString strUI, int iSleep)
{
	int  CurrentTime = GetTimeSeconds();
	while (true)
	{
		CPoint FindPoint{ NULL ,NULL };
		if (Find(strUI, FindPoint))
			return true;

		if (GetTimeSeconds() - CurrentTime >= iSleep)
			return  false;

		SleepEx(100);
	}

	return false;
}

bool CTaskThread::WFindRetPoint(CString strUI, CPoint& ptFind, int iSleep)
{
	int  CurrentTime = GetTimeSeconds();
	while (true)
	{
		CPoint FindPoint{ NULL ,NULL };
		if (Find(strUI, FindPoint)) {
			ptFind = FindPoint;
			return true;
		}

		if (GetTimeSeconds() - CurrentTime >= iSleep)
			return  false;

		SleepEx(100);
	}

	return false;

}

bool CTaskThread::WFind(CString strUI, CRect Rct, int iSleep)
{
	int  CurrentTime = GetTimeSeconds();
	while (true)
	{
		CPoint FindPoint{ NULL ,NULL };
		if (Find(strUI, FindPoint, Rct))
			return true;

		if (GetTimeSeconds() - CurrentTime >= iSleep)
			return  false;

		SleepEx(100);
	}
	return false;
}



int  CTaskThread::GetTimeSeconds()
{
	auto now = std::chrono::system_clock::now();
	auto millisec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	return  (int)millisec_since_epoch;
}

UpperCoordinates* CTaskThread::ComputeSlideData(CRect StartingPoint, CRect EndPoint, UpperCoordinates* pUp)
{

	ASSERT(StartingPoint);
	ASSERT(EndPaint);

	UpperCoordinates* Tmp = pUp;
	CRect RxTmp1 = StartingPoint;
	CRect RyTmp1 = StartingPoint;
	CRect RxTmp2 = EndPoint;
	CRect RyTmp2 = EndPoint;

	//获取随机滑动起点
	Tmp->x1 = GetRandom(RxTmp1.left, RxTmp1.right);
	Tmp->y1 = GetRandom(RxTmp1.top, RxTmp1.bottom);
	//获取随机滑动终点
	Tmp->x2 = GetRandom(RxTmp2.left, RxTmp2.right);
	Tmp->y2 = GetRandom(RxTmp2.top, RxTmp2.bottom);
	return Tmp;
}

void CTaskThread::Slide(UpperCoordinates* pUp)
{
	m_pDm->EnableRealMouse(3, GetRandom(50, 80), GetRandom(20, 30));
	m_pDm->MoveTo(pUp->x1, pUp->y1);
	//m_pDm->MoveTo(611, 154);
	SleepEx(GetRandom(20, 50));
	m_pDm->LeftDown();
	SleepEx(GetRandom(20, 50));
	m_pDm->MoveTo(pUp->x2, pUp->y2);
	//m_pDm->MoveTo(591, 467);
	SleepEx(GetRandom(20, 50));
	m_pDm->LeftUp();
	SleepEx(GetRandom(20, 50));
}

CString CTaskThread::HttpGet(CString strUrl, CString strParam)
{
	DWORD dwTick = GetTickCount();

	strUrl = strUrl + _T("?") + strParam;

	HttpRequest req;
	req.method = HTTP_GET;
	req.url = HUrl::escapeUrl(GbkToUtf8(strUrl.GetString()));
	req.headers["Connection"] = "keep-alive";
	req.timeout = 3;

	http_client_t* client = http_client_new();

	HttpResponse resp;
	int ret = http_client_send(client, &req, &resp);
	if (ret != 0)
	{
		LogE(_T("%s: HttpGet: url:%s 请求超时，检查网络"), m_pWnd->strTitle, strUrl);
	}
	else
	{
		std::string szBody;
		resp.DumpBody(szBody);

		CString strRet = CA2CW(Utf8ToGbk(szBody.c_str()).c_str());

		LogN(_T("%s: HttpGet: url:%s，耗时:%d ms"), m_pWnd->strTitle, strUrl, GetTickCount() - dwTick);

		return strRet;
	}

	return _T("");
}

CString CTaskThread::HttpPost(CString strUrl, CMapStringToString& pMap)
{
	HttpRequest req;
	req.method = HTTP_POST;
	req.url = CW2CA(strUrl);
	req.headers["Connection"] = "keep-alive";
	req.timeout = 1;
	req.content_type = APPLICATION_URLENCODED;

	CString strKey;
	CString strValue;
	POSITION Position = pMap.GetStartPosition();
	while (Position != NULL)
	{
		pMap.GetNextAssoc(Position, strKey, strValue);
		req.Set(CW2CA(strKey), CW2CA(strValue));
	}

	http_client_t* client = http_client_new();

	HttpResponse resp;
	int ret = http_client_send(client, &req, &resp);
	if (ret != 0)
	{
		LogE(_T("%s: HttpPost: url:%s  请求超时，检查网络"), m_pWnd->strTitle, strUrl);
	}
	else
	{
		std::string szBody;
		resp.DumpBody(szBody);

		return CA2CW(Utf8ToGbk(szBody.c_str()).c_str());
	}

	return _T("");
}

CString CTaskThread::HttpPost(CString strUrl, CString strKey, CString strData)
{
	HttpRequest req;
	req.method = HTTP_POST;
	req.url = CW2CA(strUrl);
	req.headers["Connection"] = "keep-alive";
	req.timeout = 3;
	req.content_type = APPLICATION_URLENCODED;

	req.Set(CW2CA(strKey), CW2CA(strData));

	http_client_t* client = http_client_new();

	HttpResponse resp;
	int ret = http_client_send(client, &req, &resp);
	if (ret != 0)
	{
		LogE(_T("%s: HttpPost: url:%s  请求超时，检查网络"), m_pWnd->strTitle, strUrl);
	}
	else
	{
		std::string szBody;
		resp.DumpBody(szBody);

		return CA2CW(szBody.c_str());
	}

	return _T("");
}


void CTaskThread::KillApp()
{
	g_pEngine->KillApp(m_pWnd->id);
}
#include "ocrlite/ocrlite.h"
CString CTaskThread::Ocr(CRect rtArea)
{
	//g_pEngine->m_lkOcr.Lock();
	//CString strRet = _T("");
	//do
	//{
	//	long screenData = 0;
	//	screenData = m_pDm->GetScreenData(rtArea.left, rtArea.top, rtArea.right, rtArea.bottom);
	//	if (screenData == 0)
	//		break;

	//	CAiTool& ocrTool = CAiTool::get_instance();
	//	strRet = ocrTool.Ocr(m_pWnd->id, (byte*)screenData, CSize(rtArea.right - rtArea.left, rtArea.bottom - rtArea.top));
	//} while (FALSE);

	//g_pEngine->m_lkOcr.UnLock();
	std::vector<OcrRes>  res = ocr2(rtArea);
	CString strTmp;
	for (int i = 0; i < res.size(); i++)
	{
		strTmp += res[i].strResult;
	}
	return strTmp;
}

CPoint CTaskThread::Ocr(CRect rtArea, CString str, int Q)
{
	if (Q == NULL)
		Q = str.GetLength();

	std::vector<OcrRes>  res = ocr2(rtArea);
	for (int j = 0; j < res.size(); j++) {
		if (Findstr(res[j].strResult, str, Q)) {
			return{ rtArea.left + res[j].iX, rtArea.top + res[j].iY };
		}
	}
	return false;
}

static int g_dwNumberOfProcessors = ([]() -> int {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
	})();

	static inline std::string Utf16ToUtf8(const std::wstring utf16)
	{
		if (utf16.empty()) {
			return {};
		}

		/*get utf16 length*/
		int read = WideCharToMultiByte(CP_UTF8, 0, utf16.data(), utf16.size(), NULL, 0, NULL, NULL);
		if (read <= 0) {
			abort();
		}

		std::string utf8 = {};
		utf8.resize(read);

		/*utf16 to utf8*/
		int write = WideCharToMultiByte(CP_UTF8, 0, utf16.data(), utf16.size(), (char*)utf8.data(), utf8.size(), NULL, NULL);
		if (write != read) {
			abort();
		}
		return utf8;
	}


	static inline std::wstring Utf8ToUtf16(const std::string utf8)
	{
		if (utf8.empty()) {
			return {};
		}

		/*get utf8 length*/
		int read = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
		if (read <= 0) {
			abort();
		}
		std::wstring utf16 = {};
		utf16.resize(read);

		/*utf8 to utf16*/
		int write = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), (wchar_t*)utf16.data(), utf16.size());
		if (write != read) {
			abort();
		}
		return utf16;
	}
	std::vector<OcrRes> CTaskThread::ocr2(CRect rtArea)
	{
		long screenData = 0;
		screenData = m_pDm->GetScreenData(rtArea.left, rtArea.top, rtArea.right, rtArea.bottom);
		if (screenData == 0) {
			return {};
		}
		CSize imgSize(rtArea.right - rtArea.left, rtArea.bottom - rtArea.top);
		cv::Mat img;
		img.create(cv::Size(imgSize.cx, imgSize.cy), CV_MAKETYPE(CV_8U, 4));
		memcpy(img.data, (void*)screenData, imgSize.cx * imgSize.cy * 4);
		cv::cvtColor(img, img, cv::IMREAD_COLOR);
		ocrlite::OcrLite ocr;
		auto rst = ocr.initModels("", "", "", "");
		ocr.setGpuIndex(-1);
		auto start = GetTickCount64();
		auto result = ocr.detect(img, 50, 1024, 0.5f, 0.3f, 1.6f, false, false);
		auto t = (GetTickCount64() - start);
		//LogE(_T("tid:%d, ms:%d"), GetCurrentThreadId(), t);
		auto results = std::vector<OcrRes>();
		for (auto e : result.textBlocks) {
			OcrRes res;
			res.iX = e.boxPoint.at(0).x;
			res.iY = e.boxPoint.at(0).y;
			res.strResult = Utf8ToUtf16(e.text).c_str();
			results.push_back(res);
		}
		return results;
	}

	std::vector<OcrUpgrade> CTaskThread::ocrOcrUpgrade(CRect rtArea)
	{
		std::vector<OcrRes> vOcrRes = ocr2(rtArea);
		std::vector<OcrUpgrade> vOcrUpgrade;
		for (size_t i = 0; i < vOcrRes.size(); i++){
			vOcrUpgrade.push_back(OcrUpgrade(vOcrRes[i].strResult, vOcrRes[i].iX, vOcrRes[i].iY, false));
		}

		return vOcrUpgrade;
	}


	bool CTaskThread::FindOcrClick(CString strUi, CString _Findstr, int Time, int Sleep)
	{
		tagResItem* pItem = g_pEngine->GetResItem(strUi);
		if (!pItem)
		{
			LogE(_T("%s: 没有发现该资源项"), strUi);
			return false;
		}
		for (int i = 0; i < Time; i++)
		{
			std::vector<OcrRes>  res = ocr2(pItem->rtArea);
			for (int j = 0; j < res.size(); j++)
			{
				if (Findstr(res[j].strResult, _Findstr, _Findstr.GetLength()))
				{
					Click(CPoint(pItem->rtArea.left + res[j].iX + GetRandom(10, 15), pItem->rtArea.top + res[j].iY + GetRandom(10, 15)));
					SleepEx(Sleep);
					return true;
				}
			}


			if (Time != 1)
			{
				SleepEx(Sleep);
			}
		}

		return false;
	}

	bool CTaskThread::WOcrFindCick(CString strUI, int iSleep)
	{
		int  CurrentTime = GetTimeSeconds();
		tagResItem* pItem = g_pEngine->GetResItem(strUI);
		if (!pItem) {
			LogE(_T("%s: 没有发现该资源项"), strUI);
			return false;
		}

		while (true) {
			std::vector<OcrRes>  res = ocr2(pItem->rtArea);
			for (int j = 0; j < res.size(); j++) {
				if (Findstr(res[j].strResult, pItem->strRes, pItem->strRes.GetLength())) {
					CPoint FindPoint{ pItem->rtArea.left + res[j].iX, pItem->rtArea.top + res[j].iY };
					Click(FindPoint);
					return true;
				}
			}

			if (GetTimeSeconds() - CurrentTime >= iSleep)
				return false;

			SleepEx(100);
		}
		return false;
	}
	bool CTaskThread::WOcrFindCick(CString strUI, CRect rtArea, int iSleep)
	{
		int  CurrentTime = GetTimeSeconds();
		tagResItem* pItem = g_pEngine->GetResItem(strUI);
		pItem->rtArea = rtArea;

		if (!pItem) {
			LogE(_T("%s: 没有发现该资源项"), strUI);
			return false;
		}

		while (true) {
			std::vector<OcrRes>  res = ocr2(pItem->rtArea);
			for (int j = 0; j < res.size(); j++) {
				if (Findstr(res[j].strResult, pItem->strRes, pItem->strRes.GetLength())) {
					CPoint FindPoint{ pItem->rtArea.left + res[j].iX, pItem->rtArea.top + res[j].iY };
					Click(FindPoint);
					return true;
				}
			}

			if (GetTimeSeconds() - CurrentTime >= iSleep)
				return false;

			SleepEx(100);
		}
		return false;
	}
	bool CTaskThread::WOcrFind(CString strUI, int iSleep)
	{
		int  CurrentTime = GetTimeSeconds();
		tagResItem* pItem = g_pEngine->GetResItem(strUI);

		if (!pItem) {
			LogE(_T("%s: 没有发现该资源项"), strUI);
			return false;
		}

		while (true) {
			std::vector<OcrRes>  res = ocr2(pItem->rtArea);
			for (int j = 0; j < res.size(); j++) {
				if (Findstr(res[j].strResult, pItem->strRes, pItem->strRes.GetLength())) {
					CPoint FindPoint{ pItem->rtArea.left + res[j].iX, pItem->rtArea.top + res[j].iY };
					return true;
				}
			}

			if (GetTimeSeconds() - CurrentTime >= iSleep)
				return false;

			SleepEx(100);
		}
		return false;
	}

	bool CTaskThread::WOcrFind(CString strUI, CRect rtArea, int iSleep)
	{
		int  CurrentTime = GetTimeSeconds();
		tagResItem* pItem = g_pEngine->GetResItem(strUI);
		pItem->rtArea = rtArea;

		if (!pItem) {
			LogE(_T("%s: 没有发现该资源项"), strUI);
			return false;
		}

		while (true) {
			std::vector<OcrRes>  res = ocr2(pItem->rtArea);
			for (int j = 0; j < res.size(); j++) {
				if (Findstr(res[j].strResult, pItem->strRes, pItem->strRes.GetLength())) {
					CPoint FindPoint{ pItem->rtArea.left + res[j].iX, pItem->rtArea.top + res[j].iY };
					return true;
				}
			}

			if (GetTimeSeconds() - CurrentTime >= iSleep)
				return false;

			SleepEx(100);
		}
		return false;
	}

	CPoint CTaskThread::FindOcrPoint(CString strUi, CString _Findstr, int& size, int Time)
	{
		tagResItem* pItem = g_pEngine->GetResItem(strUi);
		if (!pItem)
		{
			LogE(_T("%s: 没有发现该资源项"), strUi);
			return false;
		}
		for (int i = 0; i < Time; i++)
		{
			std::vector<OcrRes>  res = ocr2(pItem->rtArea);
			for (int j = 0; j < res.size(); j++)
			{
				if (Findstr(res[j].strResult, _Findstr, _Findstr.GetLength()))
				{
					size = res.size();
					return CPoint(pItem->rtArea.left + res[j].iX, pItem->rtArea.top + res[j].iY);
				}
			}

			if (Time != 1)
				SleepEx(1000);
		}

		return CPoint(NULL, NULL);
	}


	CString CTaskThread::Ocr(CString strUI)
	{
		tagResItem* pItem = g_pEngine->GetResItem(strUI);
		if (!pItem)
		{
			LogE(_T("%s: 没有发现该资源项"), strUI);
			return _T("");
		}

		//SetDraw(m_hDrawWnd, UpperCoordinates(pItem->rtArea.left - 1, pItem->rtArea.top - 1, pItem->rtArea.right + 1, pItem->rtArea.bottom + 1), pItem->strName, _T("62db03"));
		std::vector<OcrRes>  res = ocr2(pItem->rtArea);
		CString strTmp;
		for (int i = 0; i < res.size(); i++)
		{
			strTmp += res[i].strResult;
		}
		return strTmp;
	}

	int CTaskThread::yolo(int iCfgIndex, tagGameObj* pObj)
	{
		g_pEngine->m_lkYolo.Lock();
		int iCount = 0;
		do {
			long screenData = 0;
			screenData = m_pDm->GetScreenData(0, 0, g_pEngine->m_iWidth, g_pEngine->m_iHeight);
			if (screenData == 0)
				break;

			CAiTool& ocrTool = CAiTool::get_instance();
			iCount = ocrTool.Yolo(m_pWnd->id, (byte*)screenData, CSize(g_pEngine->m_iWidth, g_pEngine->m_iHeight), iCfgIndex, pObj);
		} while (FALSE);

		g_pEngine->m_lkYolo.UnLock();
		return iCount;
	}

	void CTaskThread::DeugString(CString str) {
		CTime currentTime = CTime::GetCurrentTime();
		CString CstrTime = currentTime.Format(_T("%m-%d %H.%M.%S"));
		CString DebugString;
		DebugString.Format(_T("%s%s"), CstrTime, str);
		OutputDebugString(DebugString + _T("\n"));
	}

	int CTaskThread::GetRandom(int iMin, int iMax)
	{
		return rand() % (iMax - iMin) + iMin;
	}

	int CTaskThread::GetColorNum(CRect rtArea, CString strColor, double sim)
	{
		return m_pDm->GetColorNum(rtArea.left, rtArea.top, rtArea.right, rtArea.bottom, strColor, sim);
	}

	bool CTaskThread::Findstr(CString _strSwaitstring, CString _strFindString, int _IQuantity)//找字，找到符合足够的字，返回true
	{
		CString strFindString = _strFindString;
		CString strSwaitstring = _strSwaitstring;
		int IQuantity = _IQuantity;
		for (int i = 0; i < strFindString.GetLength(); i++)
		{
			int IsExist = strSwaitstring.Find(strFindString.GetAt(i));
			if (IsExist != -1)
			{
				IQuantity--;
				if (IQuantity == 0)
				{
					return true;
				}
			}

		}
		return false;
	}


	void CTaskThread::SetDraw(long LDrawHand, UpperCoordinates  UpCoordinates, CString strTitle, CString strColour)
	{
		if (g_pEngine->tagOnDraw)
		{
			m_pDm->FoobarFillRect(m_hDrawWnd, 0, 0, 960 + 19, 640 + 30, _T("000000"));
			m_pDm->FoobarDrawText(m_hDrawWnd,
				UpCoordinates.x1,
				UpCoordinates.y1 - 28,
				UpCoordinates.x2,
				UpCoordinates.y2 - 28, strTitle, strColour, 0);

			m_pDm->FoobarFillRect(m_hDrawWnd, UpCoordinates.x1, UpCoordinates.y1, UpCoordinates.x2, UpCoordinates.y2, _T("ff0000"));
			m_pDm->FoobarFillRect(m_hDrawWnd, UpCoordinates.x1 + 1, UpCoordinates.y1 + 1, UpCoordinates.x2 - 1, UpCoordinates.y2 - 1, _T("000000"));
			m_pDm->FoobarUpdate(m_hDrawWnd);
		}

	}

	bool CTaskThread::CleanFile()
	{
		g_pEngine->m_lkTaskFile.Lock();
		if (FindClick(_T("拍照")))
		{
			CString ret = m_pDm->EnumWindow(0, _T("另存为"), _T("#32770"), 1 + 2 + 16);
			CString re = m_pDm->EnumWindow(_ttoi(ret), _T("取消"), _T(""), 1);
			if (re != _T(""))
			{
				int x = 0, y = 0;
				LPARAM lParam = MAKELPARAM(x, y);
				::SendMessage((HWND)_ttoi(re), WM_LBUTTONDOWN, MK_LBUTTON, lParam);
				::SendMessage((HWND)_ttoi(re), WM_LBUTTONUP, 0, lParam);
				return true;
			}
		}
		g_pEngine->m_lkTaskFile.UnLock();
		return false;
	}




	VOID CTaskThread::GetAllKeyValueOfIniFileOneSection(TCHAR* pIniFilePath, TCHAR* pSectionName, std::map<CString, CString>& mapIniKeyValue)
	{
		TCHAR szBuf[4096] = { 0 };
		DWORD readlen = ::GetPrivateProfileSection(pSectionName, szBuf, 4096, pIniFilePath);
		TCHAR* pbuf = szBuf;
		size_t size = _tcslen(pbuf);

		while (size)
		{
			CString str = pbuf;
			CString strKey, strValue;
			if (AfxExtractSubString(strKey, (LPCTSTR)str, 0, _T('=')) && AfxExtractSubString(strValue, (LPCTSTR)str, 1, _T('=')))
			{
				if (!mapIniKeyValue.count(strKey))
				{
					mapIniKeyValue.insert(std::make_pair(strKey, strValue));
				}
			}
			pbuf += size + 1;
			size = _tcslen(pbuf);
		}

		return;
	}

	bool CTaskThread::iniGetAllKeyValueAllNodeNames(CString strIniFilePath, CString strSectionName, std::map<CString, CString>& mapIniKeyValue)
	{
		GetAllKeyValueOfIniFileOneSection(strIniFilePath.GetBuffer(), strSectionName.GetBuffer(), mapIniKeyValue);
		if (mapIniKeyValue.size() != NULL)
			return true;
	}

	CString CTaskThread::iniGetValue(CString strIniFilePath, CString sectionName, CString strKeyName)
	{
		CString retStr = _T("");
		GetPrivateProfileString(sectionName, strKeyName, _T(""), retStr.GetBuffer(MAX_PATH), MAX_PATH, strIniFilePath);
		CString Tmpstr = retStr;
		retStr.ReleaseBuffer();
		return Tmpstr;
	}

	CPoint CTaskThread::strToPoint(CString str)
	{
		CStringArray arrStr;
		Split(str, arrStr, _T(","));
		if (arrStr.GetCount() == 2)
			return(CPoint(_ttoi(arrStr[0]), _ttoi(arrStr[1])));
	}

	//DmYolo
	long CTaskThread::AiYoloSetModel(int index, CString file, CString pwd)
	{
		return m_pDm->AiYoloSetModel(index, file, pwd);
	}

	long CTaskThread::AiYoloUseModel(int index)
	{
		return m_pDm->AiYoloUseModel(index);
	}

	CString CTaskThread::AiYoloDetectObjects(int x1, int y1, int x2, int y2, double prob, double iou)
	{
		return m_pDm->AiYoloDetectObjects(x1, y1, x2, y2, prob, iou);
	}

	long CTaskThread::LoadAi(CString file)
	{
		return m_pDm->LoadAi(file);
	}