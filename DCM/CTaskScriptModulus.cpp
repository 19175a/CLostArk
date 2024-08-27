#include "pch.h"
#include "CTaskScript.h"
#include "CDCMEngine.h"
#include "obj.h"

extern CDCMEngine* g_pEngine;

bool CTaskScript::CheckMainInterface(bool IsBackMain/*如果不在主界面是否需要返回到主界面*/)
{
	int Time = 10;
	while (Time--)
	{
		//这里是触发在主界面最高的，很多在主界面的功能都可以在这里捕获触发
		右下角事件();
		右下角使用物品();
		if ( Find(_T("主界面左下角加号")) && Find(_T("主界面任务图标")))
		{
			if (Find(_T("主界面商城"))||Find(_T("主界面包裹")))
			{
				识别主界面Lv();
				
				return true;
			}
		}

		if (IsBackMain)
		{
			Clean();
		}
		else
		{
			return false;
		}
	}
	return false;
}

void CTaskScript::Clean(CString strUiName)
{
	if (strUiName != _T(""))
	{
		if (FindClean(strUiName))
			return;
	}

	右下角使用物品();
	右下角事件();

	bool IsOnLoop = false;
	do
	{


		if (Find(_T("对话_离开副本")))
		{
			Click(_T("任务对话_点击确定"));
			Iostate(_T("任务对话_点击确定"));
			IsOnLoop = true;
		}

		if (FindClick(_T("对话_对话NPC选择接受正传任务")))
		{
			Iostate(_T("对话_对话NPC选择接受正传任务"));
			IsOnLoop = true;
		}

		if (Find(_T("任务对话_推荐需要师傅")))
		{
			Iostate(_T("任务对话_推荐需要师傅"));
			Click(_T("对话_每次升级不提示"));
			Click(_T("任务对话_不需要"));
		}
		if (Find(_T("对话_新的轮回")))
		{
			Click(_T("对话_新的轮回点击确定"));
			Iostate(_T("对话_新的轮回点击确定"));
		}


		if (FindClick(_T("对话_跳过顶部动画")))
		{
			Iostate(_T("对话_跳过顶部动画"));
			IsOnLoop = true;
		}

		if (Find(_T("对话_选择灵兽")))
		{
			Iostate(_T("对话_选择灵兽确定"));
			Click(_T("对话_选择灵兽确定"));
		}

		if (Find(_T("对话_服务器限制等级")))
		{
			Iostate(_T("对话_服务器限制等级确定"));
			Click(_T("对话_服务器限制等级确定"));
		}

		if (FindClick(_T("对话_跳过过场动画")))
		{
			Iostate(_T("对话_跳过过场动画"));
			IsOnLoop = true;
		}

		if (FindClick(_T("跳过底部对话")))
		{
			Iostate(_T("跳过底部对话"));
			IsOnLoop = true;
		}

		if (FindPoint(_T("推开房门"),_T("点击_推开访问")))
		{
			Iostate(_T("推开房门"));
		}
		
		if (FindPoint(_T("今日师门完成"), _T("今日师门完成_确定")))
		{
			Iostate(_T("推开房门"));
		}

		if (FindPoint(_T("回归到任务栏"), _T("回归到任务栏点击任务图标")))
		{
			Iostate(_T("回归到任务栏"));
		}



		if (Find(_T("任务对话_右边"))) {
			FindClick(_T("任务对话_右边随机点击"));
			Iostate(_T("任务对话_右边随机点击"));
			IsOnLoop = true;
		}
		if (Find(_T("任务对话_左边"))) {
			FindClick(_T("任务对话_左边随机点击"));
			Iostate(_T("任务对话_左边随机点击"));
			IsOnLoop = true;
		}
		Lv事件引导(iPeopleLv);	//捕获
		GetHttpCmd();

		if (FindClick(_T("清除叉号"))) { return; }
		if (FindClick(_T("主界面_被缩放"))) { return; }
		if (FindClick(_T("关闭地图"))) { return; }
		if (FindClick(_T("关闭分线界面"))) { return; }
		if (FindClick(_T("关闭活动界面"))) { return; }
		if (FindClick(_T("清除包裹设置"))) { return; }
		if (FindClick(_T("广告清除"))) { return; }
		if (FindClick(_T("关闭拍照"))) { return; }

		
		//跳过动画 #对话时没有主界面特征，或者过场动画

		if (!CheckMainInterface())
		{
			Click(_T("跳过对话"));
			Iostate(_T("跳过动画 #对话时没有主界面特征，或者过场动画"));
			IsOnLoop = true;
		}

		if (!IsOnLoop)
			return;

		IsOnLoop = false;

	} while (true);
}

