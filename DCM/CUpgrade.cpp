#include"pch.h"
#include"CTaskScript.h"
#include "CUpgrade.h"
#include "CTaskThread.h"
#include"map"
#include"CDCMEngine.h"
#include "CGettingCoordinates.h"
#include"CLifeModule.h"
#include"CWayFind.h"

#define STRFILEPATH  g_pEngine->m_strWorkPath + _T("\\taskconfig.ini")
#define unfinishedColor _T("aa6527-222222|de8433-222222")
extern CDCMEngine* g_pEngine;
class CTaskScript;


Upgrade::CMobileMapPath::CMobileMapPath()
{

}

Upgrade::CMobileMapPath::~CMobileMapPath()
{
	if (m_pMapInfo) {
		m_pMapInfo->clear();
		m_pMapInfo->shrink_to_fit();
	}
}

bool Upgrade::CMobileMapPath::Init(CTaskScript* pTaskScript)
{
	m_pTool = pTaskScript;
	m_pMapInfo = new std::vector<tagMapInfo*>;
	if (!LoadInfo()) {
		LogE(_T("CMobileMapPath::Init"));
		return false;
	}

	return false;

}

bool Upgrade::CMobileMapPath::LoadInfo()
{
	CString strIniLslandsNames = m_pTool->iniGetValue(STRFILEPATH, _T("mapLslandsName"), _T("LslandsNames"));
	if (!strIniLslandsNames)
		return false;

	CStringArray arrLslandsNames;
	Split(strIniLslandsNames, arrLslandsNames, _T("|"));
	for (size_t i = 0; i < arrLslandsNames.GetCount(); i++) {
		CString strLslandsName = arrLslandsNames[i];
		CString strRet = m_pTool->iniGetValue(STRFILEPATH, _T("mapLslandsName"), arrLslandsNames[i] + _T("_LslandEntrance"));
		if (strRet.IsEmpty()) {
			LogD(_T("MobileMapPath无资源：%x  跳过"), strRet);
			break;
		}

		CString strCitieName = strRet;
		tagMapInfo* vpTmpMapInfo = new tagMapInfo();
		vpTmpMapInfo->strLslandsName = strLslandsName;
		while (!strCitieName.IsEmpty()) {

			tagMapInAndOutInfo* pTmpMapInAndOutInfo = new tagMapInAndOutInfo();
			pTmpMapInAndOutInfo->strCitieName = strCitieName;
			CString strSectionName = strLslandsName + _T("_") + strCitieName;
			strRet = m_pTool->iniGetValue(STRFILEPATH, strSectionName, _T("previousOne"));
			tagMap* pTmpmap = new tagMap();
			if (!strRet.IsEmpty()){//返回上个路口
				pTmpmap->strName = strRet;
				pTmpmap->pEntranceCoordinate = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strSectionName, pTmpmap->strName + _T("_previousOne_EntranceCoordinate")));
				pTmpmap->pDirectionCoordinate = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strSectionName, pTmpmap->strName + _T("_previousOne_MouthDirectionCoordinate")));
			}
			if (pTmpmap->strName != _T("") && pTmpmap->pDirectionCoordinate != COORDINATE{ NULL, NULL } && pTmpmap->pDirectionCoordinate != COORDINATE{ NULL, NULL }) {
			}
			else {
				delete pTmpmap;
				pTmpmap = NULL;
			}
			pTmpMapInAndOutInfo->pPreviousOneInfo = pTmpmap;

			
			CString strMouthDirectionNames = m_pTool->iniGetValue(STRFILEPATH, strSectionName, _T("MouthDirectionNames"));//路口
			if (strMouthDirectionNames.IsEmpty()) {
				vpTmpMapInfo->vMapInfo.push_back(pTmpMapInAndOutInfo);
				break;
			}

			CStringArray arrMouthDirectionNames;
			Split(strMouthDirectionNames, arrMouthDirectionNames, _T("|"));
			for (size_t m = 0; m < arrMouthDirectionNames.GetCount(); m++) {
				tagMap* pvTmpMouthInfo = new tagMap();
				pvTmpMouthInfo->strName = arrMouthDirectionNames[m];
				pvTmpMouthInfo->pEntranceCoordinate = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strSectionName, pvTmpMouthInfo->strName + _T("_EntranceCoordinate")));
				pvTmpMouthInfo->pDirectionCoordinate = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strSectionName, pvTmpMouthInfo->strName + _T("_MouthDirectionCoordinate")));
				if (pvTmpMouthInfo->strName != _T("") && pvTmpMouthInfo->pDirectionCoordinate != COORDINATE{ NULL, NULL } && pvTmpMouthInfo->pDirectionCoordinate != COORDINATE{ NULL, NULL }) {
					pTmpMapInAndOutInfo->pvMouthInfo.push_back(pvTmpMouthInfo);
					vpTmpMapInfo->vMapInfo.push_back(pTmpMapInAndOutInfo);
					strCitieName = pvTmpMouthInfo->strName;
				}
				else {
					delete pTmpmap;
					pTmpmap = NULL;
					strCitieName.Empty();
				}
			}

		}

		m_pMapInfo->push_back(vpTmpMapInfo);
	}

	return m_pMapInfo->size() !=NULL;
}

