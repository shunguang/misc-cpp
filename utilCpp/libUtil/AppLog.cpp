/*
 *------------------------------------------------------------------------
 * abc.h
 *
 * No  any Corporation or client funds were used to develop this code.
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * Copyright (c) 2022 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

#include "AppLog.h"
using namespace std;
using namespace app;

//init static private varaiables
AppLog* AppLog::m_logPtr = NULL;
string	AppLog::m_logFilename = string("");
bool	AppLog::m_logShowMsgInConsole = false;

//private construct() and destruct()
AppLog::AppLog()
	: m_logCondition()
	, m_logSleepMutex()
	, m_goSleep(true)
	, m_logOtherMutex()
	, m_forceExit(false)
	, m_loopExited(false)
	, m_logThread(0)
{
	//m_begTime = std::time(0);
}

//-------public funcs -----------
void AppLog::logMsg(const std::string &msg)
{
	//std::string newMsg = getTimeStamp() + "->" + msg;
	std::string newMsg(msg);
	if (AppLog::m_logShowMsgInConsole) {
		cout << newMsg << endl;
	}

	{
		std::lock_guard<std::mutex> guard(m_logOtherMutex);
		m_msgQ.push_back( newMsg);
	}

	wakeUpToWork();
}

void AppLog::logMsg(const char* msg)
{
	std::string s(msg);
	logMsg(s);
}

std::string AppLog::getTimeStamp()
{
	char buf[64];
	struct tm now;
	std::time_t t = std::time(0);   // get time now

	//uint32_t dt = t - m_begTime;

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

	snprintf(buf, 64, "%02d/%02d-%02d:%02d:%02d", (now.tm_mon + 1), now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	return string(buf);
}

void AppLog::doDumpLoop()
{
	//m_goSleep = true;
	setForceExit(false);
	setLoopExited(false);
	//clear dump data in previous run
	ofstream outfile(AppLog::m_logFilename.c_str());
	if (outfile.is_open()) {
		outfile << "----start----" << endl;
		outfile.flush();
	}

	//ofstream outfile(AppLog::m_logFilename.c_str(), ios_base::app);
	std::string curMsg;
	while (1) {
		bool hasNewMsg = getNextMsg(curMsg);
		if (hasNewMsg){
			if (outfile.is_open()) {
				outfile << curMsg << endl;
				outfile.flush();
			}
		}
		else{ //no more msg to log
			std::unique_lock<std::mutex> lock(m_logSleepMutex);
			m_goSleep = true;
			while (m_goSleep) {
				m_logCondition.wait(lock);
			}
		}

		//make sure all messages are dumped before exit()
		if ( !hasNewMsg && isForceExit()) {
			break;
		}

	}
	outfile.close();
	setLoopExited(true);
}

void AppLog::startLog()
{
	m_logThread.reset(new std::thread(&AppLog::doDumpLoop, this));
}

void AppLog::endLog()
{
	uint32_t cnt=0;
	do {
		setForceExit(true);
		std::this_thread::sleep_for(10ms);
		wakeUpToWork();
		cnt++;
		if (cnt%100==0){
			printf("AppLog::endLog(): cnt=%d\n", cnt);
		}
	} while ( !isLoopExited() );
	printf("AppLog::endLog(): done!\n");
}

//----------------- global funcs ---------------------
void app::dumpLog( const std::string &x )
{
	AppLog::m_logPtr->logMsg(x);
	//app::dumpLog( x.c_str() );
}

void app::dumpLog(const char *fmt, ...)
{
	//todo: remove this LOG_MAX_MSG_LEN, using dynamic allocation idea
	char buffer[LOG_MAX_MSG_LEN];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, LOG_MAX_MSG_LEN, fmt, args);
	va_end(args);

	AppLog::m_logPtr->logMsg(buffer);
}

//-------------------------------
void app::startLogThread( const std::string &logFilePath, const bool showInConsole)
{
	std::string fDir, fName;
	splitFolder(logFilePath, fDir, fName);
	createDir(fDir);

	AppLog::createLogInstance(logFilePath);
	AppLog::m_logShowMsgInConsole = showInConsole;

	AppLog::m_logPtr->startLog();
}

void app::endLogThread()
{
	if (AppLog::m_logPtr == NULL) {
		return;
	}
	AppLog::m_logPtr->logMsg("-------Last log Msg : log thread prepare to exit -----");
	AppLog::m_logPtr->endLog();

	//delete AppLog::m_logPtr
	//AppLog::m_logPtr = NULL;
}

void  app::appExit(const int flag)
{
	if (flag != 0) {
		dumpLog("abnormal exit()!");
	}

	endLogThread();
	exit(1);
}

void  app::appExit(const char * x, ...)
{
	dumpLog(x);
	endLogThread();
	exit(1);
}

void  app::appExit(const  std::string &x, ...)
{
	dumpLog(x);
	endLogThread();
	exit(1);
}

void app::appAssert( const bool flag, const std::string &msg)
{
	if (!flag) {
		dumpLog(msg);
		endLogThread();
		exit(1);
	}
}
void  app::appAssert(const bool flag, const char *file, const int lineNum)
{
	if (!flag) {
		dumpLog( "Assert fail in File [%s], line[%d]", file, lineNum );
		endLogThread();
		exit(1);
	}
}

