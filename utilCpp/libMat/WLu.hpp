/*
 *------------------------------------------------------------------------
 * swdLu.hpp - LU decomposition with interface of WMat<T>
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * $Id: WLu.cpp,v 1.2 2011/06/02 17:30:17 swu Exp $
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
template <class T>
WLu<T> :: WLu( const WMat<T> &A ):
	m_n ( A.cols() ),
	m_LU( A )  //copy A to m_LU (keep the original matrix)
{
	initProb();
}

template <class T>
WLu<T>::~WLu()
{
	if ( m_indx )
		delete [] m_indx ;

	if ( m_vv )
		delete [] m_vv;

}

template <class T>
bool  WLu<T>::isGood() const
{ 
	return m_infoFlag==1; 
}

template <class T>
bool  WLu<T>::isFail() const
{
	return m_infoFlag==0; 
}

template <class T>
bool  WLu<T>::isSingular() const
{ 
	return m_infoFlag==-1; 
}

template <class T>
void WLu<T>::initProb()
{
	m_ptrLU = m_LU.getBuf();

	if ( m_LU.rows() != m_n ){
		errorMsg( "WLu<T>::WLu(): $m_LU$ is not a square matrix!" );
		m_infoFlag = 0;
		return;
	}

	m_indx = new int [m_n];
	m_vv = new T [m_n];

	if ( !m_indx || !m_vv ){
		errorMsg( "WLu<T>::WLu(): cannot allocate memory to m_indx!" );
		m_infoFlag = 0;
		return;
	}

	//do LU decopmosition the results are saved in the matrix of m_LU
	ludcmp( m_ptrLU, m_n, m_indx, m_d);
	
	//m_LU.print("LU results");
}

//this function is from numerical recipe
template <class T>
void WLu<T>::print( const T *a, const int n)
{
	std::cout << n <<" x " <<n << std::endl;
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++)
			std::cout << a[i*n+j] << "   ";
		std::cout<<std::endl;
	}
}

template <class T>
bool WLu<T>::inv( WMat<T> &y )
{
	if (m_infoFlag<1)
		return false;

	uint32_t i, j;
	T* py = y.getBuf();

	
	for( j=0; j<m_n; j++){

		for(i=0; i<m_n; i++) 
			m_vv[i] = 0.0;

		m_vv[j]  = 1.0;
		lubksb( m_ptrLU, m_n, m_indx, m_vv);

		for(i=0; i<m_n; i++) py[i*m_n+j] = m_vv[i];

	}

	return true;
}


// return det(A)
template <class T>
T WLu<T>::det() const
{
	T det = m_d;

	for(uint32_t j=0; j<m_n; j++)
		det *= m_ptrLU[j*m_n + j];

	return det;
}

 // Solve L*U*x = b
template <class T>
void  WLu<T>::solve( WMat<T>& b)
{
	lubksb( m_ptrLU, m_n, m_indx, b.getBuf() );
}


template <class T>
void WLu<T>::lubksb( const T* a, const int n, const int* indx, T* b )
{
	int i,ii=-1,ip,j;
	T sum;

	for (i=0;i<n;i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii != -1) {
			for (j = ii;j <= i - 1;j++) { 
				sum -= a[i * n + j] * b[j]; 
			}
		}
		else if (sum) {
			ii = i;
		}
		b[i]=sum;
	}

	for (i=n-1;i>=0;i--) {
		sum=b[i];
		for (j = i + 1;j < n;j++) {
			sum -= a[i * n + j] * b[j];
		}
		b[i]=sum/a[i*n+i];
	}
}

//this function is from numerical recipe
template <class T>
void WLu<T>::ludcmp(T* a, const int n, int *indx, T& d)
{
	int i,imax,j,k;
	T big,dum,sum,temp;

	d = 1.0;
	for (i=0;i<n;i++) {
		big=0.0;
		for (j = 0;j < n;j++) {
			if ((temp = fabs(a[i * n + j])) > big) big = temp;
		}

		if ( big == 0.0 ){
			errorMsg("WLu<T>::ludcmp(): Singular matrix in function LUDCMP!");
			m_infoFlag = -1;
			return;
		}
		m_vv[i]=(T)1.0/big;
	}

	for (j=0;j<n;j++) {
		for (i=0;i<j;i++) {
			sum=a[i*n+j];
			for (k = 0;k < i;k++) {
				sum -= a[i * n + k] * a[k * n + j];
			}
			a[i*n+j]=sum;
		}

		big=0.0;
		for (i=j;i<n;i++) {
			sum=a[i*n+j];
			for (k=0;k<j;k++)
				sum -= a[i*n+k]*a[k*n+j];
			a[i*n+j]=sum;
			if ( (dum=m_vv[i]*fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}

		if (j != imax) {
			for (k=0;k<n;k++) {
				dum=a[imax*n+k];
				a[imax*n+k]=a[j*n+k];
				a[j*n+k]=dum;
			}
			d = -d;
			m_vv[imax]=m_vv[j];
		}

		indx[j]=imax;
//		if (a[j*n+j] == 0.0) a[j*n+j]=TINY;
		if (j != n) {
			dum=(T)1.0/(a[j*n+j]);
			for (i = j + 1;i < n;i++) {
				a[i * n + j] *= dum;
			}
		}
	}

	m_infoFlag = 1;
}

template <class T>
void WLu<T>::errorMsg( const char *msgStr)
{
	std::cout << msgStr << std::endl;

	//throw runtime_error( msgStr );
}


//--------------------------------------
//global funcs
//--------------------------------------

template<class T>
T  wmatDet2x2(const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (x.rows() != x.cols() || x.rows() != 2) {
		wErrMsg("wmatDet2x2(): x is not a square matrix!");
	}
#endif
	return x(0, 0) * x(1, 1) - x(0, 1) * x(1, 0);
}

template<class T>
bool wmatInv2x2(WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (x.rows() != x.cols() || x.rows() != 2) {
		wErrMsg("wmatInv2x2(): x is not a square matrix!");
	}
#endif

	T* ptrBuf = x.getBuf();

	T	detA = ptrBuf[0] * ptrBuf[3] - ptrBuf[1] * ptrBuf[2];
	if (fabs(detA) < WMAT_F32_EPS)
		return	false;

	T tmp = ptrBuf[0];
	ptrBuf[0] = ptrBuf[3] / detA;
	ptrBuf[1] = -ptrBuf[1] / detA;
	ptrBuf[2] = -ptrBuf[2] / detA;
	ptrBuf[3] = tmp / detA;

	return	true;
}

template<class T>
bool wmatInv2x2(const WMat<T>& x, WMat<T>& xInv, T& xDet)
{
#if WMAT_BOUNDS_CHECK
	if (x.rows() != x.cols() || x.rows() != 2) {
		wErrMsg("wmatInv2x2(): x is not a square matrix!");
	}
#endif

	const T* ptrBuf = x.data();
	xDet = ptrBuf[0] * ptrBuf[3] - ptrBuf[1] * ptrBuf[2];
	if (fabs(xDet) < WMAT_F32_EPS)
		return	false;

	xInv(0) = ptrBuf[3] / xDet;
	xInv(1) = -ptrBuf[1] / xDet;
	xInv(2) = -ptrBuf[2] / xDet;
	xInv(3) = ptrBuf[0] / xDet;

	return	true;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T wmatDet3x3(const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (x.rows() != x.cols() || x.rows() != 3) {
		wErrMsg("wmatDet3x3(): x is not a 3 x 3 matrix!");
	}
#endif

	const T* a = x.data();
	T xDet = a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
		a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
		a[5] * a[7] * a[0] - a[1] * a[3] * a[8];

	return	xDet;
}

template<class T>
bool wmatInv3x3(WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (x.rows() != x.cols() || x.rows() != 3) {
		wErrMsg("wmatInv3x3(): x is not a 3 x 3 matrix!");
	}
#endif

	T* a = x.getBuf();
	T	detA = a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
		a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
		a[5] * a[7] * a[0] - a[1] * a[3] * a[8];
	if (fabs(detA) < WMAT_F32_EPS)
		return	false;

	T b[9];
	b[0] = (a[4] * a[8] - a[5] * a[7]) / detA;
	b[1] = (a[2] * a[7] - a[1] * a[8]) / detA;
	b[2] = (a[1] * a[5] - a[2] * a[4]) / detA;
	b[3] = (a[5] * a[6] - a[3] * a[8]) / detA;
	b[4] = (a[0] * a[8] - a[2] * a[6]) / detA;
	b[5] = (a[2] * a[3] - a[0] * a[5]) / detA;
	b[6] = (a[3] * a[7] - a[4] * a[6]) / detA;
	b[7] = (a[1] * a[6] - a[0] * a[7]) / detA;
	b[8] = (a[0] * a[4] - a[1] * a[3]) / detA;

	for (uint32_t k = 0; k < 9; k++)
		a[k] = b[k];

	return	true;
}

template<class T>
bool wmatInv3x3(const WMat<T>& x, WMat<T>& xInv, T& xDet)
{
#if WMAT_BOUNDS_CHECK
	if (x.rows() != x.cols() || x.rows() != 3) {
		wErrMsg("wmatInv3x3(): x is not a 3 x 3 matrix!");
	}
#endif

	const T* a = x.data();
	xDet = a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
		a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
		a[5] * a[7] * a[0] - a[1] * a[3] * a[8];

	//todo: based on type decide EPS
	if (fabs(xDet) < WMAT_F32_EPS)
		return	false;

	xInv(0) = (a[4] * a[8] - a[5] * a[7]) / xDet;
	xInv(1) = (a[2] * a[7] - a[1] * a[8]) / xDet;
	xInv(2) = (a[1] * a[5] - a[2] * a[4]) / xDet;
	xInv(3) = (a[5] * a[6] - a[3] * a[8]) / xDet;
	xInv(4) = (a[0] * a[8] - a[2] * a[6]) / xDet;
	xInv(5) = (a[2] * a[3] - a[0] * a[5]) / xDet;
	xInv(6) = (a[3] * a[7] - a[4] * a[6]) / xDet;
	xInv(7) = (a[1] * a[6] - a[0] * a[7]) / xDet;
	xInv(8) = (a[0] * a[4] - a[1] * a[3]) / xDet;

	return	true;
}

template<class T>
bool wmatInvByLu(const WMat<T>& x, WMat<T>& xInv, T& det)
{
	WLu<T> lu(x);

	if (lu.isSingular()) {
		return false;
	}

	lu.inv(xInv);
	det = lu.det();
	return true;
}

template<class T>
bool wmatVerifyInv(const WMat<T>& x, const WMat<T>& xInv, const bool verbose)
{
	uint32_t m = x.rows();

#if WMAT_BOUNDS_CHECK
	if (m != x.cols() || xInv.rows() != xInv.cols()) {
		wWarningMsg("wmatVerifyInv(): x or xInv is not a square matrix!");
		return false;
	}

	if (m != xInv.rows()) {
		wWarningMsg("wmatVerifyInv(): sizes of x and xInv are not match!");
		return false;
	}
#endif

	const T eps = x.getEps();
	for (int iCase = 0; iCase < 2; iCase++) {
		WMat<T> I =  (iCase==0) ? (x % xInv) : (xInv % x);
		if (verbose) {
			if (iCase == 0) {
				I.print("x % xInv=");
			}
			else {
				I.print("xInv % x=");
			}
		}
		//check diagonal elements
		for (uint32_t i = 0; i < m; ++i) {
			T tmp = (T)fabs(I(i, i) - 1.0);
			if (tmp > eps) {
				I.print("wmatVerifyInv(): x%y=");
				return false;
			}
		}

		//check non-diagonal elements
		for (uint32_t i = 0; i < m; ++i) {
			for (uint32_t j = 0; j < m; ++j) {
				if (i != j) {
					T tmp = fabs(I(i, j));
					if (tmp > eps) {
						I.print("wmatVerifyInv(): x%y=");
						return false;
					}
				}
			}
		}
	}
	return true;
}





template<class T>
bool wmatEigen2x2(const WMat<T>& A, WMat<T>& vec, WMat<T>& val, const bool verbose)
{
	T t = A(0) + A(3);
	T t2 = sqrt(t * t - 4 * (A(0) * A(3) - A(1) * A(2)));

	val(0) = (t + t2) / 2;
	val(1) = (t - t2) / 2;
	assert(val(0) > val(1));

	T lamda, x1, x2;
	for (uint32_t i = 0; i < 2; i++) {
		lamda = val(i);
		if (fabs(A(3) - lamda) < 1e-6) {
			x2 = 1;
			x1 = A(1) / (lamda - A(0));
		}
		else {
			x1 = 1;
			x2 = A(2) / (lamda - A(3));
		}

		//normalizing eig. vector
		lamda = sqrt(x1 * x1 + x2 * x2);
		vec(0, i) = x1 / lamda;
		vec(1, i) = x2 / lamda;
	}

	if (verbose) {
		WMat<T> L = A % vec;
		WMat<T> tmp(2, 2, 0); tmp(0) = val(0); tmp(3) = val(1);
		WMat<T> R = vec % tmp;
		L.print("L=");
		R.print("R=");
	}
	return true;
}
