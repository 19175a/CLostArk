#include "pch.h"
#include "CAstar.h"
#include <opencv2/opencv.hpp>

bool mySort(AstarPoint* p1, AstarPoint* p2)
{
	return p1->f < p2->f;
}

AstarPoint::AstarPoint()
{
	x = 0;
	y = 0;
	f = 0;
	g = 0;
	h = 0;
	parent = nullptr;
	type = AType::ATYPE_UNKNOWN;
}

AstarPoint::~AstarPoint()
{
}

CAstar::CAstar()
{
}

CAstar::~CAstar()
{
	openList.clear();
	closeList.clear();
	nearList.clear();
	dust2List.clear();
}

void CAstar::ReadMap(CString strPath)
{
	cv::Mat inputImage = cv::imread(CW2CA(strPath).GetString());

	//cv::cvtColor(inputImage, inputImage, cv::COLOR_BGR2GRAY);
	//cv::threshold(inputImage, inputImage, 250, 255, cv::THRESH_BINARY);
	//cv::imshow("显示图像", inputImage);
	//cv::waitKey();

	width = inputImage.cols;
	height = inputImage.rows;

	for (int y = 0; y < inputImage.rows; y++) {
		vector<AstarPoint*> tmp;
		vector<AstarPoint*> tmpDefault;

		for (int x = 0; x < inputImage.cols; x++) {
			AstarPoint* point = new AstarPoint();
			AstarPoint* pointDefault = new AstarPoint();

			point->x = x;
			point->y = y;

			cv::Vec3b pixel = inputImage.at<cv::Vec3b>(y, x);
			int b = pixel[0];
			int g = pixel[1];
			int r = pixel[2];

			if (b == g && g == r && r == 255)
			{
				point->type = AType::ATYPE_UNKNOWN;
			}
			else
			{
				point->type = AType::ATYPE_BARRIER;
			}

			tmp.push_back(point);

			memcpy(pointDefault, point, sizeof(AstarPoint));
			tmpDefault.push_back(pointDefault);
		}

		dust2List.push_back(tmp);
		dust2ListDefault.push_back(tmpDefault);
	}
}

void CAstar::DestoryMap()
{
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			delete dust2List[y][x];
			delete dust2ListDefault[y][x];
		}
	}
}

bool CAstar::FindPath(CPoint start, CPoint dest, std::vector<CPoint>& pathPts)
{
	AstarPoint beginPoint, endPoint;
	beginPoint.x = start.x;
	beginPoint.y = start.y;

	endPoint.x = dest.x;
	endPoint.y = dest.y;

	if (endPoint.type == AType::ATYPE_BARRIER)
		return false;
	
	if (endPoint == beginPoint)
		return false;
	
	openList.push_back(&beginPoint);
	beginPoint.type = AType::ATYPE_OPENED;
	beginPoint.f = getF(&beginPoint, endPoint);

	AstarPoint* _curPoint = nullptr;
	do
	{
		//获取最小值的节点
		_curPoint = openList[0];
		openList.erase(openList.begin());
		_curPoint->type = AType::ATYPE_CLOSED;
		closeList.push_back(_curPoint);
		
		if (*_curPoint == endPoint)
		{
			AstarPoint* end = _curPoint;

			std::vector<CPoint> tmp;
			while (end)
			{
				pathPts.insert(pathPts.begin(), CPoint(end->x, end->y));
				end = end->parent;
			}

			RestoreMapList(dust2ListDefault, dust2List);
			return true;
		}

		//获取相邻的节点
		vector<AstarPoint*> neVec = getNeighboringPoint(_curPoint);
		for (size_t i = 0; i < neVec.size(); i++)
		{
			auto tmpoint = neVec[i];
			if (tmpoint->type == AType::ATYPE_CLOSED)
				continue;

			//是否在开放列表里
			if (tmpoint->type != AType::ATYPE_OPENED)
			{
				tmpoint->parent = _curPoint;
				tmpoint->g = _curPoint->g + 10;
				//计算H值
				tmpoint->h = getH(tmpoint, endPoint);
				//添加到开放列表里
				openList.push_back(tmpoint);
				tmpoint->type = AType::ATYPE_OPENED;
			}
			else
			{
				//已经在开放列表里
				if (tmpoint->h < _curPoint->h)
				{
					tmpoint->parent = _curPoint;
					tmpoint->g = _curPoint->g + 10;
				}
			}
		}

		//排序 F值最小的排在前面
		sort(openList.begin(), openList.end(), mySort);
		
	} while (openList.size()>0);
	
	RestoreMapList(dust2ListDefault, dust2List);
	return false;
}

int CAstar::getF(AstarPoint *point, AstarPoint& endPoint)
{
	return (point->g + getH(point, endPoint));
}

int CAstar::getH(AstarPoint *point, AstarPoint& endPoint)
{
	return (abs(endPoint.y - point->y) + abs(endPoint.x - point->x))*10;
}

vector<AstarPoint*> CAstar::getNeighboringPoint(AstarPoint *point)
{
	nearList.clear();
	if (point->x < width - 1)
	{
		if (dust2List[point->y][point->x + 1]->type != AType::ATYPE_BARRIER)
		{
			nearList.push_back(dust2List[point->y][point->x + 1]);
		}
	}

	if (point->x > 0)
	{
		if (dust2List[point->y][point->x - 1]->type != AType::ATYPE_BARRIER)
		{
			nearList.push_back(dust2List[point->y][point->x - 1]);
		}
	}

	if (point->y < height - 1)
	{
		if (dust2List[point->y + 1][point->x]->type != AType::ATYPE_BARRIER)
		{
			nearList.push_back(dust2List[point->y + 1][point->x]);
		}
	}

	if (point->y >0)
	{
		if (dust2List[point->y - 1][point->x]->type != AType::ATYPE_BARRIER)
		{
			nearList.push_back(dust2List[point->y - 1][point->x]);
		}
	}
	
	return nearList;
}

void CAstar::RestoreMapList(std::vector<std::vector<AstarPoint*>>& mapListDefault, std::vector<std::vector<AstarPoint*>>& mapList)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			AstarPoint* pPointDefault = mapListDefault[y][x];
			AstarPoint* pPoint = mapList[y][x];

			pPoint->f = pPointDefault->f;
			pPoint->g = pPointDefault->g;
			pPoint->h = pPointDefault->h;
			pPoint->type = pPointDefault->type;
			pPoint->parent = nullptr;
		}
	}

	openList.clear();
	closeList.clear();
	nearList.clear();
}

cv::Mat CAstar::DrawPath(cv::Mat& matMap, std::vector<CPoint>& pts)
{
	for (int i = 0; i < pts.size(); i++)
		cv::circle(matMap, cv::Point(int(pts[i].x), int(pts[i].y)), 1, cv::Scalar(0, 0, 255));

	return matMap;
}