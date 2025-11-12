#include "libMat/WMat.h"
using namespace std;
using namespace app;

int test_mat_part1(int argc, char* argv[])
{
	WMatF32 x(3, 2, 0);
	x.print();

	WMatF32 y(3, 2, 1.0f);
	y.print();

	x = y;
	x.print();

	WMatF32 u1(5, 5, std::string("rand"));
	WMatI16 u2(5, 5, std::string("rand"));
	WMatU16 u3(5, 5, std::string("rand"));
	WMatI8 u4(5, 5, std::string("rand"));
	WMatU8 u5(5, 5, std::string("rand"));
	WMatBool u6(5, 5, std::string("rand"));
	WMatBool u7(5, 5, std::string("rand"));

	u1.print("u1=");
	u2.print("u2=");
	u3.print("u3=");
	u4.print("u4=");
	u5.print("u5=");
	u6.print("u6=");
	u7.print("u7=");

	return 0;
}
