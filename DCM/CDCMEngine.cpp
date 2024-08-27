#include "pch.h"
#include "CDCMEngine.h"
#include "CWndManager.h"
#include "CTaskManager.h"
#include "CTaskThread.h"
#include "CTaskScript.h"
#include "CHttpSrvThread.h"
#include "locale.h"
#include "CAiTool.h"
#include "CYolo7.h"
#include "obj.h"
#include"CLDScan.h"
#include "CKeyMouse.h"
#include"CReadConfigIni.h"
#include <fstream>
#include <filesystem>

CDCMEngine::CDCMEngine()
{
}

CDCMEngine::~CDCMEngine()
{
	//停止任务线程 释放任务信息数组
	for (int i = 0; i < m_arrTask.GetCount(); i++)
	{
		tagTaskInfo* pInfo = m_arrTask[i];

		if (pInfo->pTaskThread)
			pInfo->pTaskThread->StopThread();

		if (pInfo->pTaskHelperThread)
			pInfo->pTaskHelperThread->StopThread();

		SAFE_DELETE(pInfo->pStatus);
		SAFE_DELETE(pInfo->pTaskThread);
		SAFE_DELETE(pInfo->pTaskHelperThread);
		SAFE_DELETE(pInfo);
	}
	m_arrTask.RemoveAll();

	//释放窗口信息数组
	for (int i = 0; i < m_arrWnd.GetCount(); i++)
	{
		tagWndInfo* pInfo = m_arrWnd[i];
		SAFE_DELETE(pInfo);
	}
	m_arrWnd.RemoveAll();

	//释放资源信息数组
	for (int i = 0; i < m_arrRes.GetCount(); i++)
	{
		tagResItem* pInfo = m_arrRes[i];

		if (!pInfo->matPic.empty())
			pInfo->matPic.release();

		SAFE_DELETE(pInfo);
	}
	m_arrRes.RemoveAll();

	SAFE_DELETE(m_pWndMgr);
	SAFE_DELETE(m_pTaskMgr);

	for (int i = 0; i < m_arrGroup.GetCount(); i++)
	{
		tagGroupUser* pInfo = m_arrGroup[i];
		SAFE_DELETE(pInfo);
	}
	m_arrGroup.RemoveAll();

	SAFE_DELETE(m_pHttpSrv);
}

void CDCMEngine::ReadWndIni()
{


	CWHIniData ini;
	ini.SetIniFilePath(m_strWorkPath + _T("/全局配置.ini"));

	m_WndIni.strProc = ini.ReadString(_T("窗口"), _T("进程"));
	m_WndIni.strTitle = ini.ReadString(_T("窗口"), _T("标题"));
	m_WndIni.strClz = ini.ReadString(_T("窗口"), _T("类名"));

	CString strTmp;
	strTmp = ini.ReadString(_T("窗口"), _T("窗口尺寸"));

	CStringArray arrTmp;
	Split(strTmp, arrTmp, _T(","));
	if (arrTmp.GetCount() == 2)
	{
		m_iWidth = _ttoi(arrTmp[0]);
		m_iHeight = _ttoi(arrTmp[1]);
	}

	m_WndIni.strLDPath = ini.ReadString(_T("窗口"), _T("雷电路径"));
	m_WndIni.strDisplay = ini.ReadString(_T("窗口"), _T("display"));
	m_WndIni.strMouse = ini.ReadString(_T("窗口"), _T("mouse"));
	m_WndIni.strKeypad = ini.ReadString(_T("窗口"), _T("keypad"));
	m_WndIni.strPublic = ini.ReadString(_T("窗口"), _T("public"));
	m_WndIni.iMode = ini.ReadInt(_T("窗口"), _T("mode"), 0);

	CString strSwitch = ini.ReadString(_T("窗口"), _T("切换配置"));
	Split(strSwitch, m_WndIni.arrSwitch, _T(";"));

	m_WndIni.本地端口 = ini.ReadInt(_T("窗口"), _T("本地端口"), 0);
	m_WndIni.组队配置 = ini.ReadString(_T("窗口"), _T("组队配置"));

	m_WndIni.任务列表 = ini.ReadString(_T("窗口"), _T("任务列表"));

	m_WndIni.账号配置 = ini.ReadString(_T("窗口"), _T("账号配置"));
	m_WndIni.雷电共享目录 = ini.ReadString(_T("窗口"), _T("雷电共享目录"));



	LogN(_T("窗口配置文件读取完成"));
}

