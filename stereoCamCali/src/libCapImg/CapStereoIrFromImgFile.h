/**
 * @file   CapStereoIrFromImgFile.h
 * @author Shunguang Wu
 * @date
 * @brief: a wrapper of getting 8-bit images from avi or image files
 *
 */

#ifndef _CAP_STEREO_IR_FROM_IMG_FILE_H_
#define _CAP_STEREO_IR_FROM_IMG_FILE_H_

#include "CapBase.h"
namespace cali {

	class CapStereoIrFromImgFile : public CapBase {

	public:
		CapStereoIrFromImgFile( const CfgCapStereoIR &params );
		~CapStereoIrFromImgFile();

		virtual bool	startCaptureThread();
		virtual bool	endCaptureThread();

		int				getSeqL() { return m_seqL; }

	private:	
		virtual uint32_t  readNextFrm();
		static uint32_t getImgFiles(const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames);

	private:
		uint32_t			m_frmIdx;
		uint32_t			m_seqL;
		std::vector<std::string> m_vImgFilePaths;
	};

	typedef std::shared_ptr<CapStereoIrFromImgFile>	CapStereoIrFromImgFilePtr;
}
#endif
