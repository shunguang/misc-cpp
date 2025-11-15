#ifndef _CAP_BASE_H_
#define _CAP_BASE_H_

#include "libUtil/util.h"
#include "RawFrmBase.h"
#include "RawFrmIrStereoImg.h"
#include "RawFrmQ.h"
#include "CapDefs.h"

namespace cali {

	class CapBase {

	public:
		CapBase(const CfgCapStereoIR& params);
		~CapBase();

		virtual bool	startCaptureThread() = 0;
		virtual bool	endCaptureThread() = 0;

		void	setOutputQue(RawFrmQPtr& q);

		void  setPause(const bool f);
		void  setForceExit(const bool f);
		bool  isPause();
		bool  isForceExit();
		void  wakeUpFromPause();

		bool getNextFrm(RawFrmIrStereoImg& x) {
			return m_frmQ->readNext(x);
		}

	public:
		CfgCapStereoIR   m_params;

	protected:
		RawFrmQPtr  m_frmQ;            //the captured frm images are stored in this queue waiting for futher processing

		bool  m_forceExit;
		bool	m_pauseCap;
		bool	m_readLoopExit;

		std::shared_ptr<boost::thread>	m_capThread;
		boost::mutex					m_mutex4Working;
		boost::condition_variable		m_condition4Working;
	};
	typedef std::shared_ptr<CapBase>		CapBasePtr;
}
#endif