void CDCMEngine::Init()
{
	TCHAR szPath[256] = { 0 };
	CWHService::GetWorkDirectory(szPath, 256);
	m_strWorkPath = szPath;
	m_strWorkPath += _T("\\..\\fig");

	ReadWndIni();
	LoadRes();

	LoadAccountInfo();

	m_pMainDlg = AfxGetMainWnd();
	m_pWndMgr = new CWndManager();
	m_pTaskMgr = new CTaskManager();
	m_pHttpSrv = new CHttpSrvThread(m_WndIni.本地端口);
	m_pHttpSrv->StartThread();
	//Yolo7初始化
	CAiTool& ocrTool = CAiTool::get_instance();
	ocrTool.Create();
	CYolo7& yolo7 = CYolo7::get_instance();
	std::string strPath = CW2CA(m_strWorkPath);
	yolo7.SetParam((strPath + "/cs.param").c_str(), (strPath + "/cs.bin").c_str(), "292", "306", m_iWidth, m_iHeight);
	m_CKeyMouse = new CKeyMouse(m_strWorkPath + _T("/Classkey.dll"));
	m_ReadConfigIni = new CReadConfigIni();
	m_ReadConfigIni->Init();
	m_pDm = new dmsoft();

	LogN(_T("ai的共享内存初始化完成"));
}

tagWndInfo* CDCMEngine::GetWndList(int Id)
{
	return m_pWndMgr->GetWndList(Id);
}

int CDCMEngine::GetLDList()
{
	return m_pWndMgr->GetLDList();
}

void CDCMEngine::Start(tagGameAcconutInfo* GameAcconutInfo)
{
	tagWndInfo* pInfo;
	int iTme = 5;
	while (iTme--)
	{
		//如果脚本开启时 游戏存在则不写入本地LogDini时间(因为不确定当前运行的是哪个账号)，
		// 但是临时占用第一行账号的信息，当本轮逻辑任务运行完毕后 脚本会重新登入第一行账号检查是否完成的
		pInfo = GetWndList(GameAcconutInfo->Id);
		if (pInfo->hWnd != NULL) {
			pInfo->IsWriteIniLogd = false;
		}
		else{
			delete pInfo;
			pInfo = NULL;
			pInfo = m_pWndMgr->OpenPcGame(GameAcconutInfo);
			if (pInfo->hWnd) {
				pInfo->IsWriteIniLogd = true;
			}
			else{
				continue;
			}
			
		}

		if (pInfo->hWnd) {
			pInfo->id = GameAcconutInfo->Id;
			m_pTaskMgr->Start(pInfo, GameAcconutInfo);
			break;
		}

	}
}

void CDCMEngine::Stop(int id)
{
	m_pTaskMgr->Stop(id);
}

void CDCMEngine::Suspend(int id)
{
	m_pTaskMgr->Suspend(id);
}

void CDCMEngine::Resume(int id)
{
	m_pTaskMgr->Resume(id);
}

void CDCMEngine::Restart(int id, CStringArray& arrTask)
{
	//Stop(id);

	//m_pWndMgr->CloseWnd(id);

	//int i = 0;
	//while (i++ < 60)
	//{
	//	LogN(_T("CDCMEngine: 窗口:%d 正在关闭,等待时间:%d秒"), id, i * 10);

	//	Sleep(10 * 1000);

	//	if (!IsWndReady(id))
	//		break;
	//}

	//Start(id);
}

void CDCMEngine::Switch(int id, int dstId, CStringArray& arrTask)
{
	//Stop(id);

	//m_pWndMgr->CloseWnd(id);

	//int i = 0;
	//while (i++ < 60)
	//{
	//	LogN(_T("CDCMEngine: 窗口:%d 正在关闭,等待时间:%d秒"), id, i * 10);

	//	Sleep(10 * 1000);

	//	if (!IsWndReady(id))
	//		break;
	//}

	//Start(dstId);
}

