#pragma once

#ifdef OPENCV_NEED
#include <opencv2/opencv.hpp>
#endif

#include"pch.h"
#include<vector>
struct  tagGameAcconutInfo
{
	int		Id;				//����ID
	CString strAcconut;		//�˺�
	CString strPassword;	//����
	CString strRegion;		//����
	bool	iOrNot;			//�Ƿ���ɽ�������

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
	bool IsWriteIniLogd;  //����ű�����ʱ ��Ϸ������д�뱾��LogDiniʱ��(��Ϊ��ȷ����ǰ���е����ĸ��˺�)
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
	int teamId; //��Ա
	int leaderId;//�鳤

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
	CTaskThread* pTaskThread;			//�������߳�
	CTaskThread* pTaskHelperThread;		//���������߳�
	CString strState;					//ִ�� ��ͣ ֹͣ
	tagGameStatus* pStatus;				//UI����
	tagGameAcconutInfo* pAccountInfo;	//�˺���Ϣ
	bool bDrawWnd;						//���ƴ��ڱ��#��ֹ�����߳�Ҳ��������
	CArray<tagOnlieTeamUsersPeopleInfo*>	arrOnlieTeamUsersPeopleInfo;//������Ա������Ϣ
	CTaskHttpSrvThread* pTaskHttpSrv;
	CString strHttpCmd;
	CString strHttpContent;
	CLock	m_LockHttpCmd;
	void HttpCmd(bool SetOrRecover, CString strCmd = _T(""), CString strContent = _T(""))
	{
		//��ֹ�����߳� ͬʱ�޸�
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
	//�����ʱ��
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

	//������Ϣ #�����߼�����ͨ��HttpͨѶ�ģ����������Ҫ���������ͨѶ����취��ʼ�����������ip��ַ����
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

	WORD	���ض˿�;
	CString	�������;

	CString	�����б�;

	CString	�˺�����;
	CString �׵繲��Ŀ¼;


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

		���ض˿� = 0;
		������� = _T("");

		�����б� = _T("");
		�˺����� = _T("");
		�׵繲��Ŀ¼ = _T("�׵繲��Ŀ¼");

	}
};

struct tagResItem {
	CString strName;

	int iType; //0 ��ͼ 1 ��ɫ 2 ���� 3 �ֵ� 4 ʶ�� 5 λ�� 6 opencv��ͼ 7 Ocr

	CRect rtArea;
	CString strRes; //ͼƬ�� ��ɫ ������

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

//��Ǵ��ڱ���
struct tagWndSubscript {
	CString tagWndHwnd;
	CString tagTitleSubscript;

	tagWndSubscript()
	{
		tagWndHwnd = _T("");
		tagTitleSubscript = _T("");
	}
};



//�����鳤
struct tagTeamleader
{
	int leaderId;//�鳤
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
	CString strResult;	//��������
	int iX;
	int iY;
};

struct OcrUpgrade
{
	CString strResult = _T("");	//��������
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
		WaitToPass = 1,				//1�ȴ�ͨ��
		PressGPass =2,				//2��Gͨ��
		CardPointCoordinates =3		//3���꿨��
	};

	std::vector<CPoint> vpCardPointCoordinates = {};	//��������
	std::vector<CPoint> vpRecordCoordinates = {};		//¼������
	int iRadiusRectangle = NULL;						//���ΰ뾶
	int iType = NULL;									//��������
	CPoint pEndCoordinates = CPoint(NULL, NULL);		//����Ŀ�ĵ�����

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

//������Ϣ
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

//���������͵���info
struct tagTeleportIslandsinfo {
	CString strIslandsName;
	CPoint pIslandsCoordinates;
	tagTeleportIslandsinfo() {
		strIslandsName = _T("");
		pIslandsCoordinates = CPoint(NULL,NULL);
	}
};



//������Ա
struct tagTeamUsers {
	int  Id; //��Ա
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

//����
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
	int		iPeopleLv ;	//����ȼ�
	CString strPeopleName;//��������
	CString strPeopleId;//����ID
	CString strPeopleType;//��������

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
	tagPeopleInfo	 UserPeopleInfo;	//������Ϣ
	tagTeamUsers*	 pTeamUsers;		//��Ա��Ϣ

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

////������Ϣ
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

