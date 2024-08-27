#include "pch.h"
#include"CTaskScript.h"
#include "CWayFind.h"
#include "CDCMEngine.h"
#include"CKeyMouse.h"
#include"CGettingCoordinates.h"
extern CDCMEngine* g_pEngine;
#define OC




void CWayFind::Init(CTaskScript* pTaskScript, tagGameMapInfo* pGameMapInfo,CString strFilePath, CString strSectionName)
{
	m_pTool = pTaskScript;
	m_pGameMapInfo = pGameMapInfo;
	CurrentBigMap = m_pTool->cv����ͼ��(CW2CA(g_pEngine->m_strWorkPath + _T("\\res\\map\\")+ m_pGameMapInfo->mapBmpName).GetString(), cv::IMREAD_GRAYSCALE, 1);
	CurrentBigMapInfo = m_pTool->cv����ͼ��(CW2CA(g_pEngine->m_strWorkPath + _T("\\res\\map\\") + m_pGameMapInfo->astarBmpName).GetString(), cv::IMREAD_UNCHANGED, 1);
	CaptureMiniMapRect = CRect(1153 - 90, 187 - 90, 1153 + 90, 187 + 90);
	m_Astar.ReadMap(g_pEngine->m_strWorkPath + _T("\\res\\map\\") + m_pGameMapInfo->astarBmpName);
	//��ʼ����ȡ�����߳�
	m_pGetCoordinates = new CGetCoordinates(this);
	m_pGetCoordinates->StartThread();
	m_pTool->SleepEx(50);

}

CWayFind::~CWayFind()
{
	//�����ȡ�����̣߳��Ȱ����⿪
	int IState = GetCoordinatesState::STATE_Stop;
	m_pGetCoordinates->ThreadState(IState, true);
	//m_pTool->SleepEx(50); //��������ʱ��
	if (m_pGetCoordinates->StopThread(3000)) {
		LogE(_T("��ֹ�߳�Coordinates�ɹ�"));
		delete m_pGetCoordinates;
		m_pGetCoordinates = nullptr;
	}
	else
	{
		LogE(_T("������ֹ�߳�Coordinatesʧ��"));
	}
	
	m_Astar.DestoryMap();
	delete m_pGetCoordinates;
	m_pGetCoordinates = nullptr;
}

void CWayFind::Tset1()
{
	Tset();
	m_pTool->SleepEx(50);
	int time = NULL;
	int IState;
	int tmpNull;
	while (true)
	{
		//if (time >= 300)
		//	break;
		CPoint pointret =  GetGamePoint();
		LogD(_T("%dCWayFind��%d|%d"), time, pointret.x, pointret.y);


		time++;
		IState = GetCoordinatesState::STATE_Run;
		m_pGetCoordinates->ThreadState( IState, true);
		CPoint pGetPoint;
		m_pGetCoordinates->MatchGetCoordinate(pGetPoint);
		LogD(_T("%d��ǰ���꣺%d|%d"), time, pGetPoint.x, pGetPoint.y);

		IState = GetCoordinatesState::STATE_Pause;
		m_pGetCoordinates->ThreadState(IState, true);
		m_pTool->SleepEx(50);
	}

	bool b=  m_pGetCoordinates->StopThread(5000);

	b = 10;
}


void CWayFind:: Tset()
{
	WayFindStart(CPoint(725, 425));
	m_pTool->SleepEx(50);

}

CPoint CWayFind::GetGamePoint(double dSim)
{
	cv::Mat ��ǰ���� = m_pTool->cv��ͼת��ͼ��(CaptureMiniMapRect.left, CaptureMiniMapRect.top, CaptureMiniMapRect.right, CaptureMiniMapRect.bottom);

	//cv::imshow("��ʾͼ��", ��ǰ����);
	//cv::waitKey();
	//ShowMat(��ǰ����);

	CPoint ���� = CPoint(0, 0);
	m_pTool->cvģ��ƥ��(CurrentBigMap, ��ǰ����, ����, dSim);

	return ����;
}

void CWayFind::PrintGamePoint()
{
	while (true){
		CPoint ���� = GetGamePoint();
		LogD(_T("��ǰ����:%d,%d"), ����.x, ����.y);
		Sleep(1000);
	}
}

