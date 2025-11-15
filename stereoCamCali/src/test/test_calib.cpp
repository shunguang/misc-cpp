#include <stdio.h>

#include "libutil/util.h"
#include "libutil/cvUtilFuncs.h"
#include "libcalibIR/thermalvis_cali/calibration.hpp"
#include "libdc/RawFrmIrStereoImg.h"
#include "libcfg/CfgStereoMatch.h"

#include "libcalib/calib_intrinsic.h"
#include "libcalib/calib_stereo.h"
#include "libcalib/undistort_rectify.h"
#include "libperception/StereoMatch.h"

//todo: read paper confirm its meaning (comfirmed!)
#if 0
#	define MY_GRID_SIZE_MM		25.4 // (mm)
#	define MY_X_CORNERS			12   //corners in X direction: 2 x 6
#	define MY_Y_CORNERS			8    //corners in Y direction: 2 x 4
#else
#	define MY_GRID_SIZE_MM		76.2 // (mm)
#	define MY_X_CORNERS			6   //corners in X direction: 2 x 3
#	define MY_Y_CORNERS			4   //corners in Y direction: 2 x 2
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
using namespace uavca;

int test_calib_ir_find_corners(const std::string &pRoot, const MserCfg &cfgMser);
int test_calib_ir_intrisic(const std::string &pRoot, const MserCfg &cfgMser, const char LorR);
int test_calib_ir_stereo(const std::string &pRoot, const MserCfg &cfgMser);
int test_rectify_ir(const std::string &pRoot);

int test_calib_eo_intrisic(const std::string &pRoot, const MserCfg &cfgMser, const char LorR);
int test_calib_eo_stereo(const std::string &pRoot, const MserCfg &cfgMser);
int test_rectify_eo(const std::string &pRoot);

int test_stereo_match(const std::string &pRoot);
int test_stereo_match_old();
int test_stereo_match_dumpLeftRectImg();

int copyFiles();

