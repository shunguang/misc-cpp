/**
* @file   UtilDefs.h
* @author Shunguang@yahoo.com
* @date   Feb 05, 2017
* 
*------------------------------------------------------------------------------
**/

#ifndef _UTIL_DEFS_H
#define _UTIL_DEFS_H

#ifdef _WINDOWS
//------- win32 dll ------------
# if _USRDLL
#	ifdef UTIL_EXPORTS
#		define UTIL_EXPORT __declspec( dllexport )
#	else
#		define UTIL_EXPORT __declspec( dllimport )
#	endif
# else
#	define UTIL_EXPORT
# endif
//------ win32 static lib ------
#else
#	define UTIL_EXPORT
#endif

#endif

