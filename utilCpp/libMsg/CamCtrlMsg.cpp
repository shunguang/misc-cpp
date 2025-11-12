#include "CamCtrlMsg.h"

using namespace app;
using namespace std;
CamCtrlMsg::CamCtrlMsg()
: Msg(MSG_SYNC_WORD, MSG_T_CAM_CTRL)
, apertureFlag( 0 )
, focusFlag(0)
, exposureFlag(0)
, flag1(0)
, authorization(0)
, clientIp(0)
{
}

CamCtrlMsg ::~CamCtrlMsg()
{
}

CamCtrlMsg::CamCtrlMsg(const CamCtrlMsg &x)
: Msg(x)
, apertureFlag( x.apertureFlag )
, focusFlag( x.focusFlag )
, exposureFlag( x.exposureFlag )
, flag1(x.flag1)
, authorization(x.authorization)
, clientIp(x.clientIp)
{
}

CamCtrlMsg& CamCtrlMsg ::operator = (const CamCtrlMsg &x)
{
	if (this != &x) {
		Msg::operator=(x);
        apertureFlag = x.apertureFlag;
        focusFlag = x.focusFlag;
		exposureFlag = x.exposureFlag;
        flag1 = x.flag1;
        authorization = x.authorization;
		clientIp = x.clientIp;
	}
	return *this;
}


//read msg from stream <is>
void CamCtrlMsg::read(Uint8Stream &is)
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
	is.read(apertureFlag);		//class member
	is.read(focusFlag);			//class memeber	
	is.read(exposureFlag);
	is.read(flag1);				//class memeber	
	is.read(authorization);		//class memeber	
	is.read(clientIp);
	is.read(reserved);

	//----------------------------------------------
	//read tail body
	//----------------------------------------------
	is.read(checksum);
}

//write a msg into stream for sending
void CamCtrlMsg::write(Uint8Stream &os)
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
	os.write(apertureFlag);
	os.write(focusFlag);
	os.write(exposureFlag);
	os.write(flag1);
	os.write(authorization);
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


std::string CamCtrlMsg::toString() const
{
	//head
	string s = getHeadStr();

	//body
	s +=  ",apertureFlag=" + std::to_string(apertureFlag) 
           + ",focusFlag=" + std::to_string(focusFlag) 
		   + ",exposureFlag=" + std::to_string(exposureFlag);
           +  ",flag1=" +std::to_string(flag1) 
           +  ",authorization=" +std::to_string(authorization)
           +  ",clientIp=" +std::to_string(clientIp);

	//tail
	s += getTailStr();

	return s;
}
