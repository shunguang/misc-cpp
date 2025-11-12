#include "libMat/WMatTools3.h"
using namespace std;
using namespace app;

#define TT  float 
int test_mat_tools3(int argc, char* argv[])
{
	TT ans;
	bool suc;
	uint32_t m = 8, n = 10;

	{
		const WMat<TT> v(1, n, std::string("rand"));
		const WMat<TT> u(1, n, std::string("rand"));
		v.print("v=");
		u.print("u=");

		ans = wmatDotProduct(v, u);

		ans = wmatDotProduct(v.data(), u.data(), n);
	}

	{
		WMat<TT> v1(2, 1, 0), v2(2, 1, 0);
		v1(0) = 1;
		v2(1) = 1;

		v1.print("v1=");
		v2.print("v2=");

		ans = wmatAngleBetweenTwoVectorsInRad(v1, v2);


		WMat<TT> x = wmatCrossProduct(v1, v2);
		x.print("x=");

		x = wmatOutProduct(v1, v2);
		x.print("x=");
	}

	return 0;
}
