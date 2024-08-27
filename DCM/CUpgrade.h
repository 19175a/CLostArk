#pragma once
#include"CTaskScript.h"
#include"CWayFind.h"

class CTaskScript;
class  CLifeModule;
class CGetCoordinates;
class CWayFind;

typedef CPoint COORDINATE;
typedef long Type;
typedef CString STRPATh;
typedef CString TASKNAME;
namespace Upgrade {
	class CGameTeleport {
	public:
		CGameTeleport();
		~CGameTeleport();
		bool Init(CTaskScript* pTaskScript);
		bool LoadInfo();
	public:
		CTaskScript* m_pTool = NULL;
	};

	class CAWayFind : public CWayFind
	{

	public:
		//	CAWayFind();
		//	~CAWayFind();

	};
	struct tagMap {
		CString strName;
		COORDINATE pEntranceCoordinate; COORDINATE pDirectionCoordinate;
		tagMap() {
			strName = _T("");
			pEntranceCoordinate = { NULL,NULL }; pDirectionCoordinate = { NULL,NULL };
		}
	};

	struct tagMapInAndOutInfo {
		CString strCitieName; tagMap* pPreviousOneInfo; std::vector<tagMap*> pvMouthInfo;
		tagMapInAndOutInfo() {
			strCitieName = _T(""); pPreviousOneInfo = NULL; pvMouthInfo.clear();
		}
	};

	struct tagMapInfo {
		CString strLslandsName; std::vector<tagMapInAndOutInfo*> vMapInfo;
		tagMapInfo() {
			strLslandsName = _T(""); vMapInfo.clear();
		}
	};

	class CMobileMapPath {
	public:
		CTaskScript* m_pTool = NULL;
		std::vector<tagMapInfo*>* m_pMapInfo = NULL;

	public:
		CMobileMapPath();
		~CMobileMapPath();
		bool Init(CTaskScript* pTaskScript);
		std::vector<tagMap> GetMapPath(CString strCurrentMapName, CString GoalName);
	private:
		bool LoadInfo();

	};

	//子地图
	struct tagChildMap
	{
		CString strFatherName, strMapName;
		STRPATh sMapPath;
		COORDINATE cMapEntrance, cMapEntranceDirection, cMapExit, cMapExitDirection;
		tagChildMap() {
			strFatherName, strMapName, sMapPath.Empty();
			cMapEntrance, cMapEntranceDirection, cMapExit, cMapExitDirection = { NULL,NULL };
		}

	};

	struct taTaskgMap
	{
		CString strMapName;
		STRPATh sMapPath;
		taTaskgMap() {
			strMapName, sMapPath.Empty();
		}

	};
	struct tagGoals
	{
		CString strGoalsNames;
		COORDINATE cCoordinate;
		tagGoals() {
			strGoalsNames.Empty();
			cCoordinate = { NULL,NULL };
		}

		tagGoals(CString _strGoalsNames, COORDINATE _cCoordinate) {
			strGoalsNames = _strGoalsNames;
			cCoordinate = _cCoordinate;
		}
	};

	struct tagDialogue {
		int iTyp;
		int IsChild;
		taTaskgMap mapInfo;
		tagChildMap ChildMap;
		std::vector<tagGoals> vGoalsCoordinate;	//有名字 有坐标
		std::vector<COORDINATE> vcCoordinate;	//无名字 有坐标
		int Istouch;
		tagDialogue() {
			iTyp = NULL;
			mapInfo = taTaskgMap();
			ChildMap = tagChildMap();
			vGoalsCoordinate.clear(), vcCoordinate.clear();
			IsChild = NULL;
			Istouch = NULL;
		}
	};

	struct tagClearGoals {
		int iTyp;
		taTaskgMap mapInfo;
		int IsChild;
		tagChildMap ChildMap;
		std::vector<tagGoals> vGoalsCoordinate;
		STRPATh sDmx;
		tagClearGoals() {
			iTyp = NULL;
			mapInfo = taTaskgMap();
			ChildMap = tagChildMap();
			IsChild = NULL;
			vGoalsCoordinate.clear();
			sDmx.Empty();

		}
	};

	struct tagTaskStrInfo {

		CString strTaskName;
		std::vector<OcrUpgrade> vContent;
		tagTaskStrInfo() {
			strTaskName.Empty();
			vContent.clear();
		}


	};


	enum taskState {
		Done = 1,
		NoTaskName = 2,
		DataException = 3
	};

	enum taskTyp {
		Dialogue = 1,		//对话
		ClearGoals = 2,		//任务清理指定怪物
	};


}

class CUpgrade {
public:
	CUpgrade();
	~CUpgrade();
	bool Init(CTaskScript* pTaskScript);
public:
	CTaskScript* m_pTool = NULL;
	CLifeModule* m_pModule = NULL;
	Upgrade::CMobileMapPath* m_pMobileMapPath = NULL;
	Upgrade::CAWayFind* m_pAWaFInd = NULL;

public:
	Upgrade::tagTaskStrInfo GetTaskName();
	Upgrade::taskState started();
	Upgrade::taskState startedTask();

public://任务
	bool Dialogue(CString strKey, Upgrade::tagTaskStrInfo TaskContent);
	bool ClearGoals(CString strKey);

public:
	bool SetMapInfo(tagGameMapInfo GameMapInfo);

};








