#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>

#ifdef __cplusplus
}
#endif

#include "libUtil/util.h"

#ifdef av_err2str
#undef av_err2str
av_always_inline char *av_err2str(int errnum)
{
    // static char str[AV_ERROR_MAX_STRING_SIZE];
    // thread_local may be better than static in multi-thread circumstance
    thread_local char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#endif

using namespace ngv;
static AppCpuTimeProf g_cpuProf(50);
struct OutputStream
{
    AVFormatContext *ctx;

    AVCodecContext *venc;

    AVStream *vst;
    AVStream *dst;

    AVFrame *f;
    AVPacket *p;

    // video image, used to pull raw yuv420 data from vid.bin
    struct
    {
        uint8_t *d[4];
        int lsz[4];
        int sz;
        enum AVPixelFormat pf;
        int w;
        int h;
    } vi;

    struct
    {
        uint32_t fn;
        uint16_t sz;
        uint8_t *d;
    } klv;

    int64_t pts;
};

static void read_klv(struct OutputStream *os, FILE *f)
{
    if (feof(f))
        return;

    uint16_t osz = os->klv.sz;
    uint8_t r;
    int i;

    os->klv.fn = 0;
    for (i = 0; i < 4; ++i)
    {
        fread(&r, 1, 1, f);
        os->klv.fn |= ((uint32_t)r << (24 - (i * 8)));
    }

    for (i = 0; i < 2; ++i)
    {
        fread(&r, 1, 1, f);
        os->klv.sz |= ((uint16_t)r << (8 - (i * 8)));
    }

    if (osz != os->klv.sz)
        os->klv.d = (uint8_t *)realloc(os->klv.d, os->klv.sz);

    fread(os->klv.d, 1, os->klv.sz, f);

    if (os->klv.fn % 100 == 0)
    {
        printf("read klv frame n:%u, size:%u\n", os->klv.fn, os->klv.sz);
    }
}

static int write_klv(struct OutputStream *os)
{
    uint8_t *d = (uint8_t *)malloc(os->klv.sz);
    memcpy(d, os->klv.d, os->klv.sz);
    if (av_packet_from_data(os->p, d, os->klv.sz) < 0)
    {
        fprintf(stderr, "Could not initialize klv packet\n");

        return -1;
    }

    uint8_t *sd = (uint8_t *)malloc(1);
    *sd = 0xfc;
    av_packet_add_side_data(os->p, AV_PKT_DATA_METADATA_UPDATE, sd, 1);
    os->p->pts = os->klv.fn;
    os->p->dts = os->klv.fn;
    os->p->stream_index = os->dst->id;

    av_packet_rescale_ts(os->p, os->venc->time_base, os->dst->time_base);
    os->p->pts += os->dst->start_time;
    os->p->dts += os->dst->start_time;

    // printf("write_klv(): os->p->stream_index=%d, os->p->pts=%lu, os->p->dts=%lu, os->dst->start_time=%lu\n",
    //     os->p->stream_index, os->p->pts,os->p->dts,os->dst->start_time );

    int ret = av_interleaved_write_frame(os->ctx, os->p);
    if (ret < 0)
    {
        fprintf(stderr, "Error writing output packet: %s\n",
                av_err2str(ret));

        return -1;
    }

    return 0;
}

static int write_frame(struct OutputStream *os, FILE *vf, FILE *kf)
{
    int64_t dt_wrt_wall=0, dt_wrt_ucpu=0, dt_wrt_scpu=0;
    ngv::AppTicToc tt;
    ngv::AppTicToc tt2;


    AVFrame *fr = os->f;
    av_frame_make_writable(fr);

    fread(os->vi.d[0], 1, os->vi.sz, vf);
    if (0 == os->pts % 200)
    {
        printf("encoding video frame %ld\n", os->pts);
    }

    tt.tic(); //calculate enc cpu time profile

    av_image_copy(fr->data, fr->linesize, (const uint8_t **)os->vi.d, os->vi.lsz,
                  os->vi.pf, os->vi.w, os->vi.h);
    fr->pts = os->pts++;

    int ret;
    ret = avcodec_send_frame(os->venc, fr);
    if (ret < 0)
    {
        fprintf(stderr, "Error sending frame to encoder\n");

        return -1;
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(os->venc, os->p);
        if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF))
        {
            ret = 0;
            break;
        }
        else if (ret < 0)
        {
            fprintf(stderr, "Error encoding frame\n");
            ret = -1;
            break;
        }

        int64_t pts = os->p->pts;
        av_packet_rescale_ts(os->p, os->venc->time_base, os->vst->time_base);
        os->p->pts += os->vst->start_time;
        os->p->dts += os->vst->start_time;
        os->p->stream_index = 0;

        // printf("write_frame(): os->p->stream_index=%d,os->p->pts=%lu, os->p->dts=%lu, os->vst->start_time=%lu\n",
        //     os->p->stream_index, os->p->pts, os->p->dts, os->vst->start_time );

        tt2.tic();

        ret = av_interleaved_write_frame(os->ctx, os->p);
        if (ret < 0)
        {
            fprintf(stderr, "Error writing output packet: %s\n",
                    av_err2str(ret));

            ret = -1;
            break;
        }

        if (pts == os->klv.fn)
        {
            if (write_klv(os) < 0){
                fprintf(stderr, "Error writing klv packet.\n");
                ret = -1;
                break;
            }
            read_klv(os, kf);
        }
        tt2.toc();
        dt_wrt_wall += tt2.dt_wall_us;
        dt_wrt_ucpu += tt2.dt_usr_cpu_us; 
        dt_wrt_scpu += tt2.dt_sys_cpu_us;
    }

    //---------------- calculate enc cpu time profile ---------------
    // since transition time is instable, we ignor them
    if (os->pts > 50 && ret == 0)
    {
        tt.toc();
        g_cpuProf.addSamples(tt.dt_wall_us-dt_wrt_wall, tt.dt_usr_cpu_us - dt_wrt_ucpu, tt.dt_sys_cpu_us - dt_wrt_scpu);
    }
    //-------------------------------------------------

    return ret;
}

