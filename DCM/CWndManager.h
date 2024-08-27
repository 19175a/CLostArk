#pragma once
class CWndManager
{
public:
	CWndManager();
	 ~CWndManager();
	 
public:
	tagWndInfo* GetWndList(int Id);
	int GetLDList();

	void RunWnd(int id);
	void CloseWnd(int id);

	void ActivateGame(int id, CString strPkgName = _T("com.netease.l10"));
	void KillApp(int id, CString strPkgName = _T("com.netease.l10"));
	//获取雷电模拟器共享文件夹
	CString GetLDSharedFolder(int id);
	CString GetLDlist();
	void MessageClick(HWND Control);


	//启动PC端游戏
	tagWndInfo* OpenPcGame(tagGameAcconutInfo* GameAcconutInfo);
	BOOL KillProcess(DWORD ProcessId);



private:
	int m_PcCmdHwnd = NULL;
	CString	m_MorePath = _T("");
	bool m_CpuOptimize = false;

private:
	CString ExeCmd(CString pszCmd);
	CString ListVM();

};

