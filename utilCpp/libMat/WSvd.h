/*
 *------------------------------------------------------------------------
 * WSvd.h - SVD decomposition with interface of swdMat.
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: WSvd.h,v 1.1 2010/06/11 18:37:14 swu Exp $
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

#ifndef __SWD_MAT_SVD_H__
#define __SWD_MAT_SVD_H__

#include <math.h>
#include <conio.h>

#include "WMat.h"

#define WSVD_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define WSVD_SIGN(a,b)   (((b) >= 0.0) ? fabs(a) : -fabs(a))
namespace app{
	template<class T>
	class  WSvd {
	public:
		// Constructors/Destructors 
		WSvd(const WMat<T> &X); // Factor X = U*S*V', X.rows() >= X.cols();
		~WSvd();

		// Accessors
		WMat<T> getU() { return m_U.getSlice(1, 1, m_m - 1, m_n - 1); }
		WMat<T> getS() { return m_S.getSlice(1, 1, m_n - 1, m_n - 1); }
		WMat<T> getV() { return m_V.getSlice(1, 1, m_n - 1, m_n - 1); }
		int   rank(const T tol) const;

	private:
		void svdcmp(T** a, int m, int n, T* w, T** v);
		T    PYTHAG(const T a, const T b);
		void  print(T** a, const int m, const int n);


		unsigned int m_m; // = X.rows()+1
		unsigned int m_n; // = X.cols()+1
		WMat<T> m_U;       //store U, m_m x m_n
		WMat<T> m_S;       //store S, m_n x m_n
		WMat<T> m_V;       //store V, m_n x m_n

		T** m_pptrU;   //a pointer array save the each row's address of m_U
		T** m_pptrV;   //a pointer array save the each row's address of m_V
		T* m_ptrW;    //save the diagnol of m_S
	};

	//x = U*S*V'
	template<class T>
	bool  wmatSvd2x2(const WMat<T>& x, WMat<T>& U, WMat<T>& S, WMat<T>& V);

	#include "WSvd.hpp"
}
#endif

