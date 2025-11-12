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

#include "AppBagRead.h"
namespace app {
	/*
	*****************************************
	*  class AppBagRead
	*****************************************
	*/
AppBagRead::AppBagRead(const std::string& bagFilePath, const bool isBinary)
		: _isBinary(isBinary)
		, _bagFilePath(bagFilePath)
	{
		_isOpened=open();
	}

	AppBagRead::~AppBagRead() {
		close();
	}

	bool AppBagRead::open()
	{
		if (!fileExists(_bagFilePath)) {
			appExit("AppBagRead::openBag2Read(): file (%s) does not exist!", _bagFilePath.c_str());
		}

		if (_isBinary) {
			_ifs.open(_bagFilePath.c_str(), std::ofstream::in | std::ofstream::binary);
		}
		else {
			_ifs.open(_bagFilePath.c_str(), std::ofstream::in);
		}

		if (_ifs.is_open()) {
			// get length of file:
			_ifs.seekg(0, _ifs.end);
			_nTotBytes = _ifs.tellg();
			_ifs.seekg(0, _ifs.beg);
			
			dumpLog("AppBagRead::openBag2Read(): successfully opened file:%s, _nTotBytes=%lu", _bagFilePath.c_str(), _nTotBytes);
			return true;
		}
		else {
			dumpLog("AppBagRead::openBag2Read(): cannot opened file:%s", _bagFilePath.c_str());
			return false;
		}
	}

	void AppBagRead::close()
	{
		if (_ifs.is_open()) {
			_ifs.close();
		}
	}

	/*
	*****************************************
	*  class AppBagReadTxt
	*****************************************
	*/

	AppBagReadTxt::AppBagReadTxt(const std::string& bagFilePath)
		: AppBagRead(bagFilePath, false)
	{
	}

	AppBagReadTxt::~AppBagReadTxt() {
		dumpLog("AppBagReadTxt::~AppBagReadTxt() called");
	}

	uint64_t AppBagReadTxt::readRawAll(std::vector<uint8_t>& msg) {
		return 0;
	}

	uint64_t AppBagReadTxt::readRawSome(std::vector<uint8_t>& msg, const uint64_t  nBytes) {
		return 0;
	}

	//return true -- if there has a line in file, 
	//       false --  otherwise or eof
	bool AppBagReadTxt::readLine(std::string& line) 
	{
			if (_ifs.eof()) {
				return false;
			}

			getline(_ifs, line, '\n');
			return true;
	}

	/*
	*****************************************
	*  class AppBagReadBin
	*****************************************
	*/
	AppBagReadBin::AppBagReadBin(const std::string& bagFilePath)
		: AppBagRead(bagFilePath, true)
	{
	}

	AppBagReadBin::~AppBagReadBin() {
	}

	//for binary bag, we read the raw message, the caller will parse it latter 
	uint64_t AppBagReadBin::readRawAll(std::vector<uint8_t>& v) 
	{
		uint64_t length = 0;
		if (!_ifs) {
			dumpLog("AppBagReadBin::readRawAll(): cannot read any data!");
			return length;
		}

		length = _nTotBytes;
		dumpLog("AppBagReadBin::readRawAll(): reading %lu bytes", length);

		v.resize(length);
		_ifs.read((char*)v.data(), length);

		if (_ifs) {
			dumpLog("AppBagReadBin::readRawAll(): all characters read successfully.");
		}
		else {
			length = _ifs.gcount();
			dumpLog("AppBagReadBin::readRawAll(): error: only %lu could be read", length);
		}
		return length;
	}

	//the caller allocate the szie of <v>: assert( v.size() == nBytes)
	uint64_t AppBagReadBin::readRawSome(std::vector<uint8_t>& v, const uint64_t  nBytes) 
	{
		//since _ifs.readsome() is not reliable, we use read(). the caller takes the responsobility to control <nBytes>
		_ifs.read((char*)v.data(), nBytes);

		//todo:
		return nBytes;
	}

	bool AppBagReadBin::readLine(std::string& line) {
		return false;
	}
}

