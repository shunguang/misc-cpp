#include "Kml.h"

using namespace std;
using namespace app;

Kml::Kml()
	: fid(0)
{
}

Kml::~Kml()
{
}

void Kml::createKml( const std::string  &fpath, const std::string &headName)
{
	if (fid) {
		closeKml();
	}

	fopen_s(&fid, fpath.c_str(), "w");
	writeHead( headName );
}

void Kml::closeKml()
{
	if (fid) {
		writeTail();
		fclose(fid);
		fid = nullptr;
	}
	dumpLog(" Kml::closeKml(): well done!");
}


void Kml::writeHead(const string &name )
{
	fprintf(fid, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(fid, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
	fprintf(fid, "<Document>\n");
	fprintf(fid, "<name>%s</name>\n", name.c_str());
	fprintf(fid, "\n");
}

void Kml::writeTail()
{
	fprintf(fid, "</Document>\n");
	fprintf(fid, "</kml>\n");
}

void Kml::writeLineStyle(const std::string &styleId, const std::string &colorCode, int width, int labelVisibility)
{
	fprintf(fid, "<Style id=\"%s\">\n", styleId.c_str());
	fprintf(fid, "<LineStyle>\n");
	fprintf(fid, "<color>%s</color>\n", colorCode.c_str());
	fprintf(fid, "<width>%d</width>\n", width);
	fprintf(fid, "<gx:labelVisibility> %d </gx:labelVisibility>\n", labelVisibility);
	fprintf(fid, "</LineStyle>\n");
	fprintf(fid, "</Style>\n");
	fprintf(fid, "\n");
}

void Kml::writeIconPtStyle(const std::string &styleId, const std::string &iconUrl, float scale)
{
	fprintf(fid, "<Style id=\"%s\">\n", styleId.c_str());
	fprintf(fid, "<IconStyle>\n");
	fprintf(fid, "<scale>%f</scale>\n", scale);
	fprintf(fid, "<Icon>\n");
	fprintf(fid, "<href>%s</href>\n", iconUrl.c_str());
	fprintf(fid, "</Icon>\n");
	fprintf(fid, "</IconStyle>\n");
	fprintf(fid, "<LabelStyle>\n");
	fprintf(fid, "<scale>0.7</scale>\n");
	fprintf(fid, "</LabelStyle>\n");
	fprintf(fid, "</Style>\n");
	fprintf(fid, "\n");
}

void Kml::writeLine(const std::vector<GeoPt_LLH_f32> &vLine, const std::string &styleId, const std::string &name)
{
	int bExtrude = 1;
	int bTessellate = 1;

	fprintf(fid, "<Placemark>\n");
	fprintf(fid, "<name>%s</name>\n", name.c_str());
	fprintf(fid, "<styleUrl>#%s</styleUrl>\n", styleId.c_str());

	fprintf(fid, "<LineString>\n");
	fprintf(fid, "<extrude>%d</extrude>\n", bExtrude);
	fprintf(fid, "<tessellate>%d</tessellate>\n", bTessellate);
	fprintf(fid, "<coordinates>\n");
	for(const GeoPt_LLH_f32 &var : vLine) {
		fprintf(fid, "\t%.7f,%.7f,0\n", var.lon_deg, var.lat_deg);
	}
	fprintf(fid, "</coordinates>\n");
	fprintf(fid, "</LineString>\n");
	fprintf(fid, "</Placemark>\n");
	fprintf(fid, "\n");
}

void Kml::writeIconPoints(const std::vector<GeoPt_LLH_f32> &vPts, const std::string &styleId, const std::string &name, const std::string &description)
{
	for(const GeoPt_LLH_f32 &var :  vPts) {
		fprintf(fid, "<Placemark>\n");
		if (!name.empty()) {
			fprintf(fid, "<name>%s</name>\n", name.c_str());
		}

		if (!description.empty()) {
			fprintf(fid, "<description>\n");
			fprintf(fid, "%s\n", description.c_str());
			fprintf(fid, "</description>\n");
		}
		//fprintf(fid, "<styleUrl>#%s</styleUrl>\n", styleId);
		fprintf(fid, "<Point>\n");
		fprintf(fid, "<coordinates>%.7f, %.7f</coordinates>\n", var.lon_deg, var.lat_deg);
		fprintf(fid, "</Point>\n");
		fprintf(fid, "</Placemark>\n");
	}
	fprintf(fid, "\n");
}

void Kml::writeIconPoint(const GeoPt_LLH_f32& pt, const std::string& styleId, const std::string& name, const std::string& description)
{
	fprintf(fid, "<Placemark>\n");
	if (!name.empty()) {
		fprintf(fid, "<name>%s</name>\n", name.c_str());
	}

	if (!description.empty()) {
		fprintf(fid, "<description>\n");
		fprintf(fid, "%s\n", description.c_str());
		fprintf(fid, "</description>\n");
	}
	//fprintf(fid, "<styleUrl>#%s</styleUrl>\n", styleId);
	fprintf(fid, "<Point>\n");
	fprintf(fid, "<coordinates>%.7f, %.7f, %.1f</coordinates>\n", pt.lon_deg, pt.lat_deg, pt.h_m);
	fprintf(fid, "</Point>\n");
	fprintf(fid, "</Placemark>\n");
	fprintf(fid, "\n");
}

double Kml::geoDist(const double& lat1_rad, const double& lon1_rad, const double& lat2_rad, const double& lon2_rad)
{
	const double R = 6378137.0; // Radius of earth in meter

	const double dLat = lat2_rad - lat1_rad;
	const double dLon = lon2_rad - lon1_rad;
	const double sinLat = sin(dLat / 2);
	const double sinLon = sin(dLon / 2);
	const double a = sinLat * sinLat + cos(lat1_rad) * cos(lat2_rad) * sinLon * sinLon;
	const double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	double d_meter = R * c;  // meters
	return d_meter;
}
