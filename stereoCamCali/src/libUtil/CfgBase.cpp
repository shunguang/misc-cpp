#include "CfgBase.h"

using namespace std;
using namespace cali;

CfgBase::CfgBase()
{
}

std::string CfgBase::toString() const {
	boost::property_tree::ptree pt = toPropertyTree();
	return toString(pt);
}

std::string CfgBase::toString(const boost::property_tree::ptree& pt)
{
	std::ostringstream oss;
	boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
	boost::property_tree::xml_parser::write_xml(oss, pt, settings);
	return oss.str();
}

void CfgBase::dumpToLog(const string& msg)
{
	string cfgStr = toString();
	std::vector<std::string> v;
	string_to_vector(v, cfgStr, "\n", false);
	dumpLog(msg);
	for (const std::string& s : v) {
		dumpLog(s);
	}
}