std::vector<Upgrade::tagMap> Upgrade::CMobileMapPath::GetMapPath(CString strCurrentMapName, CString GoalName)
{
	std::vector<Upgrade::tagMap> vTmpMapInAndOutInfo;
	vTmpMapInAndOutInfo.clear();
	if (!m_pMapInfo->size())
		return vTmpMapInAndOutInfo;

	bool IsExist = false;

	//正向匹配
	for (auto it = m_pMapInfo->begin(); it != m_pMapInfo->end(); it++){
		auto itTmpMapInfo = *it;
		for (auto im = itTmpMapInfo->vMapInfo.begin(); im != itTmpMapInfo->vMapInfo.end(); im++)
		{
			auto imTmpMapInfo = *im;
			if (imTmpMapInfo->strCitieName == strCurrentMapName) { //开始入栈
				IsExist = true;
			}
			if (IsExist) {
				if (!imTmpMapInfo->pvMouthInfo.size()) {//多个路口
					auto &ItTmpInfo = imTmpMapInfo->pvMouthInfo;
					for (size_t i = 0; i < ItTmpInfo.size(); i++)
					{
						if (ItTmpInfo[i]->strName == GoalName) {
							vTmpMapInAndOutInfo.push_back(*ItTmpInfo[i]);
							return vTmpMapInAndOutInfo;
						}
					}

				}
				else{//只有一个路口

					if (imTmpMapInfo->strCitieName == GoalName) {//结束入栈
						if (!vTmpMapInAndOutInfo.size())
							return vTmpMapInAndOutInfo;
					}

					auto &ItTmpInfo = imTmpMapInfo->pvMouthInfo;
					vTmpMapInAndOutInfo.push_back(*ItTmpInfo[0]);
				}
			}

		}
	}

	////逆序匹配
	IsExist = false;	
	vTmpMapInAndOutInfo.clear();
	for (auto it = m_pMapInfo->begin(); it != m_pMapInfo->end(); it++) {
		auto itTmpMapInfo = *it;
		for (auto im = itTmpMapInfo->vMapInfo.rbegin(); im != itTmpMapInfo->vMapInfo.rend(); im++)
		{
			auto imTmpMapInfo = *im;
			if (imTmpMapInfo->strCitieName == strCurrentMapName) { //开始入栈
				IsExist = true;
			}
			if (IsExist) {
				if (!imTmpMapInfo->pvMouthInfo.size()) {//多个路口
					auto& ItTmpInfo = imTmpMapInfo->pvMouthInfo;
					for (size_t i = 0; i < ItTmpInfo.size(); i++)
					{
						if (ItTmpInfo[i]->strName == GoalName) {
							vTmpMapInAndOutInfo.push_back(*ItTmpInfo[i]);
							return vTmpMapInAndOutInfo;
						}
					}

				}
				else {//只有一个路口

					if (imTmpMapInfo->strCitieName == GoalName) {//结束入栈
						if (!vTmpMapInAndOutInfo.size())
							return vTmpMapInAndOutInfo;
					}

					auto& ItTmpInfo = imTmpMapInfo->pvMouthInfo;
					vTmpMapInAndOutInfo.push_back(*ItTmpInfo[0]);
				}
			}


		}
	}

	return vTmpMapInAndOutInfo;
}


