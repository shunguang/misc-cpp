#include "libUtil/AppDefs.h"
#include "libUtil/UtilFuncs.h"

//integration test goes here
int main(int argc, char* argv[])
{
	//const std::string logFilename = "./log-" + pnt::getPrettyTimeStamp().substr(0, 20) + ".txt";
	//app::startLogThread(logFilename, true);

	test_mat(argc, argv);

	//app::endLogThread();
	return 0;
}




