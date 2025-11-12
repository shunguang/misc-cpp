/**
*------------------------------------------------------------------------------
* @file   MsgDefs.h
* @author Shunguang.Wu@jhuapl.edu
* @date   12/01, 2015
* 
* This class defines msgs passed between client and server
*
* Copyright © 2015 JHU/APL
*------------------------------------------------------------------------------
**/

#ifndef __MSG_DEFS_H__
#define __MSG_DEFS_H__

#ifdef _WINDOWS
#if _USRDLL
#		ifdef MSG_EXPORTS
#			define MSG_EXPORT __declspec( dllexport )
#		else
#			define MSG_EXPORT __declspec( dllimport )
#		endif
#	else
#		define MSG_EXPORT
#	endif
#else
#	define MSG_EXPORT
#endif
#endif

