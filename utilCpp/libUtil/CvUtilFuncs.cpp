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

#include "CvUtilFuncs.h"

using namespace cv;
using namespace std;


bool app::isExternal(const cv::Mat *x)
{
	if (x->data == NULL) {
		return  true;
	}
	else if (x->u == 0) {
		return  true;
	}
	else if (x->u->refcount == 0) {
		return true;
	}

	return false;
}


bool app::isExternal(const cv::cuda::GpuMat *x)
{
	if (x->data == NULL) {
		return  true;
	}
	else if ( x->refcount == NULL ) {
		return true;
	}
	return false;
}

template<typename _Tp>
_Tp app::sumVal(const cv::Mat &x)
{
	_Tp s = 0;
	for (int i = 0; i<x.rows; ++i) {
		const _Tp *p = x.ptr<_Tp>(i);
		for (int j = 0; j<x.cols; ++j) {
			s += p[j];
		}
	}

	return s;
}


//return a col vector
Mat app::sumCols(const cv::Mat &x)
{
	//cout<<x<<endl;
	int n = x.cols;
	Mat y = Mat::ones(n, 1, x.type());
	//cout<<x*y<<endl;
	return x*y;
}

//return a row vector
Mat app::sumRows(const cv::Mat &x)
{
	//cout<<x<<endl;

	int m = x.rows;
	Mat y = Mat::ones(1, m, x.type());

	//cout<<y*x<<endl;
	return y*x;
}

//class static function
void app::gray2bgr(const cv::Mat &gray, cv::Mat &bgr)
{
	vector<Mat> ch;
	ch.push_back(gray);
	ch.push_back(gray);
	ch.push_back(gray);
	merge(ch, bgr);
}

void app::swap_br(cv::Mat &I)
{
	//change from opencv bgr order to rgb order for cuda processing
	cv::Mat	bgr[3];   //typically come from cv::imread()
	cv::split(I, bgr);

	//shoft copy ro <rgb>
	std::vector<cv::Mat> rgb(3);
	rgb[0] = bgr[2];	//soft cp
	rgb[1] = bgr[1];	//soft cp
	rgb[2] = bgr[0];	//soft cp
	
	//merge to rgb
	cv::merge(rgb, I);
}

void app::convert_to_CV_8U(const cv::Mat &in, cv::Mat &out)
{
	double	minVal, maxVal;
	//int		minIdx, maxIdx;

	minMaxIdx(in, &minVal, &maxVal);

	printf("minVal=%.2f, maxVal=%.2f\n", minVal, maxVal);

	double d = maxVal - minVal;
	Mat tmp;
	in.convertTo(tmp, CV_32F, 1, 0);
	if (fabs(d) > 1e-6) {
		tmp = (tmp - minVal)*(255.0 / d);
	}
	tmp.convertTo(out, CV_8U, 1, 0);
}



bool app::myCvInv(const cv::Mat &X, cv::Mat &invX, double &det)
{
	det = determinant(X);
	if (det < APP_EPS_32F) {
		return false;
	}

	invX = X.inv();

	return true;
}

float app::maskNonZeroPercent(const Mat &mask, const int &xo, const int &yo, const int &xb, const int &yb, size_t &n)
{
#if 1
	assert(xo >= 0 && xb<mask.cols && xo <= xb);
	assert(yo >= 0 && yb<mask.rows && yo <= yb);
	assert(mask.depth() == 8);
#endif

	size_t s = 0;
	uchar *col;
	for (int y = yo; y <= yb; ++y) {
		col = mask.data + mask.step[0] * y + mask.step[1] * xo;
		for (int x = xo; x <= xb; ++x) {
			if (*col++ > 0)
				s++;
		}
	}

	size_t a = (xb - xo + 1)*(yb - yo + 1);
	n = s;

	if (a>0)
		return (float)s / (float)a;
	else
		return -1.0f;
}

