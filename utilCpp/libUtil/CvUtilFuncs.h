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

#ifndef _CV_UTIL_FUNCS_H_
#define _CV_UTIL_FUNCS_H_

#include "AppDefs.h"
#include "Roi.h"
#include "FileUtil.h"
#include "libUtilDefs.h"

namespace app {
	bool  isExternal(const cv::Mat *x);
	bool  isExternal(const cv::cuda::GpuMat *x);

	template<typename _Tp>
	_Tp  sumVal(const cv::Mat &x);
	cv::Mat  sumCols(const cv::Mat &x);
	cv::Mat  sumRows(const cv::Mat &x);

	bool isSameMat(const cv::Mat &x, const cv::Mat &y);
	void matRead(const std::string& filename, cv::Mat &out);
	void matWrite(const std::string& filename, const cv::Mat& mat);

	void  gray2bgr(const cv::Mat &gray, cv::Mat &bgr);

	//inpput I is  bgr, and output I is rgb
	void  swap_br(cv::Mat &I);

	//for any type of given matrix $in$, convert it into a CV_8U type $out$ matrix
	void  convert_to_CV_8U(const cv::Mat &in, cv::Mat &out);
	bool  myCvInv(const cv::Mat &X, cv::Mat &invX, double &det);

	//bool  getCvImgRowStep(const IplImage *img, int &rowStep, int &row0);
	//put ROI image of $src$ into $des$ at $(desXo, desYo)$
	//void  setCvImg(IplImage *des, const int &desXo, const int &desYo, const IplImage *src, const int &srcRoiXo, const int &srcRoiYo, const int &srcRoiW, const int &srcRoiH);


	//void UTILS_EXPORT combineTwoImg_LR( const cv::Mat &imgLeft, const cv::Mat &imgRight, cv::Mat imgOut );

	float  maskNonZeroPercent(const cv::Mat &mask, const int &xo, const int &yo, const int &xb, const int &yb, size_t &n);
	//void  UTILS_EXPORT cvMat2SwfMat( const cv::Mat &x, swfMat &y );
	//void  UTILS_EXPORT swfMat2cvMat( const swfMat &x, cv::Mat &y  );

	//dL>0: increase the size of $in$ by 2^dL 
	//dL<0: decrease the size of $in$ by 2^dL 
	void  rectPyrLevelUpDown(const cv::Rect &in, const int dL, cv::Rect &out);

	//assume the channel order in <x> is BGR, since we call cv::imwrite()
	void  myImgWrite(const cv::Mat &x, const std::string &dir, const std::string &fname, const uint64_t fn, const int L = -1);

	//void  myImgWrite(const IplImage* cvTrkImg, const std::string &myPath, const std::string &fileNameWoExt, const size_t &fn);
	//void  dumpCvImgToTextFile(const IplImage *J, const std::string &fileName);

	void  drawRect(cv::Mat &img, const Roi &rect, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift);
	void  drawPluses(cv::Mat &img, const std::vector<cv::Point2f> &v, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift);
	void  drawPluses(cv::Mat &img, const std::vector<cv::Point> &v, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift);

	void  drawPlus(cv::Mat &img, const cv::Point2f &pt, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift);
	void  drawPlus(cv::Mat &img, const cv::Point &pt, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift);
	void  wrtieStrings(cv::Mat& img, const std::vector<std::string>& vStrs, cv::Point &org, int fontFace, double fontScale, cv::Scalar color, int thickness, int lineType);
}

#endif