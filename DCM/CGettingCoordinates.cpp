#include "pch.h"
#include"CTaskScript.h"
#include "CGettingCoordinates.h"
#include "CDCMEngine.h"
#include"CWayFind.h"

extern CDCMEngine* g_pEngine;

CGetCoordinates::CGetCoordinates(CWayFind* pWayFind)
{
	m_pWayFind = pWayFind;
	m_pDm = new dmsoft();
}
CGetCoordinates::~CGetCoordinates()
{
	if (m_pDm) {
		delete m_pDm;
		m_pDm = nullptr;
	}
}
void CGetCoordinates::MatchGetCoordinate(CPoint& pGetPoint, CPoint write, bool IsWrite)
{
	g_pEngine->m_ClockGetCoordinates.Lock();
	if (IsWrite) {
		m_Current = write;
	}
	else {
		pGetPoint = m_Current;
	}
	g_pEngine->m_ClockGetCoordinates.UnLock();
}

int CGetCoordinates::ThreadState(int write, bool IsWrite)
{
	//g_pEngine->m_ClockGetCoordinates_State.Lock();
	if (IsWrite)
	{
		m_IThreadState = write;
	}
	else {
		return m_IThreadState;
	}
	//g_pEngine->m_ClockGetCoordinates_State.UnLock();
}

void CGetCoordinates::Test()
{
	LogD(_T("CGettingCoordinates"));
	LogD(_T("CGettingCoordinates"));



}

CPoint CGetCoordinates::GetCVPoint()
{
	cv::Mat 当前区域 = m_pWayFind->m_pTool->cv截图转换图像(m_pDm,m_pWayFind->CaptureMiniMapRect.left, m_pWayFind->CaptureMiniMapRect.top, m_pWayFind->CaptureMiniMapRect.right, m_pWayFind->CaptureMiniMapRect.bottom);

	//cv::imshow("显示图像", 当前区域);
	//cv::waitKey();
	//ShowMat(当前区域);

	CPoint 坐标 = CPoint(0, 0);
	m_pWayFind->m_pTool->cv模板匹配(m_pWayFind->CurrentBigMap, 当前区域, 坐标, 0.5);

	return 坐标;

}

bool CGetCoordinates::OnLoop()
{

	return false;
}
bool CGetCoordinates::OnEventThreadRun()
{
	while (true)
	{
		CPoint tmp(NULL, NULL);
		MatchGetCoordinate(tmp, GetCVPoint(), true);

		int IThreadState;
		IThreadState = ThreadState();
		if (IThreadState == ThreadState::STATE_Pause)
		{
			LogD(_T("CGetCoordinates STATE_Pause"));
			while (true) {
				IThreadState = ThreadState();
				if (IThreadState == ThreadState::STATE_Run)
					break;

				if (IThreadState == ThreadState::STATE_Stop)
					break;
				Sleep(10);
				//m_pWayFind->m_pTool->SleepEx(10);
			}
			LogD(_T("CGetCoordinates STATE_Run"));
		}

		Sleep(10);
		//m_pWayFind->m_pTool->SleepEx(10);
		if (IThreadState == ThreadState::STATE_Stop) {
			LogD(_T("CGetCoordinates STATE_Stop"));
			IsConclude = true;
			break;
		}
	}

	return true;







	//while (true)
	//{
	//	CPoint tmp(NULL, NULL);
	//	MatchGetCoordinate(tmp, GetCVPoint(), true);

	//	int IThreadState;
	//	IThreadState = ThreadState(4, false);
	//	if (IThreadState == ThreadState::STATE_Pause)
	//	{
	//		LogD(_T("CGetCoordinates STATE_Pause"));
	//		while (true) {
	//			IThreadState = ThreadState(4,false);
	//			if (IThreadState == ThreadState::STATE_Run)
	//				break;
	//			
	//			if (IThreadState == ThreadState::STATE_Stop) {
	//				LogD(_T("CGetCoordinates STATE_Stop"));
	//				//AfxEndThread(0);//强制结束线程
	//				IsConclude = true;
	//				return false;
	//			}
	//			Sleep(10);
	//			//m_pWayFind->m_pTool->SleepEx(10);
	//		}
	//		LogD(_T("CGetCoordinates STATE_Run"));
	//	}

	//	Sleep(10);
	//	//m_pWayFind->m_pTool->SleepEx(10);
	//	if (IThreadState == ThreadState::STATE_Stop){
	//		LogD(_T("CGetCoordinates STATE_Stop"));
	//		//AfxEndThread(0);//强制结束线程
	//		IsConclude = true;
	//		return false;
	//	}
	//}

	//return true;
}


bool CGetCoordinates::OnEventThreadStart()
{
	return true;
}
bool CGetCoordinates::OnEventThreadConclude()
{
	if (IsConclude) {
		return false;
	}

	return true;
}