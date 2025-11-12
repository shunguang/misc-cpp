
// (1) test_gst_mux_enc_v0.cpp is the initial study version
// (2) test_gst_mux_enc_v1.cpp is the final version of test_gst_mux_enc_v0.cpp 
// (3) both versions functionally are same with each other
//
//modified from https://github.com/impleotv/gstreamer-test-klv

/*
testvideoSrc->srcCapsFilter->timeoverla->encoderH264->vidCapsFilter->video_queue->mpegtsmux->filesink
                                                                                ^
                                                                                |                   
                                                                              metadataSrc
*/  
//-------------------------------------------------------------------
// test_gst_klv_enc_min_v1.cpp
// an example to enc metadata wand image by two callback functions
//-------------------------------------------------------------------
#include <iostream>
#include <cassert>
#include <string>
#include <thread>
#include <stdio.h>
#include <unistd.h>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include <dlfcn.h>
#include <unistd.h>

#include "libUtil/util.h"
#include "libUtil/Cfg.h"


#define USE_VIDEO_TEST_SRC 0
#define APP_ASYNC_KLV 0

using namespace ngv;
struct UserData{
    UserData( const CfgEnc &cfg )
    //: m_hostYuvFrm( new ngv::HostYuvFrm(MY_VIDEO_W, MY_VIDEO_H, 0))
    : m_hostYuvFrm( new ngv::HostYuvFrm( cfg.imgW, cfg.imgH, 0) )
    , m_hostKlvFrm( new ngv::HostKlvFrm(0,0,512) )
    , m_hostKlvFrmDefault( new ngv::HostKlvFrm(0,4,4) )
    {
      	m_timeDuration_nanoSec = gst_util_uint64_scale_int( cfg.tbn /*1*/, GST_SECOND /*1sec = 1e+9 nano sec*/, cfg.tbd /*25*/);
      	m_timeDurationKlv_nanoSec = gst_util_uint64_scale_int( cfg.tbn /*1*/, GST_SECOND /*1sec = 1e+9 nano sec*/, cfg.tbd /*25*/);
        dumpLog("m_timeDurationKlv_nanoSec=%ld", m_timeDurationKlv_nanoSec);

        //open data file
        m_fpYuv = fopen(cfg.inputYuvFile.c_str(), "rb");
        if(!m_fpYuv){
            appExit("appQuit: cannot open file: %s", cfg.inputYuvFile.c_str() );
        }

        m_fpKlv = fopen( cfg.inputKlvFile.c_str(), "rb");
        if(!m_fpKlv){
            appExit("appQuit: cannot open file: %s", cfg.inputKlvFile.c_str() );
        }

        //prepare KVLV, if the 1 video frm has KLV we use <m_hostKlvFrm>, otherwise <m_hostKlvFrmDefault> will be used as facked metadata
        m_hostKlvFrmDefault->setToZeros();
        m_hasKlv = m_hostKlvFrm->readFromBinFile( m_fpKlv );

    }

    ~UserData()
    {
        if (m_fpYuv) fclose( m_fpYuv );
        if (m_fpKlv) fclose( m_fpKlv );
    }

    bool readNextYuvFrm(){
        bool suc = m_hostYuvFrm->readFromBinFile( m_fpYuv, m_imgCnt);
        m_imgCnt++;
        return suc;
    }

    uint16_t readNextKlvFrm( std::vector<uint8_t> &v ){
        Uint8Stream os;

        if( m_hasKlv && m_hostKlvFrm->fn_ == m_klvCnt ){
            m_hostKlvFrm->enSerial(os);
            os.getData(v);
            //std::cout << "used metadat:" << m_hostKlvFrm->toString() << std::endl;

            //update <m_hosKlvFrm>
            m_hasKlv = m_hostKlvFrm->readFromBinFile(m_fpKlv);
        }
        else{
          m_hostKlvFrmDefault->fn_ =  m_klvCnt;
          m_hostKlvFrmDefault->enSerial(os);
          os.getData(v);        
          //std::cout << "used metadat:" << m_hostKlvFrmDefault->toString() << std::endl;
        }

        m_klvCnt++;
        return v.size();
    }

