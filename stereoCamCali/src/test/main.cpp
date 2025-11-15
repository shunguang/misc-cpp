/*
 * test1.cpp
 *
 *  Created on: May 15, 2015
 *      Author: wus1
 */
#include "libutil/DataTypes.h"
#include "libutil/Logger.h"
#include "libutil/UtilFuncs.h"

#define RUN_LOCAL_TEST 1

int test_capture(int argc, char **argv);
int test_calib_ir(int argc, char* argv[]);
int main(int argc, char* argv[])
{
	int x = 0;
	//developing test approaches
#if _WINDOWS
	const std::string logFilename = "c:/temp/log_calib.txt";
#else
	const std::string logFilename = "./log-" + cali::getPrettyTimeStamp() + ".txt";
#endif
	std::cout << logFilename << std::endl;
	cali::startLogThread(logFilename, true, true);
	cali::dumpLog("---start---");
	
	//x = test_capture(argc, argv);
	x = test_calib_ir(argc, argv);

	cali::dumpLog("---endlog---");
	cali::endLogThread();
	return x;
}




