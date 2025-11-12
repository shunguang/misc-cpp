#include "libMat/WMat.h"
using namespace std;
using namespace app;

//set sets and gets functions();
int test_mat_part4(int argc, char* argv[])
{
	WMatI32 y(10, 10, std::string("rand"));
	y.print();
	y.resize(10, 20);
	y = 5;
	y.print();
	y.resize(10, 20, 66);
	y.print();
	y.setRand();
	y.print();
	for (uint32_t i = 0; i < y.cols(); i++) {
		y.delCol(i);
	  y.print();
	}
	y.resize(10, 20);
	y.setRand();
	y.print();
	y.delCols(0,10,1);
	y.print();

	y.resize(10, 20);
	y.setRand();
	y.delCols(10, 19, 5);
	y.print();

	y.resize(10, 20);
	y.setRand();
	y.print();
	for (int i = 0; i < 2; i++) {
		y.delRow(i);
		y.print();
	}
	y.resize(10, 20);
	y.setRand();
	y.print();
	y.delRows(0, 6, 2);
	y.print();

	return 0;
}

