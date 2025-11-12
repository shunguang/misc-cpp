/*
 *------------------------------------------------------------------------
 * abc.h
 *
 * No  any Corporation or client funds were used to develop this code.
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * Copyright (c) 2022 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <sys/types.h>		// For stat().
#include <sys/stat.h>		// For stat().
#include <cctype>
#include "AppDefs.h"

#ifdef _WINDOWS
#include <io.h>				// For access().
#include <direct.h>
#define GetCurrentDir _getcwd
#define appAccess	_access
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define appAccess	access
#endif

#include <iostream>
#include <filesystem>
namespace app {
	uint32_t  getFileNameList(const std::string& dirName, const std::string& ext, std::vector<std::string>& vFileNames);
	void  getExtName(const std::string& fileName, std::string& ext);
	bool  folderExists(const std::string& strPath);
	bool  fileExists(const std::string& name);

	bool  mkDumpDirs(const std::string& inSeqFileName, const std::string& dumpPathRoot, std::string& dumpPath4Debug, std::string& dumpPath4Customer);
	bool  findSeqName(const std::string& inSeqFileName, std::string& seqName);
	std::string  getCurDir();
	void  deleteFilesInFolder(const std::string& folderPath);

	bool  createDir(const std::string& p);

	//fPath="c:/temp/f1.txt" ---> head = "c:/temp/f1", ext="txt" 
	void  splitExt(const std::string& fPath, std::string& head, std::string& ext);
	//fPath="c:/temp/f1.txt" ---> fDir = "c:/temp", fName="f1.txt" 
	void  splitFolder(const std::string& fPath, std::string& fDir, std::string& fName);

	bool  isVideoFile(const std::string& filePath);
	bool  isImgeFile(const std::string& filePath);
	uint64_t  getAvailableDiskSpaceInByte(const std::string& folderPath);

}

#endif