void CWayFind::�ƶ�������_ŵ�Ƿ���(CPoint& Ŀ������)
{
	CPoint �������� = CPoint(631, 314);
	m_pTool->MoveTo(��������.x, ��������.y);
	m_pTool->RightClick();

	while (true)
	{
		CPoint ��ǰ���� = GetGamePoint();

		//С��ֱ�߾��뼴��Ϊ����Ŀ�ĵ�
		if (m_pTool->GetPtDis(Ŀ������, ��ǰ����) < 6)
			break;

		CPoint Ŀ��� = m_pTool->б�ʹ�ʽ(��ǰ����, Ŀ������, CPoint(645, 426), 225);
		
		m_pTool->MoveTo(Ŀ���.x, Ŀ���.y);
		m_pTool->RightClick();

		LogD(_T("��ǰ����:%d,%d Ѱ·�������:%d,%d"), ��ǰ����.x, ��ǰ����.y, Ŀ���.x, Ŀ���.y);
		Sleep(1);
	}
}

CPoint CWayFind::threadGetCoordinate()
{
	CPoint pGetPoint(NULL, NULL);
	m_pGetCoordinates->MatchGetCoordinate(pGetPoint);
	return pGetPoint;
}

int CWayFind::WayFindStart(CPoint terminal)
{
	int IState = GetCoordinatesState::STATE_Run;
	m_pGetCoordinates->ThreadState(IState, true);
	m_pTool->SleepEx(500);
	int retWayFind = NULL;
	while (true)
	{
		retWayFind = WayFind(terminal);
		if (retWayFind == STATE_Running_stuck_point) {
			continue;
		}

		break;
	}
;
	IState = GetCoordinatesState::STATE_Pause;
	m_pGetCoordinates->ThreadState(IState, true);
	m_pTool->SleepEx(100);
	return retWayFind;
}

int CWayFind::WayFind(CPoint terminal)
{

	CPoint ptMe = GetGamePoint();
	std::vector<CPoint> arrPts;
	m_Astar.FindPath(ptMe, terminal, arrPts);

	if (!arrPts.size())
		return WayFindState::STATE_CDT_Useless;

	if (CardPointWayFind(ptMe, terminal)){
		return WayFindState::STATE_Running_stuck_point;
	}

	if (arrPts.size()<=5)
	{
		LogD(_T("���ڸ�������Ѱ·"));
		return WayFindState::STATE_Done;
	}

	int detachmentStuckTime = NULL;
	for (int i = 0; i < arrPts.size(); i += 5)
	{
		CPoint arrWayCoordinates = arrPts[i];

		int tryConut = 0;
		while (tryConut++ < 15)
		{
			CPoint CurrentCoordinate = threadGetCoordinate();
			if (CardPointWayFind(CurrentCoordinate, terminal))
				return WayFindState::STATE_Running_stuck_point;

			if (m_pTool->GetPtDis(arrWayCoordinates, CurrentCoordinate) < 6)
			{
				//LogD(_T("�ƶ����:%d,%d"), arrPts[i].x, arrPts[i].y);
				break;
			}
			else
			{
				CPoint Click = m_pTool->б�ʹ�ʽ(CurrentCoordinate, arrWayCoordinates, CPoint(645, 426), 225);
				m_pTool->DRClick(Click);
				m_pTool->SleepEx(m_pTool->GetRandom(30, 50));
			}

			if (m_pTool->GetPtDis(arrWayCoordinates, CurrentCoordinate) > 15) {
				LogD(_T("�����Զ,�������"));
				return WayFindState::STATE_CDT_Useless;
			}

			m_pTool->SleepEx(80);
		}

		if (tryConut >= 10)
		{
			LogE(_T("Ѱ·��ʱ"));
			if (detachmentStuck(2)){
				detachmentStuckTime++;
				LogD(_T("У��·�߳ɹ�"));

			}
			else {
				return WayFindState::STATE_Movement_timeout;
				LogD(_T("!!У��·��ʧ��"));
				detachmentStuckTime++;
			}
			
		}

		if(detachmentStuckTime >= 2)
			return WayFindState::STATE_Movement_timeout;

	}
	
	LogD(_T("�ƶ����"));
	return WayFindState::STATE_Done;
}

