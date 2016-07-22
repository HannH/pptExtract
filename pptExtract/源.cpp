#include <opencv.hpp>
#include <vector>
#include <math.h>
#include "LineCross.h"
#include "ShowImg.h"

#define range(vec) sqrt((vec[2]-vec[0])*(vec[2]-vec[0])+(vec[1]-vec[3])*(vec[1]-vec[3]))
using namespace std;
using namespace cv;
const char *name = R"(F:\迅雷下载\IMG_20160714_111518.jpg)";

void drawDetectLines(Mat& image, const vector<Vec4i>& lines, Scalar & color) {
	// 将检测到的直线在图上画出来
	vector<Vec4i>::const_iterator it = lines.begin();
	while (it != lines.end())
	{
		Point pt1((*it)[0], (*it)[1]);
		Point pt2((*it)[2], (*it)[3]);
		line(image, pt1, pt2, color, 2); //  线条宽度设置为2
	}
}

int main(){
	CShowImg aaa;
	Mat src = imread(name, IMREAD_COLOR),lineImg;
	vector<Mat> rgb;
	split(src, rgb);
	Mat preImg = rgb[0] - (rgb[1] + rgb[2]) / 2, cutImg, edge;
	//aaa.readImg("b", rgb[0] - (rgb[1] + rgb[2]) / 2)->show(1,1,0);
	morphologyEx(preImg, preImg, MORPH_OPEN,
		getStructuringElement(MORPH_RECT, Size(30, 30)),Point(-1,-1),2);
	threshold(preImg, cutImg, 5, 255, THRESH_BINARY);
	/*vector<vector<Point>> contours;
	findContours(cutImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	vector<Point> qudrangle;
	approxPolyDP(Mat(contours[0]), qudrangle, 5, 1);
	drawContours(src, contours, -1, Scalar(0, 0, 255));
	vector<Vec4i> lines;*/
	Laplacian(cutImg, edge, CV_8U, 3);
	vector<Vec4i> lines;
	morphologyEx(edge, edge, MORPH_DILATE,
		getStructuringElement(MORPH_RECT, Size(3, 3)), Point(-1, -1), 1);
	//aaa.readImg("edge", edge)->show(1, 1, 1);
	//imshow("edge", edge); waitKey(0);
	HoughLinesP(edge, lines, 5, 0.05, 300, 100, 10);
	/*sort(lines.begin(), lines.end(), [](Vec4i a, Vec4i b){
		return range(a) > range(b);
	});*/
	CLineCross lineCrs;
	vector<Vec4i> ansLine;
	//直线筛选
	int i = 0,j=0,cnt=0;
	for (i = 0; i < 10; i++){
		cnt = 0;
		for (j = 0; j < ansLine.size(); j++)
			if (abs(lineCrs.GetLineAngle(ansLine[j], lines[i]))<0.8)
				cnt++ ;
		if (cnt < 2)
			ansLine.push_back(lines[i]);
	}
	//求交点
	vector<Point> anglePoint;
	for (i = 0; i < 4; i++){
		for (j = i+1; j < 4;j++)
			if (abs(lineCrs.GetLineAngle(ansLine[i],ansLine[j]))<0.8)
				continue;
			else{
				Point t_point;
				if (lineCrs.GetCrossPoint(ansLine[i], ansLine[j], t_point,true))
					anglePoint.push_back(t_point);
			}
	}
	Point center(0, 0);
	for (i = 0; i < 4; i++)
		center += anglePoint[i];
	center /= 4;
	//【4】依次在图中绘制出每条线段
	for (i = 0; i < 4; i++)
		circle(src, anglePoint[i], 10, Scalar(0, 255, 0));
	vector<Point> dstPoint(4);
	int pptSize[2] = { 1000, 700 };
	for (i = 0; i < 4; i++){
		switch ((anglePoint[i].x < center.x) + (anglePoint[i].y < center.y) * 2){
		case 0:dstPoint[i] = Point(pptSize[0], pptSize[1]); break;
		case 1:dstPoint[i] = Point(0, pptSize[1]); break;
		case 2:dstPoint[i] = Point(pptSize[0], 0); break;
		case 3:dstPoint[i] = Point(0, 0); break;
		}
	}
	Mat srcAngleMat(4,2,CV_32F), dstAngleMat(4,2,CV_32F);
	for (i = 0; i < 4; i++){
		srcAngleMat.at<float>(i, 0) = anglePoint[i].x, srcAngleMat.at<float>(i, 1) = anglePoint[i].y;
		dstAngleMat.at<float>(i, 0) = dstPoint[i].x, dstAngleMat.at<float>(i, 1) = dstPoint[i].y;
	}
	Mat M = getPerspectiveTransform(srcAngleMat, dstAngleMat);
	Mat resault;
	cout << srcAngleMat << endl << dstAngleMat << endl;;
	warpPerspective(src, resault, M, Size(0, 0));
	resault = resault(Rect(0, 0, pptSize[0], pptSize[1]));
	// 检测直线，最小投票为90，线条不短于50，间隙不小于10
	aaa.readImg("src", resault)->show(1, 1, 1);
	
	
	return 0;
}