void CDCMEngine::KillApp(int id)
{
	m_pWndMgr->KillApp(id);
}

bool CDCMEngine::LoadRes()
{
	CString strResFile = m_strWorkPath + _T("/res/res.txt");

	if (!PathFileExists(strResFile))
	{
		LogE(_T("res.txt文件不存在"));
		return false;
	}

	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile file;

	if (!file.Open(strResFile, CFile::modeRead))
	{
		LogE(_T("res.txt文件打开失败"));
		return false;
	}
	


	CString strRead = _T("");
	while (file.ReadString(strRead))
	{
		if (strRead.GetLength() < 10)
			continue;

		if (strRead.GetAt(0) == _T('#'))
			continue;

		CStringArray arrTmp;
		Split(strRead, arrTmp, _T("="));
		if (arrTmp.GetCount() != 2)
		{
			LogE(_T("内容无法解析:%s"), strRead);

			ASSERT(FALSE);
			return false;
		}

		CString strName = arrTmp[0];

		//判断当前项是否已经存在
		bool bExist = false;
		for (int i = 0; i < m_arrRes.GetCount(); i++)
		{
			if (m_arrRes[i]->strName == strName)
			{
				bExist = true;
				break;
			}
		}

		if (bExist)
		{
			ASSERT(FALSE);
			continue;
		}

		CString strValue = arrTmp[1];
		arrTmp.RemoveAll();
		Split(strValue, arrTmp, _T(">"));
		if (arrTmp.GetCount() != 6)
		{
			LogE(_T("内容无法解析:%s"), strRead);

			ASSERT(FALSE);
			return false;
		}

		CStringArray arrRect;
		Split(arrTmp[0], arrRect, _T(","));
		if (arrRect.GetCount() != 4)
		{
			LogE(_T("内容无法解析:%s"), strRead);

			ASSERT(FALSE);

			file.Close();
			return false;
		}

		CString strRes = arrTmp[1];
		CString strDelclr = arrTmp[2];
		CString strSim = arrTmp[3];
		CString strDir = arrTmp[4];
		CString strType = arrTmp[5];

		tagResItem* pItem = new tagResItem;
		pItem->strName = strName;
		pItem->rtArea = CRect(_ttoi(arrRect[0]), _ttoi(arrRect[1]), _ttoi(arrRect[2]), _ttoi(arrRect[3]));

		//ASSERT(pItem->rtArea.left >= 0 && pItem->rtArea.left <= pItem->rtArea.right);
		//ASSERT(pItem->rtArea.right <= m_iWidth);
		//ASSERT(pItem->rtArea.top >= 0 && pItem->rtArea.top <= pItem->rtArea.bottom);
		//ASSERT(pItem->rtArea.bottom <= m_iHeight);

		pItem->strRes = strRes;
		pItem->strDeltaClr = strDelclr;
		pItem->dSim = _ttof(strSim);
		pItem->iDir = _ttoi(strDir);

		if (strType == _T("找图"))
		{
			pItem->iType = 0;
		}
		else if (strType == _T("找色"))
		{
			pItem->iType = 1;
		}
		else if (strType == _T("找字"))
		{
			pItem->iType = 2;
		}
		else if (strType == _T("字典"))
		{
			pItem->iType = 3;
		}
		else if (strType == _T("识字"))
		{
			pItem->iType = 4;
		}
		else if (strType == _T("位置"))
		{
			pItem->iType = 5;
		}
#ifdef OPENCV_NEED
		else if (strType == _T("CV"))
		{
			pItem->iType = 6;

			CString strPath = m_strWorkPath + _T("/res/") + pItem->strRes;
			std::string strPathA = CT2A(strPath.GetString());

			// 读取24位图像
			cv::Mat image_24bit = cv::imread(strPathA);

			//// 确保图像不为空
			//if (!image_24bit.empty()) {
			//	// 创建一个空的32位图像
			//	cv::Mat image_32bit(image_24bit.rows, image_24bit.cols, CV_8UC4);

			//	// 将24位图像的通道复制到32位图像的前三个通道
			//	cv::cvtColor(image_24bit, image_32bit, cv::COLOR_BGR2BGRA);

			//	// 保存32位图像
			//	cv::imwrite(strPathA, image_32bit);

			//}
			//else {
			//	std::cout << "Error: Image not found." << std::endl;
			//}

			//pItem->matPic = cv::imread(strPathA, cv::IMREAD_UNCHANGED);

			//	// 读取32位图像
			//cv::Mat image32bit = cv::imread(strPathA,cv::IMREAD_UNCHANGED);

			//// 如果需要，将32位图像转换为24位（8位）图像
			//cv::Mat image24bit;
			//if (image32bit.channels() == 4) {
			//	cv::cvtColor(image32bit, image24bit, cv::COLOR_BGRA2BGR);
			//}
			//else {
			//	image24bit = image32bit;
			//}

			//// 保存24位图像
			//CString strPath24 =_T("D:\\Data\\转换24位图\\") + pItem->strRes;
			//std::string strPathA24 = CT2A(strPath24.GetString());

			//cv::imwrite(strPathA24, image24bit);





				// 确保图像不为空
			if (!image_24bit.empty()) {
				// 创建一个空的32位图像
				cv::Mat image_32bit(image_24bit.rows, image_24bit.cols, CV_8UC4);

				// 将24位图像的通道复制到32位图像的前三个通道
				cv::cvtColor(image_24bit, image_32bit, cv::COLOR_BGR2BGRA);

				// 保存32位图像
				//cv::imwrite(strPathA, image_32bit);
				pItem->matPic = image_32bit;
			}
			else {
				std::cout << "Error: Image not found." << std::endl;
			}


		}
#endif
		else if (strType == _T("Ocr"))
		{
			pItem->iType = 7;
		}


		m_arrRes.Add(pItem);
	}

	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

	file.Close();

	LogN(_T("资源文件读取完成"));

	return true;
}

