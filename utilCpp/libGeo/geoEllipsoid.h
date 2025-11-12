#ifndef __GEO_ELLIPSOID_H__
#define __GEO_ELLIPSOID_H__

#include <iostream>
#include <memory>
namespace app {

	class Ellipsoid {
	public:
		Ellipsoid() : m_id(-1), m_name(NULL), m_radius(-1.0), m_ecc(-1.0) {}
		
		Ellipsoid( const int id, const char *name, const double radius, const double ecc) 
		:m_id(id), m_name(name), m_radius(radius), m_ecc(ecc){}

		Ellipsoid(const Ellipsoid &x)
			:m_id(x.m_id), m_name(x.m_name), m_radius(x.m_radius), m_ecc(x.m_ecc) {}

		~Ellipsoid() {}

		const int m_id;
		const char *m_name;
		const double m_radius;
		const double m_ecc;
	};
} // app
#endif