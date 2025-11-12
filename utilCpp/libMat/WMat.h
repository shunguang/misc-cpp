/*
 *------------------------------------------------------------------------
 * WMat.h - partial emulation of matrix computation in MatLab 
 * No  any Corporation or client funds were used to develop this code. 
 *
 * $Id: WMat.h,v 1.3 2011/06/23 18:41:20 swu Exp $
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


#ifndef __WMAT_H__
#define __WMAT_H__	1

#include "WMatDefs.h"
#include "WUtil.h"

namespace app {
#if 1
#define	WMAT_ID_F32 typeid(float(1))
#define	WMAT_ID_F64 typeid(double(1))
#define	WMAT_ID_I8  typeid(int8_t(1))
#define	WMAT_ID_I16 typeid(int16_t(1))
#define	WMAT_ID_I32 typeid(int32_t(1))
#define	WMAT_ID_I64 typeid(int64_t(1))
#define	WMAT_ID_U8  typeid(uint8_t(1))
#define	WMAT_ID_U16 typeid(uint16_t(1))
#define	WMAT_ID_U32 typeid(uint32_t(1))
#define	WMAT_ID_U64 typeid(uint64_t(1))
#define	WMAT_ID_BOOL typeid(bool(1))
#else
	enum WMatType
	{
		WMAT_ID_F32 = 0,
		WMAT_ID_F64,
		WMAT_ID_I8,
		WMAT_ID_I16,
		WMAT_ID_I32,
		WMAT_ID_I64,
		WMAT_ID_U8,
		WMAT_ID_U16,
		WMAT_ID_U32,
		WMAT_ID_U64,
		WMAT_ID_UNKN
	};

	template <typename T>
	const WMatType wmatTypeOf(T&) { return WMAT_ID_UNKN; }

	template<> const WMatType wmatTypeOf(float&) { return WMAT_ID_F32; }
	template<> const WMatType wmatTypeOf(double&) { return WMAT_ID_F64; }
	template<> const WMatType wmatTypeOf(int8_t&) { return WMAT_ID_I8; }
	template<> const WMatType wmatTypeOf(int16_t&) { return WMAT_ID_I16; }
	template<> const WMatType wmatTypeOf(int32_t&) { return WMAT_ID_I32; }
	template<> const WMatType wmatTypeOf(int64_t&) { return WMAT_ID_I64; }
	template<> const WMatType wmatTypeOf(uint8_t&) { return WMAT_ID_U8; }
	template<> const WMatType wmatTypeOf(uint16_t&) { return WMAT_ID_U16; }
	template<> const WMatType wmatTypeOf(uint32_t&) { return WMAT_ID_U32; }
	template<> const WMatType wmatTypeOf(uint64_t&) { return WMAT_ID_U64; }
#endif

	template <class T>
	class WMat;

	template<class T>
	class WMat {
		//non-member friend functions
		friend std::ostream& operator<<(std::ostream& os, const WMat<T>& x) {
			return os << "abc todo ";
		}
		friend WMat<T>   operator+(const T val, const WMat<T>& x) { // 5+x
			return (x + val);
		}
		friend WMat<T>   operator-(const T val, const WMat<T>& x) { // 5-x
			return (-x + val);
		}
		friend WMat<T>   operator*(const T val, const WMat<T>& x) { // 5*x
			return (x * val);
		}
		friend WMat<T>   operator/(const T val, const WMat<T>& x) { // 5/x
			WMat<T> y(x.rows(), x.cols(), val);
			return  y / x;
		}
	public:
		//---------------------------------------------
		// part1 basic stuff
		// --------------------------------------------

		WMat(const uint32_t nRows = 0, const uint32_t nCols = 0);
		WMat(const uint32_t nRows, const uint32_t nCols, const T initValue);
		//create a "eye", "rand" matrix
		WMat(const uint32_t nRows, const uint32_t nCols, const std::string& type);

		//shallow copy -- just a wrapper
		//constructor from a buffer, matrix just a wrapper, the client in response of managing <buf>
		WMat(const uint32_t nRows, const uint32_t nCols, T* buf, const uint32_t bufLength);
		WMat(const WMat<T>& x);
		~WMat();

		uint32_t rows() const { return m_rows; }
		uint32_t cols() const { return m_cols; }
		uint32_t size() const { return m_size; }
		uint32_t numele() const { return m_size; }
		//[m,n]= size(x)
		void size(uint32_t& m, uint32_t& n) const { m = m_rows; n = m_cols; }

		//WMatType getTypeId() const {return wmatTypeOf( T(1) );}
		const type_info& getTypeId() const {	return  typeid( m_buf[0] ); }   

		//todo: return val should base on type
		T getEps() const { return (T)WMAT_F32_EPS; }

		bool isEmpty(const char *msg=nullptr) const{
			bool em = (m_size == 0);
			if (em && msg ) {
				wWarningMsg(msg);
			}
			return em;
		}

		bool isFloatType() const {
			const type_info& type = getTypeId();
			return ( WMAT_ID_F32 == type || WMAT_ID_F64 == type );
		}


		//get beging address
		T* getBuf() { return m_buf; }
		T* data() { return m_buf; }
		const T* data() const { return m_buf; }
		void getData(T** pp);

		T* getRowAddress(const uint32_t& r) { return m_buf + r * m_cols; }
		const T* getRowAddress(const uint32_t& r) const { return m_buf + r * m_cols; }
		T* getAddress(const uint32_t& r, const uint32_t& c) { return m_buf + r * m_cols + c; }
		const T* getAddress(const uint32_t& r, const uint32_t& c) const { return m_buf + r * m_cols + c; }

		T trace() const;             // get trace
		WMat<T>  transpose() const;  // get transpose

		void  print(const char* str = "") const;
		void  print(const char* str, const int precision) const;
		std::string toString(const char* str, const int precision = 0) const;

		//---------------------------------------------
		// part2 operators
		// --------------------------------------------
		//public operator overloads
		bool   operator==(const WMat& x) const; //compare equal
		bool   operator!=(const WMat& x) const; //compare not equal
		T      operator()(const uint32_t i, const uint32_t j) const;   // get a(i,j)
		T& operator()(const uint32_t i, const uint32_t j);         // get a(i,j)
		T      operator()(const uint32_t k) const;
		T& operator()(const uint32_t k);

		WMat<T>& operator = (const WMat<T>& x);  //assignment matrix x to this
		WMat<T>& operator = (const T val);       //assignment <val> to all elements of this

		//in the following explaination. we assume $y$ is the <this> matrix
		WMat<T> operator -() const;               // z = -y by add minus sign for each element
		WMat<T> operator +(const WMat& x) const;     // z=y+x  add by element
		WMat<T> operator +(const T scale) const;			// z=y+scale add scale to every element
		WMat<T> operator -(const WMat& x) const;     // z=y-x  minus by element
		WMat<T> operator -(const T scale) const;			// z=y-scale  minus ascale to every element
		WMat<T> operator *(const WMat& x) const;     // z=y .* x  times by element
		WMat<T> operator *(const T scale) const;     // z=y * scale  times scale to every element
		WMat<T> operator /(const WMat& x) const;     // z=y ./ x  dividion by element
		WMat<T> operator /(const T scale) const;			// z=y/scale  divided every element by the scale
		WMat<T> operator %(const WMat& x) const;     // z=y%x  matrix prodcution

		WMat<T>& operator +=(const WMat& x);     //y += x (a=a+x), by element
		WMat<T>& operator +=(const T scale);			//y += scale, matrix plus a scale
		WMat<T>& operator -=(const WMat& x);     //y -= x, by element
		WMat<T>& operator -=(const T scale);			//y -= scale, matrix minus a scale
		WMat<T>& operator *=(const WMat& x);     //y *= x, by element
		WMat<T>& operator *=(const T scale);			//y *= scale, amatrix times a scale
		WMat<T>& operator /=(const WMat& x);     //y /= x, by element
		WMat<T>& operator /=(const T scale);			//y /= scale, by element

		//---------------------------------------------
		// part2 set and get data
		// --------------------------------------------
		void setData(const T val);
		void setData(const T* buf, const uint32_t bufSize);
		void setData(const WMat<T>& x);
		void setRand();
		void setRandLow();  //A=L*U
		//void setRandUp();  //L.setRandLow(), U=L.tranpsoe();

		//x(r0:r1,c0:c1)=val
		void  setSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1, const T val);
		void  setSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1, const T* buf, const uint32_t bufSize);
		void  setSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1, const WMat<T>& x);

		//x(:,c) = val, buf, or x;
		void setCol(const uint32_t c, const T val);   // set the col elements as val
		void setCol(const uint32_t c, const T* buf, const uint32_t bufSize);  // set the col elements as vals stored in buf[] 
		void setCol(const uint32_t c, const WMat<T>& x);   // set the col elements as vals stored in vector x 

		//x(r0:r1,c) = val, buf, or x.data();
		void setSubCol(const uint32_t r0, const uint32_t r1, const uint32_t c, const T val);
		void setSubCol(const uint32_t r0, const uint32_t r1, const uint32_t c, const T* buf, const uint32_t bufSize);
		void setSubCol(const uint32_t r0, const uint32_t r1, const uint32_t c, const WMat<T>& x);

		//set diagnoal 
		void setDiagonal(const T val);  // set the diagnoal elements as val
		void setDiagonal(const T* buf, const uint32_t bufSize);
		void setDiagonal(const WMat& x);  // set the diagnoal elements as vals stored in vector x 

		//x(r, :)=val
		void setRow(const uint32_t r, const T val);
		void setRow(const uint32_t r, const T* buf, const uint32_t bufSize);
		void setRow(const uint32_t r, const WMat<T>& x);  // set the row elements as vals stored in vector x 

		//x(r, c0:c1)=val
		void setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const T val);
		void setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const T* buf, const uint32_t bufSize);
		void setSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1, const WMat<T>& x);

		//x(r0:r1, :)=val
		void setRows(const uint32_t r0, const uint32_t r1, const T val);
		void setRows(const uint32_t r0, const uint32_t r1, const T* buf, const uint32_t bufSize);
		void setRows(const uint32_t r0, const uint32_t r1, const WMat<T>& x);

		//the powerful 	getSlice()
		//get x([r0:r1], [c0:c1])
		WMat<T> getSlice(const uint32_t r0, const uint32_t r1, const uint32_t c0, const uint32_t c1) const;

		//x(:,col)	
		WMat<T> getCol(const uint32_t col) const;
		//x([r0:r1], col)	
		WMat<T> getSubCol(const uint32_t r0, const uint32_t r1, const uint32_t col) const;
		//x(:, [c1:c2])	
		WMat<T> getCols(const uint32_t c0, const uint32_t c1) const;

		//x(r,:) 
		WMat<T> getRow(const uint32_t r) const;
		//x(r,[c0:c1])
		WMat<T> getSubRow(const uint32_t r, const uint32_t c0, const uint32_t c1) const;
		//x([r0:r1], :)
		WMat<T> getRows(const uint32_t r0, const uint32_t r1) const;
		//get the diagnal
		WMat<T> getDiagonal() const;											// get the diagnal

		void readFromFile(const char* fileName, const uint32_t nHeadLines, const uint32_t nCols, const char delimiter = ',');

		void writeToFile(const char* fileName, const int precision) const;
		void writeToFile(const char* fileName, const int precision, const std::vector<std::string>& titleLines) const;
		void appendToFile(FILE* fp, const int precision, const char* msgStr = "", const bool printDimsionInfo = true) const;

		//---------------------------------------------
		// part4 change the matrix size
		// --------------------------------------------
		//change size of the matrix, 
		void resize(const uint32_t newRows, const uint32_t newCols);
		void resize(const uint32_t newRows, const uint32_t newCols, const T newVal);
		// keep the value, and size, but chnage raws and cols, and the data are stored row by row
		void reshape(const uint32_t newRows, const uint32_t newCols);

		// delete coloumn c
		void  delCol(const uint32_t c);
		// delete coloumns from c1 to c2 
		void  delCols(const uint32_t c1, const uint32_t c2, const uint32_t step = 1);
		// delete row r
		void  delRow(const uint32_t r);
		// delete rows from c1 to c2 
		void  delRows(const uint32_t r1, const uint32_t r2, const uint32_t step = 1);

		//insert a block into this: newThis = this(:, [0:startCol-1, [startCol:endCol], ...] )
		void  insertCols(const uint32_t startCol, const uint32_t endCol, const T val);
		void  insertCols(const uint32_t startCol, const uint32_t endCol, const T* buf, const uint32_t bufSize);
		void  insertCols(const uint32_t startCol, const uint32_t endCol, const WMat& x);
		//keep the original values at the original place, but add addtional # of cols at right
		//this = [this, x], where x is m_rows x nAddtionalCols
		void  appendCols(const uint32_t nAddtionalCols, const T val);
		void  appendCols(const uint32_t nAddtionalCols, const T* buf, const uint32_t bufSize);
		void  appendCols(const uint32_t nAddtionalCols, const WMat<T>& x);

		//this([startRow:endRow], : ) = val
		void  insertRows(const uint32_t startRow, const uint32_t endRow, const T val);
		void  insertRows(const uint32_t startRow, const uint32_t endRow, const T* buf, const uint32_t bufSize);
		void  insertRows(const uint32_t startRow, const uint32_t endRow, const WMat& x);
		//keep the original values at the original place, but add addtional # of rows at bottom
		//this = [this; x], where x is nAddtionalRows x m_cols
		void  appendRows(const uint32_t nAddtionalRows, const T val);
		void  appendRows(const uint32_t nAddtionalRows, const T* buf, const uint32_t bufSize);
		void  appendRows(const uint32_t nAddtionalRows, const WMat<T>& x);

	private:
		void creatBuf();
		void deleteBuf();
		void deepCopy(T* buf, const uint32_t size) const;

		void eleAddition(T* buf, const uint32_t n, const T val) const;			//buf[i] += val
		void eleMinus(T* buf, const uint32_t n, const T val) const;			    //buf[i] -= val
		void eleTimes(T* buf, const uint32_t n, const T val) const;					//buf[i] *= val
		void eleAddition(T* buf2, const T* buf1, const uint32_t n) const;		//buf2[i] += buf1[i]
		void eleMinus(T* buf2, const T* buf1, const uint32_t n) const;		  //buf2[i] -= buf1[i]
		void eleTimes(T* buf2, const T* buf1, const uint32_t n) const;			//buf2[i] *= buf1[i]
		void eleDivide(T* buf2, const T* buf1, const uint32_t n) const;			//buf2[i] /= buf1[i]
		bool eleEqual(const T* buf2, const T* buf1, const uint32_t n) const;//buf2[i] == buf1[i] for all i \in[0,n)

		//directly change m_buf[i] 
		void eleAdd(const T* buf, const uint32_t n);			//m_buf[i] += buf[i]
		void eleMin(const T* buf, const uint32_t n);		  //m_buf[i] -= buf[i]
		void eleTim(const T* buf, const uint32_t n);			//m_buf[i] *= buf[i]
		void eleDiv(const T* buf, const uint32_t n);			//m_buf[i] /= buf[i]
		void eleAdd(const T val);													//m_buf[i] += val
		void eleTim(const T val);													//m_buf[i] *= val

		//copy some of the columns from <src> to this.
		void copyColsFrom(const uint32_t startColInThis, const WMat<T>& src, const std::vector<uint32_t> vSrcColId);
		//copy some of the rows from <src> to this.
		void copyRowsFrom(const uint32_t startRowsInThis, const WMat<T>& src, const std::vector<uint32_t> vSrcRowId);

#if WMAT_BOUNDS_CHECK
		void checkSliceBoundary(const char* tag, const uint32_t bufDataSize,
			const uint32_t r0, const uint32_t r1,							//top left position
			const uint32_t c0, const uint32_t c1) const;						//right botton postion
#endif

		bool parseLine(WMat<T>& row, std::string& line, const char delimiter);

		T* m_buf{ nullptr };
		uint32_t	m_rows{ 0 };			//rows
		uint32_t	m_cols{ 0 };			//cols
		uint32_t	m_size{ 0 };			//m_rows * m_cols
		bool			m_isWrapper{ false };
		uint32_t	m_endRow{ 0 }; 		//m_rows-1
		uint32_t	m_endCol{ 0 };		//m_cols-1
	};

	typedef  WMat<double>	  WMatF64;
	typedef  WMat<float>	  WMatF32;
	typedef  WMat<uint8_t>  WMatU8;
	typedef  WMat<uint16_t> WMatU16;
	typedef  WMat<uint32_t> WMatU32;
	typedef  WMat<uint64_t> WMatU64;

	typedef  WMat<int8_t>   WMatI8;
	typedef  WMat<int16_t>  WMatI16;
	typedef  WMat<int32_t>  WMatI32;
	typedef  WMat<int64_t>  WMatI64;

	typedef  WMat<bool>  WMatBool;

  #include "WMatImplPart1.hpp"
	#include "WMatImplPart2.hpp"
  #include "WMatImplPart3.hpp"  
  #include "WMatImplPart4.hpp"  
}
#endif

