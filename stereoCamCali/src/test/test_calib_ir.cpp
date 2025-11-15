#include <stdio.h>

#include "libutil/util.h"
#include "libutil/cvUtilFuncs.h"
#include "libcalibIR/thermalvis_cali/calibration.hpp"
#include "libCapImg/RawFrmIrStereoImg.h"

#include "libcalib/calib_intrinsic.h"
#include "libcalib/calib_stereo.h"
#include "libcalib/undistort_rectify.h"

//todo: read paper confirm its meaning (comfirmed!)

//#define SQ_SZ_INCH		3 
#define SQ_SZ_INCH		1  

#if SQ_SZ_INCH==1
//6 x 9 squares
#	define MY_GRID_SIZE_MM	25.4 // (mm)
#	define MY_X_CORNERS			18   //corners in X direction: 9 x 2
#	define MY_Y_CORNERS			12   //corners in Y direction: 6 x 2
#else
//2 x 3 squares
#	define MY_GRID_SIZE_MM		76.2 // (mm)
#	define MY_X_CORNERS				6   //corners in X direction: 2 x 3
#	define MY_Y_CORNERS				4   //corners in Y direction: 2 x 2
#endif

//BOSON
#if 1
#	define IMG_W  640
#	define IMG_H  512
#else 
//Lapton
#	define IMG_W  160
#	define IMG_H  120
#endif

using namespace cv;
using namespace std;
using namespace cali;

int test_calib_ir_find_corners(const std::string &pRoot, const MserCfg &cfgMser);
int test_calib_ir_intrisic(const std::string &pRoot, const MserCfg &cfgMser, const CalibIntrinsicCfg& cfgCali);
int test_calib_ir_stereo(const std::string &pRoot, const MserCfg &cfgMser, const int nUsedImgsL, const int  nUsedImgsR);
int test_rectify_ir(const std::string& pRoot, const int nUsedImgs);
int test_undistort(const std::string& pRoot, const string& intrinsicYmlFile, const char lrFlag);

int copyFiles();