bool CTaskScript::CheckMove()
{
	//颜色数量不一样则为正在移动，一样则为移动完毕
	int iTime = 35;
	while (iTime--)
	{
		if (!Find(_T("坐标特征")))
			return false;

		int ColorSizeTmp = GetColorNum(CRect(853, 22, 906, 39), _T("868381-111111|bac0c8-111111"), 0.85);
		SleepEx(1000);
		if (ColorSizeTmp == GetColorNum(CRect(853, 22, 906, 39), _T("868381-111111|bac0c8-111111"), 0.85) || ColorSizeTmp == NULL)
			return true;

		DeugString(_T("窗口") + ITostr(m_pWnd->id) + _T("  :等待移动完毕，倒计时:") + ITostr(iTime));
	}

	return false;
}

bool CTaskScript::OpenControl(CString strUi/*res资源的Ocr名*/, CString _strFindString/*找的字*/, int FindQuantity, CString strControl/*没有找到 则使用快捷键打开*/)
{
	int iTime = 10;
	while (iTime--)
	{
		CString FindString = _strFindString;
		int Quantity = FindQuantity;
		CString strRetOcr = ocr(strUi);
		if (Findstr(strRetOcr, FindString, Quantity))
		{
			DeugString(_T("窗口:") + ITostr(m_pWnd->id) + _T(" ") + strUi + _T("控件已经打开 无需再次打开"));
			return true;
		}

		Clean();
		m_pDm->KeyDownChar(strControl);
		SleepEx(GetRandom(10, 20));
		m_pDm->KeyUpChar(strControl);
		SleepEx(GetRandom(1000, 1500));
	}

}

bool CTaskScript::GetPeopleInfo()
{
	if (OpenControl(_T("角色属性"), _T("角色属性"), 2, _T("c")))
	{
		//获取人物信息
		iPeopleLv = _ttoi(ocr(_T("角色Lv")));
		strPeopleName = ocr(_T("角色名"));
		strPeopleType = ocr(_T("角色职业"));
		CStringArray arrstrPeopleType;
		Split(strPeopleType, arrstrPeopleType, _T("\n"));
		if (arrstrPeopleType.GetCount() > 1)
			strPeopleType = arrstrPeopleType[0];

		strPeopleId = ocr(_T("角色Id"));

		if (!IsReport)
		{
			void AddOnlieTeamUsersInfo(tagPeopleInfo PeoPeleInfo, tagTeamUsers TeamUsers);
			int  Id; //组员
			CString strTeamIp;
			CString strTeamPort;
			CString strTeamUrl;

			tagTeamUsers* pTeamUsers = GetTeamUsersInfo(m_pWnd->id);
			//发送报告上线
			SendUser(m_pTeam->pTeamleader->leaderId, _T("报告上线"), ITostr(iPeopleLv) + _T("|") + strPeopleName + _T("|") + strPeopleId + _T("|") + strPeopleType + _T("?")
				+ ITostr(pTeamUsers->Id) + _T("|") + pTeamUsers->strTeamIp + _T("|") + pTeamUsers->strTeamPort + _T("|") + pTeamUsers->strTeamUrl);

		}

		return true;
	};

	return false;
}

