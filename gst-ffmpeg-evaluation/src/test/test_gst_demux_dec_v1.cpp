/*
test_gst_klv_dec_v1.cpp
given a ts file, this program build the following pipeline
tsFileSrc->tsDemux|->videoQueue->vidParse->videodec->autovideosink  (visualizing video)
                  |                                    or
                  |                                 ->videoConv->vidCapsFilter->yuvSink (dump individual frame in to a png file)
                  |->klvQueue->klvSink  (show binary metadata on console)

*/                 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

#include <iostream>
#include <cassert>
#include <string>
#include <thread>


#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include "libUtil/util.h"

using namespace std;
using namespace ngv;

#define DISP_ONLY 0
/* Structure to contain all information, so we can pass it to callbacks */
struct UserData
{
  UserData(const CfgDecPtr cfg)
      : m_hostYuvFrm(new ngv::HostYuvFrm(cfg->imgW, cfg->imgH, 0))
  {
    m_hostYuvFrm->setToZeros();
    m_cpuProf.setThdCntToPrintOut(cfg->nFrmsToPeekCpuTimeProfile);
  }

  //---------local data -------------
  ngv::HostYuvFrmPtr m_hostYuvFrm{nullptr}; //YUV frm read(hd copy) from <m_cudaYuvQ4Rtsp>, it will be RTSP to client,
  uint64_t m_frmNum{0};

  //--------gst elements ------------
  GstElement *pipeline{NULL};
  GstElement *source{NULL};
  GstElement *tsDemux{NULL};
  GstElement *videoQueue{NULL};
  GstElement *vidParse{NULL};
  GstElement *avdec{NULL};
#if DISP_ONLY  
  GstElement *videoSink{NULL};
#else  
  GstElement *videoConv{NULL};
  GstElement *vidCapsFilter{NULL};
  GstElement *yuvSink{NULL};
#endif

  GstElement *klvQueue{NULL};
  GstElement *klvSink{NULL};

  AppTicToc      m_att;
  AppCpuTimeProf m_cpuProf{100};
};

/* Handler for the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *pad, UserData *uData);
static GstFlowReturn new_klv_sample_cb(GstElement *sink, UserData *uData);
static GstFlowReturn new_yuv_sample_cb(GstElement *sink, UserData *uData);
static GstFlowReturn img_eos_cb(GstElement *sink, UserData *uData);
static GstFlowReturn klv_eos_cb(GstElement *sink, UserData *uData);
static GstPadProbeReturn cb_dec_sink_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data);
static GstPadProbeReturn cb_dec_src_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data);

int test_gst_demux_dec_v1(const CfgApp &cfg0)
{

   AppTicToc ttSeq;  //monitor cpu profile for whole seq
   ttSeq.tic();

  const CfgDecPtr  cfg = cfg0.dec;
  cout << "cfg="  << cfg->toString() << endl;
  UserData  uData(cfg);

  GstBus *bus=NULL;
  GstMessage *msg=NULL;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;
  GstPad *pad;
  GstPadProbeType mask;

  
  /* Initialize GStreamer */
  gst_init(NULL, NULL);

  /* Create the elements */
  uData.source = gst_element_factory_make("filesrc", "source");
  uData.tsDemux = gst_element_factory_make("tsdemux", "demux");
  uData.videoQueue = gst_element_factory_make("queue", "videoQueue");
  if (264==cfg->gstVidParser){
    uData.vidParse = gst_element_factory_make("h264parse", "vidparse");
  }
  else if (265==cfg->gstVidParser){
    uData.vidParse = gst_element_factory_make("h265parse", "vidparse");
  }
  else{
    appExit("test_gst_demux_dec_v1(): wrong cfg->gstVidParser!");
  }


  uData.avdec = gst_element_factory_make( (const gchar *)cfg->gstDecName.c_str(), "avdec");
#if DISP_ONLY  
  uData.videoSink = gst_element_factory_make("autovideosink", "videoSink");
#else
  uData.videoConv = gst_element_factory_make("nvvidconv", "videoConv");
  uData.vidCapsFilter = gst_element_factory_make("capsfilter", NULL);
  uData.yuvSink   = gst_element_factory_make("appsink", "yuvSink");
