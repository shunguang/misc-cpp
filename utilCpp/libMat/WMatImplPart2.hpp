/*
/*
 *------------------------------------------------------------------------
 * WMatImplPart2.cpp - partial emulation of matrix computation in MatLab
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
 //-----------------------------------------
 // public operator overloads
 //-----------------------------------------
 // 
 //overload assignment operator
 //const return avoid: ( a1=a2 ) = a3
template <class T>
WMat<T>& WMat<T>::operator = (const WMat<T>& x) //assignment
{
	if (m_isWrapper) {
		wErrMsg("WMat<T>::operator =: left side cannot be a wrapper matrix!");
	}

	if (&x != this) { //check for self assignment
		m_rows = x.rows();
		m_cols = x.cols();
		m_endRow = x.m_endRow;
		m_endCol = x.m_endCol;
		if (m_size != x.size()) {
			deleteBuf();
			m_size = x.size();
			creatBuf();
		}
		if (!x.isEmpty()) {
			setData((const T*)x.data(), x.size());
		}
	}
	return *this; // enables x=y=z;
}

//assignment: x=val
template <class T>
WMat<T>& WMat<T>::operator = (const T val)
{
	setData(val);
	return *this;
}

template <class T>
bool WMat<T>::operator==(const WMat& x) const
{
	if (x.size() != m_size) {
		return false;
	}
	return eleEqual((const T*)this->data(), (const T*)x.data(), m_size);
}

template <class T>
bool WMat<T>::operator!=(const WMat& x) const
{
	return (!(*this == x));
}

// z = -this
template <class T>
WMat<T> WMat<T>::operator-() const
{
	WMat<T> z(*this);

	const T s0 = -1;
	eleTimes(z.getBuf(), m_size, s0);
	return z;
}

// z = this + x add by element
template <class T>
WMat<T> WMat<T>::operator +(const WMat& x) const
{
#if WMAT_BOUNDS_CHECK
	if (this->m_rows != x.rows() || this->m_cols != x.cols())
		wErrMsg("WMat<T>::operator +(const WMat &x): size is not match!");
#endif

	WMat<T> z(*this);
	eleAddition(z.getBuf(), (const T*)x.data(), x.size());
	return z;
}

//z = this + scale, add by element
template <class T>
WMat<T> WMat<T>::operator +(const T val) const
{
	WMat<T> z(*this);
	eleAddition(z.getBuf(), this->size(), val);
	return z;
}

//z=this-x,  minus by element
template <class T>
WMat<T> WMat<T>::operator -(const WMat<T>& x) const
{
#if WMAT_BOUNDS_CHECK
	if (this->m_rows != x.rows() || this->m_cols != x.cols())
		wErrMsg("WMat<T>::operator -(const WMat &x): size is not match!");
#endif

	WMat<T> z(*this);
	eleMinus(z.getBuf(), (const T*)x.data(), x.size());
	return z;
}

//z = this - scale,   minus by scale
template <class T>
WMat<T> WMat<T>::operator -(const T val) const
{
	WMat<T> z(*this);
	eleMinus(z.getBuf(), this->size(), val);
	return z;
}

//a*x  time by element
template <class T>
WMat<T> WMat<T>::operator *(const WMat<T>& x) const
{
#if WMAT_BOUNDS_CHECK
	if (this->m_rows != x.rows() || this->m_cols != x.cols())
		wErrMsg("WMat<T>::operator *(const WMat &x): size is not match!");
#endif

	WMat<T> z(*this);
	eleTimes(z.getBuf(), (const T*)x.data(), x.size());
	return z;
}

//a*scale  add by element
template <class T>
WMat<T> WMat<T>::operator *(const T val) const
{
	WMat<T> z(*this);
	eleTimes(z.getBuf(), this->size(), val);
	return z;
}

//z = this ./ x   by element
template <class T>
WMat<T> WMat<T>::operator /(const WMat<T>& x) const
{
#if WMAT_BOUNDS_CHECK
	if (this->m_rows != x.rows() || this->m_cols != x.cols())
		wErrMsg("WMat<T>::operator /(const WMat &x): size is not match!");
#endif

	WMat z(*this);
	eleDivide(z.getBuf(), (const T*)x.data(), x.size());
	return z;
}

//a/scale  divide by a scale
template <class T>
WMat<T> WMat<T>::operator /(const T val) const
{
	WMat<T> z(*this);
	eleTimes(z.getBuf(), this->size(), (T)(1.0 / val));
	return z;
}

// matrix production  z = this % X
template <class T>
WMat<T> WMat<T>::operator %(const WMat<T>& x) const
{
	uint32_t m = this->rows();
	uint32_t n = this->cols();
	uint32_t k = x.cols();
#if WMAT_BOUNDS_CHECK
	if (n != x.rows()) {
		wErrMsg("WMat<T>::operator %(const WMat &x): size is not match!");
	}
#endif

	WMat<T> z(m, k, 0.0);
	T* des = z.getBuf();
	for (uint32_t i = 0; i < m; i++) {
		const T* p0 = this->getRowAddress(i);
		for (uint32_t j = 0; j < k; j++) {
			const T* p1 = p0;
			for (uint32_t l = 0; l < n; l++) {
				*des += (*p1++) * x.m_buf[l * k + j];
			}
			des++;
		}
	}
	return z;
}

template <class T>
T WMat<T> :: operator()(const uint32_t i, const uint32_t j) const
{
#if WMAT_BOUNDS_CHECK
	if (i < 0 || i >= this->m_rows || j < 0 || j >= this->m_cols) {
		wErrMsg("WMat<T>::operator(): subscript out of bounds!");
	}
#endif
	return this->m_buf[i * m_cols + j];
}


template <class T>
T& WMat<T> :: operator()(const uint32_t i, const uint32_t j)
{
#if WMAT_BOUNDS_CHECK
	if (i < 0 || i >= this->m_rows || j < 0 || j >= this->m_cols) {
		wErrMsg("WMat<T>::operator(): subscript out of bounds!");
	}
#endif

	return this->m_buf[i * m_cols + j];
}

template <class T>
T WMat<T> :: operator()(const uint32_t k) const
{
#if WMAT_BOUNDS_CHECK
	if (k < 0 || k >= this->m_size) {
		wErrMsg("WMat<T>::operator(): subscript out of bounds!");
	}
#endif

	return this->m_buf[k];
}

template <class T>
T& WMat<T> :: operator()(const uint32_t k)
{
#if WMAT_BOUNDS_CHECK
	if (k < 0 || k >= (this->m_size)) {
		wErrMsg("WMat<T>::operator(): subscript out of bounds!");
	}
#endif

	return this->m_buf[k];
}

//this +=x;
template <class T>
WMat<T>& WMat<T> ::  operator +=(const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (this->m_rows != x.m_rows || this->m_cols != x.m_cols)
		wErrMsg("Error! WMat<T> ::  operator +=() size is not the same.");
#endif
	eleAdd((const T*)x.data(), x.size());
	return *this;
}

//this +=val;
template <class T>
WMat<T>& WMat<T> ::  operator +=(const T val)
{
	eleAdd(val);
	return *this;
}

//this -=x;
template <class T>
WMat<T>& WMat<T> ::  operator -=(const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (m_rows != x.m_rows || m_cols != x.m_cols)
		wErrMsg("Error! WMat<T> ::  operator -=() size is not the same.");
#endif

	eleMin((const T*)x.data(), x.size());
	return *this;
}

//this -=val
template <class T>
WMat<T>& WMat<T> ::  operator -=(const T val)
{
	eleAdd(-val);
	return *this;
}

//this *=x
template <class T>
WMat<T>& WMat<T> ::  operator *=(const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (m_rows != x.m_rows || m_cols != x.m_cols)
		wErrMsg("Error! WMat<T> ::  operator -=() size is not the same.");
#endif

	eleTim((const T*)x.data(), x.size());
	return *this;
}

//this *=val
template <class T>
WMat<T>& WMat<T> ::  operator *=(const T val)
{
	eleTim(val);
	return *this;
}

//this /= x
template <class T>
WMat<T>& WMat<T> ::  operator /=(const WMat<T>& x)
{
#if WMAT_BOUNDS_CHECK
	if (m_rows != x.m_rows || m_cols != x.m_cols)
		wErrMsg("Error! WMat<T> ::  operator -=() size is not the same.");
#endif

	eleDiv((const T*)x.data(), x.size());
	return *this;
}

//this /= val
template <class T>
WMat<T>& WMat<T> ::  operator /=(const T val)
{
	T s0 = 1 / val;
	eleTim(s0);
	return *this;
}
