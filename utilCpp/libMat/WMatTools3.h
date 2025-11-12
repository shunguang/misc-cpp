/*
 *------------------------------------------------------------------------
 * WMatTools1.h - untility functions used by all project
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatUtilities.h,v 1.9 2011/07/28 20:36:26 swu Exp $
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

//part 3 of the template global funcs, some advanced algorithms

#ifndef __WMAT_TOOLS3_H__
#define __WMAT_TOOLS3_H__

#include "WMat.h"  
#include "WMatTools1.h"  
#include "WMatTools2.h"  
#include "WMat.h"  
namespace  app {

	template<class T>
	T  wmatDotProduct(const WMat<T>& x, const WMat<T>& y);

	template<class T>
	T  wmatDotProduct(const T* p1, const T* p2, const uint32_t n);

	template<class T>
	T  wmatAngleBetweenTwoVectorsInRad(const WMat<T>& refVec, const WMat<T>& insVec);


	template<class T>
	WMat<T>  wmatCrossProduct(const WMat<T>& x, const WMat<T>& y);


	//outproduct distance between two vectors
	template<class T>
	WMat<T>  wmatOutProduct(const WMat<T>& v1, const WMat<T>& v2);

	//norm2 distance between two vectors
	template<class T>
	T  wmatDist2(const WMat<T>& v1, const WMat<T>& v2);

	template<class T>
	T  wmatDistMahalanobis(const WMat<T>& v1, const WMat<T>& v2, const WMat<T>& cov1, const WMat<T>& cov2);

	template<class T>
	T  wmatDistMahalanobis2(const WMat<T>& v1, const WMat<T>& v2, const WMat<T>& cov1, const WMat<T>& cov2);

	//find all the local peaks in vector x
	template<class T>
	void  wmatPeakFinder(WMat<T>& peakValues, WMatU32& peakIds, const WMat<T>& x);

	//find all the local peaks which values are greater or equal than max_peak_value * threshold.
	//where $0 <= threshold <= 1$
	template<class T>
	void  wmatPeakFinder(WMat<T>& peakValues, WMatU32& peakIds, const WMat<T>& x, const T threshold);

	//find SNR from a given peak
	template<class T>
	T  wmatSnr(uint32_t& nSignalBins, uint32_t& nTotalBins, const WMat<T>& x, const uint32_t peakId, const T coeffThd );
	
	template<class T>
	T  wmatCrossCorrelation(WMat<T>& vCorr, const WMat<T>& vSignal, const WMat<T>& vKernel);

	template<class T>
	T  wmatCircularCorrelation(const WMat<T>& h1, const WMat<T>& h2);


	//--------------------------------------
//h1[2], heading and its variance for the 1st
//h2[2], heading and its variance for the 2nd
//return heading dist in radus
//--------------------------------------
  template<class T>
	T  HeadingFromVel(const T vx, const T vy);

	template<class T>
	T  wmatHeadingDist(const T h1_rad, const T h2_rad);

	template<class T>
	T  wmatHeadingDist(const T* h1_rad, const T* h2_rad);

	template<class T>
	void  wmatSort(WMat<T>& y, WMatU32& I, const WMat<T>& x, const bool isDescending = false);

	#include "WMatTools3.hpp"
}
#endif
