#include "pch.h"
#include "CWndManager.h"
#include "CDCMEngine.h"
#include"obj.h"
#include"CKeyMouse.h"

extern CDCMEngine* g_pEngine;

CWndManager::CWndManager()
{

}

CWndManager::~CWndManager()
{

}

tagWndInfo* CWndManager::GetWndList(int Id)
{
	tagWndInfo* pTmpInfo = new tagWndInfo();
	CString retWnd = g_pEngine->m_pDm->EnumWindow(0, g_pEngine->m_WndIni.strTitle, g_pEngine->m_WndIni.strClz, (2 + 4 + 8 + 16));
	if (retWnd != _T("")) {
		
		pTmpInfo->hWnd = (HWND)_ttoi(retWnd);
		pTmpInfo->id = Id;
		CString strId;
		strId.Format(_T("%d"), pTmpInfo->id);
		pTmpInfo->strTitle = g_pEngine->m_pDm->GetWindowTitle((long)pTmpInfo->hWnd);
		return pTmpInfo;
	}

	////�����´��ڽṹ��
	//int Iid = Id;
	//CArray<tagEnumExeWndParam> arrEnumWnd;
	//GetProcessWnd(g_pEngine->m_WndIni.strProc, g_pEngine->m_WndIni.strTitle, g_pEngine->m_WndIni.strClz, arrEnumWnd);
	//for (int i = 0; i < arrEnumWnd.GetSize(); i++)
	//{
	//	tagWndInfo info;
	//	info.id = Iid;
	//	info.hWnd = arrEnumWnd[i].hWnds[0];
	//	if (info.hWnd == NULL)
	//		continue;

	//	info.strTitle = g_pEngine->m_pDm->GetWindowTitle((long)info.hWnd);
	//	::GetWindowRect(info.hWnd, info.rtWnd);

	//	bool LstExist = false;
	//	for (int j = 0; j < g_pEngine->m_arrWnd.GetCount(); j++){
	//		if (g_pEngine->m_arrWnd[j])
	//		{
	//			if (info.hWnd == g_pEngine->m_arrWnd[j]->hWnd)
	//			{
	//				LstExist = true;
	//			}
	//		}

	//	}

	//	if (!LstExist)
	//	{
	//		tagWndInfo* pTmpInfo = new tagWndInfo;
	//		pTmpInfo->hWnd = info.hWnd;
	//		pTmpInfo->rtWnd = info.rtWnd;
	//		pTmpInfo->id = info.id;
	//		CString strId;
	//		strId.Format(_T("%d"), pTmpInfo->id);
	//		pTmpInfo->strTitle = g_pEngine->m_pDm->GetWindowTitle((long)info.hWnd);
	//		g_pEngine->m_arrWnd.Add(pTmpInfo);
	//		return pTmpInfo;
	//	}

	//}

	//�����ǰ��ȡ�����д��ھ�������� û���µĴ��ڿ���ʹ��
	return pTmpInfo;
}

int CWndManager::GetLDList()
{
	CString strRet = ListVM();
	if (strRet.GetLength() < 1)
		return 0;

	CStringArray strArray;
	Split(strRet, strArray, _T("\r\n"));

	for (int i = 0; i < strArray.GetSize(); i++)
	{
		//0,�׵�ģ����,0,0,0,-1,-1
		tagWndInfo info;
		CString strVm = strArray.GetAt(i);

		CString strTmp;
		AfxExtractSubString(strTmp, (LPCTSTR)strVm, 0, _T(','));
		info.id = _ttoi(strTmp);

		AfxExtractSubString(strTmp, (LPCTSTR)strVm, 1, _T(','));
		info.strTitle = strTmp;

		AfxExtractSubString(strTmp, (LPCTSTR)strVm, 3, _T(','));
		info.hWnd = (HWND)_ttoi(strTmp);

		if (info.hWnd != NULL)
			::GetWindowRect(info.hWnd, info.rtWnd);

		bool bExist = false;
		for (int j = 0; j < g_pEngine->m_arrWnd.GetCount(); j++)
		{
			if (g_pEngine->m_arrWnd[j]->id == info.id)
			{
				g_pEngine->m_arrWnd[j]->hWnd = info.hWnd;
				g_pEngine->m_arrWnd[j]->strTitle = info.strTitle;
				g_pEngine->m_arrWnd[j]->rtWnd = info.rtWnd;

				bExist = true;
				break;
			}
		}

		if (!bExist)
		{
			tagWndInfo* pInfo = new tagWndInfo;
			pInfo->id = info.id;
			pInfo->hWnd = info.hWnd;
			pInfo->rtWnd = info.rtWnd;
			pInfo->strTitle = info.strTitle;

			g_pEngine->m_arrWnd.Add(pInfo);
		}
	}

	return g_pEngine->m_arrWnd.GetCount();
}

