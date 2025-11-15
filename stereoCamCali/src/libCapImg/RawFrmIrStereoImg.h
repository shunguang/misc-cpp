#ifndef _RAW_FRM_IR_STEREO_IMG_H_
#define _RAW_FRM_IR_STEREO_IMG_H_

#include "libutil/util.h"
#include "RawFrmBase.h"
#include "CapDefs.h"
namespace cali {
	class CAP_EXPORT RawFrmIrStereoImg : public RawFrmBase {
	public:
		RawFrmIrStereoImg(const uint32_t imgW_ = 640, const uint32_t imgH_ = 512);
		RawFrmIrStereoImg(const RawFrmIrStereoImg &x );
		virtual ~RawFrmIrStereoImg();

		//hard copy
		virtual RawFrmIrStereoImg& operator =(const RawFrmIrStereoImg &x);

		void resetImgSize( const uint32_t imgW_, const uint32_t imgH_ );
		void loadImg(const std::string &fpath, int color_mode= cv::IMREAD_GRAYSCALE,  const uint32_t fn_=0);
		cv::Mat combineLR();
		
		void setMaskL(const cv::Mat &x) { 
			x.copyTo(maskL); 
		}

		//dump left-right images in one file
		void dumpImg(const std::string &folder, const std::string &tag = "", const uint32_t newFn=APP_NAN_UINT32, const bool tagOnly=false);

		//dump left-right images in seperate files
		void dumpImg2(const std::string &folder, const std::string &tagLeft = "left", const std::string &tagRight = "right", const uint32_t newFn = APP_NAN_UINT32);
	public:
		uint32_t		imgW;
		uint32_t		imgH;

		uint32_t				fn;
		cv::Mat				imgL;          //Left  intensity 8 bit, image 
		cv::Mat				imgR;		   //Right intensity 8 bit, image 
		cv::Mat				maskL;
	};

	typedef std::shared_ptr<RawFrmIrStereoImg>		RawFrmIrStereoImgPtr;
}

#endif
