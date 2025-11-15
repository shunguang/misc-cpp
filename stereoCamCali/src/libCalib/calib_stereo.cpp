#include "calib_stereo.h"

using namespace std;
using namespace cv;
using namespace cali;

CalibStereo::CalibStereo()
{
}

int CalibStereo::process(const CalibStereoCfg &cfg, const MserCfg &cfgMers, const CALIB_BOARD_T type, int stepToSkipImgs)
{
	m_cfg = cfg;
	m_cfgMser = cfgMers;

	FileStorage fsl(cfg.leftcalib_file, FileStorage::READ);
	FileStorage fsr(cfg.rightcalib_file, FileStorage::READ);

	int nx = fsl["board_corners_x"];
	int ny = fsl["board_corners_y"];
	float square_size = fsl["square_size"];
	int nUsedImgs = 0;
	if (type == CALIB_BOARD_CHESS) {
		nUsedImgs = load_image_points_chessboard(nx, ny, cfg.img_id_beg, cfg.img_id_end, square_size,
			cfg.leftimg_dir.c_str(), cfg.rightimg_dir.c_str(), 
			cfg.leftimg_filename.c_str(), cfg.rightimg_filename.c_str());
	}
	else if (type == CALIB_BOARD_IR) {
		float space_square_size = fsl["space_square_size"];
		int board_sq_holes_x = nx / 2,
			board_sq_holes_y = ny / 2;
		if (cfg.usePredetectedCorners) {
			nUsedImgs = load_predetected_points_irboard(board_sq_holes_x, board_sq_holes_y, square_size, space_square_size, stepToSkipImgs);
		}
		else {
			nUsedImgs = load_image_points_irboard(board_sq_holes_x, board_sq_holes_y, square_size, space_square_size, stepToSkipImgs);
		}
	}
	else {
		assert(0);
	}

	if (object_points.size() < 1) {
		printf("no correspondig pts found!\n");
		return -1;
	}

	//read intrisic params from cam1 and cam2
	printf("Starting Read intrinsics\n");
	Mat K1, K2, R, F, E;
	Vec3d T;
	Mat D1, D2;
	fsl["K"] >> K1;
	fsr["K"] >> K2;
	fsl["D"] >> D1;
	fsr["D"] >> D2;
	cout << "Read intrinsics done!" << endl;


	cout << "start Calibration: compute R,T,E,F ..." << endl;
	cv::Size imgSize(cfg.imgW, cfg.imgH);
	int flags = 0;
	flags |= CV_CALIB_FIX_INTRINSIC;
	cv::stereoCalibrate(object_points, left_img_points, right_img_points, K1, D1, K2, D2, imgSize, R, T, E, F, flags);
	
	printf("Done Calibration\n");

	printf("Starting calculate Rectification params\n");
	cv::Mat R1, R2, P1, P2, Q;

	//todo: figure out flags
	//compute: R1, R2, P1, P2, Q, roi1, roi2
	//flags = CV_CALIB_ZERO_DISPARITY;
	cv::Rect roi1, roi2;
	flags = 0; 
	double alpha = -1;
	cv::stereoRectify(K1, D1, K2, D2, imgSize, R, T, R1, R2, P1, P2, Q, flags, alpha, imgSize, &roi1, &roi2);
	/*
	CV_EXPORTS_W void stereoRectify(InputArray cameraMatrix1, InputArray distCoeffs1,
	InputArray cameraMatrix2, InputArray distCoeffs2,
	Size imageSize, InputArray R, InputArray T,
	OutputArray R1, OutputArray R2,
	OutputArray P1, OutputArray P2,
	OutputArray Q, int flags = CALIB_ZERO_DISPARITY,
	double alpha = -1, Size newImageSize = Size(),
	CV_OUT Rect* validPixROI1 = 0, CV_OUT Rect* validPixROI2 = 0);
	*/

	printf("Done Rectification\n");

	//save results
	StereoCalibParams p;
	p.D1 = D1; p.D2 = D2;
	p.K1 = K1; p.K2 = K2;
	p.P1 = P1; p.P2 = P2;
	p.R1 = R1, p.R2 = R2;
	p.T = T; p.R = R, p.Q = Q;
	p.E = E;   p.F = F;

	p.writeToYmlFile(cfg.out_file + num_to_string(nUsedImgs, "_usedImgs") + ".yml");

	return nUsedImgs;
}

