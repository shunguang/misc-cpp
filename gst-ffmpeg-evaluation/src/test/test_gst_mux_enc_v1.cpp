//modified from https://github.com/impleotv/gstreamer-test-klv

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

//-------------------------------------------------------------------
// test_gst_klv_enc_min_v1.cpp - it works!
// an example to enc metadata and image by two callback functions
//-------------------------------------------------------------------

#include <cuda_runtime_api.h>
#include "libUtil/MuxEncUserData.h"

using namespace ngv;
static void pushKlv(GstElement *src, guint, void *user_data);
static void pushImg(GstElement *src, guint size, void *user_data);
static bool createElements(MuxEncUserData *pp);
static bool createBinAndLink(MuxEncUserData *pp);
static GstPadProbeReturn cb_enc_sink_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data);
static GstPadProbeReturn cb_enc_src_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data);

#define USE_VID_CAPS_FILTER 0

int test_gst_mux_enc_v1(const CfgApp &cfg0 )
{
   AppTicToc ttSeq;  //monitor cpu profile for whole seq
   ttSeq.tic();

  GstStateChangeReturn ret;
  MuxEncUserData uData(cfg0.enc); 
  
  if(cfg0.enc->gstEncNameId == NVV4L2H264ENC ){
    int current_device = -1;
    cudaGetDevice(&current_device);
    struct cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, current_device);
  }

  gst_init(NULL, NULL);

  if( !createElements( &uData )){
    return -1;
  }
   // Assign appsrc callbacks to push image and metadata
#if USE_VIDEO_TEST_SRC
  g_signal_connect(uData.klvSrc, "need-data", G_CALLBACK(pushKlv), &uData);
#else
  g_signal_connect(uData.vidSrc, "need-data", G_CALLBACK(pushImg), &uData);
  g_signal_connect(uData.klvSrc, "need-data", G_CALLBACK(pushKlv), &uData);
#endif


  if( !createBinAndLink( &uData ) ){
    return -1;
  }

  ret = gst_element_set_state(uData.pipeline, GST_STATE_PLAYING);
  g_main_loop_run(uData.loop);

  /* Free resources */
  gst_element_set_state(uData.pipeline, GST_STATE_NULL);
  gst_object_unref(uData.pipeline);

  /* Clean up allocated resources */
  g_printerr("well done--FF, m_imgCnt=%lu, m_klvCnt=%lu\n", uData.m_imgCnt, uData.m_klvCnt);

  ttSeq.toc();   
  std::string outMsg;
  size_t nTotFrms = uData.m_cpuProf.calMeanStd(outMsg, "PerFrameStatistics");
  printf("encName=%s, %s\n", uData.m_cfg->getGstEncName(), outMsg.c_str());
  printf("\t\t%s\n", ttSeq.toString("ms", "Total Time Used for decoding the whole seq:").c_str());

  return 0;
}

/* Callback function for encoding and injection of Klv metadata */
static void pushKlv(GstElement *appsrc, guint, gpointer user_data)
{
  GstFlowReturn ret;
  std::vector<uint8_t> v;

  MuxEncUserData *pUserData = reinterpret_cast<MuxEncUserData*>(user_data);
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
  MuxEncUserData *pUserData = reinterpret_cast<MuxEncUserData*>(user_data);
  bool hasNewFrm = pUserData->readNextYuvFrm();
	if ( !hasNewFrm ) {
		dumpLog("pushImg(): no more image frames call g_main_loopquit()");
    g_main_loop_quit(pUserData->loop);
		return;
	}

  //get sample image from uData
	ngv::HostYuvFrmPtr &oriFrm = pUserData->m_hostYuvFrm;
#if 1  
    GstBuffer *buffer = gst_buffer_new_allocate(NULL, oriFrm->sz_, NULL);
    gst_buffer_fill(buffer, 0, oriFrm->buf_, oriFrm->sz_);
    //oriFrm.wrtFrmNumOnImg();  //expensive, debug only, change the data inside <yuvImgBuf>
#else
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
#endif

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

  //we need this unref(), otherwise has memo leak problem
  gst_buffer_unref (buffer);

  //12/13/2022 wus1: do we need to free resource?
  //--------------------------------------------------------------------------------------------------
  //https://stackoverflow.com/questions/36301355/memory-leaks-while-using-gstbuffer
  //If you use gst_app_src_push_buffer function I guess you do not have to free resources 
  //because gst_app_src_push_buffer will own the buffer (which means it also frees it) Check this example:
  // https://cgit.freedesktop.org/gstreamer/gst-plugins-base/tree/tests/examples/app/appsrc_ex.c
  //If you use need-data callback you may need to free data - check this example:
  //https://cgit.freedesktop.org/gstreamer/gst-plugins-base/tree/tests/examples/app/appsrc-stream.c
  //--------------------------------------------------------------------------------------------------
  //dumpLog("pushImg(): ---C!");

}


