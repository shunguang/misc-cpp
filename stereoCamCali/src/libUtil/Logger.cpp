#include "Logger.h"
using namespace std;
using namespace cali;

//init static private varaiables
cali::Logger* Logger::m_logger = NULL;
std::string		Logger::m_logFilename = string("");
bool			Logger::m_logShowInConsole = false;
bool			Logger::m_logIsDump = true;

//------- static funcs -----------
void Logger::SetLoggerFilename(const std::string &logFilename)
{
	//for debug purpose, alown to change log file name during the process
	if ( Logger::m_logger == NULL ){
		Logger::m_logFilename = logFilename;
		Logger::m_logger = new Logger();
	}
	else {
		printf( "Logger::SetLoggerFilename(): Error-can only be called once!");
		assert(0);
	}
}

Logger* Logger::getInstance()
{
	return cali::Logger::m_logger;
}

void Logger :: SetLoggerShowInConsoleFlag( const bool flag )
{
	Logger::m_logShowInConsole = flag;
}

void Logger::SetLoggerIsDumpFlag(const bool flag)
{
	Logger::m_logIsDump = flag;
}

//private construct() and destruct()
Logger::Logger()
	: m_logMutex()
	, m_goSleep (true)
	, m_forceExit(false)
	, m_loopExited(false)
	, m_logThread(0)
{
	m_begTime = std::time(0);
}

Logger::~Logger(){
}


//-------public funcs -----------
void Logger::log( const std::string &msg) 
{
	std::string t = getTime();
	logLock();
	m_msgQ.push_back( t + "->" + msg);
	logUnlock();

	if (isInSleep()) {
		wakeUpToWork();
	}
}

void Logger::log(const char* msg) 
{
	std::string s(msg);
	log(s);
}

std::string Logger :: getTime()
{
	char buf[64];
	struct tm now;
	std::time_t t = std::time(0);   // get time now
	uint32_t dt = t - m_begTime;

#if _WINDOWS
#if LOG_USING_LOCAL_TIME
	localtime_s(&now, &t);
#else
	gmtime_s(&now, &t);
#endif
#else
#if LOG_USING_LOCAL_TIME
	localtime_r(&t, &now);
#else
	gmtime_r(&t, &now);
#endif
#endif

	snprintf(buf, 64, "%02d/%02d-%02d:%02d:%02d (%ld)", (now.tm_mon + 1 ), now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, dt );
	return string(buf);
}


void Logger::copyMsgs( std::vector<std::string> &vMsg )
{
	logLock();
	vMsg = m_msgQ;
	m_msgQ.clear();
	logUnlock();
}

void Logger::doDumpLoop()
{
	m_goSleep = true;
	m_forceExit = false;
	m_loopExited = false;
	//clear dump data in previous run
	{
		ofstream outfile(Logger::m_logFilename.c_str());
		if (outfile.is_open()) {
			outfile << "----start----"<< endl;
		}
		outfile.close();
	}
	while (1) {
		//boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;
		bool pauseJob = isInSleep();
		if (pauseJob) {
			boost::mutex::scoped_lock lock(m_logMutex);
			m_logCondition.wait(lock);
		}

		//-------------do the job ---------------
		vector<std::string> vMsg;
		copyMsgs(vMsg);
		ofstream outfile(Logger::m_logFilename.c_str(), ios_base::app);
		if( outfile.is_open() ) {
			for (auto it = vMsg.begin(); it != vMsg.end(); ++it) {
				outfile << *it << endl;
				if (Logger::m_logShowInConsole) {
					cout << endl << *it << endl;
				}
			}
			outfile.flush();
			outfile.close();
		}

		if (isForceExit()) {
			break;
		}
		
		uint32_t n = getNewMsgCount();
		if (n == 0) {
			gotoSleep();
		}
	}
	m_loopExited = true;
}

void Logger::startThread()
{
	m_logThread.reset(new boost::thread(boost::bind(&Logger::doDumpLoop, this)));
}


//----------------- global funcs ---------------------
void cali::dumpLog( const std::string &x, ... )
{
	cali::dumpLog( x.c_str() );
}

void cali::dumpLog(const char *fmt, ...)
{
	//todo: remove this LOG_MAX_MSG_LEN, using dynamic allocation idea
	char buffer[LOG_MAX_MSG_LEN];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, LOG_MAX_MSG_LEN, fmt, args);
	va_end(args);

	Logger::getInstance()->log(buffer);
}

//-------------------------------
void cali::startLogThread( const std::string &logFilename, const bool showInConsole, const bool dumpLog)
{
	cali::Logger::SetLoggerShowInConsoleFlag(showInConsole);
	cali::Logger::SetLoggerIsDumpFlag(dumpLog);
	cali::Logger::SetLoggerFilename(logFilename);

	Logger::getInstance()->startThread();
}

void  cali::endLogThread()
{
	Logger* p = Logger::getInstance();
	
	p->log("-------Last log Msg : log thread exit -----");

	uint32_t n = p->getNewMsgCount();
	if (n > 0) {
		p->wakeUpToWork();
	}
	//waiting for it is in sleep (no msgs need to be downloaded)
	while ( !p->isInSleep() ) {
		APP_SLEEP(100);
	}

	p->wakeUpToWork();
	p->forceDumpLoopExit();

	//any masg in the q will be dumped during this period of time
	while (1) {
		APP_SLEEP(50);
		if (p->isDumpLoopExited()) {
			break;
		}
	}
}


void  cali::myExit(const int flag)
{
	if (flag != 0) {
		dumpLog("abnormal exit()!");
	}

	endLogThread();
	exit(1);
}

void  cali::myExit(const char * x, ...)
{
	dumpLog(x);
	endLogThread();
	exit(1);
}

void  myExit(const  std::string &x, ...)
{
	dumpLog(x);
	endLogThread();
	exit(1);
}


