#pragma once
#include <opencv.hpp>

class CLineCross
{
public:
	CLineCross(){};
	~CLineCross(){};
	double GetLineAngle(const cv::Vec4i & L1, const cv::Vec4i & L2);
	//L1：线段1，L2:线段2,angleThresh:角度阈值,isLine:是否直线
	//有交点时返回true并在p中填值
	bool GetCrossPoint(const cv::Vec4i & L1, const cv::Vec4i & L2, cv::Point &p,bool isLine);
private:
	bool IsRectCross(const cv::Point &p1, const cv::Point &p2, const cv::Point &q1, const cv::Point &q2);
	bool IsLineSegmentCross(const cv::Point &pFirst1, const cv::Point &pFirst2, const cv::Point &pSecond1, const cv::Point &pSecond2);

};

