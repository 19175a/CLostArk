#pragma once
#include "WHThread.h"
#include "CYolo7.h"
#include "CKeyMouse.h"

class dmsoft;

class StopThreadException
{
public:
	StopThreadException(std::string msg) : msg(msg) {};

private:
	std::string msg;
};

struct tagGameObj;
//
class CTaskScript;
typedef CString(CTaskScript::* FunType)(CString strParam);
typedef CMap<CString, LPCTSTR, FunType, FunType> CMapFunc;

class CKeyMouse;
class CTaskThread : public CWHThread
{
public:
	CTaskThread(tagWndInfo* pWnd);
	virtual ~CTaskThread();

protected:
	volatile bool	m_bSuspend;//��ͣ��־
	tagWndInfo*		m_pWnd;

	CKeyMouse     m_pCKeyMouse;
	
public:
	dmsoft* m_pDm;
	//��ͣ�߳�
	void Suspend();
	//�ָ��߳�
	void Resume();
	//����ģ����
	void Restart();
	//�л��˺� 0 = ��д�룬1= д��
	void Switch(int IsWriteIniLogd);



	bool IsBind() 
	{
		return true;
	}

	void AddTask(CStringArray& arrTask);

private:

	bool	m_bind;

protected:
	virtual bool OnLoop() = 0;

protected:

	//���ƴ��ھ��
	long m_hDrawWnd;
	//������ʾ��������
	long x1, y1, x2, y2;

protected:
	bool Bind();

public:
	//��ȡ��һ�����л��Ĵ���id
	int GetSwitchToWndId();

	//��Į��������
	void MoveTo(int x, int y);
	void MoveR(int rx, int ry);

	void LeftClick();
	void LeftDown();
	void LeftUp();

	void RightClick();
	void RightDown();
	void RightUp();

	void WheelDown();
	void WheelUp();

	void KeyDown(long vk);
	void KeyPress(long vk);
	void KeyUp(long vk);

	//��������
	void w(int time = 1000);
	void a(int time = 1000);
	void s(int time = 1000);
	void d(int time = 1000);

	void wa(int time = 1000);
	void wd(int time = 1000);
	void sd(int time = 1000);
	void as(int time = 1000);

	//�Լ���װ����
	void SleepEx(DWORD dwMilsec);

	void RandSleepEx(DWORD dwMilsec);

	//���
	void Click(CPoint ptClk);
	void Click(int x, int y);
	void DClick(CPoint ptClk);
	void DClick(int x, int y);

	//�������϶�
	void LeftClickDrag(CPoint StartPoint,CPoint EndPoint);
	
	//�Ҽ�
	void RClick(CPoint ptClk);
	void RClick(int x, int y);
	void DRClick(CPoint ptClk);
	void DRClick(int x, int y);

	void DeugString(CString str);

	//DLLCKeyMouse
	void DLLMoveTo(int x, int y);
	void DLLRightClick();
	void DLLLeftClick();

	void DLLLeftDown();
	void DLLLeftUp();

	void DLLLRightDown();
	void DLLRightUp();

	//��ȡ��������֮��������
	int GetRandom(int iMin, int iMax);		
			
	//��ͼ
	bool Find(CString strUI, CPoint& ptFind, CRect rtArea = CRect(0, 0, 0, 0));
	bool Find(CString strUI, CPoint& ptFind, int iTime/*�Ҷ��ٴ�*/);
	bool Find(CString strUI, CRect rtArea = CRect(0, 0, 0, 0));
	bool FindOcr(CString strUi, CString strFindString, int qa, bool IsClick = false, CString strClickUi = _T(""), int Time = 1, int clickSleep = 2000);
	
	//dmһͼ��������Ŀ��
	CString dmFindPicEx(CString strUi);


	//�Ҷ���ͼ�������ҵ��ĵ�һ��
	CPoint FindMS(CString strUI, int iTime = 1);
	bool FindClick(CString strUI);
	bool FindRClick(CString strUI);
	bool FindSkewClick(CString strUI, CPoint Skew/*ƫ��*/, int iTime = 1/*�Ҷ��ٴ�*/, int ClickSleep = 1300/*������ӳ�Ĭ��1500-2500*/);

	//�ҵ���ָ��λ�õ��
	bool FindPoint(CString strUI, CString PointUi ,int iTime = 1, int ClickSleep = 1500);
	int GetColorNum(CRect rtArea, CString strColor, double sim);
	bool Findstr(CString _strSwaitstring, CString _strFindString, int _IQuantity);
	void SetDraw(long LDrawHand, UpperCoordinates  UpCoordinates, CString strTitle, CString strColour);
	bool CleanFile();