/*
    processdure to calibration:
	A. capture images (run test_libcap.cpp):
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
int test_libcalib(int argc, char* argv[])
{

	int x = 0;
	string pRootL = "C:/Projects/2018/UavColliAvoid/dataset_flying_stereo/20181127bosonIntrisicL"; //imgs for left, camera set to most closer the board
	string pRootR = "C:/Projects/2018/UavColliAvoid/dataset_flying_stereo/20181127bosonIntrisicR"; //imgs for right, camera set to most closer the board
	string pRoot1 = "C:/Projects/2018/UavColliAvoid/dataset_flying_stereo/20181127bosonStereo";	   //imgs for stereo, camera set far away to the board to make sure it can be seen by both cameras
	string pRoot2 = "C:/Projects/2018/UavColliAvoid/dataset_flying_stereo/20181205-fieldtest";	   //test calibration perfromance

	//set MSER parameters from test_calib_ir_find_corners()
	MserCfg cfgMser;
	cfgMser.detector = 0;
	cfgMser.mserDelta = 11;             //3-lepton 4 x 6 holes, 8-BOSON, 11-lepton 2 x 3 holes
	cfgMser.max_var = 0.25;             //0.25,
	cfgMser.min_diversity = 0.2;
	cfgMser.area_threshold = 1.01;
	cfgMser.maxCornerDistanceInPixel = MAX_CORNER_SEPARATION;

	//1. find corners 
	//x = test_calib_ir_find_corners(pRootL, cfgMser);  //fingure out <cfgMser>
	//x = test_calib_ir_find_corners(pRootR, cfgMser);  //fingure out <cfgMser>
	//x = test_calib_ir_find_corners(pRoot1, cfgMser);  //fingure out <cfgMser>

	//2. cal inrinsic parameters
	//x = test_calib_ir_intrisic(pRootL, cfgMser, 'L');
	//x = test_calib_ir_intrisic(pRootR, cfgMser, 'R');

	//3. cal stereo paramters
	//x = test_calib_ir_stereo(pRoot1, cfgMser);

	//4. test and verification
	//x = test_rectify_ir(pRoot2);
	x = test_stereo_match(pRoot2);


	//x = test_calib_eo_intrisic(pRoot, cfgMser);
	//x = test_calib_eo_stereo(pRoot, cfgMser);
	//x = test_rectify_eo();

	//x = copyFiles();
	//x = test_stereo_match_dumpLeftRectImg();

	return x;
}

int test_calib_ir_find_corners(const std::string &pRoot, const MserCfg &cfgMser)
{
	//string pRoot = "C:/Projects/2018/UavColliAvoid/dataset/lepton/calib/2018-0706";
	string pFrm  = pRoot + "/frmRaw";
	string pDumpB = pRoot + "/frmGoodBoth";
	string pDumpL = pRoot + "/frmGoodLeft";
	string pDumpR = pRoot + "/frmGoodRight";
	string pDumpCorner = pRoot + "/frmCorners";

	Size	imgSz(IMG_W, IMG_H);
	uint32 len = 5;
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

	myCreateDir(pDumpB);
	myCreateDir(pDumpL);
	myCreateDir(pDumpR);
	myCreateDir(pDumpCorner);

	cv::Size patternSize(MY_X_CORNERS, MY_Y_CORNERS);
	vector<cv::Point2f> cornersL;
	vector<cv::Point2f> cornersR;


	string fPath, fDes;
	//std::vector<std::string> vGoodFileNames;
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

int test_calib_ir_intrisic(const std::string &pRoot, const MserCfg &cfgMser, const char LorR)
{
	//string pRoot = "C:/Projects/2018/UavColliAvoid/dataset/calibration/Boson0426";
	//string pRoot = "C:/Projects/2018/UavColliAvoid/dataset/lapton/calib/2018-0706";
	CalibIntrinsicCfg p;
	CalibIntrinsicPtr    cali(new CalibIntrinsic());

	//./calibrate -w 9 -h 6 -n 27 -s 0.02423 -d "../calib_imgs/1/" -i "left" -o "cam_left.yml" -e "jpg"
	p.imgW = IMG_W;
	p.imgH = IMG_H;
	p.board_corners_x = MY_X_CORNERS;
	p.board_corners_y = MY_Y_CORNERS;
	p.square_size = MY_GRID_SIZE_MM/1000;	//mm to meters
	p.space_square_size = MY_GRID_SIZE_MM / 1000;			//mm to meters
	p.extension = "png";
	p.imgs_filename = "frm-";

	p.usePredetectedCorners = true;
	p.corners_directory = pRoot + "/frmCorners";
	if (LorR == 'L') {
		p.nStep2SkipFiles = 1;
	}
	else {
		p.nStep2SkipFiles = 3;
	}
	printf("%d\n", DEBUG_MODE);
	if (DEBUG_MODE > 2) {
		namedWindow("mainWin", 1);
	}


	if (LorR == 'R' || LorR == 'r') {
		p.imgs_directory = pRoot + "/frmGoodRight/";   //this folder contain frm imgs in which the corners of right img are detect correctly
		p.out_file = pRoot + "/cam_intrinsic_right"; //will add "usedImgs123.yml" in final file name
		p.preDetectedCornerFileTag = "cornersR.txt";
	}
	else {
		p.imgs_directory = pRoot + "/frmGoodLeft/"; //this folder contain frm imgs in which the corners of left img are detect correctly
		p.out_file = pRoot + "/cam_intrinsic_left"; //will add "usedImgs123.yml" in final file name
		p.preDetectedCornerFileTag = "cornersL.txt";
	}

	cali->processIr(p, cfgMser);
	return 0;
}

int test_calib_ir_stereo(const std::string &pRoot, const MserCfg &cfgMser)
{
	CalibStereoCfg p;
	p.imgW = IMG_W;
	p.imgH = IMG_H;
	p.img_id_beg = 327;
	p.img_id_end = 1210;
	p.leftcalib_file	= pRoot + "/cam_intrinsic_left_from_2x3board_usedImgs381.yml";
	p.rightcalib_file	= pRoot + "/cam_intrinsic_right_from_2x3board_usedImgs315.yml";
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
	obj->process(p, cfgMser, uavca::CALIB_BOARD_IR, stepToSkipImgs );

	return 0;
}

int test_rectify_ir(const std::string &pRoot )
{
	CalibRectifyPtr obj(new CalibRectify());
	int type = CV_LOAD_IMAGE_GRAYSCALE;
	cv::Size imgSz(IMG_W, IMG_H);
	string calib_result_file = pRoot + "/cam_stereo_nUsedImgs258.yml";
	obj->loadCalibParams(calib_result_file);
	obj->init(imgSz);

	string pImg = pRoot + "/frmRaw";
	string pRect = pRoot + "/rectified";
	myCreateDir(pRect);

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

int test_calib_eo_intrisic(const std::string &pRoot, const MserCfg &cfgMser)
{
	CalibIntrinsicCfg p;
	CalibIntrinsicPtr    cali(new CalibIntrinsic() );

	//./calibrate -w 9 -h 6 -n 27 -s 0.02423 -d "../calib_imgs/1/" -i "left" -o "cam_left.yml" -e "jpg"
	p.board_corners_x = 9;
	p.board_corners_y = 6;
	p.square_size = 0.02423f;     //chess oneside board size in meters
	p.space_square_size = 0;
	p.imgs_directory = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/1/";
	p.extension = "jpg";

	p.imgs_filename = "left";
	p.out_file = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/cam_left1_C.yml";
	cali->processEo(p);

	p.imgs_filename = "right";
	p.out_file = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/cam_right1_C.yml";
	cali->processEo(p);

	return 0;
}

int test_calib_eo_stereo(const std::string &pRoot, const MserCfg &cfgMser)
{
	CalibStereoCfg p;

	p.img_id_beg = 1;
	p.img_id_end = 27;
	p.leftcalib_file = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/cam_left1.yml";
	p.rightcalib_file = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/cam_right1.yml";
	p.leftimg_dir = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/1/";
	p.rightimg_dir = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/1/";
	p.leftimg_filename = "left";
	p.rightimg_filename = "right";
	p.out_file = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/cam_stereo1_B.yml";

	CalibStereoPtr    obj(new CalibStereo());
	obj->process(p, cfgMser, CALIB_BOARD_CHESS);

	return 0;
}

int test_rectify_eo(const std::string &pRoot)
{
	CalibRectifyPtr obj(new CalibRectify());
	int type = CV_LOAD_IMAGE_COLOR;
	cv::Size imgSz(640, 360);
	string calib_param_file = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/cam_stereo1.yml";
	obj->loadCalibParams(calib_param_file);
	obj->init(imgSz);
	for (int i = 1; i < 10; ++i) {
		string tag = to_string(i) + ".jpg";
		std::string leftimg_filename = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/1/left" + tag;
		std::string rightimg_filename = "C:/Projects/2018/UavColliAvoid/dataset/calibration/imgs_EO/1/right" + tag;
		std::string leftout_filename = "C:/temp/calib_EO/rect_L" + tag;
		std::string rightout_filename = "C:/temp/calib_EO/rect_R" + tag;
		std::string lrout_filename = "C:/temp/calib_EO/rect_LR" + tag;

		//obj->computeRectifyImg(leftimg_filename, rightimg_filename, type, leftout_filename, rightout_filename);

		obj->computeRectifyImg(leftimg_filename, rightimg_filename, type, lrout_filename);
	}
	return 0;
}

int test_stereo_match(const std::string &pRoot)
{
	CfgStereoMatch cfg;
	cfg.alg = StereoMatchAlg::STEREO_MATCH_SGBM;
	//enum StereoMatchAlg {
	//	STEREO_MATCH_BM = 0,STEREO_MATCH_SGBM = 1,	STEREO_MATCH_SGBM_HH = 2,	STEREO_MATCH_SGBM_3WAY = 3
	//};

	if (IMG_W == 160) {
		cfg.SADWindowSize = 5;
		cfg.numberOfDisparities = 64;         //numberOfDisparities must be multiples of 16
	}
	else {
		cfg.SADWindowSize = 9;
		cfg.numberOfDisparities = 112;		 //numberOfDisparities must be multiples of 16
	}
	cfg.scale = 1;
	cfg.img_size = cv::Size(IMG_W, IMG_H);        //need to match the size in <leftImgFile> and <rightImgFile>
	cfg.calib_param_filename = pRoot + "/cam_stereo_nUsedImgs258.yml";
	cfg.img_ch = 1;							//image channels
	cfg.createDbgImgFlag = 2;			    //0, do not create debug img; 
											//1,  [desparity, depth];  
											//2, [rectfiedLeft, rectfiedRight; desparity, depth]
											//3, [rawLeft, rawRight; rectfiedLeft, rectfiedRight; desparity, depth]
											//4, [rawLeft, rectfiedLeft+depth]
	cfg.compute_point_cloud = true;
	cfg.input_images_already_rectified = false;
	cfg.zMax_meters = 35;

	StereoMatchPtr x(new StereoMatch());
	x->setParams(cfg);
	int sucFlag = x->init();
	if (sucFlag != 0) {
		dumpLog("can not init <StereoMatchPtr>!");
		return -1;
	}

	string curImgFolder = pRoot + "/frmRaw";	 //stereo image file folder
	bool dumpXYZ = true;
	string pout1 = pRoot + "/disparity";
	string pout2 = pRoot + "/xyz";
	myCreateDir(pout1);
	myCreateDir(pout2);

	uint32_t cnt = 0;
	std::vector<std::string> vFileNames;
	uint32_t n = getFileNameList(curImgFolder, "png", vFileNames);
	BOOST_FOREACH(std::string &fname, vFileNames) {
		printf("%d/%d: %s\n", cnt++, n, fname.c_str());
		string leftImgFile = curImgFolder + "/" + fname;
		string rightImgFile = curImgFolder + "/" + fname;
		string tag = fname.substr(4, 5);
		uint32 fn = atoi(tag.c_str());

		x->process(leftImgFile, rightImgFile, fn);

		string dumpFile1 = pout1 + "/disparity-frm-" + tag + ".png";
		x->dumpDebugImg(dumpFile1);

		if (dumpXYZ) {
			string dumpFile2 = pout2 + "/xyz-frm-" + tag + ".txt";
			x->dumpDepthMap(dumpFile2);
		}
		//if (cnt > 5) break;
	}
	return 0;
}


//todo
int test_stereo_match_old()
{
	const std::string pRoot = "";
	CfgStereoMatch cfg;
	cfg.alg = StereoMatchAlg::STEREO_MATCH_BM;
	cfg.SADWindowSize = 9;
	cfg.numberOfDisparities = 112;
	cfg.scale = 1.0;

	cfg.img_size = cv::Size(IMG_W, IMG_H);        //need to match the size in <leftImgFile> and <rightImgFile>
	cfg.calib_param_filename = pRoot + "/cam_stereo.yml";
	cfg.img_ch = 1;							//image channels
	cfg.createDbgImgFlag = 3;			    //0, do not create debug img; 
											//1,  [desparity, depth];  
											//2, [rectfiedLeft, rectfiedRight; desparity, depth]
											//3, [rawLeft, rawRight; rectfiedLeft, rectfiedRight; desparity, depth]
	cfg.compute_point_cloud = true;
	cfg.input_images_already_rectified = false;
	cfg.zMax_meters = 50;

	StereoMatchPtr x(new StereoMatch());
	x->setParams(cfg);
	int sucFlag = x->init();
	if (sucFlag != 0) {
		dumpLog("can not init <StereoMatchPtr>!");
		return -1;
	}

	vector<string> vSeqName;
	vSeqName.push_back("frm");
	vSeqName.push_back("apl-hallway");
	vSeqName.push_back("apl-hMove");
	vSeqName.push_back("apl-indoor");
	vSeqName.push_back("apl-static1");
	vSeqName.push_back("apl-static2");
	vSeqName.push_back("apl-vMove");
	vSeqName.push_back("backYard");
	vSeqName.push_back("community");
	vSeqName.push_back("irSeq1");
	vSeqName.push_back("irSeq2");
	vSeqName.push_back("irSeq3");
	vSeqName.push_back("irSeq4");
	vSeqName.push_back("irSeq5");

	bool dumpXYZ = false;
	string pSeqRoot = pRoot;
	string poutRoot = pRoot + "/disparity";
	BOOST_FOREACH(const string &seqName, vSeqName) {
		string pout1 = poutRoot + "/disparity_" + seqName;   //output folder
		if (!folderExists(pout1))
			_mkdir(pout1.c_str());
		else
			deleteFilesInFolder(pout1);

		string pout2 = poutRoot + "/xyz_" + seqName;         //output folder ;
		if (dumpXYZ) {
			if (!folderExists(pout2))
				_mkdir(pout2.c_str());
			else
				deleteFilesInFolder(pout2);
		}

		uint32_t cnt = 0;
		std::vector<std::string> vFileNames;
		string curImgFolder = pSeqRoot + "/" + seqName;	 //stereo image file folder
		uint32_t n = getFileNameList(curImgFolder, "png", vFileNames);
		BOOST_FOREACH(std::string &fname, vFileNames) {
			printf("%d/%d: %s\n", cnt++, n, fname.c_str());
			string leftImgFile = curImgFolder + "/" + fname;
			string rightImgFile = curImgFolder + "/" + fname;

			x->process(leftImgFile, rightImgFile);

			string tag = fname.substr(4, 5);
			string dumpFile1 = pout1 + "/disparity-frm-" + tag + ".jpg";
			x->dumpDebugImg(dumpFile1);

			if (dumpXYZ) {
				string dumpFile2 = pout2 + "/xyz-frm-" + tag + ".txt";
				x->dumpDepthMap(dumpFile2);
			}
			//if (cnt > 5) break;
		}
		break;
	}
	return 0;
}

int copyFiles()
{
	string srcFolder = "C:/Projects/2018/UavColliAvoid/dataset/boson4.4mm/vMove";
	string desFolder = "C:/Projects/2018/UavColliAvoid/dataset/boson4.4mm/apl-vMove";

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


int test_stereo_match_dumpLeftRectImg()
{
	CfgStereoMatch cfg;
	cfg.alg = StereoMatchAlg::STEREO_MATCH_SGBM;
	cfg.SADWindowSize = 5;
	cfg.numberOfDisparities = 64;         //numberOfDisparities must be multiples of 16
	cfg.scale = 1.0;
	cfg.img_size = cv::Size(IMG_W, IMG_H);        //need to match the size in <leftImgFile> and <rightImgFile>
	cfg.calib_param_filename = "C:/Projects/2018/UavColliAvoid/dataset/lepton/calib/2018-0710-b18cm/cam_stereo.yml";
	cfg.img_ch = 1;							//image channels
	cfg.createDbgImgFlag = 3;			    //0, do not create debug img; 
											//1,  [desparity, depth];  
											//2, [rectfiedLeft, rectfiedRight; desparity, depth]
											//3, [rawLeft, rawRight; rectfiedLeft, rectfiedRight; desparity, depth]
	cfg.compute_point_cloud = true;
	cfg.input_images_already_rectified = false;
	cfg.zMax_meters = 50;

	StereoMatchPtr x(new StereoMatch());
	x->setParams(cfg);
	int sucFlag = x->init();
	if (sucFlag != 0) {
		dumpLog("can not init <StereoMatchPtr>!");
		return -1;
	}
	
	std::vector<int> vBeg,vEnd;
	std::vector<std::string> vExp;

	vExp.push_back("ceiling-zGT151cm"); vBeg.push_back(0); vEnd.push_back(58);
	vExp.push_back("ceiling-zGT205cm"); vBeg.push_back(0); vEnd.push_back(84);
	vExp.push_back("ceiling-zGT279cm"); vBeg.push_back(0); vEnd.push_back(123);
	vExp.push_back("danA-zGT166cm"); vBeg.push_back(0); vEnd.push_back(38);
	vExp.push_back("danB-zGT332cm"); vBeg.push_back(0); vEnd.push_back(33);
	vExp.push_back("danC-zGT562cm"); vBeg.push_back(0); vEnd.push_back(55);
	vExp.push_back("hand-zGT53cm"); vBeg.push_back(20); vEnd.push_back(110);
	vExp.push_back("hand-zGT126cm"); vBeg.push_back(120); vEnd.push_back(230);
	vExp.push_back("head-zGT75cm"); vBeg.push_back(320); vEnd.push_back(340);
	vExp.push_back("person-zGT166cm"); vBeg.push_back(45); vEnd.push_back(64);

	
	string pRoot = "C:/Projects/2018/UavColliAvoid/dataset/lepton";
	char inFile[256], outFile[256];
	
	int n = vExp.size();
	for (int i = 0; i < n; ++i) {
		string expFoldder = pRoot + "/" + vExp[i];
		for (int fn = vBeg[i]; fn < vEnd[i]; ++fn) {
			snprintf(inFile, 256, "%s/frm-L-%05d.png", expFoldder.c_str(), fn);
			snprintf(outFile,256, "%s/frm-L-rect-%05d.png", expFoldder.c_str(), fn);
			x->recoveryRectL(std::string(inFile), std::string(outFile));
		}
	}
	return 0;
}