/*
		processdure to calibration:
	A. capture images (run test_capture.cpp):
		 A1. Set camera as closer as you can to individually capture imgs from letft and right cameras, and save the results into:
				 $pRootL/frmRaw  $pRootR/frmRaw
			 A2: pull camera alittlbe bit far way, make sure the board is covered by the FoV of both cameras, save the results to
				 $pRoot1/frmRaw

			 A3:capture a test img sequence (optional) and put the images in folder:
			 $pRoot2/frmRaw

	B. run the program
		B1. for pRootX (X='L','R','1','2'), the program will automatically create the relative subfolders, and we keep the following folder structure
			 //---------- raw frm image folder ------------
			 // $pRootX/frmRaw
			 //---------- good frms (found all corners for L, R, or both L & R images ------------
			 // $pRootX/frmGoodLeft
			 // $pRootX/frmGoodRight
			 // $pRootX/frmGoodBoth
			 //---------- calibration results files ------------
			 // $pRootX/cam_intrinsic_left.yml
			 // $pRootX/cam_intrinsic_right.yml
			 // $pRootX/cam_stereo.yml
			B2: execute the  following functions
		1. run test_libcap() to capture frm images, and manually select the "good" frames, and store the frm iamges at $pRoot/frm folder
		2. run test_calib_ir_find_corners() to figure out best parameters (cfgMser) to find corners
		3. run test_calib_ir_intrisic() using the best corner parameters found in step 2
		4. run test_calib_ir_stereo() using the best corner parameters found in step 2
*/
int test_calib_ir(int argc, char* argv[])
{

	int x = 0;

	//imgs for stereo, camera set far away to the board to make sure it can be seen by both cameras
#if SQ_SZ_INCH==1
//	string pRoot = "C:/Users/wus1/Projects/2021/pnt/dataset/calibration/boson8.7mm/9x6A";
	string pRoot = "C:/Users/wus1/Projects/2021/pnt/dataset/calibration/s0108801/6x9";
#else
	//string pRoot = "C:/Users/wus1/Projects/2021/pnt/dataset/calibration/boson8.7mm/2x3A";
	string pRoot = "C:/Users/wus1/Projects/2021/pnt/dataset/calibration/s0108801/2x3B";
#endif

	const bool usePredetectedCorners = true;
	const int nStep2SkipFiles = 2;
	const bool cali_stereo = false;
	const bool do_rectify = false;

	//======== set MSER parameters from test_calib_ir_find_corners() ==========
	MserCfg cfgMser;
	cfgMser.detector = 0;
#if SQ_SZ_INCH==1
	cfgMser.mserDelta = 8;             //3-lepton 4 x 6 holes, 8-BOSON, 11-lepton 2 x 3 holes
#else
	cfgMser.mserDelta = 11;             //3-lepton 4 x 6 holes, 8-BOSON, 11-lepton 2 x 3 holes
#endif

	cfgMser.max_var = 0.25;             //0.25,
	cfgMser.min_diversity = 0.2;
	cfgMser.area_threshold = 1.01;
	cfgMser.maxCornerDistanceInPixel = MAX_CORNER_SEPARATION;

	//1. find corners 
	if (!usePredetectedCorners) {
		x = test_calib_ir_find_corners(pRoot, cfgMser);  //fingure out <cfgMser>
	}

	//
	// ========== set cfgCaliIntrisic =======================
	//
	string intrinsicYmlFile;
	CalibIntrinsicCfg cfgCaliIntrisic;
	cfgCaliIntrisic.imgW = IMG_W;
	cfgCaliIntrisic.imgH = IMG_H;
	cfgCaliIntrisic.board_corners_x = MY_X_CORNERS;
	cfgCaliIntrisic.board_corners_y = MY_Y_CORNERS;
	cfgCaliIntrisic.square_size = MY_GRID_SIZE_MM / 1000;	//mm to meters
	cfgCaliIntrisic.space_square_size = MY_GRID_SIZE_MM / 1000;			//mm to meters
	cfgCaliIntrisic.extension = "png";
	cfgCaliIntrisic.imgs_filename = "frm-";
	cfgCaliIntrisic.usePredetectedCorners = usePredetectedCorners;
	cfgCaliIntrisic.corners_directory = pRoot + "/frmCorners";
	cfgCaliIntrisic.nStep2SkipFiles = nStep2SkipFiles;

	//
	//========== 2. cal inrinsic parameters =======================
	//
	std::vector<char> vLR = { 'L','R' };
	int nUsedImgsL, nUsedImgsR;
	for (const char LorR : vLR) {
		if (LorR == 'R') {
			cfgCaliIntrisic.imgs_directory = pRoot + "/frmGoodRight/";   //this folder contain frm imgs in which the corners of right img are detect correctly
			cfgCaliIntrisic.out_file = pRoot + "/cam_intrinsic_right"; //will add "usedImgs123.yml" in final file name
			cfgCaliIntrisic.preDetectedCornerFileTag = "cornersR.txt";
		}
		else {
			cfgCaliIntrisic.imgs_directory = pRoot + "/frmGoodLeft/"; //this folder contain frm imgs in which the corners of left img are detect correctly
			cfgCaliIntrisic.out_file = pRoot + "/cam_intrinsic_left"; //will add "usedImgs123.yml" in final file name
			cfgCaliIntrisic.preDetectedCornerFileTag = "cornersL.txt";
		}


		int nUsedImgs = test_calib_ir_intrisic(pRoot, cfgMser, cfgCaliIntrisic);
		if (LorR == 'L') {
			nUsedImgsL = nUsedImgs;
			intrinsicYmlFile = pRoot + "/cam_intrinsic_left" + num_to_string(nUsedImgsL, "_usedImgs") + ".yml";
		}
		else{
			nUsedImgsR = nUsedImgs;
			intrinsicYmlFile = pRoot + "/cam_intrinsic_right" + num_to_string(nUsedImgsR, "_usedImgs") + ".yml";
		}
		test_undistort(pRoot, intrinsicYmlFile, LorR);
	}

	//
	//============== 3. cal stereo paramters ==============
	//
	int nUsedImgs4Stereo;
	if (cali_stereo) {
		nUsedImgs4Stereo = test_calib_ir_stereo(pRoot, cfgMser, nUsedImgsL, nUsedImgsR);

	  //4. test and verification
		if (do_rectify) {
			x = test_rectify_ir(pRoot, nUsedImgs4Stereo);
			x = copyFiles();
		}
	}
	return 0;
}