		ngv::HostYuvFrmPtr	m_hostYuvFrm{nullptr};	  //YUV420  frm 
		ngv::HostKlvFrmPtr	m_hostKlvFrm{nullptr};	  //the KLV for frm i w/ metadata
		ngv::HostKlvFrmPtr	m_hostKlvFrmDefault{nullptr};	  //the KLV for frames w/o metadata

    //
		int64_t				      m_timeDuration_nanoSec{0};
    GstClockTime        m_timestamp{0};

		int64_t				      m_timeDurationKlv_nanoSec{0};
    GstClockTime        m_timestampKlv{0};

    GMainLoop *loop{nullptr};
    uint64_t m_klvCnt{0};
    uint64_t m_imgCnt{0};
    FILE *m_fpKlv{nullptr};
    FILE *m_fpYuv{nullptr};
    bool m_hasKlv{false};
};


static void pushKlv(GstElement *src, guint, void *user_data);
static void pushImg(GstElement *src, guint size, void *user_data);

int test_gst_mux_enc_v0(const CfgApp &cfg0 )
{
  GstElement *pipeline;
  GstElement *videoSrc, *srcCapsFilter, *dataSrc, *encoder264, *mpegtsmux, *parser, *vidCapsFilter, *videoConvert, *videoScale, *timeoverlay, *avsink, *video_queue, *filesink;
  GstCaps *src_filter_caps, *videoSrc_caps,*dataSrc_caps, *video_filter_caps, *time_filter_caps;
  GstStateChangeReturn ret;
  GstMessage *msg;
  gboolean terminate = FALSE;

  const CfgEnc &cfg = *(cfg0.enc.get());
  UserData userData(cfg); 

  // copy first argument to fileName  - this is the file we will write to
  dumpLog("test_gst_mux_enc_v1(): the output video file will be at:%s", cfg.ouputFilePath.c_str() );
  remove( cfg.ouputFilePath.c_str());

  gst_init(NULL, NULL);

  userData.loop = g_main_loop_new(NULL, false);

#if USE_VIDEO_TEST_SRC
  videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
#else
  videoSrc =  gst_element_factory_make("appsrc", "videoSrc");
#endif

  srcCapsFilter = gst_element_factory_make("capsfilter", NULL);
  encoder264 = gst_element_factory_make( (const gchar *)cfg.gstEncName.c_str(), NULL);
  mpegtsmux = gst_element_factory_make("mpegtsmux", NULL);
  video_queue = gst_element_factory_make("queue", "video_queue");
  parser = gst_element_factory_make("h264parse", NULL);
  vidCapsFilter = gst_element_factory_make("capsfilter", NULL);
  videoConvert = gst_element_factory_make("videoconvert", NULL);
  videoScale = gst_element_factory_make("videoscale", NULL);
  timeoverlay = gst_element_factory_make("timeoverlay", NULL);
  dataSrc = gst_element_factory_make("appsrc", NULL);
  avsink = gst_element_factory_make("autovideosink", "sink");
  filesink = gst_element_factory_make("filesink", "filesink");

  g_printerr("AAA--------\n");

  pipeline = gst_pipeline_new("encode-pipeline");

  if (!pipeline || !videoSrc || !srcCapsFilter || !dataSrc || !mpegtsmux || !parser 
                || !vidCapsFilter || !videoConvert || !videoScale || !timeoverlay 
                || !avsink || !video_queue || !filesink)
  {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }



  dataSrc_caps = gst_caps_new_simple("meta/x-klv", 
                                     "parsed", G_TYPE_BOOLEAN, TRUE, 
                                     "spare", G_TYPE_BOOLEAN, TRUE,
                                     "is-live", G_TYPE_BOOLEAN, TRUE, 
                                     "framerate", GST_TYPE_FRACTION, cfg.tbd /*30*/, cfg.tbn /*1*/,
                                     NULL);

  g_object_set(G_OBJECT(dataSrc), "caps", dataSrc_caps, NULL);
  g_object_set(G_OBJECT(dataSrc), "format", GST_FORMAT_TIME, NULL);
  g_object_set(G_OBJECT(dataSrc), "do-timestamp", TRUE, NULL);
  g_object_set(G_OBJECT(dataSrc), "max-latency", 1000000000, NULL);

#if USE_VIDEO_TEST_SRC
  g_object_set(videoSrc, "pattern", 0, NULL);
  videoSrc_caps = gst_caps_new_simple("video/x-raw", 
                                    "width", G_TYPE_INT, cfg.imgW, 
                                    "height", G_TYPE_INT, cfg.imgH, 
                                    "framerate", GST_TYPE_FRACTION, cfg.tbd, cfg.tbn, NULL);
#else
  g_object_set(videoSrc, "format", GST_FORMAT_TIME, NULL);
  g_object_set(videoSrc, "do-timestamp", TRUE, NULL);
  videoSrc_caps = gst_caps_new_simple("video/x-raw",
                                    "format", G_TYPE_STRING, "I420",
                                    "width", G_TYPE_INT, cfg.imgW,
                                    "height", G_TYPE_INT, cfg.imgH,
                                    "framerate", GST_TYPE_FRACTION, cfg.tbd, cfg.tbn, NULL);
#endif
  g_object_set(srcCapsFilter, "caps", videoSrc_caps, NULL);
 
  video_filter_caps = gst_caps_from_string("video/x-h264, stream-format=(string)byte-stream");
  g_object_set(vidCapsFilter, "caps", video_filter_caps, NULL);

  g_object_set(G_OBJECT(encoder264), "bitrate", cfg.bitrate, NULL);
  g_object_set(G_OBJECT(filesink), "location", cfg.ouputFilePath.c_str(), NULL);
  g_printerr("BBB--------\n");

  // Assign appsrc callbacks to push image and metadata
#if USE_VIDEO_TEST_SRC
  g_signal_connect(dataSrc, "need-data", G_CALLBACK(pushKlv), &userData);
#else
  g_signal_connect(videoSrc, "need-data", G_CALLBACK(pushImg), &userData);
  g_signal_connect(dataSrc, "need-data", G_CALLBACK(pushKlv), &userData);
#endif

  gst_bin_add_many(GST_BIN(pipeline), videoSrc, srcCapsFilter, videoConvert, videoScale, timeoverlay, encoder264, 
      vidCapsFilter, parser, mpegtsmux, video_queue, dataSrc, filesink, NULL);

  if (!gst_element_link_many(videoSrc, srcCapsFilter, timeoverlay, encoder264, vidCapsFilter, video_queue, mpegtsmux, NULL) ||
      !gst_element_link_many(dataSrc, mpegtsmux, NULL) ||
      !gst_element_link_many(mpegtsmux, filesink, NULL))
  {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  std::chrono::steady_clock::time_point startTime =  std::chrono::steady_clock::now();

  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);

  //---------------------------------------------------
  //swu: todo: remove g_main_loop_run(), 'casue  "CCC------" will never be reached unless quit the loop
  //     either use g_main_loop_run()
  //     or  
  //    gst_element_get_bus()
  //    gst_bus_timed_pop_filtered()
  //    use both is wired!
  //---------------------------------------------------
  g_main_loop_run(userData.loop);

  g_printerr("CCC--------\n");

  /* Free resources */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  /* Clean up allocated resources */
  g_printerr("well done--FF, m_imgCnt=%lu, m_klvCnt=%lu\n", userData.m_imgCnt, userData.m_klvCnt);

  std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
  int64_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
  printf("diff=%lu(ms)\n", diff);

  return 0;
}

