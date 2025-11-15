
//----------------------------------------------------------------------------------------
// A circular <T> que support multi-thread read/write activities
// Shunguang Wu 7/19/17
//----------------------------------------------------------------------------------------
#ifndef _RAW_FRM_Q_H_
#define _RAW_FRM_Q_H_

#include "RawFrmIrStereoImg.h"
//#include "RawFrmIrStereoOutput.h"
//#include "RawFrm2Client.h"
//#include "RawFrmTemplate4SensorX.h"

#include "CapDefs.h"

namespace cali {
	class CAP_EXPORT RawFrmQ {
	public:
		RawFrmQ(const uint32_t nTotItems, const std::string &name="Unknown");
		~RawFrmQ();

		//reset size
		void		resetSize(const uint32_t nTotItems);

		//reset <m_headW> and <m_headR> as zeros
		void		reset();

		//HD copy <x> into q[m_headW]
		bool wrtNext(const RawFrmBasePtr &x);

		//HD copy q[m_headR] into <frm>
		bool readNext(RawFrmIrStereoImg &frm);           
		//bool readNext(RawFrmIrStereoOutput &frm);
		//bool readNext(RawFrm2Client &frm);

		//bool readNext(RawFrmTemplate4SensorX &frm);
	protected:
		bool readNext(RawFrmBasePtr &x);
		void allocQ(const uint32_t nTotItems);
		void freeQ();

	private:
		//-----------------------------------------------------------------------------
		//access queue
		//-----------------------------------------------------------------------------
		uint32_t		m_items;	//the predefined # of elements of the queue.
		uint32_t		m_headW;	//the index to write	
		uint32_t		m_headR;	//the index to write	

		std::vector<RawFrmBasePtr>	m_q;
		std::vector<int>			m_v;        //count the wrt (++) / read(--) activities in m_q[i]


		boost::mutex m_mutexRW;
		std::string  m_name;                    //for debug purpose
	};

	typedef std::shared_ptr<RawFrmQ>		RawFrmQPtr;
}

#endif		