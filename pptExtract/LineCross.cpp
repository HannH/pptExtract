#include <stdlib.h>
#include <math.h>
#include "LineCross.h"

using namespace cv;

//排斥实验
bool CLineCross::IsRectCross(const Point &p1, const Point &p2, const Point &q1, const Point &q2)
{
	bool ret = min(p1.x, p2.x) <= max(q1.x, q2.x) &&
		min(q1.x, q2.x) <= max(p1.x, p2.x) &&
		min(p1.y, p2.y) <= max(q1.y, q2.y) &&
		min(q1.y, q2.y) <= max(p1.y, p2.y);
	return ret;
}
//跨立判断
bool CLineCross::IsLineSegmentCross(const Point &pFirst1, const Point &pFirst2, const Point &pSecond1, const Point &pSecond2)
{
	long line1, line2;
	line1 = pFirst1.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond1.y) +
		pSecond1.x * (pFirst2.y - pFirst1.y);
	line2 = pFirst1.x * (pSecond2.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pFirst2.y - pFirst1.y);
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;

	line1 = pSecond1.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst1.y) +
		pFirst1.x * (pSecond2.y - pSecond1.y);
	line2 = pSecond1.x * (pFirst2.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pSecond2.y - pSecond1.y);
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;
	return true;
}
double CLineCross::GetLineAngle(const cv::Vec4i & L1, const cv::Vec4i & L2){
	double angle = atan((L1[1] - L1[3]) / (L1[0] - L1[2])) - atan((L2[1] - L2[3]) / (L2[0] - L2[2]));
	return angle;
}
bool CLineCross::GetCrossPoint(const cv::Vec4i & L1, const cv::Vec4i & L2, cv::Point &p,bool isLine)
{
	Point p1, p2, q1, q2;
	p1.x = L1[0], p1.y = L1[1]; p2.x = L1[2], p2.y = L1[3];
	q1.x = L2[0], q1.y = L2[1]; q2.x = L2[2], q2.y = L2[3];
	double angle = atan((p1.y - p2.y) / (p1.x - p2.x)) - atan((q1.y - q2.y) / (q1.x - q2.x));
	long x, y;
	if (isLine||IsRectCross(p1, p2, q1, q2) )
	{
		if (isLine || IsLineSegmentCross(p1, p2, q1, q2))
		{
			//求交点
			long tmpLeft, tmpRight;
			tmpLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
			tmpRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);

			x = (int)((double)tmpRight / (double)tmpLeft);

			tmpLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
			tmpRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x - p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y);
			y = (int)((double)tmpRight / (double)tmpLeft);
			p.x = x, p.y = y;
			return true;
		}
	}
	return false;
}