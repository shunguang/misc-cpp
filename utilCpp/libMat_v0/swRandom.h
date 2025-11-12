/*
 *------------------------------------------------------------------------
 * swRandom.h - a random number generator class
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: swRandom.h,v 1.1 2010/06/11 18:37:15 swu Exp $
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
#ifndef SW_RAND_INCLUDE
#define SW_RAND_INCLUDE
#include <math.h>
#include <conio.h>
#include <iostream>
#include "swMatDefs.h"

class SWMAT_EXPORT swRandom{                                       // a random number generator
public:
	swRandom( const unsigned long seed=4096 );        // constructor
	void   randSeed( const unsigned long seed);       // set seed to reinitialization
	int    randInteger(const int min, const int max); // get integer uniform distributed random number in desired interval
	double randDouble();                              // get floating point uniform distributed random number
	double randGaussian();                            // Gaussian distribution with sigma=1 and mean=0
	double randGaussian(const double sigma, const double mean=0); // Gaussian distribution with given sigma and mean

protected:
	double m_x[5];                                    // history buffer

private:
	double m_gaussianTmp1;  //temporal uniform random # for generator a gaussian rand #
	double m_gaussianTmp2;  //temporal uniform random # for generator a gaussian rand #
};

#endif