CString CWndManager::ExeCmd(CString pszCmd)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return _T("");
	}

	// ���������н���������Ϣ(�����ط�ʽ���������λ�������hWrite
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;

	// ����������
	PROCESS_INFORMATION pi;
	if (!CreateProcess(NULL, pszCmd.GetBuffer(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		pszCmd.ReleaseBuffer();
		return _T("");
	}

	pszCmd.ReleaseBuffer();

	CloseHandle(hWrite);

	// ��ȡ�����з���ֵ
	CStringA strRetTmp;
	char buff[1024 * 2] = { 0 };
	DWORD dwRead = 0;
	strRetTmp = buff;
	while (ReadFile(hRead, buff, 1024 * 2, &dwRead, NULL))
	{
		strRetTmp += buff;
	}
	CloseHandle(hRead);

	LPCSTR pszSrc = strRetTmp.GetString();
	int nLen = MultiByteToWideChar(CP_ACP, 0, buff, -1, NULL, 0);
	if (nLen == 0)
		return _T("");

	wchar_t* pwszDst = new wchar_t[nLen];
	if (!pwszDst)
		return _T("");

	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
	CString strRet(pwszDst);
	delete[] pwszDst;
	pwszDst = NULL;

	return strRet;
}

CString CWndManager::ListVM()
{
	CString strParam;

	if (g_pEngine->m_WndIni.strLDPath != _T(""))
		strParam.Format(_T("%s/dnconsole.exe list2"), g_pEngine->m_WndIni.strLDPath);

	return ExeCmd(strParam);
}

CString CWndManager::GetLDlist()
{
	return ListVM();
}

CString CWndManager::GetLDSharedFolder(int id)
{
	CString strParam;

	if (g_pEngine->m_WndIni.strLDPath != _T(""))
		strParam.Format(_T("%s/ldconsole.exe launchex --index %d --statusSettings\.sharedPictures"), g_pEngine->m_WndIni.strLDPath, id);

	return ExeCmd(strParam);
}

void CWndManager::KillApp(int id, CString strPkgName)
{
	CString strParam;

	if (g_pEngine->m_WndIni.strLDPath != _T(""))
		strParam.Format(_T("%s/dnconsole.exe killapp --index %d --packagename %s"), g_pEngine->m_WndIni.strLDPath, id, strPkgName);

	ExeCmd(strParam);
}

void CWndManager::ActivateGame(int id, CString strPkgName)
{
	CString strParam;

	if (g_pEngine->m_WndIni.strLDPath != _T(""))
		strParam.Format(_T("%s/ldconsole.exe launchex --index %d --packagename %s"), g_pEngine->m_WndIni.strLDPath, id, strPkgName);

	ExeCmd(strParam);
}

void CWndManager::RunWnd(int id)
{
	CString strParam;

	if (g_pEngine->m_WndIni.strLDPath != _T(""))
		strParam.Format(_T("%s/dnconsole.exe launch --index %d"), g_pEngine->m_WndIni.strLDPath, id);

	ExeCmd(strParam);
}

void CWndManager::CloseWnd(int id)
{
	CString strParam;

	if (g_pEngine->m_WndIni.strLDPath != _T(""))
		strParam.Format(_T("%s/dnconsole.exe quit --index %d"), g_pEngine->m_WndIni.strLDPath, id);

	ExeCmd(strParam);
}

BOOL CWndManager::KillProcess(DWORD ProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessId);
	if (hProcess == NULL)
		return FALSE;
	if (!TerminateProcess(hProcess, 0))
		return FALSE;
	return TRUE;
}

