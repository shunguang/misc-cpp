#ifndef __TEST_FFMPEG_UTIL__H__
#define __TEST_FFMPEG_UTIL__H__

#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <assert.h>

#include <cstddef>  
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <locale>
#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#if 1
#include <opencv2/core/version.hpp>

#if CV_VERSION_MAJOR < 4
#include <opencv/cv.h>
#endif
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#endif


//#define float32		float
//#define float64		double

//--------- my owen defs ----------------
#define TS_SINCE_EPOCH_MS   (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
#define TS_SINCE_EPOCH_US   (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
#define TS_SINCE_EPOCH_NS   (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())

#define APP_BRG
#define APP_GRAY
#define APP_BRG_OR_GRAY

#define APP_EPS_64F		(1e-15)
#define APP_EPS_32F 	(1e-7)

#define APP_REALMIN_32F (1e-38f)
#define APP_REALMAX_32F (1e+38f)
#define APP_REALMIN_64F (1e-308)
#define APP_REALMAX_64F (1e+308)

#define APP_MAX_UINT16 (0xFFFF)
#define APP_MAX_uint32_t (0xFFFFFFFF)
#define APP_MAX_UINT64 (0xFFFFFFFFFFFFFFFF)
#define APP_SEQ_END	   (0xFFFFFFFF)
#define APP_NAN_uint32_t (0xFFFFFFFF)

#define APP_HALF_PI       (1.57079632679490)
#define APP_PI            (3.14159265358979)
#define APP_TWO_PI        (6.28318530717959)
#define APP_D2R           (0.01745329251994)
#define APP_R2D           (57.29577951308232)

#define APP_ROUND(x)	( (int) floor( x + 0.500 ) )
#define APP_NAN			( sqrt(-1.0) )
#define APP_ISNAN(x)	( x != x )

#define APP_MAX(a,b)	( (a) > (b) ? (a) : (b) )
#define APP_MIN(a,b)	( (a) > (b) ? (b) : (a) )
#define APP_INT_RAND_IN_RANGE(i1,i2) ( (i1) + rand() % ((i2) + 1 - (i1)) )

#define APP_USED_TIME(t0)  ( 1000 * (clock() - t0) / CLOCKS_PER_SEC )

#define USLEEP_1_SEC		1000000
#define USLEEP_1_MILSEC		1000


namespace app {

	#define app_smp_t int64_t
   class AppMeanStd
    {
    public:
        AppMeanStd(const std::string &unit_ = "")
            : unit(unit_), v()
        {
        }

        void reset()
        {
            v.clear();
        }

        size_t calMeanStd()
        {
            if (v.empty()){
                mu = APP_NAN;
                std = APP_NAN;
                return 0;
            }

            size_t n = v.size();
            double s=0;
            for( app_smp_t e : v){
                s += (double)e;
            }
            mu = s/n;

            s = 0;
            for( app_smp_t e : v){
                s += ((double)e - mu ) * ((double)e - mu );
            }
            std = sqrt(s/n);
            return n;
        }

        void addSample(const app_smp_t x)
        {
            v.push_back(x);
        }

    public:
        std::string unit{""};
        std::vector<app_smp_t> v{};
        double mu{0};
        double std{0};
    };

    class AppTicToc
    {
    public:
        AppTicToc(){}

        void tic()
        {
            wall_time_t1_us = TS_SINCE_EPOCH_US;
        }

        void toc()
        {
            if (wall_time_t1_us > 0)
            {
                wall_time_t2_us = TS_SINCE_EPOCH_US;
                dt_wall_us = wall_time_t2_us - wall_time_t1_us;
            }
            else{
                printf("Warning: you neeto call tic(), first!");
                dt_wall_us =0;
            }
            wall_time_t1_us = -1;
        }
        
        std::string toString( const std::string &unit="ms", const std::string &msg="" ){
           	std::ostringstream oss;
            if (0 == unit.compare("ms"))
            {
                oss << msg << "wall=" << dt_wall_us/1000 << "(ms)";
            }
            else
            {
                oss << msg << "wall=" << dt_wall_us << "(us)";
            }
            return oss.str();
        }

    public:        
        //used cpu time in microseconds
        int64_t dt_wall_us{0};

        int64_t  wall_time_t1_us{-1};
        int64_t  wall_time_t2_us{0};
    };

	class CfgEnc
	{
	public:
		CfgEnc()=default;
		CfgEnc(const CfgEnc &x)=default;
		CfgEnc &operator=(const CfgEnc &x)=default;
		virtual ~CfgEnc()=default;

	public:
		int32_t imgW{1920};
		int32_t imgH{1080};
		//T0 = tbn/tbd(sec), fps = 1/T0 (Hz)
		int32_t tbn{1001};  //timebase numerorator
		int32_t tbd{30000}; //timebase denominator
		int32_t bitrate{10000000};

		std::string ffmpegEncName{"mpeg4"};
		std::string inputKlvFile{"./klv.bin"};
		std::string inputYuvFile{"./vid.bin"};
		std::string ouputFilePath{"./out_muxEnc.ts"};
		bool useVidCapFilterAfterEncoder{false};
		uint32_t  nFrmsToPeekCpuTimeProfile{100};
	};
	typedef std::shared_ptr<CfgEnc> CfgEncPtr;


	class CfgDec
	{
	public:
		CfgDec()=default;
		CfgDec(const CfgDec &x) = default;
		CfgDec &operator=(const CfgDec &x) = default;
		virtual ~CfgDec() = default;


	public:
		int32_t imgW{1920};
		int32_t imgH{1080};
		uint32_t maxNumOfFrms{0xFFFFFFFF};
		std::string inputFilePath{"./Truck.ts"};
		std::string outputKlvFile{"./klv.bin"};
		std::string outputYuvFile{"./vid.bin"};
		uint32_t  nFrmsToPeekCpuTimeProfile{100};
		std::string codecName{"h264"};
	};
	typedef std::shared_ptr<CfgDec> CfgDecPtr;


   class AppCpuTimeProf
    {
    public:
        AppCpuTimeProf(const size_t thdCntTpPrintf=100 ) 
        : thdCntToPrintOut(thdCntTpPrintf)
        , wall("us")
        {
            reset();
        }

        void setThdCntToPrintOut( const size_t nFrms ){
            thdCntToPrintOut = nFrms;
        }

        void reset()
        {
            cnt=0;
            wall.reset();
        }   

        size_t calMeanStd(std::string & out, const std::string &msg = "")
        {
            size_t nSmps = 0;
            nSmps = wall.calMeanStd();

            char buf[1024];
            snprintf(buf, 1024, "%s frmCnt=%ld, wall(mu=%.0f, std=%.0f) (us)",
                                    msg.c_str(), nSmps, wall.mu, wall.std);

            out = std::string(buf);

            return nSmps;
        }

        void addSamples(const app_smp_t dt_wall)
        {
            cnt++;
            wall.addSample(dt_wall);
            if( 0 == cnt % thdCntToPrintOut)
            {
                std::string result;
                calMeanStd(result, "perFrameStatistics");
                printf("current: dt_wall=%ld, %s\n", dt_wall, result.c_str());
            }

        }

    private:
        size_t thdCntToPrintOut{100};
        size_t cnt{0};
        AppMeanStd wall;
    };

}
#endif




