// SignDlg.cpp: 实现文件
//

#include "pch.h"
#include "DCM.h"
#include "afxdialogex.h"
#include "SignDlg.h"
#include "DCMDlg.h"
#include "NetVerHelp/Help/NetworkVerHelp.h"
#include "NetVerHelp/Help/OtherHelpTools.h"
#define UM_ThreadOpertUI    WM_USER+100


void SignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SaveKey, m_ChekSaveKeyControl);
}


BEGIN_MESSAGE_MAP(SignDlg, CDialogEx)
	ON_BN_CLICKED(LoginBtn, &SignDlg::OnBnClickedLoginbtn)
	ON_MESSAGE(UM_ThreadOpertUI, &SignDlg::ThreadOpertUI)//注册线程操作UI
END_MESSAGE_MAP()



//当前软件版本号
static string versionname = "1.2";
//机器码
static string maccode = Help::NetworkVerHelp::GetMac();
//登录成功后的token令牌
static string loginToken;
//登录的卡密
static string cardnum;
//卡密到期时间 
static string cardEndTime;
//心跳key
static string heartbeatkey;
//选择的价格类型ID
string priceid = "";

//此卡密的权限
string CardJurisdictvalue;

//您自己程序的窗口
CDCMDlg dlg;

#pragma region 线程操作主界面UI控件
//线程操作主界面UI控件
LRESULT SignDlg::ThreadOpertUI(WPARAM WParam, LPARAM LParam)
{
	VMProtectBegin("GetIniSoftInfo");
	int flag = (int)WParam;
	switch (flag)
	{
	case 1://初始化软件
		if (LParam) {
			GetDlgItem(LoginBtn)->EnableWindow(LParam);
			GetDlgItem(MsgLbl)->ShowWindow(FALSE);

		}
		break;
	case 2://登录失败
		GetDlgItem(LoginBtn)->EnableWindow(true);
		break;
	case 3:
		//跳转登录成功的界面
		AfxGetApp()->GetMainWnd()->ShowWindow(SW_HIDE);
		dlg.setCardInfo(loginToken, cardnum, cardEndTime, CardJurisdictvalue);
		dlg.DoModal();
		break;
	}
	return 1;
	VMProtectEnd();
}
#pragma endregion

