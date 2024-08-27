#include "pch.h"
#include "CTaskHttpSrvThread.h"

#include "CDCMEngine.h"

extern CDCMEngine* g_pEngine;

CTaskHttpSrvThread::CTaskHttpSrvThread(int iPort)
{
	m_iPort = iPort;

	m_httpSrv.port = m_iPort;
	m_httpSrv.service = &m_router;
	m_httpSrv.worker_processes = 1;

	//m_router.base_url = "/api/v1";
	RegHandler();
}

CTaskHttpSrvThread::~CTaskHttpSrvThread()
{
	Stop();
}

void CTaskHttpSrvThread::RegHandler()
{
	m_router.GET("/ping", [this](HttpRequest* req, HttpResponse* resp) {
		req->ParseBody();

		USES_CONVERSION;
		CString strIp = A2T(req->client_addr.ip.c_str());

		return resp->String("ping back");
	});

	m_router.GET("/team", [this](HttpRequest* req, HttpResponse* resp) 
	{
		return OnTeam(req, resp);
	}
	);
}

int CTaskHttpSrvThread::OnTeam(HttpRequest* req, HttpResponse* resp)
{
	req->ParseBody();
	USES_CONVERSION;
	CString strIp = A2T(req->client_addr.ip.c_str());

	CString strCmd = A2T(Utf8ToGbk(req->GetParam("cmd").c_str()).c_str());
	CString strParam = A2T(Utf8ToGbk(req->GetParam("param").c_str()).c_str());

	LogN(_T("%s: ip:%s ����:%s ����:%s"), m_Taskinfo->pWnd->strTitle, strIp, strCmd, strParam);
	do
	{
		if (strCmd == _T("����"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else if (strCmd == _T("��ȡ������Ϣ"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else if (strCmd == _T("����������Ϣ"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else if (strCmd == _T("�����������"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else if (strCmd == _T("ȷ�Ͻ���"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else if (strCmd == _T("����ͬ�����"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else if (strCmd == _T("ȷ��ͬ�����"))
		{
			m_Taskinfo->HttpCmd(true, strCmd, strParam);
		}
		else
		{
			CStringArray arrTmp;
			Split(strParam, arrTmp, _T("-"));

			tagTaskInfo* pInfo = g_pEngine->GetTaskInfo(_ttoi(arrTmp[0]));
			if (pInfo)
			{
				pInfo->strHttpCmd = strCmd;
				pInfo->strHttpContent = MidStr(strParam, _T("-"), _T(""));
			}
		}
	} while (false);

	CString strRet;
	strRet.Format(_T("%s-%s"), strCmd, strParam);
	return resp->String(CW2CA(strRet.GetString()).GetString());
	//return resp->String(GbkToUtf8(strRet.GetString()));
}

void CTaskHttpSrvThread::SetTaskInfoPtr(tagTaskInfo* pTaskInfo)
{
	m_Taskinfo = pTaskInfo;
}


void CTaskHttpSrvThread::Stop()
{
	StopThread(1);

	http_server_stop(&m_httpSrv);
}

bool CTaskHttpSrvThread::OnEventThreadRun()
{
	LogD(_T("http�����ѿ��� �����˿�:%d"), m_iPort);

	int ret = http_server_run(&m_httpSrv);
	return true;
}

bool CTaskHttpSrvThread::OnEventThreadStart()
{
	return true;
}

bool CTaskHttpSrvThread::OnEventThreadConclude()
{
	return true;
}

//id-leaderid-port
void CTaskHttpSrvThread::Join(CString strIp, CString strParam)
{
	CStringArray arrP;
	Split(strParam, arrP, _T("-"));

	int id = _ttoi(arrP[0]);
	int leaderId = _ttoi(arrP[1]);
	CString port = arrP[2];

	bool bExist = false;
	for (int i = 0; i < g_pEngine->m_arrGroup.GetCount(); i++)
	{
		if (g_pEngine->m_arrGroup[i]->teamId == id &&
			g_pEngine->m_arrGroup[i]->leaderId == leaderId &&
			g_pEngine->m_arrGroup[i]->strTeamIp == strIp)
		{
			bExist = true;
			//������ھ͸���״̬
			//todo..d
		}
	}

	if (!bExist)
	{
		tagGroupUser* team = new tagGroupUser;
		team->leaderId = leaderId;
		team->teamId = id;
		team->strTeamIp = strIp;
		team->strTeamPort = port;
		team->strTeamUrl.Format(_T("%s:%s"), strIp, port);
		g_pEngine->m_arrGroup.Add(team);
		LogD(_T("����Ա����: ��Ա��ַ:%s ��Աid:%d �鳤id:%d"), team->strTeamUrl, team->teamId, team->leaderId);
	}
}
