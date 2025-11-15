#ifndef __APP_ENUMS_H__
#define __APP_ENUMS_H__

#include "DataTypes.h"
namespace cali {
	//do not change the order
	enum VIDEO_SRC_T {
		VIDEO_SRC_FROM_CAMERA = 0,   //using cv::VideoCapture
		VIDEO_SRC_FROM_V4L,		     //using linux V4L
		VIDEO_SRC_FROM_IMG_FILES,	 //using cv::VideoCapture
		VIDEO_SRC_UNKN
	};
	static const char *g_VideoSrcLabel[] = { "VIDEO_SRC_FROM_CAMERA",	"VIDEO_SRC_FROM_V4L", "VIDEO_SRC_FROM_IMG_FILES", "VIDEO_SRC_UNKN" };

	enum CAMERA_T {
		CAMERA_L = 0,
		CAMERA_R = 1,
		CAMERA_CNT = 2
	};
	static const char *g_cameraLabel[] = { "camera_left",	"camera_right", "camera_cnt" };

	enum SENSOR_T {
		SENSOR_T_BOSON320 = 0,
		SENSOR_T_BOSON640,
		SENSOR_T_LEPTON,
		SENSOR_T_RADAR,
		SENSOR_T_UNKN
	};
	static const char *g_sensorTypeLabel[] = { "SENSOR_T_BOSON320",	"SENSOR_T_BOSON640", "SENSOR_T_LEPTON", "SENSOR_T_RADAR", "SENSOR_T_UNKN" };

	enum VIDEO_MODE_T {
		V_MODE_YUV = 0, 
		V_MODE_RAW16,
		V_MODE_UNKN
	};
	static const char *g_videoModeLabel[] = { "V_MODE_YUV",	"V_MODE_RAW16", "V_MODE_UNKN"};

	enum StereoMatchAlg {
		STEREO_MATCH_BM = 0,
		STEREO_MATCH_SGBM = 1,
		STEREO_MATCH_SGBM_HH = 2,
		STEREO_MATCH_SGBM_3WAY = 3
		//STEREO_MATCH_SGBM_HH4 = 4 for opencv v3.3 and larger
	};
	static const char *g_stereoMatchAlgLabel[] = { "STEREO_BM",	"STEREO_SGBM", "STEREO_SGBM_HH", "STEREO_3WAY" };
}
#endif
