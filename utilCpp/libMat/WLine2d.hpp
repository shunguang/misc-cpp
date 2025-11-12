/*
 *------------------------------------------------------------------------
 * WLine2d.hpp -  a 2d line tool with interface of WMat<T>
 *$Id: WLine2d.cpp,v 1.1 2010/06/11 18:37:14 swu Exp $
 * Copyright (c) 2006 Shunguang Wu
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
//In this calss, a 2D line is defined by three parameters, i.e., a*x + b*y + c=0.

template <class T>
WLine2d<T> :: WLine2d() 
	: m_a( 0 )
	, m_b( 0 )
	, m_c( 0 )
	, m_begPt(1,2)
	, m_endPt(1,2)
	, m_isLineSegment ( false )
{
}

template <class T>
WLine2d<T> :: WLine2d( const T a, const T b, const T c )
	: m_a( a )
	, m_b( b )
	, m_c( c )
	, m_begPt(1,2)
	, m_endPt(1,2)
	, m_isLineSegment ( false )
{
}

/*
%----------------------------
%input
% p1, 1 x 2, pt1
% p2, 1 x 2, pt2
%output
% a*x + b*y + c = 0
%----------------------------
*/
template <class T>
WLine2d<T> :: WLine2d( const WMat<T> &p1, const WMat<T> &p2, const bool isLineSegment)
	: m_a( 0 )
	, m_b( 0 )
	, m_c( 0 )
	, m_begPt(1,2)
	, m_endPt(1,2)
	, m_isLineSegment (isLineSegment)
{
	resetLine( p1, p2, isLineSegment);
}

/*
%----------------------------
%input
% p, 1 x 2, pt1
% k, 1 x 1, the slope
%----------------------------
*/
template <class T>
WLine2d<T> :: WLine2d( const WMat<T> &p, const T slope )
	: m_a( 0 )
	, m_b( 0 )
	, m_c( 0 )
	, m_begPt(1,2)
	, m_endPt(1,2)
	, m_isLineSegment ( false )
{
	resetLine( p, slope );
}

template <class T>
T WLine2d<T> :: getDist( const WMat<T> &p ) const
{
	T tmp = sqrt ( m_a*m_a + m_b*m_b );

	if ( tmp > WMAT_F32_EPS ){
		return fabs( m_a * p(0) + m_b * p(1) + m_c ) / tmp;
	}
	else{
		wWarningMsg( "WLine2d<T> :: getDist(): sth wrong about the line");
		return WMAT_F32_REALMAX ;
	}
}

/*
%----------------------------
%get the intersection pt between this line and and the given line $L$
% line1: a1x+b1y+c1 = 0
% line2: a2x+b2y+c2 = 0
%output
% xy, 1 x 2, the intersection pt
% isExist, 1 x 1, if exist flag
%----------------------------
*/
template <class T>
bool WLine2d<T> :: getIntersectionPoint( const WLine2d<T> &L, WMat<T> &intsectionPt) const
{
	T a2 = L.getA();
	T b2 = L.getB();
	T c2 = L.getC();

	T e = m_a*b2 - a2*m_b;

	if ( fabs(e) < WMAT_F32_EPS ){
		intsectionPt(0) = WMAT_F32_REALMAX ;
		intsectionPt(1) = WMAT_F32_REALMAX ;
		return false;
	}
    
	intsectionPt(0) = ( c2*m_b - m_c*b2 ) / e ;
	intsectionPt(1) = ( a2*m_c - m_a*c2 ) / e ;

	return true;
}

//given y on the line, find x
template <class T>
T WLine2d<T> :: getX( const T y) const
{
	if (isParallelAxisX()) {
		//the line is parallel to x-axis, x can be any value.
		return  WMAT_F32_REALMAX;
	}
	else {
		return  -(m_b * y + m_c) / m_a;
	}
}

//given x on the line, find y
template <class T>
T WLine2d<T> :: getY( const T x) const
{
	if (isParallelAxisY()) {
		//the line is parallel to x-y, y can be any value.
		return  WMAT_F32_REALMAX;
	}
	else {
		return  -(m_a * x + m_c) / m_b;
	}
}


template <class T>
bool WLine2d<T> :: isParallelAxisX() const
{
	return ( fabs(m_a) <= WMAT_F32_EPS );
}

template <class T>
bool WLine2d<T> :: isParallelAxisY() const
{
	return ( fabs(m_b) <= WMAT_F32_EPS );
}


template <class T>
bool WLine2d<T> :: isCloseAxisX() const
{
	if ( isParallelAxisY() ){
		return false;
	}
	else{
		T slop = -m_a/m_b;
		if ( slop >= -1.0f && slop<=1.0f ){
			return true;
		}
		else{
			return false;
		}
	}
}

//given the angle (in rad.) between line and x-axis	
template <class T>
T WLine2d<T> :: getSlopeAng() const
{
	if ( isParallelAxisY() ){
		return (T)WMAT_HALF_PI;
	}
	return atan( -m_a/m_b );
}

