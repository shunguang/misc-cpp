#ifndef __WMAT_DEFS_H__
#define __WMAT_DEFS_H__

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>  
#include <stdexcept>

#include <cstddef> 
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <typeinfo>

#ifdef _WINDOWS
//------- win32 dll ------------
# if _USRDLL
#	ifdef WMAT_EXPORTS
#		define WMAT_EXPORT __declspec( dllexport )
#	else
#		define WMAT_EXPORT __declspec( dllimport )
#	endif
# else
#	define WMAT_EXPORT
# endif
//------ win32 static lib ------
#else
#	define WMAT_EXPORT
#endif

namespace app {
#define WMAT_F32_EPS		(1e-7f)
#define WMAT_F32_REALMIN (1e-38f)
#define WMAT_F32_REALMAX (1e+38f)

#define WMAT_F64_EPS		(1e-15)
#define WMAT_F64_REALMIN (1e-308)
#define WMAT_F64_REALMAX (1e+308)


#define WMAT_HALF_PI       (1.57079632679490)
#define WMAT_PI            (3.14159265358979)
#define WMAT_ONEandHALF_PI (4.71238898038469)
#define WMAT_TWO_PI        (6.28318530717959)
#define WMAT_D2R           (0.01745329251994)
#define WMAT_R2D           (57.29577951308232)

#define WMAT_ROUND(x)		( floor( (x) + 0.500 ) )
#define WMAT_NAN				( sqrt(-1.0) )
#define WMAT_ISNAN(x)		( (x) != (x) )

#if _DEBUG
#define WMAT_BOUNDS_CHECK	1
#else
#define WMAT_BOUNDS_CHECK	0
#endif

#define WMAT_MIN(a,b)  (((b) < (a)) ? (b) : (a));
#define WMAT_MAX(a,b)  (((b) < (a)) ? (a) : (b));

#define WMAT_CHECK_EMPTY(x,msg)\
					if (x.isEmpty(msg)){\
						return x.isFloatType() ? (T)WMAT_NAN : (T)0;\
					}

#define WMAT_CHECK_DIM_EQ(x,y,msg)\
					if (x.rows() != y.rows() || x.cols() != y.cols() ){\
            wWarningMsg(msg);\
						return false;\
					}
}

#endif
