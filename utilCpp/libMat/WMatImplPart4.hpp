/*
/*
 *------------------------------------------------------------------------
 * WMatImplPart4.cpp - partial emulation of matrix computation in MatLab
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

 //
 //This part is for size change
 //

 //change the size, donot keep the previous value

 //--------------------------------------------
 // the following are class private functions
 //--------------------------------------------
 // param $size$ is not neccessary, here just for safty consideration
template <class T>
void WMat<T> ::deepCopy(T* buf, const uint32_t size) const
{
#if WMAT_BOUNDS_CHECK
	if (buf == NULL)
		wErrMsg("Error: WMat<T>::deepCopy() -- buf is NULL!");

	if (size != m_size)
		wErrMsg("Error: WMat<T>::deepCopy() -- size not match the m_size!");
#endif

	for (unsigned k = 0; k < m_size; k++)
		buf[k] = m_buf[k];
}

template <class T>
void WMat<T>::resize(const uint32_t newRows, const uint32_t newCols)
{
	if (m_size != newRows * newCols) { //reallocate memory, uninitinalized 
		deleteBuf();
		m_size = newRows * newCols;
		m_rows = newRows;
		m_cols = newCols;
		creatBuf();
	}
	else { //leave the original value there, 
		m_size = newRows * newCols;
		m_rows = newRows;
		m_cols = newCols;
		m_endRow = newRows-1;
		m_endCol = newCols-1;
	}
}

template <class T>
void WMat<T>::resize(const uint32_t newRows, const uint32_t newCols, const T newVal)
{
	resize(newRows, newCols);
	setData(newVal);
}

//delete the c-th column
template <class T>
void WMat<T>::delCol(const uint32_t c)
{
	delCols(c, c);
}

//delete the c-th column
template <class T>
void WMat<T>::delCols(const uint32_t c1, const uint32_t c2, const uint32_t step)
{
#if WMAT_BOUNDS_CHECK
	if (c1 > c2 || c1 < 0 || c2 >= m_cols) {
		wErrMsg("WMat :: delCols(), col indices are out of range!(c1=%u,c2=%u, m_cols=%u)", c1, c2, m_cols);
	}
#endif
	if (c1 == 0 && c2 == (m_cols - 1) && step == 1) {
		resize(0, 0);
		return;
	}

	WMat<T> x0(*this); //copy the current matrix

  std::vector<uint32_t> vColToBeRemoved;
  std::vector<uint32_t> vColToBeCopied;
	wCreateIndices(vColToBeRemoved, c1, c2, step);
	wSelectIndices(vColToBeCopied, vColToBeRemoved, 0, m_cols);

	uint32_t newCols = (uint32_t)vColToBeCopied.size();
	resize(m_rows, newCols);
	copyColsFrom( 0, x0, vColToBeCopied);
}

//delete the r-th row
template <class T>
void  WMat<T>::delRow(const uint32_t r)
{
	delRows(r, r);
}

template <class T>
void WMat<T>::delRows(const uint32_t r1, const uint32_t r2, const uint32_t step)
{
#if WMAT_BOUNDS_CHECK
	if (r1 > r2 || r1 < 0 || r2 >= m_rows) {
		wErrMsg("WMat :: delRows(), row indices are out of range!(r1=%u,r2=%u, m_rows=%u)", r1, r2, m_rows);
	}
#endif
	if (r1 == 0 && r2 == (m_rows - 1) && step == 1){
		resize(0, 0);
		return;
	}

	WMat<T> x0(*this); //copy the current matrix

	std::vector<uint32_t> vRowToBeRemoved;
	std::vector<uint32_t> vRowToBeCopied;
	wCreateIndices(vRowToBeRemoved, r1, r2, step);
	wSelectIndices(vRowToBeCopied, vRowToBeRemoved, 0, m_rows);

	uint32_t nNewRows = (uint32_t)vRowToBeCopied.size();
	resize( nNewRows,m_cols);
	copyRowsFrom(0, x0, vRowToBeCopied);
}

// insert a column with same val
template <class T>
void  WMat<T>::insertCols(const uint32_t startCol, const uint32_t endCol, const T val)
{
	WMat<T> x(m_rows, 1, val);
	insertCols(startCol, endCol, x);
}

template <class T>
void  WMat<T>::insertCols(const uint32_t startCol, const uint32_t endCol, const WMat<T>& x)
{
	insertCols(startCol, endCol, x.getBuf(), x.size());
}

// insert a column with data from matrix x
template <class T>
void  WMat<T>::insertCols(const uint32_t startCol, const uint32_t endCol, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	if (startCol < 0 || endCol < startCol || (endCol-startCol+1)*m_rows>bufSize) {
		wErrMsg("WMat :: insertCols(): input data is incorrect!--startCol=%u, endCol=%u, m_rows=%u, bufSize=%u", startCol, endCol, m_rows, bufSize);
	}
#endif

	WMat<T> x0(*this); //copy the current matrix

	uint32_t nAddCols = endCol - startCol + 1;
	uint32_t nNewCols = m_cols + nAddCols;

	resize(m_rows, nNewCols);

	if (startCol == 0)  //insert at left
	{
		setSlice(0, m_rows, startCol, endCol, buf, bufSize);
		setSlice(0, m_rows, endCol+1, m_endCol, x0);
	}
	else if (startCol < x0.cols()) //insert in beween 
	{
		uint32_t x0_c1 = startCol - 1;
		uint32_t x0_c2 = x0.cols()-1;
		WMat<T> x1 = x0.getSlice(0, m_rows, 0, x0_c1);
		WMat<T> x2 = x0.getSlice(0, m_rows, x0_c1+1, x0_c2);
		setSlice(0, m_rows, 0, x0_c1, x1);
		setSlice(0, m_rows, startCol, endCol, buf, bufSize);
		setSlice(0, m_rows, endCol+1, m_endCol, x2);
	}
	else //insert at right end
	{
		setSlice(0, m_rows, 0, startCol-1, x0);
		setSlice(0, m_rows, startCol, m_endCol, buf, bufSize);
	}
}

template <class T>
void WMat<T>::appendCols(const uint32_t nAddtionalCols, const T val)
{
	insertCols(m_cols, m_cols + nAddtionalCols - 1, val);
}

template <class T>
void WMat<T>::appendCols(const uint32_t nAddtionalCols, const T* buf, const uint32_t bufSize)
{
	insertCols(m_cols, m_cols + nAddtionalCols - 1, buf, bufSize);
}

template <class T>
void WMat<T>::appendCols(const uint32_t nAddtionalCols, const WMat<T>& x)
{
	insertCols(m_cols, m_cols + nAddtionalCols - 1, x);
}


// insert a row with same data value
template <class T>
void  WMat<T>::insertRows(const uint32_t startRow, const uint32_t endRow, const T val)
{
	WMat x(1, m_cols, val);
	insertRows(startRow, endRow, x);
}

// insert a row with data from matrix x
template <class T>
void  WMat<T>::insertRows(const uint32_t startRow, const uint32_t endRow, const WMat<T>& x)
{
	insertRow(startRow, endRow, x.data(), x.size());
}

template <class T>
void WMat<T>::insertRows(const uint32_t startRow, const uint32_t endRow, const T* buf, const uint32_t bufSize)
{
#if WMAT_BOUNDS_CHECK
	if (startRow < 0 || endRow < startRow || (endRow - startRow + 1) * m_cols>bufSize) {
		wErrMsg("WMat :: insertRows(): input data is incorrect!--startRow=%u, endRow=%u, m_cols=%u, bufSize=%u", startRow, endRow, m_cols, bufSize);
	}
#endif

	WMat<T> x0(*this); //copy the current matrix
	const uint32_t x0_rows = x0.rows();
	uint32_t nAddRows = endRow - startRow + 1;
	uint32_t nNewRows = x0_rows + nAddRows;         

	resize(nNewRows, m_cols);

	if (startRow == 0)  //insert at top
	{
		setSlice(0, nAddRows-1,    0, m_endCol, buf, bufSize);
		setSlice(nAddRows, m_endRow, 0, m_endCol, x0);
	}
	else if (startRow < x0_rows) //insert in beween 
	{
		uint32_t x0_r1 = startRow - 1;
		uint32_t x0_r2 = x0_rows - 1;
		WMat<T> x1 = x0.getSlice(0, x0_r1, 0, m_endCol);
		WMat<T> x2 = x0.getSlice(x0_r1+1, x0_r2, 0, m_endCol);
		setSlice(0, x0_r1, 0, m_endCol, x1);
		setSlice(startRow, endRow, 0, m_endCol, buf, bufSize);
		setSlice(endRow + 1, m_endRow, 0,  m_endCol,  x2);
	}
	else //insert at bottom
	{
		setSlice(0, x0_rows-1, 0, m_endCol, x0);
		setSlice(x0_rows, m_endRow, 0, m_endCol, buf, bufSize);
	}
}


//keep the original values at the original place, but add addtional # of rows at bottom
template <class T>
void WMat<T>::appendRows(const uint32_t nAddtionalRows, const T val)
{
	insertRows(m_rows, m_rows + nAddtionalRows-1, val);
}

template <class T>
void WMat<T>::appendRows(const uint32_t nAddtionalRows, const T* buf, const uint32_t bufSize)
{
	insertRows(m_rows, m_rows + nAddtionalRows-1, buf, bufSize);
}

template <class T>
void WMat<T>::appendRows(const uint32_t nAddtionalRows, const WMat<T>& x)
{
	insertRows(m_rows, m_rows + nAddtionalRows-1, x);
}

//copy some of the columns from <src> to this.
template <class T>
void WMat<T>::copyColsFrom(const uint32_t startColInThis,  const WMat<T>& src, const std::vector<uint32_t> vSrcColId)
{
	assert(m_rows == src.rows());
	assert(m_cols > startColInThis );
	assert(m_cols - startColInThis >= vSrcColId.size());
	int desCol = startColInThis;
	for (const uint32_t srcCol : vSrcColId) {
		for (uint32_t i = 0; i < m_rows; ++i) {
			m_buf[i*m_cols +desCol] = src(i, srcCol);
		}
		desCol++;
	}
}

template <class T>
void WMat<T>::copyRowsFrom(const uint32_t startRowInThis, const WMat<T>& src, const std::vector<uint32_t> vSrcRowId)
{
	assert(m_cols == src.cols());
	assert(m_rows > startRowInThis);
	assert(m_rows - startRowInThis >= vSrcRowId.size());
	uint32_t desRow = startRowInThis;
	for ( const uint32_t srcRow : vSrcRowId) {
		T* pDes = getRowAddress(desRow);
		const T* pSrc = src.getRowAddress(srcRow);
		for (uint32_t j = 0; j < m_cols; j++) {
			*pDes++ = *pSrc++;
		}
		desRow++;
	}
}