tagResItem* CDCMEngine::GetResItem(CString strName)
{
	for (int i = 0; i < m_arrRes.GetCount(); i++)
	{
		if (m_arrRes[i]->strName == strName)
			return m_arrRes[i];
	}

	return NULL;
}

bool CDCMEngine::IsWndReady(int id)
{
	for (int i = 0; i < m_arrWnd.GetCount(); i++)
	{
		tagWndInfo* pInfo = m_arrWnd[i];
		if (pInfo->id == id)
		{
			if (pInfo->hWnd != NULL && IsWindow(pInfo->hWnd))
				return true;
		}
	}
	return FALSE;
}

tagTaskInfo* CDCMEngine::GetTaskInfo(int id)
{
	for (int i = 0; i < m_arrTask.GetCount(); i++)
	{
		tagTaskInfo* pInfo = m_arrTask[i];
		if (pInfo->id == id)
			return pInfo;
	}

	return NULL;
}

void CDCMEngine::AddTask(int id, CStringArray& arrTask)
{
	if (m_pTaskMgr)
		return m_pTaskMgr->AddTask(id, arrTask);
}

void CDCMEngine::GetAccount(int id, CString& strAcc, CString& strPwd)
{
	CStringArray arrAccount;
	Split(m_WndIni.账号配置, arrAccount, _T("-"));

	for (int i = 0; i < arrAccount.GetCount(); i++){
		CStringArray arrTmp;
		Split(arrAccount[i], arrTmp, _T(","));

		if (arrTmp.GetCount() == 3){
			if (id == _ttoi(arrTmp[0]))
			{
				strAcc = arrTmp[1];
				strPwd = arrTmp[2];
			}
		}
		else {ASSERT(FALSE);}
	}
}

tagGameStatus* CDCMEngine::GetGameStatus(int id)
{
	for (int i = 0; i < m_arrTask.GetCount(); i++)
	{
		tagTaskInfo* pInfo = m_arrTask[i];
		if (pInfo->id == id)
			return pInfo->pStatus;
	}

	return NULL;
}

