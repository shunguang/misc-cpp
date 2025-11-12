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
#ifndef __WMAT_TOOLS2_H__
#define __WMAT_TOOLS2_H__
#include "WMat.h"         
#include "WMatTools1.h"   

namespace app {
	//out(i,j) = min( x(i,j), y(i,j) ), for i \in [0,m), j \in [0, n)
	template<class T>
	bool wmatMin(WMat<T>& out, const WMat<T>& x, const WMat<T>& y);

	//out(i,j) = max( x(i,j), y(i,j) ), for i \in [0,m), j \in [0, n)
	template<class T>
	bool wmatMax(WMat<T>& out, const WMat<T>& x, const WMat<T>& y);

	//out = x(:, vIdx), $vIdx$ is logical true or false
	template<class T>
	bool  wmatGetCols(WMat<T>& out, const WMat<T>& x, const WMatBool& vIdx);

	//out = [x(:, vIdx(1)), ..., x(:, vIdx(end))], $vIdx$ is index Ids
	template<class T>
	bool  wmatGetCols(WMat<T>& out, const WMat<T>& x, const WMatU32& vIdx);

	//out = x(vIdx, :), $vIdx$ is logical
	template<class T>
	bool  wmatGetRows(WMat<T>& out, const WMat<T>& x, const WMatBool& vIdx);

	//out = [x(vIdx(1), :); ...; x(vIdx(end), :)], $vIdx$ is the indices of rows
	template<class T>
	bool  wmatGetRows(WMat<T>& out, const WMat<T>& x, const WMatU32& vIdx);

	//return true if any x(i,j) > thd
	template<class T>
	bool  wmatLogicAnyGT(const WMat<T>& x, const T thd);


	//out = x > thd,  $out$ is a logical matrxi with the same size of $x$
	template<class T>
	uint32_t  wmatLogicGT(WMatBool& out, const WMat<T>& x, const T thd);
	//out = x < thd,  $out$ is a logical matrxi with the same size of $x$
	template<class T>
	uint32_t  wmatLogicGE(WMatBool& out, const WMat<T>& x, const T thd);
	//out = x >= thd, $out$ is a logical matrxi with the same size of $x$
	template<class T>
	uint32_t  wmatLogicST(WMatBool& out, const WMat<T>& x, const T thd);
	//out = x <= thd, $out$ is a logical matrxi with the same size of $x$
	template<class T>
	uint32_t  wmatLogicSE(WMatBool& out, const WMat<T>& x, const T thd);

	//out = x == thd, $out$ is a logical matrxi with the same size of $x$
	template<class T>
	uint32_t  wmatLogicEQ(WMatBool& out, const WMat<T>& x, const T thd, const T eps);
	
	//
	//none tempalte funcs
	// 	
	//out = x & y, $out$, $x$, and $y$ all are the same size
	uint32_t  wmatLogicAND(WMatBool& out, const WMatBool& x, const WMatBool& y);

	//out = x | y, $out$, $x$, and $y$ all are the same size
	uint32_t  wmatLogicOR(WMatBool& out, const WMatBool& x, const WMatBool& y);

	// if a col or row is all true
	// [m,n]=size(x);
	// cnt = wmatLogicAll(out,x,1),  out: 1 x n
	// cnt = wmatLogicAll(out,x,2),  out: m x 1
	uint32_t  wmatLogicAll(WMatBool& out, const WMatBool& x, const int& dimFlag);

	// if a col or row has any true
	uint32_t  wmatLogicAny(WMatBool& out, const WMatBool& x, const int& dimFlag);

#include "WMatTools2.hpp"
}
#endif
