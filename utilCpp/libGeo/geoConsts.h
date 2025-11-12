#ifndef __GEO_CONSTS_H__
#define __GEO_CONSTS_H__

#include "geoEllipsoid.h"

namespace app {
	/*
		semi-major axis             (a)
		semi-minor axis             (b)
		flattening                  (f)		= (a-b)/a
		flattening inverse          (f-1)	= (1/f)
		first eccentricity          (e)		= sqrt(1-(b2/a2))
		first eccentricity squared  (e2)	= (a2-b2)/a2
		second eccentricity         (e`)	= sqrt((a2/b2)-1)
		second eccentricity squared (e`2)	= (a2-b2)/b2

	*/

	typedef enum {
		AIRY = 0,
		AUSTRALIAN_NATIONAL = 1,
		BESSEL_1841 = 2,
		BESSEL_1841_NAMBIA = 3,
		CLARKE_1866 = 4,
		CLARKE_1880 = 5,
		EVEREST = 6,
		FISCHER_1960 = 7,
		FISCHER_1968 = 8,
		GRS_1967 = 9,
		GRS_1980 = 10,
		HELMERT_1906 = 11,
		HOUGH = 12,
		INTERNATIONAL = 13,
		KRASSOVSKY = 14,
		MODIFIED_AIRY = 15,
		MODIFIED_EVEREST = 16,
		MODIFIED_FISCHER_1960 = 17,
		SOUTH_AMERICAN_1969 = 18,
		WGS_60 = 19,
		WGS_66 = 20,
		WGS_72 = 21,
		WGS_84 = 22,
		SPHERICAL = 23
	} GEODETIC_ELLIPSOID;

	static Ellipsoid ellipsoid[] =
	{	//  id, Ellipsoid name, Equatorial Radius, square of eccentricity	
		Ellipsoid(AIRY, "Airy", 6377563, 0.00667054),
		Ellipsoid(AUSTRALIAN_NATIONAL, "Australian National", 6378160, 0.006694542),
		Ellipsoid(BESSEL_1841, "Bessel 1841", 6377397, 0.006674372),
		Ellipsoid(BESSEL_1841_NAMBIA, "Bessel 1841 (Nambia) ", 6377484, 0.006674372),
		Ellipsoid(CLARKE_1866, "Clarke 1866", 6378206, 0.006768658),
		Ellipsoid(CLARKE_1880, "Clarke 1880", 6378249, 0.006803511),
		Ellipsoid(EVEREST, "Everest", 6377276, 0.006637847),
		Ellipsoid(FISCHER_1960, "Fischer 1960 (Mercury) ", 6378166, 0.006693422),
		Ellipsoid(FISCHER_1968, "Fischer 1968", 6378150, 0.006693422),
		Ellipsoid(GRS_1967, "GRS 1967", 6378160, 0.006694605),
		Ellipsoid(GRS_1980, "GRS 1980", 6378137, 0.00669438),
		Ellipsoid(HELMERT_1906, "Helmert 1906", 6378200, 0.006693422),
		Ellipsoid(HOUGH, "Hough", 6378270, 0.00672267),
		Ellipsoid(INTERNATIONAL, "International", 6378388, 0.00672267),
		Ellipsoid(KRASSOVSKY, "Krassovsky", 6378245, 0.006693422),
		Ellipsoid(MODIFIED_AIRY, "Modified Airy", 6377340, 0.00667054),
		Ellipsoid(MODIFIED_EVEREST, "Modified Everest", 6377304, 0.006637847),
		Ellipsoid(MODIFIED_FISCHER_1960, "Modified Fischer 1960", 6378155, 0.006693422),
		Ellipsoid(SOUTH_AMERICAN_1969, "South American 1969", 6378160, 0.006694542),
		Ellipsoid(WGS_60, "WGS 60", 6378165, 0.006693422),
		Ellipsoid(WGS_66, "WGS 66", 6378145, 0.006694542),
		Ellipsoid(WGS_72, "WGS-72", 6378135, 0.006694318),
		Ellipsoid(WGS_84, "WGS-84", 6378137, 0.00669438),
		Ellipsoid(SPHERICAL, "Spherical", 6371000, 0.0)
	};

}
#endif