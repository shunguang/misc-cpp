/*
 *------------------------------------------------------------------------
 * WMatTool2.h - untility functions used by all project
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatTools.h,v 1.1 2011/06/02 17:31:22 swu Exp $
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
#ifndef __WMAT_TOOLS4_H__
#define __WMAT_TOOLS4_H__
#include "WMat.h"   //double matrix computation

namespace app {
	template<class T>
	int   wmatCalNumElements(const T x1, const T dx, const T x2);

	template<class T>
	void  wmatGenVec(WMat<T>& v, const T x1, const T dx, const T x2 );

	template<class T>
	void  wmatPdistEuclidean(WMat<T>& out, const WMat<T>& x);

	template<class T>
	void  wmatRandperm(WMat<T>& out, const uint32_t& n );

	template<class T>
	void  wmatRepMat(WMat<T>& out, const WMat<T>& x, const uint32_t& m, const uint32_t& n);

	template<class T>
	void  wmatRound(WMat<T>& out,  const WMat<T>& x );

	// x: 1 x m, y: 1 x n, z:(m*n) x 2
	template<class T>
	void  wmatTwoTuples(WMat<T>& z, const WMat<T>& x, const WMat<T>& y );

	template <typename Iterator>
	bool next_combination(const Iterator first, Iterator k, const Iterator last);

	uint32_t wmat_n_choose_k(uint32_t n, uint32_t k);
	WMatI32  wmatUnique(const WMatI32& x);
	void   wmat_n_choose_k_pattern(WMatU32& out, const uint32_t n, const uint32_t k, const std::vector<uint32_t>& in);

  #include "WMatTools4.hpp"
}
#endif
