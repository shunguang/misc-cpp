#ifndef _CAP_STEREO_IR_FROM_CAMERAS_H_
#define _CAP_STEREO_IR_FROM_CAMERAS_H_

#include "CapBase.h"
namespace cali {
	class CapStereoIrFromCameras : public CapBase {
	public:
		CapStereoIrFromCameras(const CfgCapStereoIR& params);
		~CapStereoIrFromCameras();

		//<param> input and  output
		virtual bool	startCaptureThread();
		virtual bool  endCaptureThread();

	private:
		bool openDevices();
		void readStreamLoop();

	private:
		int								m_vCamId[CAMERA_CNT];
		cv::VideoCapture	m_vCap[CAMERA_CNT];
		int								m_nCameras;             //total # of cameras, =1 if left and right has the same ID.
	};
	typedef std::shared_ptr<CapStereoIrFromCameras>		CapStereoIrFromCamerasPtr;

}
#endif
