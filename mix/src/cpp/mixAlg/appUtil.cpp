
#include "appUtil.h"
using namespace std;
void mix::appLog(const std::string& msg)
{
	//todo: using cpp4log kind of 3rd party log tools
	std::cout << msg << std::endl;
}

void mix::appAssert(const bool flag, const std::string& errMsg)
{
	if (!flag) {
		mix::appLog(errMsg);
	}
}