	//ѭ���ҵ�Ŀ��ͼ��
	bool WClick(CString strUI, int iSleep = 4500);
	bool WClick(CString strUI, CRect Rct, int iSleep = 4500);
	bool WFind(CString strUI, int iSleep = 4500);
	bool WFindRetPoint(CString strUI, CPoint& ptFind, int iSleep = 4500);
	bool WFind(CString strUI, CRect Rct, int iSleep = 4500);
	int  GetTimeSeconds();

	bool Click(CString strUI, int iCount = 1, int iSleep = 4500);
	bool clk(CString strUI, int iCount = 1, int iSleep = 1);
	bool clk(CPoint ptClk);
	//ģ�⻬��
	UpperCoordinates* ComputeSlideData(CRect StartingPoint, CRect EndPoint, UpperCoordinates* pUp);
	void Slide(UpperCoordinates* pUp);
	bool cw(CString strUI, CString strWaitUI, bool bShow, int iCount, int iSleep);

	//DmYolo
	long LoadAi(CString file);
	long AiYoloSetModel(int index, CString file,CString pwd);
	long AiYoloUseModel(int index);
	CString AiYoloDetectObjects(int x1, int y1, int x2, int y2, double prob, double iou);

	//Yolo
	int yolo(int iCfgIndex, tagGameObj* pObj);

	//Ocr
	CString Ocr(CString strUI);
	CString Ocr(CRect rtArea);
	CPoint Ocr(CRect rtArea, CString str, int Q = 0);
	std::vector<OcrRes> ocr2(CRect rtArea);
	std::vector<OcrUpgrade> ocrOcrUpgrade(CRect rtArea);
	bool FindOcrClick(CString strUi, CString _Findstr, int Time = 1, int Sleep = 4500);
	bool WOcrFindCick(CString strUI, int iSleep = 4500);
	bool WOcrFindCick(CString strUI, CRect rtArea, int iSleep = 4500);
	bool WOcrFind(CString strUI, int iSleep = 4500);
	bool WOcrFind(CString strUI, CRect rtArea, int iSleep = 4500);

	//�����ҵ���Ocr������
	CPoint FindOcrPoint(CString strUi, CString _Findstr, int& size, int Time = 1);

	//ini
	VOID GetAllKeyValueOfIniFileOneSection(TCHAR* pIniFilePath, TCHAR* pSectionName, std::map<CString, CString>& mapIniKeyValue);
	CString iniGetValue(CString strIniFilePath, CString sectionName, CString strKeyName);
	bool iniGetAllKeyValueAllNodeNames(CString strIniFilePath, CString strSectionName, std::map<CString, CString>& mapIniKeyValue);

	CPoint strToPoint(CString str);


#ifdef OPENCV_NEED
	double cvfind(CString strUI, CPoint& ptFind);
	std::vector<cv::Point> OpencvFindPicEx(CString	strUI, CRect rtArea = CRect(NULL, NULL, NULL, NULL));
	cv::Mat cv��ͼת��ͼ��(long x1, long y1, long x2, long y2);
	cv::Mat cv��ͼת��ͼ��(dmsoft* dm,long x1, long y1, long x2, long y2);

	bool cvģ��ƥ��(cv::Mat& max_img, cv::Mat& min_img, CPoint& ����, double ���ƶ���ֵ = 0.4);
	bool cvģ��ƥ��(cv::Mat& max_img, cv::Mat& min_img, double ���ƶ���ֵ = 0.5);
	double cvģ��ƥ�䷵�����ƶ�(cv::Mat& max_img, cv::Mat& min_img);
	cv::Mat cv����ͼ��(cv::String ͼƬ·��, int ����ģʽ = 1);
	cv::Mat cv����ͼ��(cv::String ͼƬ·��, int ����ģʽ, double ���Ŵ�С);
	double cv��̬���Ų��Ա���(cv::Mat& ��ͼ, cv::Mat& Сͼ, bool ��ʾ���ͼ = false, int ��Сͼ = 1, double ��ʼ���� = 0.5, int ���� = 30);

	CPoint б�ʹ�ʽ(CPoint ��1, CPoint ��2, CPoint ԭ��, int ����);
	double GetPtDis(CPoint pt1, CPoint pt2);

#endif

	int Yolo(std::vector<YoloObject>& objs);
	//ʶ��
	CString DmOcr(CString strUI, CRect rtArea = CRect(0, 0, 0, 0));

	CString HttpGet(CString strUrl, CString strParam);
	CString HttpPost(CString strUrl, CMapStringToString& pMap);
	CString HttpPost(CString strUrl, CString strKey, CString strData);

	void KillApp();

public:

protected:

	CArray<tagTaskCmd> m_arrTaskCmd;

	void AddTask(CString strName, CString strParam, int count);
	bool GetTask(tagTaskCmd* cmd);
	bool GetRandTask(tagTaskCmd* cmd);
	void SetTaskCount(int count);

public:
	virtual bool OnEventThreadRun();
	virtual bool OnEventThreadStart();
	virtual bool OnEventThreadConclude();
};

