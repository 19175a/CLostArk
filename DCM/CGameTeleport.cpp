#include "pch.h"
#include"CTaskScript.h"
#include "CGameTeleport.h"
#include"CDCMEngine.h"
extern CDCMEngine* g_pEngine;

CGameTeleport::CGameTeleport()
{

}
CGameTeleport::~CGameTeleport()
{
	if (m_pvGameTeleportInfo) {
		m_pvGameTeleportInfo->clear();
		m_pvGameTeleportInfo->shrink_to_fit();
	}
	if (m_pvTeleportIslandsinfo) {
		m_pvTeleportIslandsinfo->clear();
		m_pvTeleportIslandsinfo->shrink_to_fit();
	}
	if (m_pLifeModule) {
		delete m_pLifeModule;
	}

}

void CGameTeleport::Init(CTaskScript* pTool)
{
	ASSERT(pTool);
	m_pTool = pTool;
	m_pvGameTeleportInfo = new std::vector<tagGameTeleportInfo*>;
	m_pvTeleportIslandsinfo = new std::vector<tagTeleportIslandsinfo*>;
	m_pLifeModule = new CLifeModule(pTool);

}

bool CGameTeleport::Load()
{
	//获取所有岛屿基本信息
	CString strWorkPath = g_pEngine->m_strWorkPath + _T("\\Map.ini");
	CString strIslandsName  = m_pTool->iniGetValue(strWorkPath, _T("岛屿基本信息"), _T("岛屿集"));
	CStringArray arrIslandsName;
	Split(strIslandsName, arrIslandsName, _T("|"));
	for (size_t i = 0; i < arrIslandsName.GetCount(); i++){
		
		CString arrIslandCoordinate = m_pTool->iniGetValue(strWorkPath, _T("岛屿基本信息"), _T("地图坐标_")+ arrIslandsName[i]);
		if (arrIslandCoordinate == _T("")) {
			LogD(_T("加载岛屿资源 无资源%s："), arrIslandsName[i]);
			continue;
		}

		CString strIslandsName = arrIslandsName[i];
		CPoint pIslandsCoordinates = m_pTool->strToPoint(arrIslandCoordinate);
		CString strCitiesName = m_pTool->iniGetValue(strWorkPath, _T("岛屿基本信息"), _T("岛屿名_") + strIslandsName);
		if (strCitiesName == _T("")) {
			LogD(_T("加载岛屿资源异常%s："), strCitiesName);
			continue;
		}
		
		//获取当前城市内所有坐标信息
		tagGameTeleportInfo* tmpGameTeleportInfo = new tagGameTeleportInfo();
		CStringArray ArrCitiesName;
		Split(strCitiesName, ArrCitiesName, _T("|"));
		for (size_t j = 0; j < ArrCitiesName.GetCount(); j++){
			CString strCitiesName = ArrCitiesName[j];
			CString tmpStrCoordinate = m_pTool->iniGetValue(strWorkPath, _T("城市_")+strIslandsName + _T("_地图_")+ ArrCitiesName[j]+ _T("_城市坐标"), _T("坐标"));
			CPoint pCitiesCoordinates = m_pTool->strToPoint(tmpStrCoordinate);
			CString strInsideInfo = m_pTool->iniGetValue(strWorkPath, _T("城市_") +strIslandsName + _T("_地图_") + ArrCitiesName[j] + _T("_城市内坐标"), _T("坐标info"));
			CStringArray ArrInsideInfo;
			Split(strInsideInfo, ArrInsideInfo, _T("|"));

			//tmpGameTeleportInfo->vCitiesInfo.clear();
			for (size_t k = 0; k < ArrInsideInfo.GetCount(); k++){

				tmpGameTeleportInfo->strIslandsName = strIslandsName;
				tmpGameTeleportInfo->pIslandsCoordinates = pIslandsCoordinates;
				CStringArray tmpArrInfo;
				Split(ArrInsideInfo[k], tmpArrInfo, _T(","));
				if (tmpArrInfo.GetCount() != 3){
					LogD(_T("加载岛屿坐标info资源异常%s："), ArrInsideInfo[k]);
					continue;
				}

				tagCitiesInfo* tmpCitiesInfo = new tagCitiesInfo();
				tmpCitiesInfo->strCitiesName = strCitiesName;
				tmpCitiesInfo->pCitiesCoordinates = pCitiesCoordinates;
				tmpCitiesInfo->strInsideName = tmpArrInfo[0];
				tmpCitiesInfo->pInsideCoordinates = CPoint(_ttoi(tmpArrInfo[1]),_ttoi(tmpArrInfo[2]));
				tmpGameTeleportInfo->vCitiesInfo.push_back(tmpCitiesInfo);

			}
			
		}

		m_pvGameTeleportInfo->push_back(tmpGameTeleportInfo);
	}

	LogD(_T("加载岛屿坐标info完成"));

	//加载岛蒸汽船传送岛屿
	CString str  = m_pTool->iniGetValue(strWorkPath, _T("蒸汽船传送岛屿"), _T("岛屿坐标Info"));
	CStringArray arrStrInfo;
	Split(str, arrStrInfo, _T("|"));
	for (size_t i = 0; i < arrStrInfo.GetCount(); i++){

		tagTeleportIslandsinfo* tmpTeleportIslandsinfo = new tagTeleportIslandsinfo();
		CStringArray tmpArrStr;
		Split(arrStrInfo[i], tmpArrStr, _T("&"));
		tmpTeleportIslandsinfo->strIslandsName = tmpArrStr[0];
		tmpTeleportIslandsinfo->pIslandsCoordinates = m_pTool->strToPoint(tmpArrStr[1]);
		m_pvTeleportIslandsinfo->push_back(tmpTeleportIslandsinfo);
	}
	LogD(_T("加载岛蒸汽船传送岛屿完成"));

	if(m_pvGameTeleportInfo->size() !=NULL && m_pvTeleportIslandsinfo->size()!=NULL)
		return true;
}

