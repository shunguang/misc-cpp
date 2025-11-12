#ifndef __MSG_ENUMS_H__
#define __MSG_ENUMS_H__

#include "libUtil/DataTypes.h"
#include "MsgDefs.h"

#define MSG_SYNC_WORD			0x57414D49     //--�WAMI�
#define MSG_UINT16_RESERVED		0x0000
#define MSG_UINT8_RESERVED		0x00

#define MSG_HEAD_TAIL_SIZE				16
#define MSG_HEAD_TAIL_SIZE_LFC_STREAM	36
#define MSG_SMALL_PACK_SIZE				256
#define MSG_FILE_STREAM_PACK_SIZE		1024  //for TCP 2408, 8192 for UDP 
#define MSG_ERR_MSG_LEN					128


#define MSG_ORI							0             //the original msg
#define MSG_ACK							1             //acknowledgement messege w/ same header and body  if need
#define MSG_LOCAL_TIMESTAMP_NOW		    (POSIX_LOCAL_TIME)
#define MSG_UNIVERSAL_TIMESTAMP_NOW		(boost::posix_time::second_clock::universal_time())

namespace app {
	//Msg types, donot change the order
	enum MsgType {
		MSG_T_STATUS=0,  //camera status
		MSG_T_ERROR=1,
		MSG_T_SETTINGS=2,
		MSG_T_CAM_CTRL=3,			//client request to seting camera controlling params		
		MSG_T_STAB=4,  			 	//client request to turn on/off stablization
		MSG_T_FILE_STREAM=5,		
		MSG_T_REQEST_NEW_SIZE=6,   	//client request content (Pyr Lel, ROI, etc) change
		MSG_T_LFC_STATUS=7,        	//client request content (Pyr Lel, ROI, etc) change
		MSG_T_UNKN=8,
		MSG_T_CUNT=9
	};
	static const std::vector<std::string> g_msgTypeLabel = {
		"MSG_T_STATUS",
		"MSG_T_ERROR",
		"MSG_T_SETTINGS", 
		"MSG_T_CAM_CTRL",
		"MSG_T_STAB",
		"MSG_T_FILE_STREAM",
		"MSG_T_REQEST_NEW_SIZE",
		"MSG_T_LFC_STATUS",
		"MSG_T_UNKN",
		"MSG_T_CUNT"};

	//file status in camera side
	enum FileStatuse {
		FILE_STAT_NORMAL = 0,
		FILE_STAT_CANNOT_BE_OPENED,
		FILE_STAT_EMPTY,
		FILE_STAT_SENDING_TIMEOUT,
		FILE_STAT_UNKN,
		FILE_STAT_CUNT
	};
	static const std::vector<std::string> g_fileStatusLabel = { "FILE_STAT_NORMAL", "FILE_STAT_CANNOT_BE_OPENED", "FILE_STAT_EMPTY", "FILE_STAT_SENDING_TIMEOUT", "FILE_STAT_UNKN" };


	enum FileType {
		FILE_T_H264,
		FILE_T_JPG,
		FILE_T_RAW,
		FILE_T_UNKN,
		FILE_T_CUNT
	};
	static const std::vector<std::string>g_fileTypeLabel = { "FILE_T_H264", "FILE_T_JPG", "FILE_T_RAW", "FILE_T_UNKN", "FILE_T_CUNT" };


	std::string MSG_EXPORT getMsgTypeLabel(const uint8_t t);
	std::string getFileStatuseLabel(const uint8_t t);
	std::string getFileTypeLabel(const uint8_t t);
}
#endif