bool CWayFind::isThrough(CPoint begin, CPoint terminal)
{
	std::vector<CPoint> arrPts;
	m_Astar.FindPath(begin, terminal, arrPts);
	return arrPts.size();
}

bool  CWayFind::waitMovStop(int IwaitTime)
{
	int  CurrentTime = m_pTool->GetTimeSeconds();
	while (true){
		CPoint tagCurrentPoint = GetGamePoint();
		m_pTool->SleepEx(500);
		CPoint CurrentPoint = GetGamePoint();
		if (tagCurrentPoint.x == CurrentPoint.x && tagCurrentPoint.y == CurrentPoint.y){
			LogD(_T("ͣ��"));
			return true;
		}
	
		if (m_pTool->GetTimeSeconds() - CurrentTime >= IwaitTime)
		{
			LogD(_T("waitMovStop��ʱ"));
			return false;
		}
	}
}

bool CWayFind::detachmentStuck(int CheckRect)
{
constexpr auto Max = 10;
constexpr auto time = 3;

	for (size_t i = 0; i < time; i++){

		bool isMov = false;
		CPoint tagUsefulCoordinate = m_pGameMapInfo->tagUsefulCoordinate;
		CPoint MovPint = CPoint(NULL, NULL), tagCurrentPoint = CPoint(NULL, NULL), CurrentPoint = GetGamePoint(0.3);
		if (CurrentPoint.x == NULL && CurrentPoint.y == NULL) {
			return false;
		}

		if (CheckRect) {//��Χ�Ŵ�
			CRect tagRect = CRect(CurrentPoint.x - CheckRect, CurrentPoint.y - CheckRect, CurrentPoint.x + CheckRect, CurrentPoint.y + CheckRect);
			bool IsSuccess = false;
			if (isThrough(tagRect.left, tagUsefulCoordinate))
				IsSuccess = true;
			if (isThrough(tagRect.top, tagUsefulCoordinate))
				IsSuccess = true;
			if (isThrough(tagRect.right, tagUsefulCoordinate))
				IsSuccess = true;
			if (isThrough(tagRect.bottom, tagUsefulCoordinate))
				IsSuccess = true;

			if (IsSuccess) 
				return true;
		}
		else{
			if (isThrough(CurrentPoint, tagUsefulCoordinate))
				return true;
		}

		for (size_t i = 0; i < Max; i++) {
			if (CheckRect) {
				CheckRect = i + CheckRect;//�ȷŴ�һ����Χ ����1-3
			}
			else{
				CheckRect = i;
			}

			if (isThrough(CPoint(CurrentPoint.x, CurrentPoint.y - CheckRect), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x, CurrentPoint.y - CheckRect);
			}
			else if (isThrough(CPoint(CurrentPoint.x, CurrentPoint.y + CheckRect), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x, CurrentPoint.y + CheckRect);
			}
			else if (isThrough(CPoint(CurrentPoint.x - CheckRect, CurrentPoint.y), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x - CheckRect, CurrentPoint.y);
			}
			else if (isThrough(CPoint(CurrentPoint.x + CheckRect, CurrentPoint.y), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x + CheckRect, CurrentPoint.y);
			}
			else if (isThrough(CPoint(CurrentPoint.x - CheckRect, CurrentPoint.y - int(CheckRect / 2)), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x - CheckRect, CurrentPoint.y - int(CheckRect / 2));
			}
			else if (isThrough(CPoint(CurrentPoint.x - CheckRect, CurrentPoint.y + int(CheckRect / 2)), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x - CheckRect, CurrentPoint.y + int(CheckRect / 2));
			}
			else if (isThrough(CPoint(CurrentPoint.x + i, CurrentPoint.y - int(CheckRect / 2)), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x + CheckRect, CurrentPoint.y - int(CheckRect / 2));
			}
			else if (isThrough(CPoint(CurrentPoint.x + CheckRect, CurrentPoint.y + int(CheckRect / 2)), tagUsefulCoordinate)) {
				MovPint = CPoint(CurrentPoint.x + CheckRect, CurrentPoint.y + int(CheckRect / 2));
			}
			else {
				continue;
			}

			isMov = true;
			break;
		}

		if (!isMov)
			return isMov;

		LogD(_T("ƫ�����꣺%d .%d"), MovPint.x, MovPint.y);
		CurrentPoint = GetGamePoint();
		CPoint RClick = m_pTool->б�ʹ�ʽ(CurrentPoint, MovPint, CPoint(645, 426), 225);
		for (size_t i = 0; i < time; i++)
		{
			m_pTool->RClick(RClick);
			waitMovStop(2000);
			CurrentPoint = GetGamePoint();
			if (isThrough(CurrentPoint, tagUsefulCoordinate))
				return true;
		}
	}

	return false;
}