CString CDCMEngine::获取组队身份(int id)
{
	//组长|组员-自己id-组长id-组长地址
	CStringArray arrItem;
	Split(m_WndIni.组队配置, arrItem, _T(";"));

	for (int i = 0; i < arrItem.GetCount(); i++)
	{
		CStringArray arrTmp;
		Split(arrItem[i], arrTmp, _T("-"));
		CString strRole = arrTmp[0];
		int selfId = _ttoi(arrTmp[1]);
		int leaderId = _ttoi(arrTmp[2]);
		CString leaderUrl = arrTmp[3];

		if (selfId == id)
			return strRole;
	}

	return _T("");
}

CString CDCMEngine::获取组长地址(int id)
{
	//组长|组员-自己id-组长id-组长地址
	CStringArray arrItem;
	Split(m_WndIni.组队配置, arrItem, _T(";"));

	for (int i = 0; i < arrItem.GetCount(); i++)
	{
		CStringArray arrTmp;
		Split(arrItem[i], arrTmp, _T("-"));
		CString strRole = arrTmp[0];
		int selfId = _ttoi(arrTmp[1]);
		int leaderId = _ttoi(arrTmp[2]);
		CString leaderUrl = arrTmp[3];

		if (selfId == id)
			return leaderUrl;
	}

	return _T("");
}

int CDCMEngine::获取组长ID(int id)
{
	//组长|组员-自己id-组长id-组长地址
	CStringArray arrItem;
	Split(m_WndIni.组队配置, arrItem, _T(";"));

	for (int i = 0; i < arrItem.GetCount(); i++)
	{
		CStringArray arrTmp;
		Split(arrItem[i], arrTmp, _T("-"));
		CString strRole = arrTmp[0];
		int selfId = _ttoi(arrTmp[1]);
		int leaderId = _ttoi(arrTmp[2]);
		CString leaderUrl = arrTmp[3];

		if (selfId == id)
			return leaderId;
	}

	return 0;
}

bool CreateTxtFile(const wchar_t* filePath) {
	HANDLE hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return true; // 文件创建成功
	}
	else {
		DWORD error = GetLastError();
		// 处理错误，例如输出错误信息
		return false; // 文件创建失败
	}
}

bool compareFiles(const std::string& file1, const std::string& file2) {
	std::ifstream f1(file1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(file2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		return false; // 文件问题
	}

	if (f1.tellg() != f2.tellg()) {
		return false; // 大小不匹配
	}

	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);

	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
		std::istreambuf_iterator<char>(),
		std::istreambuf_iterator<char>(f2.rdbuf()));
}
void replaceFileWin32(const wchar_t* sourcePath, const wchar_t* targetPath) {
	DeleteFile(targetPath);
	if (MoveFile(sourcePath, targetPath)) {
		wprintf(L"文件已成功替换：%s\n", targetPath);
	}
	else {
		DWORD error = GetLastError();
		wprintf(L"替换文件时出现错误：%d\n", error);
	}
}
void CopyFileWin32(const wchar_t* sourcePath, const wchar_t* targetPath) {
	DeleteFile(targetPath);
	if (CopyFile(sourcePath, targetPath, FALSE)) {
		wprintf(L"文件已成功复制：%s\n", targetPath);
	}
	else {
		DWORD error = GetLastError();
		wprintf(L"复制文件时出现错误：%d\n", error);
	}
}

void CDCMEngine::CopyGamePath() {

	CString strResFile = m_strWorkPath + _T("/GamePath.txt");
	//校验账号配置文件
	if (!PathFileExists(strResFile)) {
		LogD(_T("GamePath.txt does not exist"));
		if (!PathFileExists(_T("C:/GamePath.txt"))) {
			LogD(_T("无游戏启动器路径文件信息,不清楚的看教程"));
		}
	}

	CStdioFile IsFile;
	do
	{
		if (IsFile.Open(strResFile, CFile::modeRead)) {
			// 检查运行目录GameAccount文件是否为空
			bool isEmpty = IsFile.GetLength() == 0;

			if (isEmpty) {
				IsFile.Close();
				CopyFileWin32(_T("C:\\GamePath.txt"), m_strWorkPath + _T("\\GamePath.txt"));
				break;
			}

			CString strTmp, str;
			while (IsFile.ReadString(strTmp)) {
				str = str + strTmp;
			}

			if (str == _T("")) {
				CopyFileWin32(_T("C:\\GamePath.txt"), m_strWorkPath + _T("\\GamePath.txt"));
			}

			IsFile.Close();
		}

	} while (false);


	//复制
	std::string strPathA = CT2A(strResFile.GetString());
	if (!compareFiles(strPathA, "C:\\GamePath.txt")) {
		CopyFileWin32(m_strWorkPath + _T("\\GamePath.txt"), _T("C:\\GamePath.txt"));
	}

}

