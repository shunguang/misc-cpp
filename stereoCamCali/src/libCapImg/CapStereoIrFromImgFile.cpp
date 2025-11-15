#include  "CapStereoIrFromImgFile.h"

using namespace std;
using namespace cv;
using namespace cali;

CapStereoIrFromImgFile :: CapStereoIrFromImgFile( const CfgCapStereoIR &params)
	: CapBase(params)

	, m_frmIdx(0)
{
}

CapStereoIrFromImgFile :: ~CapStereoIrFromImgFile()
{
}

bool CapStereoIrFromImgFile::endCaptureThread()
{
	return true;
}

//todo
bool	CapStereoIrFromImgFile::startCaptureThread()
{
	//close any already opened capture
	endCaptureThread();
	m_seqL = getImgFiles( m_params.fileOrFolderPath, m_params.seqName, m_vImgFilePaths);
	if ( m_seqL < 1 ) {
		dumpLog("CapStereoIrFromImgFile::startCaptureThread(): no image files in folder!");
		return false;
	}

	cv::Mat I = cv::imread(m_vImgFilePaths[0] );
	m_frmIdx = 0;
	return true;
}

uint32_t CapStereoIrFromImgFile::readNextFrm()
{
	if (m_frmIdx >= m_seqL) {
		return APP_SEQ_END;
	}

	//todo
	cv::Mat L = cv::imread(m_vImgFilePaths[m_frmIdx] );

	++m_frmIdx;
	return m_frmIdx - 1;
}


uint32_t CapStereoIrFromImgFile::getImgFiles(const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames)
{
	namespace fs = boost::filesystem;
	fs::path someDir(dirName);
	fs::directory_iterator end_iter;

	std::string ext0(ext);
	boost::to_upper(ext0);

	vFileNames.clear();
	bool f1 = fs::exists(someDir);
	bool f2 = fs::is_directory(someDir);
	if ( f1 && f2){ 
		for (fs::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter){
			if (fs::is_regular_file(dir_iter->status())){
				fs::path pp(*dir_iter);
				string extName, fName = pp.string();
				cali::getExtName(fName, extName);
				if ( ext0 == extName) {
					vFileNames.push_back(fName);
				}
			}
		}
	}

	std::sort(vFileNames.begin(), vFileNames.end());
	return vFileNames.size();
}