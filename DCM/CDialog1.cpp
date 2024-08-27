// CDialog1.cpp: 实现文件
//

#include "pch.h"
#include "DCM.h"
#include "afxdialogex.h"
#include "CDialog1.h"


// CDialog1 对话框

IMPLEMENT_DYNAMIC(CDialog1, CDialogEx)

CDialog1::CDialog1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDialog1::~CDialog1()
{
}

void CDialog1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Pattern1, Pattern1);
	DDX_Control(pDX, IDC_Pattern2, Pattern2);
	DDX_Control(pDX, IDC_Pattern3, Pattern3);
	DDX_Control(pDX, IDC_Pattern4, Pattern4);
	DDX_Control(pDX, IDC_CHECK1, Wushigongji);
}


BEGIN_MESSAGE_MAP(CDialog1, CDialogEx)
END_MESSAGE_MAP()


// CDialog1 消息处理程序
