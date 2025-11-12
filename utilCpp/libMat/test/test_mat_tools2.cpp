#include "libMat/WMatTools2.h"
using namespace std;
using namespace app;

#define TT  float 
int test_mat_tools2(int argc, char* argv[])
{
	bool suc;
	int m=8, n = 10;
	{
		WMat<TT> x(m, n, std::string("rand"));
		WMat<TT> y(m, n, std::string("rand"));
		WMat<TT> z; //test resize 

		x.print("x=");
		y.print("y=");

		//z(i,j) = min( x(i,j), y(i,j) ), for i \in [0,m), j \in [0, n)
		suc = wmatMin(z, x, y);
		z.print("z=");

		//z(i,j) = max( x(i,j), y(i,j) ), for i \in [0,m), j \in [0, n)
		suc = wmatMax(z, x, y);
		z.print("z=");
		

		x.print("x=");
		WMatBool vIdx(1, n, true);
		for (int i=1; i<n; i+=2) vIdx(i) = false;

		WMatU32 vIdx2(1, 2);
		vIdx2(0) = 1;
		vIdx2(1) = n / 2;

		WMatBool vIdx3(1, m, true);
		for (int i = 1; i < m; i += 2) vIdx3(i) = false;

		vIdx.print("vIdx=");
		vIdx2.print("vIdx2=");
		vIdx3.print("vIdx3=");

		suc = wmatGetCols( z, x, vIdx);
		z.print("z=");

		suc = wmatGetCols(z, x, vIdx2);
		z.print("z=");

		suc = wmatGetRows( z, x, vIdx3);
		z.print("z=");

		//out = [x(vIdx(1), :); ...; x(vIdx(end), :)], $vIdx$ is the indices of rows
		suc = wmatGetRows(z, x, vIdx2);
		z.print("z=");
	}

	{
		const TT thd = 0.5;
		uint32_t nEle = 0;

		WMatBool out;
		WMat<TT> x(m, n, std::string("rand"));

		nEle = wmatLogicGT(out, x, thd);
		out.print("out=");

		nEle = wmatLogicGE(out, x, thd);
		out.print("out=");

		nEle = wmatLogicST(out, x, thd);
		out.print("out=");

		nEle = wmatLogicSE(out, x, thd);
		out.print("out=");

		nEle = wmatLogicEQ(out, x, thd, (TT)1e-7);
		out.print("out=");

		nEle = wmatLogicGT(out, x, thd);
		out.print("out=");

		suc = wmatLogicAnyGT(x, (TT)0.5);


		WMat<bool> x1(m, n, std::string(std::string("rand")));
		WMat<bool> y1(m, n, std::string(std::string("rand")));
		x1.print("x1=");
		y1.print("y1=");

		nEle = wmatLogicAND(out, x1, y1);
		out.print("out=");

		nEle = wmatLogicOR(out, x1, y1);
		out.print("out=");


		nEle = wmatLogicAll(out, x1, 1);
		out.print("out=");
		nEle = wmatLogicAll(out, x1, 2);
		out.print("out=");

		nEle = wmatLogicAny(out, x1, 1);
		out.print("out=");
		nEle = wmatLogicAny(out, x1, 2);
		out.print("out=");

	}

	return 0;
}