int CalibStereo::load_image_points_chessboard(int board_corners_x, int board_corners_y, int img_id_beg, int img_id_end,  float square_size,
	const char* leftimg_dir, const char* rightimg_dir, const char* leftimg_filename, const char* rightimg_filename) 
{
	int nUsedImgs = 0;
	Size board_size = Size(board_corners_x, board_corners_y);
	int board_n = board_corners_x * board_corners_y;

	vector< Point3f > obj;
	calChessboardObjPoints(obj, board_corners_x, board_corners_y, square_size);

	cv::Mat gray1, gray2;
	std::vector< std::vector< cv::Point2f > > imagePoints1, imagePoints2;
	std::vector< cv::Point2f > corners1, corners2;

	object_points.clear();
	left_img_points.clear();
	right_img_points.clear();

	for (int i = img_id_beg; i <= img_id_end; i++) {
		char left_img[100], right_img[100];
		sprintf(left_img, "%s%s%d.jpg", leftimg_dir, leftimg_filename, i);
		sprintf(right_img, "%s%s%d.jpg", rightimg_dir, rightimg_filename, i);
		img1 = imread(left_img, CV_LOAD_IMAGE_COLOR);
		img2 = imread(right_img, CV_LOAD_IMAGE_COLOR);
		cvtColor(img1, gray1, CV_BGR2GRAY);
		cvtColor(img2, gray2, CV_BGR2GRAY);

		bool found1 = false, found2 = false;

		found1 = cv::findChessboardCorners(img1, board_size, corners1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		found2 = cv::findChessboardCorners(img2, board_size, corners2, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		if (found1){
			cv::cornerSubPix(gray1, corners1, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray1, board_size, corners1, found1);
			sprintf(left_img, "c:/temp/calib/%s%d.jpg", leftimg_filename, i);
			cv::imwrite(left_img, gray1);
		}
		if (found2){
			cv::cornerSubPix(gray2, corners2, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray2, board_size, corners2, found2);
			sprintf(right_img, "c:/temp/calib/%s%d.jpg", rightimg_filename, i);
			cv::imwrite(right_img, gray2);
		}


		if (found1 && found2) {
			cout << i << ". Found corners!" << endl;
			left_img_points.push_back(corners1);
			right_img_points.push_back(corners2);
			object_points.push_back(obj);
			nUsedImgs++;
		}
	}
	return nUsedImgs;
}

int CalibStereo::load_image_points_irboard(int board_sq_holes_x, int board_sq_holes_y, float square_size, float space_square_size, int stepToSkipImgs)
//	const char* leftimg_dir, const char* rightimg_dir, const char* leftimg_filename, const char* rightimg_filename)
{
	int nUsedImgs = 0;
	int board_corners_x = 2 * board_sq_holes_x,
		board_corners_y = 2 * board_sq_holes_y;

	Size board_size = Size(board_corners_x, board_corners_y);
	int board_n = board_corners_x * board_corners_y;


	vector<Point3f> obj;
	cali::calIrboardObjPoints(obj, board_sq_holes_x, board_sq_holes_y, square_size, space_square_size);

	cv::Mat gray1, gray2;
	//std::vector< std::vector< cv::Point2f > > imagePoints1, imagePoints2;
	std::vector< cv::Point2f > corners1, corners2;

	left_img_points.clear();
	right_img_points.clear();
	object_points.clear();

	char left_img[128], right_img[128];
	bool isLR_insameFile = (0 == strcmp(m_cfg.leftimg_filename.c_str(), m_cfg.rightimg_filename.c_str())) ? true : false;
	for (int i = m_cfg.img_id_beg; i <= m_cfg.img_id_end; i += stepToSkipImgs ) {
		sprintf(left_img, "%s%s%05d.png", m_cfg.leftimg_dir.c_str(), m_cfg.leftimg_filename.c_str(), i);
		sprintf(right_img, "%s%s%05d.png", m_cfg.rightimg_dir.c_str(), m_cfg.rightimg_filename.c_str(), i);
		if (isLR_insameFile) {
			if (!fileExists(string(left_img))) {
				continue;
			}
			RawFrmIrStereoImg obj(m_cfg.imgW, m_cfg.imgH);
			obj.loadImg(left_img);
			img1 = obj.imgL;
			img2 = obj.imgR;
			printf("%s exist", left_img);
		}
		else {
			if ( !fileExists(string(left_img)) || !fileExists(string(right_img))) {
				continue;
			}
			img1 = imread(left_img, CV_LOAD_IMAGE_GRAYSCALE);
			img2 = imread(right_img, CV_LOAD_IMAGE_GRAYSCALE);
		}

		bool found1 = findMaskCorners(img1, board_size, corners1, m_cfgMser.detector, m_cfgMser.mserDelta, m_cfgMser.max_var, m_cfgMser.min_diversity, m_cfgMser.area_threshold, m_cfgMser.maxCornerDistanceInPixel);
		bool found2 = findMaskCorners(img2, board_size, corners2, m_cfgMser.detector, m_cfgMser.mserDelta, m_cfgMser.max_var, m_cfgMser.min_diversity, m_cfgMser.area_threshold, m_cfgMser.maxCornerDistanceInPixel);

		if (found1) {
			assert(corners1.size() == board_n);
			img1.copyTo(gray1);
			cv::cornerSubPix(gray1, corners1, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray1, board_size, corners1, found1);
			sprintf(left_img, "c:/temp/calibIR/%s-L-%d.jpg", m_cfg.leftimg_filename.c_str(), i);
			cv::imwrite(left_img, gray1);
		}
		if (found2) {
			assert(corners2.size() == board_n);
			img2.copyTo(gray2);
			cv::cornerSubPix(gray2, corners2, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray2, board_size, corners2, found2);
			sprintf(right_img, "c:/temp/calibIR/%s-R-%d.jpg", m_cfg.rightimg_filename.c_str(), i);
			cv::imwrite(right_img, gray2);
		}


		if (found1 && found2) {
			printf( ", Found corners in both images\n" );
			left_img_points.push_back(corners1);
			right_img_points.push_back(corners2);
			object_points.push_back(obj);
			nUsedImgs++;
		}
		else{
			printf(", did not find corners in both images\n");
		}
	}

	return nUsedImgs;
}


int CalibStereo::load_predetected_points_irboard(int board_sq_holes_x, int board_sq_holes_y, float square_size, float space_square_size, int stepToSkipImgs)
{
	int nUsedImgs = 0;
	int board_corners_x = 2 * board_sq_holes_x,  board_corners_y = 2 * board_sq_holes_y;
	int board_n = board_corners_x * board_corners_y;

	vector<Point3f> obj;
	cali::calIrboardObjPoints(obj, board_sq_holes_x, board_sq_holes_y, square_size, space_square_size);

	left_img_points.clear();
	right_img_points.clear();
	object_points.clear();

	std::vector< cv::Point2f > corners;
	char left_img[256], right_img[256];

	string &pGood4BothLR = m_cfg.leftimg_dir;   //this folder contains <frmGoodBoth> -- good corners for both left and right images

	std::vector<std::string> vFileNames;
	uint32_t n = getFileNameList(pGood4BothLR, ".png", vFileNames);
	for( int i = 0; i<n; i+= stepToSkipImgs){
		std::string &fname = vFileNames[i];
		std::string frmId_s = fname.substr(4, 5);
		int fimId_i = atoi(frmId_s.c_str());
		sprintf(left_img, "%s/frm-%05d.cornersL.txt", m_cfg.cornersL_directory.c_str(), fimId_i);
		sprintf(right_img, "%s/frm-%05d.cornersR.txt", m_cfg.cornersR_directory.c_str(), fimId_i);
		if ( !fileExists(string(left_img)) || !fileExists(string(right_img)) ) {
			continue;
		}
		loadCorners( string(left_img), corners );
		assert(corners.size() == board_n);
		left_img_points.push_back(corners);

		loadCorners(string(right_img), corners);
		assert(corners.size() == board_n);
		right_img_points.push_back(corners);
		object_points.push_back(obj);
		nUsedImgs++;
	}
	return nUsedImgs;
}
