#include "SettingsMsg.h"

using namespace app;
using namespace std;
SettingsMsg::SettingsMsg()
: Msg(MSG_SYNC_WORD, MSG_T_SETTINGS)
, settings()
{
}

SettingsMsg::SettingsMsg(const Settings &s)
: Msg(MSG_SYNC_WORD, MSG_T_SETTINGS)
, settings(s)
{
}

SettingsMsg ::~SettingsMsg()
{
}

SettingsMsg::SettingsMsg(const SettingsMsg &x)
: Msg(x)
, settings(x.settings)
{
}


bool SettingsMsg ::operator == (const SettingsMsg &x)
{
	return (settings == x.settings);
}

SettingsMsg& SettingsMsg ::operator = (const SettingsMsg &x)
{
	assert(this != &x);

	Msg::operator=(x);
	settings = x.settings;

	return *this;
}

//read msg from stream <is>
void SettingsMsg::read( Uint8Stream &is )
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
	settings.read(is);

	//----------------------------------------------
	//read tail body
	//----------------------------------------------
	is.read(reserved);
	is.read(checksum);
}

//write a msg into stream for sending
void SettingsMsg::write(Uint8Stream &os)
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
	settings.write(os);
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


std::string SettingsMsg::toString() const
{
	//head
	string s = getHeadStr();

	//body
	s += "\nBody:" + settings.toString();

	//tail
	s += getTailStr();

	return s;
}


void SettingsMsg::setChipInfo(int x, int y, int w, int h)
{
	settings.chipX = x;
	settings.chipY = y;
	settings.chipW = w;
	settings.chipH = h;
}


void SettingsMsg::setBkgImgSize(int w, int h)
{
	settings.bkgImgW = w;
	settings.bkgImgH = h;
}

void SettingsMsg::setSaveInRemote(bool flag)
{
	settings.setLfcSave(flag);
}
