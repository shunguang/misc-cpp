#ifndef _CAP_STEREO_V4L_H_
#define _CAP_STEREO_V4L_H_

#if !_WINDOWS
#include "CapBase.h"
#include <stdio.h>
#include <fcntl.h>               // open, O_RDWR
#include <opencv2/opencv.hpp>
#include <unistd.h>              // close
#include <sys/ioctl.h>           // ioctl
#include <asm/types.h>           // videodev2.h
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/videodev2.h>

#include "libutil/util.h"

namespace cali {
	class CapStereoV4L : public CapBase {
	public:
		CapStereoV4L( const CfgCapStereoIR &params );
		~CapStereoV4L();

		//<param> input and  output
		virtual bool	startCaptureThread()  ;
		virtual bool    endCaptureThread();

	private:
		bool openDevices();
		void readStreamLoop();
		bool openDevice(const std::string &dev, const CAMERA_T id );
		void AGC_Basic_Linear( cv::Mat input_16, cv::Mat output_8, int height, int width);
	private:
		SENSOR_T		m_thermal;
		VIDEO_MODE_T    m_video_mode;
		int				m_vFileId[CAMERA_CNT];          
		std::string		m_vDevice[CAMERA_CNT];  // "/dev/video1", "/dev/video2", etc ...

		struct v4l2_buffer			m_vBufferinfo[CAMERA_CNT];
		struct v4l2_requestbuffers	m_vBufrequest[CAMERA_CNT];

	};
	typedef std::shared_ptr<CapStereoV4L> CapStereoV4LPtr;

}
#endif
#endif
