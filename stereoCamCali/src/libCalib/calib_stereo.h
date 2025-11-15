//-----------------------------------------------------------
//for chess board pattern modified from: 
//     https://github.com/sourishg/stereo-calibration
// by shunguang@yahoo.com
//-----------------------------------------------------------

#ifndef __CALIB_STEREO_H__
#define __CALIB_STEREO_H__

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <memory>

#include "libCalib/calib_intrinsic.h"
#include "libUtil/StereoCalibParams.h"

namespace cali {
	struct CalibStereoCfg {
		int img_id_beg;
		int img_id_end;
		int imgW;
		int imgH;
		std::string leftcalib_file;
		std::string rightcalib_file;
		std::string leftimg_dir;
		std::string rightimg_dir;
		std::string leftimg_filename;
		std::string rightimg_filename;

		bool        usePredetectedCorners;
		std::string cornersL_directory;
		std::string cornersR_directory;

		std::string out_file;
	};

	class CalibStereo {
	public:
		CalibStereo();
		int process(const CalibStereoCfg &cfg, const MserCfg &cfgMser, const CALIB_BOARD_T type= CALIB_BOARD_CHESS, int stepToSkipImgs=1);

	private:
		int load_image_points_chessboard(int board_corners_x, int board_corners_y, int img_id_beg, int img_id_end,  float square_size,
			const char* leftimg_dir, const char* rightimg_dir, const char* leftimg_filename, const char* rightimg_filename);

		int load_image_points_irboard(int board_sq_holes_x, int board_sq_holes_y, float square_size, float psace_square_size, int stepToSkipImgs);
		int  load_predetected_points_irboard(int board_sq_holes_x, int board_sq_holes_y, float square_size, float psace_square_size, int stepToSkipImgs);
	private:
		CalibStereoCfg	  m_cfg;
		MserCfg			  m_cfgMser;

		std::vector< std::vector< cv::Point3f > > object_points;
		std::vector< std::vector< cv::Point2f > > left_img_points, right_img_points;
		cv::Mat img1, img2;
	};
	typedef std::shared_ptr<CalibStereo>		CalibStereoPtr;
}

#endif
