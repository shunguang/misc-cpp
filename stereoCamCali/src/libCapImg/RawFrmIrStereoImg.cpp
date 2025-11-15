#include "RawFrmIrStereoImg.h"

using namespace std;
using namespace cali;

RawFrmIrStereoImg::RawFrmIrStereoImg(const uint32_t imgW_, const uint32_t imgH_)
	:  RawFrmBase()
	, imgW(imgW_)
	, imgH(imgH_)
	, fn(0)
	, imgL(imgH_, imgW_, CV_8UC1)
	, imgR(imgH_, imgW_, CV_8UC1)
	, maskL(imgH_, imgW_, CV_8UC1)
{
}

RawFrmIrStereoImg::RawFrmIrStereoImg(const RawFrmIrStereoImg &x)
	: RawFrmBase(x)
	, imgW(x.imgW)
	, imgH(x.imgH)
	, fn(x.fn)
{
	x.imgL.copyTo(imgL);
	x.imgR.copyTo(imgR);
	x.maskL.copyTo(maskL);
}

RawFrmIrStereoImg::~RawFrmIrStereoImg()
{
}

void RawFrmIrStereoImg::resetImgSize(const uint32_t imgW_, const uint32_t imgH_)
{
	imgW = imgW_;
	imgH = imgH_;
	imgL.create(imgH_, imgW_, CV_8UC1);
	imgR.create(imgH_, imgW_, CV_8UC1);
	maskL.create(imgH_, imgW_, CV_8UC1);
}

//hard copy
RawFrmIrStereoImg& RawFrmIrStereoImg::operator = (const RawFrmIrStereoImg &x)
{
	if (this != &x) {
		RawFrmBase::operator=(x);
		imgW = x.imgW;
		imgH = x.imgH;
		fn = x.fn;

		x.imgL.copyTo(imgL);
		x.imgR.copyTo(imgR);
		x.maskL.copyTo(maskL);

		return *this;
	}
}


#define SWITCH_L_R 0
void RawFrmIrStereoImg::loadImg( const std::string &fpath, int color_mode, const uint32_t fn_ )
{
	cv::Mat I = cv::imread( fpath, color_mode);
	assert( I.cols == 2 * imgW );
	assert(I.rows == imgH);
	assert(I.channels() == 1);

#if SWITCH_L_R
	//to hack a mistack during calibaration lepton camera on 07/06/18
	cv::Mat R = I(cv::Rect(0, 0, imgW, imgH));
	cv::Mat L = I(cv::Rect(imgW, 0, imgW, imgH));
#else
	cv::Mat L = I(cv::Rect(0, 0, imgW, imgH));
	cv::Mat R = I(cv::Rect(imgW, 0, imgW, imgH));
#endif
	L.copyTo( imgL );
	R.copyTo( imgR );
	fn = fn_;
}

void RawFrmIrStereoImg::dumpImg( const std::string &folder, const std::string &tag, const uint32_t newFn, const bool tagOnly)
{
	char buf[1024];
	if (tagOnly) {
		snprintf(buf, sizeof(buf), "%s/%s", folder.c_str(), tag.c_str() );
	}
	else {
		uint32_t fnum = (newFn != APP_NAN_UINT32) ? newFn : fn;
		if (tag.empty()) {
			snprintf(buf, sizeof(buf), "%s/frm-%05d.png", folder.c_str(), fnum);
		}
		else {
			snprintf(buf, sizeof(buf), "%s/%s-%05d.png", folder.c_str(), tag.c_str(), fnum);
		}
	}

	cv::Mat I = combineLR();

	//printf("saving %s\n", buf);
	cv::imwrite( buf, I );
}

cv::Mat RawFrmIrStereoImg::combineLR()
{
	cv::Mat I;

	if (imgL.channels() == 1)
		I.create(imgH, 2 * imgW, CV_8UC1);
	else if (imgL.channels() == 3)
		I.create(imgH, 2 * imgW, CV_8UC3);
	else
		assert(0);


	cv::Rect roi(0, 0, imgW, imgH);
	cv::Mat L = I( roi );
	imgL.copyTo(L);

	roi.x = imgW;
	cv::Mat R = I(roi);
	imgR.copyTo(R);

	return I;
}

void RawFrmIrStereoImg::dumpImg2(const std::string &folder, const std::string &tagLeft, const std::string &tagRight, const uint32_t newFn)
{
	uint32_t fnum = (newFn != APP_NAN_UINT32) ? newFn : fn;
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s/%s%05d.png", folder.c_str(), tagLeft.c_str(), fnum);
	cv::imwrite(buf, imgL);

	snprintf(buf, sizeof(buf), "%s/%s%05d.png", folder.c_str(), tagRight.c_str(), fnum);
	cv::imwrite(buf, imgR);
}