int CWayFind::GetRandom(int iMin, int iMax)
{
	return rand() % (iMax - iMin) + iMin;
}

void  CWayFind::¼�����겢�ƶ�_ŵ�Ƿ���(int ��¼ʱ��)
{
	LogD(_T("��ʼ¼��!"));
	Sleep(5000);

	int ¼��ʱ�� = 0;
	std::vector<CPoint> arrPts;
	while (¼��ʱ�� < ��¼ʱ�� * 1000)
	{
		CPoint ���� = GetGamePoint();
		if (����.x != 0)
		{
			LogD(_T("¼������:%d,%d"), ����.x, ����.y);
			arrPts.push_back(����);
		}

		Sleep(200);

		¼��ʱ�� += 200;
	}

	LogD(_T("��ʼѰ·!"));

	Sleep(5000);

	for (int i = arrPts.size() - 1; i >= 0; i--)
	{
		CPoint Ѱ·���� = arrPts[i];

		int tryConut = 0;
		while (tryConut++ < 50)
		{
			CPoint ��ǰ���� = GetGamePoint();

			if (m_pTool->GetPtDis(Ѱ·����, ��ǰ����) < 6)
			{
				LogD(_T("�ƶ����"));
				break;
			}
			else
			{
				CPoint �ƶ���� = m_pTool->б�ʹ�ʽ(��ǰ����, Ѱ·����, CPoint(631, 314), 180);

				m_pTool->MoveTo(�ƶ����.x, �ƶ����.y);
				m_pTool->RightClick();
			}

			LogD(_T("��ǰ����:%d,%d ���Դ���:%d"), ��ǰ����.x, ��ǰ����.y, tryConut);
			Sleep(1);
		}

		if (tryConut >= 50)
		{
			LogE(_T("Ѱ·ʧ��"));
			break;
		}
	}

	LogD(_T("ȫ���ƶ����"));
}


int CWayFind::computation_distance_Sort(CPoint CurrentCoordinate,std::vector<CPoint>& wayCoordinate)
{
	LogD(_T("���ڼ����������꣬��һ�ν��������Եȣ�"));
	int Index = -1 , distance = 1000000,i = -1;
	CPoint tmpCoordinate{ NULL,NULL };
	for (auto it = wayCoordinate.cbegin(); it != wayCoordinate.cend(); it++){
		i++;
		std::vector<CPoint> arrPts;
		m_Astar.FindPath(CurrentCoordinate, CPoint(it->x,it->y), arrPts);
		if (arrPts.size() < distance){
			Index = i;
			distance = arrPts.size();
		}
	}

	if (Index == -1)
		return -1;

#ifdef OC
	//__________________����ڵ�
	CPoint TmpCoordinate{ NULL,NULL };
	std::vector<CPoint> TmpWayCoordinate{};
	TmpWayCoordinate.push_back(CPoint(wayCoordinate[Index].x, wayCoordinate[Index].y));
	for (auto it = wayCoordinate.cbegin(); it != wayCoordinate.cend(); it++)
	{
		int FinallyIndex = TmpWayCoordinate.size() - 1, tagdistance = 1000000, m = -1;
		for (auto i = wayCoordinate.cbegin(); i != wayCoordinate.cend(); i++)
		{
			m++;
			if (!i->x)
				continue;
			std::vector<CPoint> arrPts;
			m_Astar.FindPath(CPoint(TmpWayCoordinate[FinallyIndex].x, TmpWayCoordinate[FinallyIndex].y), CPoint(i->x, i->y), arrPts);
			if (arrPts.size() < tagdistance) {
				tagdistance = arrPts.size();
				Index = m;
				TmpCoordinate = CPoint(i->x, i->y);
			}
		}

		if (Index != -1) {
			TmpWayCoordinate.push_back(TmpCoordinate);
			wayCoordinate[Index].x = NULL;
		}
	}

	//ɾ����ǵ����
	TmpWayCoordinate.erase(TmpWayCoordinate.begin()++);
	swap(TmpWayCoordinate, wayCoordinate);

	CString str{ _T("") };
	for (auto it = wayCoordinate.cbegin(); it != wayCoordinate.cend(); it++)
	{
		str += m_pTool->ITostr(it->x) + _T(",") + m_pTool->ITostr(it->y) + _T("|");
	}

	LogD(_T("�������:%d,%d"), wayCoordinate[0].x, wayCoordinate[0].y);
	return NULL; 
#endif

	LogD(_T("��������������size:%d"), wayCoordinate.size());
	return Index; 
}


