#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include "libUtil/util.h"
//#include "libUtil/CvUtilFuncs.h"

using namespace ngv;
#define H264_FPS (25)
#define H264_W (1920)
#define H264_H (1080)
typedef struct
{
        GstPipeline *pipeline;
        GstAppSrc *src;
        GstElement *filter1;
        GstElement *encoder;
        GstElement *filter2;
        GstElement *parser;
        GstElement *qtmux;
        GstElement *sink;

        GstClockTime timestamp;
        guint sourceid;
} gst_app_t;
static gst_app_t gst_app;

void updateYuvImg(uint8_t *buf, const uint32_t sz, const int fn, const bool isDump)
{
        cv::Mat Yuv;
        cv::Mat I(H264_H, H264_W, CV_8UC3);
        cv::randu(I, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
        cv::putText(I, "fn=" + std::to_string(fn), cv::Point(10, 200), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 4, cv::Scalar(255, 255, 255), 2);
        cv::cvtColor(I, Yuv, CV_BGR2YUV_I420);
        memcpy(buf, Yuv.data, sz);

        if (isDump)
        {
                cv::Mat Iout;
                ngv::HostYuvFrm x(H264_W, H264_H, buf, sz, fn);
                x.hdCopyToBgr(Iout);
                x.dump("./", "tmp");
        }
}

int test_gst_enc_H264(const ngv::CfgApp &cfg0)
{
        gst_app_t *app = &gst_app;
        GstStateChangeReturn state_ret;
        gst_init(NULL, NULL); //Initialize Gstreamer
        app->timestamp = 0;   //Set timestamp to 0

        //Create pipeline, and pipeline elements
        app->pipeline = (GstPipeline *)gst_pipeline_new("mypipeline");
        app->src = (GstAppSrc *)gst_element_factory_make("appsrc", "mysrc");
        app->filter1 = gst_element_factory_make("capsfilter", "myfilter1");
        //app->encoder =  gst_element_factory_make ("omxh264enc", "myomx");
        app->encoder = gst_element_factory_make("x264enc", "myomx");
        app->filter2 = gst_element_factory_make("capsfilter", "myfilter2");
        app->parser = gst_element_factory_make("h264parse", "myparser");
        app->qtmux = gst_element_factory_make("qtmux", "mymux");
        app->sink = gst_element_factory_make("filesink", NULL);

        std::cout << "app->pipeline=" << app->pipeline << std::endl;
        std::cout << "app->src=" << app->src << std::endl;
        std::cout << "app->filter1=" << app->filter1 << std::endl;
        std::cout << "app->encoder=" << app->encoder << std::endl;
        std::cout << "app->filter2=" << app->filter2 << std::endl;
        std::cout << "app->parser=" << app->parser << std::endl;
        std::cout << "app->qtmux=" << app->qtmux << std::endl;
        std::cout << "app->sink=" << app->sink << std::endl;

        if (!app->pipeline ||
            !app->src || !app->filter1 ||
            !app->encoder || !app->filter2 ||
            !app->parser || !app->qtmux ||
            !app->sink)
        {
                printf("Error creating pipeline elements!\n");
                exit(2);
        }

        //Attach elements to pipeline
        gst_bin_add_many(
            GST_BIN(app->pipeline),
            (GstElement *)app->src,
            app->filter1,
            app->encoder,
            app->filter2,
            app->parser,
            app->qtmux,
            app->sink,
            NULL);

        //Set pipeline element attributes
        g_object_set(app->src, "format", GST_FORMAT_TIME, NULL);
        GstCaps *filtercaps1 = gst_caps_new_simple("video/x-raw",
                                                   "format", G_TYPE_STRING, "I420",
                                                   "width", G_TYPE_INT, H264_W,
                                                   "height", G_TYPE_INT, H264_H,
                                                   "framerate", GST_TYPE_FRACTION, H264_FPS, 1,
                                                   NULL);
        g_object_set(G_OBJECT(app->filter1), "caps", filtercaps1, NULL);
        GstCaps *filtercaps2 = gst_caps_new_simple("video/x-h264",
                                                   "stream-format", G_TYPE_STRING, "byte-stream",
                                                   NULL);
        g_object_set(G_OBJECT(app->filter2), "caps", filtercaps2, NULL);
        g_object_set(G_OBJECT(app->sink), "location", "output_h264.mp4", NULL);

        //Link elements together
        g_assert(gst_element_link_many(
            (GstElement *)app->src,
            app->filter1,
            app->encoder,
            app->filter2,
            app->parser,
            app->qtmux,
            app->sink,
            NULL));

        //Play the pipeline
        state_ret = gst_element_set_state((GstElement *)app->pipeline, GST_STATE_PLAYING);
        g_assert(state_ret == GST_STATE_CHANGE_ASYNC);

        //Get a pointer to the test input
#if 0
        FILE *testfile = fopen("test.yuv", "rb");
        g_assert(testfile != NULL);
#endif

        guint64 timeDurationNanoSec = gst_util_uint64_scale_int(1, GST_SECOND /*1sec = 1e+9 nano sec*/, H264_FPS /*25*/);

        //Push the data from buffer to gstpipeline 100 times
        const uint32_t yuvImgSz = H264_W * H264_H * 3 / 2;
        //Allocate memory for framebuffer
        uint8_t *yuvImgBuf = (uint8_t *)malloc(yuvImgSz);
        if (yuvImgBuf == NULL)
        {
                printf("Memory error\n");
                exit(-1);
        } //Errorcheck

        boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;
        bool isDump=false;
        GstFlowReturn ret; //Return value
        for (int i = 0; i < 1000; i++)
        {
                isDump = false;
                if(i%200==0) { isDump = true; }
                updateYuvImg(yuvImgBuf, yuvImgSz, i, isDump);

                GstBuffer *pushedBuffer = gst_buffer_new_allocate(NULL, yuvImgSz, NULL);
                gst_buffer_fill(pushedBuffer, 0, yuvImgBuf, yuvImgSz);

                //Set frame timestamp
                GST_BUFFER_PTS(pushedBuffer) = app->timestamp;
                GST_BUFFER_DTS(pushedBuffer) = app->timestamp;
                GST_BUFFER_DURATION(pushedBuffer) = timeDurationNanoSec;
                app->timestamp += GST_BUFFER_DURATION(pushedBuffer);

                ret = gst_app_src_push_buffer(app->src, pushedBuffer); //Push data into pipeline
                g_assert(ret == GST_FLOW_OK);

                if (i % 200 == 0)
                {
                        printf("fn=%d,frmTime=[%lu (ms), expected=%lu (ms)]\n", i, app->timestamp / 1000000, ((i + 1) * 1000) / H264_FPS);
                }

                //todo: still has memo leak issue if we comment this line out, otherwise has run-time errors
                //gst_buffer_unref (pushedBuffer);

                //12/13/2022 wus1: do we need to free resource?
                //--------------------------------------------------------------------------------------------------
                //https://stackoverflow.com/questions/36301355/memory-leaks-while-using-gstbuffer
                //If you use gst_app_src_push_buffer function I guess you do not have to free resources
                //because gst_app_src_push_buffer will own the buffer (which means it also frees it) Check this example:
                // https://cgit.freedesktop.org/gstreamer/gst-plugins-base/tree/tests/examples/app/appsrc_ex.c
                //If you use need-data callback you may need to free data - check this example:
                //https://cgit.freedesktop.org/gstreamer/gst-plugins-base/tree/tests/examples/app/appsrc-stream.c
                //--------------------------------------------------------------------------------------------------
                //<yuvImgBuf> will be freeed inside gst ?
        }

        //Declare end of stream
        gst_app_src_end_of_stream(GST_APP_SRC(app->src));
        printf("End Program, dt=%d\n", ngv::timeIntervalMillisec(t0));
        usleep(100000);

        return 0;
}
