//this thread capture Bayer images from camera and store them into a circluar que
#ifndef __RUN_CAP_BASE_H__
#define __RUN_CAP_BASE_H__
#include "libUtil/DataTypes.h"
#include "libUtil/util.h"
#include "RunThreadX.h"
#include "ClientMgr.h"

namespace app {
	class THREAD_S_EXPORT RunCapBase : public RunThreadX
	{
	public:
		RunCapBase( const CameraType_t camType, const int threadId );
		virtual ~RunCapBase();

		void setWamiThread(RunThreadX*	x) {
			m_wamiThread = x;
		}

		void setRecorderThread(RunThreadX*	x) {
			m_recThread = x;
		}

		void setClientMgr(ClientMgr *p) {
			m_pClientMgr = p;
		}

		void setWamiInRecMode(bool f) {
			m_isWamiInRecMode = f;
		}


		virtual void forceThreadQuit() {
			RunThreadX::forceThreadQuit();
			stopAndCloseCamera();
		}

		
		//apertureFlag:  0-no chnage, 1-decrease index by one, 2--increase index by one
		virtual ServerErrCode clientCamCtrlAperture( const uint8_t apertureFlag )
		{		
			return SERVER_ERR_NO_ERROR;
		}
		
		//focusFlag:      0-no change, 1-fous out ( decreasing focal lenth), 2--focus in (increase focal length)
		virtual ServerErrCode clientCamCtrlFocus( const uint8_t focusFlag, const int step ) 
		{
			return SERVER_ERR_NO_ERROR;
		}

		//focusFlag:      0-no change, 1- decreasing exposure time, 2--increase exp time
		virtual ServerErrCode clientCamCtrlExp( const uint8_t exposureFlag )
		{
			return SERVER_ERR_NO_ERROR;
		}

		virtual bool getAutoExpFlag(){ return false;}

	protected:
		//set <isEmitSignal> as false when doing single thread none GUI test
		virtual void procNextTask()=0;
		virtual bool procInit()=0;
		virtual bool getNewFrm()=0;

		virtual void stopAndCloseCamera(){}
		virtual void retreaveInitCamPrms(CfgCamXimea &cfgOut){}

		virtual bool setLensApertureFromCfg()	{ return true; }
		virtual bool setFixedExp( const int dt ){ return true; }
		virtual bool setAutoExpFromCfg()		{ return true; }
		virtual bool intiCameraSettingsFromCfg(){ return true; }
		virtual bool initFocusSettingsFromCfg() { return true; }

		cv::Mat ConvertBGR2Bayer(const cv::Mat &BGRImage);

		inline bool doWeWrtToQue()
		{
			bool wrtToQue = m_isWamiInRecMode.load();
			if (!wrtToQue)
			{
				uint32_t numClients = m_pClientMgr->getNumOfClients();
				wrtToQue = (numClients > 0);
			}
			return wrtToQue;
		}

		inline void wakeupConsumerThreads()
		{
			if (m_wamiThread)
			{
				//dumpLog("RunCapSyn::procNextTask(): wamiThread wakeup");
				m_wamiThread->wakeUpToWork();
			}
			else if (m_recThread)
			{
				//dumpLog("RunCapSyn::procNextTask(): recThread wakeup");
				m_recThread->wakeUpToWork();
			}
			else
			{
				//fack consuming, make sure wrt allways success and never have a drop frm
				if (m_dc->m_cudaCapRgbFrmQ)
					m_dc->m_cudaCapRgbFrmQ->sinkNext();

				if (m_dc->m_cudaCapBayerFrmQ)
					m_dc->m_cudaCapBayerFrmQ->sinkNext();
			}
		}

		//extraSleep: set false for real camera, true for syn and replay
		inline void logTimeUsed( const bool extralSleep )
		{
			if (extralSleep)
			{
				int dtSum = timeIntervalMillisec(m_ts[0], m_ts[2]);
				if (dtSum < m_frmInterval_ms)
				{
					BOOST_SLEEP(m_frmInterval_ms - dtSum);
					//	cout << "m_frmInterval_ms=" << m_frmInterval_ms << ",dtSum=" << dtSum << ", capSleptTime=" << (m_frmInterval_ms - dtSum) << endl;
				}
			}

			if (m_frmNum % m_frmFreqToLog == 0)
			{
				dumpLog("Capture time: fn=%llu, dt(prepareImg=%d,wrt2Q=%d), totalTime=%d(ms)",
						m_frmNum,
						timeIntervalMillisec(m_ts[0], m_ts[1]),
						timeIntervalMillisec(m_ts[1], m_ts[2]),
						timeIntervalMillisec(m_ts[0], m_ts[2]) );
			}
		}


	protected:
		const CameraType_t	m_camType;
		int					m_frmInterval_ms;

		//consumer threads
		RunThreadX*	m_wamiThread;
		RunThreadX*	m_recThread;
		ClientMgr*	m_pClientMgr;

		std::atomic<bool>	m_isWamiInRecMode;

		//----timestamp bookkeeping --------
		boost::posix_time::ptime m_ts[3];

	};
	typedef std::shared_ptr<RunCapBase> RunCapBasePtr;
}
#endif // CLIENTGUI_H
