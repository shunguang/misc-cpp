#include "libMat/WMatTools4.h"
using namespace std;
using namespace app;

#define TT  float 
int test_mat_tools4(int argc, char* argv[])
{
		//x = (0:0.1:1)
	TT x1 = 0, dx = 0.1, x2 = 1;
	int n = wmatCalNumElements<TT>(x1, dx, x2);
	WMat<TT> v;
	wmatGenVec<TT>(v, x1, dx, x2);
	v.print("v=");

	WMat<TT> out;
	{
		WMat<TT> x(3, 5, std::string("rand"));
		x.print("x=");
		wmatPdistEuclidean(out, x);
		out.print("out=");

		wmatRandperm(out, 3);
		out.print("out=");

		wmatRepMat(out, x, 2, 3);
		out.print("out=");

		wmatRound(out, x);
		out.print("out=");

		// x: 1 x m, y: 1 x n, z:(m*n) x 2
		WMat<TT> x1(1, 3, "rand");
		WMat<TT> y1(1, 5, "rand");
		wmatTwoTuples(out, x1, y1);
		out.print("out=");
	}

	{
		int n = 4, k = 3;
		uint32_t cnt = wmat_n_choose_k(n, k);

		WMatI32 x4(2, 3, "rand");
		x4.print("x4=");
		WMatI32 out1 = wmatUnique(x4);
		out1.print("out1=");

		//todo
#if 0
		WMatU32 out2;
		std::vector<uint32_t> in(k);
		in[0]=2;
		in[1]=4;
		in[2]=5;
		wmat_n_choose_k_pattern(out2, n, k, in);
		out2.print("out2=");
#endif
	}
	return 0;
}
