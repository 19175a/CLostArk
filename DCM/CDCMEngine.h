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
	tagWndIni				m_WndIni;			//���������ļ�
	CString					m_strWorkPath;		//��ǰ����ִ��Ŀ¼
	int						m_iWidth;			//���ڳߴ�
	int						m_iHeight;
	CArray<tagResItem*>		m_arrRes;			//��Դ�б�
	CArray<tagWndInfo*>		m_arrWnd;			//�����б�
	CArray<tagTaskInfo*>	m_arrTask;			//�����б�
	CArray<tagGameAcconutInfo*>	m_arrAccount;	//�˺���Ϣ
	CArray<int>				m_arrWaitStartWnd;	//�ȴ������Ĵ�������
	CWnd* m_pMainDlg;							//�������Ի��򴰿�
	CDCMDlg* m_pCDlg;
	CArray<tagGroupUser*>   m_arrGroup;
	CHttpSrvThread*			m_pHttpSrv;			//http�����߳� ��������ͨ��
	CLock					m_lkYolo;			//yolo��
	CLock					m_lkOcr;			//ocr��
	CLock					m_lkTaskFile;		//����ر��ļ�����
	
	bool tagOnDraw = false;						//��������
	dmsoft* m_pDm;	
	CArray<tagWndSubscript> m_arrWndSubscriptIndex;//����ˢ�±�Ǵ���

	CKeyMouse* m_CKeyMouse;

	enum TESKSTATE {
		_Start =1,	//��ʼ
		_Stop = -1, //����
		_Switch =3, //�л���һ��
		_SwitchEx =4,//���л���һ��
		_Suspend =5, //��ͣ
		_Resume =6 //�ָ�
	};
	int m_State = NULL; 
	CString m_strParam = _T("");

public://����
	CArray<tagTeam*>		m_arrpTeam;
	void GetTeamInfo(CString UiTeam);
	tagTeam* GetTeam(int Id);
	int GetPort(int Id);
public://���ڹ���
	CWndManager* m_pWndMgr;
	CTaskManager* m_pTaskMgr;
private:
	void ReadWndIni();
	bool LoadRes();

public:
	//ini�ļ�д���ȡ
	CLock  TeamIniClock; //��
	void	WriteTeamIni(CString strPath, CString strSection, CString strKey, CString strValue);
	CString GetTeamiIni(CString strPath, CString strSection, CString strKey, bool Tpy);

	CLock  m_ClockGetCoordinates;
	CLock  m_ClockGetCoordinates_State;
public:
	//��Ϸ��Դ

public:
	//Tool maintenance mode 
	/*
	* 1���¹���(��ɫ�ȼ� ���ᶪ��ԭ����)
	* 2���¹���(����(�������)��ɫ�ȼ� ���ǵ��򶪵�)
	* 3��ͼά�޹���
	* 4����ά�޹���
	*/
	int m_tagToolMaintenanceMode = 3;
	bool disregardLifeAttack = false;

public:
	//��Ϸ����map
	CReadConfigIni* m_ReadConfigIni;

public://����
	void Tset();
public:

	void DownESC();
	int ��ȡ�鳤ID(int id);
	CString ��ȡ������(int id);
	CString ��ȡ�鳤��ַ(int id);

};

