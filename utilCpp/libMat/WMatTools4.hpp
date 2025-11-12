/*
 *------------------------------------------------------------------------
 * WMat<T>Utilities.cpp - untility functions used by all project
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatTools.cpp,v 1.2 2011/06/23 18:41:20 swu Exp $
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
template<class T>
void wmatRandperm(WMat<T>& x, const uint32_t&n  )
{
#if _DEBUG
	assert( x.size() >= n );
#endif

	uint32_t i, j, t;
	
	for (i = 0; i < n; i++) {
		x(i) = (T)i;
	}

	for(i=0; i<n; i++) {
		j = rand()%(n-i)+i;
		t = (uint32_t)x(j);
		x(j) = x(i);
		x(i) = (T)t;
	}
}

template<class T>
void wmatRepMat( WMat<T>& y, const WMat<T> &x, const uint32_t &m, const uint32_t &n)
{	
	uint32_t mx = x.rows();
	uint32_t nx = x.cols();

	y.resize( m*mx, n*nx );

	uint32_t i,j, r0, c0, r1, c1;
	for ( i=0, r0=0, r1=mx-1; i<m; ++i, r0+=mx, r1+=mx ){
		for ( j=0, c0=0, c1=nx-1; j<n; ++j, c0+=nx,c1+=nx ){
			y.setSlice(r0, r1, c0, c1, x);
		}
	}
}

//rounds the elements of x to the nearest integers
template<class T>
void wmatRound(WMat<T>& y, const WMat<T> &x  )
{
	if (y.size() != x.size()) {
		y.resize(x.rows(), x.cols());
	}
	for( uint32_t i=0; i<x.size(); ++i){
		y(i) = WMAT_ROUND( x(i) );
	}
}


template<class T>
int   wmatCalNumElements( const T x1, const T dx, const T x2 )
{
	int n = (int) ( floor( (x2-x1)/dx ) + 1 );
	
	assert( (x1 + (n-1)*dx) <= x2 ) ;

	return n;
}

template<class T>
void  wmatGenVec(WMat<T>& v, const T x1, const T dx, const T x2 )
{
	int n = wmatCalNumElements(x1,dx,x2);
	v.resize(n,1);
	T x = x1;
	for( uint32_t i=0; i<n; ++i ){
		v(i) = x;
		x += dx;
	}
}


template<class T>
void wmatTwoTuples(WMat<T>& z, const WMat<T> &x, const WMat<T> &y )
{
	uint32_t nx = x.size();
	uint32_t ny = y.size();
	uint32_t nz = nx*ny;
	uint32_t two = 2;

	z.resize( nz, two );
	uint32_t k=0;
	const T *px = x.data();
	for( uint32_t i=0; i<nx; ++i, ++px){
		const T *py = y.data();
		for( uint32_t j=0; j<ny; ++j, ++py){
			z(k,0) = *px;
			z(k,1) = *py;
			k++;
		}
	}
	assert( k==nz );
}



//x: n x dim;
//d: 1 x (n*n-1)/2)
template<class T>
void wmatPdistEuclidean(WMat<T>& d, const WMat<T> &x  )
{
	uint32_t n = x.rows();
	uint32_t m = n*(n-1)/2;
	uint32_t dim = x.cols();

	d.resize(1, m);

	uint32_t k=0;
	T s, dx;
	const T *pi = x.data();
	for(uint32_t i=0; i<n; ++i, pi += dim ){

		const T *pj = pi+dim;
		for(uint32_t j=i+1; j<n; ++j, pj+=dim ){
			
			s=0;
			for(uint32_t ii=0; ii<dim; ++ii ){
				dx = pi[ii] - pj[ii];
				s += (dx*dx);
			}

			d(k) = sqrt(s);
			k++;
		}
	}


}

template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
   /* Credits: Thomas Draper */
   if ((first == last) || (first == k) || (last == k)){
      return false;
   }

   Iterator itr1 = first;
   Iterator itr2 = last;
   ++itr1;
   if (last == itr1)
      return false;
   itr1 = last;
   --itr1;
   itr1 = k;
   --itr2;
   while (first != itr1)
   {
      if (*--itr1 < *itr2)
      {
         Iterator j = k;
         while (!(*itr1 < *j)) ++j;
         std::iter_swap(itr1,j);
         ++itr1;
         ++j;
         itr2 = k;
         std::rotate(itr1,j,last);
         while (last != j)
         {
            ++j;
            ++itr2;
         }
         std::rotate(k,itr2,last);
         return true;
      }
   }
   std::rotate(first,k,last);
   return false;
}
