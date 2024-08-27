
// DCMDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DCM.h"
#include "DCMDlg.h"
#include "afxdialogex.h"
#include "CDCMEngine.h"
#include "obj.h"
#include "CHttpSrvThread.h"
#include"CKeyMouse.h"

#include "NetVerHelp/Help/NetworkVerHelp.h"
#include "NetVerHelp/Help/OtherHelpTools.h"
#define UM_ThreadOpertUI    WM_USER+101

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
int Is_DEBUG = 1;
#else
// 在 Release 模式下执行的代码

#endif

//#define KEYBOARDHOOK



CDCMEngine* g_pEngine = NULL;


//当前窗口句柄
static HWND CurrnetHWnd;
int index = 1;
bool stopWork = false;

//登录成功后的token令牌
string loginToken;

//登录的卡密
string cardnum;

//卡密到期时间 
string cardEndTime;
//此卡密的权限
string jurisdictvalue;





#pragma region 线程操作UI控件
//线程操作UI控件
LRESULT CDCMDlg::ThreadOpertUI(WPARAM WParam, LPARAM LParam)
{
	int flag = (int)WParam;
	if (flag == 1)
	{
		//更新MyCodeRunLbl
		std::wstring stemp2 = OtherHelp::OtherHelpTools::StoWs("自已业务代码运行" + to_string((index)++));
		LPCWSTR result2 = stemp2.c_str();
		GetDlgItem(IDD_DCM_DIALOG)->SetWindowText(result2);
	}
	return 1;
}
#pragma endregion

#pragma region 您窗口的自己的业务代码
//您窗口的自己的业务代码
UINT RunSelfBusiness(LPVOID pParam) {
	HWND hwnd = (HWND)pParam;
	while (stopWork == false)
	{
		//更新界面数据
		PostMessageA(hwnd, UM_ThreadOpertUI, 1, true);
		Sleep(1000);
	}
	return 0;
}
#pragma endregion

#pragma region 设置卡密信息
//设置卡密信息
void CDCMDlg::setCardInfo(string _loginToken, string _cardnum, string _cardEndTime, string _jurisdictvalue)
{
	VMProtectBegin("GetIniSoftInfo");
	loginToken = _loginToken;
	cardnum = _cardnum;
	cardEndTime = _cardEndTime;
	jurisdictvalue = _jurisdictvalue;
	VMProtectEnd();
}
#pragma endregion



#pragma region 设置你的窗口业务代码还是否正常工作
//设置你的窗口业务代码还是否正常工作
void CDCMDlg::setStopWork() {
	stopWork = true;
}
#pragma endregion

#pragma region 退出登录
//退出登录
void CDCMDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//调用接口退出登录
	Model::In_LoginOutArgs LoginOutArgs;
	LoginOutArgs.maccode = Help::NetworkVerHelp::GetMac();//必填
	LoginOutArgs.timestamp = Help::NetworkVerHelp::GetTimeStamp();//必填
	LoginOutArgs.cardnumorusername = cardnum;
	LoginOutArgs.token = loginToken;
	Model::Result LoginOutResult = Help::NetworkVerHelp::GeLoginOut(LoginOutArgs);

	OtherHelp::OtherHelpTools::TerminateApp();

	CDialogEx::OnCancel();
}
#pragma endregion



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CDCMDlg 对话框


#define TIME_UPDATE_WND 1

HHOOK g_keyboardHook;

CDCMDlg::CDCMDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DCM_DIALOG, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
	VMProtectBegin("GetIniSoftInfo");

	m_strDMKey = _T("jv965720b239b8396b1b7df8b768c919e86e10f");

	m_bRegDm = false;
	m_strCmd = _T("注册");
	m_strParam = _T("");

	m_iRClickID = -1;
	VMProtectEnd();
}

void CDCMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WND, m_lstWnd);
	DDX_Control(pDX, IDC_LIST_TASK, m_lstTask);
	DDX_Control(pDX, IDC_LIST_TASK_RUN, m_lstTaskRun);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
	DDX_Control(pDX, IDC_BUTTON_START, m_btStart);
	DDX_Control(pDX, IDC_BUTTON_SUSPEND, m_btSuspend);
	DDX_Control(pDX, IDC_TAB1, CGroupClip);
}

BEGIN_MESSAGE_MAP(CDCMDlg, CDialogEx)
	ON_MESSAGE(WM_RESTART_MSG, OnRestartMessage)
	ON_MESSAGE(WM_SWITCH_MSG, OnSwitchMessage)


	ON_COMMAND_RANGE(ID_MENU_START, ID_MENU_END, &CDCMDlg::OnMenuCmd)


	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CDCMDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_SUSPEND, &CDCMDlg::OnBnClickedButtonSuspend)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_WND, &CDCMDlg::OnNMRClickListWnd)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TASK, &CDCMDlg::OnNMDblclkListTask)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TASK_RUN, &CDCMDlg::OnNMDblclkListTaskRun)
	ON_BN_CLICKED(IDC_BUTTON1, &CDCMDlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDCMDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDCMDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_IsDraw, &CDCMDlg::OnBnClickedIsdraw)
	ON_BN_CLICKED(IDC_BUTTON_START2, &CDCMDlg::OnBnClickedButtonStart2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDCMDlg::OnBnClickedButton3)
END_MESSAGE_MAP()

// CDCMDlg 消息处理程序
void StartKeyboardHook();


