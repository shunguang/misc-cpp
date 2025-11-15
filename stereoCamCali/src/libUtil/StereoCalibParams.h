//-----------------------------------------------------------
// by shunguang@yahoo.com
//-----------------------------------------------------------
#ifndef __STEREO_CALIB_PARAMS_H__
#define __STEREO_CALIB_PARAMS_H__

#include <opencv2/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <memory>

namespace cali {
	struct StereoCalibParams {
		cv::Mat D1, D2;
		cv::Mat K1, K2;
		cv::Vec3d T;
		cv::Mat R, E,F;
		cv::Mat R1, R2, P1, P2, Q;
		cv::Rect roi1, roi2;

		void writeToYmlFile(const std::string &out_file)
		{
			cv::FileStorage fs( out_file, cv::FileStorage::WRITE );
			fs << "K1" << K1;
			fs << "K2" << K2;
			fs << "D1" << D1;
			fs << "D2" << D2;
			fs << "R" << R;
			fs << "T" << T;
			fs << "E" << E;
			fs << "F" << F;
			fs << "R1" << R1;
			fs << "R2" << R2;
			fs << "P1" << P1;
			fs << "P2" << P2;
			fs << "Q" << Q;
		}

		void readFromYmlFile(const std::string &in_file)
		{
			cv::FileStorage fs(in_file, cv::FileStorage::READ);
			fs["K1"] >> K1;
			fs["K2"] >> K2;
			fs["D1"] >> D1;
			fs["D2"] >> D2;
			fs["R"] >> R;
			fs["T"] >> T;
			fs["E"] >> E;
			fs["F"] >> F;
			fs["R1"] >> R1;
			fs["R2"] >> R2;
			fs["P1"] >> P1;
			fs["P2"] >> P2;
			fs["Q"] >> Q;
		}
	};
}
#endif
