/*
*top level build piplline:
*     appsrc->srcCapsFilter->timeoverla->videoEncoder->vidCapsFilter->--
*(inject YuvImg)                                                        |->video_queue->mpegtsmux->filesink
*                                                              appSrc->--             
*                                                      (inject klv metadata)                                                
* 
*    or
*testvideoSrc->srcCapsFilter->timeoverla->videoEncoder->vidCapsFilter->---
*                                                                         |->video_queue->mpegtsmux->filesink
*                                                              appSrc->----             
*                                                         (inject klv metadata)                                                
*/  

#ifndef __MUX_ENC_USER_DATA_H__
#define __MUX_ENC_USER_DATA_H__

#include <iostream>
#include <cassert>
#include <string>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

#include "AppLog.h"
#include "HostKlvFrm.h"
#include "HostYuvFrm.h"
#include "Uint8Stream.h"
#include "Cfg.h"
#include "AppTicToc.h"
#include "AppCpuTimeProf.h"

#include <sys/resource.h>        

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#define USE_VIDEO_TEST_SRC 0
#define APP_ASYNC_KLV 0

namespace ngv
{
    struct MuxEncUserData
    {
        MuxEncUserData(const CfgEncPtr &cfg);
        ~MuxEncUserData();

        bool readNextYuvFrm();
        uint16_t readNextKlvFrm(std::vector<uint8_t> &v);

        ngv::HostYuvFrmPtr m_hostYuvFrm{nullptr};        //YUV420  frm
        ngv::HostKlvFrmPtr m_hostKlvFrm{nullptr};        //the KLV for frm i w/ metadata
        ngv::HostKlvFrmPtr m_hostKlvFrmDefault{nullptr}; //the KLV for frames w/o metadata

        //-----------------------------
        const CfgEncPtr  m_cfg{nullptr};
        int64_t m_timeDuration_nanoSec{0};
        GstClockTime m_timestamp{0};

        int64_t m_timeDurationKlv_nanoSec{0};
        GstClockTime m_timestampKlv{0};

        uint64_t m_klvCnt{0};
        uint64_t m_imgCnt{0};
        FILE *m_fpKlv{nullptr};
        FILE *m_fpYuv{nullptr};
        bool m_hasKlv{false};

        GMainLoop *loop{nullptr};
        GstElement *pipeline{nullptr};
        GstElement *vidSrc{nullptr};
        GstElement *srcCapsFilter{nullptr};
        GstElement *timeoverlay{nullptr};
        //GstElement *vidScale{nullptr};
        //GstElement *vidParser {nullptr};
        GstElement *vidConvert{nullptr};
        GstElement *vidEncoder{nullptr};
        GstElement *vidCapsFilter{nullptr};

        GstElement *klvSrc{nullptr};

        GstElement *muxQueue{nullptr};
        GstElement *mpegtsmux {nullptr};
        GstElement *fileSink{nullptr};

        //GstCaps *vidSrc_caps{nullptr};
        //GstCaps *klvSrc_caps{nullptr};
        //GstCaps *vidFilter_caps{nullptr};
        
        GstStateChangeReturn ret{GST_STATE_CHANGE_FAILURE};
        GstMessage *msg{nullptr};

        //for statistic the average enc time
        AppTicToc      m_att;
        AppCpuTimeProf m_cpuProf{100};
    };
}
#endif