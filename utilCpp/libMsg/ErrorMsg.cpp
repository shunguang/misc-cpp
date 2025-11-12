#include "ErrorMsg.h"

using namespace app;
using namespace std;

ErrorMsg::ErrorMsg( const std::string &s,  const uint32_t clientIp_)
: Msg(MSG_SYNC_WORD, MSG_T_ERROR)
, clientIp(clientIp_)
{
	int L = std::min<int>(MSG_ERR_MSG_LEN - 1, s.size() );
	for (int i = 0; i < L; ++i){
		buf[i] = s[i];
	}
	buf[L] = '\0';
}

ErrorMsg::ErrorMsg(const ErrorMsg &x)
: Msg(x)
, clientIp( x.clientIp )
{
	strcpy(buf, x.buf);
}

ErrorMsg& ErrorMsg ::operator = (const ErrorMsg &x)
{
	if (this != &x) {
		Msg::operator=(x);
		strcpy(buf, x.buf);
		clientIp = x.clientIp;
	}
	return *this;
}


//read msg from stream <is>
void ErrorMsg::read(Uint8Stream &is)
{
	//----------------------------------------------
	//read message header
	//----------------------------------------------
	is.read(hd.syncWord);
	is.read(hd.type);
	is.read(hd.length);

	//----------------------------------------------
	//read message body
	//----------------------------------------------
	is.read((uint8_t *)buf, MSG_ERR_MSG_LEN);				//class member
	is.read(clientIp );
	is.read(reserved);

	//----------------------------------------------
	//read tail
	//----------------------------------------------
	is.read(checksum);
}

//write a msg into stream for sending
void ErrorMsg::write( Uint8Stream &os )
{
	os.resetWrite();
	//----------------------------------------------
	//write message header
	//----------------------------------------------
	os.write(hd.syncWord, false);
	os.write(hd.type, false);
	os.write(hd.length, false);

	//----------------------------------------------
	//write message body
	//----------------------------------------------
	os.write((uint8_t *)buf, MSG_ERR_MSG_LEN);
	os.write(clientIp);
	os.write(reserved);

	//----------------------------------------------
	//cal. body length and checksum
	//----------------------------------------------
	hd.length = os.getLength();
	checksum = os.getChecksum();

	//----------------------------------------------
	//write message tail and update length
	//----------------------------------------------
	os.write(checksum, false);
	os.writeAt(8, hd.length);   //update length
}


std::string ErrorMsg::toString() const
{
	//header
	string s = getHeadStr();

	//body
	s += std::string(",clientIp=") + std::to_string(clientIp) + "," + std::string(buf);

	//tail
	s += getTailStr();

	return s;
}
