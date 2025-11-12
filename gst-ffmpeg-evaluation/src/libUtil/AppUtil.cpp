/*
 *------------------------------------------------------------------------------
 * Copyright � 2012 Shunguang Wu (SWU)
 *
 * This program is developed by SWU in his personal interesting in evenings and holidays. 
 * SWU IS NOT supported by any angency during this work. Use, redistribute, or modify
 * is possible based on request ( shunguang@yahoo.com, 732-434-5523 (cell) )
 *------------------------------------------------------------------------------
 */
#include "AppUtil.h"
using namespace std;

void ngv::getExtName( const std::string &fileName, std::string &ext )
{
	size_t n1 = fileName.find_last_of('.');
	size_t n2 = fileName.length();

	ext = fileName.substr(n1+1, n2-n1+1);
	for(size_t i=0; i<ext.length(); i++){
		ext[i] = toupper( ext[i]);
	}
}

uint32_t ngv::getFileNameList( const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames )
{
	string fName, extName;
	string ext0 = ext;
	int L = ext0.length();

	vFileNames.clear();
	int L0 = dirName.length();
	boost::filesystem::path targetDir(dirName);
	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;
	BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod)) {
		if (is_regular_file(i)) {
			fName = i.string().substr(L0+1);

			extName = fName.substr(fName.length() - L);
			if (0 == ext0.compare(extName) || 0 == ext.compare("*")) {
				vFileNames.push_back(fName);
			}
		}
	}
	return vFileNames.size();
}


bool ngv :: folderExists( const string &strPath )
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

bool ngv ::  fileExists(const std::string& name) 
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool ngv::findSeqName( const std::string &inSeqFileName, std::string &seqName )
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

bool ngv::mkDumpDirs( const string &inSeqFileName, const string &dumpPathRoot, string &dumpPath4Debug, string &dumpPath4Customer )
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


std::string ngv::getCurDir()
{
	char cCurrentPath[FILENAME_MAX];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		return std::string("");
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	printf("The current working directory is %s", cCurrentPath);
	
	return std::string(cCurrentPath);

}

void  ngv::deleteFilesInFolder(const std::string &folderPath)
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

void ngv::createDir(const std::string &p)
{
	boost::filesystem::path p0(p);
	if (!boost::filesystem::exists(p0)) {
		if (!boost::filesystem::create_directories(p0)) {
			printf("ngv::myCreateDir(): cannot create root folder:%s", p0.string().c_str());
		}
	}
}

void ngv::splitExt(const std::string &fPath, std::string &head, std::string &ext)
{
	int id = fPath.find_last_of('.');
	if (id == std::string::npos) return;

	head = fPath.substr(0, id);
	ext = fPath.substr(id + 1);
}

void ngv::splitFolder(const std::string& fPath, std::string& fDir, std::string& fName)
{
	boost::filesystem::path p(fPath);
	boost::filesystem::path dir = p.parent_path();
	boost::filesystem::path name = p.filename();

	fDir = dir.generic_string();
	fName = name.generic_string();
}



bool ngv::isVideoFile(const std::string& filePath)
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

bool ngv::isImgeFile(const std::string& filePath)
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

uint64_t ngv::getAvailableDiskSpaceInByte(const std::string& folderPath)
{
	boost::filesystem::space_info si = boost::filesystem::space(folderPath);
	return si.available;
}

uint32_t  ngv::checkTimeOut( const boost::posix_time::ptime &start, const uint32_t thdInMillisec )
{
        boost::posix_time::ptime now = POSIX_LOCAL_TIME;
        boost::posix_time::time_duration dt = now - start;

        int64_t dt_milliseconds = dt.total_milliseconds();

        if( dt_milliseconds >= thdInMillisec ){
                //already timeout
                return 0;
        }
        else{
                return thdInMillisec - dt_milliseconds;
        }
}

uint32_t  ngv::timeIntervalMillisec( const boost::posix_time::ptime &start )
{
        boost::posix_time::ptime end = POSIX_LOCAL_TIME;
        boost::posix_time::time_duration dt = end - start;

        return (uint32_t)(dt.total_milliseconds());
}

uint32_t ngv::timeIntervalMillisec( const boost::posix_time::ptime &start, const boost::posix_time::ptime &end )
{
        boost::posix_time::time_duration dt = end - start;
        return (uint32_t)(dt.total_milliseconds());
}


void ngv::timeIntervalMillisec( const rusage &ru1, const rusage &ru2, int64_t &dt_usr_cpu_us, int64_t &dt_sys_cpu_us )
{
    dt_usr_cpu_us = (ru2.ru_utime.tv_sec - ru1.ru_utime.tv_sec)*1000000 +
                 (ru2.ru_utime.tv_usec - ru1.ru_utime.tv_usec);
    dt_sys_cpu_us = (ru2.ru_stime.tv_sec - ru1.ru_stime.tv_sec)*1000000 +
                 (ru2.ru_stime.tv_usec - ru1.ru_stime.tv_usec);
}
