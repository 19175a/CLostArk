// CDialog2.cpp: 实现文件
//

#include "pch.h"
#include "DCM.h"
#include "afxdialogex.h"
#include "CDialog2.h"


// CDialog2 对话框

IMPLEMENT_DYNAMIC(CDialog2, CDialogEx)

CDialog2::CDialog2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CDialog2::~CDialog2()
{
}

void CDialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_Life_ComboBox, m_Life_Collection_ComboBox);
	DDX_Control(pDX, IDC_COMBO_Life_Fish_ComboBox, m_Life_Fish_ComboBox);
}


BEGIN_MESSAGE_MAP(CDialog2, CDialogEx)
END_MESSAGE_MAP()


// CDialog2 消息处理程序