void app::rectPyrLevelUpDown(const cv::Rect &in, const int dL, cv::Rect &out)
{
	if (dL == 0) {
		out = in;
		return;
	}

	int x(in.x), y(in.y), w(in.width), h(in.height);
	if (dL>0) {
		x = x << dL;
		y = y << dL;
		w = w << dL;
		h = h << dL;
	}
	else {
		int L = -dL;
		x = x >> L;
		y = y >> L;
		w = w >> L;
		h = h >> L;
	}

	out = cv::Rect(x, y, w, h);
}

#if 0
void app::setCvImg(IplImage *des, const int &desRoiXo, const int &desRoiYo,
	const IplImage *src, const int &srcRoiXo, const int &srcRoiYo, const int &srcRoiW, const int &srcRoiH)
{
#if _DEBUG
	assert(desRoiXo >= 0 && desRoiYo >= 0);
	assert(srcRoiXo >= 0 && srcRoiYo >= 0);
	assert(srcRoiXo + srcRoiW <= src->width && srcRoiYo + srcRoiH <= src->height);
	//	assert( desRoiXo+srcRoiW <= des->width && desRoiYo+srcRoiH <= src->height );
#endif

	//this version only works for 8 bits images
	assert(des->depth == IPL_DEPTH_8U);
	assert(src->depth == IPL_DEPTH_8U);

	int w = des->width;
	int h = des->height;

	int desRowStep, desRow0;
	int srcRowStep, srcRow0;

	getCvImgRowStep(des, desRowStep, desRow0);
	getCvImgRowStep(src, srcRowStep, srcRow0);

	//roi in des image
	const int &xo = desRoiXo;
	const int &yo = desRoiYo;
	int xb = xo + srcRoiW - 1;
	if (xb > w - 1) xb = w - 1;
	int yb = yo + srcRoiH - 1;
	if (yb > h - 1) yb = h - 1;

	int srcColOffset = src->nChannels * srcRoiXo * sizeof(char);
	int desColOffset = des->nChannels * desRoiXo * sizeof(char);

	unsigned char *srcRow = (unsigned char *)src->imageData + srcRowStep*srcRoiYo + srcColOffset;
	unsigned char *desRow = (unsigned char *)des->imageData + desRowStep*desRoiYo + desColOffset;
	if (src->nChannels == des->nChannels) {
		int oneRoiRowSize = src->nChannels * (xb - xo + 1) * sizeof(char);
		for (int y = yo; y <= yb; ++y, desRow += desRowStep, srcRow += srcRowStep) {
			memcpy(desRow, srcRow, oneRoiRowSize);
		}
	}
	else if (src->nChannels == 1 && des->nChannels == 3) {
		for (int y = yo; y <= yb; ++y, desRow += desRowStep, srcRow += srcRowStep) {
			unsigned char *pSrc = srcRow + xo;
			unsigned char *pDes = desRow + 3 * xo;
			for (int x = xo; x <= xb; ++x, ++pSrc) {
				*pDes++ = *pSrc;
				*pDes++ = *pSrc;
				*pDes++ = *pSrc;
			}
		}
	}
	else if (src->nChannels == 3 && des->nChannels == 1) {
		//todo
		assert(0);
	}
	else {
		assert(0);
	}
}

bool app::getCvImgRowStep(const IplImage *img, int &rowStep, int &row0)
{

	if (!img) {
		rowStep = -1;
		row0 = -1;
		return false;
	}

	int h = img->height;
	row0 = 0;
	rowStep = img->widthStep;				 //assume ipl image starts from  top left

	if (img->origin == 1) {				  	 //if ipl image starts from bottom left
		row0 = (h - 1)*img->widthStep;
		rowStep = -rowStep;
	}

	return true;
}
#endif


void app::myImgWrite(const cv::Mat &x, const std::string &dir, const std::string &fname, const uint64_t fn, const int L)
{
	char buf[1024];
	if (L >= 0) {
		snprintf(buf, 1024, "%s/%s-w%dxh%d-fn%09llu-L%d.png", dir.c_str(), fname.c_str(), x.cols, x.rows, fn, L);
	}
	else {
		snprintf(buf, 1024, "%s/%s-w%dxh%d-fn%09llu.png", dir.c_str(), fname.c_str(), x.cols, x.rows, fn);
	}

	printf("!!! Writting file: %s\n", buf);

	cv::imwrite(buf, x);
}

