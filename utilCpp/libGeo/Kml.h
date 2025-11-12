#ifndef __KML_H__
#define __KML_H__

#include <iostream>
#include <vector>
#include <string>
#include "GeoConversion.h"
#include "libUtil/AppLog.h"
namespace app {
	class Kml {
	public:
		Kml();
		~Kml();
		void createKml( const std::string  &fpath, const std::string &headName);
		void closeKml();

		void writeIconPtStyle(const std::string &styleId, const std::string &iconUrl, float scale);
		void writeLineStyle(const std::string &styleId, const std::string &colorCode, int width, int labelVisibility);

		void writeLine(const std::vector<GeoPt_LLH_f32> &vLine, const std::string &styleId="", const std::string& name="");
		void writeIconPoints(const std::vector<GeoPt_LLH_f32>& v, const std::string& styleId="", const std::string& name = "", const std::string& description = "");
		void writeIconPoint(const GeoPt_LLH_f32 &pt, const std::string& styleId="", const std::string& name = "", const std::string& description = "");

		//return distance in meters between <lat1,lon1> and <lat2,lon2>
		static double geoDist(const double& lat1_rad, const double& lon1_rad, const double& lat2_rad, const double& lon2_rad);

	protected:
		void writeHead(const std::string &name);
		void writeTail();
	protected:
		FILE* fid{ nullptr };
	};
}

#endif
