#include "calib_intrinsic.h"

using namespace std;
using namespace cv;
using namespace cali;

void cali::calChessboardObjPoints(vector<Point3f> &obj, const int board_corners_x, const int board_corners_y, const float square_size)
{
	obj.clear();
	for (int i = 0; i < board_corners_y; i++) {
		for (int j = 0; j < board_corners_x; j++) {
			obj.push_back(Point3f((float)j * square_size, (float)i * square_size, 0));
		}
	}
}

void cali::calIrboardObjPoints(vector<Point3f> &obj, const int board_sq_holes_x, const int board_sq_holes_y, const float square_size, const float space_square_size)
{
	obj.clear();
	float x = 0, y = 0;
	for (int i = 0; i < board_sq_holes_y; i++) {
		//top corners of the ith row holes
		x = 0;
		for (int j = 0; j < board_sq_holes_x; j++) {
			obj.push_back(Point3f(x, y, 0));

			x += square_size;
			obj.push_back(Point3f(x, y, 0));
			x += space_square_size;
		}

		//bottom corners of the ith row holes
		y += square_size;
		x = 0;
		for (int j = 0; j < board_sq_holes_x; j++) {
			obj.push_back(Point3f(x, y, 0));

			x += square_size;
			obj.push_back(Point3f(x, y, 0));

			x += space_square_size;
		}

		//----loop for holes in the next row -----
		y += space_square_size;
	}
}


CalibIntrinsic::CalibIntrinsic()
{
}