static int initialize_stream(struct OutputStream *os, char **argv)
{

    char *filename = argv[0];
    char *encoder = argv[1];
    char *swidth = argv[2];
    char *sheight = argv[3];
    char *spixfmt = argv[4];
    char *stbn = argv[5];
    char *stbd = argv[6];
    char *br = argv[7];

    int w = (int)strtoul(swidth, (char **)0, 10);
    int h = (int)strtoul(sheight, (char **)0, 10);
    int tbn = (int)strtoul(stbn, (char **)0, 10);
    int tbd = (int)strtoul(stbd, (char **)0, 10);
    int64_t bitrate = (int64_t)strtoul(br, (char **)0, 10);

    dumpLog("initialize_stream(): filename=%s,encoder=%s,w=%d,h=%d,spixfmt=%s,tbn=%d,tbd=%d, bitrate=%ld", filename, encoder, w, h, spixfmt, tbn, tbd, bitrate);

    enum AVPixelFormat pix_fmt = av_get_pix_fmt(spixfmt);
    if (pix_fmt == AV_PIX_FMT_NONE)
    {
        fprintf(stderr, "Could not find pixel format '%s'\n", spixfmt);

        return -1;
    }

    printf("Settings:\n");
    printf("  Frame size:\t%dx%d\n", w, h);
    printf("  Frame rate:\t%.02f\n", ((double)tbd / tbn));

    os->klv.sz = 0;
    os->klv.d = 0;

    avformat_alloc_output_context2(&(os->ctx), 0, "mpegts", filename);

    if (!(os->ctx))
    {
        fprintf(stderr, "Could not allocate output format context\n");

        return -1;
    }

    os->vst = avformat_new_stream(os->ctx, 0);
    if (!(os->vst))
    {
        fprintf(stderr, "Could not allocate video stream\n");

        return -1;
    }
    os->vst->id = 0;
    os->vst->time_base = (AVRational){tbn, tbd};
    os->vst->start_time = 0;

    const AVCodec *vcodec = avcodec_find_encoder_by_name(encoder);
    if (!vcodec)
    {
        fprintf(stderr, "Could not find encoder '%s'\n", encoder);

        return -1;
    }

    os->venc = avcodec_alloc_context3(vcodec);
    if (!(os->venc))
    {
        fprintf(stderr, "Could not allocate an encoding context\n");

        return -1;
    }
    os->venc->codec_id = vcodec->id;
    os->venc->bit_rate = bitrate;
    os->venc->gop_size = 12;
    os->venc->width = w;
    os->venc->height = h;
    os->venc->time_base = (AVRational){tbn, tbd};
    os->venc->pix_fmt = pix_fmt;

    avcodec_open2(os->venc, vcodec, 0);
    avcodec_parameters_from_context(os->vst->codecpar, os->venc);

    os->pts = 0;

    os->dst = avformat_new_stream(os->ctx, 0);
    if (!(os->vst))
    {
        fprintf(stderr, "Could not allocate data stream\n");

        return -1;
    }
    os->dst->id = 1;
    os->dst->time_base = (AVRational){tbn, tbd};
    os->dst->start_time = 0;

    AVCodecContext *klvenc = avcodec_alloc_context3(0);

    if (!klvenc)
    {
        fprintf(stderr, "Could not allocate KLV encoder context\n");

        return -1;
    }
    klvenc->codec_type = AVMEDIA_TYPE_DATA;
    klvenc->codec_id = AV_CODEC_ID_SMPTE_KLV;
    klvenc->time_base = (AVRational){tbn, tbd};

    avcodec_parameters_from_context(os->dst->codecpar, klvenc);
    avcodec_free_context(&klvenc);

    os->f = av_frame_alloc();
    if (!(os->f))
    {
        fprintf(stderr, "Could not allocate internal data\n");

        return -1;
    }
    os->f->format = pix_fmt;
    os->f->width = w;
    os->f->height = h;

    if (av_frame_get_buffer(os->f, 0) < 0)
    {
        fprintf(stderr, "Could not allocate frame data\n");

        return -1;
    }

    os->p = av_packet_alloc();
    if (!(os->p))
    {
        fprintf(stderr, "Could not allocate internal data\n");

        return -1;
    }

    os->vi.sz = av_image_alloc(os->vi.d, os->vi.lsz, w, h, pix_fmt, 1);
    for (int i = 0; i < 4; i++)
    {
        printf("i=%d, d[i]=%x, lsz[i]=%d\n", i, os->vi.d[i], os->vi.lsz[i]);
    }
    printf("w=%d, h=%d, sz=%d\n", w, h, os->vi.sz);

    if (os->vi.sz < 0)
    {
        fprintf(stderr, "Could not allocate internal data\n");

        return -1;
    }
    os->vi.pf = pix_fmt;
    os->vi.w = w;
    os->vi.h = h;

    return 0;
}

