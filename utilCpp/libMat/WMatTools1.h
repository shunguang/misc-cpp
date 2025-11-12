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

 //part 1 of the template global funcs

#ifndef __WMAT_TOOLS1_H__
#define __WMAT_TOOLS1_H__

#include "WMat.h"  
#include "WLu.h"
namespace  app {
	template<class T>
	bool  wmatIsEmpty(const WMat<T>& x);
	template<class T>
	WMat<T>  wmatZeros(const uint32_t m, const uint32_t n);
	template<class T>
	WMat<T>  wmatEye(const uint32_t m);
	template<class T>
	WMat<T>  wmatOnes(const uint32_t m, const uint32_t n);
	template<class T>
	WMat<T>  wmatReshape(const WMat<T>& x, const uint32_t m, const uint32_t n);
	template<class T>
	void  wmatReshape(const WMat<T>& x, WMat<T>& y);

	//x, m x n, matrix
	//return the idxs satisfying the condition
	template<class T>
	WMatU32  wmatFindIdxGT(const WMat<T>& x, const T threshhold);
	template<class T>
	WMatU32  wmatFindIdxGE(const WMat<T>& x, const T threshhold);
	template<class T>
	WMatU32  wmatFindIdxST(const WMat<T>& x, const T threshhold);
	template<class T>
	WMatU32  wmatFindIdxSE(const WMat<T>& x, const T threshhold);
	template<class T>
	WMatU32  wmatFindIdxEQ(const WMat<T>& x, const T threshhold);


	//---------------------------------------------
	// max,min,sum, ect for all elements
	//----------------------------------------------
	template<class T>
	T  wmatMax(const WMat<T>& x);
	template<class T>
	T  wmatMax(const WMat<T>& x, uint32_t* idx);
	template<class T>
	T  wmatMin(const  WMat<T>& x);
	template<class T>
	T  wmatMin(const  WMat<T>& x, uint32_t* idx);
	template<class T>
	T  wmatMean(const WMat<T>& x);
	template<class T>
	T  wmatMedian(const WMat<T>& x);
	template<class T>
	T  wmatNorm(const WMat<T>& x);
	template<class T>
	bool  wmatMeanStd(const WMat<T>& x, T& mean, T& std);

	template<class T>
	T  wmatSum(const WMat<T>& x);

	template<>
	bool  wmatSum(const WMat<bool>& x) = delete;

	uint32_t  wmatSumBool(const WMatBool& x);


	//----------------------------------------------
	// max,min,sum, ect along the dimension
	// ex:
	//x=rand(2,3)
	//sum(x,1)=[1.7205    1.0404    0.7299]
	//sum(x,2)=[1.57,1.91]^T
	//----------------------------------------------
	template<class T>
	WMat<T>  wmatMax(const WMat<T>& x, const int dimFlag);
	template<class T>
	WMat<T>  wmatMean(const WMat<T>& x, const int dimFlag);
	template<class T>
	WMat<T>  wmatMin(const WMat<T>& x, const int dimFlag);
	template<class T>
	WMat<T>  wmatMedian(const WMat<T>& x, const int dimFlag);
	template<class T>
	WMat<T>  wmatSum(const WMat<T>& x, const int dimFlag);

	template<>
	WMat<bool>  wmatMax(const WMat<bool>& x, const int dimFlag) = delete;
	template<>
	WMat<bool>  wmatMean(const WMat<bool>& x, const int dimFlag) = delete;
	template<>
	WMat<bool>  wmatMin(const WMat<bool>& x, const int dimFlag) = delete;
	template<>
	WMat<bool>  wmatMedian(const WMat<bool>& x, const int dimFlag) = delete;
	template<>
	WMat<bool>  wmatSum(const WMat<bool>& x, const int dimFlag) = delete;

	WMatU32  wmatSumBool(const WMatBool& x, const int dimFlag);


	template<class T>
	WMat<T>  wmatAbs(const WMat<T>& x);
	template<class T>
	WMat<T>  wmatSin(const WMat<T>& x);
	template<class T>
	WMat<T>  wmatCos(const WMat<T>& x);
	template<class T>
	WMat<T>  wmatTan(const WMat<T>& x);
	template<class T>
	WMat<T>  wmatExp(WMat<T>& x);

	template<>
	WMat<bool>  wmatAbs(const WMat<bool>& x) = delete;
	template<>
	WMat<bool>  wmatSin(const WMat<bool>& x) = delete;
	template<>
	WMat<uint8_t>  wmatSin(const WMat<uint8_t>& x) = delete;
	template<>
	WMat<int8_t>  wmatSin(const WMat<int8_t>& x) = delete;
	template<>
	WMat<uint16_t>  wmatSin(const WMat<uint16_t>& x) = delete;
	//add more ..

	template<class T>
	void  wMatDump(const WMat<T>& x, const std::string& path, const std::string& fname_wo_ext, const std::string& fmt, const int& fn, const int& idx = -1);

	//the next are private funcs used in this catogray 
	template<class T>
	WMat<T>  wmatSelectCol_private(const WMat<T>& x, const int colIdx, const T checkVal, const char* flag);
	template<class T>
	WMat<T>  wmatSelectRow_private(const WMat<T>& x, const int rowIdx, const T checkVal, const char* flag);
	template<class T>
	WMatU32 wmatSelectEleIdx_private(const WMat<T>& x, const T checkVal, const char* flag);

	#include "WMatTools1.hpp"
}
#endif


