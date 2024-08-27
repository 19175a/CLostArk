#pragma once
class CTaskManager
{
public:
	CTaskManager();
	~CTaskManager();

public:
	void Start(tagWndInfo* pWndinfo, tagGameAcconutInfo* GameAcconutInfo);
	void Stop(int id = -1);

	void Suspend(int id = -1);
	void Resume(int id = -1);

	void AddTask(int id, CStringArray& arrTask);

private:
	tagTaskInfo* GetInfo(int id);
};

