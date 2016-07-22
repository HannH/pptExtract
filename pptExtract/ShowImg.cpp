#include "ShowImg.h"
#include <iostream>
#include <math.h>
using namespace std;

CShowImg::CShowImg()
{
}
CShowImg::~CShowImg()
{
}

CShowImg* CShowImg::readImg(const char *winName, cv::InputArray mat){
	m_srcImg = mat.getMat();
	m_pWinName = winName;
	return this;
}
bool CShowImg::getImgInfo(){
	if (m_srcImg.size().area() == 0)	{ cerr << "input image size=0" << endl; return false; };
	const double windowWidth = 1600, windowHeight = 900;
	m_Info.w = m_srcImg.cols, m_Info.h = m_srcImg.rows; m_Info.dp = m_srcImg.data;
	double k = m_srcImg.cols / windowWidth > m_srcImg.rows / windowHeight ? m_srcImg.cols / windowWidth : m_srcImg.rows / windowHeight;
	m_Info.xScale = m_Info.yScale = k;
	switch (m_srcImg.type()){
	case CV_8U:		m_Info.dataType = 1; break;
	case CV_8UC3:	 m_Info.dataType = 2; break;
	case CV_16U:	m_Info.dataType = 3; break;
	case CV_16UC3:  m_Info.dataType = 4; break;
	case CV_32F:  m_Info.dataType = 5; break;
	case CV_32FC3:  m_Info.dataType = 6; break;
	default: 	cerr << "wrong image type ,type=" << m_srcImg.type() << endl; return false;
	}
	resize(m_srcImg, m_showImg, Size(m_srcImg.cols / k, m_srcImg.rows / k));
	return true;
}
void CShowImg::setClickShowDN(){
	setMouseCallback(m_pWinName, [](int event, int x, int y, int flags, void* param){
		ImageInfo tInfo = *((ImageInfo *)param);
		switch (event){
		case CV_EVENT_LBUTTONUP:
			x *= tInfo.xScale, y *= tInfo.yScale;
			if (x < 0 || y < 0) return;
			cout << "该像素位置为: x=" << x << " y=" << y << ", 像素值为 ";
			switch (tInfo.dataType){
			case 1: cout << (int)((uchar*)tInfo.dp)[y*tInfo.w + x]; break;
			case 2:
				for (int i = 0; i < 3; i++)
					cout << "波段" << i << "为" << (int)((uchar*)tInfo.dp)[(y*tInfo.w + x) * 3 + i] << "  ";
				break;
			case 3:cout << (int)((ushort*)tInfo.dp)[y*tInfo.w + x]; break;
			case 4:
				for (int i = 0; i < 3; i++)
					cout << "波段" << i << "为" << (int)((ushort*)tInfo.dp)[(y*tInfo.w + x) * 3 + i] << "  ";
				break;
			case 5:cout << (float)((float*)tInfo.dp)[y*tInfo.w + x]; break;
			case 6:
				for (int i = 0; i < 3; i++)
					cout << "波段" << i << "为" << (float)((float*)tInfo.dp)[(y*tInfo.w + x) * 3 + i] << "  ";
				break;
			default: cout << tInfo.dataType; break;
			}
			cout << endl;
			break;
		}
	}, &m_Info);
}
void CShowImg::setNodataValue(int NodataValue, int showValue){
	if (NodataValue == showValue)
		return;
	Mat t_src = m_srcImg - (NodataValue - 1);
	m_srcImg.setTo(0, t_src);
}
template<typename dp> bool CShowImg::useLookupTable(InputArray src, const vector<int>& lut, OutputArray dst, int band, int bands){
	Mat tSrc = src.getMat();
	int area = tSrc.cols*tSrc.rows;
	dp *data = (dp *)tSrc.data;
	for (int i = band; i < area*bands; i += bands){
		if (data[i] < 0 || data[i]>65535){
			cout << "data error" << endl << " pos:" << i << " value:" << data[i]
				<<endl<<"fail to stretch ,please retry"<<endl;
			dst.assign(src.getMat());
			return false;
		}
		data[i] = lut[data[i]];
	}
	dst.assign(tSrc);
	return true;
}
void CShowImg::stretch(int stretchMethod){
	int pixNum = m_showImg.cols*m_showImg.rows,band;
	Mat Hist;
	vector<int> Lut;
	vector<Mat> t_vecImg(1, m_showImg);
	if (m_Info.dataType % 2)
		band = 1;
	else band = 3;
	for (int k = 0; k < band;k++){
		vector<int> t_channel (1), t_size = { 65536 }; vector<float> t_range = { 0, 65535 };//有问题
		t_channel[0] = k;
		calcHist(t_vecImg, t_channel, noArray(), Hist, t_size, t_range);
		Lut.resize(Hist.rows);
		switch (stretchMethod){
		case 1:{
			double headCut = 0.0025, tailCut = 0.0025, pbtSum = 0;
			int headCnt, tailCnt, i;
			for (i = 0; pbtSum < headCut&&i < Hist.rows; i++)
				pbtSum += ((float *)Hist.data)[i] / pixNum;
			pbtSum = 0; headCnt = i;
			for (i = Hist.rows - 1; pbtSum < tailCut&&i > 0; i--)
				pbtSum += ((float *)Hist.data)[i] / pixNum;
			pbtSum = 0; tailCnt = i;
			for (i = 0; i < Hist.rows; i++){
				if (i < headCnt)
					Lut[i] = 0;
				else if (i > tailCnt)
					Lut[i] = 255;
				else Lut[i] = 255 * (i - headCnt) / (tailCnt - headCnt);
			}
			break;
		}
		case 2:	{
			double gamma = 0.8;
			for (int i = 0; i < Hist.rows; i++)
				Lut[i] = pow(i, gamma);
			break;
		}
		case 3:
			equalizeHist(m_showImg, m_showImg);
			break;
		}
		if (stretchMethod != 3){
			switch (m_Info.dataType){
			case 1:	useLookupTable<uchar>(m_showImg, Lut, m_showImg, k, band); break;
			case 2:	useLookupTable<uchar>(m_showImg, Lut, m_showImg, k, band); break;
			case 3:	useLookupTable<ushort>(m_showImg, Lut, m_showImg, k, band); break;
			case 4:	useLookupTable<ushort>(m_showImg, Lut, m_showImg, k, band); break;
			case 5:	useLookupTable<float>(m_showImg, Lut, m_showImg, k, band); break;
			case 6:	useLookupTable<float>(m_showImg, Lut, m_showImg, k, band); break;
			}
		}
	}
	if (m_Info.dataType%2)
		m_showImg.convertTo(m_showImg, CV_8U);
	else m_showImg.convertTo(m_showImg, CV_8UC3);
}
void CShowImg::show(bool clickDN, bool strech,bool needWait){
	if (getImgInfo() == false){
		cerr << "Wrong imageinfo in show!" << endl;
		return;
	}
	cout << "Now is showing " << m_pWinName << " ,the xsize is " << m_srcImg.cols
		<< " the ysize is " << m_srcImg.rows << endl << "The type is ";
	switch (m_Info.dataType){
	case 1:	cout << "8UC1" << endl; break;
	case 2:	cout << "8UC3" << endl; break;
	case 3:	cout << "16UC1" << endl; break;
	case 4: cout << "16UC3" << endl; break;
	case 5: cout << "32FC1" << endl; break;
	case 6: cout << "32FC3" << endl; break;
	}
	if (strech)
		stretch();
	imshow(m_pWinName, m_showImg);
	if (clickDN)
		setClickShowDN();
	if (needWait)
		waitKey(0);
}