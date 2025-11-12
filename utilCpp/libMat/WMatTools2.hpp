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

 //out(i,j) = min( x(i,j), y(i,j) ), for i \in [0,m), j \in [0, n)
template<class T>
bool wmatMin(WMat<T>& out, const WMat<T>& x, const WMat<T>& y)
{
	WMAT_CHECK_DIM_EQ(x, y, "wmatMin(): the dimenstion of x and y are not equal")
	uint32_t m, n;
	x.size(m, n);
	out.resize(m, n);
	const T* px = x.data();
	const T* py = y.data();
	T* pz = out.data();
	for (uint32_t i = 0; i < m*n; ++i, px++,py++) {
		*pz++ = (*px > *py) ? (*py) : (*px);
	}
	return true;
}

//out(i,j) = min( x(i,j), y(i,j) ), for i \in [0,m), j \in [0, n)
template<class T>
bool wmatMax(WMat<T>& out, const WMat<T>& x, const WMat<T>& y)
{
	//will return false if has size issues
	WMAT_CHECK_DIM_EQ(x, y, "wmatMax(): the dimenstion of x and y are not equal")

  uint32_t m, n;
	x.size(m, n);
	out.resize(m, n);
	const T* px = x.data();
	const T* py = y.data();
	T* pz = out.data();
	for (uint32_t i = 0; i < m*n; ++i, px++, py++) {
		*pz++ = (*px > *py) ? (*px) : (*py);
	}
	return true;
}


//out = x(:, vIdx), $vIdx$ is logical true or false
template<class T>
bool wmatGetCols(WMat<T>& out, const WMat<T>& x, const WMatBool& vIdx)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const uint32_t L = vIdx.size();
	if (L != n) {
		wWarningMsg("wmatGetCols(): vIdx size (k=%u) is not equal the # of cols (n=%u) of x", L, n);
		return false;
	}

	uint32_t k = 0;   //count
	uint32_t nOut = wmatSumBool(vIdx);
	out.resize(m, nOut);
	for (uint32_t i = 0; i < L; ++i) {
		if (vIdx(i)) {
			out.setCol(k, x.getCol(i));
			k++;
		}
	}
#if _DEBUG
	assert(k == nOut);
#endif

	return true;
}

//out = [x(:, vIdx(1)), ..., x(:, vIdx(end))], $vIdx$ is index Ids
template<class T>
bool wmatGetCols(WMat<T>& out, const WMat<T>& x, const WMatU32& vIdx)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const uint32_t L = vIdx.size();

	if (L > x.cols()) {
		wWarningMsg("wmatGetCols(): # of elements in vIdx (L=%u) is more than # of cols of x (n=%u)", L, n);
		return false;
	}

	out.resize(x.rows(), L);
	for (uint32_t i=0; i < L; ++i) {
		if (vIdx(i) >= x.cols()) {
			wWarningMsg("wmatGetCols(): col # idx (%u) is out of range, should be in [0,n)!", vIdx(i), n);
			return false;
		}
		out.setCol(i, x.getCol(vIdx(i)));
	}
	return true;
}


//out = x(vIdx, :), return a sub-matrix of x whose rows are lofically true in <vIdx>
template<class T>
bool wmatGetRows(WMat<T>& out, const WMat<T> &x, const WMatBool &vIdx)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const uint32_t L = vIdx.size();
	if ( L != m ){
		wWarningMsg("wmatGetRows(): vIdx size (L=%u) is not equal the # of rows (m=%u) of x", L, m);
		return false;
	}

	uint32_t mOut = wmatSumBool(vIdx);
	out.resize(mOut, n);
	uint32_t k = 0;
	for (uint32_t i = 0; i < m; ++i) {
		if (vIdx(i)) {
			out.setRow(k, x.getRow(i));
			k++;
		}
	}
	return true;
}

