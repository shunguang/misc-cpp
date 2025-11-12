/*
test_gst_klv_dec_v2.cpp
given a ts file, this program build the following pipeline
tsFileSrc->tsDemux->videoQueue->h264parse->videodec->videoConv->vidCapsFilter->yuvSink (dump individual frame in to a png file)

   Experimental version trying to attach metadata on GstBuffer where an image frame is located. 
     -Yes, we can attach metadata inside a GstBuffer and pass it between elements in side the pipeline
     -No, the mpegtsmux cannot write the meta into a file

    - Therefore thsi aproach does not work!
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
#include <gst/video/gstvideometa.h>

#include "libUtil/util.h"
#include "libUtil/GstMyAppMeta.h"

using namespace std;
using namespace ngv;

/* Structure to contain all information, so we can pass it to callbacks */
struct DemuxDecUserData2
{
  DemuxDecUserData2(const CfgDecPtr cfg)
      : m_hostYuvFrm(new ngv::HostYuvFrm(cfg->imgW, cfg->imgH, 0))
  {
    m_hostYuvFrm->setToZeros();
  }

  //---------local data -------------
  ngv::HostYuvFrmPtr m_hostYuvFrm{nullptr}; //YUV frm read(hd copy) from <m_cudaYuvQ4Rtsp>, it will be RTSP to client,
  uint64_t m_frmNum{0};

  //--------gst elements ------------
  GstElement *pipeline{NULL};
  GstElement *source{NULL};
  GstElement *tsDemux{NULL};
  GstElement *videoQueue{NULL};
  GstElement *h264parse{NULL};
  GstElement *avdec{NULL};
  GstElement *videoConv{NULL};
  GstElement *vidCapsFilter{NULL};
  GstElement *yuvSink{NULL};

//  GstElement *klvQueue{NULL};
//  GstElement *klvSink{NULL};
};

/* Handler for the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *pad, DemuxDecUserData2 *uData);
static GstFlowReturn new_yuv_sample_cb(GstElement *sink, DemuxDecUserData2 *uData);
static GstFlowReturn img_eos_cb(GstElement *sink, DemuxDecUserData2 *uData);

int test_gst_demux_dec_v2(const CfgApp &cfg0)
{
  const CfgDecPtr  cfg = cfg0.dec;
  cout << "cfg="  << cfg->toString() << endl;
  DemuxDecUserData2  uData(cfg);

  const GType myType = gst_my_app_meta_api_get_type();
  const GstMetaInfo *myInfo = gst_my_app_meta_get_info();
  printf( "myType=%d, myInfo(api=%d,type=%d,size=%d)", myType, myInfo->api,myInfo->type, myInfo->size);


  GstBus *bus=NULL;
  GstMessage *msg=NULL;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;
  
  /* Initialize GStreamer */
  gst_init(NULL, NULL);

  /* Create the elements */
  uData.source = gst_element_factory_make("filesrc", "source");
  uData.tsDemux = gst_element_factory_make("tsdemux", "demux");
  uData.videoQueue = gst_element_factory_make("queue", "videoQueue");
  uData.h264parse = gst_element_factory_make("h264parse", "h264parse");
  uData.avdec = gst_element_factory_make( (const gchar *)cfg->gstDecName.c_str(), "avdec");
  uData.videoConv = gst_element_factory_make("nvvidconv", "videoConv");
  uData.vidCapsFilter = gst_element_factory_make("capsfilter", NULL);
  uData.yuvSink   = gst_element_factory_make("appsink", "yuvSink");

//  uData.klvQueue = gst_element_factory_make("queue", "klvQueue");
//  uData.klvSink = gst_element_factory_make("appsink", "klvSink");

  /* Create the empty pipeline */
  uData.pipeline = gst_pipeline_new("decode-pipeline");

  //
  // ----- link elements ----------------
  //
  //|| !uData.klvQueue || !uData.klvSink
  if (!uData.pipeline || !uData.source || !uData.tsDemux || !uData.videoQueue || !uData.h264parse || !uData.avdec )
  {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

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
                   uData.videoQueue, uData.h264parse, uData.avdec, uData.videoConv, uData.vidCapsFilter, uData.yuvSink,
                   /*uData.klvQueue, uData.klvSink,*/ NULL);

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
  linkSuc = gst_element_link_many(uData.videoQueue, uData.h264parse, uData.avdec,
      uData.videoConv, uData.vidCapsFilter, uData.yuvSink,  NULL);

  if( !linkSuc ){
    g_printerr("Video processing elements could not be linked.\n");
    gst_object_unref(uData.pipeline);
    return -1;
  }

#if 0
  if (!gst_element_link(uData.klvQueue, uData.klvSink))
  {
    g_printerr("Data processing elements could not be linked.\n");
    gst_object_unref(uData.pipeline);
    return -1;
  }