BOOL CDCMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	//_SignDlg.ShowWindow(SW_SHOWNORMAL); // 显示非模态对话框

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标



	DWORD dwStyle = m_lstWnd.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	//dwStyle |= LVS_EX_CHECKBOXES;
	m_lstWnd.SetExtendedStyle(dwStyle);
	m_lstWnd.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 40);
	m_lstWnd.InsertColumn(1, _T("账号"), LVCFMT_CENTER, 50);
	m_lstWnd.InsertColumn(2, _T("密码"), LVCFMT_CENTER, 50);
	m_lstWnd.InsertColumn(3, _T("大区"), LVCFMT_CENTER, 50);
	m_lstWnd.InsertColumn(4, _T("状态"), LVCFMT_CENTER, 70);
	m_lstWnd.InsertColumn(5, _T("完成"), LVCFMT_CENTER, 50);
	m_lstWnd.InsertColumn(6, _T("金币"), LVCFMT_CENTER, 70);
	
	dwStyle = m_lstTask.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_lstTask.SetExtendedStyle(dwStyle);
	m_lstTask.InsertColumn(0, _T("待选任务"), LVCFMT_CENTER, 120);

	dwStyle = m_lstTaskRun.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_lstTaskRun.SetExtendedStyle(dwStyle);
	m_lstTaskRun.InsertColumn(0, _T("执行任务"), LVCFMT_CENTER, 120);

	g_pEngine = new CDCMEngine();
	g_pEngine->Init();
	g_pEngine->m_pCDlg = this;

	TCHAR szPath[256] = { 0 };
	CWHService::GetWorkDirectory(szPath, 256);
	m_strWorkPath = szPath;

	//导入任务配置
	CStringArray arrTask;
	Split(g_pEngine->m_WndIni.任务列表, arrTask, _T("|"));
	for (int i = 0; i < arrTask.GetCount(); i++)
	{
		int iRow = m_lstTask.GetItemCount();
		m_lstTask.InsertItem(iRow, _T(""));
		m_lstTask.SetItemText(i, 0, arrTask[i]);
	}

	//导入账号至Ui
	for (int i = 0; i < g_pEngine->m_arrAccount.GetCount(); i++)
	{
		m_lstWnd.InsertItem(i, _T(""));
		m_lstWnd.SetItemText(i, 0, ITostr(i + 1));
		m_lstWnd.SetItemText(i, 1, g_pEngine->m_arrAccount[i]->strAcconut);
		m_lstWnd.SetItemText(i, 2, g_pEngine->m_arrAccount[i]->strPassword);
		m_lstWnd.SetItemText(i, 3, g_pEngine->m_arrAccount[i]->strRegion);
		if (g_pEngine->m_arrAccount[i]->iOrNot) {
			m_lstWnd.SetItemText(i, 5, _T("完成"));
		}
		else{
			m_lstWnd.SetItemText(i, 5, _T("未完成"));
		}

	}

	//分组夹
	CGroupClip.InsertItem(0, _T("工具设置"));
	CGroupClip.InsertItem(1, _T("生活目标地图"));
	CGroupClip.InsertItem(2, _T("邮寄设置"));

	m_dlg1.Create(IDD_DIALOG1, &CGroupClip);//创建窗口到选项卡
	m_dlg2.Create(IDD_DIALOG2, &CGroupClip);
	m_dlg3.Create(IDD_DIALOG3, &CGroupClip);


	CRect rect;
	CGroupClip.GetClientRect(rect);//取到选项卡窗口控件矩形设置大小
	rect.top += 23; rect.left += 2; rect.right -= 4; rect.bottom -= 3;

	// 窗口移动到选项卡位置
	m_dlg1.MoveWindow(&rect);
	m_dlg2.MoveWindow(&rect);
	m_dlg3.MoveWindow(&rect);

	//SW_SHOW
	m_dlg1.ShowWindow(SW_SHOW);//显示选择文件夹
	m_dlg2.ShowWindow(SW_HIDE);//隐藏选择文件夹
	m_dlg3.ShowWindow(SW_HIDE);

	CGroupClip.SetCurSel(0);//设置默认选项卡

	//游戏启动器路径
	g_pEngine->CopyGamePath();

	

	SetTimer(TIME_UPDATE_WND, 3000, NULL);


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);//logo

#ifdef KEYBOARDHOOK
	StartKeyboardHook();
