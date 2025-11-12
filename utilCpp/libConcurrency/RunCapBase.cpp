#include "RunCapBase.h"

using namespace std;
using namespace app;

RunCapBase::RunCapBase(const CameraType_t camType, const int threadId )
	: RunThreadX(threadId, "Capture_thread")
	, m_camType(camType)
	, m_frmInterval_ms(40)
	, m_wamiThread(NULL)
	, m_recThread(NULL)
	, m_pClientMgr(NULL)
	, m_isWamiInRecMode( false )
{
}

RunCapBase::~RunCapBase()
{
}

cv::Mat RunCapBase::ConvertBGR2Bayer(const cv::Mat &BGRImage)
{
	/*
	Assuming a Bayer filter that looks like this:
	# // 0  1  2  3  4  5
	/////////////////////
	0 // B  G  B  G  B  G
	1 // G  R  G  R  G  R
	2 // B  G  B  G  B  G
	3 // G  R  G  R  G  R
	4 // B  G  B  G  B  G
	5 // G  R  G  R  G  R
	*/
	cv::Mat BayerImage(BGRImage.rows, BGRImage.cols, CV_8UC1);
	int channel;
	for (int row = 0; row < BayerImage.rows; row++) {
		for (int col = 0; col < BayerImage.cols; col++) {
			if (row % 2 == 0) {
				//even columns and even rows = blue = channel:0
				//even columns and uneven rows = green = channel:1 
				channel = (col % 2 == 0) ? 0 : 1;
			}
			else {
				//uneven columns and even rows = green = channel:1
				//uneven columns and uneven rows = red = channel:2 
				channel = (col % 2 == 0) ? 1 : 2;
			}
			BayerImage.at<uchar>(row, col) = BGRImage.at<cv::Vec3b>(row, col).val[channel];
		}
	}
	return BayerImage;
}
