#include "undistort_rectify.h"

using namespace std;
using namespace cv;
using namespace cali;

CalibRectify::CalibRectify()
{
}

void CalibRectify::loadCalibParams(const std::string &calib_file)
{
	calibParams.readFromYmlFile(calib_file);
}

void CalibRectify::init(const cv::Size imgSz)
{
	img_sz = imgSz;

	cv::initUndistortRectifyMap(calibParams.K1, calibParams.D1, calibParams.R1, calibParams.P1, img_sz, CV_32F, lmapx, lmapy);
	cv::initUndistortRectifyMap(calibParams.K2, calibParams.D2, calibParams.R2, calibParams.P2, img_sz, CV_32F, rmapx, rmapy);
}

void CalibRectify::computeRectifyImg(const std::string &leftimg_filename, const std::string &rightimg_filename, const int type,
	const std::string &leftout_filename, const std::string &rightout_filename)
{
	Mat imgL = imread(leftimg_filename, type);
	Mat imgR = imread(rightimg_filename, type);
	cv::Mat imgUL, imgUR;

	computeRectifyImg(imgL, imgR, imgUL, imgUR);

	imwrite(leftout_filename,  imgUL);
	imwrite(rightout_filename, imgUR);
}

void CalibRectify::computeRectifyImg(const std::string &leftimg_filename, const std::string &rightimg_filename, const int type, const std::string &oneout_filename)
{
	cv::Mat imgL, imgR;
	cv::Mat imgUL, imgUR;
	bool isLR_insameFile = ( 0 == leftimg_filename.compare(rightimg_filename) );
	if (isLR_insameFile) {
		RawFrmIrStereoImg obj(img_sz.width, img_sz.height);
		obj.loadImg(leftimg_filename);
		imgL = obj.imgL;
		imgR = obj.imgR;
	}
	else {
		imgL = imread(leftimg_filename, type);
		imgR = imread(rightimg_filename, type);
	}

	computeRectifyImg(imgL, imgR, imgUL, imgUR);

	int w = imgUL.cols;
	int h = imgUL.rows;
	int outType = (imgUL.channels() == 1) ? CV_8UC1 : CV_8UC3;
	cv::Mat I(2 * h, 2 * w, outType);

	cv::Rect r(0, 0, w, h);
	cv::Mat rectI = I( r );
	imgL.copyTo( rectI );

	r.x = w;
	rectI = I(r);
	imgR.copyTo(rectI);

	r.x = 0;
	r.y = h; 
	rectI = I(r);
	imgUL.copyTo(rectI);

	r.x = w;
	rectI = I(r);
	imgUR.copyTo(rectI);

	if (outType == CV_8UC1) {
		cv::Mat I_8uc3(2 * h, 2 * w, outType);
		gray2bgr(I, I_8uc3);
		I_8uc3.copyTo(I);
	}

	int n = 10;
	int h1 = h, dh = h / n;
	for (int i = 1; i < n; ++i) {
		h1 += dh;
		cv::Scalar lineColor(0, 255, 0);
		if (i % 2 == 1) {
			lineColor = cv::Scalar(0, 0, 255);
		}
		cv::line(I, cv::Point(0, h1), cv::Point(2 * w - 1, h1), lineColor, 1, 1);
	}

	imwrite(oneout_filename, I);
}


void CalibRectify::computeRectifyImg(const cv::Mat &imgL, const cv::Mat &imgR, cv::Mat &imgUL, cv::Mat &imgUR)
{
#if _DEBUG
	assert( img_sz.width == imgL.size().width  && img_sz.height == imgL.size().height );
	assert( img_sz.width == imgR.size().width  && img_sz.height == imgR.size().height );
#endif

	cv::remap(imgL, imgUL, lmapx, lmapy, cv::INTER_LINEAR);
	cv::remap(imgR, imgUR, rmapx, rmapy, cv::INTER_LINEAR);

	imshow("imgUL", imgUL);
	cv::waitKey(1);
}

#if 0
int main(int argc, char *argv[])
{
  string leftimg_filename;
  string rightimg_filename;
  string calib_file;
  string leftout_filename;
  string rightout_filename;

  try {
	  cxxopts::Options options(argv[0], "calibrate stereo cameras");
	  options.add_options()
			( "l,leftimg_filename","Left image path",cxxopts::value<string>(leftimg_filename),"STR" )
			( "r,rightimg_filename","Right image path",cxxopts::value<string>(rightimg_filename),"STR" )
			( "c,calib_file","Stereo calibration file",cxxopts::value<string>(calib_file),"STR" )
			( "L,leftout_filename","Left undistorted image path",cxxopts::value<string>(leftout_filename),"STR" )
			( "R,rightout_filename","Right undistorted image path",cxxopts::value<string>(rightout_filename),"STR" )
		    ( "h,help", "print help")
			;

	  options.parse(argc, argv);
	  if (options.count("help"))
	  {
	    cout << options.help() << endl;
        exit(0);
	  }
	  cxxopts::check_required(options, {"l","r","c","L","R"}); // throws exception if any not present
  }
  catch (const cxxopts::OptionException& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }

  Mat R1, R2, P1, P2, Q;
  Mat K1, K2, R;
  Vec3d T;
  Mat D1, D2;

  cv::FileStorage fs1(calib_file, cv::FileStorage::READ);
  fs1["K1"] >> K1;
  fs1["K2"] >> K2;
  fs1["D1"] >> D1;
  fs1["D2"] >> D2;
  fs1["R"] >> R;
  fs1["T"] >> T;

  fs1["R1"] >> R1;
  fs1["R2"] >> R2;
  fs1["P1"] >> P1;
  fs1["P2"] >> P2;
  fs1["Q"] >> Q;

  cv::Mat lmapx, lmapy, rmapx, rmapy;
  cv::Mat imgU1, imgU2;

  cv::initUndistortRectifyMap(K1, D1, R1, P1, img1.size(), CV_32F, lmapx, lmapy);
  cv::initUndistortRectifyMap(K2, D2, R2, P2, img2.size(), CV_32F, rmapx, rmapy);
  cv::remap(img1, imgU1, lmapx, lmapy, cv::INTER_LINEAR);
  cv::remap(img2, imgU2, rmapx, rmapy, cv::INTER_LINEAR);
  
  imwrite(leftout_filename, imgU1);
  imwrite(rightout_filename, imgU2);

  return 0;
}
#endif