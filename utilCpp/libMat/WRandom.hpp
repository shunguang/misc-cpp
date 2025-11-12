/*
 *------------------------------------------------------------------------
 * WRandom.h - a random number generator class
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: WRandom.cpp,v 1.2 2011/06/02 17:30:17 swu Exp $
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

// constructor:
template<class T>
WRandom<T>::WRandom( const unsigned long seed) {
  // Check that compiler supports 80-bit long double:
  //assert(sizeof(long double)>9);

  // initialize
  randSeed( seed );
}


// returns a random number between 0 and 1:
template<class T>
T WRandom<T>::randDouble()
{
  long double c;
  c = (long double)2111111111.0 * m_x[3] +
      1492.0 * (m_x[3] = m_x[2]) +
      1776.0 * (m_x[2] = m_x[1]) +
      5115.0 * (m_x[1] = m_x[0]) +
      m_x[4];
  m_x[4] = (T) floor(c);
  m_x[0] = c - m_x[4];
  m_x[4] = m_x[4] * (1./(65536.*65536.));
  return m_x[0];
}

// returns an integer random number in desired interval:
template<class T>
int WRandom<T>::randInteger(const int min, const int max)
{
  int iinterval = max - min + 1;
  if (iinterval <= 0) return 0x80000000; // error
  int i = int(iinterval * randDouble());     // truncate
  if (i >= iinterval) i = iinterval-1;
  return min + i;
}


// this function initializes the random number generator
template<class T>
void WRandom<T>::randSeed ( const unsigned long seed)
{
  int i;
  unsigned long s = seed;
  // make random numbers and put them into the buffer
  for (i=0; i<5; i++) {
    s = s * 29943829 - 1;
    m_x[i] = s * (1./(65536.*65536.));}

  // randomize some more
  for (i=0; i<19; i++) randDouble();

  m_gaussianTmp2 = -1 + 2 * randDouble();

}

//----------------------------------------------------------
// Gaussian distribution with sigma=1 and mean=0
// by Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122 
// [1] Knuth, D.E., 1981; The Art of Computer Programming, 
//     Volume 2 Seminumerical Algorithms, Addison-Wesley, Reading Mass., 688 pages, 
//     ISBN 0-201-03822-6
//----------------------------------------------------------
template<class T>
T WRandom<T> :: randGaussian()
{
  T r2;

  do{
      /* choose two random numbers in uniform square (-1,-1) to (+1,+1) */
      m_gaussianTmp1 = m_gaussianTmp2;
      m_gaussianTmp2 = -1.0 + 2.0 * randDouble();

      /* see if it is in the unit circle */
      r2 = m_gaussianTmp1 * m_gaussianTmp1 + m_gaussianTmp2 * m_gaussianTmp2;
  }while ( r2 >= 1.0 );

  /* Box-Muller transform */
  return m_gaussianTmp2 * sqrt (-2.0 * log (r2) / r2);
}


// Gaussian distribution with given sigma and mean
template<class T>
T WRandom<T> :: randGaussian(const T sigma, const T mean)
{
	return mean + sigma * randGaussian();
}

