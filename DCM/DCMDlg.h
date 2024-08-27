
// DCMDlg.h: 头文件
//
#include "CDialog1.h"
#include "CDialog2.h"
#include "CDialog3.h"
#pragma once


// CDCMDlg 对话框
class CDCMDlg : public CDialogEx
{
// 构造
public:
	CDCMDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DCM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CWinThread* m_hDMInitThread;
	static UINT CDMEngineThread(LPVOID pParam);
public:

	CDialog1 m_dlg1;
	CDialog2 m_dlg2;
	CDialog3 m_dlg3;


	CString m_strDMKey;

	CString m_strWorkPath;

	CListCtrl m_lstWnd;
	CListCtrl m_lstTask;
	CListCtrl m_lstTaskRun;

	CTraceServiceControl	m_TraceServiceControl;

public:
	CButton m_btStart;
	CButton m_btSuspend;

	bool m_bRegDm;
	CString m_strCmd;
	CString m_strParam;
	int m_iRClickID;
	CTabCtrl CGroupClip;

public:
	//卡密
		//线程操作UI控件
	afx_msg LRESULT ThreadOpertUI(WPARAM WParam, LPARAM LParam);

	//设置卡密信息
	void setCardInfo(std::string _loginToken, std::string _cardnum, std::string _cardEndTime, std::string _jurisdictvalue);

	//设置你的窗口业务代码还是否正常工作
	void setStopWork();

	virtual void OnCancel();

	//读写UI信息配置
	void writeUiConfig();
	void readUiConfig();



private:
	bool IsWndExist(int id);
	//CString GetCheckList(CArray<int>& arrCheckID);
	CString GetListInfo();

	void PopMenu(CPoint pt);

private:
	void GetCheckedID(CStringArray& arrID);
	void GetSelectedID(CStringArray& arrID);
	CString GetDlogListAccount(int Id);

	void GetTaskRun(CStringArray& arrTaskRun);
	CString ITostr(int IStay);
	void BugString(CString str);
	void GetUiTeamInfo();
	void SetToolMaintenanceMode();
	void addStrm_Life_ComboBox();

	//ini
	VOID GetAllKeyValueOfIniFileOneSection(TCHAR* pIniFilePath, TCHAR* pSectionName, std::map<CString, CString>& mapIniKeyValue);
	bool iniGetAllKeyValueAllNodeNames(CString strIniFilePath, CString strSectionName, std::map<CString, CString>& mapIniKeyValue);

	//启动钩子监听键盘事件	// 停止钩子监听键盘事件
	void ListenKeyboard();
	void StopListenKeyboard();

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonSuspend();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void WriteLogD();
	afx_msg void OnClose();

	afx_msg LRESULT OnRestartMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSwitchMessage(WPARAM wParam, LPARAM lParam);

	afx_msg void OnMenuCmd(UINT nID);
	afx_msg void OnNMRClickListWnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkListTask(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkListTaskRun(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();

	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton2();
	CButton Draw;
	afx_msg void OnBnClickedIsdraw();
	afx_msg void OnBnClickedButtonStart2();
	afx_msg void OnBnClickedButton3();
};