#if 0
void app::myImgWrite(const IplImage* cvTrkImg, const string &myPath, const string &fileNameWoExt, const size_t &fn)
{
	char fileName[1024];
	snprintf(fileName, 1024, "%s/%s-%05d.png", myPath.c_str(), fileNameWoExt.c_str(), fn);

	if (!folderExists(myPath)) {
		printf("app::myCvImgSave(): Path: %s is not exist!\n", myPath.c_str());
		printf("app::myCvImgSave(): ould not save file: %s\n", fileName);
		return;
	}

	if (!cvSaveImage(fileName, cvTrkImg)) {
		printf("app::myCvImgSave():  Could not save: %s\n", fileName);
	}
	else {
		printf("app::myCvImgSave():  File saved: %s\n", fileName);
	}
}

void app::dumpCvImgToTextFile(const IplImage *J, const string &fileName)
{
	int w = J->width;
	int h = J->height;
	int iplRow0(0), iplRowStep(J->widthStep);  //assume ipl image starts from  top left
	if (J->origin == 1) {						//if ipl image starts from bottom left
		iplRow0 = (h - 1)*J->widthStep;
		iplRowStep = -iplRowStep;
	}

	FILE *fp = fopen(fileName.c_str(), "w");
	unsigned char *pRow = (unsigned char *)J->imageData + iplRow0;
	if (J->nChannels == 1) {
		for (int i = 0; i<h; ++i, pRow += iplRowStep) {
			unsigned char *p2 = pRow;
			for (int j = 0; j<w - 1; ++j) {
				fprintf(fp, "%d,", *p2++);
			}
			fprintf(fp, "%d\n", *p2++);
		}
	}
	else if (J->nChannels == 3) {
		for (int i = 0; i<h; ++i, pRow += iplRowStep) {
			unsigned char *p2 = pRow;
			unsigned char *pixel = p2;
			for (int j = 0; j<w; ++j, pixel += 3) {
				fprintf(fp, "(%3d,%3d,%3d)", pixel[0], pixel[1], pixel[2]);
			}
			fprintf(fp, "\n");
		}
	}
	else {
		assert(0);
	}

	fclose(fp);
}
#endif


void app::drawPlus(cv::Mat &img, const cv::Point2f &pt, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	Point p(pt.x, pt.y);
	drawPlus(img, p, len, color, thickness, lineType, shift);
}

void app::drawPlus(Mat &img, const Point &pt, const uint32_t len, const Scalar& color, int thickness, int lineType, int shift)
{
	Point pt1(pt.x - len, pt.y);
	Point pt2(pt.x + len, pt.y);
	line(img, pt1, pt2, color, thickness, lineType, shift);

	pt1.x = pt.x; pt1.y = pt.y - len;
	pt2.x = pt.x; pt2.y = pt.y + len;
	line(img, pt1, pt2, color, thickness, lineType, shift);
}

void app::drawPluses(cv::Mat &img, const std::vector<cv::Point2f> &v, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	BOOST_FOREACH(const cv::Point2f &p, v){
		drawPlus( img, p, len, color, thickness, lineType, shift);
	}
}

void app::drawPluses(cv::Mat &img, const std::vector<cv::Point> &v, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	BOOST_FOREACH(const cv::Point &p, v) {
		drawPlus(img, p, len, color, thickness, lineType, shift);
	}
}

void app::wrtieStrings(Mat& img, const vector<string>& vStrs, Point &org, int fontFace, double fontScale, Scalar color, int thickness, int lineType)
{
	std::vector<std::string>::const_iterator it = vStrs.begin();
	Point pt(org);
	for (; it != vStrs.end(); ++it) {
		putText(img, *it, pt, fontFace, fontScale, color, thickness, lineType);
		pt.y += 10;
	}
}

