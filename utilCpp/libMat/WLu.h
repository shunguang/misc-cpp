/*
 *------------------------------------------------------------------------
 * swdLu.h - LU decomposition with interface of WMat<T>
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * $Id: WLu.h,v 1.1 2010/06/11 18:37:14 swu Exp $
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
#ifndef __WMAT_LU_H__
#define __WMAT_LU_H__

#include <math.h>
#include <conio.h>

#include "WMat.h"

namespace app {

  template<class T>
  class WLu {
  public:
    // Constructors/Destructors 
    WLu(const WMat<T>& A); // Factor A = L*U 
    ~WLu();

    // Accessors
    bool  isGood() const;
    bool  isFail() const;
    bool  isSingular() const;
    T det() const;   // return det(A)
    bool inv(WMat<T> &invA);
    void solve(WMat<T> &b); // Solve L*U*x = b to find x, the results is in b
  private:
    void initProb();
    void print(const T* a, const int n);
    void errorMsg(const char* msgStr);
    void ludcmp(T* a, const int n, int* indx, T& d);
    void lubksb(const T* a, const int n, const int* indx, T* b);

    WMat<T> m_LU;       //store the pivoted LU matrix
    uint32_t m_n; //the dimension of the matrix
    int  m_infoFlag;  //a flag to show if LU is fail or the input matrix is singular
    T m_d;        // =1, or -1 depending on whether the number of row interchnage
                      //is even or order, it is used to calculate the determinant value 

    int* m_indx;      //store the pivoting/permutation row index
    T* m_ptrLU;   //a pointer points to the data of m_LU matrix
    T* m_vv;      //a temporal vector used in inv() and ludcmp()  
  };
  typedef  WLu<float>   WLuF32;
  typedef  WLu<double>  WLuF64;
  
  //inverse & det global functions
  //2 x 2 inv 
  template<class T>
  T  wmatDet2x2(const WMat<T>& x); 
  template<class T>
  bool  wmatInv2x2(const WMat<T>& x, WMat<T>& xInv, T& xDet);
  template<class T>
  bool  wmatInv2x2(WMat<T>& x);

  //3 x 3 inv 
  template<class T>
  T  wmatDet3x3(const WMat<T>& x);

  template<class T>
  bool  wmatInv3x3(WMat<T>& x);

  template<class T>
  bool  wmatInv3x3(const WMat<T>& x, WMat<T>& xInv, T& det);

  template<class T>
  bool  wmatInvByLu(const WMat<T>& x, WMat<T>& xInv, T& det);

  template<class T>
  bool  wmatVerifyInv(const WMat<T>& x, const WMat<T>& y, const bool verbose=true);

  template<class T>
  bool  wmatEigen2x2(const WMat<T>& A, WMat<T>& vec, WMat<T>& val, const bool verbose = true);

  #include "WLu.hpp"
}
#endif

