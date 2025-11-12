#include "StatusMsg.h"

using namespace app;
using namespace std;
StatusMsg::StatusMsg(const uint8_t perct_, const uint16_t errMsk_)
: Msg(MSG_SYNC_WORD, MSG_T_STATUS)
, perct( perct_ )
, reserved8(MSG_UINT8_RESERVED)
, reserved16(errMsk_)
{
}

StatusMsg ::~StatusMsg()
{
}

StatusMsg::StatusMsg(const StatusMsg &x)
: Msg(x)
, perct(x.perct)
, reserved8(x.reserved8)
, reserved16(x.reserved16)
{
}

StatusMsg& StatusMsg ::operator = (const StatusMsg &x)
{
	if (this != &x) {
		Msg::operator=(x);
		perct = x.perct;
		reserved8 = x.reserved8;
		reserved16 = x.reserved16;
	}
	return *this;
}

//read msg from stream <is>
void StatusMsg::read(Uint8Stream &is)
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
	is.read(perct);				//class member
	is.read(reserved8);			//class memeber	
	is.read(reserved16);			//class memeber	

	//----------------------------------------------
	//read tail body
	//----------------------------------------------
	is.read(reserved);
	is.read(checksum);
}

//write a msg into stream for sending
void StatusMsg::write(Uint8Stream &os)
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
	os.write(perct);
	os.write(reserved8);
	os.write(reserved16);

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


std::string StatusMsg::toString() const
{
	//header
	string s = getHeadStr();

	//body
	s += ",perct=" + std::to_string(perct) + ", reserved8=" + std::to_string(reserved8)+ ", reserved16=" + std::to_string(reserved16);

	//tail
	s += getTailStr();

	return s;
}
