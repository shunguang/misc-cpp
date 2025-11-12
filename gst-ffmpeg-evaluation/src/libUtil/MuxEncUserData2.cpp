//appsrc->srcCapsFilter->timeoverla->encoderH264->vidCapsFilter->video_queue->mpegtsmux->filesink

#include "MuxEncUserData2.h"
using namespace ngv;
MuxEncUserData2::MuxEncUserData2(const CfgEncPtr &cfg)
    : m_cfg(cfg), m_hostYuvFrm(new ngv::HostYuvFrm(cfg->imgW, cfg->imgH, 0)), m_hostKlvFrm(new ngv::HostKlvFrm(0, 0, 512)), m_hostKlvFrmDefault(new ngv::HostKlvFrm(0, 4, 4))
{
    dumpLog("MuxEncUserData2()---AA");
    m_timeDuration_nanoSec = gst_util_uint64_scale_int(m_cfg->tbn /*1*/, GST_SECOND /*1sec = 1e+9 nano sec*/, m_cfg->tbd /*25*/);
    m_timeDurationKlv_nanoSec = gst_util_uint64_scale_int(m_cfg->tbn /*1*/, GST_SECOND /*1sec = 1e+9 nano sec*/, m_cfg->tbd /*25*/);
    dumpLog("m_timeDurationKlv_nanoSec=%ld", m_timeDurationKlv_nanoSec);

    //open data file
    m_fpYuv = fopen(m_cfg->inputYuvFile.c_str(), "rb");
    if (!m_fpYuv)
    {
        appExit("appQuit: cannot open file: %s", m_cfg->inputYuvFile.c_str());
    }

    m_fpKlv = fopen(m_cfg->inputKlvFile.c_str(), "rb");
    if (!m_fpKlv)
    {
        appExit("appQuit: cannot open file: %s", m_cfg->inputKlvFile.c_str());
    }

    //prepare KVLV, if the 1 video frm has KLV we use <m_hostKlvFrm>, otherwise <m_hostKlvFrmDefault> will be used as facked metadata
    m_hostKlvFrmDefault->setToZeros();
    m_hasKlv = m_hostKlvFrm->readFromBinFile(m_fpKlv);
    dumpLog("MuxEncUserData2()---BB");
}

MuxEncUserData2::~MuxEncUserData2()
{
    if (m_fpYuv)
        fclose(m_fpYuv);
    if (m_fpKlv)
        fclose(m_fpKlv);
}

bool MuxEncUserData2::readNextYuvFrm()
{
    bool suc = m_hostYuvFrm->readFromBinFile(m_fpYuv, m_imgCnt);
    m_imgCnt++;
    return suc;
}

uint16_t MuxEncUserData2::readNextKlvFrm(std::vector<uint8_t> &v)
{
    Uint8Stream os;

    if (m_hasKlv && m_hostKlvFrm->fn_ == m_klvCnt)
    {
        m_hostKlvFrm->enSerial(os);
        os.getData(v);
        //std::cout << "used metadat:" << m_hostKlvFrm->toString() << std::endl;

        //update <m_hosKlvFrm>
        m_hasKlv = m_hostKlvFrm->readFromBinFile(m_fpKlv);
    }
    else
    {
        m_hostKlvFrmDefault->fn_ = m_klvCnt;
        m_hostKlvFrmDefault->enSerial(os);
        os.getData(v);
        //std::cout << "used metadat:" << m_hostKlvFrmDefault->toString() << std::endl;
    }

    m_klvCnt++;
    return v.size();
}

bool MuxEncUserData2::createElements()
{
    GstCaps *videoSrc_caps{nullptr};
    GstCaps *video_filter_caps{nullptr};

    loop = g_main_loop_new(NULL, false);

    videoSrc = gst_element_factory_make("appsrc", "videoSrc");
    srcCapsFilter = gst_element_factory_make("capsfilter", NULL);
    encoder264 = gst_element_factory_make((const gchar *)m_cfg->getGstEncName(), NULL);
    mpegtsmux = gst_element_factory_make("mpegtsmux", NULL);
    video_queue = gst_element_factory_make("queue", "video_queue");
    //parser = gst_element_factory_make("h264parse", NULL);
    vidCapsFilter = gst_element_factory_make("capsfilter", NULL);
    //videoConvert = gst_element_factory_make("videoconvert", NULL);
    //videoScale = gst_element_factory_make("videoscale", NULL);
    timeoverlay = gst_element_factory_make("timeoverlay", NULL);
    filesink = gst_element_factory_make("filesink", "filesink");

    dumpLog("AAA--------\n");

    pipeline = gst_pipeline_new("encode-pipeline");

    //!videoConvert || !videoScale || ! parser

    if (!pipeline || !videoSrc || !srcCapsFilter || !mpegtsmux || !vidCapsFilter || !timeoverlay || !video_queue || !filesink)
    {
        dumpLog("Not all elements could be created.\n");
        return false;
    }

    videoSrc_caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "I420",
                                        "width", G_TYPE_INT, m_cfg->imgW,
                                        "height", G_TYPE_INT, m_cfg->imgH,
                                        "framerate", GST_TYPE_FRACTION, m_cfg->tbd, m_cfg->tbn, NULL);

    g_object_set(videoSrc, "format", GST_FORMAT_TIME, NULL);
    g_object_set(videoSrc, "do-timestamp", TRUE, NULL);
    g_object_set(srcCapsFilter, "caps", videoSrc_caps, NULL);

    video_filter_caps = gst_caps_from_string("video/x-h264, stream-format=(string)byte-stream");
    g_object_set(vidCapsFilter, "caps", video_filter_caps, NULL);

    g_object_set(G_OBJECT(encoder264), "bitrate", m_cfg->bitrate, NULL);
    g_object_set(G_OBJECT(filesink), "location", m_cfg->ouputFilePath.c_str(), NULL);
    g_printerr("BBB--------\n");

    gst_caps_unref (videoSrc_caps);
    gst_caps_unref (video_filter_caps);
    
    return true;
}

bool MuxEncUserData2::createBinAndLink()
{
    //removed: videoConvert, videoScale,
    gst_bin_add_many(GST_BIN(pipeline), videoSrc, srcCapsFilter, timeoverlay, encoder264, vidCapsFilter,
                     video_queue, mpegtsmux, filesink, NULL);

    //appsrc->srcCapsFilter->timeoverla->encoderH264->vidCapsFilter->video_queue->mpegtsmux->filesink
    if (!gst_element_link_many(videoSrc, srcCapsFilter, timeoverlay, encoder264, vidCapsFilter,
                               video_queue, mpegtsmux, filesink, NULL))
    {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return false;
    }
    return true;
}