#endif

  /* Set the URI to play */
  g_object_set(G_OBJECT(uData.source), "location", cfg->inputFilePath.c_str(), NULL);

  /* Configure appsink */
  //g_object_set(uData.klvSink, "emit-signals", TRUE, NULL);
  //g_signal_connect(uData.klvSink, "new-sample", G_CALLBACK(new_klv_sample_cb), &uData);
  //g_signal_connect(uData.klvSink, "eos", G_CALLBACK(klv_eos_cb), &uData);

  /* Connect to the pad-added signal */
  g_signal_connect(uData.tsDemux, "pad-added", G_CALLBACK(pad_added_handler), &uData);

  g_object_set(uData.yuvSink, "emit-signals", TRUE, NULL);
  g_signal_connect(uData.yuvSink, "new_sample", G_CALLBACK(new_yuv_sample_cb), &uData);
  g_signal_connect(uData.yuvSink, "eos", G_CALLBACK(img_eos_cb), &uData);

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
  
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  /* Free resources */
  gst_object_unref(bus);
  gst_element_set_state(uData.pipeline, GST_STATE_NULL);
  gst_object_unref(uData.pipeline);
  return 0;
}

/* This function will be called by the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *new_pad, DemuxDecUserData2 *uData)
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
  else if (g_str_has_prefix(new_pad_type, "meta/x-klv")){
    g_print("-------- should not be called! ----------");
    //sink_pad = gst_element_get_static_pad(uData->klvQueue, "sink");
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

static GstFlowReturn new_yuv_sample_cb(GstElement *sink, DemuxDecUserData2 *user_data)
{
    GstSample *sample = NULL;
    GstBuffer *buffer = NULL;
    GstCaps *caps = NULL;
    GstMapInfo map = {0};
    GstMyAppMeta *meta = NULL;
    GstVideoCropMeta *meta2=NULL;

    //printf("new_yuv_sample_cb()\n");

    //dumpLog("CapSaveRtspH264::new_sample_cb(): AAA.");
    //gateway to access this->xyz
    DemuxDecUserData2 *pThis = reinterpret_cast<DemuxDecUserData2 *>(user_data);

    //sync "pull-sample", wait utill the sampel is ready
    //async "try-pull-sample", do not wait
    g_signal_emit_by_name(sink, "pull-sample", &sample, NULL);
    if (!sample)
    {
        printf("CapSaveRtspH264::new_sample_cb(): could not make snapshot\n");        
        return GST_FLOW_ERROR;
    }

    caps = gst_sample_get_caps(sample);
    if (!caps)
    {
        printf("CapSaveRtspH264::new_sample_cb(): could not get snapshot format.\n");
    }
    gst_caps_get_structure(caps, 0);
    buffer = gst_sample_get_buffer(sample);

    //-------------------------------------
    //dec image and dump it
    //-------------------------------------
    gst_buffer_map(buffer, &map, GST_MAP_READ);
    if (pThis->m_hostYuvFrm->sz_ == map.size)
    {
        //local dump and debug
        pThis->m_hostYuvFrm->hdCopyFrom(map.data, map.size, pThis->m_frmNum);
        if (0 == pThis->m_frmNum % 100)
        {
            pThis->m_hostYuvFrm->dump(".", "yuv_h");
            printf("new_yuv_sample_cb(): mapSize=%d, fn=%d\n", map.size, pThis->m_frmNum);
        }
    }
    else
    {
        printf("CapSaveRtspH264::new_sample_cb(): size does not match, m_hostYuvFrm->sz_=%d, map.size=%d\n", pThis->m_hostYuvFrm->sz_, map.size);
    }

    //-------------------------------------
    //dec metadata and print it out
    //-------------------------------------
#if 1
    meta2 = gst_buffer_get_video_crop_meta(buffer);
    if (meta2)
    {
        printf("meta2:(x=%d, y=%d, w=%d, h=%d)\n", meta2->x, meta2->y, meta2->width, meta2->height);
    }
#else
    meta = gst_buffer_get_my_app_meta(buffer);
    if (meta)
    {
        printf("meta->sz=%d:", meta->bufSize);
        for (int i = 0; i < meta->bufSize; ++i)
        {
            printf("%d,", meta->buf[i]);
        }
        printf("\n");
    }
#endif
    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);

    pThis->m_frmNum++;

    return GST_FLOW_OK;
}

static GstFlowReturn img_eos_cb(GstElement *sink, DemuxDecUserData2 *user_data)
{
	DemuxDecUserData2 *pThis = reinterpret_cast<DemuxDecUserData2*>(user_data);
  printf("img_eos_cb()\n");
}