#endif



	m_hDMInitThread = AfxBeginThread(CDMEngineThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	CString title;
	title.Format(L"白茶"); // 动态显示 Id 的值作为标题
	this->SetWindowText(title);

	//移动窗口位置
	CRect rect1;
	rect1.left = -6;
	rect1.top = 786;
	rect1.right = 1283;
	rect1.bottom = 1060;
	MoveWindow(&rect1);


	//生活组合框
	addStrm_Life_ComboBox();

	//默认初始化控件
	m_dlg1.Pattern2.SetCheck(1);
	m_dlg1.Wushigongji.SetCheck(1);

	//读取配置Ui
	readUiConfig();

	//初始化键鼠驱动
	g_pEngine->m_pDm->RunApp(g_pEngine->m_strWorkPath + _T("\\tmpMapA\\调试工具.exe"), 1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDCMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDCMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDCMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void OpenDebugWin32Console()
{
	FreeConsole();
	setlocale(LC_ALL, "chs");
	AllocConsole();
	FILE* f = 0;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONIN$", "r", stdin);
	HWND hConsole = GetConsoleWindow();
	RemoveMenu(GetSystemMenu(hConsole, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	RemoveMenu(GetSystemMenu(hConsole, FALSE), SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);
	RemoveMenu(GetSystemMenu(hConsole, FALSE), SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)0, true);
}


void CDCMDlg::OnBnClickedButtonStart()
{
	SetToolMaintenanceMode();
	g_pEngine->disregardLifeAttack = m_dlg1.Wushigongji.GetCheck();

	writeUiConfig();

	if (!m_lstTaskRun.GetItemCount()) {
		AfxMessageBox(_T("您未选择执行任务"));
		return;
	}

	//CStringArray arrID;
	//GetCheckedID(arrID);
	//if (arrID.GetCount() == 0)
	//{
	//	LogE(_T("没有选择任何窗口，无法启动"));
	//	return;
	//}

	CString strTxt;
	m_btStart.GetWindowText(strTxt);



	//m_strParam = GetListInfo();
	//m_strCmd = _T("启动");
	g_pEngine->m_State = CDCMEngine::TESKSTATE::_Start;
	m_btStart.SetWindowText(_T("禁止"));
	m_btStart.EnableWindow(FALSE);
}

void CDCMDlg::OnBnClickedButtonSuspend()
{
	CString strTxt;
	m_btSuspend.GetWindowText(strTxt);

	if (strTxt == _T("暂停"))
	{
		m_strCmd = _T("暂停");
		m_btSuspend.SetWindowText(_T("恢复"));
	}
	else if (strTxt == _T("恢复"))
	{
		m_strCmd = _T("恢复");
		m_btSuspend.SetWindowText(_T("暂停"));
	}
}

void CDCMDlg::WriteLogD()
{
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");
	CString strTxt;
	GetDlgItemText(IDC_TRACE_MESSAGE, strTxt); // 获取控件的内容

	CStdioFile file;
	if (file.Open(g_pEngine->m_strWorkPath + _T("\\LogD.txt"), CFile::modeCreate | CFile::modeWrite, NULL)) {
		file.WriteString(strTxt.GetString());
		file.Close();
	}
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

}

void CDCMDlg::OnTimer(UINT_PTR nIDEvent)
{
	//if (nIDEvent == TIME_UPDATE_WND)
	//{
	//	for (int i = 0; i < g_pEngine->m_arrTask.GetCount(); i++)
	//	{
	//		if (g_pEngine->m_arrTask[i])
	//		{
	//			for (int j = 0; j < m_lstWnd.GetItemCount(); j++)
	//			{
	//				CString strID = m_lstWnd.GetItemText(j, 0);
	//				int Tmp = g_pEngine->m_arrTask[i]->pAccountInfo->Id;

	//				if (ITostr(g_pEngine->m_arrTask[i]->pAccountInfo->Id) == strID)
	//				{
	//					if (g_pEngine->m_arrWnd[i])
	//					{
	//						m_lstWnd.SetItemText(i, 1, I2S((int)g_pEngine->m_arrWnd[i]->hWnd));
	//						m_lstWnd.SetItemText(i, 2, g_pEngine->m_arrWnd[i]->strTitle);
	//						if (g_pEngine->m_arrWnd[i]->pTaskInfo->pStatus)
	//						{
	//							m_lstWnd.SetItemText(i, 3, g_pEngine->m_arrWnd[i]->pTaskInfo->pStatus->state);
	//						}

	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	WriteLogD();
	CDialogEx::OnTimer(nIDEvent);
}



bool CDCMDlg::IsWndExist(int id)
{
	for (int i = 0; i < m_lstWnd.GetItemCount(); i++)
	{
		int idTmp = _ttoi(m_lstWnd.GetItemText(i, 0));
		if (id == idTmp)
			return true;
	}

	return false;
}

//CString CDCMDlg::GetCheckList(CArray<int>& arrCheckID)
//{
//	CString strID = _T("");
//	for (int i = 0; i < m_lstWnd.GetItemCount(); i++)
//	{
//		if (m_lstWnd.GetCheck(i))
//		{
//			CString strTmp = m_lstWnd.GetItemText(i, 0);
//			strTmp += m_lstWnd.GetItemText(i, 0)
//
//				arrCheckID.Add(_ttoi(strTmp));
//			strID += strTmp + _T("-");
//		}
//	}
//
//	return strID;
//}

CString CDCMDlg::GetListInfo()
{
	CString strTmpInfo = _T("");
	CString Ret = _T("");
	for (int i = 0; i < m_lstWnd.GetItemCount(); i++)
	{
		strTmpInfo = _T("");
		strTmpInfo += m_lstWnd.GetItemText(i, 0);
		strTmpInfo += _T("-");
		strTmpInfo += m_lstWnd.GetItemText(i, 1);
		strTmpInfo += _T("-");
		strTmpInfo += m_lstWnd.GetItemText(i, 2);
		strTmpInfo += _T("-");
		strTmpInfo += m_lstWnd.GetItemText(i, 3);
		strTmpInfo += _T("-");
		strTmpInfo += m_lstWnd.GetItemText(i, 4);
		strTmpInfo += _T("-");
		strTmpInfo += m_lstWnd.GetItemText(i, 5);
		strTmpInfo += _T("-");
		strTmpInfo += _T("|");
		Ret += strTmpInfo;
	}

	return Ret;
}


// 钩子函数
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboardData = (KBDLLHOOKSTRUCT*)lParam;

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

			DWORD vkCode = pKeyboardData->vkCode;
			if (vkCode == VK_RETURN) {
				CString strText = _T("");
				g_pEngine->m_pCDlg->m_btSuspend.GetWindowText(strText);
				if (strText == _T("暂停")) {
					g_pEngine->Suspend();
					g_pEngine->m_pCDlg->m_btSuspend.SetWindowText(_T("恢复"));
				}
				else if (strText == _T("恢复"))
				{
					g_pEngine->Resume();
					g_pEngine->m_pCDlg->m_btSuspend.SetWindowText(_T("暂停"));
				}
				//UnhookWindowsHookEx(g_keyboardHook);

			}
		}
	}


	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// 启动钩子
void StartKeyboardHook()
{
	g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
}

void StopKeyboardHook()
{
	UnhookWindowsHookEx(g_keyboardHook);
}



UINT CDCMDlg::CDMEngineThread(LPVOID pParam)
{
	CDCMDlg* pThis = (CDCMDlg*)pParam;
	CoInitializeEx(NULL, 0);

	//pThis->ListenKeyboard();


	while (true)
	{
		//int Stat = g_pEngine->m_State;


		if (pThis->m_strCmd == _T("退出"))
		{
			break;
		}
		else if (pThis->m_strCmd == _T("注册"))
		{
			HMODULE hDmReg = LoadLibrary(pThis->m_strWorkPath + _T("/DmReg.dll"));
			if (hDmReg == NULL)
			{
				LogE(_T("DmReg.dll文件不存在"));
				continue;
			}

			typedef long (CALLBACK* TypeSetDllPathW)(WCHAR* szPath, long mode);
			TypeSetDllPathW pfnSetDllPathW = (TypeSetDllPathW)GetProcAddress(hDmReg, "SetDllPathW");
			if (pfnSetDllPathW == NULL)
				continue;

			CString strDmPath = pThis->m_strWorkPath + _T("/dm.dll");
			long iRet = pfnSetDllPathW((WCHAR*)strDmPath.GetString(), 0);

			dmsoft* pDm = new dmsoft();
			long dm_ret = pDm->Reg(pThis->m_strDMKey, _T("jpbgck4onidt700"));
			if (dm_ret != 1)
			{
				LogE(_T("大漠注册失败! 返回值:%d"), dm_ret);
				if (pDm)
					delete pDm;

				continue;
			}
			else {
				LogD(_T("大漠注册成功! 版本号: %s..."), pDm->Ver());
				g_pEngine->m_pDm = new dmsoft();
				pThis->m_strCmd = _T("");
				pThis->m_strParam = _T("");
			}


			if (pDm)
				delete pDm;
		}
		else if (g_pEngine->m_State == CDCMEngine::TESKSTATE::_Start)
		{
			g_pEngine->m_State = NULL;
			pThis->m_strParam = pThis->GetListInfo();
			CStringArray arrTmp;
			Split(pThis->m_strParam, arrTmp, _T("|"));
			if (arrTmp.GetCount() == NULL) {
				AfxMessageBox(_T("您可能为填写账号信息,或者格式错误。请检查后重新打开脚本"));
				break;
			}

			bool IsExists = false;
			for (int i = 0; i < arrTmp.GetCount(); i++)
			{
				CStringArray arrTmpAcconutInfo;
				Split(arrTmp[i], arrTmpAcconutInfo, _T("-"));
				if (arrTmpAcconutInfo.GetCount() == NULL) {
					continue;
				}

				if (arrTmpAcconutInfo[4] == _T("完成")) {
					continue;
				}

				tagGameAcconutInfo* tmpGameAcconutInfo;
				tmpGameAcconutInfo = new tagGameAcconutInfo();
				tmpGameAcconutInfo->Id = _ttoi(arrTmpAcconutInfo[0]);
				tmpGameAcconutInfo->strAcconut = arrTmpAcconutInfo[1];
				tmpGameAcconutInfo->strPassword = arrTmpAcconutInfo[2];
				tmpGameAcconutInfo->strRegion = arrTmpAcconutInfo[3];
				tmpGameAcconutInfo->iOrNot = false;
				g_pEngine->Start(tmpGameAcconutInfo);
				IsExists = true;
				break;
			}

			if (!IsExists){
				//发送消息给 中控 关机切换下一个窗口
				g_pEngine->m_pDm->RunApp(g_pEngine->m_strWorkPath + _T("\\tmpMapA\\关机切换\\关机切换下一个窗口.exe"), 1);
				while (true)
				{
					Sleep(1000);
					LogE(_T("等待结束："));
				}
			}

			pThis->m_strCmd = _T("");
			pThis->m_strParam = _T("");
		}
		else if (pThis->m_strCmd == _T("停止"))
		{
			g_pEngine->Stop();
		}
		else if (pThis->m_strCmd == _T("暂停"))
		{
			g_pEngine->Suspend();
		}
		else if (pThis->m_strCmd == _T("恢复"))
		{
			g_pEngine->Resume();
		}
		else if (pThis->m_strCmd == _T("重启"))
		{
			CStringArray arrTask;
			pThis->GetTaskRun(arrTask);

			g_pEngine->Restart(_ttoi(pThis->m_strParam), arrTask);
		}
		else if (g_pEngine->m_State == CDCMEngine::TESKSTATE::_Switch)
		{
			g_pEngine->m_State = NULL;
			LogD(_T("切换"));

			//写入LogdIni
			CString retAccount = pThis->GetDlogListAccount(_ttoi(g_pEngine->m_strParam));
			if (retAccount != _T("")) {
				CString str;
				CTime tm;
				tm = CTime::GetCurrentTime();
				str = tm.Format(_T("%Y年%m月%d日"));
				g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\GameAccountLogD.ini"), _T("完成时间"), retAccount, str);

				//改变列表框LogD状态
				for (int i = 0; i < pThis->m_lstWnd.GetItemCount(); i++)
				{
					int tmpId = _ttoi(pThis->m_lstWnd.GetItemText(i, 0));
					if (tmpId == _ttoi(g_pEngine->m_strParam)) {
						pThis->m_lstWnd.SetItemText(i, 5, _T("完成"));
					}
				}
			}

			//销毁
			g_pEngine->Stop(_ttoi(g_pEngine->m_strParam));
			g_pEngine->m_State = CDCMEngine::TESKSTATE::_Start;
			pThis->m_strParam = pThis->GetListInfo();
			continue;
		}
		else if (g_pEngine->m_State == CDCMEngine::TESKSTATE::_SwitchEx) {
			LogD(_T("不切换"));
			//销毁
			g_pEngine->Stop(_ttoi(g_pEngine->m_strParam));
			g_pEngine->m_State = CDCMEngine::TESKSTATE::_Start;
			pThis->m_strParam = pThis->GetListInfo();
			continue;
		}
		else if (pThis->m_strCmd == _T("执行"))
		{
			CStringArray arrTask;
			pThis->GetTaskRun(arrTask);
			g_pEngine->AddTask(pThis->m_iRClickID, arrTask);
		}

		//pThis->m_strCmd = _T("");
		//pThis->m_strParam = _T("");
		Sleep(1000);
	}

	CoUninitialize();

	return 0;
}

void CDCMDlg::OnClose()
{
	//m_hDMInitThread->Delete();
	OnCancel(); //结束心跳 调用接口退出登录
	g_pEngine->Stop();

	if (g_pEngine)
		delete g_pEngine;

	CDialogEx::OnClose();
}

LRESULT CDCMDlg::OnRestartMessage(WPARAM wParam, LPARAM lParam)
{
	m_strCmd = _T("重启");

	CString strID;
	strID.Format(_T("%d"), wParam);
	m_strParam = strID;

	return 0;
}

LRESULT CDCMDlg::OnSwitchMessage(WPARAM wParam, LPARAM lParam)
{

	//判断是否需要写入LogdIni
	int iWriteIniLogd = (int)lParam;
	if (iWriteIniLogd == 1) {
		CString retAccount = GetDlogListAccount((int)wParam);
		if (retAccount != _T("")) {
			CString str;
			CTime tm;
			tm = CTime::GetCurrentTime();
			str = tm.Format(_T("%Y年%m月%d日"));
			g_pEngine->WriteTeamIni(m_strWorkPath + _T("\\GameAccountLogD.ini"), _T("完成时间"), retAccount, str);

			//改变列表框LogD状态
			for (int i = 0; i < m_lstWnd.GetItemCount(); i++)
			{
				int tmpId = _ttoi(m_lstWnd.GetItemText(i, 0));
				if (tmpId == (int)wParam) {
					m_lstWnd.SetItemText(i, 5, _T("完成"));
				}
			}
		}
	}


	CString strWriteIniLogd;
	strWriteIniLogd.Format(_T("%d"), wParam); //销毁的id

	m_strCmd = _T("切换");
	m_strParam = strWriteIniLogd;



	return 0;
}

void CDCMDlg::PopMenu(CPoint pt)
{
	CMenu mainMenu;
	mainMenu.CreatePopupMenu();
	mainMenu.AppendMenu(MF_STRING, ID_MENU_START + 1, TEXT("启动"));
	mainMenu.AppendMenu(MF_STRING, ID_MENU_START + 2, TEXT("停止"));
	mainMenu.AppendMenu(MF_SEPARATOR); //分隔线
	mainMenu.AppendMenu(MF_STRING, ID_MENU_START + 3, TEXT("暂停"));
	mainMenu.AppendMenu(MF_STRING, ID_MENU_START + 4, TEXT("恢复"));
	mainMenu.AppendMenu(MF_SEPARATOR); //分隔线
	mainMenu.AppendMenu(MF_STRING, ID_MENU_START + 5, TEXT("执行任务"));

	mainMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	mainMenu.DestroyMenu();
}

void CDCMDlg::GetCheckedID(CStringArray& arrID)
{
	//检查是否有选择checkbox的窗口
	int count = m_lstWnd.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		if (m_lstWnd.GetCheck(i))
			arrID.Add(m_lstWnd.GetItemText(i, 0));
	}
}

