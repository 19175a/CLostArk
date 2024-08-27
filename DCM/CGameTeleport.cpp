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
	//��ȡ���е��������Ϣ
	CString strWorkPath = g_pEngine->m_strWorkPath + _T("\\Map.ini");
	CString strIslandsName  = m_pTool->iniGetValue(strWorkPath, _T("���������Ϣ"), _T("���켯"));
	CStringArray arrIslandsName;
	Split(strIslandsName, arrIslandsName, _T("|"));
	for (size_t i = 0; i < arrIslandsName.GetCount(); i++){
		
		CString arrIslandCoordinate = m_pTool->iniGetValue(strWorkPath, _T("���������Ϣ"), _T("��ͼ����_")+ arrIslandsName[i]);
		if (arrIslandCoordinate == _T("")) {
			LogD(_T("���ص�����Դ ����Դ%s��"), arrIslandsName[i]);
			continue;
		}

		CString strIslandsName = arrIslandsName[i];
		CPoint pIslandsCoordinates = m_pTool->strToPoint(arrIslandCoordinate);
		CString strCitiesName = m_pTool->iniGetValue(strWorkPath, _T("���������Ϣ"), _T("������_") + strIslandsName);
		if (strCitiesName == _T("")) {
			LogD(_T("���ص�����Դ�쳣%s��"), strCitiesName);
			continue;
		}
		
		//��ȡ��ǰ����������������Ϣ
		tagGameTeleportInfo* tmpGameTeleportInfo = new tagGameTeleportInfo();
		CStringArray ArrCitiesName;
		Split(strCitiesName, ArrCitiesName, _T("|"));
		for (size_t j = 0; j < ArrCitiesName.GetCount(); j++){
			CString strCitiesName = ArrCitiesName[j];
			CString tmpStrCoordinate = m_pTool->iniGetValue(strWorkPath, _T("����_")+strIslandsName + _T("_��ͼ_")+ ArrCitiesName[j]+ _T("_��������"), _T("����"));
			CPoint pCitiesCoordinates = m_pTool->strToPoint(tmpStrCoordinate);
			CString strInsideInfo = m_pTool->iniGetValue(strWorkPath, _T("����_") +strIslandsName + _T("_��ͼ_") + ArrCitiesName[j] + _T("_����������"), _T("����info"));
			CStringArray ArrInsideInfo;
			Split(strInsideInfo, ArrInsideInfo, _T("|"));

			//tmpGameTeleportInfo->vCitiesInfo.clear();
			for (size_t k = 0; k < ArrInsideInfo.GetCount(); k++){

				tmpGameTeleportInfo->strIslandsName = strIslandsName;
				tmpGameTeleportInfo->pIslandsCoordinates = pIslandsCoordinates;
				CStringArray tmpArrInfo;
				Split(ArrInsideInfo[k], tmpArrInfo, _T(","));
				if (tmpArrInfo.GetCount() != 3){
					LogD(_T("���ص�������info��Դ�쳣%s��"), ArrInsideInfo[k]);
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

	LogD(_T("���ص�������info���"));

	//���ص����������͵���
	CString str  = m_pTool->iniGetValue(strWorkPath, _T("���������͵���"), _T("��������Info"));
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
	LogD(_T("���ص����������͵������"));

	if(m_pvGameTeleportInfo->size() !=NULL && m_pvTeleportIslandsinfo->size()!=NULL)
		return true;
}

bool CGameTeleport::Teleport(CString strTargetCitiesName, bool IsDisregard)
{
	m_pLifeModule->BackMainScreen();

	if (!m_pTool->WOcrFind(_T("�����ͼ_��ͼ��"),500)) {
		m_pTool->KeyPress(VK_M);
		m_pTool->WOcrFind(_T("�����ͼ_��ͼ��"));
	}

	SLEEPEX(500);
	CString strMapName = OCR(_T("ʶ���ͼ��"));

	CString strCurrentCitiesName = _T(""), strCurrentIslandName = _T("");
	CString _strTargetCitiesName = _T(""), strTargetIslandName = _T("");
	CPoint  pTargeInsinde = CPoint(NULL, NULL);
	bool Isexist = false;	
	tagCitiesInfo* tmpCitiesInfo = nullptr;
	if (strMapName == _T("")){
		LogD(_T("Teleport������"));
		for (auto it = m_pvGameTeleportInfo->cbegin(); it != m_pvGameTeleportInfo->cend(); it++) {
			tagGameTeleportInfo* GameTeleportInfo = *it;
			for (auto iit = GameTeleportInfo->vCitiesInfo.cbegin(); iit != GameTeleportInfo->vCitiesInfo.cend(); iit++)
			{
				auto tmpInfo = *iit;
				if (tmpInfo->strCitiesName == strTargetCitiesName)
				{//��ȡĿ���������������
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
				if (tmpInfo->strInsideName == strTargetCitiesName){//��ȡĿ���������������
					_strTargetCitiesName = tmpInfo->strCitiesName;
					strTargetIslandName = GameTeleportInfo->strIslandsName;
					pTargeInsinde = GameTeleportInfo->pIslandsCoordinates;
					tmpCitiesInfo = tmpInfo;
				}

				if (tmpInfo->strCitiesName == strMapName){ //��ȡ��ǰ�������������
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

		if (!m_pTool->WOcrFind(_T("�����ͼ_��ͼ��"))) {
			m_pTool->KeyPress(VK_M);
			if (!m_pTool->WOcrFind(_T("�����ͼ_��ͼ��"))) {
				LogD(_T("�޷���_�����ͼ"));
				return false;
			}
		}


		if (!m_pTool->WClick(_T("���ں��ߵǴ�_ͼ��"))){
			LogD(_T("�޷���_���ں��ߵǴ�_ͼ��"));
			
		}

		if (!m_pTool->WClick(_T("���϶��ں���_ȷ��"),2500)) {
			LogD(_T("������϶��ں���_ȷ��"));

		}

		if (!m_pTool->Ocr(_T("�����ʽ�"))) {
			LogD(_T("�޷���_�����ʽ�"));
			
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
			LogD(_T("��ȡ���͵�������ʧ��"));
		}
		if (ptmpClick.x) {
			m_pTool->Click(ptmpClick);
		}

		
		if (!m_pTool->WClick(_T("������_�˴�_ͼ��"))) {
			LogD(_T("���͵����쳣_������_�˴�_ͼ��"));
		}
		if (!m_pTool->WClick(_T("������_ȷ��_��"))) {
			LogD(_T("���͵����쳣_������_ȷ��_��"));
		}
		SLEEPEX(8000);
		m_pLifeModule->BackMainScreen();
	}

	if (IsCitiesTeleport) {
		if (tmpCitiesInfo->pCitiesCoordinates.x == NULL) {
			return false;
		}

		if (!m_pTool->WOcrFind(_T("�����ͼ_��ͼ��"))) {
			m_pTool->KeyPress(VK_M);
			if (!m_pTool->WOcrFind(_T("�����ͼ_��ͼ��"))) {
				LogD(_T("�޷���_�����ͼ"));
				return false;
			}
		}
		if (pTargeInsinde.x == NULL) {
			LogD(_T("��ȡĿ����Ϣ����"));
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
		if (!m_pTool->WOcrFindCick(_T("ȷ�ϴ��͵�_ȷ��_��"))) {
			LogD(_T("�޷���_ȷ�ϴ��͵�_ȷ��_��"));
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
			{//��ȡĿ���������������
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
