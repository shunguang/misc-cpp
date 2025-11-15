#include "UtilFuncs.h"

using namespace std;
using namespace boost;

string cali::num_to_string( const bool x, const std::string &msg )
{
	string s("true");
	if ( !x ){
		s = "false";
	}

	return msg+s;
}

std::string  cali::num_to_string( const uint8_t x,	const std::string &msg )
{
	ostringstream convert;   // stream used for the conversion
	convert << (int)x;		     // insert the textual representation of 'x' in the characters in the stream
	return msg+convert.str();	
}

std::string  cali:: num_to_string( const uint16_t x, const int len, const std::string &msg )
{
	ostringstream ss;
	ss << std::setw(len) << std::setfill('0') << x;
	return msg+ss.str();
}

string cali::num_to_string( const uint32_t x, const int len,  const std::string &msg )
{
	ostringstream ss;
	ss << std::setw(len) << std::setfill('0') << x;
	return msg+ss.str();	
}

string cali::num_to_string( const uint64_t x, const int len, const std::string &msg )
{
	ostringstream ss;
	ss << std::setw(len) << std::setfill('0') << x;
	return msg + ss.str();
}

string cali::num_to_string( const int32_t x, const int len, const std::string &msg )
{
	ostringstream ss;
	ss << std::setw(len) << std::setfill('0') << x;
	return msg + ss.str();
}

std::string cali::num_to_string(const int32_t x, const std::string &msg)
{
	ostringstream convert;
	convert << x;
	return msg + convert.str();
}

std::string cali::num_to_string(const uint16_t x, const std::string &msg)
{
	ostringstream convert;
	convert << x;
	return msg + convert.str();
}
std::string cali::num_to_string(const uint32_t x, const std::string &msg)
{
	ostringstream convert;
	convert << x;
	return msg + convert.str();
}
std::string cali::num_to_string(const uint64_t x, const std::string &msg)
{
	ostringstream convert;
	convert << x;
	return msg + convert.str();
}

string cali::num_to_string( const float x, const std::string &msg )
{
	ostringstream convert;
	convert << x;
	return msg+convert.str();	
}

string cali::num_to_string( const double x, const std::string &msg )
{
	ostringstream convert;
	convert << x;
	return msg+convert.str();	
}

std::string cali::vec_to_string( const std::vector<std::string> &v, const std::string &msg )
{
	ostringstream convert;   // stream used for the conversion
	convert << msg << endl;		     
	convert << "Total Items" << v.size() << endl;
	vector<string>::const_iterator it=v.begin();
	for (; it!=v.end(); ++it ){
		convert << *it << endl;		     
	}
	return msg+convert.str();	
}

bool cali::string_in_vector( const std::string &s, const std::vector<std::string> &v  )
{
	vector<string>::const_iterator it=v.begin();
	for (; it!=v.end(); ++it ){
		if ( 0 == s.compare( *it ) ){
			return true;
		}
	}
	return false;
}

int cali::string_in_vector_relax( const std::string &s, const std::vector<std::string> &v  )
{
	vector<string>::const_iterator it=v.begin();
	int i=0;
	for (; it!=v.end(); ++it, ++i ){
		if ( string::npos != (*it).find( s ) ){
			return i;
		}
	}
	return -1;
}

bool cali::string_to_bool( const std::string &in )
{
	string s(in);
	for ( uint32_t i=0; i<s.size(); ++i ){
		s[i] = std::toupper( s[i] );
	}

	if ( 0==s.compare("YES") || 0==s.compare("Y") ){
		return true;
	}
	else if ( 0==s.compare("TRUE") || 0==s.compare("T") ){
		return true;
	}
	else if( 0==s.compare("1") ){
		return true;
	}
	else{
		return false;
	}
}

uint64_t	cali::string_to_uint64( const std::string &s )
{
	uint64_t x;
	stringstream strm(s);

	strm >> x;

	return x;
}

