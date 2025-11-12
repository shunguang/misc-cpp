#include "libMat/WMat.h"
using namespace std;
using namespace app;
int test_mat_part2(int argc, char* argv[])
{
	WMatF32 x(3, 2, 0);
	x.print();

	WMatF32 y(3, 2, 1.0f);
	y.print();

	x = y;
	x.print();

	x.setData(10.0f);
	x.print();

	bool b1 = (x == y);
	bool b2 = (x != y);

	WMatF32 z = -y;
	z.print();

	WMatF32 z2 = x + 5;
	z2.print();

	z2 = x + y;
	z2.print();

	z2 = x - 3;
	z2.print();
	z2 = x - y;
	z2.print();
	z2 = x * 3;
	z2.print();
	z2 = x * y;
	z2.print();
	z2 = x / 3;
	z2.print();
	z2 = x / y;
	z2.print();

	x += 3;
	x.print();
	x -= 3;
	x.print();
	x *= 3;
	x.print();
	x /= 3;
	x.print();

	x += y;
	x.print();
	x -= y;
	x.print();
	x *= y;
	x.print();
	x /= y;
	x.print();

	//test the friend funcs
	std::cout << x << std::endl;
	z = 5 + x;
	z.print();
	z = 5 - x;
	z.print();
	z = 5 * x;
	z.print();
	z = 5 / x;
	z.print();

	return 0;
}
