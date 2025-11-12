/*
/*
 *------------------------------------------------------------------------
 * WMatImplPart3.cpp - partial emulation of matrix computation in MatLab
 *
 * No  any Corporation or client funds were used to develop this code.
 *
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

 //check if x(r0:r1,c0:c1) is inside this matrix
#if WMAT_BOUNDS_CHECK
template <class T>
void WMat<T> ::checkSliceBoundary(const char* tag, const uint32_t bufDataSize,
	const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1) const
{
	if (c0 < 0 || c1 >= m_cols) {
		wErrMsg("%s: column numbers (c0=%u,c1=%u, m_cols=%u) are out of order!", tag, c0, c1, m_cols);
	}
	if (c0 > c1) {
		wErrMsg("%s: c0 must be smaller than or equal to c1(c0=%u,c1=%u)!", tag, c0, c1);
	}

	if (r0 < 0 || r1 >= m_rows) {
		wErrMsg("%s: row numbers (r0=%u,r1=%u) are out of order!", tag, r0, r1);
	}

	if (r0 > r1) {
		wErrMsg("%s: r0 must be smaller than or equal to r1(r0=%u,r1=%u)!", tag, r0, r1);
	}

	uint32_t  rowL = r1 - r0 + 1;
	uint32_t  colL = c1 - c0 + 1;
	if (bufDataSize < rowL * colL) {
		wErrMsg("%s, not enough data expected data size=%u, actual bufDataSize=%u", rowL * colL, bufDataSize);
	}
}
#endif

template <class T>
void WMat<T> ::setSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1,
	const T val)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSlice()-A", (r1 - r0 + 1) * (c1 - c0 + 1), r0, r1, c0, c1);
#endif

	for (uint32_t i = r0; i <= r1; i++) {
		T* pDes = getRowAddress(i) + c0;
		for (uint32_t j = c0; j <= c1; j++) {
			*pDes++ = val;
		}
	}
}

template <class T>
void WMat<T> ::setSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1,
	const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSlice()-B", bufSize, r0, r1, c0, c1);
#endif

	const T* pSrc = buf;
	for (uint32_t i = r0; i <= r1; i++) {
		T* pDes = getRowAddress(i) + c0;
		for (uint32_t j = c0; j <= c1; j++) {
			*pDes++ = *pSrc++;
		}
	}
}

template <class T>
void WMat<T> ::setSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1,
	const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSlice()-C", x.size(), r0, r1, c0, c1);
#endif
	setSlice(r0, r1, c0, c1, (const T*)x.data(), x.size());
}

template <class T>
void WMat<T>::setData(const T val)
{
	setSlice(0, m_endRow, 0, m_endCol, val);
}

template <class T>
void WMat<T>::setData(const T* buf, const uint32_t bufSize)
{
	if (bufSize > 0) {
		setSlice(0, m_endRow, 0, m_endCol, buf, bufSize);
	}
}

template <class T>
void WMat<T>::setData(const WMat<T>& x)
{
	if (x.size() > 0) {
		setSlice(0, m_endRow, 0, m_endCol, x.data(), x.size());
	}
}

template <class T>
void WMat<T>::setRand()
{
	std::vector<double> v;
	wGenUniformRandNumbers(v, m_size, 0, 1);    //v: random numbers in [0,1]

	//const WMatType type = getTypeId();
	const type_info& type = getTypeId();

	if (type == WMAT_ID_F32 || type == WMAT_ID_F64) {
		for (uint32_t k = 0; k < m_size; k++) {
			m_buf[k] = (T)v[k];
		}
	}
	else if (type==WMAT_ID_BOOL) {
		for (uint32_t k = 0; k < m_size; k++) {
			if (v[k] > 0.5) {
				m_buf[k] = true;
			}
			else {
				m_buf[k] = false;
			}
		}
	}
	else {
		T a = std::numeric_limits<T>::max();
		T b = 0;
		if (type == WMAT_ID_I64 || type == WMAT_ID_I32 || type == WMAT_ID_I16 || type == WMAT_ID_I8) {
			b = a / 2;
		}
		for (uint32_t k = 0; k < m_size; k++) {
			m_buf[k] = (T)(a * v[k]) - b;
		}
	}
}

template <class T>
void WMat<T>::setRandLow()
{
	T rnd;
	//const WMatType type = wmatTypeOf(rnd);
	const type_info& type = typeid(rnd);

	setData((T)0);

	uint32_t k = 0;
	std::vector<double> v;
	wGenUniformRandNumbers(v, m_size, 0, 1);    //v: random numbers in [0,1]
	if (type == WMAT_ID_F32 || type == WMAT_ID_F64) {
		for (uint32_t i = 0; i < m_rows; i++) {
			for (uint32_t j = 0; j <= i; j++) {
				m_buf[i*m_cols +j] = (T)v[k++];
			}
		}
	}
	else {
		//todo:
	}
}

template <class T>
void WMat<T> ::setCol(const uint32_t col, const T val)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setCol()-A", m_rows, 0, m_endRow, col, col);
#endif
	this->setSlice(0, m_endRow, col, col, val);
}

// set the col elements as vals stored in buf[] 
template <class T>
void WMat<T> ::setCol(const uint32_t col, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setCol()-B", bufSize, 0, m_endRow, col, col);
#endif

	this->setSlice(0, m_endRow, col, col, buf, bufSize);
}

template <class T>
void WMat<T> ::setCol(const uint32_t col, const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setCol()-C", x.size(), 0, m_endRow, col, col);
#endif
	this->setSlice(0, m_endRow, col, col, x);
}

template <class T>
void  WMat<T> ::setSubCol(const uint32_t r0, const uint32_t r1, const uint32_t col, const T val)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubCol()-A", r1 - r0 + 1, r0, r1, col, col);
#endif
	this->setSlice(r0, r1, col, col, val);
}

template <class T>
void WMat<T> ::setSubCol(const uint32_t r0, const uint32_t r1, const uint32_t col, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubCol()-B", bufSize, r0, r1, col, col);
#endif
	this->setSlice(r0, r1, col, col, buf, bufSize);
}

template <class T>
void  WMat<T> ::setSubCol(const uint32_t r0, const uint32_t r1, const uint32_t col, const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubCol()-C", x.size(), r0, r1, col, col);
#endif
	this->setSlice(r0, r1, col, col, x);
}


template <class T>
void WMat<T> ::setDiagonal(const T val)
{
	T* p = this->getBuf();
	for (uint32_t i = 0; i < m_rows; i++) {
		*p = val;
		p += (m_rows + 1);
	}
}

template <class T>
void WMat<T> ::setDiagonal(const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	if (m_rows > bufSize) {
		wErrMsg("WMat<T> :: setDiaganol() buffer size is too small (m_rows=%u, bufSize=%u)!", m_rows, bufSize);
	}
#endif

	T* pDes = this->getBuf();
	for (uint32_t i = 0; i < m_rows; i++) {
		*pDes = buf[i];
		pDes += (m_rows + 1);
	}
}

template <class T>
void WMat<T> ::setDiagonal(const WMat<T>& x)
{
	setDiagonal(x.data(), x.size());
}

template <class T>
void WMat<T> ::setRow(const uint32_t r, const T val)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setRow()-A", m_rows, r, r, 0, m_endCol);
#endif
	this->setSlice(r, r, 0, m_endCol, val);
}

template <class T>
void  WMat<T> ::setRow(const uint32_t r, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setRow()-B", bufSize, r, r, 0, m_endCol);
#endif
	this->setSlice(r, r, 0, m_endCol, buf, bufSize);
}

template <class T>
void WMat<T> ::setRow(const uint32_t r, const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setRow()-C", x.size(), r, r, 0, m_endCol);
#endif
	this->setSlice(r, r, 0, m_endCol, x.data(), x.size());
}

template <class T>
void  WMat<T> ::setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const T val)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubRow()-A", m_size, r, r, c0, c1);
#endif
	this->setSlice(r, r, c0, c1, val);
}

template <class T>
void  WMat<T> ::setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubRow()-B", m_size, r, r, c0, c1);
#endif
	this->setSlice(r, r, c0, c1, buf, bufSize);
}


template <class T>
void  WMat<T> ::setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubRow()-C", x.size(), r, r, c0, c1);
#endif
	this->setSlice(r, r, c0, c1, x.data(), x.size());
}

template <class T>
void  WMat<T> ::setRows(const uint32_t r0, const uint32_t r1, const T val)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubRow()-B", m_size, r0, r1, 0, m_endCol);
#endif
	this->setSlice(r0, r1, 0, m_endCol, val);
}

template <class T>
void  WMat<T> ::setRows(const uint32_t r0, const uint32_t r1, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubRow()-B", m_size, r0, r1, 0, m_endCol);
#endif
	this->setSlice(r0, r1, 0, m_endCol, buf, bufSize);
}

template <class T>
void  WMat<T> ::setRows(const uint32_t r0, const uint32_t r1, const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::setSubRow()-B", x.size(), r0, r1, 0, m_endCol);
#endif
	this->setSlice(r0, r1, 0, m_endCol, x.data(), x.size());
}

template <class T>
WMat<T> WMat<T> ::getSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getSlice() ", m_size, r0, r1, c0, c1);
#endif

	WMat<T> y(r1 - r0 + 1, c1 - c0 + 1);
	T* pDes = y.getBuf();
	for (uint32_t i = r0; i <= r1; i++) {
		const T* pSrc = this->getAddress(i, c0);
		for (uint32_t j = c0; j <= c1; j++) {
			*pDes++ = *pSrc++;
		}
	}
	return y;
}

//getSlice for the matrix is a column vector
template <class T>
WMat<T> WMat<T> ::getCol(const uint32_t col) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getCol() ", m_size, 0, m_endRow, col, col);
#endif
	return this->getSlice(0, m_endRow, col, col);
}

template <class T>
WMat<T> WMat<T> ::getSubCol(const uint32_t r0, const uint32_t r1, const uint32_t col) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getSubCol() ", m_size, r0, r1, col, col);
#endif
	return this->getSlice(r0, r1, col, col);
}

template <class T>
WMat<T> WMat<T> ::getCols(const uint32_t c0, const uint32_t c1) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getCols() ", m_size, 0, m_endRow, c0, c1);
#endif
	return this->getSlice(0, m_endRow, c0, c1);
}

//x(r, c0:c1)
template <class T>
WMat<T> WMat<T> ::getSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getSubRow() ", m_size, r, r, c0, c1);
#endif
	return this->getSlice(r, r, c0, c1);
}

template <class T>
WMat<T> WMat<T> ::getRow(const uint32_t r) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getSubRow() ", m_size, r, r, 0, m_endCol);
#endif
	return this->getSlice(r, r, 0, m_endCol);
}

template <class T>
WMat<T> WMat<T> ::getRows(const uint32_t r0, const uint32_t r1) const
{
#if WMAT_BOUNDS_CHECK
	checkSliceBoundary("WMat<T> ::getRows() ", m_size, r0, r1, 0, m_endCol);
#endif
	return this->getSlice(r0, r1, 0, m_endCol);
}

// return a  min(m_rows, m_cols) x 1 matrix which contains the diagonal elements of *this
template <class T>
WMat<T> WMat<T> ::getDiagonal() const
{
	uint32_t m = (m_rows > m_cols) ? m_cols : m_rows;
	WMat<T> y(m, 1);
	T* pDes = y.getBuf();
	for (uint32_t j = 0;j < m; j++) {
		*pDes++ = m_buf[j * m_cols + j];
	}
	return y;
}

template <class T>
void WMat<T> ::writeToFile(const char* fileName, const int precision) const
{
	std::vector<std::string>vHeadLines;
	writeToFile(fileName, precision, vHeadLines);
}

template <class T>
void WMat<T> ::writeToFile(const char* fileName, const int precision, const std::vector<std::string>& vHeadLines) const
{
	FILE* fp;
	errno_t  err = fopen_s(&fp, fileName, "w+");
	if (err != 0) {
		wWarningMsg("WMat<T> :: writeToFile(): file  cannot be opend!");
		return;
	}

	std::string s0 = toString("", precision);
	//write the title lines
	for (const std::string& h : vHeadLines) {
		fprintf(fp, "%s\n", h.c_str());
	}
	fprintf(fp, "%s", s0.c_str());
	fclose(fp);
}

template <class T>
void WMat<T> ::appendToFile(FILE* fp, const int precision, const char* msgStr, const bool printDimsionInfo) const
{
	if (msgStr)
		fprintf(fp, "%s\n", msgStr);

	if (printDimsionInfo)
		fprintf(fp, "%u,%u\n", m_rows, m_cols);

	std::string s0 = toString("", precision);
	fprintf(fp, "%s", s0.c_str());
}

template <class T>
void WMat<T> ::readFromFile(const char* fileName, const uint32_t nHeadLines, const uint32_t nCols, const char delimiter)
{
	FILE* fp;
	errno_t  err = fopen_s(&fp, fileName, "r");
	if (err != 0) {
		app::wWarningMsg("WMat<T> :: readFromFile(): cannot open file:%s", fileName);
		return;
	}

	//read the headlines, and discharge them
	char line[1024];
	for (uint32_t i = 0; i < nHeadLines; i++) {
		fgets(line, 1024, fp);
		//cout << line <<endl;
	}

	//read the data lines
	std::vector<std::string>vLines;
	while (fgets(line, 1024, fp) != NULL) {
		vLines.push_back(std::string(line));
	}
	fclose(fp);

	//figure out matrix size
	uint32_t nRows = vLines.size();
	resize(nRows, nCols);

	WMat<T> currRow(1, nCols);
	for (uint32_t i = 0; i < nRows; ++i) {
		parseLine(currRow, vLines[i], delimiter);
		this->setRow(i, currRow);
	}
}

template <class T>
bool WMat<T>::parseLine(WMat<T>& row, std::string& line, const char delimiter)
{
	return true;
}
