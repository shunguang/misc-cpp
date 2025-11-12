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

#ifndef _APP_RNG_H_
#define _APP_RNG_H_

#include "AppDefs.h"
namespace app {
	template<class T>
	struct AppRng {
		//
		//define a rang in [x1,x2] with total <nPoints>
		//can call linspace(rng.x1,rng.x2, rng.nPoints) to generate the 1d array
		//
		AppRng(const T x1_, const T x2_, const int32_t nPoints_, const std::string unit_="")
			: x1(x1_), x2(x2_), nPoints(nPoints_), unit(unit_)
		{
			dx = (nPoints > 1) ? ((x2 - x1) / (nPoints - 1)) : (x2 - x1);
		}

		T getValue(const int32_t idx) const {
			return (x1 + dx * idx);
		}

		int32_t getIdx(const T val) const {
			int32_t idx = (int32_t) ((val - x1) / dx);
			return idx;
		}

		//return true if <val> inside the border, otherwise false
		bool get3Pts(float v[3], const T val) const {
#if _DEBUG
			appAssert(val >= x1 && val <= x2, "AppRng::getIdxAndCheckOnBorder(): val is out of range");
#endif
			int32_t idx = getIdx( val );
			if ( idx < 1 || idx >= (nPoints - 1) ) {
				v[0] = val;
				v[1] = val;
				v[2] = val;
				return false;
			}

			v[0] = getValue(idx - 1);
			v[1] = val;
			v[2] = getValue(idx + 1);
			return true;
		}

		std::string toString(const std::string& msg = "")  const
		{
			std::ostringstream oss;
			if (1 == sizeof(x1)) {
				oss << msg << "x1=" << (int)x1 << unit << ",x2=" << (int)x2 << unit << ", nPoints=" << nPoints;
			}
			else{
			oss << msg << "x1=" << x1 << unit << ",x2=" << x2 << unit << ", nPoints=" << nPoints;
			}

			return oss.str();
		}

		//------------------------------------
		T				x1{ 0 };
		T				x2{ 10 };
		T				dx{ 1 };
		int32_t	nPoints{ 11 };
		std::string unit{ "" };
	};
	typedef std::shared_ptr<AppRng<float>>	AppRngF32Ptr;
	typedef std::shared_ptr<AppRng<double>> AppRngF64Ptr;
}
#endif

