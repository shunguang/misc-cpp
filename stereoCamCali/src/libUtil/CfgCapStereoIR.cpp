#include "CfgCapStereoIR.h"

using namespace std;
using namespace cali;

CfgCapStereoIR::CfgCapStereoIR()
: CfgBase()
, src(VIDEO_SRC_FROM_CAMERA)
, sensor_type(SENSOR_T_BOSON320)
, video_mode(V_MODE_YUV)
, cameraIdL(0)
, cameraIdR(1)
, fileOrFolderPath("c:/temp")
, seqName("unknown")
, nChannels(3)
, imgDepth(8)
, frmW0(0)
, frmH0(0)
, frmRate(30.0f)
, nQueueSize(10)
, isDump(false)
{
}

CfgCapStereoIR::CfgCapStereoIR( const CfgCapStereoIR &x )
	: CfgBase( x )
	, src(x.src)
	, sensor_type(x.sensor_type)
	, video_mode(x.video_mode)
	, cameraIdL(x.cameraIdL)
	, cameraIdR(x.cameraIdR)
	, fileOrFolderPath( x.fileOrFolderPath )
	, seqName( x.seqName )
	, nChannels( x.nChannels )
	, imgDepth( x.imgDepth )
	, frmW0( x.frmW0  )
	, frmH0(x.frmH0)
	, frmRate(x.frmRate)
	, nQueueSize(x.nQueueSize)
	, isDump(x.isDump)
{
}

CfgCapStereoIR& CfgCapStereoIR::operator = (const CfgCapStereoIR &x)
{
	//todo: m-thread unsafe
	assert(this != &x);

	CfgBase::operator=(x);
	src = x.src;
	sensor_type = x.sensor_type;
	video_mode = x.video_mode;

	cameraIdL = x.cameraIdL;
	cameraIdR = x.cameraIdR;

	fileOrFolderPath = x.fileOrFolderPath;

	seqName = x.seqName;
	nChannels = x.nChannels;
	imgDepth = x.imgDepth;
	frmW0 = x.frmW0;
	frmH0 = x.frmH0;
	frmRate = x.frmRate;
	nQueueSize = x.nQueueSize;
	isDump = x.isDump;
	return *this;
}


void CfgCapStereoIR::fromPropertyTree(const boost::property_tree::ptree &pt)
{
	src			= (VIDEO_SRC_T)pt.get<int>("src");
	sensor_type = (SENSOR_T)pt.get<int>("sensorType");
	video_mode  = (VIDEO_MODE_T)pt.get<int>("videoMode");

	cameraIdL	= pt.get<int>("cameraIdL");
	cameraIdR	= pt.get<int>("cameraIdR");
	fileOrFolderPath  = pt.get<std::string>("fileOrFolderPath");
	seqName		= pt.get<std::string>("seqName");
	nChannels	= pt.get<int>("nChannels");
	imgDepth	= pt.get<int>("imgDepth");
	frmW0		= pt.get<int>("frmW0");
	frmH0		= pt.get<int>("frmH0");
	frmRate		= pt.get<float>("frmRate");
	nQueueSize = pt.get<int>("nQueueSize");
	isDump = pt.get<int>("isDump");
}

boost::property_tree::ptree CfgCapStereoIR::toPropertyTree() const
{
	boost::property_tree::ptree pt;

	pt.put("src", (int)src);
	pt.put("sensorType", (int)sensor_type);
	pt.put("videoMode", (int)video_mode);

	pt.put("cameraIdL", cameraIdL);
	pt.put("cameraIdR", cameraIdR);
	pt.put("fileOrFolderPath", fileOrFolderPath);
	pt.put("seqName", seqName);
	pt.put("nChannels", nChannels);
	pt.put("imgDepth", imgDepth);
	pt.put("frmW0", frmW0);
	pt.put("frmH0", frmH0);
	pt.put("frmRate", frmRate);
	pt.put("nQueueSize", nQueueSize);
	pt.put("isDump", isDump);
	return pt;
}

