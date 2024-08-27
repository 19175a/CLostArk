#pragma once
using namespace std;

enum AType
{
	ATYPE_UNKNOWN,
	ATYPE_CLOSED,
	ATYPE_OPENED,
	ATYPE_BARRIER//障碍
};

class AstarPoint
{
public:
	AstarPoint();
	~AstarPoint();

	int x;
	int y;
	int f;	//f = g+h
	int g;
	int h;

	AType type;//类型:障碍、开放列表、关闭列表

	AstarPoint *parent;
	bool operator == (const AstarPoint& po)
	{
		if (x == po.x && y == po.y)
			return true;

		return false;
	}
};

class CAstar
{
public:
	CAstar();
	~CAstar();

public:
	//读入地图数据
	void ReadMap(CString strImgPath);
	void DestoryMap();

	//找出路径
	bool FindPath(CPoint start, CPoint dest, std::vector<CPoint>& pathPts);

	cv::Mat DrawPath(cv::Mat& matMap, std::vector<CPoint>& pts);

private:
	int width;
	int height;

	std::vector<std::vector<AstarPoint*>> dust2ListDefault;//初始地图
	std::vector<std::vector<AstarPoint*>> dust2List;//寻路地图 寻路过程中 会改变地图节点状态

	std::vector<AstarPoint*> openList;//开放列表 
	std::vector<AstarPoint*> closeList;//关闭列表
	std::vector<AstarPoint*> nearList;//周边节点

	int getF(AstarPoint *point, AstarPoint& endPoint);
	int getH(AstarPoint *point, AstarPoint& endPoint);
	std::vector<AstarPoint*> getNeighboringPoint(AstarPoint* point);

	void RestoreMapList(std::vector<std::vector<AstarPoint*>>& mapListDefault, std::vector<std::vector<AstarPoint*>>& mapList);
};