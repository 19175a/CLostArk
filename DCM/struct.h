#pragma once

#ifdef OPENCV_NEED
#include <opencv2/opencv.hpp>
#endif

#include"pch.h"
#include<vector>
struct  tagGameAcconutInfo
{
	int		Id;				//窗口ID
	CString strAcconut;		//账号
	CString strPassword;	//密码
	CString strRegion;		//大区
	bool	iOrNot;			//是否完成今日任务

	tagGameAcconutInfo()
	{
		Id = NULL;
		strAcconut = _T("");
		strPassword = _T("");
		strRegion = _T("");
		iOrNot = false;
	}
};

struct tagTaskInfo;
struct tagWndInfo {
	int		id;
	HWND	hWnd;
	CRect	rtWnd;
	CString strTitle;
	bool IsWriteIniLogd;  //如果脚本开启时 游戏存在则不写入本地LogDini时间(因为不确定当前运行的是哪个账号)
	tagTaskInfo* pTaskInfo;

	tagWndInfo()
	{
		id = 0;
		hWnd = NULL;
		rtWnd = CRect(0, 0, 0, 0);
		strTitle = _T("");
		pTaskInfo = NULL;
		IsWriteIniLogd = false;
	}
};

struct tagGroupUser {
	int teamId; //组员
	int leaderId;//组长

	CString strName;

	CString strTeamIp;
	CString strTeamPort;
	CString strTeamUrl;

	tagGroupUser()
	{
		teamId = 0;
		leaderId = 0;
		strName = _T("");

		strTeamIp = _T("");
		strTeamPort = _T("");
		strTeamUrl = _T("");
	}
};

class CTaskThread;
class CHttpSrvThread;
struct tagGameStatus;
struct tagTeam;
class CTaskHttpSrvThread;
class CTaskTimer;
struct  tagOnlieTeamUsersPeopleInfo;

struct tagTaskInfo {
	int		id;
	tagWndInfo* pWnd;
	CTaskThread* pTaskThread;			//主任务线程
	CTaskThread* pTaskHelperThread;		//辅助任务线程
	CString strState;					//执行 暂停 停止
	tagGameStatus* pStatus;				//UI交互
	tagGameAcconutInfo* pAccountInfo;	//账号信息
	bool bDrawWnd;						//绘制窗口标记#防止辅助线程也开启绘制
	CArray<tagOnlieTeamUsersPeopleInfo*>	arrOnlieTeamUsersPeopleInfo;//在线组员人物信息
	CTaskHttpSrvThread* pTaskHttpSrv;
	CString strHttpCmd;
	CString strHttpContent;
	CLock	m_LockHttpCmd;
	void HttpCmd(bool SetOrRecover, CString strCmd = _T(""), CString strContent = _T(""))
	{
		//防止两个线程 同时修改
		m_LockHttpCmd.Lock();
		if (SetOrRecover)
		{
			strHttpCmd = strCmd;
			strHttpContent = strContent;
		}
		else
		{
			strHttpCmd = _T("");
			strHttpContent = _T("");
		}
		m_LockHttpCmd.UnLock();
	}
	void GetCmd(CString& strCmd, CString& strContent)
	{
		strCmd = strHttpCmd;
		strContent = strHttpContent;
	}
	//任务计时器
	CTaskTimer* m_pTskTimer;
	CLock	m_LockTskTimer;
	int		iTieme;
	void SetTimer(int _ITime)
	{
		m_LockTskTimer.Lock();
		iTieme = _ITime;
		m_LockTskTimer.UnLock();
	}
	void TiemeDecrease()
	{
		m_LockTskTimer.Lock();
		iTieme -= 1000;
		m_LockTskTimer.UnLock();
	}
	void TimeAugment()
	{
		m_LockTskTimer.Lock();
		iTieme += 1000;
		m_LockTskTimer.UnLock();
	}
	int GetTiem() { return iTieme; }

	//队伍信息 #任务逻辑中是通过Http通讯的，后期如果需要做跨局域网通讯，想办法初始化更换里面的ip地址就行
	tagTeam* pTeam;
	tagTaskInfo()
	{
		id = 0;
		pWnd = NULL;
		pTaskThread = NULL;
		pTaskHelperThread = NULL;
		pTaskHttpSrv = NULL;
		m_pTskTimer = NULL;
		strState = _T("");
		iTieme = NULL;
		bDrawWnd = false;
		strHttpCmd = _T("");
		strHttpContent = _T("");
		arrOnlieTeamUsersPeopleInfo.RemoveAll();
	}
};

class CLDScan;
struct tagScanGame {

	CLDScan* pLDScan;
	CString strCommand;
	CString strData;

	tagScanGame()
	{
		pLDScan = NULL;
		strCommand = _T("");
		strData = _T("");
	}
};


