#include "libutil/util.h"
#include "libCapImg/CapStereoIrUI.h"
#if !_WINDOWS
#include <ncurses.h>
#endif
using namespace std;
using namespace cv;
using namespace cali;

#define SAVE_CAP_IMGS_AFTER_QUIT 1

#if _WINDOWS
int test_cap_stereoIR_windows(int argc, char **argv);
#else
int test_cap_stereoIR_linux(int argc, char **argv);
#endif

int test_capture(int argc, char **argv)
{
#if _WINDOWS
	test_cap_stereoIR_windows(argc, argv);
#else
	test_cap_stereoIR_linux(argc, argv);
#endif
	return 0;
}


#if _WINDOWS
int test_cap_stereoIR_windows(int argc, char **argv)
{
	int leftId, rightId, frmRate;
	int isLepton, isDump;
	if (argc < 6) {
		printf("usage: captureStereo.exe <isLepton> <leftId> <rightId> <isDump> <frmRate>\n");
		printf("[1]    captureStereo.exe 1 0 1 1 5 (captur lepton max 5000 frms at FPS=5  and dumped to c:/temp/t1 \n");
		printf("[2]    captureStereo.exe 0 0 1 1 3 (captur Boson max 5000 frms at FPS=3 and dumped to c:/temp/t1\n");
		printf("Press esp or q Quit\n");
		printf("If it's only one camera, input the same id\n");
		return -1;
	}

	printf("Press Esp quit\n");

	CfgCapStereoIR params;

	isLepton = atoi(argv[1]);
	leftId  = atoi(argv[2]);
	rightId = atoi(argv[3]);
	isDump  = atoi(argv[4]);
	frmRate = atoi(argv[5]);

	params.src = VIDEO_SRC_FROM_CAMERA;
	if	(isLepton){
		params.sensor_type = SENSOR_T_LEPTON;
		params.video_mode = V_MODE_RAW16; 	           
		params.frmW0 = 160;
		params.frmH0 = 120;
		params.frmRate = frmRate;
	}
	else{
		params.sensor_type = SENSOR_T_BOSON640;       //
		params.video_mode = V_MODE_YUV;
		params.frmW0 = 640;
		params.frmH0 = 512;
		params.frmRate = frmRate; // 25.0f;
	}
	params.cameraIdL = leftId;   //usb port 3 in windows
	params.cameraIdR = rightId;  //usb port 4 in windows
	params.nChannels = 1;
	params.imgDepth = 8;
	params.nQueueSize = 100;

	std::string msg;
	std::string winTitle = "left-right: press Esc quit";

	RawFrmQPtr qInsideCapThread(new RawFrmQ( params.nQueueSize) );


	CapStereoIrUI_ptr x(new CapStereoIrUI());
	x->setCfg(params);
	x->setOutputFrmImgQ(qInsideCapThread);
	bool suc = x->startCapThread(msg);
	if (!suc) {
		dumpLog("%s,%s", __FUNCTION__, msg.c_str());
		return -1;
	}
	namedWindow(winTitle, 1);

	uint32_t nTotFrms =5000;
	RawFrmQPtr outQ( new RawFrmQ(nTotFrms) );

	RawFrmIrStereoImg frm;
	boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;

	int cnt=0, cnt2=0, nFrmsInQ=0;
	while(1){
		bool newFrm = x->getNextFrm(frm);
		if (newFrm) {
			cnt = frm.fn;

			RawFrmIrStereoImgPtr newFrm(new RawFrmIrStereoImg(frm));
			newFrm->fn = nFrmsInQ;
			outQ->wrtNext(newFrm);
			nFrmsInQ++;

			int dt = timeIntervalMillisec(t0);
			//frm.dumpImg2( dumFolder,  "left", "right" );
			cv::Mat I = frm.combineLR();
			cv::putText(I, num_to_string(cnt, 5, "L="), cv::Point(10, 10), 1, 1, (0, 255, 255), 1);
			cv::putText(I, "R", cv::Point(params.frmW0+10, 10), 1, 1, (0, 255, 255), 1);
			cv::putText(I, string("dt=")+to_string(dt), cv::Point(10, 50), 1, 1, (0, 255, 255), 1);

			//cv::putText(I, string("press Esc quit..."), cv::Point(10, frm.imgH-50), 1, 1, (0, 255, 255), 1);
			imshow(winTitle, I);
			t0 = POSIX_LOCAL_TIME;
			cnt2++;
		}
		
		int key = 0;
		key = waitKey(1);
		if (key == 27 || (char)key == 'q'  /*|| cnt>= nTotFrms*/) {
			x->endCaputreThread();
			break;
		}
	}

	//now dump the results
	if (isDump) {
		string dumFolder = "c:/temp/t1";
		createDir(dumFolder);

		printf("now dump the frms ... ");
		cnt = (nFrmsInQ > nTotFrms) ? nTotFrms : nFrmsInQ;
		for (int i = 0; i < cnt; ++i) {
			outQ->readNext(frm);
			frm.dumpImg(dumFolder);
			//frm.dumpImg2( dumFolder,  "left", "right" );
			if (i % 100 == 0) {
				printf("cnt=%d, i=%d\n", cnt, i);
			}
		}
	}
	return 0;
}

