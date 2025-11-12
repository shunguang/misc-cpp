/*
 * main.cpp
 *
 *  Created on: Nov 18, 2022
 *      Author: wus1
 */
#include "libUtil/util.h"

using namespace std;
using namespace ngv;

int test_ffmpeg_demux_ts(const CfgApp &cfg);
int test_ffmpeg_mux_ts(const CfgApp &cfg);
int test_gst_enc_H264(const CfgApp &cfg);
int test_gst_mux_enc_v1(const CfgApp &cfg);
int test_gst_mux_enc_v2(const CfgApp &cfg);
int test_gst_demux_dec_v1(const CfgApp &cfg);
int test_gst_demux_dec_v2(const CfgApp &cfg);
int test_small_utils();

void app_usage();

int main(int argc, char *argv[])
{
    int x = 0;
    app_usage();
    if (argc < 2)
    {
        printf("to few input params\n");
        return -1;
    }

    //---set lo gfile -----
    const std::string logFilename = "./log.txt";
    std::cout << logFilename << std::endl;
    startLogThread(logFilename, true);
    dumpLog("---log start---");

    const int flag = atoi(argv[1]);
    if (flag == 8)
    {
        x = test_small_utils();
    }
    else
    {
        //--- read cfg -----
        const std::string cfgFile(argv[2]);
        CfgApp cfg;
        if (!fileExists(cfgFile))
        {
            appExit("Cfg file <%s> does not exist!", cfgFile.c_str());
        }
        cfg.readFromFile(cfgFile.c_str());
        cout << "cfg=" << cfg.toString() << endl;

        switch (flag)
        {
        case 1:
            x = test_ffmpeg_demux_ts(cfg);
            break;
        case 2:
            x = test_ffmpeg_mux_ts(cfg);
            break;
        case 3:
            x = test_gst_mux_enc_v1(cfg);
            break;
        case 4:
            x = test_gst_mux_enc_v2(cfg);
            break;
        case 5:
            x = test_gst_demux_dec_v1(cfg);
            break;
        case 6:
            x = test_gst_demux_dec_v2(cfg);
            break;
        case 7:
            x = test_gst_enc_H264(cfg);
            break;
        default:
            ngv::dumpLog("main(): todo: ");
            break;
        }
    }

    ngv::dumpLog("---log end---");
    ngv::endLogThread();
    return x;
}

void app_usage()
{
    printf("./test.out <flag> <cfgFileName>\n");
    printf("examples: \n");
    printf("./test.out 1 cfgfile --- test_ffmpeg_demux_ts()\n");
    printf("./test.out 2 cfgfile --- test_ffmpeg_mux_ts()\n");
    printf("./test.out 3 cfgfile --- test_gst_mux_enc_v1()\n");
    printf("./test.out 4 cfgfile --- test_gst_mux_enc_v2() - experimental -does not work!\n");
    printf("./test.out 5 cfgfile --- test_gst_demux_dec_v1()\n");
    printf("./test.out 6 cfgfile --- test_gst_demux_dec_v2() - experimental -does not work!\n");
    printf("./test.out 7 cfgFile --- test_gst_enc_H264()\n");
    printf("./test.out 8 -- test_small_util()\n");
}

