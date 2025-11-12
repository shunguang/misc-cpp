#include "WUtil.h"

using namespace std;
void app::wErrMsg(const char* fmt, ...)
{
	app::wWarningMsg(fmt);
	assert(0);
}

void app::wWarningMsg(const char* fmt, ...)
{
	static char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 1024, fmt, args);
	va_end(args);

	//debug version
	std::cout << buf << std::endl;
}

void app::wGenUniformRandNumbers(std::vector<double>& v, const size_t n, const double low, const double up)
{
	double d = up - low;
	v.resize(n);
	for (uint32_t k = 0; k < n; k++) {
		v[k] = low + ((double)rand() / (double)RAND_MAX) * d;
	}


}

//v = [i1:step:i2]
void app::wCreateIndices(std::vector<uint32_t> &v, const uint32_t i1, const uint32_t i2, const uint32_t step)
{
	v.clear();
	for (uint32_t i = i1; i <= i2; i += step) {
		v.push_back(i);
	}
}

//check if <i> is in <v>
bool app::wIsIdxInSet(const std::vector<uint32_t>& vSortedAscending, const uint32_t i)
{
	for (const auto& e : vSortedAscending) {
		if (e == i) { return true; }
		if (e > i)  { return false; }
	}
	return false;
}

//vDst = i \in [i1,i2) but not in vSrc
void app::wSelectIndices(std::vector<uint32_t>& vDst, const std::vector<uint32_t>& vSrc, const uint32_t i1, const uint32_t i2)
{
	vDst.clear();
	for (uint32_t i = i1; i < i2; i++) {
		if (!wIsIdxInSet(vSrc, i)) {
			vDst.push_back(i);
		}
	}
}

double wFactorial(const uint32_t n)
{
	double x = 1.0;
	if (n == 0) {
		return x;
	}

	for (uint32_t i = 1; i <=n; i++) {
		x *= (double)i;
	}
	return x;
}





