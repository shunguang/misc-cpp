#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>

#ifdef __cplusplus
}
#endif


#include "test_ffmpeg_util.H"
//#include "HostYuvFrm.H"

using namespace app;

static AppCpuTimeProf g_cpuProf(100);

#ifdef av_err2str
#undef av_err2str
av_always_inline char* av_err2str(int errnum)
{
    // static char str[AV_ERROR_MAX_STRING_SIZE];
    // thread_local may be better than static in multi-thread circumstance
    thread_local char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#endif


struct video_frame {
    uint32_t n;
    int64_t pts;
};

static uint32_t vfn = 0;
static struct {
    struct video_frame* frames;
    size_t buffer_size;
    size_t cfr;
} vfrs;

static void add_frame(uint32_t fn, int64_t pts) {
    vfrs.frames[vfrs.cfr].n = fn;
    vfrs.frames[vfrs.cfr].pts = pts;

    if (++vfrs.cfr >= vfrs.buffer_size)
        vfrs.cfr = 0;
}

static int64_t find_frame_by_pts(int64_t pts) {
    for (size_t i = 0; i < vfrs.buffer_size; ++i) {
        if (vfrs.frames[i].pts == pts)
            return vfrs.frames[i].n;
    }

    return -1;
}

static int open_decoder(AVCodecContext** dctx, AVFormatContext* fctx, const char *codecName ) {
    int idx = av_find_best_stream(fctx, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);

    if (idx < 0) {
        fprintf(stderr, "Could not find video stream!\n");

        return -1;
    }

    AVStream* st = fctx->streams[idx];

    vfrs.cfr = 0;
    vfrs.buffer_size = (size_t)ceil(
            (double)(st->r_frame_rate.num)/st->r_frame_rate.den);
    vfrs.frames = (video_frame*)malloc(sizeof(struct video_frame) * vfrs.buffer_size);

    if (!(vfrs.frames)) {
        fprintf(stderr, "Could not allocate video frame mapping buffer\n");

        return -1;
    }

	printf( "st->codecpar->codec_id=%d,  AV_CODEC_ID_H264=%d\n", st->codecpar->codec_id,  AV_CODEC_ID_H264 );
    //const AVCodec* dec = avcodec_find_decoder(st->codecpar->codec_id);
	const AVCodec* dec = avcodec_find_decoder_by_name( codecName );
    if (!dec) {
        fprintf(stderr, "Could not find video decoder!\n");

        return -1;
    }

    *dctx = avcodec_alloc_context3(dec);

    if (!(*dctx)) {
        fprintf(stderr, "Could not allocate video codec context\n");

        return -1;
    }

    if (avcodec_parameters_to_context(*dctx, st->codecpar) < 0) {
        fprintf(stderr, "Failed to copy codec parameters to decoder context\n");

        return -1;
    }

    if (avcodec_open2(*dctx, dec, 0) < 0) {
        fprintf(stderr, "Could not open decoder\n");

        return -1;
    }

    return idx;
}

static int decode_video(AVStream* s, AVCodecContext* d, AVPacket* p, FILE* f) {

    AppTicToc tt;
    AppTicToc tt2;
    int64_t dt_wrt_wall=0;

    tt.tic(); //calculate cpu time profile
    int ret = avcodec_send_packet(d, p);
    if (ret < 0) {
        fprintf(stderr, "Error submitting packet for decoding (%s)\n",
                av_err2str(ret));

        return ret;
    }

    AVFrame* frame = av_frame_alloc();
    while (ret >= 0) {
        ret = avcodec_receive_frame(d, frame);
        if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF)) {
            //fprintf(stderr, "A-Error during decode (%s)\n", av_err2str(ret));
            ret=0;
            break;
        } else if (ret < 0) {
            fprintf(stderr, "B-Error during decode (%s)\n", av_err2str(ret));
            ret=-1;
            break;
        }

        if( 0==vfn%200){
            printf("video frame n:%d coded:%d\n", vfn, frame->coded_picture_number);
        }

        add_frame(vfn, (p->pts - s->start_time));
        ++vfn;

        //---------start tic time used for writing data into file ------
        tt2.tic();

        uint8_t* vdata[4] = { 0 };
        int vlinesz[4];
        int vbsz = av_image_alloc(vdata, vlinesz, d->width, d->height,
                d->pix_fmt, 1);

        av_image_copy(vdata, vlinesz, (const uint8_t**)(frame->data),
                frame->linesize, d->pix_fmt, d->width, d->height);


        fwrite(vdata[0], 1, vbsz, f);
        av_freep(&vdata[0]);
        //--------- end tic time used for writing data into file ------
        tt2.toc();
        dt_wrt_wall += tt2.dt_wall_us;
    }
    av_frame_free(&frame);
    if ( vfn > 50)
    {
        tt.toc();
        g_cpuProf.addSamples(tt.dt_wall_us-dt_wrt_wall);
    }

    return ret;
}


