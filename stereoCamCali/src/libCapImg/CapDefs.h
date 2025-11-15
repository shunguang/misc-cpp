/*
 *------------------------------------------------------------------------------
 *only depends on CV
 *------------------------------------------------------------------------------
 */
//$Id:  $
#ifndef _CAP_DEFS_H_
#define _CAP_DEFS_H_

#ifdef _WINDOWS
//------- win32 dll ------------
# if _USRDLL
#	ifdef CAPTURE_EXPORTS
#		define CAP_EXPORT __declspec( dllexport )
#	else
#		define CAP_EXPORT __declspec( dllimport )
#	endif
//------ win32 static lib ------
# else
#	define CAP_EXPORT
# endif
#else
#	define CAP_EXPORT
#endif

#endif