void CalibIntrinsic::setup_calibration_chessboard(int board_corners_x, int board_corners_y, int img_id_beg, int img_id_end,
                       float square_size, const char* imgs_directory, const char* imgs_filename,
                       const char* extension) {
  Size board_size = Size(board_corners_x, board_corners_y);
  int board_n = board_corners_x * board_corners_y;

  vector<Point3f> obj;
  calChessboardObjPoints(obj, board_corners_x, board_corners_y, square_size);

  image_points.clear();
  object_points.clear();
  for (int k = img_id_beg; k <= img_id_end; k++) {
    char img_file[100];
    sprintf(img_file, "%s%s%d.%s", imgs_directory, imgs_filename, k, extension);
    img = imread(img_file, CV_LOAD_IMAGE_COLOR);

	std::vector< cv::Point2f > corners;
	bool found = cv::findChessboardCorners(img, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
    if (found){
		cv::Mat gray;
		assert(corners.size() == board_n);
		cout << k << ". Found corners!" << endl;
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cornerSubPix(gray, corners, cv::Size(5, 5), cv::Size(-1, -1), TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
#if 0
		drawChessboardCorners(gray, board_size, corners, found);
		sprintf(img_file, "c:/temp/%s%d.%s", imgs_filename, k, extension);
		cv::imwrite(img_file, gray);
#endif

		image_points.push_back(corners);
		object_points.push_back(obj);
    }
  }
}

#if USE_IR_BOARD

//img_filename can only be "left", "right" and "frm-", "frm-" means the left and right images are save in one file, and based on <lrFlag> we decides which one will be used
int CalibIntrinsic::setup_calibration_irboard(int board_sq_holes_x, int board_sq_holes_y)
{
	int nUsedImgs = 0;
#if 0
	const float square_size = m_cfg.square_size;
	const float space_square_size = m_cfg.space_square_size;
	const char* imgs_directory = m_cfg.imgs_directory.c_str();
	const char* imgs_filename = m_cfg.imgs_filename.c_str();
	const char* extension = m_cfg.extension.c_str();

	int board_corners_x = 2 * board_sq_holes_x,
		board_corners_y = 2 * board_sq_holes_y;
	Size board_size = Size(board_corners_x, board_corners_y);
	int board_n = board_corners_x * board_corners_y;

	vector<Point3f> obj;
	calIrboardObjPoints(obj, board_sq_holes_x, board_sq_holes_y, square_size, space_square_size);

	char img_file[128];
	bool isLR_inOneFile = (0 == strcmp(imgs_filename, "frm-")) ? true : false;
	image_points.clear();
	object_points.clear();
	for (int k = m_cfg.img_id_beg; k <= m_cfg.img_id_end; k++) {
		sprintf(img_file, "%s%s%05d.%s", imgs_directory, imgs_filename, k, extension);
		if (!fileExists(string(img_file))) {
			continue;
		}
		printf("%s exist", img_file);
		if (isLR_inOneFile) {
			RawFrmIrStereoImg obj(m_cfg.imgW, m_cfg.imgH);
			obj.loadImg(img_file);
			img = (lrFlag == 'L') ? obj.imgL : obj.imgR;
		}
		else {
			img = imread(img_file, CV_LOAD_IMAGE_GRAYSCALE);
		}


		std::vector< cv::Point2f > corners;
		bool found = findMaskCorners( img, board_size, corners, m_cfgMers.detector, m_cfgMers.mserDelta, m_cfgMers.max_var, m_cfgMers.min_diversity, m_cfgMers.area_threshold, m_cfgMers.maxCornerDistanceInPixel);
		if (found) {
			assert(corners.size() == board_n);
			printf( ", found all corners!\n" );

#if 0
			drawChessboardCorners(img, board_size, corners, found);
			if (lrFlag == 'L') {
				sprintf(img_file, "c:/temp/%s%d-L.png", imgs_filename, k);
			}
			else {
				sprintf(img_file, "c:/temp/%s%d-R.png", imgs_filename, k);
			}
			cv::imwrite(img_file, img);
#endif

			image_points.push_back(corners);
			object_points.push_back(obj);
			nUsedImgs++;
		}
		else{
			printf(", but did not find all corners!\n");
		}
	}
#endif
	return nUsedImgs;
}
#endif


//img_filename can only be "left", "right" and "frm-", "frm-" means the left and right images are save in one file, and based on <lrFlag> we decides which one will be used
int CalibIntrinsic::read_predetected_corners(int board_sq_holes_x, int board_sq_holes_y)
{
	int nUsedImgs = 0;
	int board_corners_x = 2 * board_sq_holes_x,
		board_corners_y = 2 * board_sq_holes_y;
	Size board_size = Size(board_corners_x, board_corners_y);
	int board_n = board_corners_x * board_corners_y;

	vector<Point3f> obj;
	calIrboardObjPoints(obj, board_sq_holes_x, board_sq_holes_y, m_cfg.square_size, m_cfg.space_square_size);


	std::string cornerFolder = m_cfg.corners_directory;   

	image_points.clear();
	object_points.clear();

	char cornerTxtFile[1024];
	std::vector<cv::Point2f> vCorners;
	std::vector<std::string> vFileNames;

	//m_cfg.imgs_directory = "C:\Projects\2018\UavColliAvoid\dataset_flying_stereo\20181127bosonIntrisicR\frmGoodRight"
	//or
	//m_cfg.imgs_directory = "C:\Projects\2018\UavColliAvoid\dataset_flying_stereo\20181127bosonIntrisicR\frmGoodLeft"
	uint32_t n = getFileNameList(m_cfg.imgs_directory, "png", vFileNames);
	for (int i = 0; i < vFileNames.size(); i+= m_cfg.nStep2SkipFiles ) {
		std::string &fname = vFileNames[i];
		std::string frmName = fname.substr(0, 10);
		std::string cornerTxtFile =  m_cfg.corners_directory + "/" + frmName + m_cfg.preDetectedCornerFileTag;
		if (!fileExists(cornerTxtFile) ) {
			continue;
		}

		loadCorners(cornerTxtFile, vCorners);
		assert(vCorners.size() == board_n);
		image_points.push_back(vCorners);
		object_points.push_back(obj);
		nUsedImgs++;
	}
	return nUsedImgs;
}

double CalibIntrinsic::computeReprojectionErrors(const vector< vector< Point3f > >& objectPoints,
                                 const vector< vector< Point2f > >& imagePoints,
                                 const vector< Mat >& rvecs, const vector< Mat >& tvecs,
                                 const Mat& cameraMatrix , const Mat& distCoeffs) {
  vector< Point2f > imagePoints2;
  int i, totalPoints = 0;
  double totalErr = 0, err;
  vector< float > perViewErrors;
  perViewErrors.resize(objectPoints.size());

  for (i = 0; i < (int)objectPoints.size(); ++i) {
    projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
                  distCoeffs, imagePoints2);
    err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);
    int n = (int)objectPoints[i].size();
    perViewErrors[i] = (float) std::sqrt(err*err/n);
    totalErr += err*err;
    totalPoints += n;
  }
  return std::sqrt(totalErr/totalPoints);
}