uint32_t cali::string_to_vector( vector<string> &v, const string &s, const string &key, const bool keepKey )
{

	v.clear();

	uint32_t L = key.size();
	uint32_t pos = 0;
	std::string::size_type found = s.find(key, pos);
	while( found !=std::string::npos ){
		uint32_t len = found - pos;
		if( keepKey ){
			len += L;
		}
		if( len>0 ){
			v.push_back( s.substr( pos, len) );
		}

		pos = found + L;
		found = s.find(key, pos);
	}
	if( pos <= s.size()-1 ){
		v.push_back( s.substr( pos ) );
	}

	return v.size();
}

std::string	cali :: vector_to_string( const std::vector<std::string> &v, const std::string &seperateKey )
{
	if ( v.size()==0 ){
		return "";
	}

	string s=v[0];
	for( uint32_t i=1; i<v.size(); ++i ){
		s += (seperateKey + v[i]);
	}

	return s;
}

std::string cali::vector_to_string(const std::vector<uint8_t>& v) 
{
	std::string s;
	for ( const uint8_t x : v ) {
		s.push_back( (char)x);
	}
	return s;
}

void cali :: string_trim( string &x, const char ch )
{
	if ( x.empty() ){
		return;
	}

	//ersase end
	string::iterator it = x.end();
	while( *(--it) == ch ){
		x.erase(it);
		if( x.empty() ){
			return;
		}
		it = x.end();
	}

	//erase begin
	it = x.begin();
	while( *it==ch ){
		x.erase( it );
		if( x.empty() ){
			return;
		}
		it = x.begin();
	}

}


//---------------------------------
/*
s = "[$SYSTEMNAME]> show statworpsutbl
WORP SU Statistics Table
------------------------
Channel Bandwidth               :                 10 MHz
SU MAC Address                  :               00:20:A6:C1:5F:C8
SU Local Tx Rate                :                 6 Mbps
SU Remote Tx Rate               :                 6 Mbps
SU Average Local Signal         :                    -33
SU Average Local Noise          :                    -89
SU Average Remote Signal        :                    -35
SU Average Remote Noise         :                    -92
SU Distance                     :               100 meters"
*/
//------------------------------------------------------------------
// call string_parse(s, "Channel Bandwidth" ) return "10 MHz"

std::string cali::string_parse( const std::string &x, const std::string &key )
{
	string y;

	uint32_t found = x.find( key );
	if ( found == string::npos ){
		return "";
	}

	y = x.substr( found+key.size() );

	found = y.find( ':' );
	y = y.substr( found+1 );
	found = y.find( '\r' );
	y = y.substr(0, found );
	
	string_trim(y, ' ');
	string_trim(y, '\t');
	string_trim(y, '\n');
	string_trim(y, ' ');

	return y;
}
//        01234567890123456    
// mac = "01-23-45-67-89-ab" or 01:23:45:67:89:ab
std::string cali::string_find_mac( const std::string &x, const char &seperationKey )
{
	int L = (int)(x.size());
	for( int i=0; i<=L-17; ++i ){
		if( x[i+2]==seperationKey && x[i+5]==seperationKey && x[i+8]==seperationKey && x[i+11]==seperationKey && x[i+14]==seperationKey ){
			return x.substr(i, 17 );
		}
	}
	return "";
}

//remove the beg and end chars if any of them are in $keys$
void cali:: string_trim( string &x, const std::vector<char> &keys )
{
	if ( x.empty() ){
		return;
	}
	uint32_t n=keys.size();

	//ersase end
	string::iterator it = x.end();
	while( 1 ){
		char ch = *(--it);
		bool found=false;
		for (uint32_t i=0; i<n; ++i){
			if ( ch==keys[i] ){
				found=true;
				break;
			}
		}

		if( !found ){
			break;
		}
		else{
			x.erase(it);
			if( x.empty() ){
				return;
			}
			it = x.end();
		}
	}

	//erase begin
	it = x.begin();
	while( 1 ){
		char ch = *it;
		bool found=false;
		for (uint32_t i=0; i<n; ++i){
			if ( ch==keys[i] ){
				found=true;
				break;
			}
		}

		if( !found ){
			break;
		}
		else{
			x.erase( it );
			if( x.empty() ){
				return;
			}
			it = x.begin();
		}
	}

}

