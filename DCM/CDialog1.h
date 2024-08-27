#pragma once
#include "afxdialogex.h"


// CDialog1 对话框

class CDialog1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDialog1)

public:
	CDialog1(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialog1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	// 重新购买(绿色等级 不会丢掉原本的)
	CButton Pattern1;
	// 重新购买(霸体(钓鱼除外)绿色等级 不是的则丢掉)
	CButton Pattern2;
	// 地图维修工具
	CButton Pattern3;
	// 宠物维修工具
	CButton Pattern4;
	CButton Wushigongji;
};