#pragma region 初始化软件
//初始化软件
bool iniSoftInfo()
{
	VMProtectBegin("GetIniSoftInfo");


	bool iniResult = false;
	//平台类型  1:网络验证平台  2:自动发货平台
	Model::PlatformType platformtypeid = Model::PlatformType::NetVer;

	//加密的key；注意：如果软件设置了DES加密通讯那么此值必填(此参数是在软件列表里面进行设置)
	string encryptKey = "ce01e7e5";

	//签名盐(此参数是在软件列表里面进行设置);注意：如果软件设置了加密通讯，那么此值必填
	string signSalt = "a62898d0";

	//请求接口时，业务数据data参数加密码通讯类型(此参数是在软件列表里面进行设置)
	Model::EncryptType encrypttypeid = Model::EncryptType::RC4;

	//软件管理-->软件列表,里面找到相对应的softcode,填写到此处
	string softcode = "50d1911d23de874a";

	// 个人中心里面可以查看得到。代理商的话，那么在：代理管理--》代理商列表，可以查看得到；
	// 注意：如果是作者自己就填写自己的platformUserCode,如果是代理商的，得填写代理商的Code
	string platformUserCode = "7255baff6eab95bc";

	//构建初始化软件入参
	Model::In_IniSoftInfoArgs IniInfoArgs;
	IniInfoArgs.maccode = maccode;//必填
	IniInfoArgs.timestamp = Help::NetworkVerHelp::GetTimeStamp();//必填
	IniInfoArgs.requestflag = to_string(IniInfoArgs.timestamp);//必填
	IniInfoArgs.versionname = versionname;//必填

	//请求接口，获取初始化软件结果
	Model::Result result = Help::NetworkVerHelp::GetIniSoftInfo(IniInfoArgs, platformtypeid, encryptKey, signSalt, encrypttypeid, softcode, platformUserCode);
	if (result.code == 0)//初始软件成功
	{
		//iniSoftInfoData:初始化软件的具体数据
		if (Help::iniSoftInfoData.inisoftkey != "")
		{
			if (Help::iniSoftInfoData.requestflag != IniInfoArgs.requestflag)
			{
				iniResult = false;
				AfxMessageBox(_T("接口返回的数据已被“破解者”截持，您可以强制关闭软件或者不做任何处理"));
				OtherHelp::OtherHelpTools::TerminateApp();
				return iniResult;
			}

			//设置软件标题
			//string softname = Help::iniSoftInfoData.softInfo.softname;
			string softname = "白茶1.2";
			std::wstring softnameTemp = OtherHelp::OtherHelpTools::StoWs(softname);
			SetWindowText(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), softnameTemp.c_str());

			//公告
			CString strTmp;
			strTmp.Format(_T("%s"), CStringW(Help::iniSoftInfoData.softInfo.notice.c_str()));
			SetDlgItemText(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),IDC_AnnouncementControl, strTmp);

			
			if (Help::iniSoftInfoData.softInfo.newversionnum != "")
			{
				if (Help::iniSoftInfoData.softInfo.newversionnum != versionname)
				{
					string versionMsg = "发现新版本，新版本下载地址：\r\n";
					versionMsg = versionMsg + Help::iniSoftInfoData.softInfo.networkdiskurl +"\r\n";
					versionMsg = versionMsg + "   提取码：" + Help::iniSoftInfoData.softInfo.diskpwd + "\r\n";
					versionMsg = versionMsg + "更新内容------------------------" + "\r\n";
					versionMsg = versionMsg + Help::iniSoftInfoData.softInfo.notice + "\r\n";
					strTmp.Format(_T("%s"), CStringW(versionMsg.c_str()));
					SetDlgItemText(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), IDC_AnnouncementControl, strTmp);

					if (Help::iniSoftInfoData.softInfo.isforceupd == 1)//强制更新
					{
						MessageBoxW(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), L"强制更新", NULL, NULL);

						iniResult = false;
						//如果是强制更新的话，那么就杀死本程序，自动退出
						OtherHelp::OtherHelpTools::TerminateApp();
					}
				}
				else
				{
					string versionMsg1 = "无需更新：\r\n";
					strTmp.Format(_T("%s"), CStringW(versionMsg1.c_str()));
					SetDlgItemText(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), IDC_AnnouncementControl, strTmp);

				}
			}

			iniResult = true;
		}
	}
	else
	{
		//初始软件失败
		MessageBoxW(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), OtherHelp::OtherHelpTools::StoWs(result.msg).c_str(), NULL, NULL);
		OtherHelp::OtherHelpTools::TerminateApp();
	}

	return iniResult;

	VMProtectEnd();
}
#pragma endregion

#pragma region 退出登录
//退出登录
void LoginOut()
{
	if (loginToken != "")
	{
		Model::In_LoginOutArgs LoginOutArgs;
		LoginOutArgs.maccode = maccode;//必填
		LoginOutArgs.timestamp = Help::NetworkVerHelp::GetTimeStamp();//必填
		LoginOutArgs.cardnumorusername = cardnum;
		LoginOutArgs.token = loginToken;
		Model::Result LoginOutResult = Help::NetworkVerHelp::GeLoginOut(LoginOutArgs);
	}
}
#pragma endregion