void	 cali:: string_find_n_replace( std::string &x, const char chFind, const char chRaplace )
{
	for(uint32_t i=0; i<x.size(); ++i){
		if ( x[i]==chFind ) {
			x[i] = chRaplace;
		}
	}
}


void cali::vstr_read_txt_file( std::vector<std::string> &vLines, const std::string &fileName, const char commentChar )
{

	std::string line;
	std::ifstream myfile( fileName );

	while (std::getline(myfile, line)) {
		cali::string_trim( line, ' ' );

		if( line.empty() ){
			continue;
		}
		if ( line[0]==commentChar ){
			continue;
		}

		vLines.push_back(line);	
	}
}

void cali:: vstr_select_util( std::vector<std::string> &retLines, const std::vector<std::string> &vAllLines, const std::string &startKey, const std::string &stopKey )
{
	retLines.clear();
	uint32_t n =vAllLines.size();
	for ( uint32_t i=0; i<n; ++i ){
		const string &line = vAllLines[i];
		if( string::npos == line.find( startKey ) ){
			continue;
		}

		uint32_t j=i+1;
		while(j<n){
			const string &nextLine = vAllLines[j];
			if( string::npos == nextLine.find(stopKey) ){
				retLines.push_back( nextLine );
			}
			else{
				return;
			}
			++j;
		}
	}
}

//---------------------------------------------------------------
//given vector of lines such as:
//
//ipv4=10.0.4.101
//macWired=ba:wi:cc:dd:ee:01
//macWireless=ba:wl:cc:dd:ee:01
//
//query value (RHS) from key (LHS)
//
//example: vstr_find_value("ipv4") will return "10.0.4.101"
//---------------------------------------------------------------

std::string cali::vstr_find_value( const std::vector<std::string> &vLines, const std::string &key )
{
	std::vector<std::string>::const_iterator it=vLines.begin();
	std::vector<std::string> kv;
	for ( ; it!=vLines.end(); ++it ){
		cali::string_to_vector( kv, *it, "=", false );
		if ( kv.size()<2 ){
			continue;
		}
		if ( 0 == key.compare( kv[0] ) ){
			return kv[1];
		}
	}

	return "";
}

std::string cali::getPrettyTimeStamp(const bool dayTimeOnly)
 {
	boost::posix_time::ptime now = POSIX_LOCAL_TIME;  
	return getPrettyTimeStamp(now, dayTimeOnly);
 }

std::string cali::getPrettyTimeStamp(const boost::posix_time::ptime &t, const bool dayTimeOnly)
{
	string stimeStamp = to_simple_string(t);  //[YYYY-mmm-DD HH:MM:SS.fffffffff]
	cali::string_find_n_replace(stimeStamp, ' ', '-');
	cali::string_find_n_replace(stimeStamp, ':', '-');
	cali::string_find_n_replace(stimeStamp, '.', '-');

	if (dayTimeOnly){
		return stimeStamp.substr(12, 11);
	}
	return stimeStamp;
}

uint32_t  cali::checkTimeOut( const boost::posix_time::ptime &start, const uint32_t thdInMillisec )
{
	boost::posix_time::ptime now = POSIX_LOCAL_TIME;
	boost::posix_time::time_duration dt = now - start;
	
	int64 dt_milliseconds = dt.total_milliseconds();

	if( dt_milliseconds >= thdInMillisec ){
		//already timeout
		return 0;
	}
	else{
		return thdInMillisec - dt_milliseconds;
	}
}

uint32_t  cali::timeIntervalMillisec( const boost::posix_time::ptime &start )
{
	boost::posix_time::ptime end = POSIX_LOCAL_TIME;
	boost::posix_time::time_duration dt = end - start;
	
	return (uint32_t)(dt.total_milliseconds());
}