void app::drawRect(cv::Mat &img, const Roi &rect, const uint32_t len, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	//LT
	Point p1(rect.getInt_xo(), rect.getInt_yo());
	Point p2(p1.x + len, p1.y);
	line(img, p1, p2, color, thickness, lineType, shift);
	p2.x = p1.x; p2.y = p1.y + len;
	line(img, p1, p2, color, thickness, lineType, shift);

	//RT
	p1.x = rect.getInt_xb();  //p1.y no change
	p2.x = p1.x; p2.y = p1.y + len;
	line(img, p1, p2, color, thickness, lineType, shift);
	p2.x -= len; p2.y = p1.y;
	line(img, p1, p2, color, thickness, lineType, shift);

	//RB
	//p1.x = rect.getInt_xb();  
	p1.y = rect.getInt_yb();
	p2.x = p1.x; p2.y = p1.y - len;
	line(img, p1, p2, color, thickness, lineType, shift);
	p2.x -= len; p2.y = p1.y;
	line(img, p1, p2, color, thickness, lineType, shift);

	//LB
	p1.x = rect.getInt_xo();  
	//p1.y = rect.getInt_yb();
	p2.x = p1.x; p2.y = p1.y - len;
	line(img, p1, p2, color, thickness, lineType, shift);
	p2.x += len; p2.y = p1.y;
	line(img, p1, p2, color, thickness, lineType, shift);
}


void app::matWrite(const string& filename, const Mat& mat)
{
    ofstream fs(filename, fstream::binary);

    // Header
    int type = mat.type();
    int channels = mat.channels();
    fs.write((char*)&mat.rows, sizeof(int));    // rows
    fs.write((char*)&mat.cols, sizeof(int));    // cols
    fs.write((char*)&type, sizeof(int));        // type
    fs.write((char*)&channels, sizeof(int));    // channels

    // Data
    if (mat.isContinuous()){
        fs.write(mat.ptr<char>(0), (mat.dataend - mat.datastart));
    }
    else{
        int rowsz = CV_ELEM_SIZE(type) * mat.cols;
        for (int r = 0; r < mat.rows; ++r){
            fs.write(mat.ptr<char>(r), rowsz);
        }
    }

    //printf("isContinuous=%d\n", mat.isContinuous());
}

void app::matRead(const string& filename, Mat &out)
{
    ifstream fs(filename, fstream::binary);

    // Header
    int rows, cols, type, channels;
    fs.read((char*)&rows, sizeof(int));         // rows
    fs.read((char*)&cols, sizeof(int));         // cols
    fs.read((char*)&type, sizeof(int));         // type
    fs.read((char*)&channels, sizeof(int));     // channels

    if( out.cols != cols || out.rows != rows || type != out.type() ){
		printf("app::matRead(): wrong size or type!\n");
		assert(0);
        //out.create(rows,cols,type);
    }

	if( out.isContinuous() ){
    	fs.read((char*)out.data, CV_ELEM_SIZE(type) * rows * cols);
	}
	else{
        int rowsz = CV_ELEM_SIZE(type) * out.cols;
        for (int r = 0; r < out.rows; ++r){
            fs.read( out.ptr<char>(r), rowsz);
        }
	}
}

bool app::isSameMat(const Mat &x, const Mat &y)
{
    int type = x.type();
    int w = x.cols, h = x.rows;
    int nCh = x.channels();

    if(w!=y.cols || h!=y.rows || type != y.type() || nCh != y.channels() ){
        return false;
    }


    cv::Mat diff(h, w, type);
    cv::absdiff(x, y, diff);
    cv::Scalar v = cv::sum(diff);
    for (int k = 0; k < 3; ++k){
        if (v[k]>0)
            return false;
    }
    return true;

#if 0        
    double      minVal, maxVal;
    cv::Point   minLoc, maxLoc;
    if (nCh > 1) {
        Mat ch[3];
        cv::split(diff, ch);
        for (int k = 0; k < 3; ++k){
            cv::minMaxLoc(ch[k], &minVal, &maxVal, &minLoc, &maxLoc);
            printf("k=%d, v=(%f,%f,%f), minVal=%f, maxVal=%f\n", k, v[0], v[1], v[2], minVal, maxVal);
        }
    }
    else{
        cv::minMaxLoc(diff, &minVal, &maxVal, &minLoc, &maxLoc);
        printf("v=(%f,%f,%f), minVal=%f, maxVal=%f\n", v[0], v[1], v[2], minVal, maxVal);
    }
#endif

}
