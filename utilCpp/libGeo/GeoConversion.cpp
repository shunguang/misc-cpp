
// $Id:
#if _WINDOWS
#define _USE_MATH_DEFINES
#endif

#include <math.h>

#include "GeoConversion.h"

#define RAD2DEG(x) (x*180.0/M_PI)
#define DEG2RAD(x) (x*M_PI/180.0)
#define EPS 2.2204e-016

using namespace app;

// macros for rotation 
#define R11(x) x[0]
#define R12(x) x[1]
#define R13(x) x[2]
#define R21(x) x[3]
#define R22(x) x[4]
#define R23(x) x[5]
#define R31(x) x[6]
#define R32(x) x[7]
#define R33(x) x[8]

bool GeoConversion::SetReferenceLLH(double lat, double lon, double height)
{
	if(VerifyLatLon(lat, lon)) {
		m_refLLH.lat = lat;
		m_refLLH.lon = lon;
		m_refLLH.height = height;
		m_refLLH.valid = true;
	}
	else {
		m_refLLH.lat = 0.0;
		m_refLLH.lon = 0.0;
		m_refLLH.height = 0.0;
		m_refLLH.valid = false;
	}
	return m_refLLH.valid;
}

bool GeoConversion::GetReferenceLLH(double &lat, double &lon, double &height) const
{
	lat = m_refLLH.lat;
	lon = m_refLLH.lon;
	height = m_refLLH.height;
	return m_refLLH.valid;
}

bool GeoConversion::Convert_LLH_To_XYZ(double &X, double &Y, double &Z, double lat, double lon, double height) const
{
	double llat = lat;
	double llon = lon;

	if(m_data_type == DEGREES) {
		llat = DEG2RAD(lat);
		llon = DEG2RAD(lon);
	}

	if(!VerifyLatLon(llat, llon))
		return false;

	// Convert lat, long, height in WGS84 to ECEF X,Y,Z
	double a = m_ellipsoid.m_radius; // earth semimajor axis in meters
	double e2 = m_ellipsoid.m_ecc; // second eccentricity

	double lambda = sin(llat);
	double chi = sqrt(1-e2*(lambda*lambda)); 
	X = (a/chi+height)*cos(llat)*cos(llon); 
	Y = (a/chi+height)*cos(llat)*sin(llon); 
	Z = (a*(1-e2)/chi+height)*sin(llat);
	return true;
}

bool GeoConversion::Convert_XYZ_To_LLH(double &lat, double &lon, double &height, double X, double Y, double Z) const
{
	double a = m_ellipsoid.m_radius; // earth semimajor axis in meters
	double e2 = m_ellipsoid.m_ecc; // second eccentricity
	double b = sqrt(-(a*a*e2-a*a));	// semi-minor axis
	double f = (a-b)/a; // flattening
	double ep2 = f*(2-f)/((1-f)*(1-f)); // second eccentricity squared

	double r2 = X*X+Y*Y;
	double r = sqrt(r2);
	double E2 = a*a-b*b;
	double F = 54*(b*b)*(Z*Z);
	double G = r2 + (1-e2)*(Z*Z) - e2*E2;
	double c = (e2*e2*F*r2)/(G*G*G);
	double s = pow(1 + c + sqrt(c*c + 2*c), 1.0/3.0);
	double P = F/(3*((s+1/s+1)*(s+1/s+1))*G*G);
	double Q = sqrt(1+2*e2*e2*P);
	double ro = -(e2*P*r)/(1+Q) + sqrt((a*a/2)*(1+1/Q) - ((1-e2)*P*Z*Z)/(Q*(1+Q)) - P*r2/2);
	double tmp = (r-e2*ro)*(r-e2*ro);
	double U = sqrt( tmp + Z*Z );
	double V = sqrt( tmp + (1-e2)*Z*Z );
	double zo = (b*b*Z)/(a*V);

	height = U*( 1 - (b*b)/(a*V));
	lat = atan( (Z + ep2*zo)/r );
	lon = atan2(Y,X);

	if(m_data_type == DEGREES) {
		lat = RAD2DEG(lat);
		lon = RAD2DEG(lon);
	}

	return true;
}