void CTaskScript::Lv事件引导(int _iLv)
{
	if (_iLv<=28)
	{
		return;
	}
	int iLv = _iLv;
	if (iLv <= 10 || iLv == 0)
	{
		//引导_自动拾取
		if (FindPoint(_T("引导_自动拾取"), _T("点击_点击升级包裹")))
		{
			DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("自动拾取"));
		}
		if (Find(_T("引导_自动拾取打开自动设置")))
		{
			Click(_T("点击_点击自动拾取打开自动设置"));
		}
		if (Find(_T("引导_自动拾取_取消白装")))
		{
			Click(_T("点击_自动拾取_取消白装"));
		}
		if (Find(_T("引导_自动拾取_取消黄装")))
		{
			Click(_T("点击_自动拾取_取消黄装"));
		}
		if (Find(_T("引导_自动拾取_取消橙装")))
		{
			Click(_T("点击_自动拾取_取消橙装"));
		}

	}
	if (iLv <= 4 || iLv == 0)
	{
		FindPoint(_T("点击切换分线"), _T("点击_点击切换分线"));
	}

	if (iLv >= 5 && iLv <= 10 || iLv == 0)
	{
		//引导_技能功能
		

		//if (FindOcr(_T("引导_技能功能_引导点击技能"), _T("有新的功能指引"), 6, true, _T("点击_图标更多")))
		//{
		//	DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) +_T("引导_技能功能_新的功能") );
		//}
		//
		//if (FindOcr(_T("引导_技能功能_查看技能"), _T("你开启了技能"), 5, true, _T("点击_技能功能_主界面技能图标")))
		//{
		//	DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("引导_技能功能_主界面技能图标"));
		//}

		CPoint Point = CPoint(NULL, NULL);
		if (Find(_T("引导_技能功能_切换不同技能"), Point))
		{
			Click(Point.x -GetRandom(80,120), Point.y);
		}
		if (Find(_T("引导_技能功能_选择新技能")))
		{
			Click(_T("点击_技能功能_选择新技能"));
			Click(_T("点击_技能功能_随机点击详情"));
		}
	}
	
	if (iLv >= 10 && iLv <= 15|| iLv == 0)
	{
		//引导灵兽
		if (Find(_T("引导_获得了灵兽")))
		{
			DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("引导_获得了灵兽"));
			Click(_T("点击_点击升级包裹"));
		}
		if (FindClick(_T("引导_引导灵兽_点击灵兽")))
		{

		}
		if (FindClick(_T("引导_引导灵兽_召唤灵兽")))
		{

		}

	}

	if (iLv == 11 || iLv == 0)
	{
		//引导包裹——自动使用回复药水


		FindPoint(_T("引导包裹_刚刚的奖励"), _T("引导包裹_点击_包裹"));
		CPoint point;
		if (Find(_T("引导包裹_点击这里"), point))
			Click(GetRandom(point.x + 115, point.x + 125), point.y);

		FindPoint(_T("引导包裹_点击自动回复界面"), _T("引导包裹_点击_点击自动回复界面"));
		FindPoint(_T("引导包裹_选取"), _T("引导包裹_点击_选取"));
		FindPoint(_T("引导包裹_点击"), _T("引导包裹_点击_添加"));
		FindPoint(_T("引导包裹_点击二"), _T("引导包裹_点击_添加二"));
		FindPoint(_T("引导包裹_点击发力"), _T("引导包裹_点击_点击发力"));
		if (Find(_T("气血补充")))
		{
			long x = NULL, y = NULL;
			if (!m_pDm->FindColor(302, 419, 316, 429, _T("7ac420-222222"), 0.9, 0, &x, &y))
				FindClick(_T("引导包裹_点击_自动补充其他相似"));
		}
	}

	if (iLv == 15 || iLv == 0)
	{
		//引导打造
		if (Find(_T("引导_打造")))
		{
			DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("引导_打造"));
			Click(_T("主界面_打造图标"));
		}
		if (Find(_T("引导_点击强化")))
		{
			Click(_T("点击_强化"));
		}
		if (Find(_T("引导_选择强化的装备")))
		{
			Click(_T("点击_选择装备"));
		}
		if (Find(_T("引导_点击强化装备")))
		{
			Click(_T("点击_强化装备"));
		}

	}
	
	if (iLv == 20 || iLv == 0)
	{
		//引导帮会
		if (Find(_T("引导帮会_主界面更多选项加号")))
		{
			DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("引导_引导帮会"));
			Click(_T("点击_主界面更多选项加号"));
		}
		if (Find(_T("引导帮会_主界面帮会图标")))
		{
			Click(_T("点击_主界面帮会图标"));
		}
	}

	if (iLv == 21 || iLv == 0)
	{
		//引导技能
		if (Find(_T("引导技能_主界面更多选项加号")))
		{
			DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("引导_引导技能"));
			Click(_T("点击_主界面更多选项加号"));
		}
		//没有发现该资源项
		//if (Find(_T("引导技能_主界面技能图标")))
		//{
		//	Click(_T("点击_主界面技能图标"));
		//}
		if (Find(_T("引导技能_点击更换")))
		{
			Click(_T("点击_点击更换"));
		}
		if (Find(_T("引导技能_点击征途")))
		{
			Click(_T("点击_点击征途"));
		}

		//滑动技能添加到技能栏
		if (Find(_T("技能界面")))
		{
			UpperCoordinates Slid;
			ComputeSlideData(CRect(611, 156, 627, 173), CRect(585, 462, 603, 477), &Slid);
			Slide(&Slid);
			SleepEx(1200);
		}
	}
	
	if (iLv == 21 || iLv == 0)
	{
		//引导升级技能
		if (Find(_T("引导技能_主界面更多选项加号")))
		{
			DeugString(_T("Lv事件引导Lv: |") + ITostr(_iLv) + _T("引导_引导升级技能"));
			Click(_T("点击_主界面更多选项加号"));
		}
		//if (Find(_T("引导技能_主界面技能图标")))
		//{
		//	Click(_T("点击_主界面技能图标"));
		//}
		if (Find(_T("引导升级技能_点击升级按钮")))
		{
			Click(_T("点击_点击升级按钮"));
			Click(_T("点击_点击修为"));
		}
	}

	if (iLv == 28 || iLv == 0)
	{
		//引导师门任务
		FindPoint(_T("引导师门任务_新活动"), _T("引导师门任务_点击_活动图标"),1,3000);
		FindPoint(_T("引导师门任务_引导参加"), _T("引导师门任务_点击_引导参加"));
	}

}