template <class T>
bool WLine2d<T> :: isPointInsideLineSegment( WMat<T> &p) const
{
	if ( !m_isLineSegment ){
		wWarningMsg("Warning!!! at WLine2d<T> :: isPointInsideLineSegment(): this is not a line segment!" ); 
		return false;
	}

	T d12 = dist2D( m_begPt, m_endPt); 
	T d1 = dist2D( p, m_begPt ); 
	T d2 = dist2D( p, m_endPt ); 

	if (fabs(d12 - (d1 + d2)) < WMAT_F32_EPS) {
		return true;
	}
	return false;
}

/*
%-----------------------------
%input
%   givenPt, 1 x 2, the coordinates of the pt
%output
%   intersectionPt, 1 x 2, the closet pt from $givenPt$ to the points on the line between
%   [m_begPt, m_endPt]
%-----------------------------
*/
template <class T>
T WLine2d<T> :: getMinDistFromPt2LineSegment( const WMat<T> &givenPt, WMat<T> &intersectionPt) const
{
	//makesure p1 is at the left and p2 is at the right
	WMat<T> p1(1, 2);
	WMat<T> p2(1, 2);
	if ( m_begPt(0) > m_endPt(0) ){
		p1= m_endPt;
		p2= m_begPt;
	}
	else{
		p1= m_begPt;
		p2= m_endPt;
	}

	//find the interscetion pts between line p1p2 (ax + bz + c = 0) and its perpedicular line
	//which passes $pt$ 
	WLine2d<T> L2;
	if ( isParallelAxisX() ){
		WMat<T> synPt( givenPt );
		synPt(1) += 1.0;
		L2.resetLine ( givenPt, synPt );
	}
	else {
		T slope = m_b/m_a;  //the slope of the line  who is perpendicular to $p1  p2$
		L2.resetLine( givenPt, slope );
	}
    
	bool isExist = this -> getIntersectionPoint( L2, intersectionPt);
	assert( isExist );  //$isExist$ should always be $true$ at this point

	bool isInside =  this -> isPointInsideLineSegment( intersectionPt ); 

	if ( isInside )
		return dist2D( givenPt, intersectionPt );

	//cal. the distance from $pt$ to the two end points
	T d1 = dist2D(givenPt, p1);
	T d2 = dist2D(givenPt, p2);

	if ( d1 < d2 ){
		intersectionPt = p1;
		return d1;
	}
	else{
		intersectionPt = p2;
		return  d2;
	}
}

template <class T>
WMat<T> WLine2d<T> :: getPerpendicularProjPt( const WMat<T> &givenPt ) const
{
	WMat<T> projPtOnLine(1, 2, 0.0);

	if ( isParallelAxisX () ) {
		projPtOnLine(0) = givenPt( 0 );
		projPtOnLine(1) = getY( givenPt(0) );
	}
	else if ( isParallelAxisY() ) {
		projPtOnLine(0) = getX( givenPt(1) );
		projPtOnLine(1) = givenPt( 1 );
	}
	else{
		WLine2d<T> L1( givenPt, m_b/m_a );
		bool isIntersetion = getIntersectionPoint( L1,  projPtOnLine );
		if ( !isIntersetion ){
			wErrMsg ( "WLine2d<T> :: getPerpendicularProjPt(): sth is wrong!" );
		}
	}

	return  projPtOnLine;
}

//p1, 1 x 2;
//p2, 1 x 2;
template <class T>
T WLine2d<T> ::  dist2D( const WMat<T> &p1, const WMat<T> &p2) const
{
	T dx = p1(0) - p2(0);
	T dy = p1(1) - p2(1);

	return  sqrt( dx*dx + dy * dy);
}


template <class T>
bool WLine2d<T> :: resetLine( const WMat<T> &p1, const WMat<T> &p2, const bool isLineSegment)
{
	T d;

	d = dist2D (p1, p2);
	if ( d < WMAT_F32_EPS ){  //the line is parallel to y-axis
		wWarningMsg( "WLine2d<T> :: resetLine():  two points are too closer!");
		m_isLineSegment = false;
		return false;
	}


	d = p2(0) - p1(0);
	if ( fabs(d) < WMAT_F32_EPS ){  //the line is parallel to y-axis
		m_a = 1.0;
		m_b = 0.0;
		m_c = -p1(0);
	}
	else{
		m_a  = ( p2(1) - p1(1) ) / d ;
		m_b = -1.0;
		m_c = p1(1) - m_a*p1(0);
	}

	m_isLineSegment = isLineSegment;
	m_begPt = p1;
	m_endPt = p2;
	return true;
}


template <class T>
bool WLine2d<T> :: resetLine( const WMat<T> &p, const T slope )
{
	m_a = slope;
	m_b = -1.0;
	m_c = p(1)-slope*p(0);

	m_isLineSegment = false;
	return true;
}

