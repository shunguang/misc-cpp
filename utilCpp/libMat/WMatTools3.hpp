/*
 *------------------------------------------------------------------------
 * WMat<T>Utilities.cpp - untility functions used by all project
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

//get angle betwee two vectors in rad
//refVec, 2 x 1,
//insVec, 2 x 1,
//return [0,pi]  if ins. vector is located at the left of ref. vector
//		 [-pi,0) if ins. vector is located at the right of ref. vector
//		 SWF_MAT_REALMAX if in valid input	
template<class T>
T wmatAngleBetweenTwoVectorsInRad(const WMat<T> &refVec, const WMat<T> &insVec)
{
	const T eps = refVec.getEps();
	T refL2 =  wmatDotProduct( refVec, refVec );
	T insL2 =  wmatDotProduct( insVec, insVec );
	if ( refL2 < eps || insL2 < eps ){
		return eps;
	}
	WMat<T> R(2,2), newInsVec(2,1);

	//rotate the inspection vector
	refL2 = sqrt( refL2 );
	R(0,0) = refVec(0)/refL2; R(0,1) = refVec(1)/refL2;
	R(1,0) = -R(0,1);    R(1,1) = R(0,0);
	newInsVec = R % insVec;

	T result = atan2( newInsVec(1), newInsVec(0) );
	
	//printf( "refAng=%f, newInsVec(%f,%f), result=%f\n", refAng*SW_MAT_R2D, newInsVec(0), newInsVec(1),result*SW_MAT_R2D);
	return result;
}
//--------------------------------------
//
//--------------------------------------
template<class T>
WMat<T> wmatCrossProduct(const WMat<T> &x,const WMat<T> &y)
{
	if ((x.cols()*x.rows()!=3) ||(y.cols()*y.rows()!=3))
	{
		wWarningMsg( "cross product only works on 3 vector!" );
		WMat<T> zero(0, 0);
		return zero;
	}

	WMat<T> z(3, 3, 0);
	z(0) = x(1)*y(2)-x(2)*y(1);
  z(1) = x(2)*y(0)-x(0)*y(2);
  z(2) = x(0)*y(1)-x(1)*y(0);
	return z;
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatDotProduct( const WMat<T> &x, const WMat<T> &y )
{
#if WMAT_BOUNDS_CHECK
	if (x.isEmpty() || y.isEmpty()) {
		wWarningMsg("dotProduct(): x or y is an empty matrix!");
		return (T)WMAT_NAN;
	}

	if ( x.size() != y.size() ){
		wWarningMsg( "dotProduct(): size is not match!");
		return (T)WMAT_NAN;
	}

#endif
	return wmatDotProduct<T>(x.data(), y.data(), x.size());
}

//--------------------------------------
//
//--------------------------------------
template<class T>
T  wmatDotProduct(const T *p1, const T *p2, const uint32_t n)
{
	T d = 0.0;
	for (uint32_t i = 0; i < n; i++) {
		d += (*p1++) * (*p2++);
	}
	return d;
}

template<class T>
WMat<T> wmatOutProduct( const WMat<T> &v1, const WMat<T> &v2 )
{
	uint32_t i, j;
	uint32_t m = v1.size();
	uint32_t n = v2.size();
	WMat<T> A(m, n);
	
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			A(i, j) = v1(i) * v2(j);
		}
	}
	return  A;
}

template<class T>
T wmatDist2( const WMat<T> &v1, const WMat<T> &v2)
{
#if WMAT_BOUNDS_CHECK
	if ( v1.size() != v2.size() ){
		wWarningMsg( "wmatDist2(): the sizes of v1 and v2 is different!");
		return (T)WMAT_NAN;
	}

	if ( wmatIsEmpty(v1) || wmatIsEmpty(v2) ){
		wWarningMsg( "wmatDist2(): x or y is an emppty matrix!");
		return (T)WMAT_NAN;
	}
#endif

	const T* p1 = v1.data();
	const T* p2 = v2.data();
	T d2 = 0.0;
	T tmp;
	for(uint32_t i=0; i<v1.size(); i++){
		tmp = *p1++ - *p2++;
		d2 += tmp*tmp;
	}
	return d2;
}

//v1, v2, n x 1
//cov1, cov2, n x n,
template<class T>
T wmatDistMahalanobis( const WMat<T> &v1, const WMat<T> &v2, const WMat<T> &cov1, const WMat<T> &cov2)
{
#if WMAT_BOUNDS_CHECK
	if ( v1.isEmpty() || v2.isEmpty() ){
		wWarningMsg( "wmatDistMahalanobis(): v1 or v2 is an emppty matrix!");
		return (T)WMAT_NAN;
	}
	if ( v1.size() != v2.size() ){
		wWarningMsg( "wmatDistMahalanobis(): sizes of v1 and v2 are not match!");
		return (T)WMAT_NAN;
	}
	//TODO: also check the sizes of cov1 and cov2
#endif

	WMat<T> nu(v1 - v2);
	WMat<T> S( cov1 + cov2 );
	WMat<T> invS( v1.size(), v1.size() );
	T detS;
#if 0
		v1.print	("%10.5f ", "v1=");
		v2.print	("%10.5f ", "v2=");
		cov1.print	("%10.5f ", "cov1=");
		cov2.print	("%10.5f ", "cov2=");
		S.print		("%10.5f ", "S=");
#endif

	if ( wmatInvByLu ( S, invS, detS ) ){
		detS = sqrt( wmatDotProduct( nu, invS%nu ) );
		return detS;
	}
	else{
		printf("wmatDistMahalanobis(): inv S is not exist, return -1!");
		v1.print	("%10.5f ", "v1=");
		v2.print	("%10.5f ", "v2=");
		cov1.print	("%10.5f ", "cov1=");
		cov2.print	("%10.5f ", "cov2=");
		S.print		("%10.5f ", "S=");
		printf("--------------- wmatDistMahalanobis() -------------!");
		return (T)WMAT_NAN;
	}
}

//v1, v2, n x 1
//cov1, cov2, n x 1, only diagonal terms
template<class T>
T wmatDistMahalanobis2( const WMat<T> &v1, const WMat<T> &v2, const WMat<T> &cov1, const WMat<T> &cov2)
{
#if WMAT_BOUNDS_CHECK
	if (v1.isEmpty() || v2.isEmpty()) {
		wWarningMsg("wmatDistMahalanobis2(): v1 or v2 is an emppty matrix!");
		return (T)WMAT_NAN;
	}
	if (v1.size() != v2.size()) {
		wWarningMsg("wmatDistMahalanobis2(): sizes of v1 and v2 are not match!");
		return (T)WMAT_NAN;
	}
	//TODO: also check the sizes of cov1 and cov2
#endif
	const T eps = v1.getEps();
	WMat<T> nu(v1 - v2);
	WMat<T> S( cov1 + cov2 );
	T d=0.0;
	uint32_t n=v1.size();
	for (uint32_t i=0; i<n; i++){
		if( S(i)> eps ){
			d += ( nu(i)*nu(i) / S(i) );
		}else{
			return (T)WMAT_NAN;
		}
	}
	return sqrt(d);
}

//cal $m_heading$ w.r.t. to vx
//return [0,2*pi]
template<class T>
T HeadingFromVel( const T vx, const T vy )
{
	T eps = wGetEps<T>();
	T heading;
	if( fabs(vx)<eps && fabs(vy)< eps ){
		heading = (T)WMAT_NAN;		//invalid
	}
	else{
		heading = atan2(vy, vx);
	}
	
	if ( heading < 0 ){
		heading += WMAT_TWO_PI;
	}
	return heading; //in rad.
}

template<class T>
T wmatHeadingDist( const T h1_rad, const T h2_rad )
{
	const T eps = wGetEps<T>();
	T dotProduct =  cos( h1_rad ) * cos( h2_rad ) + sin( h1_rad ) * sin( h2_rad );
	
	if (dotProduct > 1) {
		dotProduct = 1 - eps;
	}
	else if (dotProduct < -1) {
		dotProduct = -1 + eps;
	}

	T dAng = acos(dotProduct);
	//printf( "h1=%f, h2=%f, dotProd=%f, dAng=%f\n", h1_rad, h2_rad, dotProduct, dAng );
	return dAng;  //in radius
}

//--------------------------------------
//h1_rad[2], heading and its variance for the 1st
//h2_rad[2], heading and its variance for the 2nd
//return heading dist in radus
//--------------------------------------
template<class T>
T wmatHeadingDist( const T *h1_rad, const T *h2_rad )
{
	const T eps = wGetEps<T>();
	T ang1 = h1_rad[0]; 
	T ang2 = h2_rad[0];
	T var = ( sqrt(h1_rad[1]) + sqrt(h2_rad[1]) );  //in radus
	
	T dotProduct =  cos( ang1 ) * cos( ang2 ) + sin( ang1 ) * sin( ang2 );
	if (dotProduct > 1) {
		dotProduct = 1 - eps;
	}
	else if (dotProduct < -1) {
		dotProduct = -1 + eps;
	}

	ang1 = acos(dotProduct) + var;
	//printf( "trk(%f,%f), obs(%f,%f), var=%f, dotProduct=%f, dist=%f\n", h1[0],h1[1],h2[0],h2[1], var, dotProduct, ang1);
	return ang1;  //in radius
}

//---------------------------------------------------------------------------------
// a Bubble sort alg in Acending order
// x: original vector;
// y: sortted vector
// I: index vector to satisfy: y(i) = X ( I(i) ) 
//--------------------------------------------------------------------------------
template<class T>
void wmatSort(WMat<T>& y, WMatU32& I, const WMat<T> &x, const bool isDescending )
{
	uint32_t i, j, idTmp;
	T temp;

	y.resize( x.rows(), x.cols() );
	I.resize( x.rows(), x.cols() );
	
	uint32_t n  = x.size();
	y = x;
	for (i = 0; i < n; i++) {
		I(i) = i;
	}

	//I.print("I=");
	for (i = (n - 1); i >= 0; i--){
	    for (j = 1; j <= i; j++){
			if ( y(j-1) > y(j) ){
				temp = y(j-1);
				idTmp = I(j-1);

				y(j-1) = y(j);
				I(j-1) = I(j);

				y(j) = temp;
				I(j) = idTmp;
			}
		}
	}

	if ( isDescending ){
		//todo: directly swap y(i) and y(n-i-1), i=0, ..., n/2
		WMat<T> y1 (y);
		WMatU32 I1 (I);
		for ( i=0; i<n; i++){
			y(i) = y1(n-1-i);
			I(i) = I1(n-1-i);
		}
	}
}




//find all peaks and sorted by the peak values
//input
// x, 1 x n, vector
//output
// peakValues, 1 x nPeaks, vector
// peakId, 1 x nPeaks, 
template<class T>
void wmatPeakFinder(WMat<T>& peakValues, WMatU32& peakIds, const WMat<T> &x )
{
	const T eps = wGetEps<T>();
	//get all peaks
	uint32_t nX = x.size();
	if( nX<3 ){
		peakValues.resize(0,0);
		peakIds.resize(0,0);
		return;
	}

	uint32_t i, j, k;
	WMat<T> peaks(1, nX);
	WMatU32 ids(1, nX);
	k=0;
	if( x(0) > x(1) ){
		peaks(k) = x(0);
    ids(k) = 0;
		k++;
	}

	i=1;
	while( i < nX-1 ){
		if  ( x(i) > x(i-1) && x(i) > x(i+1) ){ 
			peaks(k) = x(i);
      ids(k) = i;
			k++;
		}
		else if ( x(i) > x(i-1) && x(i) == x(i+1) ){
			j=i;
			while(1){
				i++;
				//printf( "%d, %f, %f, %d\n", i, x(i), x(i+1), ( abs(x(i) - x(i+1)) < SWF_MAT_EPS ) );
				if ( abs ( x(i) - x(i+1) ) > eps  || i>=nX-1 )
					break;
			}
			
			if( x(i+1) < x(i) ){
				j += i;
				j /= 2; 

				peaks(k) = x(j);
		    ids(k) = j;
				k++;
			}
		}
		i++;		
	}

	if( x(nX-1) > x(nX-2) ){
		peaks(k) = x(nX-1);
    ids(k) = nX-1;
		k++;
	}

	if (k > 0) {
		peakValues = peaks.getSubRow(0, 0, k);
		peakIds = ids.getSubRow(0, 0, k);
	}
	else{
		peakValues.resize(0,0);
		peakIds.resize(0,0);
	}
}

template<class T>
void wmatPeakFinder(WMat<T>& peakValues, WMatU32& peakIds, const WMat<T> &x, const T threshold )
{
	WMat<T> allPeakValues;
	WMatU32 allPeakIds;

	wmatPeakFinder( allPeakValues, allPeakIds, x );

	uint32_t n = allPeakValues.size();
	if ( n > 1 ){
		WMatU32 F ( wmatFindIdxGE<T>( allPeakValues, threshold * allPeakValues(0) ) );
		uint32_t n = F.size();

		uint32_t *p = new uint32_t[n];
		if (p==NULL){
			wErrMsg( "wmatPeakFinder(): not enough memory!");
		}

		for (uint32_t i=0; i<n; i++){
			p[i] = (uint32_t) F(i);
		}

		peakValues = allPeakValues.getData( p, n );
		peakIds = allPeakIds.getData( p, n );
		delete [] p;
	}
	else{
		peakValues.setData( allPeakValues );
		peakIds.setData( allPeakIds );
	}
}


//---------------------------------------------------------------------
//calculate the SNR (in db) of a given signal at particular peak
//x,		1d vector, x(i)>=0 for all i's
//coeffThd, in (0,1], default 0.6065
//---------------------------------------------------------------------
template<class T>
T wmatSnr(uint32_t &nSignalBins, uint32_t &nTotalBins, const WMat<T> &x, const uint32_t peakIdx, const T coeffThd )
{
	uint32_t nX = x.size();
	if ( peakIdx>=nX || peakIdx<0 ){
		wWarningMsg( "wmatSnr(): input peakId is out of range!"); 
		nSignalBins = 0;
		nTotalBins = 0;
		return WMAT_NAN;							////20*log10(1e-100)
	}
	
	const T eps = wGetEps<T>();
	//search signal + noise boundary
	uint32_t i, iL, iR;
	T xMinL, xMinR;
	//Left boundary
	for( i=peakIdx-1; i>=0; i-- ){
		if ( x(i) > x(i+1) ){
			break;
		}
	}
	iL = i+1;
	xMinL = x(iL);

	//Right boundary
	for( i=peakIdx+1; i< nX; i++ ){
		if ( x(i) > x(i-1) ){
			break;
		}
	}
	iR=i-1;
	xMinR = x(iR);

	//cal.ignal and noise strenth
	T dx;
	T As(0.0f), A(0.0f);
	T xThd =  (  x(peakIdx) - xMinL ) * coeffThd;

	nTotalBins = iR-iL+1; 
	nSignalBins = 0;
	for( i=iL; i<=peakIdx; i++){
		dx = x(i)-xMinL;
		A += x(i);
		if( dx >= xThd ){
			nSignalBins++;
			As += x(i);
		}
	}

	xThd =  (  x(peakIdx) - xMinR ) * coeffThd;
	for( i=peakIdx+1; i<=iR; i++){
		dx = x(i)-xMinR;
		A += x(i);
		if( dx >= xThd ){
			nSignalBins++;
			As += x(i);
		}
	}


	if ( nSignalBins < 1 ){
		wWarningMsg( "wmatSnr(): input data is invalid!"); 
		return WMAT_NAN;							//20*log10(1e-100)
	}
	else{
		T snr;
		if ( nTotalBins == nSignalBins ){			//no noise
			snr = 2000.0f;							//20*log10(1e+100)
		}
		else{
			A = (A-As)/(nTotalBins-nSignalBins);	//avearge noise
			As = As/nSignalBins;					//average signal
			if ( As > 0.0f ){
				snr = 20 * log10 ( As/( A+eps ) ) ; //snr in db
			}
		}
		return snr;
	}
}


//------------------------------------------------
//cross correlation bewteen signal and mask
//vSignal, nSignal x 1;
//vKernel,   nKernel   x 1,
//vCorr,    nSignal x 1
//------------------------------------------------
template<class T>
T wmatCrossCorrelation(WMat<T>& vCorr, const WMat<T> &vSignal, const WMat<T> &vKernel  )
{
	uint32_t nSignal = vSignal.size();
	uint32_t nKernel = vKernel.size();		//$nMask$ must be odd
	assert( nKernel%2 == 1 );   

	uint32_t kk = nKernel/2;

	//TODO: this is not neccessary to allocate $vPad$, but just for easy programing
	WMat<T> vPad (nSignal+kk*2, 1, 0.0f);
	vPad.setSubCol( 0, kk, kk+nSignal-1, vSignal );

	if( vCorr.size() != nSignal ){
		vCorr.resize(nSignal,1);
	}

	uint32_t k;
	T s, sMax=0.0f;
	for (uint32_t i=0; i<nSignal; i++ ){ //for mask center location
		s=0.0;
		k=i;
		for(uint32_t j=0; j<nKernel; j++, k++){
			s += ( vKernel(j)*vPad(k) );
		}
		vCorr(i) = s;

		if ( s>sMax ){
			sMax = s;
		}
	}

	return sMax;
}


//h1 and h2 have the same size
//h1 and h2 are normalized, i.e. sum(h1)=sum(h2)=1.0;
template<class T>
T wmatCircularCorrelation( const WMat<T>&h1, const WMat<T>&h2 )
{
	uint32_t i, j, k, n = (uint32_t)h1.size();
	assert( n == (uint32_t)h2.size() );

	T roh, rohMax = -1.0f;
	for ( k=0;k<n; k++){
		roh = 0;
		for ( i=0, j=k; i<n-k; ++i, ++j){
			roh += sqrt( h1(j)*h2(i) );
		}
		for ( i=n-k, j=0; i<n; ++i, ++j){
			roh += sqrt( h1(j)*h2(i) );
		}

		if( roh>rohMax ){
			rohMax = roh;
		}
	}
	//printf( "rohMax=%f\n", rohMax);
	return rohMax;
}