int test_calib_ir_find_corners(const std::string &pRoot, const MserCfg &cfgMser)
{
	string pFrm  = pRoot + "/frmRaw";
	string pDumpB = pRoot + "/frmGoodBoth";
	string pDumpL = pRoot + "/frmGoodLeft";
	string pDumpR = pRoot + "/frmGoodRight";
	string pDumpCorner = pRoot + "/frmCorners";

	Size	imgSz(IMG_W, IMG_H);
	uint32_t len = 5;
	if (IMG_W == 160) {
		len = 1;
	}

	const cv::Scalar color(255, 255, 255);
	int thickness = 1;
	int lineType = 1;
	int shift = 0;

	std::vector<std::string> vFileNames;
	getFileNameList( pFrm, "png", vFileNames);
	for (size_t i = 0; i<vFileNames.size(); ++i) {
		printf("i=%d, %s\n", i, vFileNames[i].c_str());
	}

	createDir(pDumpB);
	createDir(pDumpL);
	createDir(pDumpR);
	createDir(pDumpCorner);

	cv::Size patternSize(MY_X_CORNERS, MY_Y_CORNERS);
	vector<cv::Point2f> cornersL;
	vector<cv::Point2f> cornersR;


	string fPath, fDes;
	RawFrmIrStereoImg  frm( imgSz.width, imgSz.height);
	int cnt = 0;
	BOOST_FOREACH ( const std::string fname, vFileNames) {
		fPath = pFrm + "/" + fname;
		dumpLog("%s", fPath.c_str());

		frm.loadImg(fPath, IMREAD_GRAYSCALE, cnt);

		cornersL.clear();
		cornersR.clear();
		bool sucL = findMaskCorners(frm.imgL, patternSize, cornersL, cfgMser.detector, cfgMser.mserDelta, cfgMser.max_var, cfgMser.min_diversity, cfgMser.area_threshold, cfgMser.maxCornerDistanceInPixel);
		bool sucR = findMaskCorners(frm.imgR, patternSize, cornersR, cfgMser.detector, cfgMser.mserDelta, cfgMser.max_var, cfgMser.min_diversity, cfgMser.area_threshold, cfgMser.maxCornerDistanceInPixel);

#if 1
		drawPluses(frm.imgL, cornersL, len, color, thickness, lineType, shift);
		drawPluses(frm.imgR, cornersR, len, color, thickness, lineType, shift);
		if (cornersR.size() > 0 || cornersL.size() > 0) {
			int L = fname.length() - 4;
			fDes = pDumpCorner + "/" + fname.substr(0, L) + ".cornersL.txt";
			dumpCorners( fDes, cornersL);
			
			fDes = pDumpCorner + "/" + fname.substr(0, L) + ".cornersR.txt";
			dumpCorners(fDes, cornersR);

		}
#endif
		if (sucL) {
			frm.dumpImg(pDumpL, fname, 0, true);
		}

		if (sucR) {
			frm.dumpImg(pDumpR, fname, 0, true);
		}

		if (sucL && sucR) {
			frm.dumpImg(pDumpB, fname, 0, true);
			//vGoodFileNames.push_back(fname);
		}
		cnt++;
	}
#if 0
	fPath = num_to_string(cfgMser.mserDelta, 2,  pRoot+ "/mserDelta") + "_suc_corner_imgs.txt";
	FILE *fp = fopen(fPath.c_str(), "w");
	for (size_t i = 0; i<vGoodFileNames.size(); ++i) {
		fprintf(fp, "%s\n", vGoodFileNames[i].c_str());
	}
	fclose(fp);
#endif
	return 0;
}

int test_calib_ir_intrisic(const std::string &pRoot, const MserCfg &cfgMser, const CalibIntrinsicCfg &cfgCali)
{
	printf("%d\n", DEBUG_MODE);
	if (DEBUG_MODE > 2) {
		namedWindow("mainWin", 1);
	}

	CalibIntrinsicPtr    cali(new CalibIntrinsic());
	int nUsedImgs = cali->processIr(cfgCali, cfgMser);
	return nUsedImgs;
}

