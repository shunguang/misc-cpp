/*
appsrc - src|->srcCapsFilter->timeoverla->encoderH264->vidCapsFilter->video_queue->mpegtsmux->filesink
          ^
    inject YuvFrm into
    GstBuffer and 
    attach the 
    metadata on it       
*/  

#ifndef __MUX_ENC_USER_DATA2_H__
#define __MUX_ENC_USER_DATA2_H__

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

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>


namespace ngv
{
    struct MuxEncUserData2
    {
        MuxEncUserData2(const CfgEncPtr &cfg);
        ~MuxEncUserData2();

        bool createElements();
        bool createBinAndLink();


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
        GstElement *videoSrc{nullptr};
        GstElement *srcCapsFilter{nullptr};
        GstElement *encoder264{nullptr};
        GstElement *mpegtsmux {nullptr};

        //GstElement *parser {nullptr};
        GstElement *vidCapsFilter{nullptr};
        //GstElement *videoConvert{nullptr};
        //GstElement *videoScale{nullptr};
        GstElement *timeoverlay{nullptr};
        GstElement *video_queue{nullptr};
        GstElement *filesink{nullptr};
    };
}
#endif