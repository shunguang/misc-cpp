#if !_WINDOWS
#include "CapStereoV4L.h"

using namespace std;
using namespace cv;
using namespace cali;

// Define COLOR CODES
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

CapStereoV4L::CapStereoV4L(const CfgCapStereoIR &params)
	: CapBase(params)
	, m_thermal(SENSOR_T_BOSON320)
	, m_video_mode(V_MODE_YUV)
{
	m_vDevice[CAMERA_L] = "/dev/video" + std::to_string(m_params.cameraIdL);
	m_vDevice[CAMERA_R] = "/dev/video" + std::to_string(m_params.cameraIdR);

	m_vFileId[CAMERA_L] = -1;
	m_vFileId[CAMERA_R] = -1;
	m_thermal = params.sensor_type;
	m_video_mode = params.video_mode;
}

CapStereoV4L::~CapStereoV4L()
{
}


bool	CapStereoV4L::startCaptureThread()
{
	if ( !openDevices() ) {
		return false;
	}

	m_capThread.reset(new boost::thread(boost::bind(&CapStereoV4L::readStreamLoop, this)));
	return true;
}


bool CapStereoV4L::endCaptureThread()
{
	setForceExit(true);
	do{
		APP_SLEEP(5);
	} while ( !m_readLoopExit );


	for (int i = 0; i < CAMERA_CNT; ++i) {
		int fd = m_vFileId[i];
		int type = m_vBufferinfo[i].type;

		if ( fd >= 0 ) {
			// Deactivate streaming
			if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
				perror(RED "VIDIOC_STREAMOFF" WHT);
				myExit("CapStereoV4L::endCaptureThread(): VIDIOC_STREAMOFF");
			};

			close( fd );
			dumpLog("CapStereoV4L::endCaptureThread(): camera %s(dev=%s) closed!", g_cameraLabel[i], m_vDevice[i].c_str());
		}
		else {
			dumpLog("CapStereoV4L::endCaptureThread(): weird!  cannot close camera %s(dev=%s)", g_cameraLabel[i], m_vDevice[i].c_str());
		}
	}
	return true;
}


bool CapStereoV4L::openDevices()
{
	bool ret1 = openDevice(m_vDevice[CAMERA_L], CAMERA_L);
	bool ret2 = openDevice(m_vDevice[CAMERA_R], CAMERA_R);
	return ( ret1 && ret2 );
}


