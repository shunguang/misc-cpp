#include "RunThreadX.h"
using namespace std;
using namespace app;

RunThreadX::RunThreadX(const int threadId, const std::string &name)
	: m_threadId(threadId)
	, m_threadName(name)
	, m_cfg(0)
	, m_dc(0)
	, m_frmFreqToLog(100)
	, m_frmNum(0)
	, m_fts(0)
	, m_isExitedLoop(true)
	, m_isInSleepMode(true)
	, m_rcvdExitLoopCmd(false)
	, m_threadX(0)
	, m_mutex4Working()
	, m_mutexLocal()
	, m_condition4Working()
{
	//cout << "RunThreadX::RunThreadX(): called" << endl;
}


RunThreadX::~RunThreadX()
{

	if (m_threadX) {
		m_threadX->join();
	}
}

void RunThreadX::setCfg(CfgServerPtr &cfg)
{
	//cout << "RunThreadX::setCfg(): AAA" << endl;
	m_cfg = cfg;
	//cout << "RunThreadX::setCfg(): called" << endl;
}

int RunThreadX::setFrmFreq2Log(int idx)
{
	CfgLog cfg = m_cfg->getLog();
	if (idx >= 0 && idx < 5) {
		m_frmFreqToLog = cfg.vFrmFreqToLog[idx];
	}
	else {
		m_frmFreqToLog = 1000;
	}
	return m_frmFreqToLog;
}


void RunThreadX::setDC(DcServerPtr &dc)
{
	m_dc = dc;
	//cout << "RunThreadX::setDC(): called" << endl;
}

void RunThreadX::startThread()
{
	procInit();
	m_threadX.reset(new boost::thread(boost::bind(&RunThreadX::runLoop, this)));
}

void RunThreadX::runLoop()
{
	setExitedLoopFlag(false);
	bool quitLoop = false;
	while (!quitLoop) {
		{
			boost::mutex::scoped_lock lock(m_mutex4Working);
			while (m_isInSleepMode) {
				//dumpLog("RunThreadX::runLoop(): in sleep mode--%s", m_threadName.c_str());
				m_condition4Working.wait(lock);
			}
			//dumpLog("RunThreadX::runLoop(): waked up--%s", m_threadName.c_str());
		}

		quitLoop = isRcvdExitLoopCmd();
		if (quitLoop) {
			break;
		}

		procNextTask();

		quitLoop = isRcvdExitLoopCmd();
	}
	dumpLog("RunThreadX::run(): exit loop--%s", m_threadName.c_str());
	setExitedLoopFlag(true);
}

void RunThreadX::forceThreadQuit()
{
	setRcvdExitLoopCmdFlag(true);
	if ( isInSleepMode() ) {
		wakeUpToWork();
	}

	int cnt = 0;
	while (!isExitedRunLoop()) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		++cnt;
		if (cnt > 500) {
			dumpLog(" stuck at: forceThreadQuit() at thread %s", m_threadName.c_str());
			cnt = 0;
		}
	}

}

void RunThreadX::setExitedLoopFlag( const bool f)
{
	boost::mutex::scoped_lock lock(m_mutexLocal);
	m_isExitedLoop = f;
}

bool RunThreadX::isExitedRunLoop()
{
	bool f;
	{
		boost::mutex::scoped_lock lock(m_mutexLocal);
		f = m_isExitedLoop;
	}
	return f;
}

bool RunThreadX::isRcvdExitLoopCmd()
{
	bool f;
	{
		boost::mutex::scoped_lock lock(m_mutexLocal);
		f = m_rcvdExitLoopCmd;
	}
	return f;
}

void RunThreadX::setRcvdExitLoopCmdFlag(const bool f)
{
	boost::mutex::scoped_lock lock(m_mutexLocal);
	m_rcvdExitLoopCmd = true;
}

bool RunThreadX::isInSleepMode()
{
	bool f;
	{
		boost::mutex::scoped_lock lock(m_mutex4Working);
		f = m_isInSleepMode;
	}
	return f;
}


void RunThreadX::wakeUpToWork()
{
	boost::mutex::scoped_lock lock(m_mutex4Working);
	m_isInSleepMode = false;
	m_condition4Working.notify_one();
}

void RunThreadX::goToSleep()
{
	boost::mutex::scoped_lock lock(m_mutex4Working);
	m_isInSleepMode = true;
}

