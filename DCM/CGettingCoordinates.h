#pragma once
#include "obj.h"
#include"CTaskScript.h"


class CWayFind;
class dmsoft;

class CGetCoordinates : public CWHThread
{
public:
	enum ThreadState
	{
		STATE_Stop  = -1,
		STATE_Pause = 0,
		STATE_Run   = 1,
	};


	bool IsConclude = false;	
	CGetCoordinates(CWayFind * pWayFind);
	~CGetCoordinates();
	
	CWayFind* m_pWayFind = nullptr;
	CPoint m_Current = CPoint(NULL,NULL);
	int    m_IThreadState = NULL;

	dmsoft* m_pDm = nullptr;
	CPoint GetCVPoint();
	bool OnLoop();
	void MatchGetCoordinate(CPoint& pGetPoint = CPoint(NULL, NULL),CPoint write = CPoint(NULL,NULL), bool IsWrite = false);
	int ThreadState(int write = NULL, bool IsWrite =false);
	void Test();
	bool OnEventThreadRun();
	bool OnEventThreadStart();
	bool OnEventThreadConclude();
};