int CalibIntrinsic::processEo(const CalibIntrinsicCfg &cfg)
{
	setup_calibration_chessboard(cfg.board_corners_x, cfg.board_corners_y, cfg.img_id_beg, cfg.img_id_end, cfg.square_size, cfg.imgs_directory.c_str(),
			cfg.imgs_filename.c_str(), cfg.extension.c_str());

	printf("Starting Calibration\n");
	Mat K;
	Mat D;
	vector< Mat > rvecs, tvecs;
	int flag = 0;
	flag |= CV_CALIB_FIX_K4;
	flag |= CV_CALIB_FIX_K5;

	int nUsedImgs=0;
	calibrateCamera(object_points, image_points, img.size(), K, D, rvecs, tvecs, flag);
	double err = computeReprojectionErrors(object_points, image_points, rvecs, tvecs, K, D);

	wrtYml(cfg, K, D, rvecs, tvecs, nUsedImgs, err);
	dumpLog("Done Calibration, err=%f", err);

	return 0;
} 

#if USE_IR_BOARD
int CalibIntrinsic::processIr(const CalibIntrinsicCfg &cfg, const MserCfg &cfgMers)
{
	int nUsedImgs;
	m_cfg = cfg;
	m_cfgMers = cfgMers;

	int board_sq_holes_x = cfg.board_corners_x / 2,
		board_sq_holes_y = cfg.board_corners_y / 2;

	if (m_cfg.usePredetectedCorners) {
		nUsedImgs=read_predetected_corners( board_sq_holes_x, board_sq_holes_y );
	}
	else {
		cout << "run test_calib_ir_find_corners() first" << endl;
		assert(0);
		//nUsedImgs=setup_calibration_irboard( board_sq_holes_x, board_sq_holes_y );
	}
	dumpLog("Starting Calibration, nUsedImgs=%d", nUsedImgs);
	Mat K;
	Mat D;
	vector< Mat > rvecs, tvecs;
	int flag = 0;
	flag |= CV_CALIB_FIX_K4;
	flag |= CV_CALIB_FIX_K5;

	int miniPtsThd = 9;
	if ( object_points.size() >= miniPtsThd ) {
		cv::calibrateCamera(object_points, image_points, cv::Size( cfg.imgW, cfg.imgH), K, D, rvecs, tvecs, flag);
		double err = computeReprojectionErrors(object_points, image_points, rvecs, tvecs, K, D);

		wrtYml(cfg, K, D, rvecs, tvecs, nUsedImgs, err);
		dumpLog("Done Calibration, err=%f", err);
	}
	else {
		dumpLog("Do not have enough points!\n");
	}
	return nUsedImgs;
}
#endif


int CalibIntrinsic::wrtYml(const CalibIntrinsicCfg& cfg, const Mat& K, const Mat& D, const vector<Mat>& rvecs, const vector<Mat>& tvecs, const int nUsedImgs, const double err)
{
	FileStorage fs(cfg.out_file + num_to_string(nUsedImgs, "_usedImgs") + ".yml", FileStorage::WRITE);
	fs.writeComment("[u, v, z]^T = P[X, Y, Z 1]^T, P = K x[R | t]");
	fs.writeComment("K- intrinsic matrix: K=[fx, gamma(skew), cx; 0 fy cy; 0 0 1], where (cx,cy) is the x-, y- coordinate of camera center");
	fs.writeComment("D- distortion parameter");
	fs.writeComment("rvecs- rotation:    std::vector<cv::Mat> rvecs, each element is a 3x1 vector, and rvecs.szie()==nUsedImgs");
	fs.writeComment("tvecs- translation: std::vector<cv::Mat> tvecs, each element is a 3x1 vector, and tvecs.szie()==nUsedImgs");
	fs << "imgW" << cfg.imgW;
	fs << "imgH" << cfg.imgH;
	fs << "nUsedImgs" << nUsedImgs;
	fs << "K" << K;
	fs << "D" << D;
	fs << "rvecs" << rvecs;
	fs << "tvecs" << tvecs;
	fs << "board_corners_x" << cfg.board_corners_x;
	fs << "board_corners_y" << cfg.board_corners_y;
	fs << "square_size" << cfg.square_size;
	fs << "space_square_size" << cfg.space_square_size;
	fs << "CalibrationError" << err;
	fs.writeComment("--eof--");
	return 0;
}