Upgrade::CGameTeleport::CGameTeleport()
{


}

Upgrade::CGameTeleport::~CGameTeleport()
{



}

bool Upgrade::CGameTeleport::Init(CTaskScript* pTaskScript)
{

	m_pTool = pTaskScript;
	if (!LoadInfo()) {
		LogE(_T("CGameTeleport::Init"));
		return false;
	}

	return false;
}

bool Upgrade::CGameTeleport::LoadInfo()
{

	return false;
}


CUpgrade::CUpgrade()
{


}
CUpgrade::~CUpgrade()
{
	if (m_pModule) {
		delete m_pModule;
		m_pModule = NULL;
	}

	if (m_pMobileMapPath) {
		delete m_pMobileMapPath;
		m_pMobileMapPath = NULL;
	}

}
bool CUpgrade::Init(CTaskScript* pTaskScript)
{
	m_pTool = pTaskScript;
	m_pModule = new CLifeModule(pTaskScript);
	m_pMobileMapPath = new Upgrade::CMobileMapPath();
	m_pMobileMapPath->Init(pTaskScript);

	return false;
}

Upgrade::taskState CUpgrade::started()
{
	startedTask();
	return Upgrade::taskState::Done;
}

Upgrade::taskState CUpgrade::startedTask()
{
	Upgrade::tagTaskStrInfo retTask = GetTaskName();
	if (retTask.strTaskName.IsEmpty())
		return Upgrade::taskState::NoTaskName;

	std::map<CString, CString> mapIniKeyValue = {};
	if(!m_pTool->iniGetAllKeyValueAllNodeNames(STRFILEPATH, _T("taskNames"), mapIniKeyValue))
		return Upgrade::taskState::DataException;

	CString strTaskKey = _T("");
	for (auto it = mapIniKeyValue.cbegin(); it != mapIniKeyValue.cend(); it++) {
		CStringArray arrContent;
		Split(it->second, arrContent, _T("|"));
		for (size_t i = 0; i < arrContent.GetCount(); i++) {
			if (m_pTool->Findstr(arrContent[i], retTask.strTaskName, retTask.strTaskName.GetLength())) {
				strTaskKey = arrContent[i];
				break;
			}
		}
	}

	if (strTaskKey.IsEmpty()) {
		LogE(_T("DataException:%s"), retTask.strTaskName);
		return Upgrade::taskState::DataException;
	}
	
	int iTyp = _ttoi(m_pTool->iniGetValue(STRFILEPATH, strTaskKey, _T("iTyp")));
	if (iTyp == Upgrade::taskTyp::Dialogue) {
		if(Dialogue(strTaskKey, retTask))
			return Upgrade::taskState::Done;
	}
	else if(iTyp == Upgrade::taskTyp::ClearGoals){
		if(ClearGoals(strTaskKey))
			return Upgrade::taskState::Done;
	}


	return Upgrade::taskState::Done;
}