bool CDCMEngine::LoadAccountInfo()
{

	CString strResFile = m_strWorkPath + _T("/GameAccount.ini");
	//校验账号配置文件
	if (!PathFileExists(strResFile)){
		LogD(_T("GameAccount.ini does not exist"));
		if (!PathFileExists(_T("C:/GameAccount.ini"))) {
			LogD(_T("无账号文件信息，请完成账号配置,不清楚的看教程"));
			return false;
		}
	}

	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");
	CStdioFile IsFile;
	do
	{
		if (IsFile.Open(strResFile, CFile::modeRead)) {
			// 检查运行目录GameAccount文件是否为空

			bool isEmpty = IsFile.GetLength() == 0;

			if (isEmpty) {
				IsFile.Close();
				CopyFileWin32(_T("C:\\GameAccount.ini"), m_strWorkPath + _T("\\GameAccount.ini"));
				break;
			}

			CString strTmp, str;
			while (IsFile.ReadString(strTmp)) {
				str = str + strTmp;
			}

			if (str == _T("")) {
				CopyFileWin32(_T("C:\\GameAccount.ini"), m_strWorkPath + _T("\\GameAccount.ini"));
			}

		}
	} while (false);
	IsFile.Close();

	//复制
	std::string strPathA = CT2A(strResFile.GetString());
	if (!compareFiles(strPathA, "C:\\GameAccount.ini")) {
		CopyFileWin32(m_strWorkPath + _T("\\GameAccount.ini"), _T("C:\\GameAccount.ini"));
	}



	CStdioFile file;

	if (!file.Open(strResFile, CFile::modeRead)){
		LogD(_T("GameAccount.ini open failed"));
		return false;
	}

	CString strTmp;

	m_arrAccount.RemoveAll();
	while (file.ReadString(strTmp)){
		if (strTmp.GetLength() < 10)
			continue;

		if (strTmp.GetAt(0) == _T('#'))
			continue;

		CStringArray arrAccountTmp;
		Split(strTmp, arrAccountTmp, _T("|"));

		if (arrAccountTmp.GetCount() < 3){
			LogD(_T("账号格式无法解析"));
			ASSERT(false);
			continue;
		}

		tagGameAcconutInfo* pAccount = new tagGameAcconutInfo;
		pAccount->Id = _ttoi(arrAccountTmp[0]);
		pAccount->strAcconut = arrAccountTmp[1];
		pAccount->strPassword = arrAccountTmp[2];
		pAccount->strRegion = arrAccountTmp[3];

		CString strAccountLogD = GetTeamiIni(m_strWorkPath + _T("\\GameAccountLogD.ini"), _T("完成时间"), pAccount->strAcconut, true);
		CString str;
		CTime tm;
		tm = CTime::GetCurrentTime();
		str = tm.Format(_T("%Y年%m月%d日"));
		if (strAccountLogD == _T("")|| strAccountLogD != str) {
			pAccount->iOrNot = false;
		}
		else{
			pAccount->iOrNot = true;
		}



		LogD(_T("编号：%d，账号：%s，密码：%s，区：%s"),
			_ttoi(arrAccountTmp[0]),
			arrAccountTmp[1],
			arrAccountTmp[2],
			arrAccountTmp[3]);
		m_arrAccount.Add(pAccount);
	}

	//delete pAccount;
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
	file.Close();
	LogN(_T("账号资源读取完成"));
	return true;

}

