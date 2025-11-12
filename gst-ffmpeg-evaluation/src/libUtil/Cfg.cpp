#include "Cfg.h"

using namespace std;
using namespace ngv;
CfgBase::CfgBase()
{
}


std::string CfgBase::toString(const boost::property_tree::ptree &pt)
{
	std::ostringstream oss;
	boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
	boost::property_tree::xml_parser::write_xml(oss, pt, settings);

	return oss.str();
}


//-----------------CfgEnc ----------------------
CfgEnc::CfgEnc()
: CfgBase()
{
}


void CfgEnc::fromPropertyTree(const boost::property_tree::ptree &pt)
{
	imgW = pt.get<int32_t>("imgW");
	imgH = pt.get<int32_t>("imgH");
	tbn = pt.get<int32_t>("tbn");
	tbd = pt.get<int32_t>("tbd");
	bitrate = pt.get<int32_t>("bitrate");
	gstEncNameId =(GstVideoEncNameId) pt.get<int32_t>("gstEncNameId");
	useVidCapFilterAfterEncoder = (bool) pt.get<int>("useVidCapFilterAfterEncoder");
	ffmpegEncName = pt.get<std::string>("ffmpegEncName");
	inputKlvFile = pt.get<std::string>("inputKlvFile");
	inputYuvFile = pt.get<std::string>("inputYuvFile");
	ouputFilePath = pt.get<std::string>("ouputFilePath");
	nFrmsToPeekCpuTimeProfile = pt.get<uint32_t>("nFrmsToPeekCpuTimeProfile");
	if (nFrmsToPeekCpuTimeProfile==0){
		nFrmsToPeekCpuTimeProfile = 0XFFFFFFFF;
	}
}

boost::property_tree::ptree CfgEnc::toPropertyTree() const
{
	boost::property_tree::ptree pt;

	pt.put("imgW", imgW);
	pt.put("imgH",  imgH);
	pt.put("tbn", tbn);
	pt.put("tbd",  tbd);
	pt.put("bitrate", bitrate);
	pt.put("gstEncNameId", (int)gstEncNameId);
	pt.put("useVidCapFilterAfterEncoder", (int)useVidCapFilterAfterEncoder);
	pt.put("ffmpegEncName", ffmpegEncName);
	pt.put("inputKlvFile", inputKlvFile);
	pt.put("inputYuvFile", inputYuvFile);
	pt.put("ouputFilePath", ouputFilePath);
	pt.put("nFrmsToPeekCpuTimeProfile", nFrmsToPeekCpuTimeProfile);

	return pt;
}

std::string CfgEnc::toString() const
{
	boost::property_tree::ptree pt = toPropertyTree();
	return CfgBase::toString(pt);
}


//-----------------CfgDec ----------------------
CfgDec::CfgDec()
: CfgBase()
{
}


void CfgDec::fromPropertyTree(const boost::property_tree::ptree &pt)
{
	imgW = pt.get<int32_t>("imgW");
	imgH = pt.get<int32_t>("imgH");

	std::string tmp = pt.get<std::string>("maxNumOfFrms");
	if ( 0==tmp.compare("ALL") ){
		maxNumOfFrms = 0xFFFFFFFF;
	}
	else{
		maxNumOfFrms = atoi(tmp.c_str() );
	}
	gstVidParser = pt.get<uint32_t>("gstVidParser");
	gstDecName = pt.get<std::string>("gstDecName");
	inputFilePath = pt.get<std::string>("inputFilePath");
	outputKlvFile = pt.get<std::string>("outputKlvFile");
	outputYuvFile = pt.get<std::string>("outputYuvFile");
	nFrmsToPeekCpuTimeProfile = pt.get<uint32_t>("nFrmsToPeekCpuTimeProfile");
	if (nFrmsToPeekCpuTimeProfile==0){
		nFrmsToPeekCpuTimeProfile = 0XFFFFFFFF;
	}
}

boost::property_tree::ptree CfgDec::toPropertyTree() const
{
	boost::property_tree::ptree pt;

	pt.put("imgW", imgW);
	pt.put("imgH",  imgH);
	pt.put("maxNumOfFrms", maxNumOfFrms);
	pt.put("gstVidParser", gstVidParser);
	pt.put("gstDecName", gstDecName);
	pt.put("inputFilePath", inputFilePath);
	pt.put("outputKlvFile", outputKlvFile);
	pt.put("outputYuvFile", outputYuvFile);
	pt.put("nFrmsToPeekCpuTimeProfile", nFrmsToPeekCpuTimeProfile);

	return pt;
}

std::string CfgDec::toString() const
{
	boost::property_tree::ptree pt = toPropertyTree();
	return CfgBase::toString(pt);
}

//-------------- CfgApp --------------------
CfgApp::CfgApp()
	: enc( new CfgEnc() )
	, dec( new CfgDec() )
{
}

void CfgApp::readFromFile(const char *fname)
{
	boost::property_tree::ptree pt;
	boost::property_tree::xml_parser::read_xml( fname, pt);
	this->fromPropertyTree(pt.get_child("cfg"));
}

void CfgApp::writeToFile(const char *fname)
{
	boost::property_tree::ptree pt = toPropertyTree();
	boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
	boost::property_tree::xml_parser::write_xml(fname, pt, std::locale(), settings);
}

std::string CfgApp::toString()
{
	boost::property_tree::ptree pt = toPropertyTree();

	std::ostringstream oss;
	boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
	boost::property_tree::xml_parser::write_xml(oss, pt, settings);
	return oss.str();
}

void CfgApp::fromPropertyTree(const boost::property_tree::ptree &pt)
{
	enc->fromPropertyTree(pt.get_child("enc"));
	dec->fromPropertyTree(pt.get_child("dec"));
}


boost::property_tree::ptree CfgApp::toPropertyTree()
{
	boost::property_tree::ptree ptEnc = enc->toPropertyTree();
	boost::property_tree::ptree ptDec = dec->toPropertyTree();

	boost::property_tree::ptree pt;

	pt.add_child("cfg.enc", ptEnc);
	pt.add_child("cfg.dec", ptDec);
	return pt;
}