void CTaskScript::右下角使用物品()
{
	FindClick(_T("右下角使用物品_装备"));
	FindPoint(_T("使用大力丸"), _T("控件使用大力丸"));
	FindPoint(_T("使用河灯"), _T("控件使用"));
	FindClick(_T("右下角控件使用"));

}

void CTaskScript::右下角事件()
{
	if (Find(_T("右下角事件_领悟了新技能")))
		Click(_T("点击_叉号清除领悟了新技能提醒"));

}

int CTaskScript::识别主界面Lv()
{
	CString retDmOcr = DmOcr(_T("主界面角色等级"));
	if (retDmOcr != _T(""))
	{
		DeugString(_T("主界面角色Lv:")+ retDmOcr);
		iPeopleLv = _ttoi(retDmOcr);
		return _ttoi(retDmOcr);
	}
	return NULL;
}


bool CTaskScript::CheckBattle(bool IsAwait)
{
	int iTime = 1 * 60 * 1000;
	while (iTime--)
	{
		if (Find(_T("战斗_挂机图标")))
		{
			if (Find(_T("战斗_图标挂机中")))
			{
				if (!IsAwait)
					return  true;
				Iostate(_T("挂机中"));
			}
			else
			{
				return false;
			}
			CTime currentTime = CTime::GetCurrentTime();
			CString CstrTime = currentTime.Format(_T("%m-%d%H%M"));
			CString tagBmp = ITostr(m_pWnd->id) + CstrTime + _T(".bmp");
			m_pDm->Capture(850, 520, 878, 550, tagBmp);
			SleepEx(1000);
			long x = NULL, y = NULL;
			if (m_pDm->FindPic(850-1, 520-1, 878+1, 550+1, tagBmp,_T("020202"),0.9,0,&x,&y) != -1)
			{
				if (!IsAwait)
				{
					m_pDm->DeleteFile(tagBmp);
					return  true;
				}
				else
				{
					m_pDm->DeleteFile(tagBmp);
					Iostate(_T("战斗中"));
				}
			}

		}
		else
		{
			return false;
		}
		SleepEx(1000);
	}
	DeugString(_T("检查是否在战斗"));
	return false;
}

