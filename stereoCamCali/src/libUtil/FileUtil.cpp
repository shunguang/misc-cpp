/*
 *------------------------------------------------------------------------------
 * Copyright � 2012 Shunguang Wu (SWU)
 *
 * This program is developed by SWU in his personal interesting in evenings and holidays. 
 * SWU IS NOT supported by any angency during this work. Use, redistribute, or modify
 * is possible based on request ( shunguang@yahoo.com, 732-434-5523 (cell) )
 *------------------------------------------------------------------------------
 */
#include "FileUtil.h"
using namespace std;

void cali::getExtName( const std::string &fileName, std::string &ext )
{
	size_t n1 = fileName.find_last_of('.');
	size_t n2 = fileName.length();

	ext = fileName.substr(n1+1, n2-n1+1);
	for(size_t i=0; i<ext.length(); i++){
		ext[i] = toupper( ext[i]);
	}
}

uint32_t cali::getFileNameList( const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames )
{
	string fDir;
	string fName, extName;
	string ext0 = ext;
	int L = ext0.length();

	vFileNames.clear();
	int L0 = dirName.length();
	boost::filesystem::path targetDir(dirName);
	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;
	BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod)) {
		if (is_regular_file(i)) {
			splitFolder( i.string(), fDir, fName);
			//fName = i.string().substr(L0+1);
			extName = fName.substr(fName.length() - L);
			if (0 == ext0.compare(extName) || 0 == ext.compare("*")) {
				vFileNames.push_back(fName);
			}
		}
	}
	return vFileNames.size();
}


bool cali :: folderExists( const string &strPath )
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

bool cali ::  fileExists(const std::string& name) 
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool cali::findSeqName( const std::string &inSeqFileName, std::string &seqName )
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

bool cali::mkDumpDirs( const string &inSeqFileName, const string &dumpPathRoot, string &dumpPath4Debug, string &dumpPath4Customer )
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


std::string cali::getCurDir()
{
	char cCurrentPath[FILENAME_MAX];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		return std::string("");
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	printf("The current working directory is %s", cCurrentPath);
	
	return std::string(cCurrentPath);

}

void  cali::deleteFilesInFolder(const std::string &folderPath)
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

void cali::createDir(const std::string &p)
{
	boost::filesystem::path p0(p);
	if (!boost::filesystem::exists(p0)) {
		if (!boost::filesystem::create_directories(p0)) {
			printf("cali::myCreateDir(): cannot create root folder:%s", p0.string().c_str());
		}
	}
}

void cali::splitExt(const std::string &fPath, std::string &head, std::string &ext)
{
	int id = fPath.find_last_of('.');
	if (id == std::string::npos) return;

	head = fPath.substr(0, id);
	ext = fPath.substr(id + 1);
}

void cali::splitFolder(const std::string& fPath, std::string& fDir, std::string& fName)
{
	boost::filesystem::path p(fPath);
	boost::filesystem::path dir = p.parent_path();
	boost::filesystem::path name = p.filename();

	fDir = dir.generic_string();
	fName = name.generic_string();
}



bool cali::isVideoFile(const std::string& filePath)
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

bool cali::isImgeFile(const std::string& filePath)
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

uint64_t cali::getAvailableDiskSpaceInByte(const std::string& folderPath)
{
	boost::filesystem::space_info si = boost::filesystem::space(folderPath);
	return si.available;
}