#include "libMat/WMatTools1.h"
using namespace std;
using namespace app;

#define MY_TYPE  float 
int test_mat_tools1(int argc, char* argv[])
{
	uint32_t m = 5;
	uint32_t n = 4;
	bool b;
	{
		WMat<MY_TYPE> x = wmatZeros<MY_TYPE>(m, n);
		x = wmatEye<MY_TYPE>(m);
		x.print("x=");
		x = wmatOnes<MY_TYPE>(m, n);
		x.print("x=");
		x = wmatReshape<MY_TYPE>(x, 1, m * n);
		x.print("x=");
	}

	//inverse & det functions

	//x, m x n, matrix
	//return the idxs satisfying the condition
	{
		WMat<MY_TYPE> x(m, n, std::string("rand"));
		x.print("x=");
		MY_TYPE threshhold = 0.5;
		WMatU32 y;
		y = wmatFindIdxGT<MY_TYPE>(x, threshhold);
		y.print("y=");
		y = wmatFindIdxGE<MY_TYPE>(x, threshhold);
		y.print("y=");
		y = wmatFindIdxST<MY_TYPE>(x, threshhold);
		y.print("y=");
		y = wmatFindIdxSE<MY_TYPE>(x, threshhold);
		y.print("y=");
		y = wmatFindIdxEQ<MY_TYPE>(x, threshhold);
		y.print("y=");
	}

	//---------------------------------------------
	// max,min,sum, ect for all elements
	//----------------------------------------------
	{
		MY_TYPE  mean, std;
		uint32_t idx;
		WMat< MY_TYPE> x(n, n, std::string("rand"));
		MY_TYPE  a = wmatMax<MY_TYPE>(x);
		a = wmatMax<MY_TYPE>(x, &idx);
		a = wmatMin<MY_TYPE>(x);
		a = wmatMin<MY_TYPE>(x, &idx);
		a = wmatMean<MY_TYPE>(x);
		a = wmatNorm<MY_TYPE>(x);
		b = wmatMeanStd<MY_TYPE>(x, mean, std);
		a = wmatSum<MY_TYPE>(x);
		b = wmatIsEmpty<MY_TYPE>(x);

		WMatBool y(m, n, true);
		y.print("y=");
		uint32_t s = wmatSumBool(y);
	}

	//----------------------------------------------
	// max,min,sum, ect along the dimension
	// ex:
	//x=rand(2,3)
	//sum(x,1)=[1.7205    1.0404    0.7299]
	//sum(x,2)=[1.57,1.91]^T
	//----------------------------------------------
	{
		WMat<MY_TYPE> x(n, n, std::string("rand"));
		for (uint32_t dimFlag = 1;dimFlag <= 2; dimFlag++) {
			WMat<MY_TYPE> y = wmatMax<MY_TYPE>(x, dimFlag);
			y.print("y=");
			y = wmatMean<MY_TYPE>(x, dimFlag);
			y.print("y=");
			y = wmatMin(x, dimFlag);
			y.print("y=");
			y = wmatMedian<MY_TYPE>(x, dimFlag);
			y.print("y=");
			y = wmatSum<MY_TYPE>(x, dimFlag);
			y.print("y=");
			
			WMatBool x1(m, n, std::string("rand"));
			WMatU32 z =  wmatSumBool(x1, dimFlag);
			z.print();
		}
   }

	{
		WMat<MY_TYPE> x(m, n, std::string("rand"));
		WMat<MY_TYPE> y = wmatAbs<MY_TYPE>(x);
		y = wmatSin<MY_TYPE>(x);
		y = wmatCos<MY_TYPE>(x);
		y = wmatTan<MY_TYPE>(x);
		y = wmatExp<MY_TYPE>(x);
		wMatDump<MY_TYPE>(x, "c:/temp", "tmp", "%5.4f", 1, -1);
	}

	return 0;
}
