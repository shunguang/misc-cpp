/*
 *------------------------------------------------------------------------
 * abc.h
 *
 * No  any Corporation or client funds were used to develop this code.
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * Copyright (c) 2022 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

#ifndef __HEALTHY_MONITOR_H__
#define __HEALTHY_MONITOR_H__
#include "AppDefs.h"
namespace app {
	struct HealthyMonitor {
		HealthyMonitor() {
			reset();
		}
		void update(const float dt) {
			cnt++;
			t_min = APP_MIN(dt, t_min);
			t_max = APP_MAX(dt, t_max);
			t_mean += ((float)dt - t_mean) / ((float)cnt);

			if (reset_cnt == 0 && cnt == 1) {
				reset_minMax = t_max;
				reset_maxMax = t_max;
				reset_meanMax = t_max;
			}
		}

		void reset() {
			if (cnt > 0) {
				reset_cnt++;
				reset_minMax = APP_MIN(t_max, reset_minMax);
				reset_maxMax = APP_MAX(t_max, reset_maxMax);
				reset_meanMax += (t_max - reset_meanMax) / (float)reset_cnt;
			}

			t_min = std::numeric_limits<float>::max();
			t_max = std::numeric_limits<float>::min();
			t_mean = 0;
			cnt = 0;

		}

		std::string to_string(const std::string& msg = "", const std::string& tag = "") {
			char buf[1024];
			snprintf(buf, 1024, "(%s_min=%.1f, max=%.1f, mean=%.1f, cnt=%u), reset: (minMax=%.1f,maxMax=%.1f,meanMax=%.1f,reset_cnt=%u)",
				tag.c_str(), t_min, t_max, t_mean, cnt, reset_minMax, reset_maxMax, reset_meanMax, reset_cnt);

			return msg + std::string(buf);
		}

		//inside reset
		float  t_min{ std::numeric_limits<float>::max() };
		float  t_max{ std::numeric_limits<float>::min() };
		float  t_mean{ 0 };
		uint32_t cnt{ 0 };

		//acrossing reset
		uint32_t reset_cnt{ 0 };
		float  reset_minMax{ std::numeric_limits<float>::max() };
		float  reset_maxMax{ std::numeric_limits<float>::min() };
		float  reset_meanMax{ 0 };
	};
}
#endif