void CDCMDlg::GetSelectedID(CStringArray& arrID)
{
	for (int i = 0; i < m_lstWnd.GetItemCount(); i++)
	{
		if (CDIS_GRAYED == m_lstWnd.GetItemState(i, CDIS_GRAYED))
		{
			arrID.Add(m_lstWnd.GetItemText(i, 0));
		}
	}
}

CString CDCMDlg::GetDlogListAccount(int Id)
{
	for (int i = 0; i < m_lstWnd.GetItemCount(); i++)
	{
		int tmpId = _ttoi(m_lstWnd.GetItemText(i, 0));
		if (tmpId == Id) {
			return m_lstWnd.GetItemText(i, 1);
		}

	}

	return _T("");
}

void CDCMDlg::GetTaskRun(CStringArray& arrTaskRun)
{
	for (int i = 0; i < m_lstTaskRun.GetItemCount(); i++)
	{
		arrTaskRun.Add(m_lstTaskRun.GetItemText(i, 0));
	}
}



void CDCMDlg::OnMenuCmd(UINT nID)
{
	int iMenuId = nID - ID_MENU_START;
	switch (iMenuId)
	{
	case 1:
	{
		m_strCmd = _T("启动");
		m_strParam = I2S(m_iRClickID);
		LogD(_T("启动:%d"), m_iRClickID);
		break;
	}
	case 2:
	{
		m_strCmd = _T("停止");
		m_strParam = I2S(m_iRClickID);
		LogD(_T("停止:%d"), m_iRClickID);
		break;
	}
	case 3:
	{
		m_strCmd = _T("暂停");
		m_strParam = I2S(m_iRClickID);
		LogD(_T("暂停:%d"), m_iRClickID);
		break;
	}
	case 4:
	{
		m_strCmd = _T("恢复");
		m_strParam = I2S(m_iRClickID);
		LogD(_T("恢复:%d"), m_iRClickID);
		break;
	}
	case 5:
	{
		m_strCmd = _T("执行");
		m_strParam = _T("");

		LogD(_T("执行:%d 任务列表:%s"), m_iRClickID, m_strParam);
		break;
	}
	}
}