int test_calib_ir_stereo(const std::string &pRoot, const MserCfg &cfgMser, const int nUsedImgsL, const int  nUsedImgsR)
{
	CalibStereoCfg p;
	p.imgW = IMG_W;
	p.imgH = IMG_H;
	p.img_id_beg = 327;
	p.img_id_end = 1210;
	
	p.leftcalib_file	= pRoot + "/cam_intrinsic_left"  + num_to_string(nUsedImgsL, "_usedImgs") + ".yml";
	p.rightcalib_file	= pRoot + "/cam_intrinsic_right" + num_to_string(nUsedImgsR, "_usedImgs") + ".yml";
	p.leftimg_dir		= pRoot + "/frmGoodBoth/";
	p.rightimg_dir		= pRoot + "/frmGoodBoth/";
	p.leftimg_filename = "frm-";
	p.rightimg_filename = "frm-";
	p.out_file = pRoot + "/cam_stereo";

	p.usePredetectedCorners=true;
	p.cornersL_directory= pRoot + "/frmCorners";
	p.cornersR_directory= pRoot + "/frmCorners";

	printf("%d\n", DEBUG_MODE);
	if (DEBUG_MODE > 2) {
		namedWindow("mainWin", 1);
	}
	CalibStereoPtr    obj(new CalibStereo());
	int stepToSkipImgs = 1; //1-load all, 2-only use 1/2 image, 3 - using 1/3 images 
	int nUsedImgs = obj->process(p, cfgMser, cali::CALIB_BOARD_IR, stepToSkipImgs );

	return nUsedImgs;
}

int test_rectify_ir(const std::string &pRoot, const int nUsedImgs)
{
	CalibRectifyPtr obj(new CalibRectify());
	int type = CV_LOAD_IMAGE_GRAYSCALE;
	cv::Size imgSz(IMG_W, IMG_H);
	string calib_result_file = pRoot + "/cam_stereo" + num_to_string(nUsedImgs, "_usedImgs") + ".yml";
	obj->loadCalibParams(calib_result_file);
	obj->init(imgSz);

	string pImg = pRoot + "/frmRaw";
	string pRect = pRoot + "/rectified";
	createDir(pRect);

	std::vector<std::string> vFileNames;
	uint32_t cnt = 0;
	uint32_t n = getFileNameList(pImg, "png", vFileNames);
	BOOST_FOREACH(std::string &fname, vFileNames) {
#if 0			
		char tag[16];
		snprintf(tag, 16, "%05d.png", i);
		std::string leftimg_filename = p0 + "/1/left" + string(tag);
		std::string rightimg_filename = p0 + "/1/right" + string(tag);
		std::string leftout_filename = "C:/temp/calibIR/rect_L" + string(tag);
		std::string rightout_filename = "C:/temp/calibIR/rect_R" + string(tag);
#endif

		std::string leftimg_filename = pImg + "/" + fname;
		std::string rightimg_filename = pImg + "/" + fname;
		std::string lrout_filename = pRect + "/" + fname;

		cout << leftimg_filename << "," << rightimg_filename << endl;

		//obj->computeRectifyImg(leftimg_filename, rightimg_filename, type,  leftout_filename, rightout_filename);
		obj->computeRectifyImg(leftimg_filename, rightimg_filename, type, lrout_filename);
	}
	return 0;
}


int test_undistort(const std::string& pRoot, const string& intrinsicYmlFile, const char lrFlag )
{

	string pImg = pRoot + "/frmRaw";
	string pOut = pRoot + "/undistorted" + "-" + lrFlag;
	createDir(pOut);

	CalibUndist undistortion( cv::Size(IMG_W, IMG_H), CV_8UC1 );
	undistortion.loadCamarams(intrinsicYmlFile);

	std::vector<std::string> vFileNames;
	uint32_t n = getFileNameList(pImg, "png", vFileNames);
	BOOST_FOREACH(std::string & fname, vFileNames) {
		std::string lr_img_filename = pImg + "/" + fname;
		std::string lr_out_filename = pOut + "/" + fname;

		undistortion.undistImg(lr_img_filename, lr_out_filename, lrFlag);
	}

	return 0;
}


int copyFiles()
{
	string srcFolder = "C:/Projects/2018/UavColliAvoid/dataset/boson4.4mm/vMove";
	string desFolder = "C:/Projects/2018/UavColliAvoid/dataset/boson4.4mm/vMove2";

	std::vector<std::string> vFileNames;
	uint32_t n = getFileNameList( srcFolder, "png", vFileNames);
	char des[256];
	int i=0, cnt = 0;
	BOOST_FOREACH(std::string &fname, vFileNames) {
		if ( i % 5 == 0 ){
			string src = srcFolder + "/" + fname;
			snprintf(des, 256, "%s/frm-%05d.png", desFolder.c_str(), cnt++);

			printf("src=%s\ndes=%s\n", src.c_str(), des);
			boost::filesystem::copy_file(src, string(des), boost::filesystem::copy_option::overwrite_if_exists);
		}
		++i;
	}
	return 0;
}
