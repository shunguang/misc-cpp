#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <sys/types.h>		// For stat().
#include <sys/stat.h>		// For stat().
#include <cctype>
#include "DataTypes.h"

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

namespace cali {
	uint32_t  getFileNameList(const std::string& dirName, const std::string& ext, std::vector<std::string>& vFileNames);
	void  getExtName(const std::string& fileName, std::string& ext);
	bool  folderExists(const std::string& strPath);
	bool  fileExists(const std::string& name);

	bool  mkDumpDirs(const std::string& inSeqFileName, const std::string& dumpPathRoot, std::string& dumpPath4Debug, std::string& dumpPath4Customer);
	bool  findSeqName(const std::string& inSeqFileName, std::string& seqName);
	std::string  getCurDir();
	void  deleteFilesInFolder(const std::string& folderPath);

	void  createDir(const std::string& p);

	//fPath="c:/temp/f1.txt" ---> head = "c:/temp/f1", ext="txt" 
	void  splitExt(const std::string& fPath, std::string& head, std::string& ext);
	//fPath="c:/temp/f1.txt" ---> fDir = "c:/temp", fName="f1.txt" 
	void  splitFolder(const std::string& fPath, std::string& fDir, std::string& fName);

	bool  isVideoFile(const std::string& filePath);
	bool  isImgeFile(const std::string& filePath);
	uint64_t  getAvailableDiskSpaceInByte(const std::string& folderPath);

}

#endif