//out = [x(vIdx(1), :); ...; x(vIdx(end), :)], $vIdx$ is the indices of rows
template<class T>
bool wmatGetRows(WMat<T>& out, const WMat<T>& x, const WMatU32 &vIdx)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const uint32_t L = vIdx.size();
	if (L > m) {
		wWarningMsg("wmatGetRows(): vIdx size (L=%u) is larger than the # of rows (m=%u) of x", L, m);
		return false;
	}

	out.resize(L, x.cols());
	for (uint32_t i = 0; i < L; ++i) {
		if (vIdx(i) >= x.rows()) {
			wWarningMsg("wmatGetCols(): row idx (%u) is out of range, should be in [0,%u)!", vIdx(i), m);
			return false;
		}
		out.setRow(i, x.getRow(vIdx(i)));
	}
	return true;
}

template<class T>
bool wmatLogicAnyGT(const WMat<T>& x, const T thd)
{
	const T* px = x.data();
	for (uint32_t i = 0; i < x.size(); ++i) {
		if (*px++ > thd)
			return true;
	}

	return false;
}

//out = x > thd,  $out$ is a logical matrix with the same size of $x$
template<class T>
uint32_t wmatLogicGT(WMatBool& out, const WMat<T> &x, const T thd)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const T* px = x.data();

	out.resize(m, n);

	bool		*pOut = out.getBuf();
	uint32_t cnt=0;
	for(uint32_t i=0; i<x.size(); ++i, ++px, ++pOut ){
		if( *px>thd ){
			*pOut = true;
			++cnt;
		}
		else{
			*pOut = false;
		}
	}
	return cnt;
}

//out = x >= thd,  $out$ is a logical matrix with the same size of $x$
template<class T>
uint32_t wmatLogicGE(WMatBool& out, const WMat<T> &x, const T thd )
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const T* px = x.data();

	out.resize(m, n);

	bool* pOut = out.getBuf();
	uint32_t cnt = 0;
	for (uint32_t i = 0; i < x.size(); ++i, ++px, ++pOut) {
		if (*px >= thd) {
			*pOut = true;
			++cnt;
		}
		else {
			*pOut = false;
		}
	}
	return cnt;
}

//out = x < thd, $out$ is a logical matrix with the same size of $x$
template<class T>
uint32_t wmatLogicST(WMatBool& out, const WMat<T>& x, const T thd)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const T* px = x.data();

	out.resize(m, n);

	bool* pOut = out.getBuf();
	uint32_t cnt = 0;
	for (uint32_t i = 0; i < x.size(); ++i, ++px, ++pOut) {
		if (*px < thd) {
			*pOut = true;
			++cnt;
		}
		else {
			*pOut = false;
		}
	}
	return cnt;
}

//out = x <= thd, $out$ is a logical matrix with the same size of $x$
template<class T>
uint32_t wmatLogicSE(WMatBool& out, const WMat<T>& x, const T thd)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const T* px = x.data();

	out.resize(m, n);

	bool* pOut = out.getBuf();
	uint32_t cnt = 0;
	for (uint32_t i = 0; i < x.size(); ++i, ++px, ++pOut) {
		if (*px <= thd) {
			*pOut = true;
			++cnt;
		}
		else {
			*pOut = false;
		}
	}
	return cnt;
}

//out = x == thd,  $out$ is a logical matrix with the same size of $x$
template<class T>
uint32_t wmatLogicEQ(WMatBool& out, const WMat<T> &x, const T thd, const T eps)
{
	const uint32_t m = x.rows();
	const uint32_t n = x.cols();
	const T* px = x.data();

	out.resize(m, n);

	bool* pOut = out.getBuf();
	uint32_t cnt = 0;
	for (uint32_t i = 0; i < x.size(); ++i, ++px, ++pOut) {
		if ( fabs(*px - thd) < eps) {
			*pOut = true;
			++cnt;
		}
		else {
			*pOut = false;
		}
	}
	return cnt;
}