void CDCMDlg::OnNMRClickListWnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	POINT pt;
	GetCursorPos(&pt);

	int iLine = m_lstWnd.GetSelectionMark();
	if (iLine == -1) {
		return;
	}

	m_iRClickID = _ttoi(m_lstWnd.GetItemText(iLine, 0));

	PopMenu(pt);
}

void CDCMDlg::OnNMDblclkListTask(NMHDR* pNMHDR, LRESULT* pResult)
{
	for (int i = 0; i < m_lstTask.GetItemCount(); i++)
	{
		if (CDIS_GRAYED == m_lstTask.GetItemState(i, CDIS_GRAYED))
		{
			CString strTask = m_lstTask.GetItemText(i, 0);

			int iRow = m_lstTaskRun.GetItemCount();
			m_lstTaskRun.InsertItem(iRow, _T(""));
			m_lstTaskRun.SetItemText(iRow, 0, strTask);
			break;
		}
	}
}

void CDCMDlg::OnNMDblclkListTaskRun(NMHDR* pNMHDR, LRESULT* pResult)
{
	for (int i = 0; i < m_lstTaskRun.GetItemCount(); i++)
	{
		if (CDIS_GRAYED == m_lstTaskRun.GetItemState(i, CDIS_GRAYED))
		{
			m_lstTaskRun.DeleteItem(i);
			break;
		}
	}
}

