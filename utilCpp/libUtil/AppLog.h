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

#ifndef __APP_LOG_H__
#define __APP_LOG_H__

#include "AppDefs.h"
#include "FileUtil.h"

#define LOG_USING_LOCAL_TIME  1
#define	LOG_MAX_MSG_LEN		1024

namespace app {
	//--------- log UI funcs------------
	void   startLogThread(const std::string &logFilename, const bool showInConsole = true);
	void   endLogThread();

	void   dumpLog(const char * x, ...);
	void   dumpLog(const  std::string &x );

	void   appExit(const int flag);
	void   appExit(const char * x, ...);
	void   appExit(const  std::string &x, ...);
	void   appAssert(const bool flag, const std::string &msg);
	void   appAssert(const bool flag, const char *file, const int lineNum);

	//----------- def of AppLog -------------------
	class  AppLog {
	private:
		AppLog();

	public:
		AppLog(const AppLog &x) = delete;            //donot implement
		void operator=(const AppLog &x) = delete;    //donot implement
		
		static void createLogInstance(const std::string &logFilePath) {
			if( m_logPtr == NULL ){
				m_logPtr = new AppLog();
				AppLog::m_logFilename = logFilePath;
			}
		}

		void logMsg(const std::string &msg);
		void logMsg(const char* msg);
		void startLog();
		void endLog();

	private:
		void doDumpLoop();
		std::string getTimeStamp();
		bool getNextMsg(std::string &curMsg) {
			bool f = false;
			{
				std::lock_guard<std::mutex> guard(m_logOtherMutex);
				if (!m_msgQ.empty()) {
					curMsg = m_msgQ.front(); //hard copy
					m_msgQ.pop_front();
					f = true;
				}
			}
			return f;
		}

		inline void wakeUpToWork() {
			std::lock_guard<std::mutex> guard(m_logSleepMutex);
			m_goSleep = false;
			m_logCondition.notify_one();
		}

		inline void setForceExit( bool f) {
			std::lock_guard<std::mutex> guard(m_logOtherMutex);
			m_forceExit = f;
		}

		inline bool isForceExit() {
			bool f;
			{
				std::lock_guard<std::mutex> guard(m_logOtherMutex);
				f = m_forceExit;
			}
			return f;
		}

		inline void setLoopExited(bool f) {
			std::lock_guard<std::mutex> guard(m_logOtherMutex);
			m_loopExited = f;
		}

		bool isLoopExited() {
			bool f;
			{
				std::lock_guard<std::mutex> guard(m_logOtherMutex);
				f = m_loopExited;
			}
			return f;
		}

	private:
		// --- m_goSleep guided by m_logSleepMutex ---
		std::condition_variable	m_logCondition;
		std::mutex				m_logSleepMutex;
		bool						m_goSleep;
		
		// --- other variables guided by m_logOtherMutex ---
		std::mutex				m_logOtherMutex;
		bool m_forceExit;
		bool m_loopExited;
		std::deque<std::string> m_msgQ;
		
		//--------------
		//std::time_t 					 m_begTime;
		std::shared_ptr<std::thread>	 m_logThread;

	public:
		//todo: This isn't thread safe. Better to make <m_logPtr> a local static of createLogInstance()
		//and initialize it immediately without a test
		static AppLog* 	   m_logPtr;
		static std::string m_logFilename;
		static bool        m_logShowMsgInConsole;
	};

}
#endif