#else
int test_cap_stereoIR_linux(int argc, char **argv)
{
	CfgCapStereoIR params;
	//for Boson:
	//1.(SENSOR_T_BOSON320, V_MODE_YUV) ->   (w=640,h=512) --- choose this match windows calibration results
	//2.(SENSOR_T_BOSON320, V_MODE_RAW16) -> (w=320,h=256) --- todo: how to catpture this size in windows
	//3.(SENSOR_T_BOSON640, V_MODE_YUV) ->   (w=640,h=512), we donot have this camera yet
	//4.(SENSOR_T_BOSON640, V_MODE_RAW16) -> (w=640,h=512), we donot have this camera yet
	
	//for Lepton:
	//1.(SENSOR_T_LEPTON, V_MODE_RAW16) ->   (w=160,h=120) --- choose this match windows calibration results
	
	bool isLepton = true;
	params.src = VIDEO_SRC_FROM_V4L;
	if	(isLepton){
		params.sensor_type = SENSOR_T_LEPTON;
		params.video_mode = V_MODE_RAW16; 	           
		params.frmW0 = 160;
		params.frmH0 = 120;
		params.frmRate = 7.0;
		params.cameraIdL = 1;  // "/dev/video1" in linux
		params.cameraIdR = 2;  // "/dev/video2" in linux
	}
	else{
		params.sensor_type = SENSOR_T_BOSON320;       
		params.video_mode = V_MODE_RAW16; 	//V_MODE_YUV;
		if (params.video_mode == V_MODE_RAW16){
			params.frmW0 = 320;
			params.frmH0 = 256;
		}
		else if (params.video_mode == V_MODE_YUV){
			params.frmW0 = 640;
			params.frmH0 = 512;
		}
		else{
			assert(0);
		}
		
		params.frmRate = 20.0; // 25.0f;
		params.cameraIdL = 3;  // "/dev/vdieo3" in linux
		params.cameraIdR = 4;  // "/dev/vdieo4" in linux
	}
	params.nChannels = 1;
	params.imgDepth = 8;
	params.nQueueSize = 100;

	std::string msg;
	std::string winTitle = "left-right: press Esc quit";

	RawFrmQPtr qInsideCapThread(new RawFrmQ( params.nQueueSize) );


	CapStereoIrUI_ptr x(new CapStereoIrUI());
	x->setCfg(params);
	x->setOutputFrmImgQ(qInsideCapThread);
	bool suc = x->startCapThread(msg);
	if (!suc) {
		dumpLog("%s,%s", __FUNCTION__, msg.c_str());
		return -1;
	}
#if SAVE_CAP_IMGS_AFTER_QUIT
	uint32_t nTotFrms = 100;
	RawFrmQPtr outQ( new RawFrmQ(nTotFrms) );
#endif

	RawFrmIrStereoImg frm;
	boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;
	string dumFolder="./dump";
	myCreateDir( dumFolder );
	
	int cnt=0, cnt2=0, nFrmsInQ=0;
	while(1){
		bool newFrm = x->getNextFrm(frm);
		if (newFrm) {
			cnt = frm.fn;
#if SAVE_CAP_IMGS_AFTER_QUIT
			if ( cnt % 1 == 0) {
				RawFrmIrStereoImgPtr newFrm(new RawFrmIrStereoImg(frm));
				newFrm->fn = nFrmsInQ;
				outQ->wrtNext(newFrm);
				nFrmsInQ++;
			}
#endif
			int dt = timeIntervalMillisec(t0);
			//frm.dumpImg2( dumFolder,  "left", "right" );
			cv::Mat I = frm.combineLR();
			cv::putText(I, num_to_string(cnt, 5, "L="), cv::Point(10, 10), 1, 1, (0, 255, 255), 1);
			cv::putText(I, "R", cv::Point(params.frmW0+10, 10), 1, 1, (0, 255, 255), 1);
			cv::putText(I, string("dt=")+to_string(dt), cv::Point(10, 50), 1, 1, (0, 255, 255), 1);
			
			printf("cnt=%d\n", cnt);
			myImgWrite(I, dumFolder, "img", cnt, 0);
			t0 = POSIX_LOCAL_TIME;
			cnt2++;
		}

		if( cnt>=50 || cnt2>=100){
			x->endCaputreThread();
			break;
		}
	}

	//now dump the results
#if SAVE_CAP_IMGS_AFTER_QUIT
	printf("now dump the frms ... ");
	cnt = (nFrmsInQ > nTotFrms) ? nTotFrms : nFrmsInQ;
	for (int i = 0; i < cnt; ++i) {
		outQ->readNext(frm);
		frm.dumpImg ( dumFolder );
		//frm.dumpImg2( dumFolder,  "left", "right" );

		if (i % 100 == 0) {
			printf("cnt=%d, i=%d\n", cnt, i);
		}
	}
#endif
	return 0;
}
#endif
