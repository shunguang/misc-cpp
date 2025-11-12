/*
 *------------------------------------------------------------------------
 * WMat<T>Utilities.cpp - untility functions used by all project
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatTools.cpp,v 1.2 2011/06/23 18:41:20 swu Exp $
 * Copyright (c) 2005 Shunguang Wu
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
#include "WMatTools2.h"
namespace app{
	uint32_t wmatLogicAND(WMatBool& out, const WMatBool& x, const WMatBool& y)
	{
		//will return false (0) if has size issues
		WMAT_CHECK_DIM_EQ(x, y, "wmatLogicAND(): the dimenstion of x and y are not equal")

		const uint32_t m = x.rows();
		const uint32_t n = x.cols();
		out.resize(m, n);

		uint32_t cnt = 0;
		const bool* px = x.data();
		const bool* py = y.data();
		bool* pOut = out.getBuf();
		for (uint32_t i = 0; i < x.size(); ++i, ++px, ++py, ++pOut) {
			if (*px != 0 && *py != 0) {
				*pOut = true;
				++cnt;
			}
			else {
				*pOut = false;
			}
		}
		return cnt;
	}

	uint32_t wmatLogicOR(WMatBool& out, const WMatBool& x, const WMatBool& y)
	{
		//will return false (0) if has size issues
		WMAT_CHECK_DIM_EQ(x, y, "wmatLogicOR(): the dimenstion of x and y are not equal");
		
		const uint32_t m = x.rows();
		const uint32_t n = x.cols();
		out.resize(m, n);

		uint32_t cnt = 0;
		const bool* px = x.data();
		const bool* py = y.data();
		bool* pOut = out.getBuf();
		for (uint32_t i = 0; i < x.size(); ++i, ++px, ++py, ++pOut) {
			if (*px != 0 || *py != 0) {
				*pOut = true;
				++cnt;
			}
			else {
				*pOut = false;
			}
		}
		return cnt;
	}

//if a col or row is all true
//x: m x n, logic matrix
//dimFlag: 1 or 2
//out: 1 x n (if dimFlag==1) or  m x 1 (dimFlag==2) logic vector
	uint32_t wmatLogicAll(WMatBool& out, const WMatBool& x, const int& dimFlag)
	{
		uint32_t cnt = 0;
		const uint32_t m = x.rows();
		const uint32_t n = x.cols();
		WMatU32 y = wmatSumBool(x, dimFlag);

		const uint32_t* py = y.data();
		if (dimFlag == 1) {
			out.resize(1, n, false);
			bool* pOut = out.getBuf();
			for (uint32_t i = 0; i < n; ++i, ++py, ++pOut) {
				if (*py == m) {
					*pOut = true;
					++cnt;
				}
			}
		}
		else if (dimFlag == 2) {
			out.resize(m, 1, false);
			bool* pOut = out.getBuf();
			for (uint32_t i = 0; i < m; ++i, ++py, ++pOut) {
				if (*py == n) {
					*pOut = true;
					++cnt;
				}
			}
		}
		else {
			assert(0);
		}

		return cnt;
	}

	//if a col or row is all true
	//x: m x n, logic matrix
	//dimFlag: 1 or 2
	//out: 1 x n (if dimFlag==1) or  m x 1 (dimFlag==2) logic vector
	uint32_t wmatLogicAny(WMatBool& out, const WMatBool& x, const int& dimFlag)
	{
		uint32_t cnt = 0;
		const uint32_t m = x.rows();
		const uint32_t n = x.cols();
		WMatU32 y = wmatSumBool(x, dimFlag);

		const uint32_t* py = y.data();
		if (dimFlag == 1) {
			out.resize(1, n, false);
			bool* pOut = out.getBuf();
			for (uint32_t i = 0; i < n; ++i, ++py, ++pOut) {
				if (*py > 0) {
					*pOut = true;
					++cnt;
				}
			}
		}
		else if (dimFlag == 2) {
			out.resize(m, 1, false);
			bool* pOut = out.getBuf();
			for (uint32_t i = 0; i < m; ++i, ++py, ++pOut) {
				if (*py > 0) {
					*pOut = true;
					++cnt;
				}
			}
		}
		else {
			assert(0);
		}

		return cnt;
	}

}