/*
   Experimental version trying to attach metadata on GstBuffer where an image frame is located. 
     -Yes, we can attach metadata inside a GstBuffer and pass it between elements in side the pipeline
     -No, the mpegtsmux cannot write the meta into a file

    - Therefore thsi aproach does not work!
*/  

/*
testvideoSrc->srcCapsFilter->timeoverla->encoderH264->vidCapsFilter->video_queue->mpegtsmux->filesink
*/
//-------------------------------------------------------------------
// test_gst_klv_enc_min_v1.cpp
// an example to enc metadata wand image by two callback functions
//-------------------------------------------------------------------
#include <gst/video/gstvideometa.h>
#include "libUtil/GstMyAppMeta.h"
#include "libUtil/MuxEncUserData2.h"

using namespace ngv;
//static void pushKlv(GstElement *src, guint, void *user_data);
static void pushImg(GstElement *src, guint size, void *user_data);

int test_gst_mux_enc_v2(const CfgApp &cfg0)
{
    GstStateChangeReturn ret;
    MuxEncUserData2 uData(cfg0.enc);

    // copy first argument to fileName  - this is the file we will write to
    dumpLog("test_gst_mux_enc_v2(): the output video file will be at:%s", cfg0.enc->ouputFilePath.c_str());
    remove(cfg0.enc->ouputFilePath.c_str());

    gst_init(NULL, NULL);

    if (!uData.createElements())
    {
        return -1;
    }
    dumpLog("createElements() done!");

    // Assign appsrc callbacks to push image and metadata
    g_signal_connect(uData.videoSrc, "need-data", G_CALLBACK(pushImg), &uData);
    if (!uData.createBinAndLink())
    {
        return -1;
    }
    dumpLog("createBinAndLink() done!");
    
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    ret = gst_element_set_state(uData.pipeline, GST_STATE_PLAYING);
    g_main_loop_run(uData.loop);

    g_printerr("CCC--------\n");

    /* Free resources */
    gst_element_set_state(uData.pipeline, GST_STATE_NULL);
    gst_object_unref(uData.pipeline);

    /* Clean up allocated resources */
    g_printerr("well done--FF, m_imgCnt=%lu, m_klvCnt=%lu\n", uData.m_imgCnt, uData.m_klvCnt);

    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    int64_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    printf("diff=%lu(ms)\n", diff);

    return 0;
}

void pushImg(GstElement *appsrc, guint unused, gpointer user_data)
{
//    dumpLog("pushImg(): ---A!");
    GstMyAppMeta *meta;
    GstVideoCropMeta *meta2;

    GstFlowReturn ret;
    MuxEncUserData2 *pUserData = reinterpret_cast<MuxEncUserData2 *>(user_data);
    bool hasNewFrm = pUserData->readNextYuvFrm();
    if (!hasNewFrm)
    {
        g_main_loop_quit(pUserData->loop);
        return;
    }

    //get sample image from uData
    ngv::HostYuvFrmPtr &curFrm = pUserData->m_hostYuvFrm;
    //curFrm.wrtFrmNumOnImg();  //expensive, debug only, change the data inside <yuvImgBuf>
    GstBuffer *buffer = gst_buffer_new_allocate(NULL, curFrm->sz_, NULL);
    gst_buffer_fill(buffer, 0, curFrm->buf_, curFrm->sz_);

#if 1
    meta2 =  gst_buffer_add_video_crop_meta(buffer);
    meta2->x = 8;
    meta2->y = 8;
    meta2->width = 120;
    meta2->height = 80;
#else
    const gint sz = 32;
    gchar *klv = (gchar *)g_malloc(sz);
    memset( (void*) klv, 0, sz);
    meta =  gst_buffer_add_my_app_meta(buffer, sz,  klv);
    meta->bufSize=sz;
    std::string s = std::to_string(curFrm->fn_);
    //std::cout <<s<< std::endl;
    for(int i=0; i<s.size(); i++){
        meta->buf[i]=s[i]; 
    }
    g_free( klv );
#endif


    /* increment the timestamp every 1/MY_FPS second */
    GST_BUFFER_PTS(buffer) = pUserData->m_timestamp;
    GST_BUFFER_DTS(buffer) = pUserData->m_timestamp;
    GST_BUFFER_DURATION(buffer) = pUserData->m_timeDuration_nanoSec;
    pUserData->m_timestamp += GST_BUFFER_DURATION(buffer);

    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
    if (ret != GST_FLOW_OK)
    {
        printf("pushImg(): sth is wrong!  ret=%d, name=%s", (int)ret, gst_flow_get_name(ret));
        g_main_loop_quit(pUserData->loop);
    }

    if (0 == pUserData->m_imgCnt % 100)
    {
        g_print("m_imgCnt=%lu\n", pUserData->m_imgCnt);
    }

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

    //debug quit
    if( curFrm->fn_ > 500){
        g_main_loop_quit(pUserData->loop);
    }
}