#pragma region 心跳
//心跳
void HeartBeat()
{
	VMProtectBegin("GetIniSoftInfo");

	//心跳那一块，你得加容错机制，心跳正常的操作如下：
	//错误失败次数 = 0;
	//while (true) {
	//	心跳结果 = 请求瑞科心跳接口(心跳参数);
	//	if (心跳结果.code != 0) {
	//		if (心跳结果.code == -999) {
	//			错误失败次数 = 错误失败次数 + 1;
	//		}
	//		else {
	//			强制关闭软件
	//		}
	//	}
	//	else {
	//		错误失败次数 = 0;
	//	}
	//	if (错误失败次数 >= 12) {
	//		//跳出循环啥不做处理
	//		//至余为什么连续失败12直接跳出不做任何处理的原因：
	//		//1、确实是本当前电脑网络有问题
	//		//2、有可能服务器当前有问题
	//		//以上两种情况，不能中断你用户的操作，如果用户确实到期了 
	//		//下次登录的时候反正肯定也是登录不上去的
	//		break;
	//	}
	//	延时(5分钟)
	//}

		//构建心跳入参
	Model::In_HeartBeatArgs HeartBeatArgs;
	HeartBeatArgs.maccode = maccode;//必填
	HeartBeatArgs.timestamp = Help::NetworkVerHelp::GetTimeStamp();//必填
	HeartBeatArgs.requestflag = to_string(HeartBeatArgs.timestamp);//必填
	HeartBeatArgs.cardnumorusername = cardnum;//必填
	HeartBeatArgs.token = loginToken;//必填
	HeartBeatArgs.heartbeatkey = heartbeatkey;//必填(每次把最新的心跳Key赋值)

	string ErrorMsg = "";
	while (true)
	{
		//请求接口，获取登录结果
		Model::Result HeartBeatResult = Help::NetworkVerHelp::GetHeartBeat(HeartBeatArgs);
		if (HeartBeatResult.code != 0)
		{
			//如果接口返回来不是0的结果，可以直接关掉软件
			//不等于0的有哪些错误结果，请查看：https://www.ruikeyz.com/page/InterfaceDoc/heartBeat/heartBeatErrorMsg.html
			ErrorMsg = HeartBeatResult.msg;
			break;
		}
		else
		{
			//心跳成功后的具体数据
			Model::Out_heartBeat heartBeatData = Help::NetworkVerHelp::ConvertHeartBeat(HeartBeatResult.data);
			//这一块相当重要：一定要判断这个“心跳Key(heartbeatkey)”和"请求标识(requestflag)"，防止别人修改你请求的地址，给你返回一个假消息回来
			if (heartBeatData.heartbeatkey != "" && heartBeatData.requestflag == HeartBeatArgs.requestflag)
			{
				HeartBeatArgs.heartbeatkey = heartBeatData.heartbeatkey;//一定下记录下最新的心跳Key,在下次请求心跳接口需要把此最新的心跳Key给带上
			}
			else
			{
				//本程序被不法分子修改了接口返回值，您可以强制关闭软件
				break;
			}
		}
		//心跳间隔时间最小不能小于5分钟
		Sleep(1000 * 60 * 5);
	}

	////特别注意这块：当心跳跳出循环的时候，你自己业务的逻辑代码，不能再执行了。
	////不然心跳结束了，你的业务逻辑代码还在往下执行，那么别人就可能免费使用你的软件了，那么心跳就没起到什么作用了
	dlg.setStopWork();
	LoginOut();
	OtherHelp::OtherHelpTools::TerminateApp();

	if (ErrorMsg != "")
	{

		MessageBoxW(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), OtherHelp::OtherHelpTools::StoWs(ErrorMsg).c_str(), NULL, NULL);
	}

	LoginOut();
	OtherHelp::OtherHelpTools::TerminateApp();

	VMProtectEnd();
}
#pragma endregion



#pragma region 心跳线程
//心跳线程
UINT Thread_HeartBeat(LPVOID pParam) {
	HeartBeat();
	return 0;
}
#pragma endregion

#pragma region 初始化软件线程
//初始化软件线程方法
UINT Thread_iniSoftInfo(LPVOID pParam) {
	VMProtectBegin("GetIniSoftInfo");
	if (iniSoftInfo())
	{
		//判断C盘是否有卡密
		TCHAR szAppPath[MAX_PATH];
		GetModuleFileName(nullptr, szAppPath, MAX_PATH);
		PathRemoveFileSpec(szAppPath);

		// 构建 INI 文件的完整路径
		TCHAR szIniPath[MAX_PATH];
		PathCombine(szIniPath, szAppPath, _T("C:\\Autokey.ini"));

		// 判断 INI 文件是否存在
		if (GetFileAttributes(szIniPath) == INVALID_FILE_ATTRIBUTES) {
			// 如果文件不存在，创建默认设置的 INI 文件
			WritePrivateProfileString(_T("Section"), _T("Key"), _T(""), szIniPath);
		}

		// 读取 INI 文件中的值
		TCHAR szValue[256];
		GetPrivateProfileString(_T("Section"), _T("Key"), _T("DefaultValue"), szValue, 256, szIniPath);
		CString str;
		str.Format(L"%s", szValue);
		if (str != _T("")) {
			SetDlgItemText(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), cardNumTxt, str);
		}

		HWND hwnd = (HWND)pParam;
		PostMessageA(hwnd, UM_ThreadOpertUI, 1, true);
	}
	return 0;
	VMProtectEnd();
}
#pragma endregion

