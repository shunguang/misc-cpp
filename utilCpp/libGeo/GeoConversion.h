#ifndef __GEO_CONVERSION_H__
#define __GEO_CONVERSION_H__
/*	This class supports conversion to/from LLH, ECEF and ENU. The lat/lon parameter
	can be given in radian or degrees by giving the constructor the unit flag. The ENU
	coordinate conversion function can be called with a reference LLH, or the
	SetReferenceLLH() function can be called first, and latter calls to the ENU conversion
	function can use the reduced parameters function calls.

	Notes:

	* The default ellpsoid is the WGS-84. The other definitions are in the constants.h file.

	* The Latitude and Longitude is assumed to be in radians, unless otherwise specied using the
	unit flag in the constructor. The height is assumed to be in meters.

	* The XYZ (aka ECEF) coordinate system is in meters.

	* The ENU coordinate system is in meters from the reference (origin) LLH.

	* The intersection with an ellpsoid accepts an origin point in ECEF (X0, Y0, Z0) and a point on the ray
	defined as (X1, Y1, Z1). The point returns the first intersection with the ellpsoid since it is a convex
	shape.

	***** These function call have NOT been optimized.

	Author: shunguang wu (shunguang@yahoo.com), 12/20/2008
*/

#include "geoConsts.h"
#include "geoEllipsoid.h"

namespace app {
	struct GeoPt_LLH_f64 {
		GeoPt_LLH_f64() : lat(0), lon(0), height(0), valid(false) {}
		double lat, lon, height;
		bool valid;
	};

	struct GeoPt_LLH_f32 {
		GeoPt_LLH_f32(const float lat_deg_=0, const float lon_deg_=0, const float alt_m_=0 )
			:lat_deg(lat_deg_), lon_deg(lon_deg_), h_m(alt_m_) {}

		void set(const GeoPt_LLH_f64& p) {
			lat_deg = (float)p.lat;
			lon_deg = (float)p.lon;
			h_m = (float)p.height;
		}
		float lat_deg, lon_deg, h_m;
	};

	class GeoConversion {
	public:
		typedef enum { RADIANS, DEGREES } DATA_TYPE;
		GeoConversion(DATA_TYPE data_type = RADIANS, GEODETIC_ELLIPSOID ellipsoid_type = WGS_84)
			: m_data_type(data_type), m_useDem(true) 
			, m_ellipsoid(ellipsoid[ellipsoid_type])
		{
			m_refLLH.valid = false;
		}
		~GeoConversion() {}

		// returns true if dem is enabled
		bool demEnabled() const { return m_useDem; }

		// Reference position for ENU coordinates
		bool ReferenceSet() { return m_refLLH.valid; }
		bool SetReferenceLLH(double lat, double lon, double height);
		bool GetReferenceLLH(double& lat, double& lon, double& height) const;
		void ResetReferenceLLH() { m_refLLH.valid = false; }

		// LLH -> XYZ -> LLH
		bool Convert_LLH_To_XYZ(double& X, double& Y, double& Z, double lat, double lon, double height) const;
		bool Convert_XYZ_To_LLH(double& lat, double& lon, double& height, double X, double Y, double Z) const;

		// XYZ -> ENU -> XYZ
		bool Convert_XYZ_To_ENU(double& easting, double& northing, double& up, double X_origin, double Y_origin, double Z_origin,
			double X, double Y, double Z) const;
		bool Convert_ENU_To_XYZ(double& X, double& Y, double& Z, double X_origin, double Y_origin, double Z_origin,
			double easting, double northing, double up) const;

		// LLH -> ENU -> LLH
		bool Convert_LLH_To_ENU(double& easting, double& northing, double& up, double lat_origin, double lon_origin, double height_origin,
			double lat, double lon, double height) const;
		bool Convert_ENU_To_LLH(double& lat, double& lon, double& height, double lat_origin, double lon_origin, double height_origin,
			double easting, double northing, double up) const;

		// functions for calculating the rotation of XYZ->ENU or ENU->XYZ

		// (X, Y, Z) = Rxyz*(E, N, U) + (X0, Y0, Z0)
		// R is assumed to be in row major
		bool Calculate_ENU_To_XYZ_Rot(double* R, double X_origin, double Y_origin, double Z_origin) const;
		// (E, N, U) = Renu*(X-X0, Y-Y0, Z-Z0)
		// R is assumed to be in row major
		bool Calculate_XYZ_To_ENU_Rot(double* R, double X_origin, double Y_origin, double Z_origin) const;

		// These are convenience functions that assume the reference position has been set
		bool Convert_XYZ_To_ENU(double& easting, double& northing, double& up, double X, double Y, double Z) const;
		bool Convert_ENU_To_XYZ(double& X, double& Y, double& Z, double easting, double northing, double up) const;
		bool Convert_LLH_To_ENU(double& easting, double& northing, double& up, double lat, double lon, double height) const;
		bool Convert_ENU_To_LLH(double& lat, double& lon, double& height, double easting, double northing, double up) const;

		// functions for calculating the rotation of XYZ->ENU or ENU->XYZ. See full definition above.
		bool Calculate_ENU_To_XYZ_Rot(double* R) const; // R is assumed to be in row major
		bool Calculate_XYZ_To_ENU_Rot(double* R) const; // R is assumed to be in row major

		// Ellipsoid intersection
		bool IntersectEllipsoid(double& X, double& Y, double& Z, double X0, double Y0, double Z0, double X1, double Y1, double Z1) const;

		// This function computes ray-plane intersection of the EN-plane at the origin where (X0, Y0, Z0) and (X1, Y1, Z1) are ENU 
		// coordinates of the ray. The t variable is te parameterizatio of the ray. If t < 0 then the intersection is behind the camera.
		bool IntersectPlane(double& easting, double& northing, double& up, double& t, double X0, double Y0, double Z0, double X1, double Y1, double Z1) const;

#ifdef USE_RDM
		// DEM intersection
		bool IntersectDem(double& lat, double& lon, double& alt, double X0, double Y0, double Z0, double X1, double Y1, double Z1) const;
#endif

		// verify the latitude and longitude are reasonable
		bool VerifyLat(double lat) const;
		bool VerifyLon(double lon) const;
		bool VerifyLatLon(double lat, double lon) const;

		// return true if the reference LLH position is vaid
		bool RefValid() const { return m_refLLH.valid; }

		//Set data type
		void setDataType(DATA_TYPE dataType) {
			m_data_type = dataType;
		}

	private:
		DATA_TYPE m_data_type{ RADIANS };
		bool			m_useDem{false};
		Ellipsoid m_ellipsoid{};
		GeoPt_LLH_f64 m_refLLH{};
	};

	typedef std::shared_ptr<GeoConversion> GeoConversionPtr;
} // app
#endif