bool GeoConversion::Convert_XYZ_To_ENU(double &easting, double &northing, double &up, double X_origin, double Y_origin, double Z_origin, 
											 double X, double Y, double Z) const
{
	// convert ECEF coordinates to local east, north, up 

	double lat, lon, height;
	if(!Convert_XYZ_To_LLH(lat, lon, height, X_origin, Y_origin, Z_origin))
		return false;

	if(m_data_type== DEGREES) {
		lat = DEG2RAD(lat);
		lon = DEG2RAD(lon);
	}

	easting = -sin(lon)*(X-X_origin) + cos(lon)*(Y-Y_origin); 
	northing = -sin(lat)*cos(lon)*(X-X_origin) - sin(lat)*sin(lon)*(Y-Y_origin) + cos(lat)*(Z-Z_origin); 
	up = cos(lat)*cos(lon)*(X-X_origin) + cos(lat)*sin(lon)*(Y-Y_origin) + sin(lat)*(Z-Z_origin);
	return true;
}

bool GeoConversion::Convert_ENU_To_XYZ(double &X, double &Y, double &Z, double X_origin, double Y_origin, double Z_origin, 
											 double easting, double northing, double up) const
{
	// Convert east, north, up coordinates (labelled e, n, u) to ECEF
	// coordinates. The reference point (XYZ) must be given. All distances are in metres
 
	double lat_origin, lon_origin, height_origin;
	if(!Convert_XYZ_To_LLH(lat_origin, lon_origin, height_origin, X_origin, Y_origin, Z_origin))
		return false;
	
	if(m_data_type == DEGREES) {
		lat_origin = DEG2RAD(lat_origin);
		lon_origin = DEG2RAD(lon_origin);
	}
 
	X = -sin(lon_origin)*easting - cos(lon_origin)*sin(lat_origin)*northing + cos(lon_origin)*cos(lat_origin)*up + X_origin;
	Y = cos(lon_origin)*easting - sin(lon_origin)*sin(lat_origin)*northing + cos(lat_origin)*sin(lon_origin)*up + Y_origin;
	Z = cos(lat_origin)*northing + sin(lat_origin)*up + Z_origin;
	return true;
}

bool GeoConversion::Convert_LLH_To_ENU(double &easting, double &northing, double &up, double lat_origin, double lon_origin, double height_origin, 
											 double lat, double lon, double height) const
{
	// reference lat, lon, height
	double X_origin, Y_origin, Z_origin;
	if(!Convert_LLH_To_XYZ(X_origin, Y_origin, Z_origin, lat_origin, lon_origin, height_origin))
		return false;

	// point to find ENU of
	double X, Y, Z;
	if(!Convert_LLH_To_XYZ(X, Y, Z, lat, lon, height))
		return false;

	// map to ENU in reference to (X_origin, Y_origin, Z_origin)
	if(!Convert_XYZ_To_ENU(easting, northing, up, X_origin, Y_origin, Z_origin, X, Y, Z))
		return false;

	return true;
}

bool GeoConversion::Convert_ENU_To_LLH(double &lat, double &lon, double &height, double lat_origin, double lon_origin, double height_origin, 
											 double easting, double northing, double up) const
{
	// convert reference LLH to XYZ
	double X_origin, Y_origin, Z_origin;
	if(!Convert_LLH_To_XYZ(X_origin, Y_origin, Z_origin, lat_origin, lon_origin, height_origin))
		return false;

	// ENU to XYZ
	double X, Y, Z;
	if(!Convert_ENU_To_XYZ(X, Y, Z, X_origin, Y_origin, Z_origin, easting, northing, up))
		return false;

	// XYZ to LLH
	if(!Convert_XYZ_To_LLH(lat, lon, height, X, Y, Z))
		return false;

	return true;
}

bool GeoConversion::Calculate_ENU_To_XYZ_Rot(double *R, double X_origin, double Y_origin, double Z_origin) const
{
	double lat_origin, lon_origin, height_origin;
	if(!Convert_XYZ_To_LLH(lat_origin, lon_origin, height_origin, X_origin, Y_origin, Z_origin))
		return false;

	if(m_data_type== DEGREES) {
		lat_origin = DEG2RAD(lat_origin);
		lon_origin = DEG2RAD(lon_origin);
	}

	R11(R) = -sin(lon_origin);
	R12(R) = -cos(lon_origin)*sin(lat_origin);
	R13(R) = cos(lon_origin)*cos(lat_origin);
	R21(R) = cos(lon_origin);
	R22(R) = -sin(lon_origin)*sin(lat_origin);
	R23(R) = cos(lat_origin)*sin(lon_origin);
	R31(R) = 0;
	R32(R) = cos(lat_origin);
	R33(R) = sin(lat_origin);

	return false;
}