#endif
  uData.klvQueue = gst_element_factory_make("queue", "klvQueue");
  uData.klvSink = gst_element_factory_make("appsink", "klvSink");

  /* Create the empty pipeline */
  uData.pipeline = gst_pipeline_new("decode-pipeline");

  //
  // ----- link elements ----------------
  //
  if (!uData.pipeline || !uData.source || !uData.tsDemux || !uData.videoQueue || !uData.vidParse || !uData.avdec || !uData.klvQueue || !uData.klvSink)
  {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

#if DISP_ONLY  
    if (!uData.videoSink)
    {
      g_printerr("videoSink could be created.\n");
      return -1;
    }
    gst_bin_add_many(GST_BIN(uData.pipeline), uData.source, uData.tsDemux, 
        uData.videoQueue, uData.vidParse, uData.avdec, uData.videoSink, 
        uData.klvQueue, uData.klvSink, NULL);
#else
    if (!uData.videoConv || !uData.vidCapsFilter || !uData.yuvSink)
    {
      g_printerr("videoConv or  vidCapsFilter or yuvSink could be created.\n");
      return -1;
    }

    //GstCaps* videoCaps = gst_caps_from_string("video/x-raw, format=I420, width=1920, height=1080");
    GstCaps *videoCaps = gst_caps_new_simple("video/x-raw",
                                             "format", G_TYPE_STRING, "I420",
                                             "width", G_TYPE_INT, cfg->imgW,
                                             "height", G_TYPE_INT, cfg->imgH,
                                             NULL);

    g_object_set(uData.vidCapsFilter, "caps", videoCaps, NULL);
    /* Build the pipeline. Note that we are NOT linking the source at this point. We will do it later. */
    gst_bin_add_many(GST_BIN(uData.pipeline), uData.source, uData.tsDemux, 
        uData.videoQueue, uData.vidParse, uData.avdec, uData.videoConv, uData.vidCapsFilter, uData.yuvSink,
        uData.klvQueue, uData.klvSink, NULL);
#endif        

  //
  // ----- link elements ----------------
  //
  if (!gst_element_link(uData.source, uData.tsDemux))
  {
    g_printerr("Cannot link source to demux.\n");
    gst_object_unref(uData.pipeline);
    return -1;
  }

  bool linkSuc=false;
#if DISP_ONLY  
      linkSuc = gst_element_link_many(uData.videoQueue, uData.vidParse, uData.avdec, uData.videoSink,NULL);
#else
      linkSuc = gst_element_link_many(uData.videoQueue, uData.vidParse, uData.avdec,
      uData.videoConv, uData.vidCapsFilter, uData.yuvSink,  NULL);
#endif

  if( !linkSuc ){
    g_printerr("Video processing elements could not be linked.\n");
    gst_object_unref(uData.pipeline);
    return -1;
  }

  if (!gst_element_link(uData.klvQueue, uData.klvSink))
  {
    g_printerr("Data processing elements could not be linked.\n");
    gst_object_unref(uData.pipeline);
    return -1;
  }

 //
 //------------- use probe to measure time used in decoder --------------
 //
 //    sink -- decoder --- src
 //sink pad
  pad = gst_element_get_static_pad (uData.avdec, "sink");
  mask = (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER|GST_PAD_PROBE_TYPE_BLOCK /* |GST_PAD_PROBE_TYPE_PUSH*/);
  //mask = GST_PAD_PROBE_TYPE_BLOCK;
  gst_pad_add_probe (pad, mask, (GstPadProbeCallback)cb_dec_sink_pad_probe, &uData, NULL);
  gst_object_unref (pad);

 //src pad
  //mask = (GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER|GST_PAD_PROBE_TYPE_BLOCK/*|GST_PAD_PROBE_TYPE_PULL*/);
  mask = GST_PAD_PROBE_TYPE_BLOCK;
  pad = gst_element_get_static_pad (uData.avdec, "src");
  gst_pad_add_probe (pad, mask, (GstPadProbeCallback)cb_dec_src_pad_probe, &uData, NULL);
  gst_object_unref (pad);


  /* Set the URI to play */
  g_object_set(G_OBJECT(uData.source), "location", cfg->inputFilePath.c_str(), NULL);

  /* Configure appsink */
  g_object_set(uData.klvSink, "emit-signals", TRUE, NULL);
  g_signal_connect(uData.klvSink, "new-sample", G_CALLBACK(new_klv_sample_cb), &uData);
  g_signal_connect(uData.klvSink, "eos", G_CALLBACK(klv_eos_cb), &uData);

  /* Connect to the pad-added signal */
  g_signal_connect(uData.tsDemux, "pad-added", G_CALLBACK(pad_added_handler), &uData);

#if !DISP_ONLY  
    g_object_set(uData.yuvSink, "emit-signals", TRUE, NULL);
    g_signal_connect(uData.yuvSink, "new_sample", G_CALLBACK(new_yuv_sample_cb), &uData);
    g_signal_connect(uData.yuvSink, "eos", G_CALLBACK(img_eos_cb), &uData);
#endif

  /* Start playing */
  ret = gst_element_set_state(uData.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE)
  {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    gst_object_unref(uData.pipeline);
    return -1;
  }

  /* Listen to the bus */
  bus = gst_element_get_bus(uData.pipeline);
  do
  {
    msg = gst_bus_timed_pop_filtered( bus, GST_CLOCK_TIME_NONE, 
    (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS) );

    /* Parse message */
    if (msg != NULL)
    {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE(msg))
      {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
        g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);
        terminate = TRUE;
        break;
      case GST_MESSAGE_EOS:
        g_print("test_gst_demux_dec_v1(): End-Of-Stream reached.\n");
        terminate = TRUE;
        break;
      case GST_MESSAGE_STATE_CHANGED:
        /* We are only interested in state-changed messages from the pipeline */
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(uData.pipeline))
        {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
          g_print("Pipeline state changed from %s to %s:\n",
                  gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
        }
        break;
      default:
        /* We should not reach here */
        g_printerr("Unexpected message received.\n");
        break;
      } //switch()

      gst_message_unref(msg);
    }
  } while (!terminate);

  //std::this_thread::sleep_for(std::chrono::milliseconds(100));

  /* Free resources */
  gst_object_unref(bus);
  gst_element_set_state(uData.pipeline, GST_STATE_NULL);
  gst_object_unref(uData.pipeline);

  ttSeq.toc();   

  std::string outMsg;
  size_t nTotFrms = uData.m_cpuProf.calMeanStd(outMsg, "PerFrameStatistics");
  printf("decName=%s, %s\n", cfg->gstDecName.c_str(), outMsg.c_str());
  printf("\t\t%s\n", ttSeq.toString("ms", "Total Time Used for decoding the whole seq:").c_str());

  return 0;
}

