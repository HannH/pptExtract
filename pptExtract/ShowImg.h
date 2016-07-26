#pragma once
#include <opencv.hpp>
#include <vector>
using namespace cv;

struct ImageInfo{ int dataType, w, h; void* dp; double xScale, yScale; };

class CShowImg
{
public:
	CShowImg();
	~CShowImg();
	CShowImg * readImg(const char *winName, cv::InputArray mat);
	void show(bool clickDN=true,bool strech=true,bool needWait=true);
	double getScale(){ return m_Info.xScale; };
private:
	void stretch(int stretchMethod = 1);
	void setNodataValue(int NodataValue, int showValue = 0);
	void setClickShowDN();
	bool getImgInfo();
	template<typename dp> bool useLookupTable(InputArray, const std::vector<int>&, OutputArray,int band=1,int bands=1);
	Mat m_srcImg,m_showImg;
	ImageInfo m_Info;
	const char *m_pWinName;

};

