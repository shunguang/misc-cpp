#include "CapStereoIrFromCameras.h"

using namespace std;
using namespace cv;
using namespace cali;

CapStereoIrFromCameras::CapStereoIrFromCameras(const CfgCapStereoIR& params)
	: CapBase(params)
	, m_nCameras(CAMERA_CNT)
{
	m_vCamId[CAMERA_L] = m_params.cameraIdL;
	m_vCamId[CAMERA_R] = m_params.cameraIdR;
	if (m_params.cameraIdL == m_params.cameraIdR) {
		m_nCameras = 1;
	}
}

CapStereoIrFromCameras::~CapStereoIrFromCameras()
{
}


bool	CapStereoIrFromCameras::startCaptureThread()
{
	if (!openDevices()) {
		return false;
	}

	m_capThread.reset(new boost::thread(boost::bind(&CapStereoIrFromCameras::readStreamLoop, this)));
	return true;
}


bool CapStereoIrFromCameras::endCaptureThread()
{
	setForceExit(true);
	do {
		APP_SLEEP(5);
	} while (!m_readLoopExit);


	for (int i = 0; i < m_nCameras; ++i) {
		if (m_vCap[i].isOpened()) {
			m_vCap[i].release();
			dumpLog("CapStereoIrFromCameras::endCaptureThread(): camera %s(id=%d) closed!", g_cameraLabel[i], m_vCamId[i]);
		}
		else {
			dumpLog("CapStereoIrFromCameras::endCaptureThread(): weird!  cannot close camera %s(id=%d)", g_cameraLabel[i], m_vCamId[i]);
		}
	}
	return true;
}

void CapStereoIrFromCameras::readStreamLoop()
{
	//---------------------------------------------
	//read some from stream
	//---------------------------------------------
	int32_t dt0 = 1000 / m_params.frmRate;   //ms
	cv::Mat rgbL, rgbR;
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

		//capture a frame
		if (m_nCameras == 2) {
			m_vCap[CAMERA_L] >> rgbL;
			m_vCap[CAMERA_R] >> rgbR;
		}
		else{
			m_vCap[CAMERA_L] >> rgbL;
			rgbL.copyTo(rgbR);
		}
		int dtSinceLastWrt = timeIntervalMillisec(tPreWrt);  //ms
		if (dtSinceLastWrt >= dt0) {
			RawFrmIrStereoImgPtr frm(new RawFrmIrStereoImg(m_params.frmW0, m_params.frmH0));
			frm->fn = fn++;
			cvtColor(rgbL, frm->imgL, cv::COLOR_RGB2GRAY);
			cvtColor(rgbR, frm->imgR, cv::COLOR_RGB2GRAY);
#if _DEBUG
			if (frm->imgR.rows != m_params.frmH0 || frm->imgR.cols != m_params.frmW0) {
				dumpLog("CapStereoIrFromCameras::readStreamLoop(): R-img sz does not match!");
			}
			if (frm->imgL.rows != m_params.frmH0 || frm->imgL.cols != m_params.frmW0) {
				dumpLog("CapStereoIrFromCameras::readStreamLoop():L-img sz does not match!");
			}
#endif
			m_frmQ->wrtNext(frm);
			tPreWrt = POSIX_LOCAL_TIME;
		}

		if (isForceExit()) {
			break;
		}
	}
	m_readLoopExit = true;
}

bool CapStereoIrFromCameras::openDevices()
{
	int failCnt = 0;

	for (int i = 0; i < m_nCameras; ++i) {
		int id = m_vCamId[i];
		m_vCap[i].open(id);
		if (m_vCap[i].isOpened()) {
			//set properpties
			bool suc;
			suc = m_vCap[i].set(cv::CAP_PROP_FPS, m_params.frmRate);
			if (!suc) {
				dumpLog("cannot set FPS property for camera %s(id=%d)\n", g_cameraLabel[i], id);
			}
		}
		else {
			dumpLog("camera %s(id=%d) cannot be opened\n", g_cameraLabel[i], id);
			failCnt++;
		}
	}

	return (failCnt == 0);
}