static void free_stream(struct OutputStream *os)
{
    avcodec_free_context(&(os->venc));

    avio_closep(&(os->ctx->pb));

    avformat_free_context(os->ctx);

    av_frame_free(&(os->f));
    av_packet_free(&(os->p));

    av_freep(&(os->vi.d[0]));

    free(os->klv.d);
}

int test_ffmpeg_mux_ts(const CfgApp &cfg0)
{
    AppTicToc ttSeq;
    ttSeq.tic();

    const CfgEncPtr cfg = cfg0.enc;
    dumpLog("test_ffmpeg_mux_ts():CfgEnc:" + cfg->toString());

   	g_cpuProf.setThdCntToPrintOut(cfg->nFrmsToPeekCpuTimeProfile);
    const std::string outputFilename = cfg->ouputFilePath + "/ffmpeg-" + cfg->ffmpegEncName + ".ts";

    std::string imgW_s = std::to_string(cfg->imgW);
    std::string imgH_s = std::to_string(cfg->imgH);
    std::string pixfmt = "yuv420p";
    std::string stbn_s = std::to_string(cfg->tbn);
    std::string stbd_s = std::to_string(cfg->tbd);
    std::string br_s = std::to_string(cfg->bitrate);

    char **argv = (char **)malloc(8 * sizeof(char *));
    argv[0] = (char *)outputFilename.c_str();
    argv[1] = (char *)cfg->ffmpegEncName.c_str(); //"mpeg4"
    argv[2] = (char *)imgW_s.c_str();
    argv[3] = (char *)imgH_s.c_str();
    argv[4] = (char *)pixfmt.c_str();
    argv[5] = (char *)stbn_s.c_str();
    argv[6] = (char *)stbd_s.c_str();
    argv[7] = (char *)br_s.c_str();

    if( fileExists(outputFilename))
        remove(outputFilename.c_str());

    struct OutputStream os;
    if (initialize_stream(&os, argv) < 0)
        return 1;

    FILE *vf = fopen(cfg->inputYuvFile.c_str(), "rb");
    if (!vf)
    {
        fprintf(stderr, "Could not open %s\n", cfg->inputYuvFile.c_str());
        return 1;
    }
    FILE *kf = fopen(cfg->inputKlvFile.c_str(), "rb");
    if (!kf)
    {
        fprintf(stderr, "Could not open %s\n", cfg->inputKlvFile.c_str());
        return 1;
    }
    read_klv(&os, kf);

    avio_open(&(os.ctx->pb), outputFilename.c_str(), AVIO_FLAG_WRITE);

    // to avoid: Packets poorly interleaved, failed to avoid negative
    //  timestamp -9223372036853274308 in stream 0.
    //  Try -max_interleave_delta 0 as a possible workaround.
    os.ctx->max_interleave_delta = 0;

    (void)avformat_write_header(os.ctx, 0);
    while (!feof(vf))
    {
        if (write_frame(&os, vf, kf) < 0)
            return 1;
    }
    av_write_trailer(os.ctx);

    fclose(kf);
    fclose(vf);
    free_stream(&os);
    free(argv);

    ttSeq.toc();
    std::string outMsg;
    size_t nTotFrms = g_cpuProf.calMeanStd(outMsg, "PerFrameStatistics");
    printf("encName=%s, %s\n", cfg->ffmpegEncName.c_str(), outMsg.c_str());
    printf("\t\t%s\n", ttSeq.toString("ms", "Total Time Used for decoding the whole seq:").c_str());
    return 0;
}
