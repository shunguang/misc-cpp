#ifndef _CAP_STEREO_IR_UI_H_
#define _CAP_STEREO_IR_UI_H_

#include "CapBase.h"
#include "CapStereoIrFromImgFile.h"
#include "CapStereoIrFromCameras.h"
#include "CapStereoV4L.h"

#include "CapDefs.h"
namespace cali {
	class CAP_EXPORT CapStereoIrUI {
	public:
		CapStereoIrUI();
		~CapStereoIrUI();

		//call order
		void setCfg(const CfgCapStereoIR &params);
		void setOutputFrmImgQ( RawFrmQPtr &q);
		bool startCapThread( std::string &errMsg );
		
		//get a new frm, return true if has new frm in <m_cap->m_frmQ>, false otherwise
		bool  getNextFrm( RawFrmIrStereoImg &des );

		void endCaputreThread();

	protected:
		CapBasePtr		m_cap;
		VIDEO_SRC_T		m_src;
	};

	typedef std::shared_ptr<CapStereoIrUI>		CapStereoIrUI_ptr;
}
#endif