bool CGameTeleport::Teleport(CString strTargetCitiesName, bool IsDisregard)
{
	m_pLifeModule->BackMainScreen();

	if (!m_pTool->WOcrFind(_T("世界地图_地图名"),500)) {
		m_pTool->KeyPress(VK_M);
		m_pTool->WOcrFind(_T("世界地图_地图名"));
	}

	SLEEPEX(500);
	CString strMapName = OCR(_T("识别地图名"));

	CString strCurrentCitiesName = _T(""), strCurrentIslandName = _T("");
	CString _strTargetCitiesName = _T(""), strTargetIslandName = _T("");
	CPoint  pTargeInsinde = CPoint(NULL, NULL);
	bool Isexist = false;	
	tagCitiesInfo* tmpCitiesInfo = nullptr;
	if (strMapName == _T("")){
		LogD(_T("Teleport无内容"));
		for (auto it = m_pvGameTeleportInfo->cbegin(); it != m_pvGameTeleportInfo->cend(); it++) {
			tagGameTeleportInfo* GameTeleportInfo = *it;
			for (auto iit = GameTeleportInfo->vCitiesInfo.cbegin(); iit != GameTeleportInfo->vCitiesInfo.cend(); iit++)
			{
				auto tmpInfo = *iit;
				if (tmpInfo->strCitiesName == strTargetCitiesName)
				{//获取目标所处岛屿与城市
					_strTargetCitiesName = tmpInfo->strCitiesName;
					strTargetIslandName = GameTeleportInfo->strIslandsName;
					pTargeInsinde = GameTeleportInfo->pIslandsCoordinates;
					tmpCitiesInfo = tmpInfo;
				}

				if ( _strTargetCitiesName != _T("")) {
					Isexist = true;
					break;
				}
			}

			if (Isexist)
				break;
		}
	}
	else{

		for (auto it = m_pvGameTeleportInfo->cbegin(); it!= m_pvGameTeleportInfo->cend();it++){
			tagGameTeleportInfo* GameTeleportInfo = *it;
			for (auto iit = GameTeleportInfo->vCitiesInfo.cbegin(); iit != GameTeleportInfo->vCitiesInfo.cend(); iit++)
			{
				auto tmpInfo = *iit;
				if (tmpInfo->strInsideName == strTargetCitiesName){//获取目标所处岛屿与城市
					_strTargetCitiesName = tmpInfo->strCitiesName;
					strTargetIslandName = GameTeleportInfo->strIslandsName;
					pTargeInsinde = GameTeleportInfo->pIslandsCoordinates;
					tmpCitiesInfo = tmpInfo;
				}

				if (tmpInfo->strCitiesName == strMapName){ //获取当前所处岛屿与城市
					 strCurrentCitiesName = tmpInfo->strCitiesName;
					 strCurrentIslandName = GameTeleportInfo->strIslandsName;
				}


				if (strCurrentCitiesName != _T("") && _strTargetCitiesName != _T("")) {
					Isexist = true;
					break;
				}
			}

			if (Isexist)
				break;
		}
	}

	bool IsCitiesTeleport = true ,IsIsland = true;
	if (strCurrentCitiesName != _T("") && _strTargetCitiesName != _T("") && strCurrentCitiesName == _strTargetCitiesName) {
		IsCitiesTeleport = false;
	}
	if (strCurrentIslandName != _T("") && strTargetIslandName != _T("") && strCurrentIslandName == strTargetIslandName) {
		IsIsland = false;
	}

	if (IsDisregard) {
		IsCitiesTeleport = true;
		IsIsland = true;
	}

	if (IsIsland) {

		if (!m_pTool->WOcrFind(_T("世界地图_地图名"))) {
			m_pTool->KeyPress(VK_M);
			if (!m_pTool->WOcrFind(_T("世界地图_地图名"))) {
				LogD(_T("无法打开_世界地图"));
				return false;
			}
		}


		if (!m_pTool->WClick(_T("定期航线登船_图标"))){
			LogD(_T("无法打开_定期航线登船_图标"));
			
		}

		if (!m_pTool->WClick(_T("登上定期航线_确认"),2500)) {
			LogD(_T("无需登上定期航线_确认"));

		}

		if (!m_pTool->Ocr(_T("持有资金"))) {
			LogD(_T("无法打开_持有资金"));
			
		}

		CPoint ptmpClick = CPoint(NULL, NULL);
		for (auto It = m_pvTeleportIslandsinfo->begin(); It != m_pvTeleportIslandsinfo->end(); It++) {
			auto tmpInfo = *It;
			if (strTargetIslandName == tmpInfo->strIslandsName) {
				 ptmpClick = tmpInfo->pIslandsCoordinates;
				 break;
			}
		}
		if (!ptmpClick.x){
			LogD(_T("获取传送岛屿坐标失败"));
		}
		if (ptmpClick.x) {
			m_pTool->Click(ptmpClick);
		}

		
		if (!m_pTool->WClick(_T("蒸汽船_乘船_图标"))) {
			LogD(_T("传送岛屿异常_蒸汽船_乘船_图标"));
		}
		if (!m_pTool->WClick(_T("蒸汽船_确认_字"))) {
			LogD(_T("传送岛屿异常_蒸汽船_确认_字"));
		}
		SLEEPEX(8000);
		m_pLifeModule->BackMainScreen();
	}

	if (IsCitiesTeleport) {
		if (tmpCitiesInfo->pCitiesCoordinates.x == NULL) {
			return false;
		}

		if (!m_pTool->WOcrFind(_T("世界地图_地图名"))) {
			m_pTool->KeyPress(VK_M);
			if (!m_pTool->WOcrFind(_T("世界地图_地图名"))) {
				LogD(_T("无法打开_世界地图"));
				return false;
			}
		}
		if (pTargeInsinde.x == NULL) {
			LogD(_T("获取目标信息错误"));
			return false;
		}

		m_pTool->RClick(800, 478);
		m_pTool->SleepEx(m_pTool->GetRandom(1000, 1500));
		m_pTool->RClick(800, 478);
		m_pTool->SleepEx(m_pTool->GetRandom(1000, 1500));
		m_pTool->Click(pTargeInsinde);
		m_pTool->SleepEx(m_pTool->GetRandom(1000, 1500));
		m_pTool->Click(tmpCitiesInfo->pCitiesCoordinates);
		m_pTool->SleepEx(m_pTool->GetRandom(1000, 1500));
		m_pTool->Click(tmpCitiesInfo->pInsideCoordinates);
		if (!m_pTool->WOcrFindCick(_T("确认传送点_确认_字"))) {
			LogD(_T("无法打开_确认传送点_确认_字"));
			return false;
		}
		SLEEPEX(8000);
	}
	
	m_pLifeModule->BackMainScreen();
	return true;
}

