/*
 *------------------------------------------------------------------------
 * WMatTools1.hpp - untility functions part1
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatUtilities.cpp,v 1.9 2011/07/28 20:36:26 swu Exp $
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
bool wmatIsEmpty(const WMat<T>& x)
{
	return  x.isEmpty();
}

template <class T>
WMat<T>  wmatZeros(const uint32_t m, const uint32_t n)
{
	WMat<T> x(m, n, 0);
	return x;
}

template <class T>
WMat<T>  wmatOnes(const uint32_t m, const uint32_t n)
{
	WMat<T> x(m, n, 1);
	return x;
}

template <class T>
WMat<T>  wmatEye(const uint32_t m)
{
	WMat<T> x(m, m, 0);
	x.setDiagonal(1.0);
	return x;
}

template <class T>
WMat<T>  wmatReshape(const WMat<T>& x, const uint32_t m, const uint32_t n)
{
#if WMAT_BOUNDS_CHECK	
	if (x.size() != m * n) {
		wErrMsg("wmatReshape(): size is not match!");
	}
#endif

	WMat<T> y(m, n);
	y.setData(x.data(), x.size());
	return y;
}

template <class T>
void wmatReshape(const WMat<T>& x, WMat<T>& y)
{
#if WMAT_BOUNDS_CHECK	
	if (x.size() != y.cols() * y.rows()) {
		wErrMsg("wmatReshape(): size is not match!");
	}
#endif

	y.setData(x.data(), x.size());
}






template<class T>
WMatU32 wmatFindIdxGT(const WMat<T>& x, const T threshhold)
{
	return wmatSelectEleIdx_private<T>(x, threshhold, "GT");
}

template<class T>
WMatU32 wmatFindIdxGE(const WMat<T>& x, const T threshhold)
{
	return wmatSelectEleIdx_private<T>(x, threshhold, "GE");
}

template<class T>
WMatU32 wmatFindIdxEQ(const WMat<T>& x, const T threshhold)
{
	return wmatSelectEleIdx_private<T>(x, threshhold, "EQ");
}

template<class T>
WMatU32 wmatFindIdxST(const WMat<T>& x, const T threshhold)
{
	return wmatSelectEleIdx_private<T>(x, threshhold, "ST");
}

template<class T>
WMatU32 wmatFindIdxSE(const WMat<T>& x, const T threshhold)
{
	return wmatSelectEleIdx_private<T>(x, threshhold, "SE");
}

template<class T>
T  wmatMax(const WMat<T>& x)
{
	WMAT_CHECK_EMPTY(x, "wmatMax(): x is an emppty matrix!")

	const T* px = x.data();
	T d = *px++;
	for (uint32_t i = 1; i < x.size(); i++, px++) {
		if (d < *px) {
			d = *px;
		}
	}
	return d;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatMax(const WMat<T>& x, uint32_t* idx)
{
	WMAT_CHECK_EMPTY(x, "wmatMax(): x is an emppty matrix!")

	const T* px = x.data();
	T d = *px++;
	*idx = 0;
	for (uint32_t i = 1; i < x.size(); i++, px++) {
		if (d < *px) {
			d = *px;
			*idx = i;
		}
	}
	return d;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatMin(const WMat<T>& x)
{
	WMAT_CHECK_EMPTY(x, "wmatMin(): x is an emppty matrix!")

	const uint32_t n = x.size();
	const T* px = x.data();
	T d = *px++;
	for (uint32_t i = 1; i < n; i++,  px++) {
		if ( d > *px) {
			d = *px;
		}
	}
	return d;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatMin(const WMat<T>& x, uint32_t* idx)
{
	WMAT_CHECK_EMPTY(x, "wmatMin(): x is an emppty matrix!")

	const T* px = x.data();
	T d = *px++;
	*idx = 0;
	for (uint32_t i = 1; i < x.size(); i++, px++) {
		if (d > *px) {
			d = *px;
			*idx = i;
		}
	}
	return d;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatMean(const WMat<T>& x)
{
	WMAT_CHECK_EMPTY(x, "wmatMean(): x is an emppty matrix!")

	const uint32_t n = x.size();
	const T* px = x.data();
	T d = 0.0;
	for (uint32_t i = 0; i < n; i++) {
		d += (*px++);
	}
	return d / n;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatMedian(const WMat<T>& x)
{
		WMAT_CHECK_EMPTY(x, "wmatMedian(): x is an emppty matrix!")

	//todo: this impl is wrong, need sort and thake the elements at midlle
	T a = wmatMin(x);
	T b = wmatMax(x);

	return (T)(0.5 * (a + b));
}

//--------------------------------------
//
//--------------------------------------
//todo: add exception for WMatBool type
template<class T>
T  wmatSum(const WMat<T>& x)
{
	WMAT_CHECK_EMPTY(x, "wmatSum(): x is an emppty matrix!")
  
	const T* px = x.data();
	T d = 0;
	for (uint32_t i = 0; i < x.size(); i++, px++) {
		d += *px;
	}
	return d;
}


//--------------------------------------
//
//--------------------------------------
template<class T>
T wmatNorm(const WMat<T>& x)
{
	WMAT_CHECK_EMPTY(x, "wmatNorm(): x is an emppty matrix!")

	const T* px = x.data();
	T res = 0.0;
	for (uint32_t i = 0; i < x.size(); i++, px++) {
		res += (*px) * (*px);
	}

	return (T)(sqrt(res));
}

//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T>  wmatMax( const WMat<T> &x, const uint32_t dimFlag )
{
	if ( x.isEmpty( "warning -- wmatMax(): x is empty!" )) {
		return x;
	}

	if (dimFlag==1) //return a row vector
	{
		uint32_t n = x.cols();
		WMat<T> y(1, n);
		for(uint32_t i=0; i<n; i++){
			y(i) = wmatMax( x.getCol(i) ) ;
		}

		return y;
	}
	else //return a col vector
	{
		uint32_t m = x.rows();
		WMat<T> y(m, 1);
		for(uint32_t i=0; i<m; i++){
			y(i) = wmatMax( x.getRow(i) ) ;
		}
		return y;
	}
}


//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T>  wmatMean( const WMat<T> &x, const uint32_t dimFlag )
{
	if ( x.isEmpty("warning -- wmatMean(): x is empty!") ){
		return x;
	}

	if (dimFlag==1) //return a row vector
	{
		uint32_t n = x.cols();
		WMat<T> y(1, n);
		for(uint32_t i=0; i<n; i++){
			y(i) = wmatMean( x.getCol(i) ) ;
		}

		return y;
	}
	else //return a col vector
	{
		uint32_t m = x.rows();
		WMat<T> y(m, 1);
		for(uint32_t i=0; i<m; i++){
			y(i) = wmatMean( x.getRow(i) ) ;
		}
		return y;
	}
}


//--------------------------------------
template<class T>
bool wmatMeanStd( const WMat<T> &x, T &mean, T &std)
{
	if ( x.isEmpty("warning -- wmatMeanStd(): x is empty!") ){
		return false;
	}
	
	//x.print();

	mean = wmatMean( x );

	uint32_t  n = x.size(); 
	T dx,s = 0.0;
	const T *px = x.data();
	for(uint32_t i=0; i<n; i++){
		dx = *(px++) - mean;
		s += dx*dx;
	}

	if ( n>1 )
		n--;

	std = sqrt ( s/(T)n );

	return true;
}


//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T>  wmatMedian( const WMat<T> &x, const uint32_t dimFlag )
{
	if ( x.isEmpty("warning -- wmatMedian(): x is empty!") ){
		return x;
	}

	if (dimFlag==1) //return a row vector
	{
		uint32_t n = x.cols();
		WMat<T> y(1, n);
		for(uint32_t i=0; i<n; i++){
			y(i) = wmatMedian( x.getCol(i) ) ;
		}
		return y;
	}
	else //return a col vector
	{
		uint32_t m = x.rows();
		WMat<T> y(m, 1);
		for(uint32_t i=0; i<m; i++){
			y(i) = wmatMedian( x.getRow(i) ) ;
		}
		return y;
	}
}


//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T>  wmatMin( const WMat<T> &x,  const uint32_t dimFlag )
{
	if ( x.isEmpty("warning -- wmatMin(): x is empty!") ){
		return x;
	}

	if (dimFlag==1) //return a row vector
	{
		uint32_t n = x.cols();
		WMat<T> y(1, n);
		for(uint32_t i=0; i<n; i++){
			y(i) = wmatMin( x.getCol(i) ) ;
		}

		return y;
	}
	else //return a col vector
	{
		uint32_t m = x.rows();
		WMat<T> y(m, 1);
		for(uint32_t i=0; i<m; i++){
			y(i) = wmatMin( x.getRow(i) ) ;
		}
		return y;
	}
}



//************************************************
// start from S
//************************************************

//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T>  wmatSum( const WMat<T> &x, const uint32_t dimFlag )
{
	if ( x.isEmpty("warning -- wmatSum(): x is empty!") ){
		return x;
	}

	if (dimFlag==1) //return a row vector
	{
		uint32_t n = x.cols();
		WMat<T> y(1, n);
		for(uint32_t i=0; i<n; i++){
			y(i) = wmatSum( x.getCol(i) ) ;
		}
		return y;
	}
	else //return a col vector
	{
		uint32_t m = x.rows();
		WMat<T> y(m, 1);
		for(uint32_t i=0; i<m; i++){
			y(i) = wmatSum( x.getRow(i) ) ;
		}
		return y;
	}
}


//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T> wmatAbs(const WMat<T>& x)
{
	if (x.isEmpty("warning -- wmatAbs(): x is empty!")) {
		return x;
	}

	WMat<T> y(x);
	T* p = y.getBuf();
	for (uint32_t i = 0; i < y.size(); i++) {
		if (p[i] < 0) {
			p[i] = -p[i];
		}
	}
	return y;
}

//--------------------------------------
//arithmatic functions
//--------------------------------------
template<class T>
WMat<T>  wmatExp( WMat<T> &x )
{
	if ( x.isEmpty("warning -- wmatExp(): x is empty!") ){
		return x;
	}

	uint32_t m=x.rows();
	uint32_t n=x.cols();
	WMat<T> y(m, n);

	const T* px=x.data();
	T* py=y.getBuf();
	for (uint32_t i=0; i<m*n; i++){
			*py++ = exp( *px++ ) ;
	}

	return y;
}

template<class T>
WMat<T>  wmatSin( const WMat<T> &x )
{
	if ( x.isEmpty("warning -- wmatSin(): x is empty!") ){
		return x;
	}

	uint32_t m=x.rows();
	uint32_t n=x.cols();
	WMat<T> y(m, n);

	const T* px=x.data();
	T* py=y.getBuf();
	for (uint32_t i=0; i<x.size(); i++){
			*py++ = sin( *px++ ) ;
	}

	return y;
}

template<class T>
WMat<T>  wmatCos( const WMat<T> &x )
{
	if ( x.isEmpty("warning -- wmatCos(): x is empty!") ){
		return x;
	}

	uint32_t m=x.rows();
	uint32_t n=x.cols();
	WMat<T> y(m, n);

	const T* px=x.data();
	T* py=y.getBuf();
	for (uint32_t i=0; i<m*n; i++){
			*py++ = cos( *px++ ) ;
	}

	return y;
}

template<class T>
WMat<T>  wmatTan( const WMat<T> &x )
{
	if ( x.isEmpty("warning -- wmatTan(): x is empty!") ){
		return x;
	}

	uint32_t m=x.rows();
	uint32_t n=x.cols();
	WMat<T> y(m, n);

	const T* px=x.data();
	T* py=y.getBuf();
	for (uint32_t i=0; i<m*n; i++){
			*py++ = tan( *px++ ) ;
	}

	return y;
}


//-----------------------------------
//private funcs
//-----------------------------------
template<class T>
WMat<T> wmatSelectCol_private( const WMat<T> &x, const uint32_t colIdx,  const T checkVal, char*flag )
{
	uint32_t m = x.rows();
	uint32_t n = x.cols();

	if ( colIdx < 0 || colIdx >= n ){
		wErrMsg("wmatSelectColEq(): wrong col idx!");
	}

	WMat<T> y( m, n );
	uint32_t j=0;

	for( uint32_t i=0; i<m; i++ ){
		if ( x(i, colIdx) == checkVal ){
			y.setRow( j, x.getRow(i) );
			j++;
		}
	}
	y.delRows( j, m-1 );

	return y;
}
//-----------------------
template<class T>
WMatU32 wmatSelectEleIdx_private(const WMat<T>& x, const T threshhold, const char* flag)
{
	uint32_t n = x.size();
	uint32_t i, k = 0;

	WMatU32 y(1, n);
	const T* px = x.data();
	uint32_t* py = y.getBuf();
	if (0 == strcmp(flag, "GT")) {
		for (i = 0; i < n; ++i, ++px) {
			if (*px > threshhold) {
				*py++ = i;
				++k;
			}
		}
	}
	else if (0 == strcmp(flag, "GE")) {
		for (i = 0; i < n; ++i, ++px) {
			if (*px >= threshhold) {
				*py++ = i; 
				++k;
			}
		}
	}
	else if (0 == strcmp(flag, "EQ")) {
		for (i = 0; i < n; ++i, ++px) {
			if (*px == threshhold) {
				*py++ = i; 
				++k;
			}
		}
	}
	else if (0 == strcmp(flag, "ST")) {
		for (i = 0; i < n; ++i, ++px) {
			if (*px < threshhold) {
				*py++ = i; 
				++k;
			}
		}
	}
	else if (0 == strcmp(flag, "SE")) {
		for (i = 0; i < n; ++i, ++px) {
			if (*px <= threshhold) {
				*py++ = i; 
				++k;
			}
		}
	}
	else {
		y.resize(0, 0);
		return y;
	}

	if (k > 1) {
		return y.getSubRow(0, 0, k - 1);
	}
	else {
		y.resize(0, 0);
		return y;
	}

}


template<class T>
void  wMatDump(const WMat<T>& x, const std::string& path, const std::string& fname_wo_ext, const std::string& fmt, const int& fn, const int& idx)
{

	uint32_t m = x.rows();
	uint32_t n = x.cols();

	char fileName[1024];
	if (idx < 0)
		sprintf_s(fileName, 1024, "%s\\%s-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn);
	else
		sprintf_s(fileName, 1024, "%s\\%s-%05d-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn, idx);

	std::string fmt1 = fmt + ",";
	std::string fmt2 = fmt + "\n";
	FILE* fp;
	fopen_s(&fp, fileName, "w");
	if (fp == NULL) {
		printf("my_text_writer2(): file: \n %s \n  cannot be opened!\n", fileName);
		return;
	}
	const T* px = x.data();
	uint32_t i, j;
	fprintf(fp, "%d,%d\n", m, n);
	for (j = 0; j < m; ++j) {  //row
		for (i = 0; i < n - 1; ++i) {  //col
			fprintf(fp, fmt1.c_str(), *px++);
		}
		fprintf(fp, fmt2.c_str(), *px++);
	}
	fclose(fp);
}
