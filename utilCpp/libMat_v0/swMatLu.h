/*
 *------------------------------------------------------------------------
 * swdLu.h - LU decomposition with interface of swdMat or swfMat.
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * $Id: swMatLu.h,v 1.1 2010/06/11 18:37:14 swu Exp $
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
#ifndef SWD_MAT_LU_H
#define SWD_MAT_LU_H

#include <math.h>
#include <conio.h>

#include "swMatDefs.h"
#include "swdMat.h"
#include "swfMat.h"

class SWMAT_EXPORT swMatLu {
public:
    // Constructors/Destructors 
    swMatLu( const swdMat &A ); // Factor A = L*U 
    swMatLu( const swfMat &A ); // Factor A = L*U 
    ~swMatLu(); 

    // Accessors
    bool  isGood() const;
    bool  isFail() const;
    bool  isSingular() const;
    double det() const;   // return det(A)
	bool inv( swdMat &invA );
	bool inv( swfMat &invA );
    void  solve( swdMat& b); // Solve L*U*x = b to find x, the results is in b
    void  solve( swfMat& b); // Solve L*U*x = b to find x, the results is in b
private:
	void initProb();
	void print( const double *a, const int n);
	void errorMsg( const char *msgStr);
	void ludcmp(double* a, const int n, int *indx, double& d);
	void lubksb( const double* a, const int n, const int* indx, double* b );

    swdMat m_LU;       //store the pivoted LU matrix
	unsigned int m_n; //the dimension of the matrix
    int  m_infoFlag;  //a flag to show if LU is fail or the input matrix is singular
	double m_d;        // =1, or -1 depending on whether the number of row interchnage
	                  //is even or order, it is used to calculate the determinant value 

    int* m_indx;      //store the pivoting/permutation row index
	double *m_ptrLU;   //a pointer points to the data of m_LU matrix
	double* m_vv;      //a temporal vector used in inv() and ludcmp()  
};
#endif

