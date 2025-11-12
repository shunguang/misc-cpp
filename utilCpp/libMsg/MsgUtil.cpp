#include "MsgUtil.h"
using namespace std;
using namespace app;

MsgPtr app::rawToMsgPtr(const std::string &raw, bool &passCrc)
{
	MsgPtr newMsg(0);
	uint16_t crc;
	uint32_t synWord, type, len;
	bool good = getMsgInfo(raw, synWord, type, len, crc);
	if (!good){
		dumpLog( "app::rawToMsgPtr(): cannot pass msg info!" );
		return newMsg;
	}

	switch (type){
	case MSG_T_STATUS:
		newMsg.reset( new StatusMsg());
		break;
	case MSG_T_SETTINGS:
		newMsg.reset(new SettingsMsg());
		break;
	case MSG_T_STAB:
		newMsg.reset(new StabMsg());
		break;
	case MSG_T_CAM_CTRL:
		newMsg.reset(new CamCtrlMsg());
		break;
	case MSG_T_ERROR:
		newMsg.reset(new ErrorMsg());
		break;
	default:
		dumpLog("app::rawToMsgPtr(): wrong msg type (=%d), hit assert(0)!", type);
	}
	if (newMsg) {
		passCrc = newMsg->readFromStr(raw);
		if (!passCrc) {
			dumpLog("app::rawToMsgPtr(): Warnning --- cannot pass crc check!");
		}
	}
	return newMsg;
}


#if 0
MsgPtr app::rawToStreamMsgPtr(const std::string &raw, const uint32_t fileType, const uint32_t udpPackSize)
{
	MsgPtr newMsg(0);
	uint16_t crc;
	uint32_t synWord, type, len;
	bool good = getMsgInfo(raw, synWord, type, len, crc);
	if (!good){
		dumpLog("app::rawToStreamMsgPtr(): cannot pass msg info!");
		return newMsg;
	}

	//appAssert(type == MSG_T_FILE_STREAM, __FILE__, __LINE__);

	if (type == MSG_T_FILE_STREAM) {
		StreamMsgHd hd2(fileType, 0, 0, 0, udpPackSize);
		newMsg.reset(new StreamMsg(hd2));
		bool passCrc = newMsg->readFromStr(raw);
		if (!passCrc) {
			dumpLog("app::rawToStreamMsgPtr(): A-cannot pass crc check!");
		}
	}
	else if (type == MSG_T_LFC_STATUS) {
		newMsg.reset(new LfcStatusMsg() );
		bool passCrc = newMsg->readFromStr(raw);
		if (!passCrc) {
			dumpLog("app::rawToStreamMsgPtr(): B-cannot pass crc check!");
		}
	}
	else {
		appAssert( false, __FILE__, __LINE__);
	}
	return newMsg;
}


//todo: this function only called once with <isResponseMsg=true>
//      this is too expensive (large memo allocate)  
MsgPtr app::rawToStreamMsgPtr(const std::string &raw, const MsgHd &hd1, const StreamMsgHd &hd2, const bool isResponseMsg)
{
	MsgPtr newMsg(new StreamMsg(hd2, isResponseMsg));
	if (isResponseMsg){
		StreamMsg *p = dynamic_cast<StreamMsg*>(newMsg.get());
		p->hd.length = hd1.length;
	}
	else{
		//todo:  newMsg->readFromStr() too expensive to large size msgs
		bool passCrc = newMsg->readFromStr(raw);
		if (!passCrc){
			dumpLog("app::rawToMsgPtr(): cannot pass crc check!");
		}
	}
	return newMsg;
}

bool app::getStreamMsgHead(const MsgPtr &msg, MsgHd &hd1, StreamMsgHd &hd2)
{
	appAssert( msg->hd.type == MSG_T_FILE_STREAM, __FILE__, __LINE__);

	hd1.syncWord = msg->hd.syncWord;
	hd1.type = msg->hd.type;
	hd1.length = msg->hd.length;

	StreamMsg *p = dynamic_cast<StreamMsg*>(msg.get());
	hd2 = p->hd2;

	return true;
}
#endif