CString CGameTeleport::GetCurrentCities(CString strCurrentInsideName)
{
	CString _strTargetCitiesName = _T(""), strTargetIslandName = _T("");
	CPoint  pTargeInsinde = CPoint(NULL, NULL);
	bool Isexist = false;
	tagCitiesInfo* tmpCitiesInfo = nullptr;
	for (auto it = m_pvGameTeleportInfo->cbegin(); it != m_pvGameTeleportInfo->cend(); it++) {
		tagGameTeleportInfo* GameTeleportInfo = *it;
		for (auto iit = GameTeleportInfo->vCitiesInfo.cbegin(); iit != GameTeleportInfo->vCitiesInfo.cend(); iit++)
		{
			auto tmpInfo = *iit;
			if (tmpInfo->strInsideName == strCurrentInsideName)
			{//获取目标所处岛屿与城市
				_strTargetCitiesName = tmpInfo->strInsideName;
				strTargetIslandName = GameTeleportInfo->strIslandsName;
				pTargeInsinde = GameTeleportInfo->pIslandsCoordinates;
				tmpCitiesInfo = tmpInfo;
			}

			if (_strTargetCitiesName != _T("")) {
				Isexist = true;
				break;
			}
		}

		if (Isexist)
			break;
	}

	return  strTargetIslandName;
}