int main( int argc, char *argv[] ) 
{
    AppTicToc ttSeq;
    ttSeq.tic();
    vfrs.buffer_size = 0;
    vfrs.frames = 0;
	AVFormatContext* ctx = 0;

	if( argc !=3 ){
		printf("usage: test_dec MaxNumOfFramesToDec codecName\n");
        printf("note:\n");
        printf("  (1) $ffmpeg -decoders | grep h264 to find the supported coders in your machine\n");
        printf("  (2) some decoder codec name list: [h264_cuvid,h264_nvmpi,h264_v4l2m2m; cpu based: h264]\n");
		return -1;
	}

    CfgDec cfg;
	cfg.inputFilePath = "/osi/test-data/videos/Truck.ts";
	cfg.maxNumOfFrms = atoi( argv[1] ) ;
	cfg.codecName = std::string( argv[2] );  //h264_cuvid, h264_nvmpi, h264

    const char *tsFile = cfg.inputFilePath.c_str();


   	g_cpuProf.setThdCntToPrintOut(cfg.nFrmsToPeekCpuTimeProfile);


    int ret = avformat_open_input(&ctx, tsFile, 0, 0);
    if ( ret < 0) {
        char buf[512];
        av_strerror( ret, buf, 512 );
        fprintf(stderr, "could not open source file:<%s>, ret=%d, error=%s\n", tsFile, ret, buf);

        return 1;
    }

    if (avformat_find_stream_info(ctx, 0) < 0) {
        fprintf(stderr, "could not find stream info\n");

        return 1;
    }

    av_dump_format(ctx, 0, tsFile, 0);
    // init video decoder
    AVCodecContext* dctx;
    int vst = open_decoder(&dctx, ctx, cfg.codecName.c_str() );
    if (vst < 0)
        return 1;
    int dst = av_find_best_stream(ctx, AVMEDIA_TYPE_DATA, -1, -1, 0, 0);
    if (dst < 0)
        return 1;

    // extract KLV and video
    remove( cfg.outputKlvFile.c_str());
    remove( cfg.outputYuvFile.c_str());
    FILE* klvf = fopen(cfg.outputKlvFile.c_str(), "wb");
    FILE* vidf = fopen(cfg.outputYuvFile.c_str(), "wb");
    AVPacket* p = av_packet_alloc();
    av_init_packet(p);
    p->data = 0;
    p->size = 0;

    uint32_t nImgCnt=0, nKlvCnt=0;
    while ( av_read_frame(ctx, p) >= 0 ) {
        if (p->stream_index == vst) {
            decode_video(ctx->streams[p->stream_index], dctx, p, vidf);
            nImgCnt++;
        } else if (p->stream_index == dst) {
            //printf("KLV packet of size %d\n", p->size);

            int64_t pts = p->pts - ctx->streams[dst]->start_time;
            int64_t vf = find_frame_by_pts(pts);

            if (vf >= 0) {
                //printf("Found correlated video frame %lld\n", vf);
                uint32_t fid = (uint32_t)vf;
                uint8_t to_write;

                to_write = (uint8_t)((fid & 0xff000000) >> 24);
                fwrite(&to_write, 1, 1, klvf);
                to_write = (uint8_t)((fid & 0x00ff0000) >> 16);
                fwrite(&to_write, 1, 1, klvf);
                to_write = (uint8_t)((fid & 0x0000ff00) >> 8);
                fwrite(&to_write, 1, 1, klvf);
                to_write = (uint8_t)(fid & 0x000000ff);
                fwrite(&to_write, 1, 1, klvf);
                to_write = (uint8_t)((p->size & 0xff00) >> 8);
                fwrite(&to_write, 1, 1, klvf);
                to_write = (uint8_t)(p->size & 0xff);
                fwrite(&to_write, 1, 1, klvf);
                fwrite(p->data, 1, p->size, klvf);

                nKlvCnt++;
            } else {
               printf("Could not find matching video frame\n");
            }
        }

        if (nImgCnt%200==0){
            printf("nImgCnt=%u, nKlvCnt=%u, maxNumOfFrms=%u\n", nImgCnt, nKlvCnt, cfg.maxNumOfFrms);
        }

        if ( nImgCnt>cfg.maxNumOfFrms ){
            break;
        }
    }
    fclose(vidf);
    fclose(klvf);
	
    printf("\nVideo info: nImgCnt=%u, nKlvCnt=%u\n", nImgCnt, nKlvCnt);
    printf("  Frame size:\t%dx%d\n", dctx->width, dctx->height);
    printf("  Pixel format:\t%s\n", av_get_pix_fmt_name(dctx->pix_fmt));
    printf("  Framerate:\t%d/%d\n", ctx->streams[vst]->r_frame_rate.num,
            ctx->streams[vst]->r_frame_rate.den);
    printf("  # frames:\t%u\n", vfn);


    avcodec_free_context(&dctx);
    av_packet_free(&p);
    avformat_close_input(&ctx);
    if (vfrs.frames)
        free(vfrs.frames);

   ttSeq.toc();   
   std::string outMsg;
   size_t nTotFrms = g_cpuProf.calMeanStd(outMsg, "PerFrameStatistics: ");
   printf("decFileName=%s, %s\n", cfg.inputFilePath.c_str(), outMsg.c_str());
   printf("\t\t%s\n", ttSeq.toString("ms", "Total Time Used for decoding the whole seq:").c_str());

    return 0;
}
 
