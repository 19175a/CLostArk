#pragma once
#include"CTaskScript.h"
#include"CAstar.h"
class CTaskScript;
class CGetCoordinates;
class CWayFind
{
public:
	~CWayFind();

	enum GetCoordinatesState {
		STATE_Stop = -1,
		STATE_Pause = 0,
		STATE_Run = 1,
	};
	enum WayFindState {
		STATE_CDT_Useless = -1,
		STATE_Done = 1,
		STATE_Running_stuck_point = 2,//卡点寻路
		STATE_Movement_timeout = 3,
	};

	void Tset();
	void Tset1();
	void Init(CTaskScript* pTaskScript, tagGameMapInfo* pGameMapInfo, CString strFilePath = _T(""), CString strSectionName = _T(""));
	CTaskScript* m_pTool;
	CGetCoordinates* m_pGetCoordinates;

	//WayFindInfo
	cv::Mat CurrentBigMap;
	cv::Mat CurrentBigMapInfo;
	CRect CaptureMiniMapRect;
	//TaskScriptGameMapInfo																															
	tagGameMapInfo* m_pGameMapInfo;


	void PrintGamePoint();
	CPoint GetGamePoint(double dSim = 0.5);
	bool  waitMovStop(int IwaitTime);
	void LoadCardPointInfo(CString strFilePath, CString strSectionName);		//获取卡点数据
	bool CardPointWayFind(CPoint pCurrentCoordinate, CPoint pEndPoint);
	bool detachmentStuck(int CheckRect = NULL);														//偏移坐标 脱离卡点
	bool isThrough(CPoint begin, CPoint terminal);

	void 移动到坐标_诺亚方舟(CPoint& 目的坐标);
	void 录制坐标并移动_诺亚方舟(int 记录时间);

	CPoint threadGetCoordinate();		//使用前请确保线程已启动并且状态为运行
	int  WayFindStart(CPoint terminal);
	int  WayFind(CPoint terminal);
	int GetRandom(int iMin, int iMax);

	int computation_distance_Sort(CPoint CurrentCoordinate, std::vector<CPoint>& wayCoordinate);

	CAstar	m_Astar;
};