bool GeoConversion::Calculate_XYZ_To_ENU_Rot(double *R, double X_origin, double Y_origin, double Z_origin) const
{
	double lat_origin, lon_origin, height_origin;
	if(!Convert_XYZ_To_LLH(lat_origin, lon_origin, height_origin, X_origin, Y_origin, Z_origin))
		return false;

	if(m_data_type== DEGREES) {
		lat_origin = DEG2RAD(lat_origin);
		lon_origin = DEG2RAD(lon_origin);
	}

	R11(R) = -sin(lon_origin);
	R12(R) = cos(lon_origin);
	R13(R) = 0.0;
	R21(R) = -sin(lat_origin)*cos(lon_origin);
	R22(R) = - sin(lat_origin)*sin(lon_origin);
	R23(R) = cos(lat_origin);
	R31(R) = cos(lat_origin)*cos(lon_origin);
	R32(R) = cos(lat_origin)*sin(lon_origin);
	R33(R) = sin(lat_origin);
	return true;
}

bool GeoConversion::Convert_XYZ_To_ENU(double &easting, double &northing, double &up, double X, double Y, double Z) const
{
	if(!m_refLLH.valid)
		return false;

	double X_origin, Y_origin, Z_origin;
	if(!Convert_LLH_To_XYZ(X_origin, Y_origin, Z_origin, m_refLLH.lat, m_refLLH.lon, m_refLLH.height)) 
		return false;

	return Convert_XYZ_To_ENU(easting, northing, up, X_origin, Y_origin, Z_origin, X, Y, Z);
}

bool GeoConversion::Convert_ENU_To_XYZ(double &X, double &Y, double &Z, double easting, double northing, double up) const
{
	if(!m_refLLH.valid)
		return false;

	double X_origin, Y_origin, Z_origin;
	if(!Convert_LLH_To_XYZ(X_origin, Y_origin, Z_origin, m_refLLH.lat, m_refLLH.lon, m_refLLH.height)) 
		return false;

	return Convert_ENU_To_XYZ(X, Y, Z, X_origin, Y_origin, Z_origin, easting, northing, up);
}

bool GeoConversion::Convert_LLH_To_ENU(double &easting, double &northing, double &up, double lat, double lon, double height) const
{
	return Convert_LLH_To_ENU(easting, northing, up, m_refLLH.lat, m_refLLH.lon, m_refLLH.height, lat, lon, height);
}

bool GeoConversion::Convert_ENU_To_LLH(double &lat, double &lon, double &height, double easting, double northing, double up) const
{
	return Convert_ENU_To_LLH(lat, lon, height, m_refLLH.lat, m_refLLH.lon, m_refLLH.height, easting, northing, up);
}

bool GeoConversion::Calculate_ENU_To_XYZ_Rot(double *R) const
{
	if(!m_refLLH.valid)
		return false;

	// convert reference LLH to XYZ
	double X_origin, Y_origin, Z_origin;
	if(!Convert_LLH_To_XYZ(X_origin, Y_origin, Z_origin, m_refLLH.lat, m_refLLH.lon, m_refLLH.height))
		return false;

	// set the rotation matrix given XYZ origin and ENU coordinates
	return Calculate_ENU_To_XYZ_Rot(R, X_origin, Y_origin, Z_origin);
}

bool GeoConversion::Calculate_XYZ_To_ENU_Rot(double *R) const
{
	if(!m_refLLH.valid)
		return false;

	// convert reference LLH to XYZ
	double X_origin, Y_origin, Z_origin;
	if(!Convert_LLH_To_XYZ(X_origin, Y_origin, Z_origin, m_refLLH.lat, m_refLLH.lon, m_refLLH.height))
		return false;

	// set the rotation matrix given XYZ origin and ENU coordinates
	return Calculate_XYZ_To_ENU_Rot(R, X_origin, Y_origin, Z_origin);
}

