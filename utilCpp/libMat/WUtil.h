#ifndef __WMAT_UTIL_H__
#define __WMAT_UTIL_H__


#include "WMatDefs.h"
namespace app {

	//todo: interface with a log engin
	void wErrMsg(const char* fmt, ...);
	void wWarningMsg(const char* fmt, ...);
	void wGenUniformRandNumbers(std::vector<double>& v, const size_t n, const double low=0, const double up=1 );

	//v = [i1:step:i2]
	void wCreateIndices(std::vector<uint32_t>& v, const uint32_t i1, const uint32_t i2, const uint32_t step);

	//check if <i> is in <vSorted>
	bool wIsIdxInSet( const std::vector<uint32_t>& vSortedAscending, const uint32_t i);

	//vDst = i \in [i1,i2) but not in vSrc
	void wSelectIndices(std::vector<uint32_t>& vDst, const std::vector<uint32_t>& vSrc, const uint32_t i1, const uint32_t i2);

	double  wFactorial(const int n);
	template<class T>
	T wGetEps() {
		//do return based on type
		return WMAT_F32_EPS;
	}
}

#endif
