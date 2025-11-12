#include "Msg.h"

using namespace std;
using namespace app;

Msg::Msg()
	: hd (MSG_SYNC_WORD, MSG_T_UNKN, 0)
	, reserved	( 0x0000 )
	, checksum	( 0 )
	, ts		( MSG_LOCAL_TIMESTAMP_NOW )
{
}

Msg::~Msg()
{
}

Msg::Msg( const uint32_t syncWord_, const uint32_t type_ )
	: hd( syncWord_, type_ ,0 )
	, reserved	( 0x0000 )
	, checksum	( 0 )
	, ts		(MSG_LOCAL_TIMESTAMP_NOW)
{
}

Msg::Msg( const Msg &x )
	: hd( x.hd )
	, reserved	( x.reserved )
	, checksum	( x.checksum )
	, ts		( x.ts )
{
}

Msg& Msg::operator = (  const Msg &x )
{
	assert( this != &x );
	
	hd= x.hd;
	reserved	= x.reserved;
	checksum	= x.checksum;
	ts			= x.ts;
	return *this;	
}


bool Msg::readFromStr( const std::string &msg )
{
	if (msg.size() < MSG_HEAD_TAIL_SIZE) {
		return false;
	}

	uint32_t synWord_, type_, length_, newLength_;
	uint16_t crc_, newCrc_;

	bool good = getMsgInfo( msg, synWord_, type_, length_, crc_ );
	if( !good || hd.syncWord != MSG_SYNC_WORD ){
		return false;
	}


	uint32_t capacity = MSG_SMALL_PACK_SIZE;
	if (type_ == MSG_T_FILE_STREAM){
		capacity = MSG_FILE_STREAM_PACK_SIZE;
	}

	//write msg into $Uint8Stream$
	uint32_t L = msg.size() - 2;   //not include crc which is 2 bytes
	app::Uint8Stream is(capacity); 
	is.write( synWord_, false );
	is.write( type_, false );
	is.write( length_, false );

	uint8_t *p0 = (uint8_t*)msg.c_str();
	p0 += 12;
	is.write( p0, L-12 );

	newCrc_		= is.getChecksum();
	newLength_	= is.getLength();
	if( crc_ != newCrc_ || length_ != newLength_  ){
		dumpLog("Msg::readFromStr(): crc and length check error, msg does not read successfully crcFromMsg=%d, crcCal=%d, lenFrmMsg=%d, lenCal=%d",
		crc_, newCrc_, length_, newLength_ );
		return false;
	}
	is.write( crc_, false );
	//if pass crc read msg from $Uint8Stream$
	read( is );
	return true;
}

bool Msg::verifyLengthAndChecksum()
{
	uint32_t readLenth	= hd.length;
	uint16_t readChecksum	= checksum;

	//inside read function, the $length$ and $checksum$ will be recalculated and reassigned
	app::Uint8Stream os;
	write( os );
		
	if( readLenth != hd.length || readChecksum != checksum ){
		hd.length = readLenth;
		checksum = readChecksum;
		return false;
	}

	return true;
}

std::string Msg::toString() const
{
	return hd.toString();	
}

std::string Msg::getHeadStr() const
{
	ostringstream ss;   // stream used for the conversion
	ss << ", ts=" << app::getPrettyTimeStamp(ts);
	ss << ", syncWord=" << hd.syncWord;
	ss << ", type=" << (int)hd.type << "(" << getMsgTypeLabel(hd.type) << ")";
	ss << ", length=" << hd.length;
	return ss.str();
}

std::string Msg::getTailStr() const
{
	ostringstream ss;   // stream used for the conversion
	ss << ", reserved=" << reserved;
	if (reserved == 1){
		ss << "(ack)";
	}
	ss <<",crc=" + std::to_string(checksum);
	return ss.str();
}

void Msg::dumpTxt(const std::string& fileName, const uint16_t crc) const
{
	MyCout mycout(fileName);
	mycout.out << toString() << endl;
}

void Msg::dumpBinary( const std::string &fileName ) 
{
	Uint8Stream os(1024);
	write( os );
	os.dumpBin( fileName.c_str() );
}

void Msg::loadBinary(  const std::string &fileName )
{
	Uint8Stream is(1024);
	is.loadBin( fileName.c_str() );
	read( is );
}


uint32_t app::getMsgType(const std::string &msg)
{
	uint32_t n = msg.size();
	if (n < MSG_HEAD_TAIL_SIZE){
		return MSG_T_UNKN;
	}

	uint32_t *p1 = (uint32_t *)msg.c_str();
	return *(p1 + 1);
}

//return true if msg.size()>=16, false otherwise
bool app::getMsgInfo( const std::string &msg, uint32_t &synWord, uint32_t &type, uint32_t &len, uint16_t &crc )
{
	uint32_t n = msg.size();
	if(  n < MSG_HEAD_TAIL_SIZE ){
		return false;
	}

	uint32_t *p1 = (uint32_t *)msg.c_str();
	synWord = *p1++;
	type = *p1++;
	len = *p1;

	uint32_t L = n - sizeof(uint16_t);  //crc is uint16_t type
	uint16_t *p2 = (uint16_t *) &(msg[L]);
	crc = *p2;


	//printfStr(msg, "msg=");
	//printf("synWord=%d, type=%d, len=%d, crc=%d\n", synWord, type, len, crc);
	//assert( n == 14+len );
	if (n != 14 + len){
		dumpLog("app::getMsgInfo(): msg len invalid!");
	}

	return true;
}

bool app::getMsgFromRawStr( std::string &raw, std::string &msg )
{
	uint32_t n = raw.size();
	if ( n < MSG_HEAD_TAIL_SIZE){
		return false;
	}

	//printfStr(raw, "raw1=");

	uint32_t synWord, type, len;
	const char *p0 = raw.c_str();


	uint32_t *p1 = (uint32_t *)p0;
	synWord = *p1++;
	if (synWord != MSG_SYNC_WORD){
		//todo: be smart to skip all the bytes util find the SYNC_WORD
		raw = raw.substr(4, string::npos);
		dumpLog("app::getMsgFromRawStr(): sth is wrong!!!, raw=%s", raw.c_str() );
		return false;
	}

	type = *p1++;
	len = *p1;

	uint32_t L = 14 + len;
	if (n < L) {
		return false;
	}

	msg = raw.substr(0, L);
	raw = raw.substr(L, string::npos );

	//printfStr(raw, "raw2=");
	//printfStr(msg, "msg=");

	return true;
}