bool createElements( MuxEncUserData *pp)
{
  const CfgEnc *cfg= pp->m_cfg.get();
  const gchar *encName = (const gchar *)cfg->getGstEncName();
  dumpLog("createElements(): encName=%s", (char*)encName);

  pp->loop = g_main_loop_new(NULL, false);
#if USE_VIDEO_TEST_SRC
  pp->vidSrc = gst_element_factory_make("videotestsrc", "vidSrc");
#else
  pp->vidSrc = gst_element_factory_make("appsrc", "vidSrc");
#endif
  pp->srcCapsFilter = gst_element_factory_make("capsfilter", NULL);
  pp->timeoverlay = gst_element_factory_make("timeoverlay", NULL);
  pp->vidEncoder = gst_element_factory_make(encName, NULL);
  
  if (cfg->useVidCapFilterAfterEncoder){
    pp->vidCapsFilter = gst_element_factory_make("capsfilter", NULL);
    if (!pp->vidCapsFilter)
    {
      dumpLog("cannot create vidCapsFilter!");
      return false;
    }
  }
  
  pp->muxQueue = gst_element_factory_make("queue", "muxQueue");
  pp->mpegtsmux = gst_element_factory_make("mpegtsmux", NULL);
  pp->klvSrc = gst_element_factory_make("appsrc", NULL);
  pp->fileSink = gst_element_factory_make("filesink", "fileSink");


  if (cfg->gstEncNameId == NVV4L2H264ENC ||  cfg->gstEncNameId == NVV4L2H265ENC)
  {
    pp->vidConvert = gst_element_factory_make("nvvidconv", NULL);
    if (!pp->vidConvert)
    {
      dumpLog("cannot create vidConvert!");
      return false;
    }
  }
  //vidScale = gst_element_factory_make("videoscale", NULL);
  //vidParser = gst_element_factory_make("h264parse", NULL);

  pp->pipeline = gst_pipeline_new("encode-pipeline");

  if (!pp->pipeline || !pp->vidSrc || !pp->srcCapsFilter || !pp->klvSrc || !pp->mpegtsmux || !pp->timeoverlay || !pp->muxQueue || !pp->fileSink)
  {
    dumpLog("pipeline=%0x,vidSrc=%0x,srcCapsFilter=%0x,klvSrc=%0x,mpegtsmux=%0x, timeoverlay=%0x,muxQueue=%0x,fileSink=%0x\n", 
    pp->pipeline,pp->vidSrc,pp->srcCapsFilter,pp->klvSrc,pp->mpegtsmux,pp->timeoverlay,pp->muxQueue,pp->fileSink);

    dumpLog("Not all elements could be created!");
    return false;
  }

//-----------------------------------
  //pp->vidSrc: set params
  //-----------------------------------
#if USE_VIDEO_TEST_SRC
  g_object_set(pp->vidSrc, "pattern", 0, NULL);
#else
  g_object_set(pp->vidSrc, "format", GST_FORMAT_TIME, NULL);
  g_object_set(pp->vidSrc, "do-timestamp", TRUE, NULL);
#endif

  //-----------------------------------
  //pp->srcCapsFilter: set caps
  //-----------------------------------
  GstCaps *vidSrc_caps = nullptr;
#if USE_VIDEO_TEST_SRC
  vidSrc_caps = gst_caps_new_simple("video/x-raw",
                                    "width", G_TYPE_INT, cfg->imgW,
                                    "height", G_TYPE_INT, cfg->imgH,
                                    "framerate", GST_TYPE_FRACTION, cfg->tbd, cfg->tbn, NULL);
#else
  vidSrc_caps = gst_caps_new_simple("video/x-raw",
                                    "format", G_TYPE_STRING, "I420",
                                    "width", G_TYPE_INT, cfg->imgW,
                                    "height", G_TYPE_INT, cfg->imgH,
                                    "framerate", GST_TYPE_FRACTION, cfg->tbd, cfg->tbn, NULL);
#endif
  g_object_set(pp->srcCapsFilter, "caps", vidSrc_caps, NULL);
  gst_object_unref(vidSrc_caps);


  //-----------------------------------
  //pp->klvSrc: set caps and params
  //-----------------------------------
  GstCaps *klvSrc_caps = gst_caps_new_simple("meta/x-klv",
                                    "parsed", G_TYPE_BOOLEAN, TRUE,
                                    "spare", G_TYPE_BOOLEAN, TRUE,
                                    "is-live", G_TYPE_BOOLEAN, TRUE,
                                    "framerate", GST_TYPE_FRACTION, cfg->tbd /*30*/, cfg->tbn /*1*/,
                                    NULL);

  g_object_set(G_OBJECT(pp->klvSrc), "caps", klvSrc_caps, NULL);
  g_object_set(G_OBJECT(pp->klvSrc), "format", GST_FORMAT_TIME, NULL);
  g_object_set(G_OBJECT(pp->klvSrc), "do-timestamp", TRUE, NULL);
  g_object_set(G_OBJECT(pp->klvSrc), "max-latency", 1000000000, NULL);
  gst_object_unref(klvSrc_caps);


  
  //-----------------------------------
  //pp->vidCapsFilter: set caps
  //-----------------------------------
  if (cfg->useVidCapFilterAfterEncoder){
    GstCaps *vidFilter_caps = NULL;
    if (cfg->gstEncNameId < X265ENC)
    {
      vidFilter_caps = gst_caps_from_string("video/x-h264, stream-format=(string)byte-stream");
    }
    else
    {
      vidFilter_caps = gst_caps_from_string("video/x-h265, stream-format=(string)byte-stream");
    }
    g_object_set(pp->vidCapsFilter, "caps", vidFilter_caps, NULL);
    gst_object_unref(vidFilter_caps);
  }

  //
  // Bitrate unit is bit/sec (range : 1 - 2048000) for X264ENC and X265ENC all the others are bit/sec
  //
  uint32_t bitrate = cfg->bitrate;
  if(cfg->gstEncNameId == X264ENC  ||  cfg->gstEncNameId == X265ENC){
    bitrate /=1000;
  }
  g_object_set(G_OBJECT(pp->vidEncoder), "bitrate", bitrate, NULL);  

  //prepare output video path
  std::string fpath = cfg->ouputFilePath + "/gst-" + std::string(g_gstEncName[(int)cfg->gstEncNameId]) + ".ts";
  remove( fpath.c_str());

  g_object_set(G_OBJECT(pp->fileSink), "location", fpath.c_str(), NULL);
  dumpLog("test_gst_mux_enc_v1::createElements(): the output video file will be at:%s", fpath.c_str() );
  g_printerr("createElements(): done--------\n");

  return true;
}