bool CapStereoV4L::openDevice(const std::string &video_device, const CAMERA_T id)
{
	// We open the Video Device
	int fd;
	struct v4l2_capability cap;

	if ((fd = open(video_device.c_str(), O_RDWR)) < 0) {
		myExit( "CapStereoV4L::openDevice(): Invalid video device: %s", video_device.c_str() );
	}
	m_vFileId[id] = fd;

	// Check VideoCapture mode is available
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
		myExit("CapStereoV4L::openDevice(): ERROR : VIDIOC_QUERYCAP. Video Capture is not available");
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		myExit("CapStereoV4L::openDevice(): The device does not handle single-planar video capture.");
	}

	//assertion img size settings in cfg
	if (m_thermal == SENSOR_T_BOSON320 || m_thermal == SENSOR_T_BOSON640) {
		if (m_video_mode == V_MODE_RAW16) {
			if (m_thermal == SENSOR_T_BOSON320) {
				if (m_params.frmW0 != 320 || m_params.frmH0 != 256) {
					myExit("CapStereoV4L::openDevice(): image size set check fail! -- A ");
				}
			}
			else {
				if (m_params.frmW0 != 640 || m_params.frmH0 != 512) {
					myExit("CapStereoV4L::openDevice(): image size set check fail! -- B ");
				}
			}
		}
		else {
			if (m_params.frmW0 != 640 || m_params.frmH0 != 512) {
				myExit("CapStereoV4L::openDevice(): image size set check fail! -- C ");
			}
		}
	}
	else if (m_thermal == SENSOR_T_LEPTON) {
		if (m_params.frmW0 != 160 || m_params.frmH0 != 120) {
			myExit("CapStereoV4L::openDevice(): image size set check fail! -- D ");
		}
	}

	struct v4l2_format format;
	// Common varibles
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = m_params.frmW0;
	format.fmt.pix.height = m_params.frmH0;

	// request desired FORMAT
	if (ioctl(fd, VIDIOC_S_FMT, &format) < 0) {
		myExit("CapStereoV4L::openDevice(): Unable to set video format");
	}

	// we need to inform the device about buffers to use.
	// and we need to allocate them.
	// we’ll use a single buffer, and map our memory using mmap.
	// All this information is sent using the VIDIOC_REQBUFS call and a
	// v4l2_requestbuffers structure:
	struct v4l2_requestbuffers &bufrequest = m_vBufrequest[id];
	bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufrequest.memory = V4L2_MEMORY_MMAP;
	bufrequest.count = 1;  // we are asking for one buffer

	if (ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0) {
		perror(RED "VIDIOC_REQBUFS" WHT);
		myExit("CapStereoV4L::openDevice(): VIDIOC_REQBUFS faile!");
	}

	// Now that the device knows how to provide its data,
	// we need to ask it about the amount of memory it needs,
	// and allocate it. This information is retrieved using the VIDIOC_QUERYBUF
	// call, and its v4l2_buffer structure.

	struct v4l2_buffer &bufferinfo = m_vBufferinfo[id];

	memset(&bufferinfo, 0, sizeof(bufferinfo));

	bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufferinfo.memory = V4L2_MEMORY_MMAP;
	bufferinfo.index = 0;

	if (ioctl(fd, VIDIOC_QUERYBUF, &bufferinfo) < 0) {
		perror(RED "VIDIOC_QUERYBUF" WHT);
		myExit("CapStereoV4L::openDevice(): VIDIOC_QUERYBUF faile!");
	}

	// map fd+offset into a process location (kernel will decide due to our NULL).
	// length and properties are also passed
	dumpLog( "CapStereoV4L::openDevice(): device = %s, cameraId=%s", video_device.c_str(), g_cameraLabel[id]);
	dumpLog( "CapStereoV4L::openDevice(): Image width = %d", m_params.frmW0);
	dumpLog( "CapStereoV4L::openDevice(): Image height = %d", m_params.frmH0);
	dumpLog( "CapStereoV4L::openDevice(): Buffer length = %d",  bufferinfo.length);
	return true;
}