#pragma region 登录线程
//登录线程方法
UINT Thread_login(LPVOID pParam) {
	VMProtectBegin("GetIniSoftInfo");
	cardnum = OtherHelp::OtherHelpTools::PVOIDToString(pParam);

	//构建登录入参
	Model::In_CardLoginArgs args;
	args.maccode = maccode;//必填
	args.timestamp = Help::NetworkVerHelp::GetTimeStamp();//必填
	args.requestflag = to_string(args.timestamp);//必填
	args.cardnum = cardnum;//必填

	//请求接口，获取登录结果
	Model::Result result = Help::NetworkVerHelp::GetLoginByCard(args);
	if (result.code == 0)
	{
		//登录成功后的具体数据
		Model::Out_Login LoginData = Help::NetworkVerHelp::ConvertLoginData(result.data);
		if (LoginData.requestflag != args.requestflag)
		{
			AfxMessageBox(_T("接口返回的数据已被“破解者”截持，您可以强制关闭软件或者不做任何处理"));
			OtherHelp::OtherHelpTools::TerminateApp();
			return 0;
		}

		loginToken = LoginData.token;
		heartbeatkey = LoginData.heartbeatkey;
		cardEndTime = LoginData.endtime;


		if (LoginData.jurisdictionlist.empty() == false && LoginData.jurisdictionlist.size() > 0)
		{
			list<Model::Out_LoginJurisdiction>::iterator it = LoginData.jurisdictionlist.begin();
			while (it != LoginData.jurisdictionlist.end())
			{
				if ((*it).jurisdictionname == "显示功能")
				{
					CardJurisdictvalue = (*it).jurisdictvalue;
					break;
				}
				*it++;
			}
		}

		//开始心跳
		AfxBeginThread(Thread_HeartBeat, AfxGetApp()->GetMainWnd()->GetSafeHwnd());

		//登录成功，弹出您自己的窗口
		HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		PostMessageA(hwnd, UM_ThreadOpertUI, 3, true);
	}
	else
	{
		HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
		//登录失败
		MessageBoxW(hwnd, OtherHelp::OtherHelpTools::StoWs(result.msg).c_str(), NULL, NULL);
		PostMessageA(hwnd, UM_ThreadOpertUI, 2, true);
	}

	return 0;
	VMProtectEnd();
}
#pragma endregion


// LoginVerDemoDlg 对话框
IMPLEMENT_DYNAMIC(SignDlg, CDialogEx)

SignDlg::SignDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SignDlg, pParent)
{

}

BOOL SignDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog(); //开始

	//设置默认控件状态
	m_ChekSaveKeyControl.SetCheck(1);
	//初始化软件
	AfxBeginThread(Thread_iniSoftInfo, this->m_hWnd);

	//

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

SignDlg::~SignDlg()
{
}




// LoginVerDemoDlg 消息处理程序

#pragma region 登录按钮被点击
//登录按钮被点击
void SignDlg::OnBnClickedLoginbtn()
{
	VMProtectBegin("GetIniSoftInfo");
	VMProtectEnd();
	//获取文本框的卡密
	CString cardNum;
	GetDlgItemText(cardNumTxt, cardNum);
	if (cardNum.IsEmpty())
	{
		AfxMessageBox(_T("卡密不能为空"));
		return;
	}
	GetDlgItem(LoginBtn)->EnableWindow(false);
	if (m_ChekSaveKeyControl.GetCheck() == 1) {
		TCHAR szAppPath[MAX_PATH];
		GetModuleFileName(nullptr, szAppPath, MAX_PATH);
		PathRemoveFileSpec(szAppPath);

		// 构建 INI 文件的完整路径
		TCHAR szIniPath[MAX_PATH];
		PathCombine(szIniPath, szAppPath, _T("C:\\Autokey.ini"));

		// 判断 INI 文件是否存在
		if (GetFileAttributes(szIniPath) == INVALID_FILE_ATTRIBUTES) {
			// 如果文件不存在，创建默认设置的 INI 文件
			WritePrivateProfileString(_T("Section"), _T("Key"), cardNum, szIniPath);
		}
		else
		{
			WritePrivateProfileString(_T("Section"), _T("Key"), cardNum, szIniPath);
		}
	}

	USES_CONVERSION;
	char* cardNumTemp = W2A(cardNum);
	//调用线程处理登录
	AfxBeginThread(Thread_login, (LPVOID)cardNumTemp);

	VMProtectEnd();
}
#pragma endregion



