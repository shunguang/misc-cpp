#include "MsgEnums.h"

using namespace std;
using namespace app;

std::string app::getMsgTypeLabel(const uint8_t t)
{
	if (t >= 0 && t < MSG_T_CUNT){
		return string(g_msgTypeLabel[t]);
	}
	else{
		return string("Err: Msg Type undefined!");
	}
}

std::string app::getFileStatuseLabel(const uint8_t t)
{
	if (t >= 0 && t < FILE_STAT_CUNT) {
		return string(g_fileStatusLabel[t]);
	}
	else {
		return string("Err: File Statusee undefined!");
	}
}


std::string app::getFileTypeLabel(const uint8_t t)
{
	if (t >= 0 && t < FILE_T_CUNT) {
		return string(g_fileTypeLabel[t]);
	}
	else {
		return string("Err: File Type undefined!");
	}
}


