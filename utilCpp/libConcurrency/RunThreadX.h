#ifndef __RUN_THREAD_X_H__
#define __RUN_THREAD_X_H__

#include "ThreadDefsS.h"
#include "libUtil/util.h"
#include "libCfg/CfgServer.h"
#include "libDc/DcServer.h"
#include "libDc/FrmTs.h"

namespace app {
	class THREAD_S_EXPORT RunThreadX {
	public:
		RunThreadX( const int threadId=0, const std::string &name="unkn" );
		virtual ~RunThreadX();

		virtual void startThread();    //start thread
		virtual void forceThreadQuit();        //force thread quit from its inifinity loop

		void wakeUpToWork();     //wake up thread to  procNextTask()
		void goToSleep();        //ask thread go to sleep
		bool isExitedRunLoop();
		bool isInSleepMode();

		void setCfg(CfgServerPtr  &cfg);
		void setDC(DcServerPtr &dc);
		int setFrmFreq2Log(const int id);


	protected:
		void setExitedLoopFlag(const bool f);
		void setRcvdExitLoopCmdFlag(const bool f);
		bool isRcvdExitLoopCmd();

		virtual void procNextTask() = 0;
		virtual bool procInit() = 0;

		void runLoop();

	public:
		int 			m_threadId;		//for debugging purpose
		std::string		m_threadName;	//for debugging purpose

	protected:
		CfgServerPtr	m_cfg;			//cfg ptr
		DcServerPtr		m_dc;			//shared data pointer

		int             m_frmFreqToLog;
		uint64_t		m_frmNum;
		FrmTs			m_fts;
		
		bool    		m_isExitedLoop;
		bool			m_isInSleepMode;
		bool    		m_rcvdExitLoopCmd;

		std::shared_ptr<boost::thread>	 m_threadX;
		boost::mutex					 m_mutex4Working;
		boost::condition_variable		 m_condition4Working;
		boost::mutex					 m_mutexLocal;
	};
	typedef std::shared_ptr<RunThreadX>		RunThreadXPtr;
}
#endif
