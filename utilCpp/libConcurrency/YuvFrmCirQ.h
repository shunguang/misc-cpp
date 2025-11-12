//----------------------------------------------------------------------------------------
// A circular <T> que support multi-thread read/write activities
// Shunguang Wu 3/13/19
//----------------------------------------------------------------------------------------
#ifndef _YUV_CIRCULAR_Q_H_
#define _YUV_CIRCULAR_Q_H_

#include "YuvFrm.h"
namespace app {
	class  YuvFrmCirQ {
	public:
		YuvFrmCirQ(const uint32_t imgW=64, const uint32_t imgH=32, const uint32_t nTotItems=10);
		~YuvFrmCirQ();

		//reset size
		void resetSize(const uint32_t imgW, const uint32_t imgH, const uint32_t nTotItems=0);

		//reset <m_headW> and <m_headR> as zeros
		void reset();

		//HD copy <x> into q[m_headW]
		bool wrtNext(const YuvFrmPtr &src);             //host interface
		bool readNext(YuvFrmPtr &dst);
	protected:
		void allocQ(const uint32_t nTotItems);
		void freeQ();

	private:
		//-----------------------------------------------------------------------------
		//access queue
		//-----------------------------------------------------------------------------
		uint32_t		m_items;	//the predefined # of elements of the queue.
		uint32_t		m_headW;	//the index to write	
		uint32_t		m_headR;	//the index to write	

		std::vector<YuvFrmPtr>	m_q;
		std::vector<int>	m_v;        //count the wrt (++) / read(--) activities in m_q[i]


		boost::mutex m_mutexRW;
		uint32_t m_w;
		uint32_t m_h;
	};
	typedef std::shared_ptr<YuvFrmCirQ> YuvFrmCirQPtr;
}

#endif		