void CDCMEngine::Tset()
{
	//m_pWndMgr->OpenPcGame();
}

//void CDCMEngine::InitScanGame()
//{
//	m_pScanGame = new tagScanGame;
//	m_pScanGame->pLDScan = new CLDScan(NULL, m_arrAccount[NULL],m_strWorkPath);
//	//m_pScanGame->pLDScan->StartThread();
//}

void CDCMEngine::GetTeamInfo(CString UiTeam)
{
	CString strTeaminfo = UiTeam;
	CStringArray arrTeamInfo;
	Split(strTeaminfo, arrTeamInfo, _T(","));		//分割队伍
	for (int i = 0; i < arrTeamInfo.GetCount(); i++){
		if (arrTeamInfo[i] == _T(""))
			continue;

		tagTeam* pTeam = new tagTeam();
		pTeam->pTeamleader = new tagTeamleader();
		CStringArray arrTeam;
		Split(arrTeamInfo[i], arrTeam, _T("|"));
		for (int j = 0; j < arrTeam.GetCount(); j++){
			CStringArray arrInfo;
			Split(arrTeam[j], arrInfo, _T(":"));	//1(Id):1号队伍(队伍编号/名称):1成员:队长(身份)

			int id = _ttoi(arrInfo[0]);
			CString TeamName = arrInfo[1];
			CString 身份 = arrInfo[3];
			bool IsExistTeam = false;

			pTeam->strTeamName = TeamName;
			if (身份 == _T("队长")){
				pTeam->pTeamleader->leaderId = id;
				pTeam->pTeamleader->strTeamIp = L"127.0.0.1";
				pTeam->pTeamleader->strTeamPort = ITostr(8000 + id);
				pTeam->pTeamleader->strTeamUrl = pTeam->pTeamleader->strTeamIp + _T(":") + pTeam->pTeamleader->strTeamPort;
			}
			else if (身份 == _T("队员")){
				tagTeamUsers* pTmpUsers = new tagTeamUsers();
				pTmpUsers->Id = id;
				pTmpUsers->strTeamIp = _T("127.0.0.1");
				pTmpUsers->strTeamPort = ITostr(8000 + id);;
				pTmpUsers->strTeamUrl = pTmpUsers->strTeamIp + _T(":") + pTmpUsers->strTeamPort;
				pTeam->arrTeamUsers.Add(pTmpUsers);
			}
		}
		m_arrpTeam.Add(pTeam);
	}

	for (int i = 0; i < m_arrpTeam.GetCount(); i++){
		BugString(_T("队伍名字=") + m_arrpTeam[i]->strTeamName);
		BugString(_T("队长编号=") + ITostr(m_arrpTeam[i]->pTeamleader->leaderId));
		BugString(_T("队长Ip地址=") + m_arrpTeam[i]->pTeamleader->strTeamIp);
		BugString(_T("队长端口=") + m_arrpTeam[i]->pTeamleader->strTeamPort);
		BugString(_T("队长URL=") + m_arrpTeam[i]->pTeamleader->strTeamUrl);
		for (int j = 0; j < m_arrpTeam[i]->arrTeamUsers.GetCount(); j++)
		{
			BugString(_T("		队员编号=" + ITostr(m_arrpTeam[i]->arrTeamUsers[j]->Id)));
			BugString(_T("		队员Ip地址=" + m_arrpTeam[i]->arrTeamUsers[j]->strTeamIp));
			BugString(_T("		队员Ip端口=" + m_arrpTeam[i]->arrTeamUsers[j]->strTeamPort));
			BugString(_T("		队员IpURL=" + m_arrpTeam[i]->arrTeamUsers[j]->strTeamUrl));
		}
		BugString(_T("_________________________________________________"));
	}

	return;
}

CString CDCMEngine::ITostr(int Iargument)
{
	CString str;
	str.Format(_T("%d"), Iargument);
	return str;
}

