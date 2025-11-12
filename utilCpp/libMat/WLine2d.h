/*
 *------------------------------------------------------------------------
 * WLine2d.h - the class about 2D line operations
 *
 * $Id: WLine2d.h,v 1.1 2010/06/11 18:37:14 swu Exp $
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

//In this class, a 2D line is defined by three parameters, i.e., a*x + b*y + c=0.

#ifndef __WLINE_2D_H__
#define __WLINE_2D_H__ 1

#include "WMat.h"
#include "WUtil.h"
#include "WLu.h"

namespace app{
	template<class T>
	class  WLine2d {
	public:
		//set line from two intersection pts
		WLine2d();
		WLine2d(const T a, const T b, const T c);
		WLine2d(const WMat<T>& p1, const  WMat<T>&  p2, const bool isLineSegment);
		WLine2d(const WMat<T>&  p, const T slope);

		//a line is defined by three parameters, i.e., a*x + b*y + c=0.
		//user can get these parameters by calling the following functions
		T getA() const { return m_a; }
		T getB() const { return m_b; }
		T getC() const { return m_c; }

		T getX(const T y) const;        //given y on the line, find x
		T getY(const T x) const;        //given x on the line, find y
		T getSlopeAng() const;				  //given the angle (in rad.) between line and x-axis	
		T getDist(const WMat<T>& pt) const;  //get the distance from a pt to the line 
		bool getIntersectionPoint(const WLine2d<T>& L, WMat<T>& intsectionPt) const;

		//Find the minimum distance from a pt to a line segment if $m_isLineSegment$ is true
		//return the minDist
		T getMinDistFromPt2LineSegment(const WMat<T>& givenPt, WMat<T>& intersectionPt) const;

		//Find the perpendicular project pt on the line from a given pt
		//return the projection point on the THIS line,which is a $1 x 2$ matrix
		WMat<T> getPerpendicularProjPt(const WMat<T>& givenPt) const;

		//return n equal distance sample pts long the line but around (x0,y0), 
		//which means each side has equal # of point if n is odd.
		void getSamplePtsAloneLine(WMat<T>& tab, const T x0, const T y0, const T distL, const int n);

		//return n sample pts starting from (x0,y0) on the line along $dirAngInRad$ direction
		void getSamplePtsAloneLine(WMat<T>& tab, const T x0, const T y0, const T dirAngInRad, const T distL, const int n);

		bool isParallelAxisX() const;
		bool isParallelAxisY() const;
		bool isLineSegment() const { return m_isLineSegment; }
		bool isPointInsideLineSegment(WMat<T>& p) const;
		bool isCloseAxisX() const;
		bool resetLine(const WMat<T>& p1, const WMat<T>& p2, const bool isLineSegment);
		bool resetLine(const WMat<T>& p, const T slope);

		//vXY: m x 2 (m>=2), given m-point, useing lsq method to fit a line: y = ax +b 
		bool resetLineLsq(const WMat<T>& vXY );

		std::string toString(const char* msg) const;
		void print(const char* msg) const {
			std::cout << toString(msg) << std::endl;
		}
	private:
		T dist2D(const WMat<T>& p1, const WMat<T>& p2) const;
		//a line is defined by three parameters, i.e., a*x + b*y + c=0.
		T  m_a;
		T  m_b;
		T  m_c;

		//if the following two pts exist, this is a line-segment
		bool     m_isLineSegment;
		WMat<T>  m_begPt;
		WMat<T>  m_endPt;
	};

	typedef  WLine2d<float>   WLine2dF32;
	typedef  WLine2d<double>  WLine2dF64;

	#include "WLine2d.hpp"
}
#endif