void CWayFind::LoadCardPointInfo(CString strFilePath, CString strSectionName)
{
	std::map<CString, CString> mapIniKeyValue = {};
	if (!m_pTool->iniGetAllKeyValueAllNodeNames(strFilePath, strSectionName, mapIniKeyValue))
		return;



	for (auto it = mapIniKeyValue.cbegin(); it != mapIniKeyValue.cend(); it++)
	{
		CardPointCoordinatesInfo tmpCardPointInfo = CardPointCoordinatesInfo();
		CString strInfo = it->second;

		//��������
		CStringArray  arrStr;
		Split(strInfo, arrStr, _T("("));
		CStringArray arrCardPoint;
		Split(arrStr[0], arrCardPoint, _T("&"));
		for (size_t i = 0; i < arrCardPoint.GetCount(); i++){
			if (arrCardPoint[i] == _T(""))
				continue;

			CPoint tmpCpprdinate = m_pTool->strToPoint(arrCardPoint[i]);
			tmpCardPointInfo.vpCardPointCoordinates.push_back(tmpCpprdinate);
		}

		//�뾶ֵ
		CStringArray arrRadiusRectangle;
		arrStr.RemoveAll();
		Split(strInfo, arrStr, _T("("));
		Split(arrStr[1], arrRadiusRectangle, _T(")"));
		if (arrRadiusRectangle.GetCount() == 1);
		tmpCardPointInfo.iRadiusRectangle = _ttoi(arrRadiusRectangle[0]);

		//��������
		CStringArray arrType;
		arrStr.RemoveAll();
		Split(strInfo, arrStr, _T("{"));
		Split(arrStr[1], arrType, _T("}"));
		if (arrType.GetCount() == 2)
			tmpCardPointInfo.iType = _ttoi(arrType[0]);

		//����Ŀ�ĵ����� 
		CStringArray arrpEndCoordinates;
		arrStr.RemoveAll();
		Split(strInfo, arrStr, _T("-"));
		Split(arrStr[1], arrpEndCoordinates, _T("<"));
		if (arrpEndCoordinates.GetCount() == 2) {
			//CPoint tmpPoint = m_pTool->strToPoint(arrpEndCoordinates[0]);
			//if (tmpPoint.x == NULL && tmpPoint.y == NULL) {//�������Ŀ������Ϊ0��������Ѱ·Ŀ��
			//	CStringArray tmpName;
			//	Split(strSectionName, tmpName, _T("_"));
			//	std::map<CString, CString> mapIniKeyValue{};
			//	if (!m_pTool->iniGetAllKeyValueAllNodeNames(strFilePath, _T("�ɼ�_") + tmpName[0] + _T("_coordinates"), mapIniKeyValue))
			//		continue;
			//	for (auto it = mapIniKeyValue.cbegin(); it != mapIniKeyValue.cend(); it++) {
			//		CStringArray arrContent;
			//		Split(it->second, arrContent, _T("|"));
			//		for (size_t i = 0; i < arrContent.GetCount(); i++)
			//			m_pGameMapInfo->wayFindCoordinate.emplace_back(m_pTool->strToPoint(arrContent[i]));
			//	}
			//}
			//else{
			//	tmpCardPointInfo.pEndCoordinates = m_pTool->strToPoint(arrpEndCoordinates[0]);
			//}

			tmpCardPointInfo.pEndCoordinates = m_pTool->strToPoint(arrpEndCoordinates[0]);

		}

		//¼������
		CStringArray arrpRecordCoordinates;
		arrStr.RemoveAll();
		Split(strInfo, arrStr, _T("<"));
		Split(arrStr[1], arrpRecordCoordinates, _T("|"));
		for (size_t j = 0; j < arrpRecordCoordinates.GetCount(); j++)
		{
			CPoint ttmpPoint = m_pTool->strToPoint(arrpRecordCoordinates[j]);
			tmpCardPointInfo.vpRecordCoordinates.push_back(m_pTool->strToPoint(arrpRecordCoordinates[j]));
		}

		m_pGameMapInfo->vCardPointCoordinatesInfo.push_back(tmpCardPointInfo);
	}
}

