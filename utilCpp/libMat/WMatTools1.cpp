/*
 *------------------------------------------------------------------------
 * WMatTools1.hpp - untility functions part1
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatUtilities.cpp,v 1.9 2011/07/28 20:36:26 swu Exp $
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
#include "WMatTools1.h"
namespace app {
	uint32_t  wmatSumBool(const WMatBool& x)
	{
		//WMAT_CHECK_EMPTY(x, "wmatSum(): x is an emppty matrix!")

		const bool* px = x.data();
		uint32_t d = 0;
		for (uint32_t i = 0; i < x.size(); i++, px++) {
			d += uint32_t(*px);
		}
		return d;
	}

	WMatU32  wmatSumBool(const WMatBool& x, const int dimFlag)
	{
		//WMAT_CHECK_EMPTY(x, "wmatSum(): x is an emppty matrix!")

		uint32_t m = x.rows();
		uint32_t n = x.cols();


		if (dimFlag == 1) //return a row vector
		{
			WMatU32 y(1, n);
			for (uint32_t i = 0; i < n; i++) {
				y(i) = wmatSumBool(x.getCol(i));
			}
			return y;
		}
		else //return a col vector
		{
			WMatU32 y(m, 1);
			for (uint32_t i = 0; i < m; i++) {
				y(i) = wmatSumBool(x.getRow(i));
			}
			return y;
		}
	}
}