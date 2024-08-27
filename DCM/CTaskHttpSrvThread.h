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
	//�����¼�
	virtual bool OnEventThreadRun();
	//��ʼ�¼�
	virtual bool OnEventThreadStart();
	//��ֹ�¼�
	virtual bool OnEventThreadConclude();


};