void CDCMDlg::BugString(CString str)
{
	CTime currentTime = CTime::GetCurrentTime();
	CString CstrTime = currentTime.Format(_T("%m-%d %H.%M.%S"));
	std::string strTime = CT2A(CstrTime.GetString());
	std::string m_strContent = CT2A(str.GetString());
	std::cout << strTime << ":" << m_strContent << std::endl;

}

void CDCMDlg::GetUiTeamInfo()
{
	//获取一个队伍的所有信息 已“,”分割不同的退伍
	CString strTeam = _T("");
	CString strTmpTeam = _T("");
	CString strTeamName = _T("");

	int itemCount = m_lstWnd.GetItemCount();
	for (int i = 0; i < itemCount; i++)
	{
		CString Id = m_lstWnd.GetItemText(i, 0);
		CString teamInfo = m_lstWnd.GetItemText(i, 7);

		CStringArray arrTeamTmp;
		Split(teamInfo, arrTeamTmp, _T(":"));
		CString strUiTmpTeamName = arrTeamTmp[0];

		if (strTeamName == _T(""))
		{
			strTeamName = strUiTmpTeamName;
			strTmpTeam = Id + _T(":") + teamInfo + _T("|");
		}
		else if (strTeamName == strUiTmpTeamName)
		{
			strTmpTeam += Id + _T(":") + teamInfo + _T("|");
		}
		else if (strTeamName != strUiTmpTeamName)
		{
			strTeamName = _T("");
			strTeam += strTmpTeam + _T(",");
			strTmpTeam = _T("");
			strTeamName = strUiTmpTeamName;
			strTmpTeam = Id + _T(":") + teamInfo + _T("|");
		}

		//最后队伍，最后成员时保存
		if (i == itemCount - 1)
		{
			strTeam += strTmpTeam + _T(",");
		}

	}

	//查看
	CString str = strTeam;
	//BugString(str);
	if (strTeam != _T(""))
	{
		g_pEngine->GetTeamInfo(strTeam);
		strTeamName = _T("");
	}

}

CString CDCMDlg::ITostr(int IStay)
{
	CString strTmp;
	strTmp.Format(_T("%d"), IStay);
	return strTmp;
}

int HexToDecimal(const std::string& hexString) {
	int decimalValue = 0;

	// 遍历16进制字符串中的每个字符
	for (size_t i = 0; i < hexString.length(); ++i) {
		char c = hexString[i];

		// 将字符转换为对应的数字值
		int digitValue;
		if (isdigit(c)) {
			digitValue = c - '0';
		}
		else if (isxdigit(c)) {
			if (islower(c)) {
				digitValue = c - 'a' + 10;
			}
			else {
				digitValue = c - 'A' + 10;
			}
		}
		else {
			// 非法字符，可以根据需要进行处理
			// 这里简单地返回0
			return 0;
		}

		// 更新累积的10进制值
		decimalValue = decimalValue * 16 + digitValue;
	}

	return decimalValue;
}

void OpenDebugWin32Console();





void CDCMDlg::ListenKeyboard()
{
	StartKeyboardHook();
}

void CDCMDlg::StopListenKeyboard()
{
	StopKeyboardHook();
}

VOID GetAllKeyValueOfIniFileOneSection(TCHAR* pIniFilePath, TCHAR* pSectionName, std::map<CString, CString>& mapIniKeyValue)
{
	TCHAR szBuf[4096] = { 0 };
	DWORD readlen = ::GetPrivateProfileSection(pSectionName, szBuf, 4096, pIniFilePath);
	TCHAR* pbuf = szBuf;
	size_t size = _tcslen(pbuf);

	//如果读取到的行，长度不为0时，说明此行存在，继续取值
	while (size)
	{
		CString str = pbuf;
		CString strKey, strValue;
		if (AfxExtractSubString(strKey, (LPCTSTR)str, 0, _T('=')) && AfxExtractSubString(strValue, (LPCTSTR)str, 1, _T('=')))
		{
			if (!mapIniKeyValue.count(strKey))
			{
				mapIniKeyValue.insert(std::make_pair(strKey, strValue));
			}
		}
		pbuf += size + 1;
		size = _tcslen(pbuf);
	}


	return;
}

void ListFilesInDirectory(LPCTSTR lpDirectory) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(lpDirectory, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("Error opening directory: %s\n"), lpDirectory);
		return;
	}

	do {
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			_tprintf(TEXT("File: %s\n"), FindFileData.cFileName);
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
}

int _tmain(int argc, TCHAR* argv[]) {
	if (argc != 2) {
		_tprintf(TEXT("Usage: %s [directory_path]\n"), argv[0]);
		return 1;
	}

	ListFilesInDirectory(argv[1]);
	return 0;
}

