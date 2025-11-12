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

#include "AppBagWrite.h"
using namespace std;
using namespace app;
/*
*****************************************
*  class AppBagWrite
*****************************************
*/
AppBagWrite::AppBagWrite(const std::string& dir, const std::string& tag, const int maxBagFileSize_GB, const int minFreeSpace_GB,  const bool isBinary)
	: _bagDir(dir)
	, _bagFilepathBaseName(dir + "/" + tag)
	, _maxBagFileSize_B( (uint64_t)maxBagFileSize_GB * APP_DISK_GB )
	, _minFreeSpace_B  ( (uint64_t)minFreeSpace_GB  * APP_DISK_GB )
	, _isBinary(isBinary)
	, _checkFileSize(true)
{
	_isSuccefullyOpen = this->open();
	dumpLog("AppBagWrite::AppBagWrite(): _bagDir=%s, _bagFilepathBaseName=%s, _isSuccefullyOpen=%d", _bagDir.c_str(), _bagFilepathBaseName.c_str(), _isSuccefullyOpen);
}

AppBagWrite::AppBagWrite(const std::string& filepath, const bool isBinary)
: _bagFilePath( filepath )
, _isBinary(isBinary)
, _checkFileSize(false)
{
	std::string fName;
	splitFolder(filepath, _bagDir, fName);

	_isSuccefullyOpen = this->open();
	dumpLog("AppBagWrite::AppBagWrite(): _bagDir=%s, _bagFilePath=%s, _isSuccefullyOpen=%d", _bagDir.c_str(), _bagFilePath.c_str(), _isSuccefullyOpen);
}

AppBagWrite::~AppBagWrite()
{
	this->close();
}

bool AppBagWrite::open()
{
	bool dirGood = true;
	if (!folderExists(_bagDir)) {
		dirGood = createDir(_bagDir);
	}
	if (!dirGood) {
		dumpLog("AppBagWrite::open(): error -- cannot create dir=%s", _bagDir.c_str() );
		return false;
	}

	uint64_t available_space_B = 0;
	if (_checkFileSize) {
		available_space_B = getAvailableDiskSpaceInByte(_bagDir);
		if (available_space_B > (_maxBagFileSize_B + _minFreeSpace_B)) {
			if (_isBinary) {
				_bagFilePath = _bagFilepathBaseName + std::to_string(_bagCnt) + ".bag";
				_ofs.open(_bagFilePath.c_str(), std::ofstream::out | std::ofstream::binary);
			}
			else {
				_bagFilePath = _bagFilepathBaseName + std::to_string(_bagCnt) + ".txt";
				_ofs.open(_bagFilePath.c_str(), std::ofstream::out);
			}
		}
	}
	else {
		if (_isBinary) {
			_ofs.open(_bagFilePath.c_str(), std::ofstream::out | std::ofstream::binary);
		}
		else {
			_ofs.open(_bagFilePath.c_str(), std::ofstream::out);
		}
	}

	if (!_ofs.is_open()) {
		dumpLog("AppBagWrite::open(): cannot opened file:%s", _bagFilePath.c_str());
		return false;
	}

	//-- summarizing ----------
	dumpLog("AppBagWrite::open(): successfully opened file:%s", _bagFilePath.c_str());
	if (_checkFileSize) {
		_bagCnt++;
		_currFileSize = 0;
		dumpLog("Start a new bag: bagCnt=%u, bagFile=%s, availableSpace = % llu(B), neededSpace = % lu(B)", _bagCnt, _bagFilePath.c_str(), available_space_B, _maxBagFileSize_B);
	}
	else {
		dumpLog("Start a new bag: bagFile=%s", _bagFilePath.c_str());
	}
	return true;
}


bool AppBagWrite::close()
{
	if (_ofs.is_open()) {
		_ofs.close();
	}
	return true;
}


void AppBagWrite::wrtMsgWrapUp(const APP_TIME_MS tBeg)
{
	_ofs.flush();
	++_msgCnt;
	
	if (_checkFileSize) {
		if (isBagBigEnough()) {
			this->close();
			this->open();
		}
	}

	float dt = (float)(APP_TIME_CURRENT_MS - tBeg);
	_hmWrtBagCupTime_ms.update(dt);
	if (0 == _msgCnt % 5000) {
		dumpLog("AppBagWrite Healthy: msgCnt=%lu,%s, %s, sz=%lu(B)", _msgCnt, _hmWrtBagCupTime_ms.to_string("cpuTime(ms)/msg:", "dump").c_str(), _bagFilePath.c_str(), _currFileSize);
		_hmWrtBagCupTime_ms.reset();
	}
}

/*
*****************************************
*  class AppBagWriteTxt
*****************************************
*/
AppBagWriteTxt::AppBagWriteTxt(const std::string& dir, const std::string& tag, const int maxBagFileSize_GB, const int minFreeSpace_GB)
	: AppBagWrite(dir, tag, maxBagFileSize_GB, minFreeSpace_GB, false) 
{
}

AppBagWriteTxt::AppBagWriteTxt(const std::string& filepath)
	: AppBagWrite(filepath, false)
{
}

AppBagWriteTxt::~AppBagWriteTxt()
{
}

void AppBagWriteTxt::write(const char* fmt, ...)
{
	//todo: remove this LOG_MAX_MSG_LEN, using dynamic allocation idea
	char buf[LOG_MAX_MSG_LEN];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, LOG_MAX_MSG_LEN, fmt, args);
	va_end(args);
	write(std::string(buf));
}

void AppBagWriteTxt::write(const	std::string& s, const bool isAddNewLine )
{
	if ( !_isSuccefullyOpen ) {
		return;
	}

	APP_TIME_MS tBeg = APP_TIME_CURRENT_MS;
	if (isAddNewLine) {
		_ofs << s << endl;
	}
	else{
		_ofs << s ;
	}
	_currFileSize += s.length();
	wrtMsgWrapUp(tBeg);
}


/*
*****************************************
*  class AppBagWriteBin
*****************************************
*/
AppBagWriteBin::AppBagWriteBin(const std::string& dir, const std::string& tag, const int maxBagFileSize_GB, const int minFreeSpace_GB)
	: AppBagWrite(dir, tag, maxBagFileSize_GB, minFreeSpace_GB, true) 
{
}

AppBagWriteBin::AppBagWriteBin(const std::string& filepath)
	: AppBagWrite(filepath, true)
{
}

AppBagWriteBin::~AppBagWriteBin()
{
}


