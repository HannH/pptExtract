#include <opencv.hpp>
#include <vector>
#include <math.h>
#include <string>
#include "LineCross.h"
#include "ShowImg.h"

#define range(vec) sqrt((vec[2]-vec[0])*(vec[2]-vec[0])+(vec[1]-vec[3])*(vec[1]-vec[3]))
using namespace std;
using namespace cv;
const char *name = R"(E:\c++ excise\pptExtract\ISPRS=PPT照片\2\IMG_20160713_090009.jpg)";
const char *name2 = R"(E:\c++ excise\pptExtract\ISPRS=PPT照片\IMG_20160717_092915.jpg)";
const char *name3 = R"(E:\c++ excise\pptExtract\ISPRS=PPT照片\IMG_20160713_142613.jpg)";


int main(int argc,char** argv){
	if (argc == 1 || argc > 4) {
		cout << "usage: [exe] [Input image path] [output dictory] [is manual(0 or 1)]";
		return 1;
	}
	//strcpy(argv[1], name);
	string name = argv[1];
	name.replace(0, name.find_last_of('\\')+1,"");
	name.replace(name.find_last_of('.'),name.size()+1,"");
	CShowImg aaa;
	Mat src = imread(argv[1], IMREAD_COLOR),lineImg,hls;
	vector<Mat> rgb;
	vector<Point> anglePoint(4);
	int i=0;
	if(!strcmp(argv[3],"0")){
		split(src, rgb);
		//影像预处理（蓝波段特征）
		Mat preImg = rgb[0]/3 + rgb[1]/3 + rgb[2]/3,cutImg, cutImg_clr,cutImg_gray, edge;
		threshold(preImg, cutImg_clr, 50, 255, THRESH_OTSU);
		//cvtColor(src, hls, COLOR_RGB2HLS);
		//split(hls, rgb);
		//Mat preImg=rgb[1], cutImg, edge;
		//rgb[1] =  preImg ;
		//threshold(rgb[1], cutImg_gray, 50, 255, THRESH_OTSU);
		/*aaa.readImg("1", cutImg_gray)->show(1, 1, 0);
		aaa.readImg("2", cutImg_clr)->show(1, 1, 0);
		int chooseI = waitKey(0);
		switch (chooseI){
		case 49:cutImg = cutImg_gray; break;
		case 50:cutImg = cutImg_clr; break;
		default: 
			cout << "wrong choose!" << endl;
			cutImg = cutImg_clr - 254 + cutImg_gray - 255;
			break;
		}*/
		//aaa.readImg("cut", src)->show(1, 1, 1);
		morphologyEx(cutImg_clr, cutImg, MORPH_OPEN,
			getStructuringElement(MORPH_RECT, Size(30, 50)), Point(-1, -1), 2);
		//边缘检测
		Laplacian(cutImg, edge, CV_8U, 3);
		vector<Vec4i> lines;
		morphologyEx(edge, edge, MORPH_DILATE,
			getStructuringElement(MORPH_RECT, Size(3, 3)));
		HoughLinesP(edge, lines, 1, CV_PI / 180, 50, 10, 10);
		//检查线位置
		/*for (int i = 0; i < lines.size(); i++)
			line(src, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255),10);
			aaa.readImg("src", src)->show(1, 1, 1);*/
		sort(lines.begin(), lines.end(), [](Vec4i a, Vec4i b){
			return range(a) > range(b);
		});
		/*for (int i = 0; i < lines.size(); i++)
			line(src, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]),Scalar(255,0,0));*/
		CLineCross lineCrs;
		vector<Vec4i> ansLine(4);
		double minmaxArr[4] = { 1e6, 0, 1e6, 0 };//minx,maxx,miny,maxy
		//直线筛选
		int j = 0, cnt = 0;
		vector<int> lineNo(4);//边缘直线号
		for (int j = 0; j < 4; j++)
			for (i = 0; i < lines.size(); i++){
				float weightPoint[2] = { (lines[i][0] + lines[i][2]) / 2, (lines[i][1] + lines[i][3]) / 2 };
				if ((weightPoint[j / 2] - minmaxArr[j])*(j % 2 * 2 - 1) > 0)	minmaxArr[j] = weightPoint[j / 2], lineNo[j] = i;
			}
		for (i = 0; i < 4; i++)
			ansLine[i] = lines[lineNo[i]];
		//检查线位置
		/*for (int i = 0; i < ansLine.size(); i++)
			line(src, Point(ansLine[i][0], ansLine[i][1]), Point(ansLine[i][2], ansLine[i][3]), Scalar(0, 0, 255),10);
		aaa.readImg("src", src)->show(1, 1, 1);*/
		//l,r,t,b
		//求交点
		lineCrs.GetCrossPoint(ansLine[0], ansLine[2], anglePoint[0], 1);//lt
		lineCrs.GetCrossPoint(ansLine[1], ansLine[2], anglePoint[1], 1);//rt
		lineCrs.GetCrossPoint(ansLine[0], ansLine[3], anglePoint[2], 1);//lb
		lineCrs.GetCrossPoint(ansLine[1], ansLine[3], anglePoint[3], 1);//lt
	}
	//检查匹配点位置代码
	/*Mat dst1 = src.clone();
	for (i = 0; i < 4; i++)
		circle(dst1, anglePoint[i], 10, Scalar(0, 0, 255), 10);
	aaa.readImg("src", dst1)->show(1,1,0);*/
	//空格进行人工编辑
	if (!strcmp(argv[3], "1")){
		manual:
		aaa.readImg(name.c_str(), src)->show(0, 1, 0);
		Point ang;
		setMouseCallback(name.c_str(), [](int event, int x, int y, int flags, void* param){
			if (event == CV_EVENT_LBUTTONUP){
				Point *l = (Point *)param;
				l->x = x, l->y = y;
			}
		}, &ang);
		double scale = aaa.getScale();
		for (i = 0; i < 4; i++){
			switch (i){
			case 0:cout << "top left" << endl; break;
			case 1:cout << "top right" << endl; break;
			case 2:cout << "bottom left" << endl; break;
			case 3:cout << "bottom right" << endl; break;
			}
			if (waitKey(0) == 122)
				i--;
			ang *= scale;
			anglePoint[i] = ang;
			cout << "该像素位置为: x=" << ang.x << " y=" << ang.y << endl;
		}
	}

	Point center(0, 0);
	for (i = 0; i < 4; i++)
		center += anglePoint[i];
	center /= 4;
	//依次在图中绘制出每各交点
	/*for (i = 0; i < 4; i++)
		circle(src, anglePoint[i], 10, Scalar(0, 0, 255), 3);*/

	//单应点集
	vector<Point> dstPoint(4);
	int pptSize[2] = { 1000, 700 };
	pptSize[0] = src.cols / 2, pptSize[1] = src.rows / 2;
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
	//cout << srcAngleMat << endl << dstAngleMat << endl;;
	warpPerspective(src, resault, M, Size(0, 0));
	resault = resault(Rect(0, 0, pptSize[0], pptSize[1]));
	// 检测直线，最小投票为90，线条不短于50，间隙不小于10
	aaa.readImg("resault", resault)->show(1,1,0);
	if (waitKey(0) == 32) goto manual;
	imwrite(string(argv[2]) + "\\" + name + "_context.jpg", resault);
	return 0;
}