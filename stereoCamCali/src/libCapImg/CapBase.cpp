
#include "CapBase.h"

using namespace std;
using namespace cali;

CapBase::CapBase(const CfgCapStereoIR &params)
	: m_frmQ( NULL )
	, m_params(params)
	, m_forceExit(false)
	, m_pauseCap(true)
	, m_readLoopExit(true)
	, m_capThread(0)
{
}

CapBase::~CapBase() {
}

void CapBase::setOutputQue( RawFrmQPtr &q )
{
	m_frmQ = q;
}

void    CapBase::setPause(const bool f)
{
	boost::mutex::scoped_lock lock(m_mutex4Working);
	m_pauseCap = f;
}


void  CapBase::setForceExit(const bool f)
{
	boost::mutex::scoped_lock lock(m_mutex4Working);
	m_forceExit = f;
}

bool    CapBase::isPause()
{
	bool f;
	{
		boost::mutex::scoped_lock lock(m_mutex4Working);
		f = m_pauseCap;
	}
	return f;
}

bool    CapBase::isForceExit()
{
	bool f;
	{
		boost::mutex::scoped_lock lock(m_mutex4Working);
		f = m_forceExit;
	}
	return f;
}

void CapBase::wakeUpFromPause()
{
	boost::mutex::scoped_lock lock(m_mutex4Working);
	m_condition4Working.notify_one();
	m_pauseCap = false;
}