bool CWayFind::CardPointWayFind(CPoint pCurrentCoordinate, CPoint pEndPoint)
{
	for (auto i = 0; i < m_pGameMapInfo->vCardPointCoordinatesInfo.size(); i++)
	{
		bool IsWaFind = false;
		int iRadiusRectangle = m_pGameMapInfo->vCardPointCoordinatesInfo[i].iRadiusRectangle;
		CPoint pEndCoordinates = m_pGameMapInfo->vCardPointCoordinatesInfo[i].pEndCoordinates;
		int iType = m_pGameMapInfo->vCardPointCoordinatesInfo[i].iType;
		std::vector<CPoint> vpRecordCoordinates = m_pGameMapInfo->vCardPointCoordinatesInfo[i].vpRecordCoordinates;

		for (auto j = 0; j < m_pGameMapInfo->vCardPointCoordinatesInfo[i].vpCardPointCoordinates.size(); j++)
		{
			CPoint tmpPoint = m_pGameMapInfo->vCardPointCoordinatesInfo[i].vpCardPointCoordinates[j];

			if (tmpPoint.x == NULL && tmpPoint.y == NULL)
				continue;

			//LogD(_T("pCurrentCoordinate%d,%d"), pCurrentCoordinate.x, pCurrentCoordinate.y);
			//CRect tmpRect = CRect(,, , );
			if (pCurrentCoordinate.x >= tmpPoint.x - iRadiusRectangle && pCurrentCoordinate.y >= tmpPoint.y - iRadiusRectangle && pCurrentCoordinate.x <= tmpPoint.x + iRadiusRectangle && pCurrentCoordinate.y <= tmpPoint.y + iRadiusRectangle) {
				//�������Ŀ������Ϊ0����û��Ŀ��
				if (pEndCoordinates.x == NULL && pEndCoordinates.y == NULL) {
					IsWaFind = true;
					break;
				}
				
				if (pEndCoordinates.x == pEndPoint.x && pEndCoordinates.y == pEndPoint.y) {
					IsWaFind = true;
					break;
				}
			}
		}

		if (!IsWaFind)
			continue;
		
		LogD(_T("CardPointWayFind"));

		if (iType == 2) {
			m_pTool->KeyPress(VK_G);
			return true;
		}
		if (vpRecordCoordinates.size() == 1) { //����Ҫ̫����Ѱ·��ֻ��Ҫ����ƶ�һ�� !!!ע�����������Ļ���겻�ǵ�ͼ����
			m_pTool->DRClick(CPoint(vpRecordCoordinates[0].x, vpRecordCoordinates[0].y));
			m_pTool->SleepEx(m_pTool->GetRandom(800, 1000));
			return true;
		}

		for (size_t k = 0; k < vpRecordCoordinates.size(); k++)
		{
			CPoint pPathCoordinates = vpRecordCoordinates[k];
			while (true)
			{
				CPoint tmpCurrentCoordinate = threadGetCoordinate();
				if (tmpCurrentCoordinate.x != NULL) {
					double distance = m_pTool->GetPtDis(pPathCoordinates, tmpCurrentCoordinate);
					if (distance > 15) {
						LogD(_T("����Ѱ·�����쳣"));
						return true;
					}

					if (distance <= 5) {
						break;
					}

					CPoint Click = m_pTool->б�ʹ�ʽ(tmpCurrentCoordinate, pPathCoordinates, CPoint(645, 426), 225);
					m_pTool->DRClick(Click);
					m_pTool->SleepEx(m_pTool->GetRandom(30, 50));
				}
				else
				{
					detachmentStuck(2);
				}

				m_pTool->SleepEx(m_pTool->GetRandom(80, 100));
			}
		}

		return true;
	}

	return false;
}


