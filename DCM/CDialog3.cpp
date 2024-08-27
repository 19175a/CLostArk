// CDialog3.cpp: 实现文件
//

#include "pch.h"
#include "DCM.h"
#include "afxdialogex.h"
#include "CDialog3.h"


// CDialog3 对话框

IMPLEMENT_DYNAMIC(CDialog3, CDialogEx)

CDialog3::CDialog3(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{

}

CDialog3::~CDialog3()
{
}

void CDialog3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ByPostName, m_ByPostNameControl);
}


BEGIN_MESSAGE_MAP(CDialog3, CDialogEx)
END_MESSAGE_MAP()


// CDialog3 消息处理程序
