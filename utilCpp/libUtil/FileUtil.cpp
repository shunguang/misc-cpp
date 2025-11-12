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
#include "FileUtil.h"
using namespace std;

void app::getExtName( const std::string &fileName, std::string &ext )
{
	size_t n1 = fileName.find_last_of('.');
	size_t n2 = fileName.length();

	ext = fileName.substr(n1+1, n2-n1+1);
	for(size_t i=0; i<ext.length(); i++){
		ext[i] = toupper( ext[i]);
	}
}

uint32_t app::getFileNameList( const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames )
{
	string fName, extName;
	string ext0 = ext;
	int L = ext0.length();

	vFileNames.clear();
#if 0
	int L0 = dirName.length();
	filesystem::path targetDir=dirName;
	filesystem::recursive_directory_iterator iter(targetDir), eod;
	for( const std::filesystem::path & i : make_pair(iter, eod)) {
		if (is_regular_file(i)) {
			fName = i.string().substr(L0+1);

			extName = fName.substr(fName.length() - L);
			if (0 == ext0.compare(extName) || 0 == ext.compare("*")) {
				vFileNames.push_back(fName);
			}
		}
	}
	std::sort(vFileNames.begin(), vFileNames.end());
#endif

	return vFileNames.size();
}


bool app :: folderExists( const string &strPath )
{  
	if (appAccess( strPath.c_str(), 0 ) == 0 ){
		struct stat status;
		stat( strPath.c_str(), &status );
		if ( status.st_mode & S_IFDIR ){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

#if 0
#include <sys/stat.h>
#endif
bool app ::  fileExists(const std::string& name) 
{
#if 0 
	//this does not work for ~4GB file
	struct stat buffer;
	int ret = stat(name.c_str(), &buffer);
	return (ret == 0);
#endif

	int ret = _access(name.c_str(), 0);
	return ( ret != -1 );
}

bool app::findSeqName( const std::string &inSeqFileName, std::string &seqName )
{
	string t = inSeqFileName;
	size_t p1 = t.find_last_of('/\\' );
	if( p1 < 0 )
		return false;

	t.replace(p1,1,"a");
	size_t p2 = t.find_last_of('/\\' );
	if( p2 < 0 )
		return false;
	
	seqName = inSeqFileName.substr(p2+1, p1-p2-1 );
	return true;	
}

bool app::mkDumpDirs( const string &inSeqFileName, const string &dumpPathRoot, string &dumpPath4Debug, string &dumpPath4Customer )
{

	string seqName;
	if( !findSeqName( inSeqFileName, seqName ) ){
		return false;
	}
	printf ("seqName=%s\n", seqName.c_str() );
	printf ("dumpPathRoot=%s\n", dumpPathRoot.c_str() );
#if 0
	dumpPath4Debug = dumpPathRoot + string("\\Results-") + seqName + string("-debug");
	dumpPath4Customer = dumpPathRoot+"\\Results-" + seqName + "-out";
	printf ("dumpPath4Debug=%s\n", dumpPath4Debug.c_str() );
	printf ("dumpPath4Customer=%s\n", dumpPath4Customer.c_str() );

	if( !folderExists( dumpPath4Debug ) ){
		mkdir( dumpPath4Debug.c_str() );
	}

	if( !folderExists( dumpPath4Customer ) ){
		mkdir( dumpPath4Customer.c_str() );
	}
#endif

	return true;
}


std::string app::getCurDir()
{
	char cCurrentPath[FILENAME_MAX];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		return std::string("");
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	printf("The current working directory is %s", cCurrentPath);
	
	return std::string(cCurrentPath);

}

void  app::deleteFilesInFolder(const std::string &folderPath)
{
	std::vector<std::string> vFileNames;
	uint32_t n = getFileNameList(folderPath, "*", vFileNames);
	string filepath;
	for (uint32_t i=0; i<n; ++i){
		filepath = folderPath + "/" + vFileNames[i];
		remove( filepath.c_str() );
	}
	printf("totally %d files are removed from %s\n", n, folderPath.c_str());
}

bool app::createDir(const std::string &p)
{
#if __cplusplus > 201703L
	std::filesystem::path p0(p);
	if (!std::filesystem::exists(p0)) {
		if (!std::filesystem::create_directories(p0)) {
			printf("app::myCreateDir(): cannot create root folder:%s", p0.string().c_str());
			return false;
		}
	}
	return true;
#else
	return false;
#endif
}

void app::splitExt(const std::string &fPath, std::string &head, std::string &ext)
{
	int id = fPath.find_last_of('.');
	if (id == std::string::npos) return;

	head = fPath.substr(0, id);
	ext = fPath.substr(id + 1);
}

//fPath="c:/temp/f1.txt" ---> fDir = "c:/temp", fName="f1.txt"
void app::splitFolder(const std::string& fPath, std::string& fDir, std::string& fName)
{
	std::size_t found = fPath.find_last_of("/\\");
	fDir = fPath.substr(0, found);
	fName = fPath.substr(found + 1);
}



bool app::isVideoFile(const std::string& filePath)
{
	std::string  head, ext;

	splitExt(filePath, head, ext);

	if (0 == ext.compare("AVI")) {
		return true;
	}
	if (0 == ext.compare("MP4")) {
		return true;
	}

	return false;
}

bool app::isImgeFile(const std::string& filePath)
{
	std::string  head, ext;

	splitExt(filePath, head, ext);

	if (0 == ext.compare("BMP")) {
		return true;
	}
	if (0 == ext.compare("JPG")) {
		return true;
	}
	if (0 == ext.compare("PNG")) {
		return true;
	}
	if (0 == ext.compare("GIF")) {
		return true;
	}
	if (0 == ext.compare("JPEG")) {
		return true;
	}
	if (0 == ext.compare("PBM")) {
		return true;
	}
	if (0 == ext.compare("PGM")) {
		return true;
	}
	if (0 == ext.compare("PPM")) {
		return true;
	}
	if (0 == ext.compare("XBM")) {
		return true;
	}
	if (0 == ext.compare("XPM")) {
		return true;
	}

	return false;
}

uint64_t app::getAvailableDiskSpaceInByte(const std::string& folderPath)
{
#if __cplusplus > 201703L
	std::filesystem::space_info si = std::filesystem::space(folderPath);
	return si.available;
#else
	return 0;
#endif
}