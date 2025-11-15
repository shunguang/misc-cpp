#include "RawFrmQ.h"

using namespace std;
using namespace cali;

RawFrmQ::RawFrmQ( const uint32_t nTotItems, const std::string &name)
	: m_v()
	, m_q()
	, m_name(name)
{
	allocQ( nTotItems );
}

RawFrmQ::~RawFrmQ()
{
	freeQ();

}

void RawFrmQ::resetSize(const uint32_t nTotItems)
{
	freeQ();
	allocQ(nTotItems);
}

void RawFrmQ::reset()
{
	m_mutexRW.lock();

	m_headW = 0;
	m_headR = 0;
	m_v.resize(m_items, 0);

	m_mutexRW.unlock();
}


void RawFrmQ::freeQ()
{
	boost::mutex::scoped_lock lock(m_mutexRW);
	
	m_q.clear();
	m_v.clear();
	m_headW = 0;
	m_headR = 0;
}

void RawFrmQ::allocQ(const uint32_t nTotItems)
{
	boost::mutex::scoped_lock lock(m_mutexRW);

	m_items = nTotItems;
	m_q.clear();
	RawFrmBasePtr tmp = NULL;
	for (uint32_t i = 0; i < m_items; ++i) {
		m_q.push_back( tmp );
	}

	m_v.resize(m_items, 0);
	m_headW = 0;
	m_headR = 0;
}

//only source filter can add an new frm
bool RawFrmQ::wrtNext(const RawFrmBasePtr &x)
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
		dumpLog("RawFrmQ::wrtNext(): writen is too fast, this frame droped, qName=%s", m_name.c_str() );
	}

	return sucWrt;
}

//the consumer read  <x> from que
bool RawFrmQ::readNext( RawFrmBasePtr &x )
{
	bool hasData = false;
	{
		//this lock moved to readNext() for specific RawFrmXYZ Type
		//boost::mutex::scoped_lock lock(m_mutexRW);

		uint32_t &idx = m_headR;
		int   &cnt = m_v[idx];
		if (cnt > 0) {
			x = m_q[idx];		//hd copy the poiter
			//cnt = cnt - 1;
			cnt = 0;
			hasData = true;
			//move head to the next slot
			++idx;
			if (idx >= m_items) {
				idx = 0;
			}
		}
	}

#if 0
	if ( !hasData ){
		dumpLog("RawFrmQ::readNext(): read is too fast, return nothing!");
	}
#endif

	return hasData;
}

bool RawFrmQ::readNext(RawFrmIrStereoImg &frm)
{
	bool hasData = false;
	{
		boost::mutex::scoped_lock lock(m_mutexRW);
		RawFrmBasePtr p0;
		hasData = this->readNext(p0);
		if (hasData) {
			RawFrmBase *p1 = p0.get();
			RawFrmIrStereoImg *p2 = dynamic_cast<RawFrmIrStereoImg*>(p1);
			frm = *p2;
		}
	}
	return hasData;
}

#if 0
bool RawFrmQ::readNext(RawFrmIrStereoOutput &frm)
{
	bool hasData = false;
	{
		boost::mutex::scoped_lock lock(m_mutexRW);
		RawFrmBasePtr p0;
		hasData = this->readNext(p0);
		if (hasData) {
			RawFrmBase *p1 = p0.get();
			RawFrmIrStereoOutput *p2 = dynamic_cast<RawFrmIrStereoOutput*>(p1);
			frm = *p2;
		}
	}
	return hasData;
}

bool RawFrmQ::readNext(RawFrm2Client &frm)
{
	bool hasData = false;
	{
		boost::mutex::scoped_lock lock(m_mutexRW);
		RawFrmBasePtr p0;
		hasData = this->readNext(p0);
		if (hasData) {
			RawFrmBase *p1 = p0.get();
			RawFrm2Client *p2 = dynamic_cast<RawFrm2Client*>(p1);
			frm = *p2;
		}
	}
	return hasData;
}

bool RawFrmQ::readNext(RawFrmTemplate4SensorX &frm)
{
	bool hasData = false;
	{
		boost::mutex::scoped_lock lock(m_mutexRW);
		RawFrmBasePtr p0;
		hasData = this->readNext(p0);
		if (hasData) {
			RawFrmBase *p1 = p0.get();
			RawFrmTemplate4SensorX *p2 = dynamic_cast<RawFrmTemplate4SensorX*>(p1);
			frm = *p2;
		}
	}
	return hasData;
}
#endif
