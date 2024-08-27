#pragma once
#include "afxdialogex.h"


// SignDlg 对话框

class SignDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SignDlg)

public:
	SignDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SignDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SignDlg };
#endif

	//线程操作主界面UI控件
	afx_msg LRESULT ThreadOpertUI(WPARAM WParam, LPARAM LParam);
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoginbtn();
	CEdit m_AnnouncementControl;

	CButton m_ChekSaveKeyControl;
};
