#ifndef __CFG_CAP_STEREO_IR_H__
#define __CFG_CAP_STEREO_IR_H__

#include "CfgBase.h"
#include "AppEnums.h"
#include "Roi.h"
namespace cali {
	class  UTIL_EXPORT CfgCapStereoIR : public CfgBase {
	public:
		CfgCapStereoIR();
		CfgCapStereoIR( const CfgCapStereoIR &x );
		CfgCapStereoIR& operator = (const CfgCapStereoIR& x);
		virtual ~CfgCapStereoIR()=default;

		virtual boost::property_tree::ptree toPropertyTree() const override;
		virtual void fromPropertyTree(const boost::property_tree::ptree &pt) override;

	public:
		//video part
		VIDEO_SRC_T		src;
		SENSOR_T		  sensor_type;
		VIDEO_MODE_T	video_mode;

		//camera index used by capture
		int				cameraIdL;
		int				cameraIdR;

		std::string		fileOrFolderPath;
		std::string		seqName;

		int				nChannels;
		int				imgDepth;		//bit-depth of an individual channel: 8,16,24,32 etc
		int				frmW0;			//frm width at L0
		int				frmH0;			//frm height at L0
		float32   frmRate;    //frms/sec
		int				nQueueSize;	//tot # of frms if cap from <VIDEO_SRC_FROM_IMG_FILES>
		bool			isDump;
	};
	typedef std::shared_ptr<CfgCapStereoIR>		CfgCapStereoIR_ptr;
}

#endif