bool CUpgrade::Dialogue(CString strKey, Upgrade::tagTaskStrInfo TaskContent)
{
	Upgrade::tagDialogue DialogueInfo = Upgrade::tagDialogue();
	DialogueInfo.mapInfo.strMapName = m_pTool->iniGetValue(STRFILEPATH, strKey, _T("strMapName"));
	DialogueInfo.mapInfo.sMapPath = m_pTool->iniGetValue(STRFILEPATH, strKey, _T("sMapPath"));
	DialogueInfo.IsChild = _ttoi(m_pTool->iniGetValue(STRFILEPATH, strKey, _T("IsChild")));
	DialogueInfo.Istouch = _ttoi(m_pTool->iniGetValue(STRFILEPATH, strKey, _T("Istouch")));
	if (DialogueInfo.IsChild) {
		DialogueInfo.ChildMap.strMapName = m_pTool->iniGetValue(STRFILEPATH, strKey, _T("C_strMapName"));
		DialogueInfo.ChildMap.sMapPath = m_pTool->iniGetValue(STRFILEPATH, strKey, _T("C_sMapPath"));
		DialogueInfo.ChildMap.cMapEntrance = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strKey, _T("C_cMapEntrance")));
		DialogueInfo.ChildMap.cMapEntranceDirection = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strKey, _T("C_cMapEntranceDirection")));
		DialogueInfo.ChildMap.cMapExit = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strKey, _T("C_cMapExit")));
		DialogueInfo.ChildMap.cMapExitDirection = m_pTool->strToPoint(m_pTool->iniGetValue(STRFILEPATH, strKey, _T("C_cMapExitDirection")));
	}

	CString strGoals = m_pTool->iniGetValue(STRFILEPATH, strKey, _T("vGoalsCoordinate"));
	CStringArray arrGoalsInfo;
	Split(strGoals, arrGoalsInfo, _T("|"));
	for (size_t i = 0; i < arrGoalsInfo.GetCount(); i++){
		CStringArray arrTmpInfo;
		Split(arrGoalsInfo[i], arrTmpInfo, _T("-"));
		if (arrTmpInfo.GetCount() != 2)
			continue;

		DialogueInfo.vGoalsCoordinate.push_back(Upgrade::tagGoals(arrTmpInfo[0], m_pTool->strToPoint(arrTmpInfo[1])));
	}
	
	CString strCoordinate = m_pTool->iniGetValue(STRFILEPATH, strKey, _T("vcCoordinate"));
	CStringArray arrStrCoordinate;
	Split(strGoals, arrStrCoordinate, _T("|"));
	for (size_t i = 0; i < arrStrCoordinate.GetCount(); i++){
		DialogueInfo.vcCoordinate.push_back(m_pTool->strToPoint(arrStrCoordinate[i]));
	}

	do
	{
		m_pModule->BackMainScreen();
		if (!m_pTool->Find(_T("世界地图_地图名"))) {
			m_pTool->KeyPress(VK_M);
			if (m_pTool->WOcrFind(_T("世界地图_地图名")))
				break;
		}
		LogD(_T("checkMap"));
	} while (true);

	m_pTool->SleepEx(500);
	CString strMapName = m_pTool->Ocr(_T("识别地图名"));

	std::vector<Upgrade::tagMap>vMapPath = m_pMobileMapPath->GetMapPath(strMapName, DialogueInfo.mapInfo.strMapName);


	int tset = NULL;


	return false;
}
bool CUpgrade::ClearGoals(CString strKey)
{

	return false;
}

Upgrade::tagTaskStrInfo CUpgrade::GetTaskName()
{
	m_pModule->BackMainScreen();
	if (!m_pTool->WClick(_T("主线图标")))return{};

	COORDINATE tagPOint{ NULL,NULL };
	if (!m_pTool->WFindRetPoint(_T("任务日志图标"), tagPOint)) return {};

	Upgrade::tagTaskStrInfo tmpInfo = Upgrade::tagTaskStrInfo();
	tmpInfo.strTaskName = m_pTool->Ocr(CRect(tagPOint.x + 135, tagPOint.y + 60, tagPOint.x + 350, tagPOint.y + 100));
	COORDINATE cStart,cEnd = { NULL,NULL };
	if (!m_pTool->Find(_T("目标"), cStart, CRect(tagPOint.x, tagPOint.y, tagPOint.x + 363, tagPOint.y + 471))) return {};
	if (!m_pTool->Find(_T("内容"), cEnd, CRect(tagPOint.x, tagPOint.y, tagPOint.x + 363, tagPOint.y + 471))) return {};

	tmpInfo.vContent = m_pTool->ocrOcrUpgrade(CRect(cStart.x - 10, cStart.y - 10, cEnd.x + 320, cEnd.y));
	for (auto i = 0; i < tmpInfo.vContent.size(); i++){
		tmpInfo.vContent[i].iX += cStart.x - 10;
		tmpInfo.vContent[i].iY += cStart.y - 10;

		//check unfinishedColor
		if (m_pTool->GetColorNum(CRect(tmpInfo.vContent[i].iX, tmpInfo.vContent[i].iY, tmpInfo.vContent[i].iX + 296, tmpInfo.vContent[i].iY + 14), unfinishedColor, 0.9) > NULL) {
			tmpInfo.vContent[i].IsDone = true;
		}
	}

	return tmpInfo;
}

//bool Upgrade::CUpgrade::SetMapInfo(tagGameMapInfo GameMapInfo)
//{
//
//
//	return false;
//}
