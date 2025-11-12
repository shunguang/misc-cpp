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

#ifndef __TP8_LIN_SPACE_H__
#define __TP8_LIN_SPACE_H__

#include "AppDefs.h"
namespace app {
	template <typename T>
	class LinSpace {
	public:
		LinSpace(const T x1_, const T x2_, const size_t n = 100, const std::string& unit_ = "unkn")
			: x1(x1_)
			, x2(x2_)
			, size(n)
			, unit(unit_)
		{
			dx = (n > 1) ? ((x2 - x1) / (n - 1)) : (x2 - x1);
			T tmp = x1;
			for (size_t i = 0; i < n; ++i) {
				x.push_back(tmp);
				tmp += dx;
			}
		}

		//the class take responbility to check boundaries
		T getValue(const size_t idx) const {
			return x[idx];
		}

		//the class take responbility to check boundaries
		int32_t getIdx(const T val) const {
			int32_t idx = (int32_t)((val - x1) / dx);
			return idx;
		}

	public:
		T x1{ 0 };
		T x2{ 1 };
		T dx{ 1 };
		size_t size;
		std::vector<T> x;            //x[i] = x1 + dx (i=0,1,...,n-1)
		std::string unit{ "unkn" };
	};

	typedef std::shared_ptr<LinSpace<uint8_t>>	LinSpaceU8Ptr;
	typedef std::shared_ptr<LinSpace<float>>	  LinSpaceF32Ptr;
	typedef std::shared_ptr<LinSpace<double>>   LinSpaceF64Ptr;
}
#endif

