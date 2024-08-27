#pragma once
using namespace std;

enum AType
{
	ATYPE_UNKNOWN,
	ATYPE_CLOSED,
	ATYPE_OPENED,
	ATYPE_BARRIER//�ϰ�
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

	AType type;//����:�ϰ��������б��ر��б�

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
	//�����ͼ����
	void ReadMap(CString strImgPath);
	void DestoryMap();

	//�ҳ�·��
	bool FindPath(CPoint start, CPoint dest, std::vector<CPoint>& pathPts);

	cv::Mat DrawPath(cv::Mat& matMap, std::vector<CPoint>& pts);

private:
	int width;
	int height;

	std::vector<std::vector<AstarPoint*>> dust2ListDefault;//��ʼ��ͼ
	std::vector<std::vector<AstarPoint*>> dust2List;//Ѱ·��ͼ Ѱ·������ ��ı��ͼ�ڵ�״̬

	std::vector<AstarPoint*> openList;//�����б� 
	std::vector<AstarPoint*> closeList;//�ر��б�
	std::vector<AstarPoint*> nearList;//�ܱ߽ڵ�

	int getF(AstarPoint *point, AstarPoint& endPoint);
	int getH(AstarPoint *point, AstarPoint& endPoint);
	std::vector<AstarPoint*> getNeighboringPoint(AstarPoint* point);

	void RestoreMapList(std::vector<std::vector<AstarPoint*>>& mapListDefault, std::vector<std::vector<AstarPoint*>>& mapList);
};