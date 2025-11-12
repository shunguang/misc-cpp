#include "libMat/WMat.h"

using namespace std;
using namespace app;

//set sets and gets functions();
int test_mat_part3(int argc, char* argv[])
{
	float buf[] = { 1,2,3,4,5};
	WMatF32 y(1, 5, buf, 5);
	WMatF32 z(1, 3, buf, 3);
	y.print("y=");
	z.print("z=");

	WMatF32 u(5, 5, 6);
	u.print("u=");

	WMatF32 x(5, 5, 1);
	x.print();
	
	x.setCol(1, 3);
	x.print();

	x.setCol(0, buf, 5);
	x.print();

	x.setCol(1, y);
	x.print();

	x.setSubCol(0, 2, 0, 10);
	x.print();

	x.setSubCol(1, 3, 1, buf, 5);
	x.print();

	x.setSubCol(2, 4, 2, z);
	x.print();

	x.setDiagonal(10);
	x.print();

	x.setDiagonal(y.data(), 5);
	x.print();

	x.setDiagonal(y);
	x.print();

	x.setRow(0, 23);
	x.print();
	x.setRow(1, y.data(), 5);
	x.print();
	x.setRow(2, y);
	x.print();

	//x(r, c0:c1)=val
	x.setSubRow(0, 0, 2, 25);
	x.print();
	x.setSubRow(2, 0, 2, y.data(), 5);
	x.print();
	x.setSubRow(4, 0, 2, y);
	x.print();

	//x(r0:r1, :)=val
	x.setRows(0, 2, 32);
	x.print();
	x.setRows(2, 3, u.data(), u.size());
	x.print();
	x.setRows(4, 4, u);
	x.print();

	//get funcs()
	WMatF32 w =x.getSlice(1, 2, 1, 3);
	w.print();

	//x(:,col)	
	w=x.getCol(2); 
	w.print();

	//x(r0:r1, col)	
	w =x.getSubCol( 2, 4, 4);
	w.print();

	//x(:, c1:c2)	
	w = x.getCols(2,4);
	w.print();

	w=x.getRow(1);
	w.print();

	//x(r,c0:c1)
	w=x.getSubRow(1, 2,4);
	w.print();

	//x(r0:r1, :)
	w=x.getRows(0,2);
	w.print();

	w=x.getDiagonal();
	w.print();

	cout << x.trace() << endl;

	x.writeToFile("c:/temp/t1.txt", 2);

	std::vector<std::string> v = { "line 1", "line 2" };
	x.writeToFile("c:/temp/t2.txt", 2, v);

	return 0;
}