/* The appsink has received a buffer */
static GstFlowReturn new_klv_sample_cb(GstElement *sink, UserData *user_data)
{
  GstSample *sample;

  //g_print("new_klv_sample_cb--AA\n");
  /* Retrieve the buffer, will wait untill a sample is ready */
  g_signal_emit_by_name(sink, "pull-sample", &sample);
  if (sample)
  {
    GstBuffer *gstBuffer = gst_sample_get_buffer(sample);

    if (gstBuffer)
    {
      auto pts = GST_BUFFER_PTS(gstBuffer);
      auto dts = GST_BUFFER_DTS(gstBuffer);

      //gsize bufSize = gst_buffer_get_size(gstBuffer);
      //g_print("Klv buffer size %ld. PTS %ld   DTS %ld\n", bufSize, pts, dts);

      GstMapInfo map;
      gst_buffer_map(gstBuffer, &map, GST_MAP_READ);

#if 0
      const char *p = (char *)map.data;
      for(int i=0; i<map.size; ++i){
          printf("%c", p[i]);
      }
      printf("\n");

#else
      HostKlvFrm curFrm;
      curFrm.readFromBuf( map.data, map.size);
      //printf( "%s\n", curFrm.toString().c_str() );
#endif
      //char *jsonPckt = decode601Pckt((char *)map.data, map.size);
      //g_print("%s \n", jsonPckt);

      gst_sample_unref(sample);
      return GST_FLOW_OK;
    }
  }

  return GST_FLOW_ERROR;
}

