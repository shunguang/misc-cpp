#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__
#include <random>
#include <chrono>
#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>  
#include <math.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>

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
#include <fstream>

//we do not need this when debug ekf in vs2019
#include <opencv2/core/version.hpp>
#if CV_VERSION_MAJOR < 4
#include <opencv/cv.h>
#endif
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videostab/global_motion.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define POSIX_LOCAL_TIME	  	(boost::posix_time::microsec_clock::local_time())
#define POSIX_UNIVERSAL_TIME	(boost::posix_time::microsec_clock::universal_time())
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
#define APP_ISNAN(x)	  ( x != x )
#define APP_NAN_UINT32  (0xFFFFFFFF)
#define APP_MAX(a,b)	( (a) > (b) ? (a) : (b) )
#define APP_MIN(a,b)	( (a) > (b) ? (b) : (a) )
#define APP_INT_RAND_IN_RANGE(i1,i2) ( (i1) + rand() % ((i2) + 1 - (i1)) )

#define APP_USED_TIME_MS(t0)  ( 1000 * (clock() - t0) / CLOCKS_PER_SEC )

#define USLEEP_1_SEC		1000000
#define USLEEP_1_MILSEC		1000

#define APP_SLEEP( ms )   boost::this_thread::sleep(boost::posix_time::milliseconds(ms))

#ifdef __GNUC__
#define  sscanf_s  sscanf
#define  swscanf_s swscanf
   #define  GNU_PACK  __attribute__ ((packed))
#else
   #define  GNU_PACK
#endif

#define float32 float
#define float64 double

#define APP_FRM_CNT			      uint64_t
#define APP_TIME_MS           uint64_t
#define APP_TIME_US           uint64_t
#define APP_TIME_CURRENT_US (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())
#define APP_TIME_CURRENT_MS (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())
#define APP_TIME_US2MS( t_us )  ( (t_us)/1000 )
#define APP_TIME_MS2US( t_ms )  ( (t_ms)*1000 )

#endif