void CDCMDlg::OnBnClickedButton1()
{

}


void CDCMDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int sel = CGroupClip.GetCurSel();
	if (sel == 0)
	{
		m_dlg1.ShowWindow(SW_SHOW);//显示
		m_dlg2.ShowWindow(SW_HIDE);//隐藏
		m_dlg3.ShowWindow(SW_HIDE);//隐藏
	}
	else if (sel == 1)
	{
		m_dlg1.ShowWindow(SW_HIDE);//隐藏
		m_dlg2.ShowWindow(SW_SHOW);//显示
		m_dlg3.ShowWindow(SW_HIDE);//隐藏
	}
	else if (sel == 2)
	{
		m_dlg1.ShowWindow(SW_HIDE);//隐藏
		m_dlg2.ShowWindow(SW_HIDE);//隐藏
		m_dlg3.ShowWindow(SW_SHOW);//显示

	}

}


void CDCMDlg::OnBnClickedButton2()
{

	//不使用脚本自带的多开器
	//刷新窗口序号
	//如果有窗口掉线了，重新设置掉线的窗口序号

	CString strHend = g_pEngine->m_pDm->EnumWindowByProcess(_T("qnyh.exe"), _T(""), _T(""), 8 + 16 + 32);
	CStringArray arrHwnd;
	Split(strHend, arrHwnd, _T(","));
	for (int i = 0; i < arrHwnd.GetCount(); i++)
	{
		tagWndSubscript TmptagWnd;
		TmptagWnd.tagTitleSubscript = I2S(i + 1);
		TmptagWnd.tagWndHwnd = arrHwnd[i];

		//遍历之前的数据判断是否为新窗口
		bool 新窗口 = true;
		for (int k = 0; k < g_pEngine->m_arrWndSubscriptIndex.GetCount(); k++)
		{
			if (TmptagWnd.tagWndHwnd == g_pEngine->m_arrWndSubscriptIndex[k].tagWndHwnd)
			{
				新窗口 = false;
				break;
			}
		}

		bool Is替换 = false;
		if (新窗口)
		{
			for (int j = 0; j < g_pEngine->m_arrWndSubscriptIndex.GetCount(); j++)
			{
				int Hwnd = _ttoi(g_pEngine->m_arrWndSubscriptIndex[j].tagWndHwnd);
				if (!IsWindow((HWND)Hwnd))
				{
					//之前的窗口不存在 替换之前的句柄
					g_pEngine->m_pDm->SetWindowText(_ttoi(TmptagWnd.tagWndHwnd), g_pEngine->m_arrWndSubscriptIndex[j].tagTitleSubscript);
					CString tag丢失的句柄 = g_pEngine->m_arrWndSubscriptIndex[j].tagWndHwnd;
					g_pEngine->m_arrWndSubscriptIndex[j].tagWndHwnd = TmptagWnd.tagWndHwnd;
					Is替换 = true;
					BugString(_T("发现之前窗口丢失：")_T(" 句柄：") + tag丢失的句柄 + _T("把新窗口替换给之前的窗口标记。窗户序号：") + g_pEngine->m_arrWndSubscriptIndex[j].tagTitleSubscript + _T(" 新句柄：") + TmptagWnd.tagWndHwnd);
					break;
				}
			}
		}



		if (!Is替换 && 新窗口)
		{
			//遍历没有替换，却是新窗口 正常修改标题，保存标记
			g_pEngine->m_pDm->SetWindowText(_ttoi(TmptagWnd.tagWndHwnd), TmptagWnd.tagTitleSubscript);
			g_pEngine->m_arrWndSubscriptIndex.Add(TmptagWnd);
			BugString(_T("刷新添加窗户序号：") + TmptagWnd.tagTitleSubscript + _T(" 句柄：") + TmptagWnd.tagWndHwnd);
		}

	}




	// TODO: 在此添加控件通知处理程序代码
}



void CDCMDlg::OnBnClickedIsdraw()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_pEngine->tagOnDraw)
	{
		g_pEngine->tagOnDraw = false;
		return;
	}

	g_pEngine->tagOnDraw = true;
}



void CDCMDlg::SetToolMaintenanceMode()
{
	if (m_dlg1.Pattern1.GetCheck() == 1)
	{
		g_pEngine->m_tagToolMaintenanceMode = 1;
	}
	else if (m_dlg1.Pattern2.GetCheck() == 1)
	{
		g_pEngine->m_tagToolMaintenanceMode = 2;
	}
	else if (m_dlg1.Pattern3.GetCheck() == 1)
	{
		g_pEngine->m_tagToolMaintenanceMode = 3;
	}
	else if (m_dlg1.Pattern4.GetCheck() == 1)
	{
		g_pEngine->m_tagToolMaintenanceMode = 4;
	}
	else
	{
		LogD(_T("未选择工具维修方式,默认为：购买新工具"));
		m_dlg1.Pattern2.SetCheck(1);
		g_pEngine->m_tagToolMaintenanceMode = 2;
	}

}

VOID CDCMDlg::GetAllKeyValueOfIniFileOneSection(TCHAR* pIniFilePath, TCHAR* pSectionName, std::map<CString, CString>& mapIniKeyValue)
{
	TCHAR szBuf[4096] = { 0 };
	DWORD readlen = ::GetPrivateProfileSection(pSectionName, szBuf, 4096, pIniFilePath);
	TCHAR* pbuf = szBuf;
	size_t size = _tcslen(pbuf);

	//如果读取到的行，长度不为0时，说明此行存在，继续取值
	while (size)
	{
		CString str = pbuf;
		CString strKey, strValue;
		if (AfxExtractSubString(strKey, (LPCTSTR)str, 0, _T('=')) && AfxExtractSubString(strValue, (LPCTSTR)str, 1, _T('=')))
		{
			if (!mapIniKeyValue.count(strKey))
			{
				mapIniKeyValue.insert(std::make_pair(strKey, strValue));
			}
		}
		pbuf += size + 1;
		size = _tcslen(pbuf);
	}

	return;
}

