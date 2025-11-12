#include "StabMsg.h"

using namespace app;
using namespace std;
StabMsg::StabMsg(const uint8_t stabOn_, const uint32_t clientIp_ )
: Msg(MSG_SYNC_WORD, MSG_T_STAB)
, stabOn( stabOn_ )
, authorization(0)
, reserved16(MSG_UINT16_RESERVED)
, clientIp(clientIp_)
{
}

StabMsg ::~StabMsg()
{
}

StabMsg::StabMsg(const StabMsg &x)
: Msg(x)
, stabOn(x.stabOn)
, authorization(x.authorization)
, reserved16(x.reserved16)
, clientIp(x.clientIp)
{
}

StabMsg& StabMsg ::operator = (const StabMsg &x)
{
	if (this != &x) {
		Msg::operator=(x);
		stabOn = x.stabOn;
		authorization = x.authorization;
		reserved16 = x.reserved16;
		clientIp = x.clientIp;
	}
	return *this;
}

//read msg from stream <is>
void StabMsg::read(Uint8Stream &is)
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
	is.read(stabOn);				//class member
	is.read(authorization);			//class memeber	
	is.read(reserved16);			//class memeber	
	is.read(clientIp);				//class memeber	
	is.read(reserved);
	//----------------------------------------------
	//read tail
	//----------------------------------------------
	is.read(checksum);
}

//write a msg into stream for sending
void StabMsg::write(Uint8Stream &os)
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
	os.write(stabOn);
	os.write(authorization);
	os.write(reserved16);
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
	os.writeAt(8, hd.length);
}


std::string StabMsg::toString() const
{
	//header
	string s = getHeadStr();

	//body
	s += ",stab=" + std::to_string(stabOn) 
		+ ", authorization=" + std::to_string(authorization) 
		+ ", reserved16=" + std::to_string(reserved16)
		+ ", clientIp=" + std::to_string(clientIp);

	//tail
	s += getTailStr();

	return s;
}
