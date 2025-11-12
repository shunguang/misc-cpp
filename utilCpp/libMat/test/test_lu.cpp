#include "libMat/WLu.h"
using namespace std;
using namespace app;

#define MY_TYPE  float  //double
int test_lu(int argc, char* argv[])
{
	bool suc;

	//test wmatInv2x2
	{
		int n = 2;
		WMatF64 x(n, n, std::string("rand"));
		WMatF64 xInv(n, n);
		double det = wmatDet2x2(x);
		suc = wmatInv2x2(x, xInv, det);
		xInv.print("xInv = ");
		wmatVerifyInv(x, xInv);

		suc = wmatInv2x2(x);
		x.print("x = ");

		suc = wmatInvByLu(x, xInv, det);
		suc = wmatVerifyInv(x, xInv);
	}

	//test wmatInv3x3
	{
		int n = 3;
		WMatF64 x(n, n, std::string("rand"));
		WMatF64 xInv(n, n);
		double det = wmatDet3x3(x);
		suc = wmatInv3x3(x, xInv, det);
		xInv.print("xInv = ");
		suc = wmatVerifyInv(x, xInv);

		suc = wmatInv3x3(x);
		x.print("x = ");

		suc = wmatInvByLu(x, xInv, det);
		suc = wmatVerifyInv(x, xInv);
	}

  //test lu
	{
		WMatF32 x(2, 2, "eye");
		WMatF32 xInv(2, 2);
		x.print();

		WLuF32 lu1(x);
		lu1.inv(xInv);
		xInv.print();
		suc = wmatVerifyInv(x, xInv);
	}

	{
		int n = 5;
		WMatF32 xL(n, n);
		xL.setRandLow();
		xL.print("xL=");

		WMatF32 xU(n, n);
		xU.setRandLow();
		xU = xU.transpose();
		xU.print("xU=");

		WMatF32  A = xL % xU;
		WMatF32  B(n, n);   //B=inv(A);
		WLuF32 lu(A);
		lu.inv(B);
		B.print();
		wmatVerifyInv(A, B);
	}


	{
		int n = 10;
		WMatF64 xL(n, n);
		xL.setRandLow();
		xL.print("xL=");

		WMatF64 xU(n, n);
		xU.setRandLow();
		xU = xU.transpose();
		xU.print("xU=");

		WMatF64  A = xL % xU;
		WMatF64  B(n, n);   //B=inv(A);
		WLuF64 lu(A);
		lu.inv(B);
		B.print();
		wmatVerifyInv(A, B);
	}

	//test wmatEigen2x2()
	{
		WMatF64 x(2, 2, std::string("rand"));
		WMatF64 v(2, 2);
		WMatF64 val(2,1);
	  suc = wmatEigen2x2(x, v, val);
	}
	return 0;
}
