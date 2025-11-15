//-----------------------------------------------------------
//undistortion and rectify raw images
//     https://github.com/sourishg/stereo-calibration
// by shunguang@yahoo.com
//-----------------------------------------------------------
#ifndef __CALIB_RECTIFY_H__
#define __CALIB_RECTIFY_H__

#include <opencv2/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <memory>

#include "libUtil/CvUtilFuncs.h"
#include "libUtil/StereoCalibParams.h"
#include "libCapImg/RawFrmIrStereoImg.h"

namespace cali {
	class CalibUndist {
	public:
		CalibUndist(const cv::Size imgSz, const int imgType=CV_8UC1 ) : img_sz(imgSz), img_type(imgType) {};
		~CalibUndist() = default;

		void loadCamarams(const std::string& ymlFilePath) {
				cv::FileStorage fs(ymlFilePath, cv::FileStorage::READ);
				fs["K"] >> K;
				fs["D"] >> D;

				std::cout << K << std::endl;
				std::cout << D << std::endl;
		}

		void undistImg(const std::string inFile, const std::string outFile, const char lrFlag)
		{
			const int w = img_sz.width;
			const int h = img_sz.height;
			RawFrmIrStereoImg frm(w, h);
			frm.loadImg(inFile);

			cv::Mat I(h, 2 * w, img_type);
			cv::Rect r(0, 0, w, h);
			cv::Mat rectI = I(r);
			cv::Mat undist;
			if (lrFlag == 'L' || lrFlag == 'l') {
				cv::undistort(frm.imgL, undist, K, D);
				frm.imgL.copyTo(rectI);
				//imshow("L", frm.imgL);
				//cv::waitKey(1);
			}
			else
			{
				cv::undistort(frm.imgR, undist, K, D);
				frm.imgR.copyTo(rectI);
			}

			//imshow("DL", undist);
			//cv::waitKey(1);

			r.x = w;
			rectI = I(r);      //get head
			undist.copyTo(rectI);

			imwrite(outFile, I);
		}

		void undistImg(const cv::Mat& img, cv::Mat& out)
		{
			cv::undistort(img, out, K, D);
		}

	private:
		cv::Mat K, D, R, t;  //P=[K, {R|t}]
		cv::Size img_sz;
		int      img_type;
	};
	typedef std::shared_ptr<CalibUndist>		CalibUndistPtr;

	class CalibRectify {
	public:
		CalibRectify();
		void loadCalibParams( const std::string &filePath );
		void init(const cv::Size imgSz);

		void computeRectifyImg(const cv::Mat &imgL, const cv::Mat &imgR, cv::Mat &imgUL, cv::Mat &imgUR);
		void computeRectifyImg(const std::string &leftimg_filename, const std::string &rightimg_filename, const int type, const std::string &oneout_filename);
		void computeRectifyImg(const std::string &leftimg_filename, const std::string &rightimg_filename, const int type, const std::string &leftout_filename, const std::string &rightout_filename);
	private:
		StereoCalibParams calibParams;
		cv::Mat lmapx, lmapy, rmapx, rmapy;
		cv::Size img_sz;
	};

	typedef std::shared_ptr<CalibRectify>		CalibRectifyPtr;
}
#endif
