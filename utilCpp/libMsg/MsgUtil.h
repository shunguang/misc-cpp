#ifndef __MSG_UTIL_H__
#define __MSG_UTIL_H__

#include "libUtil/util.h"

#include "StatusMsg.h"
#include "ErrorMsg.h"
#include "SettingsMsg.h"
#include "StabMsg.h"
#include "CamCtrlMsg.h"
namespace app{
	MsgPtr MSG_EXPORT rawToMsgPtr( const std::string &raw, bool &passCrc);

	//MsgPtr MSG_EXPORT rawToStreamMsgPtr(const std::string &raw, const uint32_t fileType, const uint32_t udpPackSize );
	//MsgPtr MSG_EXPORT rawToStreamMsgPtr(const std::string &raw, const MsgHd &hd1, const StreamMsgHd &hd2, const bool isResponseMsg);
	//bool MSG_EXPORT getStreamMsgHead(const MsgPtr &msg, MsgHd &hd1, StreamMsgHd &hd2);
}
#endif
