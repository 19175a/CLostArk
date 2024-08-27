#pragma once

class CWndManager;
class CTaskManager;
class CHttpSrvThread;
class dmsoft;
class CLDScan;
class CKeyMouse;
class CDCMDlg;
class CReadConfigIni;

class CDCMEngine
{
public:
	CDCMEngine();
	virtual  ~CDCMEngine();

public:
	void intafind();
	void Init();
	bool LoadAccountInfo();
	tagWndInfo* GetWndList(int Id);
	int GetLDList();
	void Start(tagGameAcconutInfo* GameAcconutInfo);
	void Stop(int id = -1);
	void Suspend(int id = -1);
	void Resume(int id = -1);
	void Restart(int id, CStringArray& arrTask);
	void Switch(int id, int dstId, CStringArray& arrTask);
	void KillApp(int id);
	tagResItem* GetResItem(CString strName);
	bool IsWndReady(int id);
	tagTaskInfo* GetTaskInfo(int id);
	void AddTask(int id, CStringArray& arrTask);
	void GetAccount(int id, CString& strAcc, CString& strPwd);
	CString ITostr(int Iargument);
	void BugString(CString str);
	void CopyGamePath();
public:
	tagGameStatus* GetGameStatus(int id);

public:
	tagWndIni				m_WndIni;			//窗口配置文件
	CString					m_strWorkPath;		//当前程序执行目录
	int						m_iWidth;			//窗口尺寸
	int						m_iHeight;
	CArray<tagResItem*>		m_arrRes;			//资源列表
	CArray<tagWndInfo*>		m_arrWnd;			//窗口列表
	CArray<tagTaskInfo*>	m_arrTask;			//任务列表
	CArray<tagGameAcconutInfo*>	m_arrAccount;	//账号信息
	CArray<int>				m_arrWaitStartWnd;	//等待启动的窗口序列
	CWnd* m_pMainDlg;							//程序主对话框窗口
	CDCMDlg* m_pCDlg;
	CArray<tagGroupUser*>   m_arrGroup;
	CHttpSrvThread*			m_pHttpSrv;			//http服务线程 用于内网通信
	CLock					m_lkYolo;			//yolo锁
	CLock					m_lkOcr;			//ocr锁
	CLock					m_lkTaskFile;		//任务关闭文件夹锁
	
	bool tagOnDraw = false;						//开启绘制
	dmsoft* m_pDm;	
	CArray<tagWndSubscript> m_arrWndSubscriptIndex;//窗口刷新标记窗口

	CKeyMouse* m_CKeyMouse;

	enum TESKSTATE {
		_Start =1,	//开始
		_Stop = -1, //结束
		_Switch =3, //切换下一个
		_SwitchEx =4,//不切换下一个
		_Suspend =5, //暂停
		_Resume =6 //恢复
	};
	int m_State = NULL; 
	CString m_strParam = _T("");

public://队伍
	CArray<tagTeam*>		m_arrpTeam;
	void GetTeamInfo(CString UiTeam);
	tagTeam* GetTeam(int Id);
	int GetPort(int Id);
public://窗口管理
	CWndManager* m_pWndMgr;
	CTaskManager* m_pTaskMgr;
private:
	void ReadWndIni();
	bool LoadRes();

public:
	//ini文件写入读取
	CLock  TeamIniClock; //锁
	void	WriteTeamIni(CString strPath, CString strSection, CString strKey, CString strValue);
	CString GetTeamiIni(CString strPath, CString strSection, CString strKey, bool Tpy);

	CLock  m_ClockGetCoordinates;
	CLock  m_ClockGetCoordinates_State;
public:
	//游戏资源

public:
	//Tool maintenance mode 
	/*
	* 1重新购买(绿色等级 不会丢掉原本的)
	* 2重新购买(霸体(钓鱼除外)绿色等级 不是的则丢掉)
	* 3地图维修工具
	* 4宠物维修工具
	*/
	int m_tagToolMaintenanceMode = 3;
	bool disregardLifeAttack = false;

public:
	//游戏传动map
	CReadConfigIni* m_ReadConfigIni;

public://测试
	void Tset();
public:

	void DownESC();
	int 获取组长ID(int id);
	CString 获取组队身份(int id);
	CString 获取组长地址(int id);

};

