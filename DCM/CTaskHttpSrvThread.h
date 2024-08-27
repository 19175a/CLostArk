#pragma once
#include "WHThread.h"
#include "WHService.h"
#include "httpserver.h"
#include "HttpMessage.h"
#include "HttpResponseWriter.h"
#include "htime.h"
#include "EventLoop.h" // import setTimeout, setInterval
#pragma comment(lib, "hv.lib")

class CTaskHttpSrvThread : public CWHThread
{
public:
	CTaskHttpSrvThread(int iPort);
	~CTaskHttpSrvThread();

public:
	void Stop();
	void SetTaskInfoPtr(tagTaskInfo* pTaskInfo);


private:
	HttpService		m_router;
	http_server_t	m_httpSrv;

	int				m_iPort;
	tagTaskInfo*    m_Taskinfo;
private:
	void RegHandler();

private:
	int OnTeam(HttpRequest* req, HttpResponse* resp);

private:
	void Join(CString strIp, CString strParam);

protected:
	//运行事件
	virtual bool OnEventThreadRun();
	//开始事件
	virtual bool OnEventThreadStart();
	//终止事件
	virtual bool OnEventThreadConclude();


};