//-------------------------------------------------------------------------------
//vXY: m x 2 (m>=2), given m-point, useing lsq method to fit a line: y = ax +b 
//    each row is [x,y]
// recall our line is: 	m_a*x + m_b*y + m_c=0.
//-------------------------------------------------------------------------------
template <class T>
bool WLine2d<T>::resetLineLsq(const WMat<T>& vXY)
{
	//vXY.print("vXY =");

	uint32_t m = vXY.rows();
	uint32_t n = vXY.cols();
	if (m < 2 || n !=2 ) {
		app::wWarningMsg("WLine2d<T>::resetLineLsq(): in put matrix size does not match!, (m=%d, n=%d)", m, n);
		return false;
	}
	
	T det;

	//Ax = y =>  A' * A x = A' * y => x = inv(A'*A) * (A'*y), where x =[a,b]
	WMat<T> A(m, 2);              //m x 2
	WMat<T> y(m, 1);              //m x 1

	WMat<T> AT(2, m);              //2 x m
	WMat<T> ATA(2, 2);
	WMat<T> invATA(2, 2);
	WMat<T> ATy(2, 1);
	WMat<T> x(2, 1);              //[a,b]';


	A.setCol(0, vXY.getCol(0));   //A(:,0)=x[i]
	A.setCol(1, 1);               //A(:,1)=1;
	y = vXY.getCol(1);            //y[i], m x 1

	AT = A.transpose();
	ATA = AT  % A;        // % -- matrix times
	ATy = AT  % y;        // % -- matrix times

	bool suc =  wmatInv2x2<T>( ATA, invATA, det);
	if (!suc) {
		return false;
	}

	//solve ans assign class params (m_a, m_b, m_c)
	//recall our line is: 	m_a*x + m_b*y + m_c=0.
	y = invATA % ATy;   //a=y[0],b=y[1]
	m_a = y(0);
	m_b = -1;
	m_c = y(1);

	m_isLineSegment = true;
	m_begPt = vXY.getRow(0);
	m_endPt = vXY.getRow(m-1);
	return true;
}

//-------------------------------------------------------------
//input
//sampling n equal-distance pts from the center of (x0,y0) on the line
//(x0,y0): given point on the line
//distL:   the distance between two sampling pts
//n:       total # of sampleing pts
//
//output
//tab:     2 x n, return points table
//-------------------------------------------------------------
template <class T>
void WLine2d<T> :: getSamplePtsAloneLine(WMat<T>& tab, const T x0, const T y0,
									   const T distL, const int n  )
{
	//const T c1 = atan(1.0f);
	int m = n/2;
	bool isCloseX = isCloseAxisX();
	T theta = getSlopeAng();
	if( isCloseX ){
		//assert( theta >= -c1 && theta <= c1 );
		T step = distL * cos(theta);  
		T x = x0-m*step;
		for(int i=0; i<n; i++){
			tab(0, i) = x;
			tab(1, i) = getY(x);
			x += step;
		}
	}
	else{
		//assert( theta < -c1 || theta > c1 );
		T step = fabs( distL * sin(theta) );
		T y = y0-m*step;
		for(int i=0; i<n; i++){
			tab(0, i) = getX(y);
			tab(1, i) = y;
			y += step; 
		}
	}
}

//-------------------------------------------------------------
//input
//sampling n equal-distance pts start from (x0,y0), and along particular direction
//(x0,y0):		given point on the line
//dirAngInRad:  theta or theta + pi, where tan(theta)=slope of the line
//distL:		the distance between two sampling pts
//n:			total # of sampleing pts
//
//output
//tab:     2 x n, return points table
//-------------------------------------------------------------
template <class T>
void WLine2d<T> :: getSamplePtsAloneLine(WMat<T>& tab, const T x0, const T y0, const T dirAngInRad,
									   const T distL, const int n)
{
	//const T c1 = atan(1.0f);
	bool isCloseX = isCloseAxisX();
	if( isCloseX ){
		//assert( theta >= -c1 && theta <= c1 );
		T step = distL * cos(dirAngInRad);  
		T x = x0;
		for(int i=0; i<n; i++){
			tab(0, i) = x;
			tab(1, i) = getY(x);
			x += step;
		}
	}
	else{
		//assert( theta < -c1 || theta > c1 );
		T step = distL * sin(dirAngInRad) ;
		T y = y0;
		for(int i=0; i<n; i++){
			tab(0, i) = getX(y);
			tab(1, i) = y;
			y += step; 
		}
	}
}

template <class T>
std::string WLine2d<T> ::toString( const char *msg ) const
{
	std::ostringstream oss;
	oss << msg << "Line:  a*x + b*y + c=0, (a=" << m_a << ", b=" << m_b << ", c=" << m_c << ")" << std::endl;
	return oss.str();
}
