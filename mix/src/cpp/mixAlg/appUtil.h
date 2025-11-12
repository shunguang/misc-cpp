/*
* appUtil.h
*
* It contains util data structures and global functions of the mixing application
*
*/

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

#define _USE_MATH_DEFINES
#include <math.h>
namespace mix {
	//a class to take of math interval/range
	template <class T>
	struct AppRng {
		AppRng(const T left_, const T right_, const std::string unit_="")
			: left(left_)
			, right(right_)
			, unit(unit_) 
		{}


		bool isInside(const T x) {
			return (x >= left && x <= right);
		}

		std::string toString( const std::string &msg ) const {
			std::ostringstream ss;
			ss << msg << "[" << left << ", " << right << "]" << unit;
			return ss.str();
		}

	public:
		T left={0};
		T right={0};
		std::string unit = {""};
	};

	//some global functions
	//over simplified log and assertion functions
	void appLog(const std::string& msg);
	void appAssert( const bool flag, const std::string &errMsg );

	//add two vectors such that z[i] = x[i] + y[i], for i=[0, n), where n=x.size()
	template<typename T>
	void vecAdd(std::vector<T>& z, const std::vector<T>& x, const std::vector<T>& y)
	{
		size_t n = x.size();
#if _DEBUG
		appAssert(n == y.size() && n == z.size(), "vecAdd(): sizes do not matche with each other");
#endif

		for (size_t i = 0; i < n; ++i) {
			z[i] = x[i] + y[i];
		}
	}

	//sub two vectors such that z[i] = x[i] - y[i], for i=[0, n), where n=x.size()
	template<typename T>
	void vecSub(std::vector<T>& z, const std::vector<T>& x, const std::vector<T>& y)
	{
		size_t n = x.size();
#if _DEBUG
		appAssert(n == y.size() && n == z.size(), "vecSub(): sizes do not matche with each other");
#endif

		for (size_t i = 0; i < n; ++i) {
			z[i] = x[i] - y[i];
		}
	}

	//multi two vectors such that z[i] = x[i] * y[i], for i=[0, n), where n=x.size()
	template<typename T>
	void vecMul(std::vector<T>& z, const std::vector<T>& x, const std::vector<T>& y)
	{
		size_t n = x.size();
#if _DEBUG
		appAssert(n == y.size() && n == z.size(), "MixAlg::vecMulti(): sizes do not matche with each other");
#endif

		for (size_t i = 0; i < n; ++i) {
			z[i] = x[i] * y[i];
		}
	}

	//<y> is the vertor by inserting (L-1) zero points between x[k] and x[k+1]
	template<typename T>
	void appInsertZeros(std::vector<T>& y, const std::vector<T>& x, const size_t L)
	{
#if _DEBUG
		appAssert(y.size() == x.size() * L, "insertZeros(): size dose not match");
#endif
		size_t i = 0;
		for (const T e : x) {
			y[i++] = e;
			for (size_t j = 1; j < L; ++j) {
				y[i++] = 0;
			}
		}
	}

	//a simple FIR filter
	//y=fir(x,h), where x is the signal and h the kernal
	template<typename T>
	void appFir(std::vector<T>& y, const std::vector<T>& x, const std::vector<T>& h)
	{
		size_t i, j;
		const size_t m = h.size();   //the size of kernal
		const size_t n = x.size();   //the size of uppsampled signal

		//do fir \in [iBeg, iEnd);
		const size_t iBeg = m / 2;
		const size_t iEnd = n - m / 2;

#if _DEBUG
		appAssert( m % 2 != 0, "appFir(): m must be odd");
		appAssert( n == y.size(), "appFir(): x and y must have the same size");
#endif

		//copy head elements
		//todo: 
		for (i = 0; i < iBeg; i++) {
			y[i] = x[i];
		}

		for (i = iBeg; i < iEnd; ++i) {
			T sum = 0;
			const T* ph = h.data();               //start from h[0] 
			const T* px = x.data() + (i - iBeg);   //start from x[i-iBeg]
			for (j = 0; j < m; ++j) {
				sum += (*ph++) * (*px++);
			}
			y[i] = sum;
		}

		//copy tail
		//todo :
		for (i = iEnd; i < n; i++) {
			y[i] = x[i];
		}
	}
}
