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
	volatile bool	m_bSuspend;//暂停标志
	tagWndInfo*		m_pWnd;

	CKeyMouse     m_pCKeyMouse;
	
public:
	dmsoft* m_pDm;
	//暂停线程
	void Suspend();
	//恢复线程
	void Resume();
	//重启模拟器
	void Restart();
	//切换账号 0 = 不写入，1= 写入
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

	//绘制窗口句柄
	long m_hDrawWnd;
	//窗口显示绘制坐标
	long x1, y1, x2, y2;

protected:
	bool Bind();

public:
	//获取下一个待切换的窗口id
	int GetSwitchToWndId();

	//大漠基础操作
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

	//按键操作
	void w(int time = 1000);
	void a(int time = 1000);
	void s(int time = 1000);
	void d(int time = 1000);

	void wa(int time = 1000);
	void wd(int time = 1000);
	void sd(int time = 1000);
	void as(int time = 1000);

	//自己封装操作
	void SleepEx(DWORD dwMilsec);

	void RandSleepEx(DWORD dwMilsec);

	//左键
	void Click(CPoint ptClk);
	void Click(int x, int y);
	void DClick(CPoint ptClk);
	void DClick(int x, int y);

	//左键点击拖动
	void LeftClickDrag(CPoint StartPoint,CPoint EndPoint);
	
	//右键
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

	//获取两个数字之间的随机数
	int GetRandom(int iMin, int iMax);		
			
	//找图
	bool Find(CString strUI, CPoint& ptFind, CRect rtArea = CRect(0, 0, 0, 0));
	bool Find(CString strUI, CPoint& ptFind, int iTime/*找多少次*/);
	bool Find(CString strUI, CRect rtArea = CRect(0, 0, 0, 0));
	bool FindOcr(CString strUi, CString strFindString, int qa, bool IsClick = false, CString strClickUi = _T(""), int Time = 1, int clickSleep = 2000);
	
	//dm一图返回所有目标
	CString dmFindPicEx(CString strUi);


	//找多张图，返回找到的第一个
	CPoint FindMS(CString strUI, int iTime = 1);
	bool FindClick(CString strUI);
	bool FindRClick(CString strUI);
	bool FindSkewClick(CString strUI, CPoint Skew/*偏移*/, int iTime = 1/*找多少次*/, int ClickSleep = 1300/*点击后延迟默认1500-2500*/);

	//找到后指点位置点击
	bool FindPoint(CString strUI, CString PointUi ,int iTime = 1, int ClickSleep = 1500);
	int GetColorNum(CRect rtArea, CString strColor, double sim);
	bool Findstr(CString _strSwaitstring, CString _strFindString, int _IQuantity);
	void SetDraw(long LDrawHand, UpperCoordinates  UpCoordinates, CString strTitle, CString strColour);
	bool CleanFile();

	//循环找到目标图像
	bool WClick(CString strUI, int iSleep = 4500);
	bool WClick(CString strUI, CRect Rct, int iSleep = 4500);
	bool WFind(CString strUI, int iSleep = 4500);
	bool WFindRetPoint(CString strUI, CPoint& ptFind, int iSleep = 4500);
	bool WFind(CString strUI, CRect Rct, int iSleep = 4500);
	int  GetTimeSeconds();

	bool Click(CString strUI, int iCount = 1, int iSleep = 4500);
	bool clk(CString strUI, int iCount = 1, int iSleep = 1);
	bool clk(CPoint ptClk);
	//模拟滑动
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

	//返回找到的Ocr的坐标
	CPoint FindOcrPoint(CString strUi, CString _Findstr, int& size, int Time = 1);

	//ini
	VOID GetAllKeyValueOfIniFileOneSection(TCHAR* pIniFilePath, TCHAR* pSectionName, std::map<CString, CString>& mapIniKeyValue);
	CString iniGetValue(CString strIniFilePath, CString sectionName, CString strKeyName);
	bool iniGetAllKeyValueAllNodeNames(CString strIniFilePath, CString strSectionName, std::map<CString, CString>& mapIniKeyValue);

	CPoint strToPoint(CString str);


#ifdef OPENCV_NEED
	double cvfind(CString strUI, CPoint& ptFind);
	std::vector<cv::Point> OpencvFindPicEx(CString	strUI, CRect rtArea = CRect(NULL, NULL, NULL, NULL));
	cv::Mat cv截图转换图像(long x1, long y1, long x2, long y2);
	cv::Mat cv截图转换图像(dmsoft* dm,long x1, long y1, long x2, long y2);

	bool cv模板匹配(cv::Mat& max_img, cv::Mat& min_img, CPoint& 坐标, double 相似度阈值 = 0.4);
	bool cv模板匹配(cv::Mat& max_img, cv::Mat& min_img, double 相似度阈值 = 0.5);
	double cv模板匹配返回相似度(cv::Mat& max_img, cv::Mat& min_img);
	cv::Mat cv读入图像(cv::String 图片路径, int 读入模式 = 1);
	cv::Mat cv读入图像(cv::String 图片路径, int 读入模式, double 缩放大小);
	double cv动态缩放测试比例(cv::Mat& 大图, cv::Mat& 小图, bool 显示结果图 = false, int 大小图 = 1, double 开始比例 = 0.5, int 次数 = 30);

	CPoint 斜率公式(CPoint 点1, CPoint 点2, CPoint 原点, int 长度);
	double GetPtDis(CPoint pt1, CPoint pt2);

#endif

	int Yolo(std::vector<YoloObject>& objs);
	//识字
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

