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

#ifndef __UTIL_FUNCS_H__
#define __UTIL_FUNCS_H__
#include "AppDefs.h"
namespace app {
	std::string  num_to_string( const bool x ,const std::string &msg="");
	std::string  num_to_string(const uint8_t x,const std::string &msg = "");

	std::string  num_to_string( const int32_t x, const int len, const std::string &msg="" );
	std::string  num_to_string( const uint16_t x, const int len, const std::string &msg="" );
	std::string  num_to_string( const uint32_t x, const int len, const std::string &msg="" );
	std::string  num_to_string( const uint64_t x, const int len, const std::string &msg="" );

	std::string  num_to_string(const int32_t x,  const std::string &msg = "");
	std::string  num_to_string(const uint16_t x, const std::string &msg = "");
	std::string  num_to_string(const uint32_t x, const std::string &msg = "");
	std::string  num_to_string(const uint64_t x, const std::string &msg = "");

	std::string  num_to_string( const float x,	const std::string &msg="" );
	std::string  num_to_string(const double x, const std::string &msg = "");

	std::string  vec_to_string( const std::vector<std::string> &v, const std::string &msg="" );

	bool	 string_in_vector( const std::string &s, const std::vector<std::string> &v );
	int		 string_in_vector_relax( const std::string &s, const std::vector<std::string> &v );

	bool	 string_to_bool( const std::string &s );
	uint64_t	 string_to_uint64( const std::string &s );
	size_t	 string_to_vector( std::vector<std::string> &v, const std::string &s, const std::string &key, const bool keepKey=false );

	int8_t	 int32_to_int8( const int32_t x );

	std::string	 vector_to_string( const std::vector<std::string> &v, const std::string &seperateKey );
	std::string  vector_to_string( const std::vector<uint8_t>& v);

	//remove all chars $ch$ at the beging and end of the string.
	void	 string_trim( std::string &x, const char ch=' ' );
	void	 string_trim( std::string &x, const std::vector<char> &keys );
	void	 string_find_n_replace( std::string &x, const char chFind, const char chRaplace );

	std::string   string_parse( const std::string &x, const std::string &key );
	std::string   string_find_mac( const std::string &x, const char &seperationKey=':' );

	//process vector of string
	void  vstr_read_txt_file( std::vector<std::string> &vLines, const std::string &fileName, const char commentChar );
	void  vstr_select_util( std::vector<std::string> &retLines, const std::vector<std::string> &vAllLines, const std::string &startKey, const std::string &stopKey );
	std::string  vstr_find_value(  const std::vector<std::string> &vLines, const std::string &key );

	//time related funcs
	////return [YYYY-mmm-DD-HH-MM-SS.fffffffff]
	std::string  getPrettyTimeStampNow( const bool dayTimeOnly=false );
	std::string  getPrettyTimeStampNow(const APP_TIME_MS t, const bool dayTimeOnly = false);


	bool  isTimeOut( const APP_TIME_MS start, const uint32_t thdInMillisec );
	APP_TIME_MS  timeIntervalMillisec( APP_TIME_MS &start );

	void  printByteInStr( const std::string &s, const std::string &msg );

	//generate about <n> distinct/unique uniform distributed random number in [a,b]
	void  genUniformDistinctRands( std::vector<int> &v, const int n, const int a, const int b );

	uint32_t  ipConvertStr2Num(const std::string &ip);
	std::string   ipConvertNum2Str(const uint32_t ip);
	std::string   appToString(const char *fmt, ...);

	template <typename T>
	void sort_indexes(const std::vector<T>& v, std::vector<size_t>& vIdx)
	{
		assert(v.size() == vIdx.size());

		//initialize original index locations
		std::iota(vIdx.begin(), vIdx.end(), 0);

		// sort indexes based on comparing values in v
		// using std::stable_sort instead of std::sort
		// to avoid unnecessary index re-orderings
		// when v contains elements of equal values 
		stable_sort(vIdx.begin(), vIdx.end(),
			[&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
	}

	void  shutdownLinuxMachine();
}
#endif
