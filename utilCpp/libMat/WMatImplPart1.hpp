/*
/*
 *------------------------------------------------------------------------
 * WMatImplPart1.hpp - partial emulation of matrix computation in MatLab
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
template <class T>
WMat<T>::WMat(const uint32_t nRows, const uint32_t nCols)
	: m_buf(NULL)
	, m_rows(nRows)
	, m_cols(nCols)
	, m_size(nRows* nCols)
{
	if (m_size > 0) {
		creatBuf();
	}
}

template <class T>
WMat<T>::~WMat()
{
	deleteBuf();
}

template <class T>
WMat<T>::WMat(const uint32_t nRows, const uint32_t nCols, const T initValue)
	: m_buf(NULL)
	, m_rows(nRows)
	, m_cols(nCols)
	, m_size(nRows* nCols)
{
	creatBuf();
	setData(initValue);
}

template <class T>
WMat<T>::WMat(const uint32_t nRows, const uint32_t nCols, const std::string& type)
	: m_buf(NULL)
	, m_rows(nRows)
	, m_cols(nCols)
	, m_size(nRows* nCols)
{
	creatBuf();

	if (0 == strcmp(type.c_str(), "eye")) {
		setData((T)0);
		setDiagonal((T)1);
	}
	else if (0 == strcmp(type.c_str(), "rand")) {
		setRand();
	}
	else {
		setData((T)0);
	}
}


//constructor from a buffer, matrix just a wrapper, the client in response of managing <buf>
template <class T>
WMat<T>::WMat(const uint32_t nRows, const uint32_t nCols, T* buf, const uint32_t bufLength)
	: m_buf(buf)
	, m_rows(nRows)
	, m_cols(nCols)
	, m_size(nRows* nCols)
	, m_isWrapper(true)
	, m_endRow(nRows - 1)
	, m_endCol(nCols - 1)
{
	// NOTE: since we do not know the length of buf, there is a pentential risk to use this function
	if (bufLength != m_size) {
		wErrMsg("Warning! WMat<T>::WMat(): the input size not match the buffer size.");
	}
}

// constructor from a matrix
template <class T>
WMat<T>::WMat(const WMat<T>& x) :
	m_buf(NULL),
	m_rows(x.rows()),
	m_cols(x.cols()),
	m_size(x.size())
{
	if (!x.isEmpty()) {
		creatBuf();
		setData((const T*)x.data(), x.size());
	}
}

//asign the data of matrix to a double point
template <class T>
void WMat<T> ::getData(T** pp)
{
	for (uint32_t i = 0; i < m_rows; i++)
		*(pp + i) = &m_buf[i * m_cols];
}


//keep the values, just change the shape
template <class T>
void WMat<T>::reshape(const uint32_t newRows, const uint32_t newCols)
{
#if WMAT_BOUNDS_CHECK
	if (m_size != newRows * newCols) {
		wErrMsg("Warning: WMat<T>::reshape() -- input parameters are not match the orioginal ones!");
	}
#endif

	// leave the original value there, now they are stored still in the order of original (raw by raw)
	m_rows = newRows;
	m_cols = newCols;
}

template <class T>
T WMat<T> ::trace() const
{
	if (m_rows != m_cols) {
		wErrMsg("WMat<T> :: trace(): matrix is not square, m_rows=%d, m_cols=%d", m_rows, m_cols);
		return std::numeric_limits<T>::max();
	}

	T tr = 0;
	const T* p = m_buf;
	for (uint32_t i = 0; i < m_rows; i++, p += (m_cols + 1)) {
		tr += *p;
	}
	return tr;
}

template <class T>
void WMat<T>::creatBuf()
{
	if (m_size > 0) {
		if (m_buf) {
			deleteBuf();
		}
		m_buf = new T[m_size];
		if (!m_buf) {
			wErrMsg("WMat<T>::creatBuf(): cannot allocate memory m_size=%u!", m_size);
		}

		m_isWrapper = false;
		m_endRow = m_rows - 1;
		m_endCol = m_cols - 1;
	}
	else {
		m_size = 0;
		m_rows = 0;
		m_cols = 0;
		m_buf = NULL;
		m_isWrapper = false;
		m_endRow = 0;
		m_endCol = 0;
	}
}

template <class T>
void WMat<T>::deleteBuf()
{
	if (!m_isWrapper) {
		if (m_buf) {
			delete[] m_buf;
			m_buf = NULL;
		}
	}
}

template <class T>
WMat<T> WMat<T> ::transpose() const
{
	WMat<T> y(m_cols, m_rows);
	for (uint32_t i = 0; i < m_rows; i++)
		for (uint32_t j = 0; j < m_cols; j++)
			y.m_buf[j * m_rows + i] = m_buf[i * m_cols + j];

	return y;
}

template <class T>
std::string WMat<T>::toString(const char* str, const int precision) const
{
	//const WMatType type = getTypeId();
	const type_info& type = getTypeId();
	const T* p = data();
	std::ostringstream oss;
	oss << str << "(" << m_rows << "," << m_cols << ")=[" << std::endl;
  if (type == WMAT_ID_I8 || type == WMAT_ID_U8){
		for (uint32_t i = 0; i < m_rows; i++) {
			for (uint32_t j = 0; j < m_endCol; j++) {
				oss << static_cast<int>(*p++) << ",";
			}
			oss << static_cast<int>(*p++);
			if (i < m_endRow) {
				oss << std::endl;
			}
		}
	}
	else {
		if (type == WMAT_ID_F64) {
			oss << std::setprecision(precision == 0 ? 14 : precision) << std::fixed;
		}
		else if (type == WMAT_ID_F32) {
			oss << std::setprecision(precision == 0 ? 6 : precision) << std::fixed;
		}
		for (uint32_t i = 0; i < m_rows; i++) {
			for (uint32_t j = 0; j < m_endCol; j++) {
				oss << *p++ << ",";
			}
			oss << *p++;
			if (i < m_endRow) {
				oss << std::endl;
			}
		}
	}
	oss << "];" << std::endl;

	return oss.str();
}

template <class T>
void WMat<T>::print(const char* str) const
{
	std::cout << toString(str, 0) << std::endl;
}

template <class T>
void WMat<T>::print(const char* str, const int precision) const
{
	std::cout << toString(str, precision) << std::endl;
}


//buf[i] += val
template <class T>
void WMat<T>::eleAddition(T* buf, const uint32_t n, const T val) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf[i] += val;
	}
}

//buf[i] += val
template <class T>
void WMat<T>::eleMinus(T* buf, const uint32_t n, const T val) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf[i] -= val;
	}
}


//buf[i] *= val
template <class T>
void WMat<T>::eleTimes(T* buf, const uint32_t n, const T val) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf[i] *= val;
	}
}

//buf2[i] += buf1[i]
template <class T>
void WMat<T>::eleAddition(T* buf2, const T* buf1, const uint32_t n) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf2[i] += buf1[i];
	}
}

//buf2[i] -= buf1[i]
template <class T>
void WMat<T>::eleMinus(T* buf2, const T* buf1, const uint32_t n) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf2[i] -= buf1[i];
	}
}

//buf2[i] *= buf1[i]
template <class T>
void WMat<T>::eleTimes(T* buf2, const T* buf1, const uint32_t n) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf2[i] *= buf1[i];
	}
}

//buf2[i] /= buf1[i]
template <class T>
void WMat<T>::eleDivide(T* buf2, const T* buf1, const uint32_t n) const
{
	for (uint32_t i = 0; i < n; i++) {
		buf2[i] /= buf1[i];
	}
}

template <class T>
bool WMat<T>::eleEqual(const T* buf2, const T* buf1, const uint32_t n) const
{
	for (uint32_t i = 0; i < m_size; i++) {
		if (buf2[i] != buf1[i]) {
			return false;
		}
	}
	return true;
}

//m_buf[i] += buf[i]
template <class T>
void WMat<T>::eleAdd(const T* buf, const uint32_t n)
{
	T* p = m_buf;
	for (uint32_t i = 0; i < m_size; i++, ++p, ++buf) {
		*p += *buf;
	}
}
//m_buf[i] -= buf[i]
template <class T>
void WMat<T>::eleMin(const T* buf, const uint32_t n)
{
	T* p = m_buf;
	for (uint32_t i = 0; i < m_size; i++, ++p, ++buf) {
		*p -= *buf;
	}
}

//m_buf[i] *= buf[i]
template <class T>
void WMat<T>::eleTim(const T* buf, const uint32_t n)
{
	T* p = m_buf;
	for (uint32_t i = 0; i < m_size; i++, ++p, ++buf) {
		*p *= *buf;
	}
}

//m_buf[i] /= buf[i]
template <class T>
void WMat<T>::eleDiv(const T* buf, const uint32_t n)
{
	T* p = m_buf;
	for (uint32_t i = 0; i < m_size; i++, ++p, ++buf) {
		*p /= *buf;
	}
}

//m_buf[i] += val
template <class T>
void WMat<T>::eleAdd(const T val)
{
	T* p = m_buf;
	for (uint32_t i = 0; i < m_size; i++, ++p) {
		*p += val;
	}
}
//m_buf[i] *= val
template <class T>
void WMat<T>::eleTim(const T val)
{
	T* p = m_buf;
	for (uint32_t i = 0; i < m_size; i++, ++p) {
		*p *= val;
	}
}