/* Callback function for encoding and injection of Klv metadata */
static void pushKlv(GstElement *appsrc, guint, gpointer user_data)
{
  GstFlowReturn ret;
  std::vector<uint8_t> v;

  UserData *pUserData = reinterpret_cast<UserData*>(user_data);
  uint16_t L = pUserData->readNextKlvFrm( v );
  GstBuffer *buffer = gst_buffer_new_allocate(NULL, L, NULL);
  gst_buffer_fill(buffer, 0, v.data(), L);
 
 //GST_BUFFER_PTS(buf)
 //Gets the presentation timestamp (pts) in nanoseconds (as a GstClockTime) of the data in the buffer. 
 //This is the timestamp when the media should be presented to the user. 

 //GST_BUFFER_DTS(buf)
 //Gets the decoding timestamp (dts) in nanoseconds (as a GstClockTime) of the data in the buffer. 
 //This is the timestamp when the media should be decoded or processed otherwise. 
#if APP_ASYNC_KLV
  // For ASYNC_KLV, we need to remove timestamp and duration from the buffer
  GST_BUFFER_PTS(buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DTS(buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DURATION(buffer) = GST_CLOCK_TIME_NONE;
#else
  // For SYNC_KLV, we need to add timestamp and duration from the buffer
  GST_BUFFER_PTS(buffer) = pUserData->m_timestampKlv; 
  GST_BUFFER_DTS(buffer) = pUserData->m_timestampKlv;
  GST_BUFFER_DURATION(buffer) = pUserData->m_timeDurationKlv_nanoSec; 
	pUserData->m_timestampKlv += GST_BUFFER_DURATION(buffer);
#endif

  ret = gst_app_src_push_buffer((GstAppSrc *)appsrc, buffer);
//	g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);

  if (ret != GST_FLOW_OK)
  {
		printf("pushKlv(): Flow error!  ret=%d, name=%s", (int)ret, gst_flow_get_name(ret));
    g_main_loop_quit(pUserData->loop);
  }

  if( 0==pUserData->m_klvCnt%200){
    g_print("Klv packet count: %lu.  klv buf size: %d \n", pUserData->m_klvCnt, L);
  }

#if USE_VIDEO_TEST_SRC
  if (pUserData->m_klvCnt>=500){
		gst_app_src_end_of_stream(GST_APP_SRC(appsrc));
    g_main_loop_quit(pUserData->loop);
  }
#endif

}

void pushImg(GstElement *appsrc, guint unused, gpointer user_data)
{
	//dumpLog("GstRtspStreamer::cb_need_data(): called!");
	GstFlowReturn ret;
  UserData *pUserData = reinterpret_cast<UserData*>(user_data);
  bool hasNewFrm = pUserData->readNextYuvFrm();
	if ( !hasNewFrm ) {
    g_main_loop_quit(pUserData->loop);
		return;
	}

  //get sample image from userData
	ngv::HostYuvFrmPtr &oriFrm = pUserData->m_hostYuvFrm;

  //allocate a new buf for current frm
	uint8_t* yuvImgBuf = (uint8_t*)malloc(oriFrm->sz_);
	if (yuvImgBuf == NULL) {
    printf("pushImg(): cannot alocated memory!\n");
		assert(false);
	}
  //wrapper <yuvImgBuf> into <curFrm> which type is <ngv::HostYuvFrm>
  ngv::HostYuvFrm curFrm( oriFrm->w_, oriFrm->h_, yuvImgBuf, oriFrm->sz_, pUserData->m_imgCnt);
  oriFrm->hdCopyTo( &curFrm );  
	//curFrm.wrtFrmNumOnImg();  //expensive, debug only, change the data inside <yuvImgBuf>

	GstBuffer *buffer = gst_buffer_new_wrapped((guchar *)yuvImgBuf, gsize(oriFrm->sz_));
  //if( 0 == curFrm.fn_%100 ){
  //  curFrm.dump( "./", "input");
  //}

	/* increment the timestamp every 1/MY_FPS second */
	GST_BUFFER_PTS(buffer) = pUserData->m_timestamp;
	GST_BUFFER_DTS(buffer) = pUserData->m_timestamp;
	GST_BUFFER_DURATION(buffer) = pUserData->m_timeDuration_nanoSec;
	pUserData->m_timestamp += GST_BUFFER_DURATION(buffer);

	g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
	if (ret != GST_FLOW_OK) {
		printf("pushImg(): sth is wrong!  ret=%d, name=%s", (int)ret, gst_flow_get_name(ret));
    g_main_loop_quit(pUserData->loop);
	}
  
  if( 0==pUserData->m_imgCnt%200){
    g_print("m_imgCnt=%lu\n", pUserData->m_imgCnt);
  }
  //wus1: do not free <yuvImgBuf> at here, it will be freeed inside gst via <buffer>
}
