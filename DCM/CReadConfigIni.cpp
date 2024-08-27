#include "pch.h"
#include "CReadConfigIni.h"

CReadConfigIni::CReadConfigIni()
{

}
CReadConfigIni::~CReadConfigIni()
{

}

void CReadConfigIni::Init()
{
	m_GameTeleportInfo = new tagGameTeleportInfo();
    ReadTeleportIni();

}

void CReadConfigIni::ReadTeleportIni()
{
	TCHAR szPath[256] = { 0 };
	CWHService::GetWorkDirectory(szPath, 256);
	m_strWorkPath = szPath;

	CWHIniData ini;
	ini.SetIniFilePath(m_strWorkPath + _T("/TeleportMapInfo.ini"));
    std::vector<std::string> itemNames = getAllKeys(std::string(m_strWorkPath + "/TeleportMapInfo.ini"),"城市");
    


	CString strTeleportInfoTmp = ini.ReadString(_T("城市"), _T("托托克"));



	//m_GameTeleportInfo.
	//m_WndIni.strProc = ini.ReadString(_T("窗口"), _T("进程"));
	//m_WndIni.strTitle = ini.ReadString(_T("窗口"), _T("标题"));
	//m_WndIni.strClz = ini.ReadString(_T("窗口"), _T("类名"));
}

std::vector<std::string> CReadConfigIni::getAllSections(std::string filename) {
    std::vector<std::string> ret = {};
    char sections[8192];
    DWORD size = GetPrivateProfileSectionNamesA(sections, 8192, filename.c_str());

    char* section = sections;
    while (*section) {
        std::cout << section << "\n";
        ret.push_back(section);
        section += strlen(section) + 1;
    }
    return  ret;
}
  std::vector<std::string> CReadConfigIni:: getAllKeys(std::string filename, std::string sectionName) {
    std::vector<std::string> ret = {};
    char keys[8192];
    DWORD size = GetPrivateProfileSectionA(sectionName.c_str(), keys, 8192, filename.c_str());

    char* key = keys;
    while (*key) {
        //std::cout << key << "\n";
        ret.push_back(key);
        key += strlen(key) + 1;
    }
    return  ret;
}