bool CTaskScript::自动寻路(CPoint 目标点)
{
	if (CheckMainInterface(true))
	{
		FindClick(_T("官方寻路_打开地图"));
		if (FindClick(_T("官方寻路_定位图标")))
		{
			CPoint PointX = CPoint(NULL, NULL);
			if (Find(_T("输入坐标X图标"),PointX))
			{
				Click(CPoint(GetRandom(PointX.x + 70, PointX.x + 100), PointX.y));
				SleepEx(GetRandom(1200, 1500));
				CString PointX = ITostr(目标点.x);
				for (int i = 0; i < PointX.GetLength(); i++)
				{
					char myChar = PointX.GetAt(i); 
					CString x(myChar);
					CString Findx = _T("数字") + x;
					CPoint retPoint;
					FindClick(Findx);
				}
				FindClick(_T("确定"));
			}
			SleepEx(GetRandom(1500, 2000));
			CPoint PointY = CPoint(NULL, NULL);
			if (Find(_T("输入坐标Y图标"), PointY))
			{
				Click(CPoint(GetRandom(PointY.x + 70, PointY.x + 100), PointY.y));
				SleepEx(GetRandom(1200, 1500));
				CString PointY = ITostr(目标点.y);
				for (int i = 0; i < PointY.GetLength(); i++)
				{
					char myChar = PointY.GetAt(i); 
					CString y(myChar);
					CPoint retPoint;
					FindClick(_T("数字")+y);
				}
				FindClick(_T("确定"));
			}

			SleepEx(GetRandom(1500, 2000));
			FindClick(_T("前往"));
			FindClick(_T("关闭地图"));
		}
	}
	return false;
}

bool CTaskScript::GetActiveTask(CString _TaskName/*需要对应res的名字*/)
{
	BOOL Confirmation = FALSE;	INT  Time = 3; 	CString TaskName = _TaskName;

	while (Time--){
		CPoint TaskNamePoint(NULL, NULL);
		if (Find(TaskName,TaskNamePoint)){
			if (Find(_T("活动界面_参加"), CRect(TaskNamePoint.x + 175, TaskNamePoint.y, TaskNamePoint.x + 253, TaskNamePoint.y + 51))){
				Click(CPoint(TaskNamePoint.x + 200 + GetRandom(0, 20), TaskNamePoint.y + 20 + GetRandom(0, 10)));
				return TRUE;
			}
			else{return FALSE;}
		}

		if (!Confirmation){
			UpperCoordinates Upset;
			ComputeSlideData(CRect(350, 427, 414, 463), CRect(373, 192, 425, 223), &Upset);
			Slide(&Upset);
			SleepEx(GetRandom(1000, 1500));
		}
	}
	return FALSE;

	if (!Confirmation){

	}
}
