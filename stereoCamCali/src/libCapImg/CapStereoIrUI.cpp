#include "CapStereoIrUI.h"
using namespace std;
using namespace cv;
using namespace cali;

CapStereoIrUI::CapStereoIrUI()
: m_cap(0)
{
}

CapStereoIrUI::~CapStereoIrUI()
{
}

void CapStereoIrUI::setCfg(const CfgCapStereoIR &params)
{
	m_src = params.src;
	switch (m_src) {
	case VIDEO_SRC_FROM_CAMERA:
		m_cap.reset(new CapStereoIrFromCameras(params));
		break;
	case VIDEO_SRC_FROM_V4L:
#if !_WINDOWS
		m_cap.reset(new CapStereoV4L(params));
#endif
		break;
	case VIDEO_SRC_FROM_IMG_FILES:
		m_cap.reset(new CapStereoIrFromImgFile(params));
		break;
	}
}

void CapStereoIrUI::setOutputFrmImgQ( RawFrmQPtr &q)
{
	m_cap->setOutputQue(q);
}

bool CapStereoIrUI::startCapThread( std::string &errMsg)
{
	errMsg = "";
	bool suc = m_cap->startCaptureThread(); 
	if( !suc ){
		switch (m_src) {
		case VIDEO_SRC_FROM_CAMERA:
		case VIDEO_SRC_FROM_V4L:
			errMsg = "cannot open device!";
			break;
		case VIDEO_SRC_FROM_IMG_FILES:
			errMsg = string("cannot open read files in folder: ") + m_cap->m_params.fileOrFolderPath;
			break;
		}
	}
	return suc;
}


//return true if has new frm in <m_cap->m_frmQ>, false otherwise
bool CapStereoIrUI:: getNextFrm(RawFrmIrStereoImg &des)
{
	CapBase *p = m_cap.get();
	return p->getNextFrm(des);
	
#if 0	
	if (m_src == VIDEO_SRC_FROM_CAMERA) {
		CapStereoIrFromCameras *p2 = dynamic_cast<CapStereoIrFromCameras *>(p);
		return p2->getNextFrm(des);
	}
	else if (m_src == VIDEO_SRC_FROM_V4L) {
		CapStereoV4L *p2 = dynamic_cast<CapStereoV4L *>(p);
		return p2->getNextFrm(des);
	}
	else if (m_src == VIDEO_SRC_FROM_IMG_FILES) {
		CapStereoIrFromImgFile *p2 = dynamic_cast<CapStereoIrFromImgFile *>(p);
		return p2->getNextFrm(des);
	}
#endif
}

void CapStereoIrUI::endCaputreThread()
{
	if (m_cap.get()) {
		m_cap->endCaptureThread();
	}
}