struct tagWndIni {
	CString strProc;
	CString strTitle;
	CString strClz;
	CString strLDPath;

	CString strDisplay;
	CString strMouse;
	CString strKeypad;
	CString	strPublic;
	int		iMode;

	CStringArray arrSwitch;

	WORD	本地端口;
	CString	组队配置;

	CString	任务列表;

	CString	账号配置;
	CString 雷电共享目录;


	tagWndIni()
	{
		strProc = _T("");
		strTitle = _T("");
		strClz = _T("");
		strLDPath = _T("");

		strDisplay = _T("");
		strMouse = _T("");
		strKeypad = _T("");
		strPublic = _T("");
		iMode = 0;

		本地端口 = 0;
		组队配置 = _T("");

		任务列表 = _T("");
		账号配置 = _T("");
		雷电共享目录 = _T("雷电共享目录");

	}
};

struct tagResItem {
	CString strName;

	int iType; //0 找图 1 找色 2 找字 3 字典 4 识字 5 位置 6 opencv找图 7 Ocr

	CRect rtArea;
	CString strRes; //图片名 颜色 或者字

	CString strDeltaClr;
	double dSim;
	int iDir;

#ifdef OPENCV_NEED

	cv::Mat matPic;

#endif
	tagResItem()
	{
		strName = _T("");
		iType = 0;
		rtArea = CRect(0, 0, 0, 0);
		strRes = _T("");
		strDeltaClr = _T("");
		dSim = 0;
		iDir = 0;
	}
};

struct tagGameStatus {
	CString strAccount;
	CString strPw;

	DWORD dwRunTick;

	CString strTime;

	int lv;
	CString strName;
	CString state;
	tagGameStatus()
	{
		lv = 0;
		strName = _T("");
		state = _T("");
		dwRunTick = 0;
		strTime = _T("");
	}
};

struct tagTaskCmd {
	CString strName;
	CString strParam;
	int iCount;

	tagTaskCmd()
	{
		strName = _T("");
		strParam = _T("");
		iCount = 0;
	}
};

//标记窗口标题
struct tagWndSubscript {
	CString tagWndHwnd;
	CString tagTitleSubscript;

	tagWndSubscript()
	{
		tagWndHwnd = _T("");
		tagTitleSubscript = _T("");
	}
};



//队伍组长
struct tagTeamleader
{
	int leaderId;//组长
	CString strTeamIp;
	CString strTeamPort;
	CString strTeamUrl;
	bool IsleadeOnlie;
	tagTeamleader()
	{
		IsleadeOnlie = false;
		leaderId = NULL;
		strTeamIp = _T("");
		strTeamPort = _T("");
		strTeamUrl = _T("");
	}
};

struct OcrRes
{
	CString strResult;	//返回内容
	int iX;
	int iY;
};

struct OcrUpgrade
{
	CString strResult = _T("");	//返回内容
	int iX = NULL;
	int iY = NULL;
	bool IsDone = false;

	OcrUpgrade(CString _strResult, int _iX, int _iY, bool _IsDone) {
		strResult = _strResult;
		iX = _iX;
		iY = _iY;
		IsDone = _IsDone;
	}

};

struct CardPointCoordinatesInfo
{
	enum Type {
		WaitToPass = 1,				//1等待通过
		PressGPass =2,				//2按G通过
		CardPointCoordinates =3		//3坐标卡点
	};

	std::vector<CPoint> vpCardPointCoordinates = {};	//卡点坐标
	std::vector<CPoint> vpRecordCoordinates = {};		//录制坐标
	int iRadiusRectangle = NULL;						//矩形半径
	int iType = NULL;									//卡点类型
	CPoint pEndCoordinates = CPoint(NULL, NULL);		//卡点目的地坐标

	CardPointCoordinatesInfo() {
		vpCardPointCoordinates = {};
		vpRecordCoordinates = {};
		iRadiusRectangle = NULL;
		iType = NULL;
		pEndCoordinates = CPoint(NULL, NULL);
	}
};


struct tagGameMapInfo
{
	CString mapBmpName = _T("");
	CString	astarBmpName = _T("");
	CPoint RepairmanCoordinates = CPoint(NULL, NULL);
	CString TeleportName = _T("");
	CPoint ExceptionReturnCoordinates = CPoint(NULL, NULL);
	std::vector<CPoint> wayFindCoordinate = {};
	std::vector<CardPointCoordinatesInfo> vCardPointCoordinatesInfo = {};
	CPoint tagUsefulCoordinate = CPoint(NULL, NULL);


	tagGameMapInfo(CString _mapBmpName = _T(""),
				   CString	_astarBmpName = _T(""),
				   CPoint _RepairmanCoordinates = CPoint(NULL, NULL),
					CString _TeleportName = _T(""),
				   CPoint _ExceptionReturnCoordinates = CPoint(NULL, NULL), 
				   std::vector<CPoint> _wayfindCoordinate = {},
					std::vector<CardPointCoordinatesInfo> _vCardPointCoordinatesInfo = {},
					CPoint _tagUsefulCoordinate = CPoint(NULL, NULL))
					
