#include "libMat/WLine2d.h"
using namespace std;
using namespace app;

int test_line2d(int argc, char* argv[])
{
	{
		WLine2dF32 line;
		WMatF32 p1(1, 2), p2(1, 2);
		p1(0) = 0;  p1(1) = 0;
		p2(0) = 1;  p2(1) = 1;
		line.resetLine(p1, p2, true);
		float y1 = line.getX(0.5f);
		float y2 = line.getX(0.7f);
		line.print("lineX=");


		WMatF32 vXY(3, 2);
		vXY.setRow(0, p1);
		vXY.setRow(2, p2);
		vXY(1, 0) = 0.5f;
		vXY(1, 1) = 0.5f;

		bool suc = line.resetLineLsq(vXY);
		if (suc) {
			line.print("lineLsq=");
		}
		
		//------------------
		vXY(1, 0) = 0.5f;
		vXY(1, 1) = 0.55f;
		suc = line.resetLineLsq(vXY);
		if (suc) {
			line.print("lineLsq=");
		}

	}

	return 0;
}