bool CDCMDlg::iniGetAllKeyValueAllNodeNames(CString strIniFilePath, CString strSectionName, std::map<CString, CString>& mapIniKeyValue)
{
	GetAllKeyValueOfIniFileOneSection(strIniFilePath.GetBuffer(), strSectionName.GetBuffer(), mapIniKeyValue);
	if (mapIniKeyValue.size() != NULL)
		return true;
}


void CDCMDlg::OnBnClickedButtonStart2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strCmd = _T("停止");
	m_btStart.SetWindowText(_T("启动"));
	m_btStart.EnableWindow(TRUE);
}

void CDCMDlg::addStrm_Life_ComboBox()
{
	CString strPath = g_pEngine->m_strWorkPath + _T("\\findMapData.ini");
	std::map<CString, CString> mapCollectionIniKeyValue = {};

	//采集
	mapCollectionIniKeyValue.clear();
	if (iniGetAllKeyValueAllNodeNames(strPath, _T("采集_mapInfo"), mapCollectionIniKeyValue))
	{
		m_dlg2.m_Life_Collection_ComboBox.ResetContent();
		for (auto it = mapCollectionIniKeyValue.cbegin(); it != mapCollectionIniKeyValue.cend(); it++)
		{
			m_dlg2.m_Life_Collection_ComboBox.AddString(it->first);
			m_dlg2.m_Life_Collection_ComboBox.SetCurSel(0);
		}
	}
	//钓鱼
	mapCollectionIniKeyValue.clear();
	if (iniGetAllKeyValueAllNodeNames(strPath, _T("钓鱼_mapInfo"), mapCollectionIniKeyValue))
	{
		m_dlg2.m_Life_Fish_ComboBox.ResetContent();
		for (auto it = mapCollectionIniKeyValue.cbegin(); it != mapCollectionIniKeyValue.cend(); it++)
		{
			m_dlg2.m_Life_Fish_ComboBox.AddString(it->first);
			m_dlg2.m_Life_Fish_ComboBox.SetCurSel(0);
		}
	}
}

//写UI信息配置
void CDCMDlg::writeUiConfig()
{
	CString strText;
	m_dlg3.m_ByPostNameControl.GetWindowText(strText);
	if (strText != _T("")) {
		g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("ByPostNameControl"), strText);
	}

	//m_dlg1 Ui
	if (m_dlg1.Pattern1.GetCheck() == 1) {
		g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern1"),_T("1"));
	}
	if (m_dlg1.Pattern2.GetCheck() == 1) {
		g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern2"), _T("1"));
	}
	if (m_dlg1.Pattern3.GetCheck() == 1) {
		g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern3"), _T("1"));
	}
	if (m_dlg1.Pattern4.GetCheck() == 1) {
		g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Patter4"), _T("1"));
	}
	if (m_dlg1.Wushigongji.GetCheck() == 1) {
		g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_CHECK1"), _T("1"));
	}

	//dlg Ui 已选任务
	CString strTaskName;
	for (int i = 0; i < m_lstTaskRun.GetItemCount(); i++) {
		strTaskName = strTaskName + m_lstTaskRun.GetItemText(i, 0) + _T("|");
	}
	g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("m_lstTaskRun"), strTaskName);

	//dlg2 采集地图与钓鱼地图
	int index = m_dlg2.m_Life_Collection_ComboBox.GetCurSel();
	g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("m_Life_Collection_ComboBox"), ITostr(index));

	index = m_dlg2.m_Life_Fish_ComboBox.GetCurSel();
	g_pEngine->WriteTeamIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("m_Life_Fish_ComboBox"), ITostr(index));


}

//读UI信息配置
void CDCMDlg::readUiConfig()
{
	CString retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("ByPostNameControl"), true);
	if (retStr != _T("")) {
		m_dlg3.m_ByPostNameControl.SetWindowText(retStr);
	}

	//m_dlg1 Ui
	retStr = _T("");
	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern1"), true);
	if (retStr != _T("")) {
		m_dlg1.Pattern1.SetCheck(1);
	}
	retStr = _T("");
	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern2"), true);
	if (retStr != _T("")) {
		m_dlg1.Pattern2.SetCheck(1);
	}
	retStr = _T("");
	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern3"), true);
	if (retStr != _T("")) {
		m_dlg1.Pattern3.SetCheck(1);
	}
	retStr = _T("");
	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("IDC_Pattern4"), true);
	if (retStr != _T("")) {
		m_dlg1.Pattern4.SetCheck(1);
	}

	//dlg Ui 已选任务
	retStr = _T("");
	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("m_lstTaskRun"), true);
	CStringArray arrTask;
	Split(retStr, arrTask, _T("|"));
	for (int i = 0; i < arrTask.GetCount(); i++){
		int iRow = m_lstTaskRun.GetItemCount();
		m_lstTaskRun.InsertItem(iRow, _T(""));
		m_lstTaskRun.SetItemText(i, 0, arrTask[i]);
	}

	//dlg2 采集地图与钓鱼地图
	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("m_Life_Collection_ComboBox"), true);
	m_dlg2.m_Life_Collection_ComboBox.SetCurSel(_ttoi(retStr));

	retStr = g_pEngine->GetTeamiIni(g_pEngine->m_strWorkPath + _T("\\全局配置.ini"), _T("UI配置"), _T("m_Life_Fish_ComboBox"), true);
	m_dlg2.m_Life_Fish_ComboBox.SetCurSel(_ttoi(retStr));

}


void CDCMDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	writeUiConfig();
}