tagWndInfo* CWndManager::OpenPcGame(tagGameAcconutInfo* GameAcconutInfo)
{
	tagWndInfo* tmpWndInfo = new tagWndInfo();
	CStdioFile file;
	CString strLine;
	if (file.Open(g_pEngine->m_strWorkPath + _T("\\GamePath.txt"), CFile::modeRead)){
		file.ReadString(strLine);
		file.Close();
	}else{
		AfxMessageBox(_T("����Ϸ������·����"));
		tmpWndInfo->hWnd = NULL;
		return tmpWndInfo;
	}

	long retWnd = NULL;
	//�����Ϸ�����������ȹرյ�
	CString strPid = g_pEngine->m_pDm->EnumProcess(_T("GameCenter.exe"));
	if (strPid != _T("")) {
		LogD(_T("δ������Ϸ���ȹرյ����� ���´�ָ���˺�Ŀ¼��Ϸ������"));
		CStringArray arrStrPid;
		Split(strPid, arrStrPid, _T(","));
		for (size_t i = 0; i < arrStrPid.GetCount(); i++)
		{
			KillProcess(_ttoi(arrStrPid[i]));
		}
	}
	Sleep(5000);

	retWnd = _ttoi(g_pEngine->m_pDm->EnumWindow(0, _T("VK Play GameCenter"), _T("GameCenter.TMainForm"), (2 + 4 + 8)));
	if (retWnd) {
		LogD(_T("�ر���Ϸ������ʧ��"));
		return tmpWndInfo;
	}

	//������Ϸ������
	CString strGamePath = strLine + _T("\\") + GameAcconutInfo->strAcconut + _T("\\GameCenter\\GameCenter.exe");
	bool IsExistence = false;
	//ShellExecute(NULL, _T("open"), strGamePath, NULL, NULL, SW_SHOWNORMAL);
	if (g_pEngine->m_pDm->RunApp(strGamePath, 1)) {
		LogD(_T("�����������ɹ�"));
	}
	else
	{
		LogD(_T("����������ʧ��"));
	}
	for (size_t i = 0; i < 30; i++)
	{
		retWnd =_ttoi(g_pEngine->m_pDm->EnumWindow(0, _T("VK Play GameCenter"), _T("GameCenter.TMainForm"), (2 + 4 + 8)));
		if (retWnd) {
			g_pEngine->m_pDm->SetWindowState(retWnd, 1);
			Sleep(1000);
			g_pEngine->m_pDm->SetWindowState(retWnd, 7);
			IsExistence = true;
			break;
		}

		Sleep(1000);
		LogD(_T("�ȴ���Ϸ����������:%d"), i);
	}
	if (!IsExistence) {
		return tmpWndInfo;
	}

	//�������
	Sleep(1000);
	g_pEngine->m_pDm->MoveWindow(retWnd, 0, 0);
	IsExistence = false;
	Sleep(1000);
	for (size_t i = 0; i < 10; i++)
	{
		retWnd = _ttoi(g_pEngine->m_pDm->EnumWindow(0, _T("VK Play GameCenter"), _T("GameCenter.TMainForm"), (2 + 4 + 8)));
		g_pEngine->m_pDm->MoveWindow(retWnd, 0, 0);
		long x = NULL, y = NULL;
		if (g_pEngine->m_pDm->FindPic(0, 0, 1693, 955, g_pEngine->m_strWorkPath + _T("\\res\\��Ϸ����_Playͼ��.bmp"), _T("202020"), 0.8, 0, &x, &y) != -1) {
			g_pEngine->m_CKeyMouse->MoveTo((int)x, (int)y);
			Sleep(100);
			g_pEngine->m_CKeyMouse->LeftDown();
			Sleep(100);
			g_pEngine->m_CKeyMouse->LeftUp();
			LogD(_T("������Ϸ�ɹ����ȴ���Ϸ����"));
			IsExistence = true;
			break;
			
		}

		Sleep(1000);
	}

	if (!IsExistence) {
		LogD(_T("������������Ϸʧ��"));
		return tmpWndInfo;
	}

	//�ȴ�ѡ���������
	LogD(_T("�ȴ�ѡ���������"));
	IsExistence = false;
	for (size_t i = 0; i < 600; i++)
	{
		retWnd = _ttoi(g_pEngine->m_pDm->EnumWindow(0, g_pEngine->m_WndIni.strTitle, g_pEngine->m_WndIni.strClz, (2 + 4 + 8 + 16)));
		if (retWnd) {
			g_pEngine->m_pDm->MoveWindow(retWnd, 0, 0);
			Sleep(1000);
			for (size_t j = 0; j < 240; j++)
			{
				long x = NULL, y = NULL;
				if (g_pEngine->m_pDm->FindPic(642 - 1, 677 - 1, 654 + 1, 690 + 1, g_pEngine->m_strWorkPath + _T("\\res\\��Ϸ�������_ѡ��ͼ��.bmp"), _T("202020"), 0.7, 0, &x, &y) != -1) {
					IsExistence = true;
					break;
				}

				LogD(_T("�ȴ�ѡ���������%d"), j);
				Sleep(1000);
			}

		}

		LogD(_T("�ȴ����ڳ���%d"), i);
		if (IsExistence)
			break;

		Sleep(1000);
	}

	if (!IsExistence)
		return tmpWndInfo;

	if (retWnd) {

		tmpWndInfo->hWnd = (HWND)retWnd;
		tmpWndInfo->id = GameAcconutInfo->Id;
		CString strId;
		strId.Format(_T("%d"), GameAcconutInfo->Id);
		tmpWndInfo->strTitle = g_pEngine->m_pDm->GetWindowTitle((long)tmpWndInfo->hWnd);
		return tmpWndInfo;
	}

	return tmpWndInfo;
}

void CWndManager::MessageClick(HWND Control)
{
	if (Control != NULL)
	{
		// ģ�������������Ϣ
		int x = 0, y = 0;
		LPARAM lParam = MAKELPARAM(x, y); // x��y�������������
		::SendMessage(Control, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
		::SendMessage(Control, WM_LBUTTONUP, 0, lParam);
		Sleep(2000);
	}
}