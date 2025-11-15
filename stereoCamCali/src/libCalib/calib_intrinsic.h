//-----------------------------------------------------------
//for chess board pattern modified from: 
//     https://github.com/sourishg/stereo-calibration
// by shunguang@yahoo.com
//-----------------------------------------------------------
#ifndef __CALIB_INTRINSIC_H__
#define __CALIB_INTRINSIC_H__

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <memory>

#define USE_IR_BOARD	1

#if USE_IR_BOARD
#include "libcalibIR/thermalvis_cali/calibration.hpp"
#include "libcalibIR/thermalvis_cali/intrinsics.hpp"
#include "libCapImg/RawFrmIrStereoImg.h"
#endif

#if CV_VERSION_MAJOR >= 4
#define CV_CALIB_FIX_INTRINSIC (cv::CALIB_FIX_INTRINSIC)
#define CV_LOAD_IMAGE_COLOR (cv::IMREAD_COLOR)
#define CV_LOAD_IMAGE_GRAYSCALE (cv::IMREAD_GRAYSCALE)
#define CV_CALIB_CB_ADAPTIVE_THRESH (cv::CALIB_CB_ADAPTIVE_THRESH)
#define CV_CALIB_CB_FILTER_QUADS (cv::CALIB_CB_FILTER_QUADS)
#define CV_BGR2GRAY cv::COLOR_BGR2GRAY
#define CV_CALIB_FIX_K4 cv::CALIB_FIX_K4
#define CV_CALIB_FIX_K5 cv::CALIB_FIX_K5
//#define CV_TERMCRIT_EPS cv::TermCriteria::EPS
//#define CV_TERMCRIT_ITER cv::TermCriteria::COUNT
#endif

namespace cali {
	//do not change the order
	enum CALIB_BOARD_T {
		CALIB_BOARD_CHESS = 0,
		CALIB_BOARD_IR,
		CALIB_BOARD_UNKN,
	};

	struct CalibIntrinsicCfg {
		int  imgW, imgH;
		int board_corners_x;               //horizontal groud truth corners  for clibration board
		int board_corners_y;              //vertical groud truth corners for clibration board

		float square_size;             //chess board square side length in meters for EO, or square hole size length for IR
		float space_square_size;       //IR calibration board square space side length between squared holes for IR
		std::string imgs_directory;
		std::string imgs_filename;
		std::string extension;
		std::string preDetectedCornerFileTag;
		std::string out_file;
		int nStep2SkipFiles;

		bool        usePredetectedCorners;
		std::string corners_directory;

		int img_id_beg;
		int img_id_end;
	};

	struct MserCfg {
		int detector = 0;
		int mserDelta = 8;
		double max_var = 0.25;
		double min_diversity = 0.2;
		double area_threshold = 1.01;
		double maxCornerDistanceInPixel = 30;
	};

	class CalibIntrinsic {
	public:
		CalibIntrinsic();

		//-w 9 -h 6 -n 27 -s 0.02423 -d "../calib_imgs/1/" -i "left" -o "cam_left.yml" -e "jpg"
		int processEo(const CalibIntrinsicCfg &cfg);

#if USE_IR_BOARD
		int processIr(const CalibIntrinsicCfg &cfg, const MserCfg &cfgMers);
#endif

	private:
		void setup_calibration_chessboard(int board_corners_x, int board_corners_y, int img_id_beg, int img_id_end, float square_size,
			const char* imgs_directory, const char* imgs_filename,const char* extension);

#if USE_IR_BOARD
		int setup_calibration_irboard(int board_corners_x, int board_corners_y);
#endif

		int read_predetected_corners(int board_sq_holes_x, int board_sq_holes_y);

		double computeReprojectionErrors(const std::vector< std::vector< cv::Point3f > > &objectPoints,
			const std::vector< std::vector< cv::Point2f > >& imagePoints,
			const std::vector< cv::Mat >& rvecs, const std::vector< cv::Mat >& tvecs,
			const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs);

		int wrtYml(const CalibIntrinsicCfg& cfg, const cv::Mat& K, const cv::Mat& D, const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs, const int nUsedImgs, const double err);

	private:
		CalibIntrinsicCfg m_cfg;
		MserCfg			  m_cfgMers;
		cv::Mat img;
		std::vector< std::vector< cv::Point3f > > object_points;
		std::vector< std::vector< cv::Point2f > > image_points;
	};
	typedef std::shared_ptr<CalibIntrinsic>		CalibIntrinsicPtr;

	void calChessboardObjPoints(std::vector<cv::Point3f> &obj, const int board_corners_x, const int board_corners_y, const float square_size);
	void calIrboardObjPoints(std::vector<cv::Point3f> &obj, const int board_sq_holes_x, const int board_sq_holes_y, const float square_size, const float space_square_size);
}
#endif
