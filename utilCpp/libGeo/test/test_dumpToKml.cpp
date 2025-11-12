#include <iostream>
#include "libGeo/geoConversion.h"
#include "libGeo/Kml.h"

using namespace std;
using namespace app;
void test_dumpToKml(const std::string f, vector<app::GeoPt_LLH_f32> &vLine, vector<app::GeoPt_LLH_f32> &vPts)
{
	Kml K;

	string headName = "MyKml";
	string line_styleId = "LineStyId";
	string line_colorCode = "FF0000ff"; //"aabbggrr"
	string line_name = "imgBorder";

	string pt_styleId = "PointStyId";
	string pt_colorCode = "FFff0000"; //"aabbggrr"
	string pt_name = "";

	K.createKml(f, headName);
	K.writeIconPtStyle(pt_styleId, "noUrl", 1.0);
	K.writeLineStyle(line_styleId, line_colorCode, 1, 1);

	K.writeLine(vLine, line_styleId, line_name);
	K.writeIconPoints(vPts, pt_styleId, pt_name, "");
	K.closeKml();
}