bool GeoConversion::IntersectPlane(double &easting, double &northing, double &up, double &t, double X0, double Y0, double Z0, double X1, double Y1, double Z1) const
{
	double xd = X1-X0;
	double yd = Y1-Y0;
	double zd = Z1-Z0;

	// plane equation assuming the normal is the UP in the ENU frame
	double A = 0;
	double B = 0;
	double C = 1;
	double D = 0;

	// Plane intersection in expanded form:
	//
	// t = -(AX0 + BY0 + CZ0 + D) / (AXd + BYd + CZd)
	//   = -(Pn R0 + D) / (Pn Rd)

	double vd = (A*xd + B*yd + C*zd);
	if(fabs(vd) < EPS)
		return false;
	t = -(A*X0 + B*Y0 + C*Z0 + D) / vd;
	
	easting		= X0+t*xd;
	northing	= Y0+t*yd;
	up			= Z0+t*zd;

	return true;
}

bool GeoConversion::IntersectEllipsoid(double &X, double &Y, double &Z, double X0, double Y0, double Z0, double X1, double Y1, double Z1) const
{
	double a = m_ellipsoid.m_radius; // earth semimajor axis in meters 
	double e2 = m_ellipsoid.m_ecc; // second eccentricity
	double b = sqrt(-(a*a*e2-a*a));	// semi-minor axis
	//double f = (a-b)/a; // flattening

	Ellipsoid earth_sphere = ellipsoid[SPHERICAL];
	double r = earth_sphere.m_radius; // earth spherical radius in meters

	// transformation from earth sphere to designated ellpsoid
	double ax = a/r;
	double ay = a/r;
	double az = b/r;

	// compute n
	double nx = X1-X0;
	double ny = Y1-Y0;
	double nz = Z1-Z0;
	double m = sqrt(nx*nx+ny*ny+nz*nz);

	// make sure m isn't too small
	if(m < EPS)
		return false;

	nx /= m;
	ny /= m;
	nz /= m;

	// transform ray for intersecting with earth sphere
	double ox = 1/ax*X0;
	double oy = 1/ay*Y0;
	double oz = 1/az*Z0;
	double dx = 1/ax*nx;
	double dy = 1/ay*ny;
	double dz = 1/az*nz;

	// intersect with earth sphere
	double A = dx*dx+dy*dy+dz*dz;  // A = dot(d,d) which is 1 if normalized
	double B = 2*(ox*dx+oy*dy+oz*dz);
	double C = (ox*ox+oy*oy+oz*oz)-(r*r);
	double D = (B*B-4*A*C);

	// verify ray intersects with the sphere
	if(D < 0) return false;

	// two quadratic solutions
	double t1 = (-B+sqrt(D))/(2*A);
	double t2 = (-B-sqrt(D))/(2*A);

	// choose intersection where abs(t) is smaller
	double t = (fabs(t1) < fabs(t2)) ? t1 : t2;

	// intersection on sphere
	double px = ox+t*dx;
	double py = oy+t*dy;
	double pz = oz+t*dz;

	// transform intersection to ellpsoid
	X = ax*px;
	Y = ay*py;
	Z = az*pz;
	return true;
}

#ifdef USE_RDM
bool GeoConversion::IntersectDem(double &lat, double &lon, double &alt, double X0, double Y0, double Z0, double X1, double Y1, double Z1) const
{
	rdm::RayEcef ray;
    rdm::PointWgs84 intersectPt;
    double cep, lep;
	
	rdm::PointEcef p0(X0, Y0, Z0);
	rdm::PointEcef p1(X1, Y1, Z1);
	ray.setFromPoints(p0, p1);

	rdm::Rdm* rdm = rdm::getInstance();
	Lockit lockit(rdm->mutex());
	if(!rdm->findRayIntersection(ray, intersectPt, &cep, &lep)) {
		lat = lon = alt = 0;
		return false;
	}

	lat = intersectPt.lat();
	lon = intersectPt.lon();
	alt = intersectPt.hae();

	if(m_data_type == DEGREES) {
		lat = RAD2DEG(lat);	
		lon = RAD2DEG(lon);	
	}

	return true;
}
#endif 

bool GeoConversion::VerifyLat(double lat) const
{
	if(m_data_type == DEGREES) 
		lat = DEG2RAD(lat);

	if(lat < -M_PI/2.0)
		return false;
	if(lat > M_PI/2.0)
		return false;
	return true;
}
bool GeoConversion::VerifyLon(double lon) const
{
	if(m_data_type == DEGREES) 
		lon = DEG2RAD(lon);

	if(lon < -M_PI)
		return false;
	if(lon > M_PI)
		return false;
	return true;
}

bool GeoConversion::VerifyLatLon(double lat, double lon) const
{
	if(!VerifyLat(lat))
		return false;
	if(!VerifyLon(lon))
		return false;
	return true;
}