	{
		mapBmpName					= _astarBmpName;
		astarBmpName				= _astarBmpName;
		RepairmanCoordinates		= _RepairmanCoordinates;
		TeleportName				= _TeleportName;
		ExceptionReturnCoordinates	= _ExceptionReturnCoordinates;
		wayFindCoordinate			= _wayfindCoordinate;
		vCardPointCoordinatesInfo	= _vCardPointCoordinatesInfo;
		tagUsefulCoordinate			= _tagUsefulCoordinate;
	}
};

//传送信息
struct tagCitiesInfo
{
	CString strCitiesName;
	CPoint	pCitiesCoordinates;
	CString strInsideName;
	CPoint  pInsideCoordinates;

	tagCitiesInfo() {
		strCitiesName = _T("");
		pCitiesCoordinates = CPoint(NULL, NULL);
		strInsideName = _T("");
		pInsideCoordinates = CPoint(NULL, NULL);
	}
};

struct tagGameTeleportInfo {

	CString strIslandsName;
	CPoint  pIslandsCoordinates;
	std::vector<tagCitiesInfo*> vCitiesInfo;

	tagGameTeleportInfo() {
		strIslandsName = _T("");
		pIslandsCoordinates = CPoint(NULL, NULL);
		vCitiesInfo = {};
	}
};

//蒸汽船传送岛屿info
struct tagTeleportIslandsinfo {
	CString strIslandsName;
	CPoint pIslandsCoordinates;
	tagTeleportIslandsinfo() {
		strIslandsName = _T("");
		pIslandsCoordinates = CPoint(NULL,NULL);
	}
};



//队伍组员
struct tagTeamUsers {
	int  Id; //组员
	CString strTeamIp;
	CString strTeamPort;
	CString strTeamUrl;
	bool IsOnlie;
	tagTeamUsers()
	{
		Id = NULL;
		strTeamIp = _T("");
		strTeamPort = _T("");
		strTeamUrl = _T("");
		IsOnlie = false;
	}
};

//队伍
struct tagTeam {

	CString					strTeamName;
	tagTeamleader*			pTeamleader;
	CArray<tagTeamUsers*>	arrTeamUsers;

	tagTeam()
	{
		pTeamleader = NULL;
		strTeamName = _T("");
		arrTeamUsers.RemoveAll();
	}
};

struct UpperCoordinates
{
	int x1;
	int y1;
	int x2;
	int y2;

	UpperCoordinates()
	{
		x1 = NULL;
		y1 = NULL;
		x2 = NULL;
		y2 = NULL;
	}

	UpperCoordinates(int m_x1, int m_y1, int m_x2, int m_y2)
	{
		x1 = m_x1;
		y1 = m_y1;
		x2 = m_x2;
		y2 = m_y2;
	}

};

struct tagPeopleInfo
{
	int		iPeopleLv ;	//人物等级
	CString strPeopleName;//人物名字
	CString strPeopleId;//人物ID
	CString strPeopleType;//人物类型

	tagPeopleInfo()
	{
		iPeopleLv = NULL;	
		strPeopleName = _T("");
		strPeopleId = _T("");
		strPeopleType = _T("");
	}

};

struct tagOnlieTeamUsersPeopleInfo
{
	tagPeopleInfo	 UserPeopleInfo;	//人物信息
	tagTeamUsers*	 pTeamUsers;		//组员信息

	tagOnlieTeamUsersPeopleInfo()
	{
		UserPeopleInfo = tagPeopleInfo();
		pTeamUsers = NULL;
	}

};




//struct tagTeleportCitiesInfo
//{
//	CString strCitiesName;
//	CPoint	ptCitesPoint;
//	CString strContaineCitesdName;
//	CPoint	ptContainedCitesPint;
//
//	tagTeleportCitiesInfo()
//	{
//		strCitiesName = _T("");
//		ptCitesPoint = { NULL,NULL };
//		strContaineCitesdName = _T("");
//		ptContainedCitesPint = { NULL,NULL };
//	}
//
//};

////传送消息
//struct tagGameTeleportInfo
//{
//	CString strIslandName;
//	CPoint ptIslandPoint;
//	CArray<tagTeleportCitiesInfo> AyTpCitiesInfo;
//
//	tagGameTeleportInfo()
//	{
//		strIslandName = _T("");
//		ptIslandPoint = { NULL,NULL };
//		AyTpCitiesInfo.RemoveAll();
//	}
//
//};



#define WM_RESTART_MSG (WM_USER + 100)
#define WM_SWITCH_MSG (WM_USER + 101)