void CapStereoV4L::readStreamLoop()
{
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PXM_BINARY);
	
	//-----------------------------
	int h = m_params.frmH0, w = m_params.frmW0;
	// Declarations for 8bits YCbCr mode
	// Will be used in case we are reading YUV format
	// Boson320, 640 :  4:2:0
	int luma_height = h + h / 2;
	int luma_width = w;

	int fd_L = m_vFileId[CAMERA_L];
	struct v4l2_buffer &bufferinfo_L = m_vBufferinfo[CAMERA_L];


	void* buffer_start_L = mmap(nullptr, bufferinfo_L.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_L, bufferinfo_L.m.offset);
	if (buffer_start_L == MAP_FAILED) {
		perror(RED "mmap" WHT);
		myExit("CapStereoV4L::openDevice(): mmap faile -- L !");
	}
	// Fill this buffer with zeros. Initialization. Optional but nice to do
	memset(buffer_start_L, 0, bufferinfo_L.length);
	// Declarations for RAW_16 representation
	// Will be used in case we are reading RAW_16 format
	cv::Mat thermal16_L(h, w, CV_16U, buffer_start_L);
	//cv::Mat thermal8_linear_L = cv::Mat::zeros(h, w, CV_8UC1);
	cv::Mat thermal_luma_L(luma_height, luma_width, CV_8UC1, buffer_start_L);
	cv::Mat thermal_gray_L = cv::Mat::zeros(h, w, CV_8UC1);

	int fd_R = m_vFileId[CAMERA_R];
	struct v4l2_buffer &bufferinfo_R = m_vBufferinfo[CAMERA_R];
	void* buffer_start_R = mmap(nullptr, bufferinfo_R.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_R, bufferinfo_R.m.offset);
	if (buffer_start_R == MAP_FAILED) {
		perror(RED "mmap" WHT);
		myExit("CapStereoV4L::openDevice(): mmap faile -- R !");
	}
	// Fill this buffer with zeros. Initialization. Optional but nice to do
	memset(buffer_start_R, 0, bufferinfo_R.length);
	cv::Mat thermal16_R(h, w, CV_16U, buffer_start_R);
	//cv::Mat thermal8_linear_R = cv::Mat::zeros(h, w, CV_8UC1);
	cv::Mat thermal_luma_R(luma_height, luma_width, CV_8UC1, buffer_start_R);
	cv::Mat thermal_gray_R = cv::Mat::zeros(h, w, CV_8UC1);

	// Activate streaming
	int type_L = bufferinfo_L.type;
	if (ioctl(fd_L, VIDIOC_STREAMON, &type_L) < 0) {
		perror(RED "CapStereoV4L::readStreamLoop(): VIDIOC_STREAMON fail-- L" WHT);
		myExit("CapStereoV4L::openDevice(): VIDIOC_STREAMON--L!");
	}

	int type_R = bufferinfo_R.type;
	if (ioctl(fd_R, VIDIOC_STREAMON, &type_R) < 0) {
		perror(RED "CapStereoV4L::readStreamLoop(): VIDIOC_STREAMON fail-- R" WHT);
		myExit("CapStereoV4L::openDevice(): VIDIOC_STREAMON - R!");
	}

	//---------------------------------------------
	//read some from stream
	//---------------------------------------------
	string dumpFolder = "./dump";
	myCreateDir(dumpFolder);

	int32_t dt0 = 1000 / m_params.frmRate;   //ms
	m_pauseCap = false;
	m_forceExit = false;
	m_readLoopExit = false;
	uint32_t fn = 0;
	boost::posix_time::ptime tPreWrt = POSIX_LOCAL_TIME;
	while (1) {
		bool pauseJob = isPause();
		if (pauseJob) {
			boost::mutex::scoped_lock lock(m_mutex4Working);
			m_condition4Working.wait(lock);
		}

		//capture L frame
		// Put the buffer in the incoming queue.
		if (ioctl(fd_L, VIDIOC_QBUF, &bufferinfo_L) < 0) {
			perror(RED "VIDIOC_QBUF--L" WHT);
			myExit("VIDIOC_QBUF--L");
		}
		// The buffer's waiting in the outgoing queue.
		if (ioctl(fd_L, VIDIOC_DQBUF, &bufferinfo_L) < 0) {
			perror(RED "VIDIOC_QBUF--L" WHT);
			myExit("VIDIOC_QBUF--L");  // TODO: exit() is not OK, throw exception and catch in thread
		}

		//capture R frame
		// Put the buffer in the incoming queue.
		if (ioctl(fd_R, VIDIOC_QBUF, &bufferinfo_R) < 0) {
			perror(RED "VIDIOC_QBUF--R" WHT);
			myExit("VIDIOC_QBUF--R");
		}
		// The buffer's waiting in the outgoing queue.
		if (ioctl(fd_R, VIDIOC_DQBUF, &bufferinfo_R) < 0) {
			perror(RED "VIDIOC_QBUF--R" WHT);
			myExit("VIDIOC_QBUF--R");  // TODO: exit() is not OK, throw exception and catch in thread
		}

		//------------------------------------------------------
		int dtSinceLastWrt = timeIntervalMillisec(tPreWrt);  //ms
		if (dtSinceLastWrt >= dt0) {
		// Linearly normalize 16-bit image to 0-255:
			if (m_video_mode == V_MODE_RAW16) {
			AGC_Basic_Linear(thermal16_L, thermal_gray_L, h, w);
			AGC_Basic_Linear(thermal16_R, thermal_gray_R, h, w);
		}
		else {
			cv::cvtColor(thermal_luma_L, thermal_gray_L, cv::COLOR_YUV2GRAY_420);
			cv::cvtColor(thermal_luma_R, thermal_gray_R, cv::COLOR_YUV2GRAY_420);
			
				//cv::Mat thermal_rgb_L = cv::Mat::zeros(h, w, CV_8UC3);
				//cv::Mat thermal_rgb_R = cv::Mat::zeros(h, w, CV_8UC3);
				//cv::cvtColor(thermal_luma_L, thermal_rgb_L, cv::COLOR_YUV2RGB_I420);
				//cv::cvtColor(thermal_luma_R, thermal_rgb_R, cv::COLOR_YUV2RGB_I420);
			
		}

#if 0
			char filename[64];
			sprintf(filename, "./I%lu_gray_L.png", fn);
			imwrite(filename, thermal_gray_L , compression_params );
			sprintf(filename, "./I%lu_gray_R.png", fn);
			imwrite(filename, thermal_gray_R , compression_params );
#endif

			RawFrmIrStereoImgPtr frm(new RawFrmIrStereoImg(w, h));
			frm->fn = fn;
			thermal_gray_L.copyTo(frm->imgL);
			thermal_gray_R.copyTo(frm->imgR);
#if _DEBUG
			assert(frm->imgR.rows == m_params.frmH0);
			assert(frm->imgR.cols == m_params.frmW0);
#endif
			if (m_params.isDump) {
				frm->dumpImg(dumpFolder);
			}
			m_frmQ->wrtNext(frm);
			tPreWrt = POSIX_LOCAL_TIME;
			if( fn%100==0 ){
				printf( "CapStereoV4L::readStreamLoop(): frm->fn = %d\n", frm->fn );
			}
			
			fn++;
		}

		if (isForceExit()) {
			break;
		}
	}
	m_readLoopExit = true;
}


