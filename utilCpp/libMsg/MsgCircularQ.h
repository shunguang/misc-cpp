//----------------------------------------------------------------------------------------
// A circular <T> que support multi-thread read/write activities
// Shunguang Wu 7/19/17
//----------------------------------------------------------------------------------------
#ifndef _MSG_CIRCULAR_Q_H_
#define _MSG_CIRCULAR_Q_H_

#include "Msg.h"
#include "ErrorMsg.h"
#include "MsgDefs.h"

namespace app {
	class MSG_EXPORT MsgCircularQ {
	public:
		MsgCircularQ(const uint32_t nTotItems, const std::string &name="Unknown");
		~MsgCircularQ();

		//reset size
		void		resetSize(const uint32_t nTotItems);

		//reset <m_headW> and <m_headR> as zeros
		void		reset();

		//HD copy <x> into q[m_headW]
		bool wrtNext(const MsgPtr &x);
		bool readNext(MsgPtr &x);

		//HD copy q[m_headR] into <msg>
		bool readNext(ErrorMsg &msg);           
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

		std::vector<MsgPtr>	m_q;
		std::vector<int>	m_v;        //count the wrt (++) / read(--) activities in m_q[i]


		boost::mutex m_mutexRW;
		std::string  m_name;                    //for debug purpose
	};

	typedef std::shared_ptr<MsgCircularQ>		MsgCircularQPtr;
}

#endif		