/* This function will be called by the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *new_pad, UserData *uData)
{
  GstElement *sink;
  GstPad *sink_pad = NULL;
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;

  /* Check the new pad's type */
  new_pad_caps = gst_pad_get_current_caps(new_pad);
  new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
  new_pad_type = gst_structure_get_name(new_pad_struct);

  g_print("Received new pad '%s' from '%s' of type '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src), new_pad_type);

  if (g_str_has_prefix(new_pad_type, "video/x-h264")){
    sink_pad = gst_element_get_static_pad(uData->videoQueue, "sink");
  }
  else if (g_str_has_prefix(new_pad_type, "video/x-h265")){
    sink_pad = gst_element_get_static_pad(uData->videoQueue, "sink");
  }
  else if (g_str_has_prefix(new_pad_type, "meta/x-klv")){
    sink_pad = gst_element_get_static_pad(uData->klvQueue, "sink");
  }
  else
  {
    sink = gst_element_factory_make("fakesink", NULL);
    gst_bin_add(GST_BIN(uData->pipeline), sink);
    sink_pad = gst_element_get_static_pad(sink, "sink");
    gst_element_sync_state_with_parent(sink);    
  }

  if (gst_pad_is_linked(sink_pad) || sink_pad == NULL)
  {
    g_print("We are already linked. Ignoring.\n");
    goto exit;
  }

  /* Attempt the link */
  ret = gst_pad_link(new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED(ret))
    g_print("Type is '%s' but link failed.\n", new_pad_type);
  else
    g_print("Link succeeded (type '%s').\n", new_pad_type);

exit:
  /* Unreference the new pad's caps, if we got them */
  if (new_pad_caps != NULL)
    gst_caps_unref(new_pad_caps);

  /* Unreference the sink pad */
  if (sink_pad != NULL)
    gst_object_unref(sink_pad);


  g_print("pad_added_handler(): well done!\n");
}

static GstFlowReturn new_yuv_sample_cb(GstElement *sink, UserData *user_data)
{
	//dumpLog("new_yuv_sample_cb(): AAA.");
	//gateway to access this->xyz
	UserData *pThis = reinterpret_cast<UserData*>(user_data);

	GstSample *sample = NULL;

  //sync "pull-sample", wait utill the sampel is ready
  //async "try-pull-sample", do not wait 
	g_signal_emit_by_name(sink, "pull-sample", &sample, NULL);
	if (sample){
		GstBuffer *buffer = NULL;
		GstCaps   *caps = NULL;
		GstMapInfo map = { 0 };

		caps = gst_sample_get_caps(sample);
		if (!caps){
			printf("new_yuv_sample_cb(): could not get snapshot format.\n");
		}
		gst_caps_get_structure(caps, 0);
		buffer = gst_sample_get_buffer(sample);
		gst_buffer_map(buffer, &map, GST_MAP_READ);

		if( pThis->m_hostYuvFrm->sz_ == map.size ){
		  //local dump and debug 
		  pThis->m_hostYuvFrm->hdCopyFrom(map.data, map.size, pThis->m_frmNum);
      //if ( 0 == pThis->m_frmNum%500 ){
		  //  pThis->m_hostYuvFrm->dump(".", "yuv_h");
      //}
		  //printf( "new_yuv_sample_cb(): mapSize=%d, fn=%d\n", map.size, pThis->m_frmNum);
    }
    else{
      printf("new_yuv_sample_cb(): size does not match, m_hostYuvFrm->sz_=%d, map.size=%d\n", pThis->m_hostYuvFrm->sz_, map.size);
    }

		gst_buffer_unmap(buffer, &map);
		gst_sample_unref(sample);

		pThis->m_frmNum++;
	}
	else{
		printf("new_yuv_sample_cb(): could not make snapshot\n");
	}

	return GST_FLOW_OK; 
}

static GstFlowReturn img_eos_cb(GstElement *sink, UserData *user_data)
{
	UserData *pThis = reinterpret_cast<UserData*>(user_data);
  printf("img_eos_cb()\n");
}

static GstFlowReturn klv_eos_cb(GstElement *sink, UserData *user_data)
{
	UserData *pThis = reinterpret_cast<UserData*>(user_data);
  printf("klv_eos_cb()\n");
}


static GstPadProbeReturn cb_dec_sink_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data)
{
    UserData *pUserData = reinterpret_cast<UserData*>(user_data);
    if( pUserData->m_frmNum>50){
      pUserData->m_att.tic();
    }
    return GST_PAD_PROBE_PASS;
}

static GstPadProbeReturn cb_dec_src_pad_probe (GstPad *pad, GstPadProbeInfo *info, void *user_data)
{
    UserData *pUserData = reinterpret_cast<UserData*>(user_data);
    AppTicToc      &att = pUserData->m_att;
    if ( att.wall_time_t1_us > 0 ){
      att.toc();     
      pUserData->m_cpuProf.addSamples( att.dt_wall_us, att.dt_usr_cpu_us, att.dt_sys_cpu_us);
    }
    return GST_PAD_PROBE_PASS;
}