void CDCMEngine::BugString(CString str)
{
	CTime currentTime = CTime::GetCurrentTime();
	CString CstrTime = currentTime.Format(_T("%m-%d %H.%M.%S"));
	//std::string strTime = CT2A(CstrTime.GetString());
	//std::string m_strContent = CT2A(str.GetString());
	//std::cout << strTime << ":" << m_strContent << std::endl;

	CString DebugString;
	DebugString.Format(_T("%s%s"), CstrTime, str);
	OutputDebugString(DebugString + _T("\n"));
}

tagTeam* CDCMEngine::GetTeam(int Id)
{
	tagTeam* pTeam;
	bool IsExist = false;
	int	_Id = Id;
	for (int i = 0; i < m_arrpTeam.GetCount(); i++)
	{
		pTeam = m_arrpTeam[i];
		if (pTeam->pTeamleader->leaderId == _Id)
			IsExist = true;

		for (int j = 0; j < pTeam->arrTeamUsers.GetCount(); j++)
		{
			if (pTeam->arrTeamUsers[j]->Id == _Id)
			{
				IsExist = true;
				break;
			}
		}

		if (IsExist)
			return pTeam;
	}

	return NULL;
}

int CDCMEngine::GetPort(int Id)
{
	tagTeam* pTeam;
	int	_Id = Id;
	int IPort = NULL;
	bool IsExist = false;
	for (int i = 0; i < m_arrpTeam.GetCount(); i++)
	{
		pTeam = m_arrpTeam[i];
		if (pTeam->pTeamleader->leaderId == _Id)
		{
			IPort = _ttoi(pTeam->pTeamleader->strTeamPort);
			IsExist = true;
		}

		for (int j = 0; j < pTeam->arrTeamUsers.GetCount(); j++)
		{
			if (pTeam->arrTeamUsers[j]->Id == _Id)
			{
				IPort = _ttoi(pTeam->arrTeamUsers[j]->strTeamPort);
				IsExist = true;
				break;
			}
		}

		if (IsExist)
			return IPort;
	}

	return NULL;
}

void CDCMEngine::DownESC()
{
	//CString ret = m_pDm->EnumWindow(0, _T("另存为"), _T("#32770"), 1 + 2 + 16);

	//CString re = m_pDm->EnumWindow(_ttoi(ret), _T("取消"), _T(""), 1);

	//long retbind = m_pDm->BindWindowEx(_ttoi(ret),
	//	_T("normal"),
	//	_T("dx.mouse.position.lock.api|dx.mouse.position.lock.message|dx.mouse.clip.lock.api|dx.mouse.input.lock.api|dx.mouse.state.api|dx.mouse.api|dx.mouse.cursor"),
	//	_T("dx.keypad.input.lock.api|dx.keypad.state.api|dx.keypad.api"),
	//	_T("dx.public.active.api"),
	//	0);
	////long retbind =m_pDm->BindWindowEx(266100,
	////	m_WndIni.strDisplay,
	////	m_WndIni.strMouse,
	////	m_WndIni.strKeypad,
	////	m_WndIni.strPublic,
	////	m_WndIni.iMode);
	//if (retbind)
	//{
	//	m_pDm->KeyDownChar(_T("esc"));
	//	Sleep(20);
	//	m_pDm->KeyUpChar(_T("esc"));
	//}

	//if (retbind)
	//{ 
	//	m_pDm->UnBindWindow();
	//}
}

void CDCMEngine::WriteTeamIni(CString strPath, CString strSection, CString strKey, CString strValue)
{
	TeamIniClock.Lock();
	WritePrivateProfileString(strSection, strKey, strValue, strPath);
	TeamIniClock.UnLock();
}

CString CDCMEngine::GetTeamiIni(CString strPath, CString strSection, CString strKey, bool Tpy)
{
	//TCHAR szPath[256] = { 0 };
	//CWHService::GetWorkDirectory(szPath, 256);
	//m_strWorkPath = szPath;
	CWHIniData ini;
	ini.SetIniFilePath(strPath);
	CString Retstr = _T("");

	if (Tpy)
	{
		Retstr = ini.ReadString(strSection, strKey);
	}
	else
	{
		WORD TmpIret = ini.ReadInt(strSection, strKey, 0);
		int Iret = TmpIret;
		Retstr = ITostr(Iret);
	}
	return Retstr;
}
