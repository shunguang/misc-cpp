#include "MsgCircularQ.h"

using namespace std;
using namespace app;

MsgCircularQ::MsgCircularQ( const uint32_t nTotItems, const std::string &name)
	: m_v()
	, m_q()
	, m_name(name)
{
	allocQ( nTotItems );
}

MsgCircularQ::~MsgCircularQ()
{
	freeQ();

}

void MsgCircularQ::resetSize(const uint32_t nTotItems)
{
	freeQ();
	allocQ(nTotItems);
}

void MsgCircularQ::reset()
{
	m_mutexRW.lock();

	m_headW = 0;
	m_headR = 0;
	m_v.resize(m_items, 0);

	m_mutexRW.unlock();
}


void MsgCircularQ::freeQ()
{
	boost::mutex::scoped_lock lock(m_mutexRW);
	
	m_q.clear();
	m_v.clear();
	m_headW = 0;
	m_headR = 0;
}

void MsgCircularQ::allocQ(const uint32_t nTotItems)
{
	boost::mutex::scoped_lock lock(m_mutexRW);

	m_items = nTotItems;
	m_q.clear();
	MsgPtr tmp = NULL;
	for (uint32_t i = 0; i < m_items; ++i) {
		m_q.push_back( tmp );
	}

	m_v.resize(m_items, 0);
	m_headW = 0;
	m_headR = 0;
}

//only source filter can add an new frm
bool MsgCircularQ::wrtNext(const MsgPtr &x)
{
	bool sucWrt = false;
	{
		boost::mutex::scoped_lock lock(m_mutexRW);
		uint32_t &idx = m_headW;
		int   &cnt = m_v[idx];
		if (cnt == 0) {
			m_q[idx] = x;	
			cnt = cnt + 1;

			//move head to the next slot
			++idx;
			if (idx >= m_items) {
				idx = 0;
			}
			sucWrt = true;
		}
	}

	if ( !sucWrt ) {
		dumpLog("MsgCircularQ::wrtNext(): writen is too fast, this frame droped, qName=%s", m_name.c_str() );
	}

	return sucWrt;
}

//the consumer read  <x> from que
bool MsgCircularQ::readNext( MsgPtr &x )
{
	bool hasData = false;
	{
		//this lock moved to readNext() for specific RawFrmXYZ Type
		boost::mutex::scoped_lock lock(m_mutexRW);
		uint32_t &idx = m_headR;
		int   &cnt = m_v[idx];
		if (cnt > 0) {
			x = m_q[idx];		//hd copy the poiter
			cnt = 0;
			hasData = true;
			//move head to the next slot
			++idx;
			if (idx >= m_items) {
				idx = 0;
			}
		}
	}
	return hasData;
}

bool MsgCircularQ::readNext(ErrorMsg &msg)
{
	MsgPtr p0;
	bool hasData = this->readNext(p0);
	if (hasData) {
		Msg *p1 = p0.get();
		ErrorMsg *p2 = dynamic_cast<ErrorMsg*>(p1);
		msg = *p2;
	}
	return hasData;
}

