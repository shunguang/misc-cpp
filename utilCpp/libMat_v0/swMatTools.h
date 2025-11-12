/*
 *------------------------------------------------------------------------
 * swdMatUtilities.h - untility functions used by all project
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: swMatTools.h,v 1.1 2011/06/02 17:31:22 swu Exp $
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
#ifndef SW_MAT_TOOLS
#define SW_MAT_TOOLS

#include <assert.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>

#include "swMatDefs.h"
#include "swfMat.h"   //double matrix computation
#include "swdMat.h"   //double matrix computation
#include "swiMat.h"   //int matrix computation
#include "swMatUtilities.h"   //int matrix computation

//out(i,j) = min( x(i,j), y(i,j) )
bool SWMAT_EXPORT swMatMin( const swiMat &x, const swiMat &y, swiMat &out); 
bool SWMAT_EXPORT swMatMin( const swiMat &x, const swiMat &y, swdMat &out); 

//out = x(:, vIdx), $logical$ - of $vIdx$ is logical
bool SWMAT_EXPORT swMatGetCols( const swiMat &x, const swiMat &vIdx, const bool &logical, swiMat &out );
bool SWMAT_EXPORT swMatGetCols( const swfMat &x, const swiMat &vIdx, const bool &logical, swfMat &out );
bool SWMAT_EXPORT swMatGetCols( const swdMat &x, const swiMat &vIdx, const bool &logical, swdMat &out );
//out = x(vIdx, :), $logical$ - of $vIdx$ is logical
bool SWMAT_EXPORT swMatGetRows( const swiMat &x, const swiMat &vIdx,  const bool &logical, swiMat &out );
bool SWMAT_EXPORT swMatGetRows( const swfMat &x, const swiMat &vIdx,  const bool &logical, swfMat &out );
bool SWMAT_EXPORT swMatGetRows( const swdMat &x, const swiMat &vIdx,  const bool &logical, swdMat &out );

//out = x > thd,  $out$ is a logical matrxi with the same size of $x$
int SWMAT_EXPORT swMatLogicGT( const swfMat &x, const float &thd, swiMat &out );
int SWMAT_EXPORT swMatLogicGT( const swdMat &x, const double &thd, swiMat &out );
//out = x < thd,  $out$ is a logical matrxi with the same size of $x$
int SWMAT_EXPORT swMatLogicGE( const swfMat &x, const float &thd, swiMat &out );
int SWMAT_EXPORT swMatLogicGE( const swdMat &x, const double &thd, swiMat &out );
//out = x >= thd, $out$ is a logical matrxi with the same size of $x$
int SWMAT_EXPORT swMatLogicST( const swfMat &x, const float &thd, swiMat &out );
int SWMAT_EXPORT swMatLogicST( const swdMat &x, const double &thd, swiMat &out );
//out = x <= thd, $out$ is a logical matrxi with the same size of $x$
int SWMAT_EXPORT swMatLogicSE( const swfMat &x, const float &thd, swiMat &out );
int SWMAT_EXPORT swMatLogicSE( const swdMat &x, const double &thd, swiMat &out );

//out = x == thd, $out$ is a logical matrxi with the same size of $x$
int SWMAT_EXPORT swMatLogicEQ( const swdMat &x, const double &thd, swiMat &out );
int SWMAT_EXPORT swMatLogicEQ( const swiMat &x, const int &thd, swiMat &out );

//out = x & y, $out$, $x$, and $y$ all are the same size
int SWMAT_EXPORT swMatLogicAND( const swiMat &x, const swiMat &y, swiMat &out );
//out = x | y, $out$, $x$, and $y$ all are the same size
int SWMAT_EXPORT swMatLogicOR ( const swiMat &x, const swiMat &y, swiMat &out );
//out = all(x,1), out: 1 by x.cols()
//out = all(x,2), out: x.rows() by 1
int SWMAT_EXPORT swMatLogicAll( const swiMat &x, const int &dimFlag, swiMat &out );

//return true if any x(i,j) > thd
bool SWMAT_EXPORT swMatLogicAnyGT( const swdMat &x, const double &thd);


int  SWMAT_EXPORT swMatCalNumElements( const float &x1, const float &dx, const float &x2 );
int  SWMAT_EXPORT swMatCalNumElements( const double &x1, const double &dx, const double &x2 );
void SWMAT_EXPORT swMatGenVec( const float &x1, const float &dx, const float &x2, swfMat &v );
void SWMAT_EXPORT swMatGenVec( const double &x1, const double &dx, const double &x2, swdMat &v );
void SWMAT_EXPORT swMatGenVec( const double &x1, const double &dx, const double &x2, swiMat &v );

void SWMAT_EXPORT  swMatPdistEuclidean( const swdMat &x, swdMat &d );

void SWMAT_EXPORT swMatRandperm( const size_t &n, swiMat &x );
void SWMAT_EXPORT swMatRepMat( const swfMat &x, const unsigned int &m, const unsigned int &n, swfMat &y);
void SWMAT_EXPORT swMatRepMat( const swdMat &x, const unsigned int &m, const unsigned int &n, swdMat &y);
void SWMAT_EXPORT swMatRound( const swfMat &x, swiMat &y );
void SWMAT_EXPORT swMatRound( const swdMat &x, swiMat &y );

// x: 1 x m, y: 1 x n, z:  (m*n) x 2
void SWMAT_EXPORT swMatTwoTuples( const swiMat &x, const swiMat &y, swiMat &z );
void SWMAT_EXPORT swMatTwoTuples( const swfMat &x, const swfMat &y, swfMat &z );
void SWMAT_EXPORT swMatTwoTuples( const swdMat &x, const swdMat &y, swdMat &z );

swiMat SWMAT_EXPORT swMatUnique( const swiMat &x );

void SWMAT_EXPORT my_text_writer2 ( const swdMat &x, const std::string &path, const std::string &fname_wo_ext, const std::string &fmt, const int &fn, const int &idx=-1 );
void SWMAT_EXPORT my_text_writer2 ( const swiMat &x, const std::string &path, const std::string &fname_wo_ext, const std::string &fmt, const int &fn, const int &idx=-1 );

void SWMAT_EXPORT swMatD2I( const swdMat &x, swiMat &y );

template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last);
size_t SWMAT_EXPORT swMat_n_choose_k( size_t n, size_t k );
void  SWMAT_EXPORT swMat_n_choose_k_pattern( const size_t n, const size_t k, const std::vector<size_t> &in, swiMat &out );

#endif