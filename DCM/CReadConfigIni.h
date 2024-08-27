#pragma once
class CReadConfigIni
{
public:
	CReadConfigIni();
	~CReadConfigIni();
	void Init();
	CString m_strWorkPath;

protected:
	tagGameTeleportInfo* m_GameTeleportInfo;
	void ReadTeleportIni();

	inline std::vector<std::string> getAllSections(std::string filename);
	inline std::vector<std::string> getAllKeys(std::string filename, std::string sectionName);
};