uint32_t cali::timeIntervalMillisec( const boost::posix_time::ptime &start, const boost::posix_time::ptime &end )
{
	boost::posix_time::time_duration dt = end - start;
	return (uint32_t)(dt.total_milliseconds());
}


int8_t cali::int32_to_int8( const int32_t x )
{
	if ( x < -128 )
		return -128;
	else if( x>127 )
		return 127;
	else 
		return x;
}


void cali::printfStr(const std::string &s, const std::string &msg)
{
	uint32_t n = s.size();
	printf( "%s, (n=%d): ", msg.c_str(),n );
	for (uint32_t i = 0; i < n; ++i){
		printf("(%d,%c,%d);", i, s[i], s[i]);
	}
	printf("\n");
}

//generate about <n> distinct/unique uniform distributed random number in [a,b]
void cali::genUniformDistinctRands(std::vector<int> &v, const int n, const int a, const int b)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(a, b);

	v.resize(n);
	for (int i = 0; i < n; ++i) {
		v[i] = distribution(generator);
	}

	//v ={1,2,3,1,2,3,3,4,5,4,5,6,7}
	std::sort(v.begin(), v.end());  // 1 1 2 2 3 3 3 4 4 5 5 6 7 
	auto last = std::unique(v.begin(), v.end());
	// v now holds {1 2 3 4 5 6 7 x x x x x x}, where 'x' is indeterminate
	v.erase(last, v.end());
}

void cali::dumpCorners(const std::string &fPath, const std::vector<cv::Point2f> &corners)
{
	if (corners.size() <= 0) { 
		return;
	}

	FILE *fp = fopen(fPath.c_str(), "w");
	for( int i=0; i<corners.size(); ++i) {
		const cv::Point2f &p = corners[i];
		fprintf(fp, "%d,%.6f,%.6f\n", i, p.x, p.y);
	}
	fclose(fp);
}

void cali::loadCorners(const std::string &fPath, std::vector<cv::Point2f> &corners)
{
	int i;
	cv::Point2f p;
	corners.clear();
	FILE *fp = fopen(fPath.c_str(), "r");
	while (1) {
		int ret = fscanf(fp, "%d,%f,%f", &i, &p.x, &p.y);
		if ( feof(fp) || ret < 3 ) { break; }

		printf("%d,%.6f,%.6f\n", i, p.x, p.y);
		corners.push_back(p);
	}
	fclose(fp);
}


std::string  cali::ipConvertNum2Str(const uint32_t ip)
{
	char buf[16];
	unsigned char bytes[4];
	bytes[0] = ip & 0xFF;
	bytes[1] = (ip >> 8) & 0xFF;
	bytes[2] = (ip >> 16) & 0xFF;
	bytes[3] = (ip >> 24) & 0xFF;
	snprintf(buf, 16, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
	return std::string(buf);
}

uint32_t cali::ipConvertStr2Num(const std::string &ip)
{
	int a, b, c, d;

	sscanf(ip.data(), "%d.%d.%d.%d", &a, &b, &c, &d);
	assert(a <= 255);
	assert(b <= 255);
	assert(c <= 255);
	assert(d <= 255);

	uint32_t x = d;
	x |= (c << 8);
	x |= (b << 16);
	x |= (a << 24);

	return x;
}

std::string cali::appToString(const char *fmt, ...)
{
	char buffer[2048];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 2048, fmt, args);
	va_end(args);
	return std::string(buffer);
}


//#include <unistd.h>
//#include <linux/reboot.h>
//#include <sys/reboot.h>
void cali::shutdownLinuxMachine() {
    //sync();
    //reboot(LINUX_REBOOT_CMD_POWER_OFF);

	//reboot(LINUX_REBOOT_MAGIC1, 
    //       LINUX_REBOOT_MAGIC2, 
    //      LINUX_REBOOT_CMD_POWER_OFF, 0);
}
