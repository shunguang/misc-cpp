/*
 *------------------------------------------------------------------------
 * swdMatSvd.h - SVD decomposition with interface of swdMat.
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: swdMatSvd.h,v 1.1 2010/06/11 18:37:14 swu Exp $
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

#ifndef SWD_MAT_SVD_H
#define SWD_MAT_SVD_H

#include <math.h>
#include <conio.h>

#include "swMatDefs.h"
#include "swdMat.h"

class SWMAT_EXPORT swdMatSvd {
public:
    // Constructors/Destructors 
    swdMatSvd(const swdMat &X ); // Factor X = U*S*V', X.rows() >= X.cols();
    ~swdMatSvd(); 

    // Accessors
	swdMat getU() { return m_U.getSlice(1, 1, m_m-1, m_n-1); }
	swdMat getS() { return m_S.getSlice(1, 1, m_n-1, m_n-1); }
	swdMat getV() { return m_V.getSlice(1, 1, m_n-1, m_n-1); }
    int   rank( const double tol ) const;
    int   rank() const;
private:
	void errMsg( const char *msgStr);
	void svdcmp(double **a, int m, int n, double *w, double **v);
	double PYTHAG( const double a, const double b );
	double MAX   ( const double a, const double b );
	double SIGN  ( const double a, const double b );
	void  print( double **a, const int m, const int n);


	unsigned int m_m; // = X.rows()+1
	unsigned int m_n; // = X.cols()+1
    swdMat m_U;       //store U, m_m x m_n
	swdMat m_S;       //store S, m_n x m_n
	swdMat m_V;       //store V, m_n x m_n

	double** m_pptrU;   //a pointer array save the each row's address of m_U
	double** m_pptrV;   //a pointer array save the each row's address of m_V
	double*  m_ptrW;    //save the diagnol of m_S
};
#endif