// AGC Sample ONE: Linear from min to max.
// Input is a MATRIX (height x width) of 16bits. (CV_16U mat)
// Output is a MATRIX (height x width) of 8 bits (CV_8U mat)
void CapStereoV4L::AGC_Basic_Linear(Mat input_16, Mat output_8, int height, int width) 
{
	int i, j;  // aux variables

	// auxiliary variables for AGC calcultion
	unsigned int max1 = 0;         // 16 bits
	unsigned int min1 = 0xFFFF;    // 16 bits
	unsigned int value1, value2, value3, value4;

	// RUN a super basic AGC
	for (i = 0; i<height; i++) {
		for (j = 0; j<width; j++) {
			value1 = input_16.at<uchar>(i, j * 2 + 1) & 0XFF;  // High Byte
			value2 = input_16.at<uchar>(i, j * 2) & 0xFF;    // Low Byte
			value3 = (value1 << 8) + value2;
			if (value3 <= min1) {
				min1 = value3;
			}
			if (value3 >= max1) {
				max1 = value3;
			}
			//printf("%X.%X.%X  ", value1, value2, value3);
		}
	}
	//printf("max1=%04X, min1=%04X\n", max1, min1);

	for (int i = 0; i<height; i++) {
		for (int j = 0; j<width; j++) {
			value1 = input_16.at<uchar>(i, j * 2 + 1) & 0XFF;  // High Byte
			value2 = input_16.at<uchar>(i, j * 2) & 0xFF;    // Low Byte
			value3 = (value1 << 8) + value2;
			value4 = ((255 * (value3 - min1))) / (max1 - min1);
			// printf("%04X \n", value4);

			output_8.at<uchar>(i, j) = (uchar)(value4 & 0xFF);
		}
	}

}
#endif
