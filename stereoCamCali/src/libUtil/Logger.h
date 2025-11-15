#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "DataTypes.h"
#include <stdarg.h>
#include <fstream>
#include <time.h>

#define LOG_USING_LOCAL_TIME  1
#define LOG_USE_BOOST_MUTEX   1		//0 - use linux POSIX mutex
#define	LOG_MAX_MSG_LEN		2048

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include "UtilDefs.h"

namespace cali{
	class UTIL_EXPORT Logger {

	public:
		static void SetLoggerShowInConsoleFlag(const bool flag);
		static void SetLoggerIsDumpFlag(const bool flag);
		static void SetLoggerFilename(const std::string &logFilename);
		static Logger* getInstance();

		void log(const std::string &msg);
		void log(const char* msg);

		void startThread();
		void doDumpLoop();

		inline bool isDumpLoopExited() {
			bool f;
			logLock();
			f = m_loopExited;
			logUnlock();
			return f;
		}

		inline void forceDumpLoopExit() {
			logLock();
			m_forceExit = true;
			logUnlock();
		}

		inline bool isInSleep() {
			bool f;
			logLock();
			f = m_goSleep;
			logUnlock();
			return f;
		}

		inline void wakeUpToWork() {
			logLock();
			m_logCondition.notify_one();
			m_goSleep = false;
			logUnlock();
		}

		uint32_t getNewMsgCount() {
			uint32_t n = 0;
			logLock();
			n = m_msgQ.size();
			logUnlock();
			return n;
		}

	private:
		Logger();
		~Logger();

		void copyMsgs(std::vector<std::string> &vMsg);
		std::string getTime();

		inline void logLock() {
#if LOG_USE_BOOST_MUTEX
			m_logMutex.lock();
#else
			pthread_mutex_lock(&m_logMutex);
#endif
		}

		inline void logUnlock() {
#if LOG_USE_BOOST_MUTEX
			m_logMutex.unlock();
#else
			pthread_mutex_unlock(&m_logMutex);
#endif
		}


		inline void gotoSleep() {
			logLock();
			m_goSleep = true;
			logUnlock();
		}

		inline bool isForceExit() {
			bool f;
			logLock();
			f = m_forceExit;
			logUnlock();
			return f;
		}

	private:
		std::vector<std::string> m_msgQ;

		bool m_goSleep;
		bool m_forceExit;
		bool m_loopExited;
		boost::condition_variable	m_logCondition;
		boost::mutex				m_logMutex;
		std::time_t 				m_begTime;
		std::shared_ptr<boost::thread>	 m_logThread;

	private:
		static Logger      *m_logger;
		static std::string m_logFilename;
		static bool        m_logShowInConsole;
		static bool        m_logIsDump;

	public:
	};

void  UTIL_EXPORT startLogThread(const std::string &logFilename, const bool showInConsole=true, const bool dumpLog=true );
void  UTIL_EXPORT endLogThread();

void  UTIL_EXPORT dumpLog(const char * x, ...);
void  UTIL_EXPORT dumpLog(const  std::string &x, ...);

void  UTIL_EXPORT myExit(const int flag);
void  UTIL_EXPORT myExit(const char * x, ...);
void  UTIL_EXPORT myExit(const  std::string &x, ...);
}
#endif

