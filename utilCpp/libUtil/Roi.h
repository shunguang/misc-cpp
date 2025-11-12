/*
 *------------------------------------------------------------------------
 * abc.h
 *
 * No  any Corporation or client funds were used to develop this code.
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * Copyright (c) 2022 Shunguang Wu
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
#ifndef _ROI_H_
#define _ROI_H_

#include "AppDefs.h"
#include "libUtilDefs.h"

//
//(xo,yo): upper-left corner of the roi,
//(w, h): width and height of the roi.
//(xb,yb): bottom-right corner of the roi, with xb = xo + w - 1 and yb = yo + h - 1.
//

namespace app {
class  Roi{
	public:
		float m_xo, m_yo, m_w, m_h, m_xb, m_yb;  

		inline static int myround(const float &x)
		{
			return (int)((x > 0.0) ? floor(x + 0.5) : ceil(x - 0.5));
		}

	public:
		Roi();
		Roi( const int xo, const int yo, const int w, const int h );
		Roi( const float xo, const float yo, const float w, const float h  );
		Roi( const Roi &p );

		Roi& operator =( const Roi &p);

		float	getCenterX() const	{ return  0.5f*(m_xo + m_xb); }
		float	getCenterY() const	{ return  0.5f*(m_yo + m_yb); }
		float   getSize() const		{ return  m_w * m_h; }
		void    getIntParams(int &xo, int &yo, int &xb, int &yb, int &w, int &h, const int imgW=-1, const int imgH=-1) const;
		Roi     getRoi(const int L) const;

		int		getInt_xc() const   { return  myround( 0.5f*(m_xo + m_xb ) ); }
		int		getInt_yc() const   { return  myround( 0.5f*(m_yo + m_yb ) ); }
		int		getInt_xo() const   { return  myround( m_xo ); }
		int		getInt_yo() const   { return  myround( m_yo ); }
		int		getInt_xb() const   { return  myround( m_xb ); }
		int		getInt_yb() const	{ return  myround( m_yb ); }
		int		getInt_w() const	{ return  myround( m_w  ); }
		int		getInt_h() const	{ return  myround( m_h  ); }

		bool	invalid() const						{ return ( m_w<=0 || m_h<=0 ); }
		bool    sameSize(int w, int h) const		{ return ( w == getInt_w() && h == getInt_h() ); }
		bool	sameCenter(int xc, int yc) const	{ return (xc == getInt_xc() && yc == getInt_yc()); }

		void	setRoi( const int &xo, const int &yo, const int &w, const int &h );
		void	setRoi( const float &xo, const float &yo, const float &w, const float &h  );
		void    setByCenter( const float &xc, const float &yc, const float &w, const float &h);
		void	setByIntCenter( const int &xc, const int &yc, const int &w, const int &h);

		void	print ( const char *str = NULL ) const;

		//force roi inside the image via change the roi size
		bool	trimSize( const int imgW, const int imgH );
		//force roi inside the image via change the roi location, may change roi size
		bool	trimLocation( const int imgW, const int imgH );
		//force roi inside the image via change the roi location, w/o  change roi size
		bool	trimLocation_wo_chg_sz( const int imgW, const int imgH );
		
		void	oneLevelUp();
		void	oneLevelDown();
		void    oneLevelUpOrDown( const int &step, const float &newW, const float &newH );
		void    forceSizeOdd();
		void    forceSizeOdd2();

		void	move2L( const int &step ) { m_xo -=step; m_xb -= step; }
		void	move2R( const int &step ) { m_xo +=step; m_xb += step; }
		void	move2U( const int &step ) { m_yo -=step; m_yb -= step; }
		void	move2D( const int &step ) { m_yo +=step; m_yb += step; }
		void    fromString(const std::string &s);
		std::string   toString();
		std::string   toString2();
#if 0
		inline cv::Rect toCvRect() const{
			cv::Rect rect( myround(m_xo), myround(m_yo), myround(m_w), myround(m_h) );
			return rect;
		}
#endif
};

bool  isRoiOutOfImg( const Roi &roi, const int &frmImgW, const int &frmImgH );
bool  isRoi1InRoi2( const Roi &roi1, const Roi &roi2 );
}	//end namespace util
#endif

