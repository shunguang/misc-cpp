/*
 *------------------------------------------------------------------------
 * swdLu.cpp - LU decomposition with interface of swdMat or swfMat.
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * $Id: swMatLu.cpp,v 1.2 2011/06/02 17:30:17 swu Exp $
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

//#include "stdafx.h"
#include "swMatLu.h"

swMatLu :: swMatLu( const swdMat &A ):
	m_n ( A.cols() ),
	m_LU( A )  //copy A to m_LU (keep the original matrix)
{
	initProb();
}

swMatLu :: swMatLu( const swfMat &A ):
	m_n ( A.cols() ),
	m_LU( A.rows(), A.cols() )
{
	unsigned int L=A.size();
	for (unsigned int i=0; i<L; i++)
		m_LU(i) = (float) A(i);		//copy A to m_LU

	initProb();
}

swMatLu :: ~swMatLu()
{
	if ( m_indx )
		delete [] m_indx ;

	if ( m_vv )
		delete [] m_vv;

}

bool  swMatLu :: isGood() const
{ 
	return m_infoFlag==1; 
}

bool  swMatLu :: isFail() const
{
	return m_infoFlag==0; 
}
bool  swMatLu :: isSingular() const 
{ 
	return m_infoFlag==-1; 
}

void swMatLu :: initProb()
{
	m_ptrLU = m_LU.getBuf();

	if ( m_LU.rows() != m_n ){
		errorMsg( "swMatLu :: swMatLu(): $m_LU$ is not a square matrix!" );
		m_infoFlag = 0;
		return;
	}

	m_indx = new int [m_n];
	m_vv = new double [m_n];

	if ( !m_indx || !m_vv ){
		errorMsg( "swMatLu :: swMatLu(): cannot allocate memory to m_indx!" );
		m_infoFlag = 0;
		return;
	}

	//do LU decopmosition the results are saved in the matrix of m_LU
	ludcmp( m_ptrLU, m_n, m_indx, m_d);
	
	//m_LU.print("LU results");
}

//this function is from numerical recipe
void swMatLu :: print( const double *a, const int n)
{
	cout << n <<" x " <<n << endl;
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++)
			cout << a[i*n+j] << "   ";
		cout<<endl;
	}
}

bool swMatLu :: inv( swdMat &y )
{
	if (m_infoFlag<1)
		return false;

	unsigned int i, j;
	double* py = y.getBuf();

	
	for( j=0; j<m_n; j++){

		for(i=0; i<m_n; i++) 
			m_vv[i] = 0.0;

		m_vv[j]  = 1.0;
		lubksb( m_ptrLU, m_n, m_indx, m_vv);

		for(i=0; i<m_n; i++) py[i*m_n+j] = m_vv[i];

	}

	return true;
}

bool swMatLu :: inv( swfMat &y )
{
	if (m_infoFlag<1)
		return false;

	unsigned int i, j;
	float* py = y.getBuf();

	
	for( j=0; j<m_n; j++){

		for(i=0; i<m_n; i++) 
			m_vv[i] = 0.0;

		m_vv[j]  = 1.0;
		lubksb( m_ptrLU, m_n, m_indx, m_vv);

		for(i=0; i<m_n; i++) py[i*m_n+j] = (float) m_vv[i];
	}

	return true;
}


// return det(A)
double swMatLu :: det() const  
{
	double det = m_d;

	for(unsigned int j=0; j<m_n; j++)
		det *= m_ptrLU[j*m_n + j];

	return det;
}

 // Solve L*U*x = b
void  swMatLu :: solve( swdMat& b) 
{
	lubksb( m_ptrLU, m_n, m_indx, b.getBuf() );
}

void  swMatLu :: solve( swfMat& b) 
{
	unsigned int n=b.size();

	double* b2= new double[n];
	
	lubksb( m_ptrLU, m_n, m_indx, b2 );
	
	for (unsigned int i=0; i<n; i++)
		b(i) = (float)b2[i];

}


void swMatLu :: lubksb( const double* a, const int n, const int* indx, double* b )
{
	int i,ii=-1,ip,j;
	double sum;

	for (i=0;i<n;i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii != -1)
			for (j=ii;j<=i-1;j++) sum -= a[i*n+j]*b[j];
		else if (sum) ii=i;
		b[i]=sum;
	}

	for (i=n-1;i>=0;i--) {
		sum=b[i];
		for (j=i+1;j<n;j++) sum -= a[i*n+j]*b[j];
		b[i]=sum/a[i*n+i];
	}
}

//this function is from numerical recipe
void swMatLu :: ludcmp(double* a, const int n, int *indx, double& d)
{
	int i,imax,j,k;
	double big,dum,sum,temp;

	d = 1.0;
	for (i=0;i<n;i++) {
		big=0.0;
		for (j=0;j<n;j++)
			if ((temp=fabs(a[i*n+j])) > big) big=temp;

		if ( big == 0.0 ){
			errorMsg("swMatLu :: ludcmp(): Singular matrix in function LUDCMP!");
			m_infoFlag = -1;
			return;
		}
		m_vv[i]=(double)1.0/big;
	}

	for (j=0;j<n;j++) {
		for (i=0;i<j;i++) {
			sum=a[i*n+j];
			for (k=0;k<i;k++) sum -= a[i*n+k]*a[k*n+j];
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
			dum=(double)1.0/(a[j*n+j]);
			for (i=j+1;i<n;i++) a[i*n+j] *= dum;
		}
	}

	m_infoFlag = 1;
}

void swMatLu :: errorMsg( const char *msgStr)
{
	cout << msgStr << endl;

	//throw runtime_error( msgStr );
}
