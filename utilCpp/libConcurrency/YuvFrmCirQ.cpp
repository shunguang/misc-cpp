#include "YuvFrmCirQ.h"

using namespace std;
using namespace app;

YuvFrmCirQ::YuvFrmCirQ(const uint32_t imgW, const uint32_t imgH, const uint32_t nTotItems)
	: m_v()
	, m_q()
	, m_w(imgW)
	, m_h(imgH)
{
	allocQ( nTotItems );
	cout << "YuvFrmCirQ::YuvFrmCirQ(): done!" << endl;
}

YuvFrmCirQ::~YuvFrmCirQ()
{
	freeQ();
}

void YuvFrmCirQ::resetSize(const uint32_t imgW, const uint32_t imgH, const uint32_t nTotItems)
{
	int n = m_items;
	m_w = imgW;
	m_h = imgH;
	freeQ();

	if (nTotItems > 0) {
		n = nTotItems;
	}
	allocQ(n);

	//cout << "YuvFrmCirQ::resetSize(): Q allocted!" << endl;
}

void YuvFrmCirQ::reset()
{
	m_mutexRW.lock();

	m_headW = 0;
	m_headR = 0;
	m_v.resize(m_items, 0);

	m_mutexRW.unlock();
}


void YuvFrmCirQ::freeQ()
{
	boost::mutex::scoped_lock lock(m_mutexRW);
	
	m_q.clear();
	m_v.clear();
	m_headW = 0;
	m_headR = 0;
}

void YuvFrmCirQ::allocQ(const uint32_t nTotItems)
{
	boost::mutex::scoped_lock lock(m_mutexRW);

	m_items = nTotItems;
	dumpLog("YuvFrmCirQ::allocQ m_w=%d, m_h=%d, m_items=%d", m_w, m_h, m_items);
	m_q.clear();
	for (uint32_t i = 0; i < m_items; ++i) {
		//dumpLog("i=%d, push_back caled!", i);
		YuvFrmPtr tmp( new YuvFrm(m_w, m_h, 0) );
		tmp->fn_ = i;
		m_q.push_back( tmp );
	}

	m_v.resize(m_items, 0);
	m_headW = 0;
	m_headR = 0;
	cout << "YuvFrmCirQ::allocQ done!" << endl;
}

//wrt from host
bool YuvFrmCirQ::wrtNext(const YuvFrmPtr &src)
{
	static int wrtDropCnt = 0;
	bool sucWrt = false;
	{
		boost::mutex::scoped_lock lock(m_mutexRW);
		uint32_t &idx = m_headW;
		int   &cnt = m_v[idx];
		if (cnt == 0) {
			m_q[idx]->copyFrom( src->buf_, src->sz_, src->fn_);	  //hard copy
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
		++wrtDropCnt;
		if (wrtDropCnt > 999) {
			dumpLog("YuvFrmCirQ::wrtNext(): writen is too fast, %d frames droped", wrtDropCnt);
			wrtDropCnt = 0;
		}
	}
	return sucWrt;
}

//the consumer read  <x> from que
bool YuvFrmCirQ::readNext(YuvFrmPtr &dst)
{
	bool hasData = false;
	{
		//this lock moved to readNext() for specific RawFrmXYZ Type
		boost::mutex::scoped_lock lock(m_mutexRW);
		uint32_t &idx = m_headR;
		int   &cnt = m_v[idx];
		if (cnt > 0) {
			YuvFrmPtr &src = m_q[idx];
			if (dst->sz_ != src->sz_) {
				dst->resetSz(src->w_, src->h_);
			}
			dst->copyFrom( src->buf_, src->sz_, src->fn_ ); //only copy image data

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
