#ifndef _APP_TIC_TOC_H_
#define _APP_TIC_TOC_H_

#include "AppDefs.h"
#include "AppMeanStd.h"
namespace app
{
    class AppTicToc
    {
    public:
        AppTicToc(){}

        void tic()
        {
            wall_time_t1_us = TS_SINCE_EPOCH_US;
#ifdef __linux__ 
            getrusage(RUSAGE_SELF, &ru1);    
#endif
        }

        void toc()
        {
            if (wall_time_t1_us > 0)
            {
                wall_time_t2_us = TS_SINCE_EPOCH_US;
                dt_wall_us = wall_time_t2_us - wall_time_t1_us;
#ifdef __linux__ 
                getrusage(RUSAGE_SELF, &ru2);
                dt_usr_cpu_us = (ru2.ru_utime.tv_sec - ru1.ru_utime.tv_sec) * 1000000 +
                                (ru2.ru_utime.tv_usec - ru1.ru_utime.tv_usec);
                dt_sys_cpu_us = (ru2.ru_stime.tv_sec - ru1.ru_stime.tv_sec) * 1000000 +
                                (ru2.ru_stime.tv_usec - ru1.ru_stime.tv_usec);
#endif
            }
            else{
                printf("Warning: you need to call tic(), first!");
                dt_wall_us =0;
                dt_usr_cpu_us=0;
                dt_sys_cpu_us;
            }
            wall_time_t1_us = -1;
        }

        std::string toString( const std::string &unit="ms", const std::string &msg="" ){
           	std::ostringstream oss;
            if (0 == unit.compare("ms"))
            {
                oss << msg << "wall=" << dt_wall_us/1000
                    << "(ms), usr_cpu=" << dt_usr_cpu_us/1000
                    << "(ms), sys_cpu=" << dt_sys_cpu_us/1000 << "(ms)";
            }
            else
            {
                oss << msg << "wall=" << dt_wall_us
                    << "(us), usr_cpu=" << dt_usr_cpu_us
                    << "(us), sys_cpu=" << dt_sys_cpu_us << "(us)";
            }
            return oss.str();
        }

    public:        
        //used cpu time in microseconds
        int64_t dt_wall_us{0};
        int64_t dt_usr_cpu_us{0};
        int64_t dt_sys_cpu_us{0};

        int64_t  wall_time_t1_us{-1};
        int64_t  wall_time_t2_us{0};
#ifdef __linux__ 
        rusage   ru1{};
        rusage   ru2{};
#endif
    };
}

#endif