bool createBinAndLink(MuxEncUserData *pp)
{
  /*
*top level build piplline:
*     appsrc->srcCapsFilter->timeoverla->videoEncoder->vidCapsFilter->--
*(inject YuvImg)                                                        |->video_queue->mpegtsmux->filesink
*                                                              appSrc->--             
*                                                      (inject klv metadata)                                                
*/
  GstPad *pad;
  GstPadProbeType mask;
  const CfgEnc *cfg = pp->m_cfg.get();
  if (cfg->gstEncNameId == NVV4L2H264ENC || cfg->gstEncNameId == NVV4L2H265ENC)
  {
    //we need vidConvert before nvv4l2h264enc/nvv4l2h264enc, 'cause its cap is  "video/x-raw(memory:NVMM)"
    if (cfg->useVidCapFilterAfterEncoder)
    {
      gst_bin_add_many(GST_BIN(pp->pipeline), pp->vidSrc, pp->srcCapsFilter, pp->vidConvert, pp->timeoverlay, pp->vidEncoder,
                       pp->vidCapsFilter, pp->mpegtsmux, pp->muxQueue, pp->klvSrc, pp->fileSink, NULL);

      if (!gst_element_link_many(pp->vidSrc, pp->srcCapsFilter, pp->timeoverlay, pp->vidConvert, pp->vidEncoder, pp->vidCapsFilter, pp->muxQueue, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->klvSrc, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->mpegtsmux, pp->fileSink, NULL))
      {
        g_printerr("Elements could not be linked.\n");
        return false;
      }
    }
    else
    {
      gst_bin_add_many(GST_BIN(pp->pipeline), pp->vidSrc, pp->srcCapsFilter, pp->vidConvert, pp->timeoverlay, pp->vidEncoder,
                       /*pp->vidCapsFilter,*/ pp->mpegtsmux, pp->muxQueue, pp->klvSrc, pp->fileSink, NULL);

      if (!gst_element_link_many(pp->vidSrc, pp->srcCapsFilter, pp->timeoverlay, pp->vidConvert, pp->vidEncoder, /*pp->vidCapsFilter,*/ pp->muxQueue, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->klvSrc, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->mpegtsmux, pp->fileSink, NULL))
      {
        g_printerr("Elements could not be linked.\n");
        return false;
      }
    }
  }
  else
  {
    if (cfg->useVidCapFilterAfterEncoder)
    {
      gst_bin_add_many(GST_BIN(pp->pipeline), pp->vidSrc, pp->srcCapsFilter, pp->timeoverlay, pp->vidEncoder,
                       pp->vidCapsFilter, pp->mpegtsmux, pp->muxQueue, pp->klvSrc, pp->fileSink, NULL);

      if (!gst_element_link_many(pp->vidSrc, pp->srcCapsFilter, pp->timeoverlay, pp->vidEncoder, pp->vidCapsFilter, pp->muxQueue, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->klvSrc, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->mpegtsmux, pp->fileSink, NULL))
      {
        g_printerr("Elements could not be linked.\n");
        return false;
      }
    }
    else
    {
      gst_bin_add_many(GST_BIN(pp->pipeline), pp->vidSrc, pp->srcCapsFilter, pp->timeoverlay, pp->vidEncoder,
                       /*pp->vidCapsFilter,*/ pp->mpegtsmux, pp->muxQueue, pp->klvSrc, pp->fileSink, NULL);

      if (!gst_element_link_many(pp->vidSrc, pp->srcCapsFilter, pp->timeoverlay, pp->vidEncoder, /*pp->vidCapsFilter,*/ pp->muxQueue, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->klvSrc, pp->mpegtsmux, NULL) ||
          !gst_element_link_many(pp->mpegtsmux, pp->fileSink, NULL))
      {
        g_printerr("Elements could not be linked.\n");
        return false;
      }
    }
  }

#if 1
 //use probe to measure time used in encoder
 //    sink -- encoder --- srx
 //sink pad
  pad = gst_element_get_static_pad (pp->vidEncoder, "sink");
  mask = (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER|GST_PAD_PROBE_TYPE_BLOCK /* |GST_PAD_PROBE_TYPE_PUSH*/);
  //mask = GST_PAD_PROBE_TYPE_BLOCK;
  gst_pad_add_probe (pad, mask, (GstPadProbeCallback)cb_enc_sink_pad_probe, pp, NULL);
  gst_object_unref (pad);

 //src pad
  //mask = (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER|GST_PAD_PROBE_TYPE_BLOCK/*|GST_PAD_PROBE_TYPE_PULL*/);
  mask = GST_PAD_PROBE_TYPE_BLOCK;
  pad = gst_element_get_static_pad (pp->vidEncoder, "src");
  gst_pad_add_probe (pad, mask, (GstPadProbeCallback)cb_enc_src_pad_probe, pp, NULL);
  gst_object_unref (pad);
#endif

  return true;
}


  //    sink -- encoder --- srx
static GstPadProbeReturn cb_enc_sink_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data)
{
    MuxEncUserData *pUserData = reinterpret_cast<MuxEncUserData*>(user_data);
    //since transition time is instable, we ignore it
    if( pUserData->m_imgCnt>50){
      pUserData->m_att.tic();
    }
    return GST_PAD_PROBE_PASS;
}


static GstPadProbeReturn cb_enc_src_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data)
{   
    MuxEncUserData *pUserData = reinterpret_cast<MuxEncUserData*>(user_data);
    AppTicToc      &att = pUserData->m_att;
    if ( att.wall_time_t1_us > 0 ){
      att.toc();
      pUserData->m_cpuProf.addSamples(att.dt_wall_us, att.dt_usr_cpu_us, att.dt_sys_cpu_us);
    }
    return GST_PAD_PROBE_PASS;
}
