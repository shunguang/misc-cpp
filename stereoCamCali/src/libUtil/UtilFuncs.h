#ifndef __UTIL_FUNCS_H__
#define __UTIL_FUNCS_H__
#include "DataTypes.h"
namespace cali {
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
	uint32_t	 string_to_vector( std::vector<std::string> &v, const std::string &s, const std::string &key, const bool keepKey=false );

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
	std::string  getPrettyTimeStamp( const bool dayTimeOnly=false );
	std::string  getPrettyTimeStamp(const boost::posix_time::ptime &t, const bool dayTimeOnly = false);


	uint32_t  checkTimeOut( const boost::posix_time::ptime &start, const uint32_t thdInMillisec );
	uint32_t  timeIntervalMillisec( const boost::posix_time::ptime &start );
	uint32_t  timeIntervalMillisec( const boost::posix_time::ptime &start, const boost::posix_time::ptime &end );

	void  printfStr( const std::string &s, const std::string &msg );

	//generate about <n> distinct/unique uniform distributed random number in [a,b]
	void  genUniformDistinctRands( std::vector<int> &v, const int n, const int a, const int b );
	void  dumpCorners(const std::string &fPath, const std::vector<cv::Point2f> &corners);
	void  loadCorners(const std::string &fPath, std::vector<cv::Point2f> &corners);

	uint32_t  ipConvertStr2Num(const std::string &ip);
	std::string   ipConvertNum2Str(const uint32_t ip);
	std::string   appToString(const char *fmt, ...);

	void  shutdownLinuxMachine();
}
#endif
